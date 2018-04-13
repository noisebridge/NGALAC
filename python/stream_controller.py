import asyncio
from obswsrc import OBSWS
from obswsrc.requests import (GetStreamingStatusRequest,
        StartStreamingRequest,
        StopStreamingRequest,
        StartStopStreamingRequest,
        StartStopRecordingRequest
        )
from obswsrc.types import Stream, StreamSettings
from arduino_controller import ArduinoController as arduino

# need port detection to pass to arduino controller
async def main():
    g = arduino()
    streaming = False

    async with OBSWS('localhost', 4444, 'password') as obsws:
        while True:
            response = await obsws.require(GetStreamingStatusRequest())
            state = g.get_state()
            if state[11] == 1:
                g.flip_lights()
                g.release_latches()
                await obsws.require(StartStopRecordingRequest())


loop = asyncio.get_event_loop()
try:
    asyncio.ensure_future(main())
    loop.run_forever()
except KeyboardInterrupt:
    pass
finally:
    loop.close()
