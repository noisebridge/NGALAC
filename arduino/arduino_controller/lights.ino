
void go_on_air_lights() {
  int i = 0;
  int red = 240;  
  static int j = 0; 

  if (stage == 0) {
    // Off
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 0, 0);
    }
    timers[2] = millis();
    stage++;
  }

  if (stage == 1 && (millis() - timers[2]) > 200) {

    // on low for 10s
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 30, 0);
    }
    timers[2] = millis();
    stage++;
  }

  if (stage == 2 && (millis() - timers[2]) > 300) {
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 0, 0);
    }
    timers[2] = millis();
    stage++;
  }

  if (stage == 3 && (millis() - timers[2]) > 700) {
    timers[2] = millis();
    stage++;
  }

  if ((stage == 4 || stage == 6) && (millis() - timers[2]) > 100) {
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 50, 0);
    }
    timers[2] = millis();
    stage++;
  }

  if ((stage == 5 || stage == 7) && (millis() - timers[2]) > 100) {

    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 0, 0);
    }
    timers[2] = millis();
    stage++;
  }

  if (stage == 8  && (millis() - timers[2]) > 500) {
    timers[2] = millis();
    stage++;
  }

  if ((stage >= 9 && stage <= 73) && (millis() - timers[2]) > 70) {
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, j * 3, 0);
    }
    timers[2] = millis();
    stage++;
    j++;
  }

  // set toggle off
  if (stage > 73 ) {
    status[14] = 0;
    stage = 0;
  }
  // this will leave the lights fully on
}

void go_off_air_lights() {
  int i = 0;
  int red = 240;  
  static int j = 0; 
  
  if (stage == 0) {
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 250, 0);
    }
    timers[2] = millis();
    stage++;
  }
  
  if (stage == 1 && (millis()-timers[2]) > 250) {
    // Off
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 0, 0);
    }
    timers[2] = millis();
    stage++;
  }

  if (stage == 2 && (millis() - timers[2]) > 75) {

    // on low for 10s
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 180, 0);
    }
    timers[2] = millis();
    stage++;
  }

  if (stage == 3 && (millis() - timers[2]) > 75) {
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 0, 0);
    }
    timers[2] = millis();
    stage++;
  }

  if (stage == 4 && (millis() - timers[2]) > 75) {
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 210, 0);
    }    
    timers[2] = millis();
    stage++;
  }
  
  if (stage == 5 && (millis() - timers[2]) > 350) {
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 0, 0);
    }    
    timers[2] = millis();
    stage++;
  }

  if (stage == 6 && (millis() - timers[2]) > 75) {
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 255, 0);
    }    
    timers[2] = millis();
    stage++;
  }
  
  if (stage == 7 && (millis() - timers[2]) > 75) {
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 0, 0);
    }    
    timers[2] = millis();
    stage++;
  } // leave then off

  // set toggle off
  if(stage > 7) {
    status[14] = 0;
    stage = 0;
  }
}

void stream_button_on_air() {
  
  static int pulse_dir;
  static int pulse_amt = 5;
  static int pulse;
    
  if (pulse_amt >= 250) {
    pulse_dir = -1;
  }

  if (pulse_amt <= 10) {
    pulse_dir = 1;
  }

  if (millis() - timers[3] > 50) {
    pulse = analogRead(analog_pins[stream_button_light]);
    pulse += (pulse_dir * pulse_amt);
    analogWrite(analog_pins[stream_button_light], pulse);
    timers[3] = millis();
  }

}

void stream_button_off_air() {
  int val = analogRead(analog_pins[stream_button_light]);

  if(val > 300) {
    val = 0;
  } else {
    val = 255;
  }
  
  if (timers[3] > 330) {
    analogWrite(analog_pins[stream_button_light], val);
    timers[3] = millis();
  }

}

void controller_boxes() {
  
}


