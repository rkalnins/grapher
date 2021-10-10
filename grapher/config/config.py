import json
import pickle

import PyQt6.QtCore
from pyqtgraph.parametertree import ParameterTree, Parameter

plot_cfg = [
    {"name": "xmin", "type": "int", "value": -20},
    {"name": "xmax", "type": "int", "value": 20},
    {"name": "ymin", "type": "int", "value": -10},
    {"name": "ymax", "type": "int", "value": 50},
]

action_cfg = [{"name": "Save", "type": "action"}, {"name": "Load", "type": "action"}]

param_structure = [
    {"name": "Grapher configuration dock"},
    {"name": "Grapher configuration actions", "type": "group", "children": action_cfg},
    {"name": "Plot configuration", "type": "group", "children": plot_cfg},
]


class ConfigurationHandler(PyQt6.QtCore.QObject):
    def __init__(self):
        super().__init__()
        self.parameters = Parameter.create(
            name="config", type="group", children=param_structure
        )
        self.tree = ParameterTree()
        self.tree.setParameters(self.parameters, showTop=False)
        self.channels = dict()
        self.parameters.param(
            "Grapher configuration actions", "Save"
        ).sigActivated.connect(self.save)
        self.parameters.param(
            "Grapher configuration actions", "Load"
        ).sigActivated.connect(self.load)

        self.channels_file = "channels.json"
        self.dat_file = ".grapher.dat"

        self.load_channels()

    def save(self):
        with open(self.dat_file, "wb") as fid:
            pickle.dump(self.parameters.saveState(), fid)

    def load_channels(self):
        with open(self.channels_file, "r") as fid:
            self.channels = json.load(fid)

    def load(self):
        with open(self.dat_file, "rb") as fid:
            data = pickle.load(fid)

        self.parameters.restoreState(data)

        self.parameters.param(
            "Grapher configuration actions", "Save"
        ).sigActivated.connect(self.save)
        self.parameters.param(
            "Grapher configuration actions", "Load"
        ).sigActivated.connect(self.load)
