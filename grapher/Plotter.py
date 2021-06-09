import logging
from threading import Thread, Lock

import PyQt6.QtCore
import numpy as np
import pyqtgraph as pg
from PyQt6.QtCore import QThread

import grapher.util.grapher_logging as gl
from grapher.sinks.DataProvider import DataPacket
from sinks.tcp import TCPSink
from sinks.mqtt import MqttSink

logger = gl.get_logger(__name__, logging.DEBUG)

TCP_SINK = 0
MQTT_SINK = 1

class Source:
    def __init__(self, chunk_size, buffer_size):
        self.curves = list()
        self.data = np.zeros((chunk_size + 1, 2))
        self.buffer = np.zeros((buffer_size + 1, 2))
        self.ptr = 0
        self.chunk_idx = 0
        self.buffer_idx = 0
        self.rgb = (100, 120, 240)


class Plotter(PyQt6.QtCore.QObject):
    def __init__(self):
        super().__init__()
        self.tcp_thread = None
        self.data_mtx = Lock()
        self.curve_mtx = Lock()

        self.plot = None
        self.win = None

        self.chunk_size = 300
        self.buffer_size = 100
        self.max_chunks = 10
        self.start_time = 0

        self.sources = dict()

        self.timer = pg.QtCore.QTimer()
        self.sink_type = MQTT_SINK

        if self.sink_type == TCP_SINK:
            self.tcp_sink = TCPSink('127.0.0.1', 8888, self.post_data)  # TODO: configuration
        elif self.sink_type == MQTT_SINK:
            self.mqtt_sink = MqttSink()

        logger.debug('done init')

    def init_io(self):
        logger.debug('Getting data')
        if self.sink_type == TCP_SINK:
            self.tcp_sink.start()
            self.tcp_thread = QThread()
            self.tcp_sink.moveToThread(self.tcp_thread)
            self.tcp_thread.started.connect(self.tcp_sink.run)
        elif self.sink_type == MQTT_SINK:
            self.mqtt_sink.connect()

    def start(self):
        logger.debug('starting')

        self.win = pg.GraphicsLayoutWidget(show=True)
        self.win.setWindowTitle('pyqtgraph example: Scrolling Plots')

        self.plot = self.win.addPlot(colspan=1)
        self.plot.setLabel('bottom', 'Time', 's')
        self.plot.setRange(xRange=[-10, 0], yRange=[-2, 50])

        self.timer.timeout.connect(self.update)
        self.timer.start(50)

        if self.sink_type == TCP_SINK:
            self.tcp_sink.post_signal.connect(self.post_data)
            self.tcp_thread.start()
        elif self.sink_type == MQTT_SINK:
            self.mqtt_sink.post_signal.connect(self.post_data)
            self.mqtt_sink.start()

        logger.debug('done setup')

        self.start_time = pg.ptime.time()

        pg.exec()

    def close(self):
        if self.sink_type == TCP_SINK:
            self.tcp_sink.close()
        elif self.sink_type == MQTT_SINK:
            pass

    @PyQt6.QtCore.pyqtSlot(DataPacket)
    def post_data(self, msg: DataPacket):

        if msg.source_id not in self.sources:
            self.sources[msg.source_id] = Source(self.chunk_size, self.buffer_size)
            self.create_new_curve(self.sources[msg.source_id])

        s = self.sources[msg.source_id]
        logger.debug('%s %s %s %s', s.buffer_idx, msg.data, msg.timestamp, msg.timestamp - self.start_time)

        with self.data_mtx:
            s.buffer[s.buffer_idx, 0] = msg.timestamp - self.start_time
            s.buffer[s.buffer_idx, 1] = msg.data
            s.buffer_idx += 1

    def create_new_curve(self, source: Source):
        logger.debug('New curve %s', source.chunk_idx)

        with self.curve_mtx:
            curve = self.plot.plot(pen=source.rgb)
            source.curves.append(curve)
            last = source.data[source.chunk_idx - 1]

            source.chunk_idx = 0
            source.ptr = 0

            source.data = np.zeros((self.chunk_size + 1, 2))
            source.data[0] = last

            while len(source.curves) > self.max_chunks:
                c = source.curves.pop(0)
                self.plot.removeItem(c)

        return curve

    def add_new_data(self, curve: pg.plot, source: Source):
        logger.debug('new data %s', source.buffer_idx)
        # set data with accumulated new data
        start = source.chunk_idx + 1
        end = start + source.buffer_idx

        print(start, end, source.chunk_idx, source.buffer_idx)

        source.data[start:end] = source.buffer[:source.buffer_idx]

        curve.setData(x=source.data[:end, 0],
                      y=source.data[:end, 1])

        # reset buffer and counter
        source.ptr += source.buffer_idx
        source.buffer = np.zeros((self.buffer_size + 1, 2))
        source.buffer_idx = 0

    def add_constant_data(self, now, curve, source: Source):
        source.data[source.chunk_idx + 1, 0] = now - self.start_time
        source.data[source.chunk_idx + 1, 1] = source.data[source.chunk_idx, 1]

        curve.setData(x=source.data[:source.chunk_idx + 1, 0],
                      y=source.data[:source.chunk_idx + 1, 1])

        source.ptr += 1

    def update(self):
        now = pg.ptime.time()

        for _, s in self.sources.items():
            for c in s.curves:
                c.setPos(-(now - self.start_time), 0)

        with self.data_mtx:

            for _, s in self.sources.items():
                s.chunk_idx = s.ptr % self.chunk_size

                if s.chunk_idx == 0 or (s.chunk_idx * 1.25) > self.chunk_size:
                    curve = self.create_new_curve(s)
                else:
                    curve = s.curves[-1]

                if s.buffer_idx == 0:
                    self.add_constant_data(now, curve, s)
                else:
                    self.add_new_data(curve, s)

