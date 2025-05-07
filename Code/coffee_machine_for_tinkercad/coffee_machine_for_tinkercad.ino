#include <LiquidCrystal.h>
//#include <Stepper.h>
//#include <Ultrasonic.h>
#include <Servo.h>

Servo myservo;
int servo_pin = 29;
//Ultrasonic ultrasonic(27, 28);
int pingPin_water = 3, pingPin_tank1 = A2, pingPin_tank2 = A1, pingPin_tank3 = 2;  //for ultrasonic 3 pin
const int stepsPerRevolution = 2038;
/*Stepper myStepper_sugar = Stepper(stepsPerRevolution, 14, 16, 15, 17);  //stepper
Stepper myStepper_coffee = Stepper(stepsPerRevolution, 18, 20, 19, 21);
Stepper myStepper_milk = Stepper(stepsPerRevolution, 22, 24, 23, 25);*/

const int rs = 9, en = 8, d4 = 13, d5 = 12, d6 = 11, d7 = 10;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int bt_1 = 5, bt_2 = 4, bt_3 = 7, bt_4 = 6;  //bt for buttons
int ir_s_cup = A3;                            // ir sensors
int tank_toboiler_pump = 26;
int tank_tocup_pump_cold = 27;
int tank_tocup_pump_hot = 32;
int boiler = 28;
int mixer_motor = 30;
int buzzer = A0;
int load_cell = A5;
int temp_sens = A4;

bool TEMP_SETUP = 0;

bool TEMP_CHOOSING_SCREEN = 0;
bool temp_start_choosing_milk_or_not = 0, temp_start_choosing_milk_or_not_2 = 0;
bool temp_start_choosing_hot_or_cold = 0, temp_start_choosing_hot_or_cold_2 = 0;
bool temp_start_choosing_sugar_level = 0, temp_start_choosing_sugar_level_2 = 0;
bool temp_start_choosing_water_level = 0, temp_start_choosing_water_level_2 = 0;
bool temp_start_choosing_coffe_level = 0, temp_start_choosing_coffe_level_2 = 0;
int milk_level = -1, milk_selected_value = -1, hot_cold_selected_value = -1, sugar_level_selection = -1, water_level_selection = -1, coffe_level_selection = -1;

bool TEMP_START_OPERATING = 0;

unsigned long delay_value_between_each_push = 200;  //ms

int L_sugar = 5, M_sugar = 10, H_sugar = 20;      //unit : g
int L_water = 100, M_water = 200, H_water = 300;  //unit : ml
int L_coffee = 4, M_coffee = 8, H_coffee = 16;    //unit : g
int coffee_tempreature_setpoint = 60;


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
void drop_the_required_ingredients(int& powder_level_selection, String name_of_powder, /* Stepper& stepper_name,*/ bool& end_curent_temp, bool& next_temp);

int temp = 0;  //just to fill some spaces on a functions
bool temp_bool = 0;
int limiting_repeating_same_screen_many_time =0 ; 
//for bluetooth connection:
char data_received = 0;
bool temp_changing_L_value = 0, temp_changing_M_value = 0, temp_changing_H_value = 0;
bool temp_reset_after_changing_values = 0;

void setup() {
  pinMode(bt_1, INPUT_PULLUP);
  pinMode(bt_2, INPUT_PULLUP);
  pinMode(bt_3, INPUT_PULLUP);
  pinMode(bt_4, INPUT_PULLUP);
  pinMode(ir_s_cup, INPUT_PULLUP);
  pinMode(tank_toboiler_pump, OUTPUT);
  pinMode(tank_tocup_pump_cold, OUTPUT);
  pinMode(tank_tocup_pump_hot, OUTPUT);
  pinMode(boiler, OUTPUT);
  myservo.write(0);  //initial postition of my servo
  pinMode(mixer_motor, OUTPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
  /*  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.noCursor();
  lcd.noBlink();*/
  lcd.setCursor(5, 0);
  lcd.print("hello!");
  delay(500);
  lcd.clear();
}
//  milk or no    /  hot or cold  /   sugar levle  /    water level /   coffee level
void loop() {
  bool button_1_value = digitalRead(bt_1), button_2_value = digitalRead(bt_2), button_3_value = digitalRead(bt_3), button_4_value = digitalRead(bt_4) ;
  bool there_is_a_cup_y_n = digitalRead(ir_s_cup);
  Serial.println (there_is_a_cup_y_n);
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
    lcd.print("coffe ");
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

    if (temp_check_sugar == 1) check_values_of_engrediens(pingPin_tank1, "sugar", temp_check_sugar, temp_check_if_theire_isa_cup, 0);  // i can let this be else if
    else if (temp_check_if_theire_isa_cup == 1) check_values_of_engrediens(ir_s_cup, "Cup", temp_check_if_theire_isa_cup, temp_check_cofee, 1);
    else if (temp_check_cofee == 1) check_values_of_engrediens(pingPin_tank2, "coffee", temp_check_cofee, temp_check_milk, 0);
    else if (milk_selected_value == 0 && temp_check_milk == 1) check_values_of_engrediens(pingPin_tank3, "milk", temp_check_milk, temp_check_water, 0);  // 0 mean yes i want milk
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
    if (temp_drop_sugar == 1) drop_the_required_ingredients(sugar_level_selection, "sugar", temp_drop_sugar, temp_drop_coffee);
    else if (temp_drop_coffee == 1) drop_the_required_ingredients(coffe_level_selection, "coffee", temp_drop_coffee, temp_drop_milk);
    // 0= want milk 1= cont want
    else if (milk_selected_value == 0 && temp_drop_milk == 1) {
      milk_level = water_level_selection / 15;  //15 can be cahnge, we need a linear relation between the water level and milk
      drop_the_required_ingredients(milk_level, "milk", temp_drop_milk, temp_drop_water);
    } else if (milk_selected_value == 1 && temp_drop_milk == 1) end_start_temp(temp_drop_milk, temp_drop_water);

    //hot water  =1 cold = 0
    else if (hot_cold_selected_value == 1 && temp_drop_water == 1) drop_the_required_water_ultrasonic(TEMP_DROPING_VALUES, temp_drop_water, temp_start_heating);
    else if (hot_cold_selected_value == 0 && temp_drop_water == 1) drop_the_required_water_load_cell(tank_tocup_pump_cold, TEMP_DROPING_VALUES, temp_drop_water, temp_start_heating);
  }

  //hot water  =1 cold = 0
  else if (temp_start_heating == 1 && hot_cold_selected_value == 1)
    start_heating(temp_start_heating, temp_drop_hotwater_to_cup);
  else if (temp_start_heating == 1 && hot_cold_selected_value == 0) end_start_temp(temp_start_heating, temp_start_mixing);

  else if (temp_drop_hotwater_to_cup == 1) drop_the_required_water_load_cell(tank_tocup_pump_hot, temp_bool, temp_drop_hotwater_to_cup, temp_start_mixing);

  else if (temp_start_mixing == 1) start_mixing(temp_start_mixing, temp_removing_cup);

  else if (temp_removing_cup == 1) /*// 0=no object  1=ther is a object*/ {
    Serial.println (there_is_a_cup_y_n);
    if (there_is_a_cup_y_n == 0) {
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
void check_values_of_engrediens(int& sensor_pin_number, String name_of_powder, bool& end_curent_temp, bool& next_temp, int ultra0_ir1) {
  if (ultra0_ir1 == 1) {
    int ir_object_check = digitalRead(sensor_pin_number);
    if (ir_object_check == 0) low_level_screen("cup", 1);  // 0=no object  1=ther is a object
    else {                                                
      end_start_temp(end_curent_temp, next_temp);
    }
  }

  else if (ultra0_ir1 == 0) {
    int ultrasinoc_low_level_check = calculating_distance(sensor_pin_number);//pingPin_water.read(); 
    if (ultrasinoc_low_level_check >= 10) low_level_screen(name_of_powder, 0);
    else {
      end_start_temp(end_curent_temp, next_temp);
    }
  }
}
void drop_the_required_ingredients(int& powder_level_selection, String name_of_powder, /* Stepper& stepper_name,*/ bool& end_curent_temp, bool& next_temp) {
  int weight = analogRead(load_cell);
  weight = map(weight, 0, 1023, 0, 400);
  float reference_mass_of_load_cell = weight /* = calculate_mass_of_the_cup()*/;
  float new_mass_of_load_cell = reference_mass_of_load_cell;
  //stepper_name.setSpeed(15);
  while (abs(reference_mass_of_load_cell - new_mass_of_load_cell) < powder_level_selection * 0.8) {
    int ir_sens = digitalRead(ir_s_cup);
    if (ir_sens == 0) emergency_alarm();
    else {
      if (limiting_repeating_same_screen_many_time == 1) {
        waiting_screen();
        limiting_repeating_same_screen_many_time = 0;
      }
      Serial.print("droping ");
      Serial.println(name_of_powder);
      //stepper_name.step(1);
      weight = analogRead(load_cell);
      weight = map(weight, 0, 1023, 0, 400);
      new_mass_of_load_cell = weight /* = calculate_mass_of_the_cup()*/;
      Serial.print("weight of the cup: ");
      Serial.println(weight);
    }
  }
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
  int hight = 70, long1 = 5, width = 4;
  //float distance_of_empty_Space = ultrasonic.read();                       //cm
  float distance_of_empty_Space = calculating_distance(pingPin_water);//pingPin_water.read(); 
  float water_volume = (hight - distance_of_empty_Space) * long1 * width;  //ml
  if (water_volume < 0) water_volume = 0;
  return water_volume;
}
void drop_the_required_water_ultrasonic(bool& end_overall_temp, bool& end_curent_temp, bool& next_temp) {
  float reference_volume = calculate_water_volume();
  float new_volume = reference_volume;
  while (abs(reference_volume - new_volume) < water_level_selection) {
    digitalWrite(tank_toboiler_pump, HIGH);
    new_volume = calculate_water_volume();
    Serial.println("droping COLD water to boiler ");
    Serial.println(new_volume);
  }
  digitalWrite(tank_toboiler_pump, LOW);
  end_start_temp(end_curent_temp, next_temp);
  end_overall_temp = 0;
}
void drop_the_required_water_load_cell(int& pump, bool& end_overall_temp, bool& end_curent_temp, bool& next_temp) {
  int weight = analogRead(load_cell);
  weight = map(weight, 0, 1023, 0, 400);
  float reference_mass_of_load_cell = weight /* = calculate_mass_of_the_cup()*/;
  float new_mass_of_load_cell = reference_mass_of_load_cell;
  while (abs(reference_mass_of_load_cell - new_mass_of_load_cell) < water_level_selection * 0.9) {
    bool ir_sens = digitalRead(ir_s_cup);
    if (ir_sens == 0) emergency_alarm();
    else {
      if (limiting_repeating_same_screen_many_time == 1) {
        waiting_screen();
        limiting_repeating_same_screen_many_time = 0;
      }
      Serial.println("droping water to cup");
      digitalWrite(pump, HIGH);
      weight = analogRead(load_cell);
      weight = map(weight, 0, 1023, 0, 400);
      new_mass_of_load_cell = weight /* = calculate_mass_of_the_cup()*/;
      Serial.print("weight of the cup: ");
      Serial.println(weight);
    }
  }
  digitalWrite(pump, LOW);
  end_overall_temp = 0;
  end_start_temp(end_curent_temp, next_temp);
}

int calculating_distance(int& pin_number) {
  long duration, cm;
  pinMode(pin_number, OUTPUT);
  digitalWrite(pin_number, LOW);
  delayMicroseconds(2);
  digitalWrite(pin_number, HIGH);
  delayMicroseconds(2);
  digitalWrite(pin_number, LOW);
  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(pin_number, INPUT);
  duration = pulseIn(pin_number, HIGH);
  cm = microsecondsToCentimeters(duration);
  cm = cm + 0;  // Ultrasonic Calibration factor
                /* Serial.print(cm);
  Serial.print("cm");
  Serial.println();*/
  delay(100);
  return cm;
}
long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}
long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
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
  int water_tempreature = analogRead(temp_sens);
  water_tempreature = map(water_tempreature, 0, 1023, 0, 90);
  Serial.println("start heating ");
  Serial.println(water_tempreature);
  if (water_tempreature < coffee_tempreature_setpoint) {
    digitalWrite(boiler, HIGH);
  } else {
    digitalWrite(boiler, LOW);
    end_start_temp(end_curent_temp, next_temp);
  }
}
void start_mixing(bool& end_curent_temp, bool& next_temp) {
  Serial.println("start mixing ");
  unsigned long delay_time = 0;
  if (hot_cold_selected_value == 1) delay_time = 5000;  //when its hot it will take 30 s
  else if (hot_cold_selected_value == 0) delay_time = 6000;
    //myservo.attach(29);
    myservo.write(60);                //servo on ;
    digitalWrite(mixer_motor, HIGH);  //dc on
    delay(delay_time);
    digitalWrite(mixer_motor, LOW);  //dc off
    myservo.write(0);                //servo back to initial position
    Done_screen();
    end_start_temp(end_curent_temp, next_temp);
}
void Done_screen() {
  //lcd print that cofffe is done
  lcd.clear();
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
  limiting_repeating_same_screen_many_time=1;
  for (int freq = startFreq; freq <= (startFreq * 2); ++freq) {
    tone(buzzer, freq, 10);
    delay(3);
  }
}
