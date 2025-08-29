#include "rcj_robot.h"
#include <Wire.h>
#include <Adafruit_SH1106_STM32.h>

TwoWire newWire(2, I2C_FAST_MODE);
#define Wire newWire

void RCJ_Robot::init()
{
  pinMode(PC13, OUTPUT);
  pinMode(L1_Pin, OUTPUT);
  pinMode(L2_Pin, OUTPUT);
  pinMode(R2_Pin, OUTPUT);
  pinMode(R1_Pin, OUTPUT);
  pinMode(L1_PWM, PWM);
  pinMode(L2_PWM, PWM);
  pinMode(R2_PWM, PWM);
  pinMode(R1_PWM, PWM);
  pinMode(BOOST_CTRL_Pin, PWM);
  pinMode(SPIN_Pin, OUTPUT);   //////SPIN
  pinMode(SHOOT_Pin, OUTPUT);  //////SHOOT
  pinMode(Buzzer_Pin, OUTPUT); 
  pinMode(SW1_Pin, INPUT);
  pinMode(SW2_Pin, INPUT);
  motor(0, 0, 0, 0);
  setBoostPWM(0);
  if (digitalRead(ID_Select_Pin))
    robot_id = 1;
  else
    robot_id = 2;
  if (robot_id == 1)
  {
    robot_x = robot1_x;
    robot_y = robot1_y;
  }
  else
  {
    robot_x = robot2_x;
    robot_y = robot2_y;
  }
  display.begin(0x2, 0x3C);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.clearDisplay();
  display.print("robot");
  display.print(robot_id);
  display.display();
  Serial1.begin(115200);
  Wire.begin();
  // -------------------- Serial1 Settings for GY-25
  Serial1.write(0xA5);
  Serial1.write(0x54); // set 0
  delay(500);
  Serial1.write(0xA5);
  Serial1.write(0x51); // poll
  delay(500);
  Serial1.write(0XA5);
  Serial1.write(0X55);
  // -------------------- OLED Display Settings
  

  for (uint8_t i = 0; i < 2; i++)
  {
    digitalWrite(LED_Pin, 1);
    delay(100);
    digitalWrite(LED_Pin, 0);
    delay(100);
  }
}
void RCJ_Robot::motor(int ML1, int ML2, int MR2, int MR1)
{
  // ML1 *= 0.5;
  // MR1 *= 0.5;
  
  if (use_GY)
  {
    ML1 += GY * GY_SENS;
    ML2 += GY * GY_SENS;
    MR2 += GY * GY_SENS;
    MR1 += GY * GY_SENS;
  }
  if (ML1 > 65535)
    ML1 = 65535;
  if (ML2 > 65535)
    ML2 = 65535;
  if (MR2 > 65535)
    MR2 = 65535;
  if (MR1 > 65535)
    MR1 = 65535;
  if (ML1 < -65535)
    ML1 = -65535;
  if (ML2 < -65535)
    ML2 = -65535;
  if (MR2 < -65535)
    MR2 = -65535;
  if (MR1 < -65535)
    MR1 = -65535;
  if (ML1 > 0)
  {
    digitalWrite(L1_Pin, 0);
    pwmWrite(L1_PWM, ML1);
  }
  else
  {
    digitalWrite(L1_Pin, 1);
    pwmWrite(L1_PWM, ML1 + 65535);
  }
  if (ML2 > 0)
  {
    digitalWrite(L2_Pin, 0);
    pwmWrite(L2_PWM, ML2);
  }
  else
  {
    digitalWrite(L2_Pin, 1);
    pwmWrite(L2_PWM, ML2 + 65535);
  }
  if (MR2 > 0)
  {
    digitalWrite(R2_Pin, 0);
    pwmWrite(R2_PWM, MR2);
  }
  else
  {
    digitalWrite(R2_Pin, 1);
    pwmWrite(R2_PWM, MR2 + 65535);
  }
  if (MR1 > 0)
  {
    digitalWrite(R1_Pin, 0);
    pwmWrite(R1_PWM, MR1);
  }
  else
  {
    digitalWrite(R1_Pin, 1);
    pwmWrite(R1_PWM, MR1 + 65535);
  }
}
void RCJ_Robot::read_MV()
{
  Wire.requestFrom(openMV_Address, 12); // Request 12 bytes (6 values * 2 bytes each)
  if (Wire.available() == 12)
  {
    uint8_t buffer[12];
    for (int i = 0; i < 12; i++)
    {
      buffer[i] = Wire.read(); // Read each byte into buffer
    }

    // Unpack the data
    ball_x = (int16_t)(buffer[0] | (buffer[1] << 8));
    ball_y = (int16_t)(buffer[2] | (buffer[3] << 8));
    yellow_x = (int16_t)(buffer[4] | (buffer[5] << 8));
    yellow_y = (int16_t)(buffer[6] | (buffer[7] << 8));
    blue_x = (int16_t)(buffer[8] | (buffer[9] << 8));
    blue_y = (int16_t)(buffer[10] | (buffer[11] << 8));

    ball_angle = get_angle(ball_x, ball_y);
    direction_ball = get_direction(ball_angle);
    distance_ball = sqrt(pow(ball_x - robot_x, 2) + pow(ball_y - robot_y, 2));
    yellow_angle = get_angle(yellow_x, yellow_y);
    direction_yellow = get_direction(yellow_angle);
    distance_yellow = sqrt(pow(yellow_x - robot_x, 2) + pow(yellow_y - robot_y, 2));
    blue_angle = get_angle(blue_x, blue_y);
    direction_blue = get_direction(blue_angle);
    distance_blue = sqrt(pow(blue_x - robot_x, 2) + pow(blue_y - robot_y, 2));
    if (ball_x == 0 && ball_y == 0)
      is_ball = false;
    else
      is_ball = true;
    if (yellow_x == 0 && yellow_y == 0)
      is_yellow = false;
    else
      is_yellow = true;
    if (blue_x == 0 && blue_y == 0)
      is_blue = false;
    else
      is_blue = true;
  }
}
int RCJ_Robot::get_angle(int x, int y)
{
  int angle = atan2(x - robot_x, y - robot_y) * 180 / PI;
  if (angle < 0)
    angle += 360;
  return angle;
}
int RCJ_Robot::get_direction(int angle)
{
  int direction;
  for (int i = 0; i < 16; i++)
  {
    if ((angle - 11.25 >= i * 22.5) && (angle - 11.25 < (i + 1) * 22.5))
      direction = i + 1;
  }
  if (angle <= 11.25 || angle >= 348.5)
    direction = 0;
  return direction;
}
void RCJ_Robot::read_GY()
{
  // -------------------- GY-25 Read Data
  Serial1.write(0xA5);
  Serial1.write(0x51);
  while (true)
  {
    buff[counter] = Serial1.read();
    if (counter == 0 && buff[0] != 0xAA)
      break;
    counter++;
    if (counter == 8)
    {
      counter = 0;
      if (buff[0] == 0xAA && buff[7] == 0x55)
      {
        GY = (int16_t)(buff[1] << 8 | buff[2]) / 100;
        if(reverse_GY){
          if(GY > 0) GY -= 180;
          else GY += 180;
        }
        if (GY > 180)
          GY -= 360;
        if (GY < -180)
          GY += 360;

      }
    }
  }
}
void RCJ_Robot::moveAngle(int a)
{
  if (a > 360)
    a -= 360;
  if (a < 0)
    a += 360;
  int x = -speed * cos(a * M_PI / 180);
  int y = -speed * sin(a * M_PI / 180);
  motor(-(x + y), -(x - y), -(-x - y), -(y - x));
}
void RCJ_Robot::moveXY(int x, int y)
{
  int max_v = 40000;
  if(x > max_v) x = max_v;
  if(x <-max_v) x =-max_v;
  if(y > max_v) y = max_v;
  if(y <-max_v) y =-max_v;
  motor(x+y, -x+y, -x-y, x-y);
}
void RCJ_Robot::move(int direction)
{
  if (direction == 0)
    motor(speed, speed, -speed, -speed);
  if (direction == 1)
    motor(speed, speed / 2, -speed, -speed / 2);
  if (direction == 2)
    motor(speed, 0, -speed, 0);
  if (direction == 3)
    motor(speed, -speed / 2, -speed, speed / 2);
  if (direction == 4)
    motor(speed, -speed, -speed, speed);
  if (direction == 5)
    motor(speed / 2, -speed, -speed / 2, speed);
  if (direction == 6)
    motor(0, -speed, 0, speed);
  if (direction == 7)
    motor(-speed / 2, -speed, speed / 2, speed);
  if (direction == 8)
    motor(-speed, -speed, speed, speed);
  if (direction == 9)
    motor(-speed, -speed / 2, speed, speed / 2);
  if (direction == 10)
    motor(-speed, 0, speed, 0);
  if (direction == 11)
    motor(-speed, speed / 2, speed, -speed / 2);
  if (direction == 12)
    motor(-speed, speed, speed, -speed);
  if (direction == 13)
    motor(-speed / 2, speed, speed / 2, -speed);
  if (direction == 14)
    motor(0, speed, 0, -speed);
  if (direction == 15)
    motor(speed / 2, speed, -speed / 2, -speed);
}
void RCJ_Robot::stop()
{
  motor(0, 0, 0, 0);
}
void RCJ_Robot::moveForSec(int dir, int sec)
{
  for (int i = 0; i < sec; i++)
  {
    // sensors();
    // print_all();
    move(dir);
  }
}
void RCJ_Robot::spin(bool stat)
{
  digitalWrite(SPIN_Pin, stat);
}
void RCJ_Robot::readSensors()
{
  sensor[0] = analogRead(PA0);
  sensor[1] = analogRead(PA1);
  sensor[2] = analogRead(PA2);
  sensor[3] = analogRead(PA3);
  sensor[4] = analogRead(PA4);
  sensor[5] = analogRead(PA5);
  sensor[6] = analogRead(PA6);
  sensor[7] = analogRead(PA7);
  sensor[8] = analogRead(PB0);
  sensor[9] = analogRead(PB1);
  shoot_sens = sensor[8];
  v_bat = sensor[9];
  if (shoot_sens > 2000)
    Ball_In_Kicker = true;
  else{
    Ball_In_Kicker = false;
    already_shooted = false;

  }
  if( sensor[0] > 4093) setBoostPWM(0);
  if( sensor[0] < 3800) setBoostPWM(6000);
}
void RCJ_Robot::setMotorPins(int ML1, int ML2, int MR2, int MR1)
{
  L1_Pin = ML1;
  L2_Pin = ML2;
  R2_Pin = MR2;
  R1_Pin = MR1;
}
void RCJ_Robot::setMotorPWMs(int ML1, int ML2, int MR2, int MR1)
{
  L1_PWM = ML1;
  L2_PWM = ML2;
  R2_PWM = MR2;
  R1_PWM = MR1;
}
void RCJ_Robot::buzzer(bool stat)
{
  digitalWrite(Buzzer_Pin, stat);
}
void RCJ_Robot::led(bool stat)
{
  digitalWrite(LED_Pin, stat);
}
void RCJ_Robot::readInputs()
{
  SW1 = digitalRead(SW1_Pin);
  SW2 = digitalRead(SW2_Pin);
  DIP1 = digitalRead(DIP1_Pin);
  DIP2 = digitalRead(DIP2_Pin);
}
void RCJ_Robot::update()
{
  readSensors();
  readInputs();
  read_MV();
  read_GY();
}
void RCJ_Robot::printAll()
{
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
void RCJ_Robot::printCam()
{
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

  display.setCursor(0, 0);
  display.print("bx:");
  display.println(ball_x);
  display.setCursor(0, 10);
  display.print("by:");
  display.println(ball_y);
  display.setCursor(0, 20);
  display.print("ba:");
  display.println(ball_angle);
  display.setCursor(0, 30);
  display.print("bd:");
  display.println(distance_ball);
  display.setCursor(0, 40);
  display.print("GY:");
  display.println(GY);
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
void RCJ_Robot::printSensors()
{
  display.clearDisplay();
  for (int i = 0; i < 10; i++)
  {
    if (i > 5)
      display.setCursor(64, (i - 6) * 10);
    else
      display.setCursor(0, i * 10);
    display.print(i);
    display.print(":");
    display.println(sensor[i]);
  }
  display.setCursor(64, (10 - 6) * 10);
  display.print("BAT:");
  display.println(v_bat);
  display.display();
}
void RCJ_Robot::setBoostPWM(int value)
{
  pwmWrite(BOOST_CTRL_Pin, value);
}
void RCJ_Robot::shoot(int delay_value)
{
  if (shoot_cnt > 20)
  {
    shoot_cnt = 0;
    already_shooted = false;
  }
  if (already_shooted)
  {
    shoot_cnt++;
    return;
  }
  digitalWrite(SHOOT_Pin, 1);
  delay(delay_value);
  digitalWrite(SHOOT_Pin, 0);
  shoot_cnt = 0;
  already_shooted = true;
}
void RCJ_Robot::shoot()
{
  if (shoot_cnt > 50)
  {
    shoot_cnt = 0;
    already_shooted = false;
  }
  if (already_shooted)
  {
    shoot_cnt++;
    return;
  }
  digitalWrite(SHOOT_Pin, 1);
  delay(70);
  digitalWrite(SHOOT_Pin, 0);
  shoot_cnt = 0;
  already_shooted = true;
}
bool RCJ_Robot::lookToward(int angle)
{
  int turn_speed = 8000;
  GY = 0;
  if (angle < look_sens || angle > 360-look_sens)
    return true;
  else if (angle < 180)
    motor(turn_speed, turn_speed, turn_speed, turn_speed);
  else
    motor(-turn_speed, -turn_speed, -turn_speed, -turn_speed);
  return false;
}
void RCJ_Robot::set_GY(){
  Serial1.write(0XA5);
  Serial1.write(0X55);
  delay(100);
  Serial1.write(0XA5);
  Serial1.write(0X55);
  delay(100);
}