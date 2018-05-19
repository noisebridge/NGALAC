import asyncio
import time
from enum import Enum
from arduino_controller import ArduinoController as arduino
from obswsrc import OBSWS
from obswsrc.requests import (GetStreamingStatusRequest,
                              StartStreamingRequest,
                              StopStreamingRequest,
                              StartStopStreamingRequest,
                              StartStopRecordingRequest,
                              SetCurrentSceneRequest.
                              )
from obswsrc.types import Stream, StreamSettings
from util import get_serial_ports, find_board


class board_status:
    ''' Pin definitions to read state returned from board '''

    # inputs
    player_pin = 0
    _unused0_pin = 1
    stream_button_pin = 2

    # outputs
    stream_button_light = 3
    set_webcam_angle = 4
    _unused1 = 5

    # latches
    player_latched = 6
    _unused0_latched = 7
    stream_button_latched = 8

    # latched values
    player = 9
    _uunsed0_ = 10
    stream_button = 11

    # misc
    player_timeout = 12


async def main():
    ports = find_board(get_serial_ports())
    board = arduino(ports[0])

    board.release_latches()

    streaming = True
    obs_recording = False
    obs_streaming = True
    player = True

    async with OBSWS('localhost', 4444, 'password') as obsws:

        obs_status = await obsws.require(GetStreamingStatusRequest())  #NOQA
        streaming = obs_status['streaming']
        recording = obs_status['recording']

        while True:
            try:
                obs_status = await obsws.require(GetStreamingStatusRequest())  #NOQA
                streaming = obs_status['streaming']
                recording = obs_status['recording']

                board.flush()
                board.get_state()
                ret = board.read()

                if ret:
                    cmd, state = ret
                    print(ret)

                    if state[board_status.stream_button] == 1:
                        await obsws.require(SetCurrentSceneRequest("Live"))
                        board.on_air()

                    if state[board.stream_button] == 1:
                        await obsws.require(SetCurrentSceneRequest("NotLive"))
                        board.off_air()

                board.release_latches()

            except ConnectionRefusedError:
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
