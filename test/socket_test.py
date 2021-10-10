import asyncio
import math
import sys
import struct
import pyqtgraph as pg


class DataPacket:
    source_id: int = 0
    timestamp = 0
    data: float = 0


async def tcp_test_client():
    reader, writer = await asyncio.open_connection("127.0.0.1", 8888)

    i = 0

    while i < 3000:
        data = DataPacket()
        data.source_id = int(sys.argv[1])
        data.data = math.tan((i - float(sys.argv[2])) / float(sys.argv[3])) + float(
            sys.argv[4]
        )
        data.timestamp = round(pg.ptime.time() * 1000)

        writer.write(struct.pack("ilf", data.source_id, data.timestamp, data.data))
        await writer.drain()
        i += 1

        await asyncio.sleep(0.03)

    print("Close the connection")
    writer.close()
    await writer.wait_closed()


asyncio.run(tcp_test_client())
