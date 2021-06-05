class DataPacket:
    device_id: int = 0
    timestamp: float = 0
    data: float = 0


class DataProvider:
    def __init__(self):
        self.data = DataPacket()


