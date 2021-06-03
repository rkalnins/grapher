import logging

import numpy as np
import pyqtgraph as pg

import threading

import grapher.util.grapher_logging as gl
from sinks.tcp import TCPSink

win = pg.GraphicsLayoutWidget(show=True)
win.setWindowTitle('pyqtgraph example: Scrolling Plots')

logger = gl.get_logger('main', logging.DEBUG)

tcp_sink = TCPSink('127.0.0.1', 8888)  # TODO: configuration

chunkSize = 100
# Remove chunks after we have 10
maxChunks = 10
startTime = pg.ptime.time()
win.nextRow()
plt = win.addPlot(colspan=2)
plt.setLabel('bottom', 'Time', 's')
plt.setXRange(-10, 0)
curves = []
data = np.empty((chunkSize + 1, 2))
ptr = 0


def update():
    global plt, data, ptr, curves

    now = pg.ptime.time()
    for c in curves:
        c.setPos(-(now - startTime), 0)

    i = ptr % chunkSize
    if i == 0:
        curve = plt.plot()
        curves.append(curve)
        last = data[-1]
        data = np.empty((chunkSize + 1, 2))
        data[0] = last
        while len(curves) > maxChunks:
            c = curves.pop(0)
            plt.removeItem(c)
    else:
        curve = curves[-1]
    data[i + 1, 0] = now - startTime
    data[i + 1, 1] = tcp_sink.get()
    curve.setData(x=data[:i + 2, 0], y=data[:i + 2, 1])
    ptr += 1


def init_io():
    logger.debug('Getting data')
    tcp_sink.start()
    tcp_t = threading.Thread(target=tcp_sink.run)
    tcp_t.start()


timer = pg.QtCore.QTimer()
timer.timeout.connect(update)
timer.start(50)

if __name__ == '__main__':
    logger.info('Starting grapher app')
    init_io()

    logger.debug('exec')
    pg.exec()
