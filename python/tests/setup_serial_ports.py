import os
import pty
import serial
import subprocess


if os.name == 'nt':
    # running in Windows
    pass
elif os.name == 'posix':
    setup_linux()


class SerialPort():

    def __init__(self):
        self.master, slave = pty.openpty()
        self.port = serial.Serial(os.ttyname(slave))

    def read(self):
        return os.read(master, 1000)

    def write(self, data):
        self.port.write(data)


def create_tty():
    master, slave = pty.openpty()
    slave_name = os.ttyname(slave)
    ser = serial.Serial(slave_name)
    return
