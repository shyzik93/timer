#include <EEPROM.h> 
#include <timer-api.h> 

/* relay */

const byte pin_relay = 0;

/* editing time */

byte edit_pos = 4;

const byte btn_pos_left = A2;
const byte btn_pos_right = A4;
const byte btn_pos_up = A5;
const byte btn_pos_down = A3;
const byte btn_pos_start = A0;

byte is_pressed = 0;
int pressed_btn = 0;

unsigned int counter = 0;

/* showing time on indicator */

byte ind_segments[] = {
  9, 5, 4, 12, 13, 8, 3 // A B C D E F G
};
byte ind_positions[] = {
  10, 7, 6, 2 // 1 2 3 4
};
const byte ind_point = 11;

unsigned int number_to_show = 100;
unsigned int _number_to_show = 100;
byte show_pos = 0;

void next_pos() {

  // turn position off

  digitalWrite(ind_positions[show_pos], LOW);
  show_pos += 1;
  if (show_pos == 4) show_pos = 0;

  // show digit

  int number;

  if (edit_pos == show_pos && counter < 50) {
    number = EEPROM[10];
  } else {
    //number = EEPROM[int((number_to_show/pow(10, 3-show_pos)))%10];
    number = EEPROM[get_value_pos(3-show_pos)];
  }
  counter += 1;
  if (counter == 200) counter = 0; 

  int i;
  for (i=0; i<7; i++) {
    digitalWrite(ind_segments[i], (number >> i) & 1);
  }

  // show point

  if (show_pos == 0) digitalWrite(ind_point, LOW);
  else digitalWrite(ind_point, HIGH);

  // turn position on
  
  digitalWrite(ind_positions[show_pos], HIGH);  

}

/* other */

int get_value_pos(byte pos) {
  return int(number_to_show/pow(10, pos))%10;
}

unsigned int get_pow(byte i) {
  unsigned int r = 1;
  for(;i>0;i--) r *= 10;
  return r;
}

void timer_handle_interrupts(int timer) {
  if (number_to_show > 0) number_to_show -= 1;
  else {
    digitalWrite(pin_relay, LOW);
    number_to_show = _number_to_show;
    timer_stop_ISR(TIMER_DEFAULT);
  }
  
}

void setup() {

  // from G to A
  EEPROM[0] = 0b1000000;
  EEPROM[1] = 0b1111001;
  EEPROM[2] = 0b0100100;
  EEPROM[3] = 0b0110000;
  EEPROM[4] = 0b0011001;
  EEPROM[5] = 0b0010010;
  EEPROM[6] = 0b0000010;
  EEPROM[7] = 0b1111000;
  EEPROM[8] = 0b0000000;
  EEPROM[9] = 0b0010000;
  EEPROM[10] = 0b1111111; // edit mode

  int i;
  for (i=0; i<7; i++) pinMode(ind_segments[i], OUTPUT);
  for (i=0; i<4; i++) pinMode(ind_positions[i], OUTPUT);
  pinMode(ind_point, OUTPUT);

  pinMode(btn_pos_left, INPUT);
  pinMode(btn_pos_right, INPUT);
  pinMode(btn_pos_up, INPUT);
  pinMode(btn_pos_down, INPUT);
  pinMode(btn_pos_start, INPUT);

  pinMode(pin_relay, OUTPUT);
  digitalWrite(pin_relay, LOW);

}

void loop() {
  // put your main code here, to run repeatedly:

  /*byte i;
  for (i=0;i<8;i++) {
    digitalWrite(PB1, 0);
    digitalWrite(PB4, 1);
    digitalWrite(PB5, 0);
  }*/

  delay(5);
  next_pos();


  if (is_pressed == 1) {
    if (digitalRead(pressed_btn) == LOW) is_pressed = 0;
  }

  if (is_pressed == 0) {
    if (digitalRead(btn_pos_left) == HIGH){
      if (edit_pos == 0) edit_pos = 5;
      edit_pos -= 1;
      is_pressed = 1;
      pressed_btn = btn_pos_left;
    } else if (digitalRead(btn_pos_right) == HIGH) {
      edit_pos += 1;
      if (edit_pos == 5) edit_pos = 0;
      is_pressed = 1;
      pressed_btn = btn_pos_right;
    } else if (digitalRead(btn_pos_up) == HIGH) {

      if (edit_pos >= 0 && edit_pos < 4) {
        byte v = get_value_pos(3-edit_pos);
        if (v == 9) number_to_show -= 9 * get_pow(3-edit_pos);
        else number_to_show += get_pow(3-edit_pos);
      }

      is_pressed = 1;
      pressed_btn = btn_pos_up;

    } else if (digitalRead(btn_pos_down) == HIGH) {

      if (edit_pos >= 0 && edit_pos < 4) {
        byte v = get_value_pos(3-edit_pos);
        if (v == 0) number_to_show += 9 * get_pow(3-edit_pos);
        else number_to_show -= get_pow(3-edit_pos);
      }

      is_pressed = 1;
      pressed_btn = btn_pos_down;

    } else if (digitalRead(btn_pos_start) == HIGH) {

        _number_to_show = number_to_show;

        digitalWrite(pin_relay, HIGH);
        timer_init_ISR_1KHz(TIMER_DEFAULT);
        /*for (unsigned int i=0; i<_number_to_show; i++) {
          delay(1);
          number_to_show -= 1;
          next_pos();
        }*/
        //delay(number_to_show);
        //digitalWrite(pin_relay, LOW);

        //number_to_show = _number_to_show;

        //delay(500);
      
        is_pressed = 1;
        pressed_btn = btn_pos_start;

    }

  }

}
