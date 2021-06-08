import PyQt6.QtCore


class DataPacket:
    device_id: int = 0
    timestamp: float = 0
    data: float = 0


class DataProvider(PyQt6.QtCore.QObject):
    def __init__(self):
        super().__init__()
        self.data = DataPacket()
