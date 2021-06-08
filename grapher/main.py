import logging

import grapher.util.grapher_logging as gl
from grapher.Plotter import Plotter

logger = gl.get_logger(__name__, logging.DEBUG)


if __name__ == '__main__':
    logger.info('Starting grapher app')
    plotter = Plotter()
    plotter.init_io()
    logger.debug('exec')
    plotter.start()
    plotter.close()

