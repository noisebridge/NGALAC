import sys
import glob
import serial


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


if __name__ == '__main__':
    print(get_serial_ports())
