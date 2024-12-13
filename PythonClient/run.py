import asyncio
import socket
from WebSocketClass import WebSocket_Py as WEB


FLAG_USE_DISCORD = True
FLAG_USE_SOCKET = False

IP = "ESP's_IP"
PORT = 80




async def main():
    print("SET UP...")
    web = None

    try:
        web = WEB.WebSocket(ip=IP, port=PORT)
        await web.connect()
        cmds = await web.getCommands(";") #Built in function inside the websocket arduino library (keep in mind that it takes space of the functions array, so in reality you have N-1 free space)
        print(cmds)
        await web.close()
    except Exception as e:
        print(f"Failed to initialize WebSocket: {e}")
        return

    choice = 0
    while choice != len(cmds):
        print("AVAILABLE CMDS:")
        for i, cmd in enumerate(cmds):
            print(f"{i}: {cmd}")
        print(f"{len(cmds)}: Quit")

        try:
            choice = int(input("> "))
            if choice < 0 or choice > len(cmds):
                print("Please type a valid choice")
                continue
        except ValueError:
            print("Please type an integer...")
            continue

        if choice == len(cmds):
            break

        message = cmds[choice]

        if "=" in message:  # Check if message contains '='
            try:
                number = float(input("Type the value you want to send: "))
                message += str(number)
            except ValueError:
                print("Please type a number...")
                continue

        await web.connect()
        await web.send(message)
        print(await web.recv())
        await web.close()


asyncio.run(main())