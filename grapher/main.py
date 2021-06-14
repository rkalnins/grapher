import logging

import pyqtgraph as pg
from PyQt6.QtWidgets import QMainWindow
from pyqtgraph.dockarea import *

import grapher.util.grapher_logging as gl
from grapher.Plotter import Plotter
from grapher.config.config import ConfigurationHandler

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


class App:
    def __init__(self):
        self.cfg = ConfigurationHandler()

        self.plotter = Plotter(self.cfg.parameters, self.cfg.channels)
        self.plotter.init_io()
        self.plotter.start()

    def reset(self):
        self.plotter.close()
        self.plotter.reset(self.cfg.parameters, self.cfg.channels)
        self.plotter.init_io()
        self.plotter.start()


if __name__ == '__main__':
    logger.info('Starting grapher app')

    app = App()

    dock_plot.addWidget(app.plotter.plot)
    dock_cfg.addWidget(app.cfg.tree)

    win.show()
    logger.info('Executing...')
    pg.exec()
    logger.debug('Done...')

    app.plotter.close()
