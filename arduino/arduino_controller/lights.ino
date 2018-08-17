
void go_on_air_lights() {
  int i = 0;
  int red = 240;  
  static int j = 0; 

  if (stage == 0) {
    // Off
    for (i = 0; i < ONAIR_NUM; i++) {
      onair_leds[i] = CRGB(0, 0, 0);
    }
    timers[2] = millis();
    stage++;
  }

  if (stage == 1 && (millis() - timers[2]) > 200) {

    // on low for 10s
    for (i = 0; i < ONAIR_NUM; i++) {
      onair_leds[i] = CRGB(0, 30, 0);
    }
    timers[2] = millis();
    stage++;
  }

  if (stage == 2 && (millis() - timers[2]) > 300) {
    for (i = 0; i < ONAIR_NUM; i++) {
      onair_leds[i] = CRGB(0, 0, 0);
    }
    timers[2] = millis();
    stage++;
  }

  if (stage == 3 && (millis() - timers[2]) > 700) {
    timers[2] = millis();
    stage++;
  }

  if ((stage == 4 || stage == 6) && (millis() - timers[2]) > 100) {
    for (i = 0; i < ONAIR_NUM; i++) {
      onair_leds[i] = CRGB(0, 50, 0);
    }
    timers[2] = millis();
    stage++;
  }

  if ((stage == 5 || stage == 7) && (millis() - timers[2]) > 100) {

    for (i = 0; i < ONAIR_NUM; i++) {
      onair_leds[i] = CRGB(0, 0, 0);
    }
    timers[2] = millis();
    stage++;
  }

  if (stage == 8  && (millis() - timers[2]) > 500) {
    timers[2] = millis();
    stage++;
  }

  if ((stage >= 9 && stage <= 73) && (millis() - timers[2]) > 70) {
    for (i = 0; i < ONAIR_NUM; i++) {
      onair_leds[i] = CRGB(0, j * 3, 0);
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
    for (i = 0; i < ONAIR_NUM; i++) {
      onair_leds[i] = CRGB(0, 250, 0);
    }
    timers[2] = millis();
    stage++;
  }
  
  if (stage == 1 && (millis()-timers[2]) > 250) {
    // Off
    for (i = 0; i < ONAIR_NUM; i++) {
      onair_leds[i] = CRGB(0, 0, 0);
    }
    timers[2] = millis();
    stage++;
  }

  if (stage == 2 && (millis() - timers[2]) > 75) {

    // on low for 10s
    for (i = 0; i < ONAIR_NUM; i++) {
      onair_leds[i] = CRGB(0, 180, 0);
    }
    timers[2] = millis();
    stage++;
  }

  if (stage == 3 && (millis() - timers[2]) > 75) {
    for (i = 0; i < ONAIR_NUM; i++) {
      onair_leds[i] = CRGB(0, 0, 0);
    }
    timers[2] = millis();
    stage++;
  }

  if (stage == 4 && (millis() - timers[2]) > 75) {
    for (i = 0; i < ONAIR_NUM; i++) {
      onair_leds[i] = CRGB(0, 210, 0);
    }    
    timers[2] = millis();
    stage++;
  }
  
  if (stage == 5 && (millis() - timers[2]) > 350) {
    for (i = 0; i < ONAIR_NUM; i++) {
      onair_leds[i] = CRGB(0, 0, 0);
    }    
    timers[2] = millis();
    stage++;
  }

  if (stage == 6 && (millis() - timers[2]) > 75) {
    for (i = 0; i < ONAIR_NUM; i++) {
      onair_leds[i] = CRGB(0, 255, 0);
    }    
    timers[2] = millis();
    stage++;
  }
  
  if (stage == 7 && (millis() - timers[2]) > 75) {
    for (i = 0; i < ONAIR_NUM; i++) {
      onair_leds[i] = CRGB(0, 0, 0);
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
  static int pulse = 0;
    
  if (pulse >= 250) {
    pulse_dir = -1;
  }

  if (pulse <= 10) {
    pulse_dir = 1;
  }

  if (millis() - timers[3] > 50) {
    // status[7] = pulse;
    pulse += (pulse_dir * pulse_amt);
    analogWrite(analog_pins[stream_button_light], pulse);
    timers[3] = millis();
  }

}

void stream_button_off_air() {
  static int val = 255;

  if(val >= 255) {
    val = 0;
  } else {
    val = 255;
  }
  
  if ((millis() - timers[3]) > 500) {
    analogWrite(analog_pins[stream_button_light], val);
    timers[3] = millis();
  }

}

// void fadeall(int leds_num, CRGB leds) {
//  for(int i = 0; i < leds_num; i++) {
//    leds[i].nscale8(250);
//  }
//}

void test_patterns() {
  
  static int fade_wait=0;
  static int hue = 0;
  static int val = 255;
  int sat = 255;

  if((fade_wait % 10) == 0) {
    for(int i = 0; i<NGALAC_NUM;i++) {
      ngalac_leds[i] = CHSV(hue++, sat, val);
    }
    
    for(int i = 0; i<CONT_BOX_A_NUM;i++) {
      controller_boxA[i] = CHSV(hue++, sat, val);
      controller_boxB[i] = CHSV(hue++, sat, val);
    }
    fade_wait = 0;
//    val = -1*val;
  }
  fade_wait++;
  
  
}
