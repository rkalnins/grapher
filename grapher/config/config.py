import PyQt6.QtCore
import pyqtgraph.parametertree.parameterTypes as pTypes
from pyqtgraph.parametertree import ParameterTree, Parameter

MAX_SOURCES = 10
MAX_MQTT_TOPICS_PER_SOURCE = 10

mqtt_topic_cfg = [
    {'name': 'ID', 'type': 'int', 'value': -1},
    {'name': 'path', 'type': 'str', 'value': ''},
    {'name': 'color', 'type': 'color', 'value': '#000FF0'},
]

tcp_topic_cfg = [
    {'name': 'ID', 'type': 'int', 'value': -1},
    {'name': 'color', 'type': 'color', 'value': '#000FF0'},
]


class ScalableSources(pTypes.GroupParameter):
    def __init__(self, cfg=None, **opts):
        self.topic_count = 0
        self.cfg = cfg
        opts['type'] = 'group'
        opts['addText'] = "Add topic"
        pTypes.GroupParameter.__init__(self, **opts)

    def addNew(self, typ=None):
        name = 'Topic {:d}'.format(self.topic_count % MAX_MQTT_TOPICS_PER_SOURCE)
        child = dict(name=name, type='group', children=self.cfg, removable=True, renamable=True)

        self.addChild(child)
        self.topic_count += 1


mqtt_cfg = [
    {'name': 'enabled', 'type': 'bool', 'value': True},
    {'name': 'host', 'type': 'str', 'value': '127.0.0.1:1883'},
    ScalableSources(cfg=mqtt_topic_cfg, name='topics', type='group', children=[])
]

tcp_cfg = [
    {'name': 'enabled', 'type': 'bool', 'value': True},
    {'name': 'host', 'type': 'str', 'value': '127.0.0.1:8888'},
    ScalableSources(cfg=tcp_topic_cfg, name='topics', type='group', children=[])

]

serial_cfg = [
    {'name': 'enabled', 'type': 'bool', 'value': True}
]


class ScalableSourcesGroup(pTypes.GroupParameter):
    def __init__(self, **opts):
        self.source_counts = {
            'MQTT': 0,
            'TCP': 0,
            'Serial': 0,
        }
        opts['type'] = 'group'
        opts['addText'] = "Add source"
        opts['addList'] = ['MQTT', 'TCP', 'Serial']
        pTypes.GroupParameter.__init__(self, **opts)

    def addNew(self, typ=None):
        val = {
            'MQTT': mqtt_cfg,
            'TCP': tcp_cfg,
            'Serial': serial_cfg
        }[typ]

        self.source_counts[typ] += 1

        name = '{} {:d}'.format(typ, self.source_counts[typ] % MAX_SOURCES)
        child = dict(name=name, type='group', children=val, removable=True, renamable=True)

        self.addChild(child)


plot_cfg = [
    {'name': 'xmin', 'type': 'int', 'value': -20},
    {'name': 'xmax', 'type': 'int', 'value': 20},
    {'name': 'ymin', 'type': 'int', 'value': -10},
    {'name': 'ymax', 'type': 'int', 'value': 50}
]

param_structure = [
    {'name': 'Grapher configuration dock'},
    {'name': 'Plot configuration', 'type': 'group', 'children': plot_cfg},
    ScalableSourcesGroup(name='Sources', tip='Select source', type='group', children=[])
]


class ConfigurationHandler(PyQt6.QtCore.QObject):
    def __init__(self):
        super().__init__()
        self.parameters = Parameter.create(name='config', type='group', children=param_structure)
        self.tree = ParameterTree()
        self.tree.setParameters(self.parameters, showTop=False)
        self.tree.sizeHint().width()
