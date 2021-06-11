import pickle
import json

import PyQt6.QtCore
import pyqtgraph.parametertree.parameterTypes as pTypes
from pyqtgraph.parametertree import ParameterTree, Parameter

MAX_SOURCES = 10
MAX_MQTT_TOPICS_PER_SOURCE = 10

mqtt_topic_cfg = [
    {'name': 'ID', 'type': 'int', 'value': -1},
    {'name': 'enabled', 'type': 'bool', 'value': False},
    {'name': 'topic', 'type': 'str', 'value': ''},
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
        opts['addText'] = "Add channel"
        pTypes.GroupParameter.__init__(self, **opts)

    def addNew(self, typ=None):
        name = 'Channel {:d}'.format(self.topic_count % MAX_MQTT_TOPICS_PER_SOURCE)
        child = dict(name=name, type='group', children=self.cfg, removable=True, renamable=True)

        self.addChild(child)
        self.topic_count += 1


mqtt_cfg = [
    {'name': 'enabled', 'type': 'bool', 'value': True},
    {'name': 'host', 'type': 'str', 'value': '127.0.0.1:1883'},
    ScalableSources(cfg=mqtt_topic_cfg, name='channels', type='group', children=[])
]

tcp_cfg = [
    {'name': 'enabled', 'type': 'bool', 'value': True},
    {'name': 'host', 'type': 'str', 'value': '127.0.0.1:8888'},
    ScalableSources(cfg=tcp_topic_cfg, name='channels', type='group', children=[])

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

action_cfg = [
    {'name': 'Save', 'type': 'action'},
    {'name': 'Load', 'type': 'action'}
]

param_structure = [
    {'name': 'Grapher configuration dock', 'type': 'group', 'children': action_cfg},
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
        self.parameters.param('Grapher configuration dock', 'Save').sigActivated.connect(self.save)
        self.parameters.param('Grapher configuration dock', 'Load').sigActivated.connect(self.load)

    def save(self):
        user_data = 'grapher.save'
        private = '.grapher.save'
        with open(user_data, 'w') as fid:
            json.dump(self.parameters.saveState(filter='user'), fid, indent=4)

        with open(private, 'w') as fid:
            json.dump(self.parameters.saveState(), fid, indent=4)

    def load(self):
        user_data = 'grapher.save'
        private = '.grapher.save'

        with open(user_data) as fid:
            user_data_dict = json.load(fid)

        with open(private) as fid:
            data = json.load(fid)

        data.update(user_data_dict)

        self.parameters.restoreState(data)
