import logging
from threading import Thread, Lock

import numpy as np
import pyqtgraph as pg

import grapher.util.grapher_logging as gl
from grapher.sinks.DataProvider import DataPacket
from sinks.tcp import TCPSink

logger = gl.get_logger(__name__, logging.DEBUG)


class Device:
    def __init__(self, chunk_size, buffer_size):
        self.curves = list()
        self.data = np.zeros((chunk_size + 1, 2))
        self.buffer = np.zeros((buffer_size + 1, 2))
        self.ptr = 0
        self.chunk_idx = 0
        self.buffer_idx = 0


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

        self.devices = dict()

        self.timer = pg.QtCore.QTimer()

        self.tcp_sink = TCPSink('127.0.0.1', 8888, self.post_data)  # TODO: configuration

        logger.debug('done init')

    def post_data(self, msg: DataPacket):
        self.data_mtx.acquire()

        if msg.device_id not in self.devices:
            self.devices[msg.device_id] = Device(self.chunk_size, self.buffer_size)
            self.create_new_curve(self.devices[msg.device_id])

        d = self.devices[msg.device_id]
        logger.debug('%s %s %s %s', d.buffer_idx, msg.data, msg.timestamp, msg.timestamp - self.start_time)

        d.buffer[d.buffer_idx, 0] = msg.timestamp - self.start_time
        d.buffer[d.buffer_idx, 1] = msg.data
        d.buffer_idx += 1

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

        self.timer.timeout.connect(self.update)
        self.timer.start(50)

        logger.debug('done setup')

        self.start_time = pg.ptime.time()

        pg.exec()

    def create_new_curve(self, device):
        logger.debug('New curve %s', device.chunk_idx)

        curve = self.plot.plot()
        device.curves.append(curve)
        last = device.data[device.chunk_idx - 1]

        device.chunk_idx = 0
        device.ptr = 0

        device.data = np.zeros((self.chunk_size + 1, 2))
        device.data[0] = last

        while len(device.curves) > self.max_chunks:
            c = device.curves.pop(0)
            self.plot.removeItem(c)

        return curve

    def update(self):
        now = pg.ptime.time()

        for _, d in self.devices.items():
            for c in d.curves:
                c.setPos(-(now - self.start_time), 0)

        self.data_mtx.acquire()

        for _, d in self.devices.items():
            d.chunk_idx = d.ptr % self.chunk_size

            if d.chunk_idx == 0 or (d.chunk_idx * 1.25) > self.chunk_size:
                curve = self.create_new_curve(d)
            else:
                curve = d.curves[-1]

            if d.buffer_idx == 0:
                d.data[d.chunk_idx + 1, 0] = now - self.start_time
                d.data[d.chunk_idx + 1, 1] = d.data[d.chunk_idx, 1]

                curve.setData(x=d.data[:d.chunk_idx + 1, 0],
                              y=d.data[:d.chunk_idx + 1, 1])

                d.ptr += 1

            else:
                logger.debug('new data %s', d.buffer_idx)
                # set data with accumulated new data
                start = d.chunk_idx + 1
                end = start + d.buffer_idx

                print(start, end, d.chunk_idx, d.buffer_idx)

                d.data[start:end] = d.buffer[:d.buffer_idx]

                curve.setData(x=d.data[:end, 0],
                              y=d.data[:end, 1])

                # reset buffer and counter
                d.ptr += d.buffer_idx
                d.buffer = np.zeros((self.buffer_size + 1, 2))
                d.buffer_idx = 0

        self.data_mtx.release()
