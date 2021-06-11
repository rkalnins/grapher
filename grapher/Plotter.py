import logging
from threading import Lock

import PyQt6.QtCore
import numpy as np
import pyqtgraph as pg
from PyQt6.QtCore import QThread
from PyQt6.QtGui import QColor
from pyqtgraph.parametertree import Parameter

import grapher.util.grapher_logging as gl
from grapher.sinks.DataProvider import DataPacket
from sinks.mqtt import MqttSink
from sinks.tcp import TCPSink

logger = gl.get_logger(__name__, logging.DEBUG)

CHUNK_SIZE = 300
BUFFER_SIZE = 100


class Channel:
    def __init__(self, color: QColor):
        self.curves = list()
        self.data = np.zeros((CHUNK_SIZE + 1, 2))
        self.buffer = np.zeros((BUFFER_SIZE + 1, 2))
        self.ptr = 0
        self.chunk_idx = 0
        self.buffer_idx = 0
        self.rgb = color.getRgb()
        self.init = False
        self.enabled = True

    def update_enabled(self, val):
        self.enabled = val

    def update_color(self, color):
        self.enabled = color.getRgb()

        for c in self.curves:
            c.setPen(QColor(self.rgb))


class Plotter(PyQt6.QtCore.QObject):
    def __init__(self, ptree):
        super().__init__()
        self.plot = pg.PlotWidget(title="Plot")
        self.win = None

        self.data_mtx = Lock()
        self.curve_mtx = Lock()

        self.chunk_size = 300
        self.buffer_size = 100
        self.max_chunks = 10
        self.start_time = 0

        self.tcp_thread = None
        self.tcp_sink = None
        self.mqtt_sink = None
        self.mqtt_enabled = False
        self.tcp_enabled = False

        self.channels = dict()
        self.populate_channels(ptree)

        self.timer = pg.QtCore.QTimer()

        logger.debug('done init')

    def reset(self, ptree):
        self.data_mtx = Lock()
        self.curve_mtx = Lock()

        self.chunk_size = 300
        self.buffer_size = 100
        self.max_chunks = 10
        self.start_time = 0

        self.tcp_thread = None
        self.tcp_sink = None
        self.mqtt_sink = None
        self.mqtt_enabled = False
        self.tcp_enabled = False

        self.channels = dict()
        self.populate_channels(ptree)

        self.timer = pg.QtCore.QTimer()

    def populate_channels(self, ptree: Parameter):
        sources = ptree.child('Sources')
        for s in sources:
            src_type = get_source_type(s)
            if src_type == 'MQTT':
                self.add_mqtt_source(s)
            elif src_type == 'TCP':
                self.add_tcp_source(s)

    def add_mqtt_source(self, s):
        logger.debug('Adding MQTT source')
        # support only one MQTT source right now
        if not self.mqtt_enabled:
            self.mqtt_enabled = True
            self.mqtt_sink = MqttSink()

            self.update_mqtt_topics(s)
            self.update_mqtt_host(s)

    def update_mqtt_host(self, s):
        addr: str = s['host']
        parts = addr.split(':')

        if parts[0] != self.mqtt_sink.host or int(parts[1]) != self.mqtt_sink.port:
            self.mqtt_sink.update_host(parts[0], int(parts[1]))

    def update_mqtt_topics(self, s):
        topics = self.update_channels_for_source(s)
        self.mqtt_sink.update_topics(topics)

    def update_channels_for_source(self, s):
        topics = []

        for topic in s.child('channels'):
            if len(topic['topic']) > 0 and topic['ID'] != -1:
                self.channels[topic['ID']] = Channel(topic['color'])
                self.channels[topic['ID']] = topic['ID']

                topic['color'].sigValueChanged.connect(self.channels[topic['ID']].update_color)
                topic['enabled'].sigValueChanged.connect(self.channels[topic['ID']].update_enabled)

                topics.append(topic['topic'])

        return topics

    def add_tcp_source(self, s):
        logger.debug('Adding TCP source')

        # support only one TCP source right now
        if not self.tcp_enabled:
            self.tcp_enabled = True

            for topic in s.child['topics']:
                self.channels[topic['ID']] = Channel(topic['color'])

            addr: str = s['host']
            parts = addr.split(':')
            self.tcp_sink = TCPSink(parts[0], int(parts[1]), self.post_data)

    def init_io(self):
        logger.debug('Getting data')
        if self.tcp_enabled:
            self.tcp_sink.start()
            self.tcp_thread = QThread()
            self.tcp_sink.moveToThread(self.tcp_thread)
            self.tcp_thread.started.connect(self.tcp_sink.run)

        if self.mqtt_enabled:
            self.mqtt_sink.connect_client()

    def start(self):
        logger.debug('starting')

        self.plot.setLabel('bottom', 'Time', 's')
        self.plot.setRange(xRange=[-10, 0], yRange=[-2, 40])

        self.timer.timeout.connect(self.update)
        self.timer.start(50)

        if self.tcp_enabled:
            self.tcp_sink.post_signal.connect(self.post_data)
            self.tcp_thread.start()

        if self.mqtt_enabled:
            self.mqtt_sink.post_signal.connect(self.post_data)
            self.mqtt_sink.start()

        logger.debug('done setup')

        self.start_time = pg.ptime.time()

    def close(self):
        if self.tcp_enabled:
            self.tcp_sink.close()
        if self.mqtt_enabled:
            self.mqtt_sink.close()

    @PyQt6.QtCore.pyqtSlot(DataPacket)
    def post_data(self, msg: DataPacket):

        if not self.channels[msg.source_id].init:
            self.channels[msg.source_id].init = True
            self.create_new_curve(self.channels[msg.source_id])

        s = self.channels[msg.source_id]
        logger.debug('%s %s %s %s', s.buffer_idx, msg.data, msg.timestamp, msg.timestamp - self.start_time)

        with self.data_mtx:
            s.buffer[s.buffer_idx, 0] = msg.timestamp - self.start_time
            s.buffer[s.buffer_idx, 1] = msg.data
            s.buffer_idx += 1

    def create_new_curve(self, channel: Channel):
        logger.debug('New curve %s', channel.chunk_idx)

        with self.curve_mtx:
            curve = self.plot.plot(pen=channel.rgb)
            channel.curves.append(curve)
            last = channel.data[channel.chunk_idx - 1]

            channel.chunk_idx = 0
            channel.ptr = 0

            channel.data = np.zeros((self.chunk_size + 1, 2))
            channel.data[0] = last

            while len(channel.curves) > self.max_chunks:
                c = channel.curves.pop(0)
                self.plot.removeItem(c)

        return curve

    def add_new_data(self, curve: pg.plot, channel: Channel):
        logger.debug('new data %s', channel.buffer_idx)
        # set data with accumulated new data
        start = channel.chunk_idx + 1
        end = start + channel.buffer_idx

        print(start, end, channel.chunk_idx, channel.buffer_idx)

        channel.data[start:end] = channel.buffer[:channel.buffer_idx]

        curve.setData(x=channel.data[:end, 0],
                      y=channel.data[:end, 1])

        # reset buffer and counter
        channel.ptr += channel.buffer_idx
        channel.buffer = np.zeros((self.buffer_size + 1, 2))
        channel.buffer_idx = 0

    def add_constant_data(self, now, curve, channel: Channel):
        channel.data[channel.chunk_idx + 1, 0] = now - self.start_time
        channel.data[channel.chunk_idx + 1, 1] = channel.data[channel.chunk_idx, 1]

        curve.setData(x=channel.data[:channel.chunk_idx + 1, 0],
                      y=channel.data[:channel.chunk_idx + 1, 1])

        channel.ptr += 1

    def update(self):
        now = pg.ptime.time()

        for _, channel in self.channels.items():
            for curve in channel.curves:
                curve.setPos(-(now - self.start_time), 0)

        with self.data_mtx:

            for _, channel in self.channels.items():
                channel.chunk_idx = channel.ptr % self.chunk_size

                if channel.chunk_idx == 0 or (channel.chunk_idx * 1.25) > self.chunk_size:
                    curve = self.create_new_curve(channel)
                else:
                    curve = channel.curves[-1]

                if channel.buffer_idx == 0:
                    self.add_constant_data(now, curve, channel)
                else:
                    self.add_new_data(curve, channel)


def get_source_type(s):
    return s.opts['name'].split()[0]
