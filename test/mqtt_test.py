import time

import paho.mqtt.client as mqtt

import asyncio
import sys
import struct

import pyqtgraph as pg


host = '127.0.0.1'

client = mqtt.Client('test-' + str(sys.argv[1]))
client.connect(host)


class DataPacket:
    source_id: int = 0
    timestamp = 0
    data: float = 0

def mqtt_test():
    i = 0

    print('starting')
    while i < 400:
        data = DataPacket()
        data.source_id = int(sys.argv[1])
        data.data = -1 * i % int(sys.argv[2]) + 0.7
        data.timestamp = round(pg.ptime.time() * 1000)

        packed = struct.pack('ilf', data.source_id, data.timestamp, data.data)

        res = client.publish('topic/test/a', packed)

        if res[0] != 0:
            print('failed to send')
        else:
            print('sent ', packed)


        time.sleep(0.5)


if __name__ == '__main__':
    print('running')
    mqtt_test()
