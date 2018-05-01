import asyncio
import time
from enum import Enum
from arduino_controller import ArduinoController as arduino
from obswsrc import OBSWS
from obswsrc.requests import (GetStreamingStatusRequest,
                              StartStreamingRequest,
                              StopStreamingRequest,
                              StartStopStreamingRequest,
                              StartStopRecordingRequest
                              )
from obswsrc.types import Stream, StreamSettings
from util import get_serial_ports, find_board


class board_status(Enum):
    ''' Pin definitions to read state returned from board '''

    # inputs
    player_pin = 0
    _unused_pin0 = 1
    stream_button_pin = 2

    # outputs
    stream_button_light = 3
    set_webcam_angle = 4
    _unused_btn0 = 5

    # latches
    player_latched = 6
    _unused_latched0 = 7
    stream_button_latched = 8

    # latched values
    player = 9
    _unused_value0 = 10
    stream_button = 11

    # misc
    player_timeout = 13


async def main():
    ports = find_board(get_serial_ports())
    board = arduino(ports[0])

    # Free the buttons!
    board.release_latches()

    # local state, probably should put in a dict
    streaming = False
    obs_recording = False
    obs_streaming = False
    player = False

    # obsws is an asynchronous websocket interface to the obs streaming
    # software.  This opens a connection to the target machine
    async with OBSWS('localhost', 4444, 'password') as obsws:

        # while True:
        try:
            board.flush()
            board.get_state()
            state = board.read()

            if state:
                cmd, state = state

                if not player and state[board_status.player] == 1:
                    player = True
                    # exit standby, run help script or execute some fun
                    # stuff

                if state[board_status.stream_button] == 1:
                    await obsws.require(StartStopStreamingRequest())
                    await obsws.require(StartStopRecordingRequest())

                    obs_status = await obsws.require(GetStreamingStatusRequest())  #NOQA
                    obs_streaming = obs_status['streaming']
                    obs_recording = obs_status['recording']

                if state[board_status.player_timeout] == 1:
                    # go to stadby, script needs to be implemented
                    pass

            if streaming and not (obs_recording or obs_streaming):
                streaming = False
                board.lights(0)

            elif not streaming and (obs_recording or obs_streaming):
                streaming = True
                board.lights(1)

            board.release_latches()

        except ConnectionRefusedError:
            # Yes, blocking.  If I'm not connected, I wait.
            # A max_retry would be nice but this is a one-off.
            print("sleeping a bit, yawn")
            time.sleep(2)


loop = asyncio.get_event_loop()
try:
    asyncio.ensure_future(main())
    loop.run_forever()
except KeyboardInterrupt:
    pass
finally:
    loop.close()
