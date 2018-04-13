#include "CmdMessenger.h"

/* Define available CmdMessenger commands */
enum {
    ping,
    pong,
    player,
    lights,
    get_state,
    ret_state,
    release_latches,
    error
};

unsigned long debounce[3] = {0, 0, 0};
uint8_t pin_state[3] = {0, 0, 0};
uint8_t pin_latched[3] = {0, 0, 0};
uint8_t pin_latch_value[3] = {0, 0, 0};
unsigned long bounce_delay = 75;

int input_pins[3] = {11, 12, 13};
int pressure_btn = 0;
int stream_btn = 1;
int pin = 0;
int output_pins[3] = {5, 6, 7};
int light_state = LOW;

/* Initialize CmdMessenger -- this should match PyCmdMessenger instance */
const int BAUD_RATE = 9600;
CmdMessenger c = CmdMessenger(Serial,',',';','/');

/* Create callback functions to deal with incoming messages */

void do_pong(void){
    c.sendCmd(pong, "pong");
}

void is_player(void){
    c.sendBinCmd(player, (int)pin_state[pressure_btn]);
}

void blink_lights(void){
    light_state = ~light_state;

    for(pin=0; pin<3; pin++) {
        digitalWrite(output_pins[pin], light_state);
    }
    c.sendCmd(lights, "ok");
}

void send_state(void){
    c.sendCmdStart(ret_state);
    for(pin=0; pin<3; pin++) {
        c.sendCmdBinArg(digitalRead(pin_state[pin]));
    }
    for(pin=0; pin<3; pin++) {
        c.sendCmdBinArg(digitalRead(output_pins[pin]));
    }
    for(pin=0; pin<3; pin++) {
        c.sendCmdBinArg(digitalRead(pin_latched[pin]));
    }
    for(pin=0; pin<3; pin++) {
        c.sendCmdBinArg(digitalRead(pin_latch_value[pin]));
    }
    c.sendCmdEnd();
}

void on_unknown_command(void){
    c.sendCmd(error,"Command without callback.");
}

void unlatch_pins(){
  for(pin=0;pin<3;pin++){
    pin_latched[pin]=0;
    pin_latch_value[pin]=0;
  }
    c.sendCmd(lights, "pins unlatched");
}

/* Attach callbacks for CmdMessenger commands */
void attach_callbacks(void) {
    c.attach(ping, do_pong);
    c.attach(player, is_player);
    c.attach(lights, blink_lights);
    c.attach(get_state, send_state);
    c.attach(on_unknown_command);
    c.attach(release_latches, unlatch_pins);
}

void read_btns(void) {
  static uint8_t y_old[3]={0,0,0}, flag[3]={0,0,0};
  uint8_t temp[3];

  for(pin=0;pin<3;pin++){
    temp[pin] = (y_old[pin] >> 2);
    y_old[pin] = y_old[pin] - temp[pin];

    if(digitalRead(input_pins[pin])){y_old[pin] = y_old[pin] + 0x3F;}

    if((y_old[pin] > 0xF0)&&(flag[pin]==0)){
      flag[pin]=1; 
      pin_state[pin]=1;
      if (pin_latched[pin]==0){
        pin_latch_value[pin]=1;
        pin_latched[pin]=1;
      }
    }
    if((y_old[pin] < 0x0F)&&(flag[pin]==1)){
      flag[pin]=0; 
      pin_state[pin]=0;
      }
   }
}


void setup() {
    Serial.begin(BAUD_RATE);
    attach_callbacks();
    for(pin=0; pin<3; pin++) {
        pinMode(input_pins[pin], INPUT);
        pinMode(output_pins[pin], OUTPUT);
        digitalWrite(output_pins[pin], LOW);
    }
}

void loop() {
    c.feedinSerialData();
    read_btns();
}

