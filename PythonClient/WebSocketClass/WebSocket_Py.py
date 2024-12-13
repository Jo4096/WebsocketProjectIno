import asyncio
import socket

class WebSocket:  # Maybe rename this class to something like 'SocketClient' since it's no longer using websockets
    def __init__(self, ip, port):
        self.ip = ip
        self.port = port
        self.is_connected = False  # Track connection state
        self.blocking_mode = True  # Default is blocking mode
        self.client_socket = None
        self.commands = []

        self.sendValue = None
        self.recvValue = None

    async def connect(self):
        if not self.is_connected:
            try:
                # Create a socket and connect to the server
                self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self.client_socket.connect((self.ip, self.port))
                self.is_connected = True
                print(f"Connected to {self.ip}:{self.port}")
            except OSError as e:
                print(f"Connection error: {e}")
        else:
            print("Already connected to the server")

    async def send(self, message):
        if self.is_connected:
            # Ensure the message ends with a newline character
            if not message.endswith('\n'):
                message += '\n'
            self.client_socket.sendall(message.encode())
        else:
            print("Not connected. Unable to send data.")

    async def recv(self):
        if self.blocking_mode:
            self.recvValue = await self.recv_blocking()
            return self.recvValue
        else:
            self.recvValue = await self.recv_non_blocking()
            return self.recvValue

    async def recv_non_blocking(self):
        if self.is_connected:
            print("Waiting for response (non-blocking)...")
            try:
                self.client_socket.setblocking(False)
                response = self.client_socket.recv(1024).decode()
                return response.strip()  # Remove any trailing newlines or whitespace
            except socket.error as e:
                print(f"Receive error: {e}")
                return None
        else:
            print("Not connected. Unable to receive data.")
            return None

    async def recv_blocking(self):
        if self.is_connected:
            print("Waiting for response (blocking)...")
            try:
                self.client_socket.setblocking(True)
                response = self.client_socket.recv(1024).decode()
                return response.strip()  # Remove any trailing newlines or whitespace
            except socket.error as e:
                print(f"Receive error: {e}")
                return None
        else:
            print("Not connected. Unable to receive data.")
            return None

    async def close(self):
        if self.is_connected:
            self.client_socket.close()
            self.is_connected = False
            print("Connection closed")
        else:
            print("Socket is not connected")

    async def getDataAsNumber(self, separator=";", dataDict=False, definer="="):
        dataString = await self.recv()

        # Check if dataString is valid
        if not isinstance(dataString, str):
            print("No instance")
            return None

        # Split the string using the provided separator or space if the separator is not found
        try:
            if separator in dataString:
                dataVec = dataString.split(separator)
            else:
                # Treat the entire string as a single item if there's no separator
                dataVec = [dataString]
        except Exception:
            print("ISSUE SPLITTING")
            return None  # Return None if there's an issue during splitting

        # Initialize the result based on whether a dict or list is needed
        data = {} if dataDict else []

        # Parse each item in the split data
        for item in dataVec:
            # If expecting a dict, handle key-value pairs
            if dataDict:
                try:
                    if definer in item:
                        key, value = item.split(definer)
                    elif ':' in item:  # Fallback for cases where ":" is used instead of "="
                        key, value = item.split(':')
                    else:
                        continue

                    key = key.strip()  # Remove extra spaces around key
                    value = value.strip()  # Remove extra spaces around value

                    #print(f"Processing key: {key}, value: {value}")  # Debugging output

                    try:
                        data[key] = float(value)  # Convert value to a float
                    except ValueError:
                        print(f"Failed to convert value for key: {key}")  # Debugging output
                        return None
                except Exception:
                    print(f"Error in splitting or processing item: {item}")  # Debugging output
                    return None
            else:
                # Try to extract the numeric part from the item
                if definer in item:
                    key, value = item.split(definer)
                    try:
                        value = value.strip()
                        data.append(float(value))  # Convert the numeric value to float
                    except ValueError:
                        print(f"Conversion failed for: {item}")  # Debugging output
                        return None
                else:
                    # Handle cases where there is no definer (if that occurs)
                    try:
                        item_stripped = item.strip()
                        data.append(float(item_stripped))  # Convert to float
                    except ValueError:
                        print(f"Conversion failed for: {item_stripped}")  # Debugging output
                        return None

        return data


    
    async def getCommands(self, separator = ";"):
        await self.send("GETCMD")
        await self.recv()
        self.commands = self.recvValue.split(separator)
        return self.commands
