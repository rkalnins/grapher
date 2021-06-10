import logging

import pyqtgraph as pg
from PyQt6.QtWidgets import QMainWindow
from pyqtgraph.Qt import QtGui
from pyqtgraph.dockarea import *

import grapher.util.grapher_logging as gl
from grapher.Plotter import Plotter

logger = gl.get_logger(__name__, logging.DEBUG)

app = pg.mkQApp("Grapher")

win = QMainWindow()
area = DockArea()

win.setCentralWidget(area)
win.resize(1000, 500)
win.setWindowTitle('Grapher')

dock_plot = Dock("Plot", size=(500, 200))
dock_cfg = Dock("Config", size=(500, 200))

area.addDock(dock_plot, 'left')
area.addDock(dock_cfg, 'right', dock_plot)

if __name__ == '__main__':
    logger.info('Starting grapher app')

    plotter = Plotter()
    plotter.init_io()
    plotter.start()

    dock_plot.addWidget(plotter.plot)

    win.show()
    logger.debug('exec')
    pg.exec()

    plotter.close()
