try:
    import usocket as socket
except:
    import socket

from machine import UART
uart = UART(1, baudrate=115200)

s = socket.socket()

SERVER_IP = sta_if.ifconfig()[0]
PORT = 8000
HEADER = 1
FORMAT = 'utf-8'

s.bind((SERVER_IP, PORT))
s.listen(10)
print("[LISTENING] Server listening in ({},{})".format(SERVER_IP, PORT))

while True:

    conn, addr = s.accept()
    connected = True
    while connected:
        length = conn.recv(HEADER)[0]
        data = conn.recv(length)
        buffer = [length]
        for b in data:
            buffer.append(b)
        print("Received data: {}".format(buffer))
        uart.write(bytearray(buffer))
        connected = False

