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
                              SetCurrentSceneRequest
                              )
from obswsrc.types import Stream, StreamSettings
from util import get_serial_ports, find_board
# from aiologger import Logger

# logger = Logger.with_default_handlers()


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

    streaming = False
    startup = True
    board.release_latches()

    async with OBSWS('localhost', 4444, 'password') as obsws:

        if startup:
            board.flush()
            board.get_state()
            ret = board.read()
            if ret:
                cmd, state = ret

            board.off_air()
            await obsws.require(SetCurrentSceneRequest(
                {"scene-name": "NotLive"}))
            await obsws.require(StopStreamingRequest())
            streaming = False
            state[board_status.stream_button] = 0
            startup = False

        obs_status = await obsws.require(GetStreamingStatusRequest())  #NOQA
        streaming = obs_status['streaming']

        while True:
            try:
                obs_status = await obsws.require(GetStreamingStatusRequest())  #NOQA
                if obs_status is not None:
                    streaming = obs_status['streaming']

                board.flush()
                board.get_state()
                ret = board.read()

                if ret:
                    cmd, state = ret
                    # print("{}  :  {}".format(ret, streaming))
                    # logger.info(cmd)
                    # logger.info(state)
                    if state[board_status.stream_button] == 1 \
                            and streaming is False:
                        board.on_air()
                        await obsws.require(SetCurrentSceneRequest(
                            {"scene-name": "Live"}))
                        await obsws.require(StartStreamingRequest())
                        streaming = True
                        state[board_status.stream_button] = 0

                    elif state[board_status.stream_button] == 1 \
                            and streaming is True:
                        board.off_air()
                        await obsws.require(SetCurrentSceneRequest(
                            {"scene-name": "NotLive"}))
                        await obsws.require(StopStreamingRequest())
                        streaming = False
                        state[board_status.stream_button] = 0

                    elif state[board_status.player_timeout] == 0 \
                            and streaming is True:
                        board.off_air()
                        await obsws.require(SetCurrentSceneRequest(
                            {"scene-name": "NotLive"}))
                        await obsws.require(StopStreamingRequest())
                        streaming = False
                        state[board_status.stream_button] = 0

                board.release_latches()

            except ValueError:
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
