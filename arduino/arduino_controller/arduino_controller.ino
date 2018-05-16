#include "CmdMessenger.h"
#include "Servo.h"
#include <FastLED.h>

#define FASTLED_ALLOW_INTERRUPTS 0
#define NUM_LEDS 130
#define DATA_PIN 30
CRGB leds[NUM_LEDS];
volatile static int stage;

#define SERVO_MAX_ANGLE 145
#define SERVO_MIN_ANGLE 30
Servo webcam_angle;       // Servo to adjust webcam angle

#define NUM_INPUT 3
enum inputs {
    blank0,              // Infrared sensor
    blank1,
    stream_button     // big green stream button
};
const int input_pins[NUM_INPUT] = {32, 34, 11};

#define NUM_OUTPUT 3
enum ouptuts {
    blank2,
    set_webcam_angle,
    blank3
};
const int output_pins[NUM_OUTPUT] = {43, 13, 47};

#define NUM_ANALOG 3
enum analogs {
    read_webcam_angle,
    read_pir,
    stream_button_light,  // output to control the stream button light
};
const int analog_pins[NUM_ANALOG] = {A1, A0, 12};

/*
#if defined(__AVR_ATmega328P__)
const int input_pins[NUM_INPUT] = {5, 6, 7};
const int output_pins[NUM_OUTPUT] = {2, 3, 4};
const int analog_pins[NUM_ANALOG] = {A0, A1, A2};
#endif

#if defined(__AVR_ATmega32U4__)
const int input_pins[NUM_INPUT] = {9, 10, 11};
const int output_pins[NUM_OUTPUT] = {5, 6, 7};
const int analog_pins[NUM_ANALOG] = {ADC0, ADC1, ADC2};
#endif
*/

/* firmware version */
const static int firmware_version[3] = {0, 1, 3};

/* system status messaging to stream PC */
#define STATUS_BITS 15
int status[STATUS_BITS];

/* 0 digital input 0
 * 1 digital input 1
 * 2 digital input 2
 * 
 * 3 digital output 0
 * 4 digital output 1
 * 5 digital output 2
 * 
 * 6 digital input pin latched 0
 * 7 digital input pin latched 1
 * 8 digital input pin latched 2
 * 
 *  9 digital input pin value 0
 * 10 digital input pin value 1
 * 11 digital input pin value 2
 * 
 * 12 digital input pin player 
 * 13 digital input pin stream status
 * 14 digital input pin stream toggle
 * 
 */


/* Define available CmdMessenger commands */
enum {
    ping,
    pong,
    req_firmware,
    send_firmware,
    player,
    lights,
    get_state,
    ret_state,
    release_latches,
    on_air,
    off_air,
    error
};

/* Timers - may move to hw timers but currently unecessary */
#define NUM_DELAYS 4
enum delays{
    servo_delay,
    player_activity,
    air_status,
    stream_light
};
static unsigned long timers[NUM_DELAYS]={0, 0, 0, 0};
const unsigned long waits[NUM_DELAYS]={50, 900000, 0, 0};  // 15m * 60s * 1000ms
//const unsigned long waits[NUM_DELAYS]={50, 15000};  // 15m * 60s * 1000ms


/* 
 * Button handling 
 * The buttons need debouncing, and values ma or may not need latchhing.  
 */
int pin_latch_value[NUM_INPUT];
int pin_latched[NUM_INPUT];
int pin_active[NUM_INPUT];
static int pin_state[NUM_INPUT];
static uint16_t y_old[NUM_INPUT];
static int flag[NUM_INPUT];

/* Initial Light state */
int light_state = LOW;

int pin = 0;            // pin interation

/* Initialize CmdMessenger -- this should match PyCmdMessenger instance */
const int BAUD_RATE = 9600;
CmdMessenger c = CmdMessenger(Serial,',',';','/');

/* Create callback functions to deal with incoming messages */

/* reply to ping with a pong */
void do_pong(void){
    c.sendCmd(pong, "pong");
}

/* send firmware version */
void do_send_firmware(void) {
    c.sendCmdStart(send_firmware);
    c.sendCmdBinArg(firmware_version[0]);  // major number
    c.sendCmdBinArg(firmware_version[1]);
    c.sendCmdBinArg(firmware_version[2]);
    c.sendCmdEnd();
}

/* State is defined as the currently values of:
 * {
 *   debounced pins,
 *   current pin values,
 *   pin lateched state,
 *   pin latched value
 * }
 * This lets the microcontroller do a standard send of information to the server, and the server can
 * decide which information is interesting.  
*/
void send_state(void){
    c.sendCmdStart(ret_state);

    for(pin=0; pin<STATUS_BITS; pin++) {
        c.sendCmdBinArg(status[pin]);
    }

    c.sendCmdEnd();
}

/* sends only value of pir input */
void is_player(void){
    c.sendBinCmd(player, (int)pin_state[read_pir]);
}

/* a 'pretty much a stub' for handling lights.  Once specs start rolling in, this will be fleshed out */
void handle_lights(void){
  
    if(status[13]==1) {
      // on air lights
      if(status[14] == 1) {
        go_on_air_lights();
      }
      // button light
      stream_button_on_air();
    } else {
      // off air lights
      if(status[14] == 1) {
        go_off_air_lights();
      }
      // button light
      stream_button_off_air();
    }
}

void on_unknown_command(void){
    c.sendCmd(error,"Command without callback.");
}

/* When the microcontroller ack's the state, it may (probably will) return a call to unlatch the pins to capture new
 * button presses.  This will need to change as button behavior changes.  only "latchable" pins will be "unlatched."
 * or more specifically longer presses for the servo motor cannot be handled via a latched momentary press.
 */
void unlatch_pins(){
    for(pin=0;pin<NUM_INPUT;pin++){
        pin_latched[pin]=0;
        pin_latch_value[pin]=0;
    }
}

/* Attach callbacks for CmdMessenger commands */
void attach_callbacks(void) {
    c.attach(ping, do_pong);
    c.attach(req_firmware, do_send_firmware);
    c.attach(player, is_player);
    c.attach(get_state, send_state);
    c.attach(on_unknown_command);
    c.attach(on_air, go_on_air);
    c.attach(off_air, go_off_air);
    c.attach(release_latches, unlatch_pins);
}

void go_on_air() {
  stage = 0;
  status[13] = 1;
  status[14] = 1;  // trigger lights
}

void go_off_air() {
  stage = 0;
  status[13] = 0;
  status[14] = 1;  // trigger lights
}

/*
 * Button management.  All the reading, debouncings, latchings happens here.
 * the debounce is handled in two stages, a simulated RC filter and a schmitt trigger.
 */
void read_btns(void) {
    long temp=0;
    int state_change = 0;

    for(pin=0; pin<NUM_INPUT; pin++){
        y_old[pin] = y_old[pin] - (y_old[pin] >> 2);
        
        if(digitalRead(input_pins[pin])==1){
          y_old[pin] = y_old[pin] + 0x3F;
        }
                
        if((y_old[pin] > 0xF0)&&(flag[pin]==1)) {
            flag[pin]=0;
            pin_state[pin]=1;
            state_change=1;
        }
        if((y_old[pin] < 0x0F)&&(flag[pin]==0)) {
            flag[pin]=1;
            pin_state[pin]=0;
            state_change=1;
        }
        // handle this better for active high/low switches.
        if((state_change==1)&&(pin_latched[pin]==0)&&pin_state[pin]==1) {
            pin_latch_value[pin]=1;
            pin_latched[pin]=1;
        }
        state_change=0;
    }
}

/* 
 * Adjust servo height.  Input us an analog signal between ground and VCC+, constrained
 * by the values which make the camera visible.  Easiest adjustor is a slider/knob/rocker potentiometer, 
 * hower may move to buttons (momentary: short adjustment, long press: continuous adjustment.
 */
void adjust_webcam_angle() {
    int val;
    val = analogRead(analog_pins[read_webcam_angle]);
    val = map(val, 0, 1024, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
    webcam_angle.write(val);
    timers[servo_delay]=millis();
}

/* 
 * Input handling.  Anything the arduino received gets processed here.
 */
void handle_input() {

    int idx = 0;

    for(pin=0; pin<NUM_INPUT; pin++) {
        status[pin]=pin_state[pin];
    }
    idx += NUM_INPUT;

    for(pin=0; pin<NUM_OUTPUT; pin++) {
        status[idx + pin]=digitalRead(output_pins[pin]);
    }
    idx += NUM_OUTPUT;

    for(pin=0; pin<NUM_INPUT; pin++) {
//        status[idx + pin]=pin_latched[pin];
    }
    //test
    status[6] = millis()-timers[2];
    status[7] = timers[2];
    status[8] = stage;
    
    idx += NUM_INPUT;    

    for(pin=0; pin<NUM_INPUT; pin++) {
        status[idx + pin]=pin_latch_value[pin];
    }
}

/*
 * Process wait based execution here
 */
void keep_time() {

    if (millis() - timers[servo_delay] > waits[servo_delay]) {
      // timer reset in adjust_webcam_angle IF servo is moved
        adjust_webcam_angle();
    }
    
    if(analogRead(analog_pins[read_pir]) > 500) {
      status[12] = 1; // indicate a player is at the machine
      timers[player_activity] = millis();
    }
    
    if ((millis() - timers[player_activity]) > waits[player_activity]) {
        status[12] = 0;  // timeout indication ot stream PC
        // need to send to rpi also, tbd till interface defined
    }
}

void setup() {
    Serial.begin(BAUD_RATE);
    FastLED.addLeds<WS2812, DATA_PIN>(leds, NUM_LEDS); // GRB

    attach_callbacks();
    stage = 0;
    
    for(pin=0; pin<NUM_INPUT; pin++) {
        pin_latch_value[pin] = 0;   // Latched value set to 0, if not latched, this doesn't matter and is just an init
        pin_latched[pin] = 0;       // initially, no pins are latched.
        pin_active[pin] = 1;        // This is hard coded where it may be needed per pin in the future.
        pin_state[pin] = 0;         // This is just an initial value, doesn't matter.
        y_old[pin]=0;
        flag[pin]=0;
    }

    for(pin=0; pin<NUM_INPUT; pin++) {
        pinMode(input_pins[pin], INPUT_PULLUP);
    }

    for(pin=0; pin<NUM_ANALOG; pin++) {
        pinMode(analog_pins[pin], INPUT_PULLUP);
    }

    for(pin=0; pin<NUM_OUTPUT; pin++) {
        pinMode(output_pins[pin], OUTPUT);
        digitalWrite(output_pins[pin], HIGH);
    }

    webcam_angle.attach(output_pins[set_webcam_angle]);
    timers[servo_delay] = millis();
    timers[player_activity] = millis();
}

void loop() {

    c.feedinSerialData();
    read_btns();
    handle_input();
    handle_lights();
    keep_time();
//    FastLED.show();
}

