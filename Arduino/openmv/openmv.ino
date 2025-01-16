#include <Wire.h>
#include <Adafruit_SH1106_STM32.h>
Adafruit_SH1106 display(-1);
TwoWire newWire(2, I2C_FAST_MODE);
#define Wire newWire
int16_t ball_x = 0;
int16_t ball_y = 0;
int16_t yellow_x = 0;
int16_t yellow_y = 0;
int16_t blue_x = 0;
int16_t blue_y = 0;
const int slaveAddress = 0x42;
int ball_angle, direction_ball, distance_ball;
int yellow_angle, direction_yellow, distance_yellow;
int blue_angle, direction_blue, distance_blue;
bool is_ball = false;
bool is_yellow = false;
bool is_blue = false;
#define robot_x 152
#define robot_y 94
#define v 30000
int buff[8];
int counter, GY;

void setup() {
  pinMode(PC13, OUTPUT);
  pinMode(PB12, OUTPUT);
  pinMode(PB13, OUTPUT);
  pinMode(PB14, OUTPUT);
  pinMode(PB15, OUTPUT);
  pinMode(PA8, PWM);
  pinMode(PB8, PWM);
  pinMode(PB7, PWM);
  pinMode(PB6, PWM);
  pinMode(PC14, OUTPUT);  //////SPIN
  pinMode(PC15, OUTPUT);  //////SHOOT
  motor(0, 0, 0, 0);
  Serial1.begin(115200);

  delay(500);
  Wire.begin();
  delay(500);
  display.begin(0x2, 0x3C);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  // -------------------- Loading
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print("Wellcome");
  display.display();
  delay(500);
  display.clearDisplay();
  display.setTextSize(1);
  // -------------------- Serial Settings for GY-25
  Serial1.write(0xA5);
  Serial1.write(0x54); // set 0
  delay(500);
  Serial1.write(0xA5);
  Serial1.write(0x51); // poll
}

void loop() {
  read_MV();
  read_GY();
  

  if(is_ball) {
    if (ball_angle > 350 || ball_angle < 10) moveAngle(ball_angle);
    else if(ball_angle < 180) moveAngle(ball_angle + 40 );
    else if(ball_angle >=180) moveAngle(ball_angle - 40 );
  }
  else        motor(0,0,0,0);

  display_all();
}
void read_MV(){
  Wire.requestFrom(slaveAddress, 12);  // Request 12 bytes (6 values * 2 bytes each)
  if (Wire.available() == 12) {
    uint8_t buffer[12];
    for (int i = 0; i < 12; i++) {
      buffer[i] = Wire.read();  // Read each byte into buffer
    }

    // Unpack the data
    ball_x = (int16_t)(buffer[0] | (buffer[1] << 8));
    ball_y = (int16_t)(buffer[2] | (buffer[3] << 8));
    yellow_x = (int16_t)(buffer[4] | (buffer[5] << 8));
    yellow_y = (int16_t)(buffer[6] | (buffer[7] << 8));
    blue_x = (int16_t)(buffer[8] | (buffer[9] << 8));
    blue_y = (int16_t)(buffer[10] | (buffer[11] << 8));

    ball_angle = get_angle(ball_x, ball_y);
    // direction_ball = get_direction(ball_angle);
    distance_ball = sqrt(pow(ball_x - robot_x, 2) + pow(ball_y - robot_y, 2));
    yellow_angle = get_angle(yellow_x, yellow_y);
    // direction_yellow = get_direction(yellow_angle);
    distance_yellow = sqrt(pow(yellow_x - robot_x, 2) + pow(yellow_y - robot_y, 2));
    blue_angle = get_angle(blue_x, blue_y);
    // direction_blue = get_direction(blue_angle);
    distance_blue = sqrt(pow(blue_x - robot_x, 2) + pow(blue_y - robot_y, 2));
    if (ball_x == 0 && ball_y == 0) is_ball = false;
    else  is_ball = true;
    if (yellow_x == 0 && yellow_y == 0) is_yellow = false;
    else  is_yellow = true;
    if (blue_x == 0 && blue_y == 0) is_blue = false;
    else  is_blue = true;
  }
}
void read_GY(){
  // -------------------- GY-25 Read Data
  Serial1.write(0xA5);
  Serial1.write(0x51);
  while (true) {
    buff[counter] = Serial1.read();
    if (counter == 0 && buff[0] != 0xAA) break;
    counter++;
    if (counter == 8) {
      counter = 0;
      if (buff[0] == 0xAA && buff[7] == 0x55) {
        GY = (int16_t)(buff[1] << 8 | buff[2]) / 100;
        if(GY > 180) GY -= 360;
        if(GY <-180) GY += 360;
        
      }
    }
  }
}
void display_all(){
  display.clearDisplay();
  display.drawCircle(64, 32, 5, WHITE);
  display.drawLine(
    64 + sin(GY * PI / 180) * 4, 
    32 - cos(GY * PI / 180) * 4, 
    64 - sin(GY * PI / 180) * 4, 
    32 + cos(GY * PI / 180) * 4, 
    WHITE);
  if (is_yellow)
    display.drawLine(
        64 + sin((yellow_angle - 10) * PI / 180) * 15, 
        32 - cos((yellow_angle - 10) * PI / 180) * 15, 
        64 + sin((yellow_angle + 10) * PI / 180) * 15, 
        32 - cos((yellow_angle + 10) * PI / 180) * 15, 
      WHITE);
  if (is_blue)
    display.drawLine(
        64 + sin((blue_angle - 10) * PI / 180) * 16, 
        32 - cos((blue_angle - 10) * PI / 180) * 16, 
        64 + sin((blue_angle + 10) * PI / 180) * 16, 
        32 - cos((blue_angle + 10) * PI / 180) * 16, 
      WHITE);
  if (is_ball)
    display.fillCircle(
      64 + sin(ball_angle * PI / 180) * 10, 
      32 - cos(ball_angle * PI / 180) * 10, 
      2, WHITE);

  // display.setCursor(0, 20);
  // display.print("a:");
  // display.println(ball_angle);

  display.setCursor(0, 20);
  display.print("ball_x:");
  display.println(ball_x);
  display.print("ball_y:");
  display.println(ball_y);
  // display.print("yellow_x:");
  // display.println(yellow_x);
  // display.print("yellow_y:");
  // display.println(yellow_y);
  // display.print("blue_x:");
  // display.println(blue_x);
  // display.print("blue_y:");
  // display.println(blue_y);
  display.display();
}
int get_angle(int x, int y) {
  int angle = atan2(x - robot_x, y - robot_y) * 180 / PI;
  if (angle < 0) angle += 360;
  return angle;
}
int get_direction(int angle) {
  int direction;
  for (int i = 0; i < 16; i++) {
    if ((angle - 11.25 >= i * 22.5) && (angle - 11.25 < (i + 1) * 22.5)) direction = i + 1;
  }
  if (angle <= 11.25 || angle >= 348.5) direction = 0;
  return direction;
}
void motor(int ML1, int ML2, int MR2, int MR1) {
  // if (GY > 6 && GY <= 30) GY = 30;
  // else if (GY > 30 && GY <= 80) GY = 80;
  // else if (GY > 80 && GY <= 180) GY = 150;
  // else if (GY < -6 && GY >= -30) GY = -30;
  // else if (GY < -30 && GY >= -80) GY = -80;
  // else if (GY < -80 && GY >= -180) GY = -150;
  // ML1 += GY;
  // ML2 += GY;
  // MR2 += GY;
  // MR1 += GY;
  // ML1 *= 255;
  // ML2 *= 255;
  // MR2 *= 255;
  // MR1 *= 255;
  if (ML1 > 65535) ML1 = 65535;
  if (ML2 > 65535) ML2 = 65535;
  if (MR2 > 65535) MR2 = 65535;
  if (MR1 > 65535) MR1 = 65535;
  if (ML1 < -65535) ML1 = -65535;
  if (ML2 < -65535) ML2 = -65535;
  if (MR2 < -65535) MR2 = -65535;
  if (MR1 < -65535) MR1 = -65535;
  if (ML1 > 0) {
    digitalWrite(PB15, 0);
    pwmWrite(PA8, ML1);
  } else {
    digitalWrite(PB15, 1);
    pwmWrite(PA8, ML1 + 65535);
  }
  if (ML2 > 0) {
    digitalWrite(PB14, 0);
    pwmWrite(PB8, ML2);
  } else {
    digitalWrite(PB14, 1);
    pwmWrite(PB8, ML2 + 65535);
  }
  if (MR2 > 0) {
    digitalWrite(PB13, 0);
    pwmWrite(PB7, MR2);
  } else {
    digitalWrite(PB13, 1);
    pwmWrite(PB7, MR2 + 65535);
  }
  if (MR1 > 0) {
    digitalWrite(PB12, 0);
    pwmWrite(PB6, MR1);
  } else {
    digitalWrite(PB12, 1);
    pwmWrite(PB6, MR1 + 65535);
  }
}
void moveAngle(int a){
	if(a>360)     a-=360;
	if(a<0)       a+=360;
	int x = -v * cos(a * M_PI / 180);
	int y = -v * sin(a * M_PI / 180);
	motor((x + y) , (x - y) , (- x - y), (y - x));
}