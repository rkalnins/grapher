import logging

import pyqtgraph as pg
from grapher.util import get_logger
from grapher.app import App, MainWindow


logger = get_logger(__name__, logging.DEBUG)

qapp = pg.mkQApp("Grapher")

logger.info("Starting grapher app")

grapher_app = App()
win = MainWindow(grapher_app)

win.show()
logger.info("Executing...")
pg.exec()
logger.debug("Done...")

grapher_app.plotter.close()
