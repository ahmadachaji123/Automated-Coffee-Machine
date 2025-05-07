#include <LiquidCrystal_I2C.h>
#include <Stepper.h>
#include <Ultrasonic.h>
#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "HX711.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);  // A5 scl     A4 sda

Servo servoBase;
Servo servoarm;
int servo_Base_pin = 7, servo_Arm_pin = 8;
int initial_value_arm = 172, initial_value_Base = 172;
unsigned long delay_time_each_step_arm = 1000;

int mixer = 9, mixer_0v = 10;
unsigned long mixing_time = 0;
int mixer_speed_PMW_0_255 = 30;
int j = 0;
int min_val_ARM = 0;
int min_val_BASE = 0;

const int LOADCELL_DOUT_PIN = 30;
const int LOADCELL_SCK_PIN = 28;
HX711 scale;


const int oneWireBus = A0;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

Ultrasonic UR_cofee(A6, A7);  //trig echo
Ultrasonic UR_sugar(38, 40);
Ultrasonic UR_water(24, 26);
Ultrasonic UR_milk(42, 44);

const int stepsPerRevolution = 2038;
Stepper myStepper_milk = Stepper(stepsPerRevolution, 41, 45, 43, 47);
int DCmotor_Sugar = 11, DCmotor_coffee = 49, DCmotor_coffee_0V = 12;

int bt_1 = 3, bt_2 = 2, bt_3 = 5, bt_4 = 6;
int ir_s_cup = 22;
int tank_toboiler_pump = A2;
int tank_tocup_pump_cold = A1;
int boiler_tocup_pump_hot = A3;
int boiler = 46;
int buzzer = 13;
int load_cell = A0;
int tempreature_sens = A1;
int LED1 = 50, LED2 = 53, LED3 = 52, LED4 = 51, LED5 = 48;

unsigned long delay_value_between_each_push = 500;  //ms

int L_sugar = 0, M_sugar = 5, H_sugar = 10;       //unit : g
int L_water = 100, M_water = 150, H_water = 200;  //unit : ml
int L_coffee = 3, M_coffee = 6, H_coffee = 9;     //unit : g
int coffee_tempreature_setpoint = 50;
bool TEMP_SETUP = 0;

bool TEMP_CHOOSING_SCREEN = 0;
bool temp_start_choosing_milk_or_not = 0, temp_start_choosing_milk_or_not_2 = 0;
bool temp_start_choosing_hot_or_cold = 0, temp_start_choosing_hot_or_cold_2 = 0;
bool temp_start_choosing_sugar_level = 0, temp_start_choosing_sugar_level_2 = 0;
bool temp_start_choosing_water_level = 0, temp_start_choosing_water_level_2 = 0;
bool temp_start_choosing_coffe_level = 0, temp_start_choosing_coffe_level_2 = 0;
int milk_level = -1, milk_selected_value = -1, hot_cold_selected_value = -1, sugar_level_selection = -1, water_level_selection = -1, coffe_level_selection = -1;

bool TEMP_START_OPERATING = 0;

bool TEMP_CHECKING_VALUES = 0;
bool temp_check_sugar = 0, temp_check_if_theire_isa_cup = 0, temp_check_cofee = 0, temp_check_milk = 0, temp_check_water = 0;

bool TEMP_START_OPERATING_BUTTON = 0;
bool temp_stop_bt_4_back_button = 0;

bool TEMP_DROPING_VALUES = 0;
bool temp_drop_sugar = 0, temp_drop_coffee = 0, temp_drop_milk = 0, temp_drop_water = 0;

bool temp_start_heating = 0;
bool temp_drop_hotwater_to_cup = 0;
bool temp_start_mixing = 0;
bool temp_removing_cup = 0;
void drop_the_required_ingredients(int& powder_level_selection, String name_of_powder, Stepper& stepper_name, bool& end_curent_temp, bool& next_temp);

int temp = 0;  //just to fill some spaces on a functions
bool temp_bool = 0;
bool limiting_repeating_same_screen_many_time = 0;
//for bluetooth connection:
char data_received = 0;
bool temp_changing_L_value = 0, temp_changing_M_value = 0, temp_changing_H_value = 0;
bool temp_reset_after_changing_values = 0;

void setup() {

  sensors.begin();

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(773.500);
  scale.tare();

  pinMode(bt_1, INPUT_PULLUP);
  pinMode(bt_2, INPUT_PULLUP);
  pinMode(bt_3, INPUT_PULLUP);
  pinMode(bt_4, INPUT_PULLUP);
  pinMode(ir_s_cup, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);

  pinMode(tank_toboiler_pump, OUTPUT);  //relay initially HIGH=OFF
  digitalWrite(tank_toboiler_pump, !LOW);
  pinMode(tank_tocup_pump_cold, OUTPUT);
  digitalWrite(tank_tocup_pump_cold, !LOW);
  pinMode(boiler_tocup_pump_hot, OUTPUT);  //to start with all the relays close
  digitalWrite(boiler_tocup_pump_hot, !LOW);
  pinMode(boiler, OUTPUT);
  digitalWrite(boiler, !LOW);

  servoBase.attach(servo_Base_pin);
  servoarm.attach(servo_Arm_pin);
  servoarm.write(initial_value_arm);  // initial position
  servoBase.write(initial_value_Base);

  pinMode(mixer, OUTPUT);  // connected to motor drive
  pinMode(mixer_0v, OUTPUT);
  digitalWrite(mixer_0v, LOW);  // a ground to our motor

  pinMode(DCmotor_coffee, OUTPUT);  // connected to motor drive
  pinMode(DCmotor_coffee_0V, OUTPUT);
  digitalWrite(DCmotor_coffee_0V, LOW);  // a ground to our motor

  pinMode(DCmotor_Sugar, OUTPUT);    // connected to Relay
  digitalWrite(DCmotor_Sugar, LOW);  // this relay work initialy low

  Serial.begin(9600);
  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.noCursor();
  lcd.noBlink();
  lcd.setCursor(5, 0);
  lcd.print("hello!");
  delay(500);
  lcd.clear();
}

void loop() {
  bool button_1_value = digitalRead(bt_1), button_2_value = digitalRead(bt_2), button_3_value = digitalRead(bt_3), button_4_value = digitalRead(bt_4);
 
   
  if ((Serial.available() > 0) && temp_stop_bt_4_back_button == 0) {  //the user can change the values of the ingredients if he is not in the operating screen
    recieving_data('L_sugar_availebel', 'M_sugar_availebel', 'H_sugar_availebel', L_sugar, M_sugar, H_sugar);
    recieving_data('L_coffee_availebel', 'M_coffee_availebel', 'H_coffee_availebel', L_coffee, M_coffee, H_coffee);
    recieving_data('L_water_availebel', 'M_water_availebel', 'H_water_availebel', L_water, M_water, H_water);
    recieving_data('coffee_tempreature_availebell', 'nothing', 'nothing', coffee_tempreature_setpoint, temp, temp);
  }

  if ((button_4_value == 0 || temp_reset_after_changing_values == 1) && temp_stop_bt_4_back_button == 0) {  // return to home main screen
    temp_reset_after_changing_values = 0;
    TEMP_SETUP = 0;
    TEMP_CHECKING_VALUES = 0;
    temp_start_choosing_milk_or_not = 0;
    temp_start_choosing_milk_or_not_2 = 0;
    temp_start_choosing_hot_or_cold = 0;
    temp_start_choosing_hot_or_cold_2 = 0;
    temp_start_choosing_sugar_level = 0;
    temp_start_choosing_sugar_level_2 = 0;
    temp_start_choosing_water_level = 0;
    temp_start_choosing_water_level_2 = 0;
    temp_start_choosing_coffe_level = 0;
    temp_start_choosing_coffe_level_2 = 0;
    milk_selected_value = -1;
    hot_cold_selected_value = -1;
    sugar_level_selection = -1;
    water_level_selection = -1;
    coffe_level_selection = -1;
    TEMP_CHECKING_VALUES = 0;
    temp_check_sugar = 0;
    temp_check_cofee = 0;
    temp_check_milk = 0;
    temp_check_water = 0;
    TEMP_START_OPERATING_BUTTON = 0;
    TEMP_START_OPERATING = 0;
    lcd.clear();
  }

  if (TEMP_SETUP == 0) {  // main screen
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("1-Make your ");
    lcd.setCursor(5, 1);
    lcd.print("coffee ");
    TEMP_SETUP = 1;
    temp_start_choosing_milk_or_not = 1;
    TEMP_CHOOSING_SCREEN = 1;
    delay(delay_value_between_each_push);
  }

  else if (TEMP_CHOOSING_SCREEN == 1) {  // choosing the values of your coffevoid choosing_value_screen(String first_line,String second_line,  int& L_level,int& M_level,int& H_level, int& end_curent_temp, int& next_temp, int 2or3_value_screen, int g0_ml1)

    if (temp_start_choosing_milk_or_not == 1 && button_1_value == 0) choosing_value_screen("Add milk?", "1-Yes  2-No", temp, temp, temp, temp_start_choosing_milk_or_not, temp_start_choosing_milk_or_not_2, 0, 0);
    else if (temp_start_choosing_milk_or_not_2 == 1) return_2option_value(milk_selected_value, temp_start_choosing_milk_or_not_2, temp_start_choosing_hot_or_cold);

    else if (temp_start_choosing_hot_or_cold == 1) choosing_value_screen("Cold or Hot?", "1-Cold  2-Hot", temp, temp, temp, temp_start_choosing_hot_or_cold, temp_start_choosing_hot_or_cold_2, 0, 0);
    else if (temp_start_choosing_hot_or_cold_2 == 1) return_2option_value(hot_cold_selected_value, temp_start_choosing_hot_or_cold_2, temp_start_choosing_sugar_level);

    else if (temp_start_choosing_sugar_level == 1) choosing_value_screen("Sugar:", "1tsp  2tsp  3tsp", L_sugar, M_sugar, H_sugar, temp_start_choosing_sugar_level, temp_start_choosing_sugar_level_2, 1, 0);
    else if (temp_start_choosing_sugar_level_2 == 1) return_3option_value(sugar_level_selection, temp_start_choosing_sugar_level_2, temp_start_choosing_water_level, L_sugar, M_sugar, H_sugar, temp_bool, temp_bool);

    else if (temp_start_choosing_water_level == 1) choosing_value_screen("water:", "1-L  2-M  3-H ", L_water, M_water, H_water, temp_start_choosing_water_level, temp_start_choosing_water_level_2, 1, 1);
    else if (temp_start_choosing_water_level_2 == 1) return_3option_value(water_level_selection, temp_start_choosing_water_level_2, temp_start_choosing_coffe_level, L_water, M_water, H_water, temp_bool, temp_bool);

    else if (temp_start_choosing_coffe_level == 1) choosing_value_screen("Coffe:", "1-L  2-M  3-H ", L_coffee, M_coffee, H_coffee, temp_start_choosing_coffe_level, temp_start_choosing_coffe_level_2, 1, 0);
    else if (temp_start_choosing_coffe_level_2 == 1) return_3option_value(coffe_level_selection, temp_start_choosing_coffe_level_2, temp_check_sugar, L_coffee, M_coffee, H_coffee, TEMP_CHOOSING_SCREEN, TEMP_CHECKING_VALUES);
  }

  else if (TEMP_CHECKING_VALUES == 1) {  // check if we have some low value of the engredients

    if (temp_check_sugar == 1) check_values_of_engrediens_UR(UR_sugar, "sugar", temp_check_sugar, temp_check_if_theire_isa_cup);  // i can let this be else if
    else if (temp_check_if_theire_isa_cup == 1) check_values_of_engrediens_IR(ir_s_cup, "Cup", temp_check_if_theire_isa_cup, temp_check_cofee);
    else if (temp_check_cofee == 1) check_values_of_engrediens_UR(UR_cofee, "coffee", temp_check_cofee, temp_check_milk);
    else if (milk_selected_value == 0 && temp_check_milk == 1) check_values_of_engrediens_UR(UR_milk, "milk", temp_check_milk, temp_check_water);  // 0 mean yes i want milk
    else if (milk_selected_value == 1 && temp_check_milk == 1) {
      temp_check_water = 1;  //1 mean i dont want milk
      temp_check_milk = 0;
    } else if (temp_check_water == 1) check_value_of_water(temp_check_water, TEMP_START_OPERATING_BUTTON, TEMP_CHECKING_VALUES);
  }

  else if (TEMP_START_OPERATING_BUTTON == 1) {  // pressing 1 to start operating screen
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("1-start the ");
    lcd.setCursor(4, 1);
    lcd.print("operation");
    end_start_temp(TEMP_START_OPERATING_BUTTON, TEMP_START_OPERATING);
  }

  else if (TEMP_START_OPERATING == 1 && button_1_value == 0) {  // stop the back button and start operating
    waiting_screen();
    TEMP_START_OPERATING = 0;
    temp_drop_sugar = 1;
    temp_stop_bt_4_back_button = 1;
    TEMP_DROPING_VALUES = 1;
    Serial.println("_____________________________________________________________________________ ");
    Serial.print("milk selected value: ");
    Serial.println(milk_selected_value);
    Serial.print("hot cold: ");
    Serial.println(hot_cold_selected_value);
    Serial.print("sugar selected value: ");
    Serial.println(sugar_level_selection);
    Serial.print("water selected value: ");
    Serial.println(water_level_selection);
    Serial.print("coffee selected value: ");
    Serial.println(coffe_level_selection);
    Serial.println("_____________________________________________________________________________ ");
  }

  else if (TEMP_DROPING_VALUES == 1) {
    if (temp_drop_sugar == 1) drop_the_required_ingredients_DC_motor(sugar_level_selection, "sugar", DCmotor_Sugar, temp_drop_sugar, temp_drop_coffee);
    else if (temp_drop_coffee == 1) drop_the_required_ingredients_DC_motor(coffe_level_selection, "coffee", DCmotor_coffee, temp_drop_coffee, temp_drop_milk);
    // 0= want milk 1= cont want
    else if (milk_selected_value == 0 && temp_drop_milk == 1) {
      milk_level = water_level_selection / 15;  //15 can be cahnge, we need a linear relation between the water level and milk
      drop_the_required_ingredients_stepper(milk_level, "milk", myStepper_milk, temp_drop_milk, temp_drop_water);
    } else if (milk_selected_value == 1 && temp_drop_milk == 1) end_start_temp(temp_drop_milk, temp_drop_water);

    //hot water  =1 cold = 0
    else if (hot_cold_selected_value == 1 && temp_drop_water == 1) drop_the_required_water_ultrasonic(TEMP_DROPING_VALUES, temp_drop_water, temp_start_heating);
    else if (hot_cold_selected_value == 0 && temp_drop_water == 1) drop_the_required_water_load_cell(tank_tocup_pump_cold, TEMP_DROPING_VALUES, temp_drop_water, temp_start_heating);
  }

  //hot water  =1 cold = 0
  else if (temp_start_heating == 1 && hot_cold_selected_value == 1)
    start_heating(temp_start_heating, temp_drop_hotwater_to_cup);
  else if (temp_start_heating == 1 && hot_cold_selected_value == 0) end_start_temp(temp_start_heating, temp_start_mixing);

  else if (temp_drop_hotwater_to_cup == 1) drop_the_required_water_load_cell(boiler_tocup_pump_hot, temp_bool, temp_drop_hotwater_to_cup, temp_start_mixing);

  else if (temp_start_mixing == 1) start_mixing(temp_start_mixing, temp_removing_cup);

  else if (temp_removing_cup == 1) /*// 1=no object  0=ther is a object*/ {
      digitalWrite(LED3,LOW);
    bool there_is_a_cup_y_n = digitalRead(ir_s_cup);
    if (there_is_a_cup_y_n == 1) {
      temp_removing_cup = 0;
      TEMP_SETUP = 0;                  //back to main screen
      temp_stop_bt_4_back_button = 0;  // we can use back button again
    }
  }
}

void choosing_value_screen(String first_line, String second_line, int& L_level, int& M_level, int& H_level, bool& end_curent_temp, bool& next_temp, int value_screen_option, int g0_ml1) {
  String unit = "";
  if (g0_ml1 == 0) unit = "g";
  else if (g0_ml1 == 1) unit = "ml";

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(first_line);

  if (value_screen_option == 0) {
    lcd.setCursor(0, 1);
    lcd.print(second_line);
  } else if (value_screen_option == 1) {
    lcd.print("(");
    lcd.print(unit);
    lcd.print(")");
    lcd.setCursor(0, 1);
    lcd.print(L_level);

    lcd.setCursor(6, 1);
    lcd.print(M_level);

    lcd.setCursor(12, 1);
    lcd.print(H_level);
  }
  end_start_temp(end_curent_temp, next_temp);
  delay(delay_value_between_each_push);
}
void return_2option_value(int& value, bool& end_curent_temp, bool& next_temp) {
  int button_1_value = digitalRead(bt_1), button_2_value = digitalRead(bt_2), button_3_value = digitalRead(bt_3), button_4_value = digitalRead(bt_4);
  if (button_1_value == 0) {
    value = 0;
    end_start_temp(end_curent_temp, next_temp);
  } else if (button_2_value == 0) {
    value = 1;
    end_start_temp(end_curent_temp, next_temp);
  }
}
void return_3option_value(int& value, bool& end_curent_temp, bool& next_temp, int& Low_value, int& Medium_value, int& Max_value, bool& end_overall_temp, bool& start_next_overall_temp) {
  int button_1_value = digitalRead(bt_1), button_2_value = digitalRead(bt_2), button_3_value = digitalRead(bt_3), button_4_value = digitalRead(bt_4);
  if (button_1_value == 0) {
    value = Low_value;
    end_start_temp(end_curent_temp, next_temp);
    end_start_temp(end_overall_temp, start_next_overall_temp);
  } else if (button_2_value == 0) {
    value = Medium_value;
    end_start_temp(end_curent_temp, next_temp);
    end_start_temp(end_overall_temp, start_next_overall_temp);
  } else if (button_3_value == 0) {
    value = Max_value;
    end_start_temp(end_curent_temp, next_temp);
    end_start_temp(end_overall_temp, start_next_overall_temp);
  }
}
void waiting_screen() {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("your coffee");
  lcd.setCursor(1, 1);
  lcd.print("is being made");
}
void low_level_screen(String tank_type, int different_error) {
  lcd.clear();
  if (different_error == 0) {
    lcd.setCursor(3, 0);
    lcd.print("LOW ");
    lcd.print(tank_type);
    lcd.setCursor(1, 1);
    lcd.print("please refill ");
    delay(200);

  } else if (different_error == 1) {
    lcd.setCursor(6, 0);
    lcd.print(tank_type);
    lcd.setCursor(2, 1);
    lcd.print("not detected");
    delay(200);
  }
}
void check_values_of_engrediens_IR(int& sensor_pin_number, String name_of_powder, bool& end_curent_temp, bool& next_temp) {
  bool ir_object_check = digitalRead(sensor_pin_number);
  if (ir_object_check == 1) low_level_screen("cup", 1);  //*// 1=no object  0=ther is a object*/
  else end_start_temp(end_curent_temp, next_temp);
}
void check_values_of_engrediens_UR(Ultrasonic& ultrasonic, String name_of_powder, bool& end_curent_temp, bool& next_temp) {
  int ultrasinoc_low_level_check = ultrasonic.read();
  
  if (ultrasinoc_low_level_check == 5) low_level_screen(name_of_powder, 0);
  else end_start_temp(end_curent_temp, next_temp);
}
void ultrasoni_led(Ultrasonic& ultrasonic, int& LED){
 int ultrasinoc_low_level_check_sugar = ultrasonic.read();
  if (ultrasinoc_low_level_check_sugar == 5) digitalWrite(LED,HIGH);
  else digitalWrite(LED,LOW);

}
void drop_the_required_ingredients_stepper(int& powder_level_selection, String name_of_powder, Stepper& stepper_name, bool& end_curent_temp, bool& next_temp) {
  scale.tare();  // reset the scale to 0
  float weight = scale.get_units();
  stepper_name.setSpeed(15);
  while (weight < (powder_level_selection * 0.8)) {
    bool ir_sens = digitalRead(ir_s_cup); /* 1=no object  0=ther is a object*/
    if (ir_sens == 1) emergency_alarm();
    else {
      if (limiting_repeating_same_screen_many_time == 1) {
        waiting_screen();
        limiting_repeating_same_screen_many_time = 0;
      }
      Serial.print("droping ");
      Serial.println(name_of_powder);
      weight = scale.get_units();
      Serial.print("weight of the cup: ");
      Serial.println(weight);
      stepper_name.step(-1);
    }
  }
  end_start_temp(end_curent_temp, next_temp);
}
void drop_the_required_ingredients_DC_motor(int& powder_level_selection, String name_of_powder, int Motor, bool& end_curent_temp, bool& next_temp) {
  scale.tare();  // reset the scale to 0
  float weight = scale.get_units();

  while (weight < powder_level_selection * 0.8) {
    bool ir_sens = digitalRead(ir_s_cup); /* 1=no object  0=ther is a object*/
    if (ir_sens == 1) emergency_alarm();
    else {
      if (limiting_repeating_same_screen_many_time == 1) {
        waiting_screen();
        limiting_repeating_same_screen_many_time = 0;
      }
      Serial.print("droping ");
      Serial.println(name_of_powder);
      weight = scale.get_units();
      Serial.print("weight of the cup: ");
      Serial.println(weight);
      digitalWrite(Motor, HIGH);
    }
  }
  digitalWrite(Motor, LOW);
  end_start_temp(end_curent_temp, next_temp);
}
void check_value_of_water(bool& end_curent_temp, bool& next_temp, bool& end_overall_temp) {
  float water_volume = calculate_water_volume();
  if (water_volume <= (water_level_selection * 1.2)) low_level_screen("Water", 0);  //water level : high medium low
  else {
    end_start_temp(end_curent_temp, next_temp);
    end_overall_temp = 0;
  }
}
float calculate_water_volume() {
  int hight = 17, long1 = 12, width = 8;
  float distance_of_empty_Space = UR_water.read();
  if (distance_of_empty_Space > 17) distance_of_empty_Space = 17;
  else if (distance_of_empty_Space > 30) distance_of_empty_Space = 0;
  float water_volume = (hight - distance_of_empty_Space) * long1 * width;  //ml
  if (water_volume < 0) water_volume = 0;
  return water_volume;
}
void drop_the_required_water_ultrasonic(bool& end_overall_temp, bool& end_curent_temp, bool& next_temp) {
  float reference_volume = calculate_water_volume();
  float new_volume = reference_volume;
  while (abs(reference_volume - new_volume) < water_level_selection * 0.8) {
    digitalWrite(tank_toboiler_pump, !HIGH);
    new_volume = calculate_water_volume();
    Serial.println("droping COLD water to boiler ");
    Serial.println(new_volume);
  }
  digitalWrite(tank_toboiler_pump, !LOW);
  end_start_temp(end_curent_temp, next_temp);
  end_overall_temp = 0;
}
void drop_the_required_water_load_cell(int& pump, bool& end_overall_temp, bool& end_curent_temp, bool& next_temp) {
  scale.tare();  // reset the scale to 0
  float weight = scale.get_units();

  while (weight < water_level_selection * 0.9) {  // 0.9 that to reduce error of falling time
    bool ir_sens = digitalRead(ir_s_cup);         /* 1=no object  0=ther is a object*/
    if (ir_sens == 1) emergency_alarm();
    else {
      if (limiting_repeating_same_screen_many_time == 1) {
        waiting_screen();
        limiting_repeating_same_screen_many_time = 0;
      }
      Serial.println("droping water to cup");
      digitalWrite(pump, !HIGH);
      Serial.print("weight of the cup: ");
      Serial.println(weight);
      weight = scale.get_units();
    }
  }
  digitalWrite(pump, !LOW);
  end_overall_temp = 0;
  end_start_temp(end_curent_temp, next_temp);
}
void recieving_data(char L_availebel_char, char M_availebel_char, char H_availebel_char, int& L_tank_type, int& M_tank_type, int& H_tank_type) {
  data_received = Serial.read();
  if (data_received == L_availebel_char) temp_changing_L_value = 1;
  else if (data_received == M_availebel_char) temp_changing_M_value = 1;
  else if (data_received == H_availebel_char) temp_changing_H_value = 1;
  else if (data_received == 'reset_values') {
    L_sugar = 5;
    M_sugar = 10;
    H_sugar = 20;
    L_water = 100;
    M_water = 200;
    H_water = 300;
    L_coffee = 4;
    M_coffee = 8;
    H_coffee = 16;
    coffee_tempreature_setpoint = 60;
  } else if (data_received == 'back') {
    temp_changing_L_value = 0;
    temp_changing_M_value = 0;
    temp_changing_H_value = 0;
  } else if (temp_changing_L_value == 1) converting_and_saving_values(data_received, L_tank_type, temp_changing_L_value);
  else if (temp_changing_M_value == 1) converting_and_saving_values(data_received, M_tank_type, temp_changing_M_value);
  else if (temp_changing_H_value == 1) converting_and_saving_values(data_received, H_tank_type, temp_changing_H_value);
}
void converting_and_saving_values(char& data_received, int& igredient_new_value, bool& end_current_temp) {
  int convert_to_integer = data_received - '0';
  igredient_new_value = convert_to_integer;
  end_current_temp = 0;
  //send a message to confirm that data recieved sucsecfully
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Change Done");
  temp_reset_after_changing_values = 1;
}
void end_start_temp(bool& end_curent_temp, bool& next_temp) {
  end_curent_temp = 0;
  next_temp = 1;
}
void start_heating(bool& end_curent_temp, bool& next_temp) {
  sensors.requestTemperatures();
  float water_tempreature = sensors.getTempCByIndex(0);
  Serial.println("start heating ");
  Serial.println(water_tempreature);
  if (water_tempreature < coffee_tempreature_setpoint) {
    digitalWrite(boiler, !HIGH);
  } else {
    digitalWrite(boiler, !LOW);
    end_start_temp(end_curent_temp, next_temp);
  }
}
void start_mixing(bool& end_curent_temp, bool& next_temp) {
  Serial.println("start mixing ");
  if (hot_cold_selected_value == 1) mixing_time = 1000;       //when its hot it will take 30 s
  else if (hot_cold_selected_value == 0) mixing_time = 1000;  //hot water  =1 cold = 0
  active_arm();
  Done_screen();
  end_start_temp(end_curent_temp, next_temp);
}
void Done_screen() {
  //lcd print that cofffe is done
  lcd.clear();
  digitalWrite(LED3,HIGH);
  lcd.setCursor(0, 0);
  lcd.print("<Your coffee is>");
  lcd.setCursor(0, 1);
  lcd.print(">>>>>READY!<<<<<");
  for (int i = 0; i < 3; i++) {
    tone(buzzer, 380, 250);
    delay(300);
    tone(buzzer, 400, 250);
    delay(300);
    noTone(buzzer);  // Stop sound...
    delay(800);
  }
}
void emergency_alarm() {
  const int startFreq = 523;  // C5 is 523Hz
  low_level_screen("cup", 1);
  limiting_repeating_same_screen_many_time = 1;
  for (int freq = startFreq; freq <= (startFreq * 2); ++freq) {
    tone(buzzer, freq, 10);
    delay(3);
  }
}
void active_arm() {
  if (j == 0) repeat_itera(3, 22, 10);
  else if (j == 1) repeat_itera(1, -10, 0);
  else if (j == 2) repeat_itera(2, 0, 7);
  else if (j == 3) repeat_itera(1, 5, 0);
  else if (j == 4) repeat_itera(2, 0, 10);
  else if (j == 5) repeat_itera(1, 15, 15);
  else if (j == 6) {
    analogWrite(mixer, mixer_speed_PMW_0_255);
    delay(mixing_time);
    analogWrite(mixer, 0);
    j = j + 1;
  } else if (j == 7) repeat_itera(1, -5, 0);
  else if (j == 8) repeat_itera(1, -0, -16);
  else if (j == 9) repeat_itera(4, 15, -20);
  else if (j == 10) {  // back to initial position
    min_val_ARM = 0;
    min_val_BASE = 0;
    go_to_pos(min_val_ARM, min_val_BASE);
    delay(delay_time_each_step_arm);
    j = 0;
  }
}
void go_to_pos(int minus_arm, int minus_Base) {
  servoarm.write(initial_value_arm - minus_arm);
  servoBase.write(initial_value_Base - minus_Base);
}
void repeat_itera(int NB_iterration, int change_ARM, int change_Base) {
  for (int i = 0; i < NB_iterration; i++) {
    min_val_ARM = change_ARM + min_val_ARM;
    min_val_BASE = change_Base + min_val_BASE;
    go_to_pos(min_val_ARM, min_val_BASE);
    delay(delay_time_each_step_arm);
  }
  j = j + 1;
}