import sys
import glob
import serial
from arduino_controller import ArduinoController


# to spy:  sudo picocom -b 9600 /dev/ttyUSB0
def get_serial_ports():

    if sys.platform.startswith('win'):
        ports = ['COM{}'.format(i+1) for i in range(255)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cyg'):
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('What computer is this? An atari?')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass

    return result


def find_board(ports):

    boards = []

    for port in ports:
        try:
            board = ArduinoController(serial_port=port)
            board.ping()
            ayt = board._recv_cmd()
            if not ayt:
                raise ValueError('No board on port {}'.format(port))

            if ayt[1][0] == 'pong' and board.check_firmware():
                board.close()
                boards.append(port)

        except BaseException as e:
            raise e
            # bad.append((port, 'port open, but no response. firmware?'))

    return boards
