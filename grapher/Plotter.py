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

class Source:
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

        self.sources = dict()
        self.populate_sources(ptree)

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

        self.sources = dict()
        self.populate_sources(ptree)

        self.timer = pg.QtCore.QTimer()

    def populate_sources(self, ptree: Parameter):
        sources = ptree.child('Sources')
        for s in sources:
            src_type = s.opts['name'].split()[0]
            if src_type == 'MQTT':
                self.add_mqtt_source(s)
            elif src_type == 'TCP':
                self.add_tcp_source(s)

    def add_mqtt_source(self, s):
        logger.debug('Adding MQTT source')
        # support only one MQTT source right now
        if not self.mqtt_enabled:
            self.mqtt_enabled = True
            topics = []

            for topic in s.child('topics'):
                if len(topic['path']) > 0:
                    self.sources[topic['ID']] = Source(self.chunk_size, self.buffer_size, topic['color'])
                    topics.append(topic['path'])

            addr: str = s['host']
            parts = addr.split(':')

            self.mqtt_sink = MqttSink(parts[0], int(parts[1]), topics)

    def add_tcp_source(self, s):
        logger.debug('Adding TCP source')

        # support only one MQTT source right now
        if not self.tcp_enabled:
            self.tcp_enabled = True

            for topic in s.child['topics']:
                self.sources[topic['ID']] = Source(topic['color'])

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
            self.mqtt_sink.connect()

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

        if not self.sources[msg.source_id].init:
            self.sources[msg.source_id].init = True
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
