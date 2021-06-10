import logging

import pyqtgraph as pg
from PyQt6.QtWidgets import QMainWindow
from pyqtgraph.dockarea import *

import grapher.util.grapher_logging as gl
from grapher.Plotter import Plotter, Source
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
        self.cfg.parameters.sigTreeStateChanged.connect(self.update_param)

        self.plotter = Plotter(self.cfg.parameters)
        self.plotter.init_io()
        self.plotter.start()

    def update_param(self, param, changes):
        self.plotter.close()

        for p, change, data in changes:
            path = self.cfg.parameters.childPath(p)
            print(path)
            if path[0] == 'Sources':
                if change == 'childAdded':
                    print('child added')
                    pass
                elif change == 'value':
                    source = self.cfg.parameters.child(path[0], path[1])

                    # if topic is being updated
                    if len(path) == 5:
                        topic = source.child(path[2], path[3])
                        en = topic.child('enabled')
                        c = topic.child('color')

                        # create new source or update existing
                        if path[4] == 'ID':
                            if data not in self.plotter.sources:
                                self.plotter.sources[data] = Source(c)
                        else:
                            topic_id = topic.child('ID')
                            self.plotter.sources[topic_id].enabled = en
                            self.plotter.sources[topic_id].rgb = c.getRgb()

            elif path[0] == 'Plot Configuration':
                pass

        self.plotter.init_io()
        self.plotter.start()

    def reset(self):
        self.plotter.close()
        self.plotter.reset(self.cfg.parameters)
        self.plotter.init_io()
        self.plotter.start()


if __name__ == '__main__':
    logger.info('Starting grapher app')

    app = App()

    dock_plot.addWidget(app.plotter.plot)
    dock_cfg.addWidget(app.cfg.tree)

    win.show()
    logger.debug('exec')
    pg.exec()

    app.plotter.close()
