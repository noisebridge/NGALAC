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

uint8_t pin_latch_value[3] = {0, 0, 0};
uint8_t pin_latched[3] = {1, 1, 1};
uint8_t pin_active[3] = {1, 1, 1};
uint8_t pin_state[3] = {1, 1, 1};

int input_pins[3] = {9, 10, 11};
int output_pins[3] = {5, 6, 7};

int light_state = LOW;
int pressure_btn = 0;
int stream_btn = 1;
int pin = 0;

/* Initialize CmdMessenger -- this should match PyCmdMessenger instance */
const int BAUD_RATE = 9600;
CmdMessenger c = CmdMessenger(Serial,',',';','/');

/* Create callback functions to deal with incoming messages */

void do_pong(void){
    c.sendCmd(pong, "pong");
}

void send_state(void){
    c.sendCmdStart(ret_state);
    for(pin=0; pin<3; pin++) {
        c.sendCmdBinArg((int)pin_state[pin]);
    }
    for(pin=0; pin<3; pin++) {
        c.sendCmdBinArg((int)digitalRead(output_pins[pin]));
    }
    for(pin=0; pin<3; pin++) {
        c.sendCmdBinArg((int)pin_latched[pin]);
    }
    for(pin=0; pin<3; pin++) {
        c.sendCmdBinArg((int)pin_latch_value[pin]);
    }
    c.sendCmdEnd();
}

void is_player(void){
    c.sendBinCmd(player, (int)pin_state[pressure_btn]);
}

void lights_handler(void){
    int param1 = c.readBinArg<bool>();
    // int param2 = c.readBinArg<int>();

    if(param1 == 0) {
        light_state = LOW;
    } else {
        light_state = HIGH;
    }

    for(pin=0; pin<3; pin++) {
        digitalWrite(output_pins[pin], light_state);
    }
    send_state();
}

void on_unknown_command(void){
    c.sendCmd(error,"Command without callback.");
}

void unlatch_pins(){
    for(pin=0;pin<3;pin++){
        pin_latched[pin]=0;
        pin_latch_value[pin]=0;
    }
    send_state();
}

/* Attach callbacks for CmdMessenger commands */
void attach_callbacks(void) {
    c.attach(ping, do_pong);
    c.attach(player, is_player);
    c.attach(lights, lights_handler);
    c.attach(get_state, send_state);
    c.attach(on_unknown_command);
    c.attach(release_latches, unlatch_pins);
}

void read_btns(void) {
    static uint8_t y_old[3]={0,0,0};
    int state_change = 0;
    uint8_t temp[3];

    for(pin=0;pin<3;pin++){
        y_old[pin] = y_old[pin] - (y_old[pin] >> 2);

        if(digitalRead(input_pins[pin])){y_old[pin] = y_old[pin] + 0x3F;}

        if((y_old[pin] > 0xF0)&&(pin_state[pin]==0)) {
            pin_state[pin]=1;
            state_change=1; 
        }
        if((y_old[pin] < 0x0F)&&(pin_state[pin]==1)) {
            pin_state[pin]=0;
            state_change=1;
        }
        // handle this better for active high/low switches.
        if((state_change==1)&&(pin_latched[pin]==0)&&pin_state[pin]==pin_active[pin]) {
            pin_latch_value[pin]=pin_active[pin];
            pin_latched[pin]=1;
        }
        state_change=0;
    }
}


void setup() {
    Serial.begin(BAUD_RATE);
    attach_callbacks();
    for(pin=0; pin<3; pin++) {
        pinMode(input_pins[pin], INPUT_PULLUP);
        pinMode(output_pins[pin], OUTPUT);
        digitalWrite(output_pins[pin], LOW);
    }
}

void loop() {
    c.feedinSerialData();
    read_btns();
}

