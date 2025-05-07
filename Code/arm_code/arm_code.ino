#include <Servo.h>

Servo servoBase;  // create servo object to control a servo

void setup() {
  servoBase.attach(32);
  servoBase.write(30);  // initial position
}
void loop() {
  servoBase.write(30);  // initial position
}
  /*
void active_arm() {
int j = 0;
  int i = 0;
  int min_val_ARM = 0;
  int min_val_BASE = 0;
  if (j == 0) {
    for (i = 0; i < 3; i++) {
      min_val_ARM = 22 + min_val_ARM;
      min_val_BASE = 10 + min_val_BASE;
      go_to_pos(min_val_ARM, min_val_BASE);
      delay(delay_time_each_step_arm);
    }
    j = 1;
  }
  if (j == 1) {
    for (i = 0; i < 1; i++) {
      min_val_ARM = -10 + min_val_ARM;
      min_val_BASE = 0 + min_val_BASE;
      go_to_pos(min_val_ARM, min_val_BASE);
      delay(delay_time_each_step_arm);
    }
    j = 2;
  }
  if (j == 2) {
    for (i = 0; i < 2; i++) {
      min_val_ARM = 0 + min_val_ARM;
      min_val_BASE = 7 + min_val_BASE;
      go_to_pos(min_val_ARM, min_val_BASE);
      delay(delay_time_each_step_arm);
    }
    j = 3;
  }
  if (j == 3) {
    for (i = 0; i < 1; i++) {
      min_val_ARM = 5 + min_val_ARM;
      min_val_BASE = 0 + min_val_BASE;
      go_to_pos(min_val_ARM, min_val_BASE);
      delay(delay_time_each_step_arm);
    }

    j = 4;
  }

  if (j == 4) {
    for (i = 0; i < 2; i++) {
      min_val_ARM = 0 + min_val_ARM;
      min_val_BASE = 10 + min_val_BASE;
      go_to_pos(min_val_ARM, min_val_BASE);
      delay(delay_time_each_step_arm);
    }

    j = 5;
  }
  if (j == 5) {
    for (i = 0; i < 1; i++) {
      min_val_ARM = 15 + min_val_ARM;
      min_val_BASE = 15 + min_val_BASE;
      go_to_pos(min_val_ARM, min_val_BASE);
      delay(delay_time_each_step_arm);
    }

    j = 6;
  }


  if (j == 6) {
    analogWrite(mixer, 30);
    delay(4000);
    analogWrite(mixer, 0);
    j = 7;
  }



  if (j == 7) {
    for (i = 0; i < 1; i++) {
      min_val_ARM = -5 + min_val_ARM;
      min_val_BASE = 0 + min_val_BASE;
      go_to_pos(min_val_ARM, min_val_BASE);
      delay(delay_time_each_step_arm);
    }

    j = 8;
  }
  if (j == 8) {
    for (i = 0; i < 1; i++) {
      min_val_ARM = -0 + min_val_ARM;
      min_val_BASE = -16 + min_val_BASE;
      go_to_pos(min_val_ARM, min_val_BASE);
      delay(delay_time_each_step_arm);
    }
    j = 9;
  }
  if (j == 9) {
    for (i = 0; i < 4; i++) {
      min_val_ARM = -15 + min_val_ARM;
      min_val_BASE = -20 + min_val_BASE;
      go_to_pos(min_val_ARM, min_val_BASE);
      delay(delay_time_each_step_arm);
    }
    j = 10;
  }
  if (j == 10) {
    for (i = 0; i < 1; i++) {
      min_val_ARM = 0;
      min_val_BASE = 0;
      go_to_pos(min_val_ARM, min_val_BASE);
      delay(delay_time_each_step_arm);
    }

    j = 11;
  }
}*/
/*void go_to_pos(int minus_arm, int minus_Base) {
  servoarm.write(initial_value_arm - minus_arm);     // initial position
  servoBase.write(initial_value_Base - minus_Base);  // initial position
}*/