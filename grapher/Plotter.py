import logging
import sys
from threading import Thread, Lock

import numpy as np
import pyqtgraph as pg

import grapher.util.grapher_logging as gl
from grapher.sinks.DataProvider import DataPacket
from sinks.tcp import TCPSink

logger = gl.get_logger(__name__, logging.DEBUG)


class Plotter:
    def __init__(self):
        self.tcp_thread = None
        self.data_mtx = Lock()

        self.plot = None
        self.win = None

        self.chunk_size = 300
        self.buffer_size = 100
        self.max_chunks = 10
        self.start_time = 0

        self.curves = []
        self.data = np.zeros((self.chunk_size + 1, 2))
        self.buffer = np.zeros((self.buffer_size + 1, 2))
        self.ptr = 0
        self.chunk_idx = 0
        self.buffer_idx = 0

        self.flag = 0

        self.timer = pg.QtCore.QTimer()

        self.tcp_sink = TCPSink('127.0.0.1', 8888, self.post_data)  # TODO: configuration

        logger.debug('done init')

    def post_data(self, msg: DataPacket):
        self.data_mtx.acquire()
        logger.debug('%s %s %s %s', self.buffer_idx, msg.data, msg.timestamp, msg.timestamp - self.start_time)
        self.buffer[self.buffer_idx, 0] = msg.timestamp - self.start_time
        self.buffer[self.buffer_idx, 1] = msg.data
        self.buffer_idx += 1

        self.data_mtx.release()

    def init_io(self):
        logger.debug('Getting data')
        self.tcp_sink.start()
        self.tcp_thread = Thread(target=self.tcp_sink.run)
        self.tcp_thread.start()

    def start(self):
        logger.debug('starting')

        self.win = pg.GraphicsLayoutWidget(show=True)
        self.win.setWindowTitle('pyqtgraph example: Scrolling Plots')

        self.plot = self.win.addPlot(colspan=2)
        self.plot.setLabel('bottom', 'Time', 's')
        self.plot.setRange(xRange=[-10, 0], yRange=[-2, 50])

        self.create_new_curve()

        self.timer.timeout.connect(self.update)
        self.timer.start(50)

        logger.debug('done setup')

        self.start_time = pg.ptime.time()

        pg.exec()

    def create_new_curve(self):
        logger.debug('New curve %s', self.chunk_idx)

        curve = self.plot.plot()
        self.curves.append(curve)
        last = self.data[self.chunk_idx - 1]

        self.chunk_idx = 0
        self.ptr = 0

        self.data = np.zeros((self.chunk_size + 1, 2))
        self.data[0] = last

        logger.debug(last)
        logger.debug(self.data[0])

        while len(self.curves) > self.max_chunks:
            c = self.curves.pop(0)
            self.plot.removeItem(c)

        return curve

    def update(self):
        now = pg.ptime.time()
        for c in self.curves:
            c.setPos(-(now - self.start_time), 0)

        self.data_mtx.acquire()

        self.chunk_idx = self.ptr % self.chunk_size

        if self.chunk_idx == 0 or (self.chunk_idx * 1.25) > self.chunk_size:
            curve = self.create_new_curve()
        else:
            curve = self.curves[-1]

        if self.buffer_idx == 0:
            self.data[self.chunk_idx + 1, 0] = now - self.start_time
            self.data[self.chunk_idx + 1, 1] = self.data[self.chunk_idx, 1]

            curve.setData(x=self.data[:self.chunk_idx + 1, 0],
                          y=self.data[:self.chunk_idx + 1, 1])

            self.ptr += 1

        else:
            logger.debug('new data %s', self.buffer_idx)
            # set data with accumulated new data
            start = self.chunk_idx + 1
            end = start + self.buffer_idx

            print(start, end, self.chunk_idx, self.buffer_idx)

            self.data[start:end] = self.buffer[:self.buffer_idx]

            curve.setData(x=self.data[:end, 0],
                          y=self.data[:end, 1])

            # reset buffer and counter
            self.ptr += self.buffer_idx
            self.buffer = np.zeros((self.buffer_size + 1, 2))
            self.buffer_idx = 0

        self.data_mtx.release()
