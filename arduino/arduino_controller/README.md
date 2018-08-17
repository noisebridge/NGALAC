# Arduino Controller
This software runs on the Arduino Mega 2560 in the NGALAC. It's responsible for a few things:
1. Controlling the lights, including stage left/right (i.e. controller storage), the On-Air sign, and the NGALAC sign.
2. Detecting when the stream button is pressed and notifying the streaming PC.
3. Moving the player camera when the knob is turned.
4. Detecting when the player leaves the console based on the PIR sensor.

## Webcam servo control using PWM
The Arduino uses a library called FastLED to control the LED strips. FastLED requires precise timing and so it disables interrupts when it is updating the LEDs. The Arduino Servo library uses interrupts to control servos, so FastLED interferes with the Arduino Servo library, and we can't use it. See [this article](https://github.com/FastLED/FastLED/wiki/Interrupt-problems) for more details.

To work around this, we use PWM on pin 2 to control the servo. PWM is unaffected by whether interrupts are disabled. This has a side-effect of interfering with `analogWrite()` functionality on pins 3 and 5. So if you notice some issue with `analogWrite` on those pins, this is probably why.

I was unable to find a library to configure the PWM pins, so I directly modified some registers on the Arduino to do what I wanted.

### Low-level details
[See this Wikipedia article for an explanation of the control signal that the servo expects.](https://en.wikipedia.org/wiki/Servo_control)

We use timer3 (a 16-bit timer) in Fast PWM mode. We set it in PWM mode 14 (`WGM33:WGM30 == 0b1110`), which means the timer will count up to ICR3 and then reset to 0.

We set the prescaler to `0b010`, which means `clk / 8`, which means 2 MHz (since the Arduino is 16 MHz). We set ICR3 to 40000. This means that the timer will wrap around every `1 sec / 2e6 * 4e4 == 20 msec`, or 50 Hz, which is the period that a servo expects.

We set OCR3B to some value between 2000 and 4000, depending on the position that we want to set the servo. This means that the pulse will vary from 1ms to 2ms, which is what the servo expects.

### Using a different pin
If, for some reason, you want to put the servo on some pin other than 2, here are some pointers:

* Only pins 2, 3, 5, 6, 7, 8, 11, and 12 will work. Pins 4, 9, 10, and 13 are controlled by 8-bit timers, which are too coarse-grained (the servo will only be able to move to 10 or so positions). Pins 8, 11, and 12 aren't wired up on the shield currently, so those are also probably not good. The rest of the pins won't work because they're not PWM.
* Once you've picked which pin you want, you will need to figure out which timer and output compare register it corresponds to. [This post](https://forum.arduino.cc/index.php?topic=72092.0) discusses the pin -> timer mapping. [This page](http://astro.neutral.org/arduino/arduino-pwm-pins-frequency.shtml) gives the pin -> output compare register mapping.
* Update the source code and replace references to TCCR3A, TCCR3B, ICR3, and OCR3B to the appropriate registers.
* If you want to add another servo for some reason, I'd recommend putting it on pins 3 or 5, because TCCR3A, TCCR3B, and ICR3 are already set up for you—you just need to set OCR3A (pin 5) or OCR3C (pin 3) appropriately.

