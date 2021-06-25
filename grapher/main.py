import logging

import PyQt6
import PyQt6.QtCore
import pyqtgraph as pg
from PyQt6.QtWidgets import QMainWindow
from pyqtgraph.dockarea import *

import grapher.util.grapher_logging as gl
from grapher.Plotter import Plotter
from grapher.config.config import ConfigurationHandler

logger = gl.get_logger(__name__, logging.DEBUG)

qapp = pg.mkQApp("Grapher")


class MainWindow(QMainWindow):
    def __init__(self, app):
        super().__init__()
        area = DockArea()
        self.setCentralWidget(area)
        self.resize(1000, 500)
        self.setWindowTitle('Grapher')
        self.app = app

        self.dock_plot = Dock("Plot", size=(750, 500))
        self.dock_cfg = Dock("Config", size=(250, 500))

        area.addDock(self.dock_plot, 'left')
        area.addDock(self.dock_cfg, 'right', self.dock_plot)

        self.dock_plot.addWidget(app.plotter.plot)
        self.dock_cfg.addWidget(app.cfg.tree)

    def keyPressEvent(self, event: PyQt6.QtGui.QKeyEvent):
        super(MainWindow, self).keyPressEvent(event)
        logger.debug('key pressed')
        if event.key() == PyQt6.QtCore.Qt.Key.Key_Space:
            app.plotter.toggle_pause()


class App:
    def __init__(self):
        self.cfg = ConfigurationHandler()
        self.plotter = Plotter(self.cfg.parameters, self.cfg.channels)
        self.plotter.init_io()
        self.plotter.start()


if __name__ == '__main__':
    logger.info('Starting grapher app')

    grapher_app = App()
    win = MainWindow(grapher_app)

    win.show()
    logger.info('Executing...')
    pg.exec()
    logger.debug('Done...')

    grapher_app.plotter.close()
