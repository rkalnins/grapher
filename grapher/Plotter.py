import logging
from threading import Thread, Lock

import PyQt6.QtCore
import numpy as np
import pyqtgraph as pg
from PyQt6.QtCore import QThread

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

        self.devices = dict()

        self.timer = pg.QtCore.QTimer()

        self.tcp_sink = TCPSink('127.0.0.1', 8888, self.post_data)  # TODO: configuration

        logger.debug('done init')

    def init_io(self):
        logger.debug('Getting data')
        self.tcp_sink.start()
        self.tcp_thread = QThread()
        self.tcp_sink.moveToThread(self.tcp_thread)
        self.tcp_thread.started.connect(self.tcp_sink.run)

    def start(self):
        logger.debug('starting')

        self.tcp_sink.post_signal.connect(self.post_data)

        self.win = pg.GraphicsLayoutWidget(show=True)
        self.win.setWindowTitle('pyqtgraph example: Scrolling Plots')

        self.plot = self.win.addPlot(colspan=1)
        self.plot.setLabel('bottom', 'Time', 's')
        self.plot.setRange(xRange=[-10, 0], yRange=[-2, 50])

        self.timer.timeout.connect(self.update)
        self.timer.start(50)

        self.tcp_thread.start()

        logger.debug('done setup')

        self.start_time = pg.ptime.time()

        pg.exec()

    def close(self):
        self.tcp_sink.close()

    @PyQt6.QtCore.pyqtSlot(DataPacket)
    def post_data(self, msg: DataPacket):

        if msg.device_id not in self.devices:
            self.devices[msg.device_id] = Device(self.chunk_size, self.buffer_size)
            self.create_new_curve(self.devices[msg.device_id])

        d = self.devices[msg.device_id]
        logger.debug('%s %s %s %s', d.buffer_idx, msg.data, msg.timestamp, msg.timestamp - self.start_time)

        self.data_mtx.acquire()
        d.buffer[d.buffer_idx, 0] = msg.timestamp - self.start_time
        d.buffer[d.buffer_idx, 1] = msg.data
        d.buffer_idx += 1
        self.data_mtx.release()

    def create_new_curve(self, device: Device):
        logger.debug('New curve %s', device.chunk_idx)

        self.curve_mtx.acquire()

        curve = self.plot.plot(pen=device.rgb)
        device.curves.append(curve)
        last = device.data[device.chunk_idx - 1]

        device.chunk_idx = 0
        device.ptr = 0

        device.data = np.zeros((self.chunk_size + 1, 2))
        device.data[0] = last

        while len(device.curves) > self.max_chunks:
            c = device.curves.pop(0)
            self.plot.removeItem(c)

        self.curve_mtx.release()

        return curve

    def add_new_data(self, curve: pg.plot, device: Device):
        logger.debug('new data %s', device.buffer_idx)
        # set data with accumulated new data
        start = device.chunk_idx + 1
        end = start + device.buffer_idx

        print(start, end, device.chunk_idx, device.buffer_idx)

        device.data[start:end] = device.buffer[:device.buffer_idx]

        curve.setData(x=device.data[:end, 0],
                      y=device.data[:end, 1])

        # reset buffer and counter
        device.ptr += device.buffer_idx
        device.buffer = np.zeros((self.buffer_size + 1, 2))
        device.buffer_idx = 0

    def add_constant_data(self, now, curve, device: Device):
        device.data[device.chunk_idx + 1, 0] = now - self.start_time
        device.data[device.chunk_idx + 1, 1] = device.data[device.chunk_idx, 1]

        curve.setData(x=device.data[:device.chunk_idx + 1, 0],
                      y=device.data[:device.chunk_idx + 1, 1])

        device.ptr += 1

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
                self.add_constant_data(now, curve, d)
            else:
                self.add_new_data(curve, d)

        self.data_mtx.release()
