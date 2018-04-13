from PyCmdMessenger import ArduinoBoard, CmdMessenger
import obswsrc
import pudb

COMMANDS = [['ping', ''],
            ['pong', 's'],
            ['player', 'i'],
            ['lights', 's'],
            ['get_state', ''],
            ['ret_state', 'i*'],
            ['release_latches', 's'],
            ['error', '']]


class ArduinoController():
    '''  NGALAC uses an arduino to control button presses and turn on
        various lights.

        Use pseudo terminal to test, ie:
        https://stackoverflow.com/questions/52187/virtual-serial-port-for-linux
        miniterm.py to spy
    '''

    def __init__(self,
                 serial_port="/dev/pts/2",
                 baud_rate=9600):

        self.arduino = ArduinoBoard(serial_port, baud_rate)
        self.commands = dict(COMMANDS)
        self.c = CmdMessenger(self.arduino, COMMANDS)
        self.cmd_seq_num = 0

    def _send_cmd(self, cmd, *args):
        if cmd in self.commands:
            self.c.send(cmd, *args)
        else:
            pass  # heh, logging pls thx
        self.cmd_seq_num += 1

    def _recv_cmd(self, cmd):
        try:
            msg = self.c.receive()
        except EOFError:
            # bad command format
            pass

        if msg:
            return _parse_msg(msg)
        else:
            pass

    def _parse_msg(self, msg):
        # self.last_cmd, value, self.exec_time = msg
        value = msg
        return value

    def ping(self):
        return self._send_cmd('ping')

    def is_player(self):
        return self._send_cmd('player')

    def lights(self, on=True):
        return self._send_cmd('lights', True)

    def get_state(self):
        return self._send_cmd('get_state')

    def release_latches(self):
        return self._send_cmd('release_latches')

    def open(self):
        self.arduino.open()

    def close(self):
        self.arduino.close()
