import logging
from threading import Lock

import PyQt6.QtCore
import numpy as np
import pyqtgraph as pg
from PyQt6.QtCore import QThread
from PyQt6.QtWidgets import QWidget

import util.grapher_logging as gl
from sinks.DataProvider import DataPacket
from sinks.mqtt import MqttSink
from sinks.tcp import TCPSink

logger = gl.get_logger(__name__, logging.DEBUG)

CHUNK_SIZE = 300
BUFFER_SIZE = 100


class Channel:
    def __init__(self, color, enabled: bool):
        self.curves = list()
        self.data = np.zeros((CHUNK_SIZE + 1, 2))
        self.buffer = np.zeros((BUFFER_SIZE + 1, 2))
        self.ptr = 0
        self.chunk_idx = 0
        self.buffer_idx = 0
        self.rgb = color
        self.init = False
        self.enabled = enabled


def add_new_data(curve, channel: Channel):
    # set data with accumulated new data
    start = channel.chunk_idx + 1
    end = start + channel.buffer_idx

    channel.data[start:end] = channel.buffer[: channel.buffer_idx]

    curve.setData(x=channel.data[:end, 0], y=channel.data[:end, 1])

    # reset buffer and counter
    channel.ptr += channel.buffer_idx
    channel.buffer = np.zeros((BUFFER_SIZE + 1, 2))
    channel.buffer_idx = 0


class Plotter(QWidget):
    def __init__(self, plot_cfg, channels_cfg):
        super().__init__()
        self.plot = pg.PlotWidget(title="Plot")
        self.win = None

        self.data_mtx = Lock()
        self.curve_mtx = Lock()

        self.max_chunks = 10
        self.start_time = 0

        self.tcp_thread = None
        self.tcp_sink = None
        self.mqtt_sink = None
        self.mqtt_enabled = False
        self.tcp_enabled = False

        self.running = True

        self.channels = dict()
        self.populate_channels(channels_cfg)

        self.timer = pg.QtCore.QTimer(self)

        self.plot_cfg = plot_cfg
        self.plot_cfg.param("Plot configuration").sigTreeStateChanged.connect(
            self.rescale
        )

    def rescale(self, param, changes):

        for p, c, d in changes:
            dimension = self.plot_cfg.childPath(p)[-1]

            if dimension == "xmin":
                self.plot.setXRange(
                    d, self.plot_cfg.param("Plot configuration", "xmax").value()
                )
            elif dimension == "xmax":
                self.plot.setXRange(
                    self.plot_cfg.param("Plot configuration", "xmin").value(), d
                )
            elif dimension == "ymin":
                self.plot.setYRange(
                    d, self.plot_cfg.param("Plot configuration", "ymax").value()
                )
            elif dimension == "ymax":
                self.plot.setYRange(
                    self.plot_cfg.param("Plot configuration", "xmin").value(), d
                )

    def toggle_pause(self):
        logger.debug("Toggling pause")
        self.running = not self.running

    def populate_channels(self, channels_cfg: dict):
        if "MQTT" in channels_cfg:
            if channels_cfg["MQTT"]["enabled"]:
                self.add_mqtt_source(channels_cfg["MQTT"])

        if "TCP" in channels_cfg:
            if channels_cfg["TCP"]["enabled"]:
                self.add_tcp_source(channels_cfg["TCP"])

    def add_mqtt_source(self, s: dict):
        logger.debug("Adding MQTT source")
        # support only one MQTT source right now
        if not self.mqtt_enabled:
            self.mqtt_enabled = True
            self.mqtt_sink = MqttSink()

            self.update_mqtt_topics(s)
            self.update_mqtt_host(s)

    def update_mqtt_host(self, s: dict):

        hostname = s["host"]
        port = s["port"]

        if hostname != self.mqtt_sink.host or port != self.mqtt_sink.port:
            self.mqtt_sink.update_host(hostname, port)

    def update_mqtt_topics(self, s: dict):
        topics = self.update_channels_for_source(s["channels"])
        self.mqtt_sink.update_topics(topics)
        logger.info("Updated MQTT topics")

    def update_channels_for_source(self, channels: list):
        topics = []

        for c in channels:
            logger.debug("Adding channel %d", c["id"])
            self.channels[c["id"]] = Channel(c["color"], c["enabled"])
            self.channels[c["id"]].init = True
            self.create_new_curve(self.channels[c["id"]])

            if "topic" in c:
                topics.append(c["topic"])

        return topics

    def add_tcp_source(self, s: dict):
        logger.debug("Adding TCP source")

        # support only one TCP source right now
        if not self.tcp_enabled:
            self.tcp_enabled = True

            self.update_channels_for_source(s["channels"])
            self.tcp_sink = TCPSink(s["host"], s["port"], self.post_data)

    def init_io(self):
        if self.tcp_enabled:
            self.tcp_sink.start()
            self.tcp_thread = QThread()
            self.tcp_sink.moveToThread(self.tcp_thread)
            self.tcp_thread.started.connect(self.tcp_sink.run)
            logger.info("TCP connected and ready to run")

        if self.mqtt_enabled:
            self.mqtt_sink.connect_client()
            logger.info("MQTT connected")

    def start(self):
        logger.info("Starting logger")

        self.plot.setLabel("bottom", "Time", "s")
        self.plot.setRange(xRange=[-10, 0], yRange=[-2, 40])

        self.timer.timeout.connect(self.update)
        self.timer.start(50)

        if self.tcp_enabled:
            self.tcp_sink.post_signal.connect(self.post_data)
            self.tcp_thread.start()
            logger.info("TCP started")

        if self.mqtt_enabled:
            self.mqtt_sink.post_signal.connect(self.post_data)
            self.mqtt_sink.start()
            logger.info("MQTT started")

        self.start_time = pg.ptime.time()
        logger.debug("Start time: %s", self.start_time)

    def close(self):
        if self.tcp_enabled:
            self.tcp_sink.close()
        if self.mqtt_enabled:
            self.mqtt_sink.close()

    @PyQt6.QtCore.pyqtSlot(DataPacket)
    def post_data(self, msg: DataPacket):

        if not self.running:
            return

        s = self.channels[msg.source_id]

        with self.data_mtx:
            s.buffer[s.buffer_idx, 0] = msg.timestamp - self.start_time
            s.buffer[s.buffer_idx, 1] = msg.data
            s.buffer_idx += 1

    def create_new_curve(self, channel: Channel):
        with self.curve_mtx:
            curve = self.plot.plot(pen=channel.rgb)
            channel.curves.append(curve)
            last = channel.data[channel.chunk_idx - 1]

            channel.chunk_idx = 0
            channel.ptr = 0

            channel.data = np.zeros((CHUNK_SIZE + 1, 2))
            channel.data[0] = last

            while len(channel.curves) > self.max_chunks:
                c = channel.curves.pop(0)
                self.plot.removeItem(c)

        return curve

    def add_constant_data(self, now, curve: pg.plot, channel: Channel):
        channel.data[channel.chunk_idx + 1, 0] = now - self.start_time
        channel.data[channel.chunk_idx + 1, 1] = channel.data[channel.chunk_idx, 1]

        curve.setData(
            x=channel.data[: channel.chunk_idx + 1, 0],
            y=channel.data[: channel.chunk_idx + 1, 1],
        )

        channel.ptr += 1

    def update(self):
        if not self.running:
            return

        now = pg.ptime.time()

        for _, channel in self.channels.items():
            for curve in channel.curves:
                curve.setPos(-(now - self.start_time), 0)

        with self.data_mtx:

            for _, channel in self.channels.items():
                channel.chunk_idx = channel.ptr % CHUNK_SIZE

                if channel.chunk_idx == 0 or (channel.chunk_idx * 1.25) > CHUNK_SIZE:
                    curve = self.create_new_curve(channel)
                else:
                    curve = channel.curves[-1]

                if not channel.enabled:
                    for c in channel.curves:
                        c.hide()

                if channel.buffer_idx == 0:
                    self.add_constant_data(now, curve, channel)
                else:
                    add_new_data(curve, channel)


def get_source_type(s):
    return s.opts["name"].split()[0]
