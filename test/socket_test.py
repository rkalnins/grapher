import asyncio


async def tcp_echo_client(message):
    reader, writer = await asyncio.open_connection(
        '127.0.0.1', 8888)

    i = 100

    while i > 0:
        print(f'Sending: {message!r}')
        writer.write(message.encode())
        await writer.drain()
        i -= 1

        await asyncio.sleep(2.0)

    print('Close the connection')
    writer.close()
    await writer.wait_closed()


asyncio.run(tcp_echo_client('0.05'))
