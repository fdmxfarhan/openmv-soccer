#include <rcj_robot.h>
RCJ_Robot r;
int cnt_back = 0;
int cnt_ball_in_kick = 0;
int target;
#define Look_At_Goal false
void setup() {
  r.robot1_x = 158;
  r.robot1_y = 90;
  r.GY_SENS = 80;
  r.init();
  r.buzzer(1);
  delay(100);
  r.buzzer(0);
}

void loop() {
  r.readSensors();
  r.readInputs();
  if (r.is_yellow && Look_At_Goal) {
    target = r.yellow_angle;
    if (target > 180) target -= 360;
    r.GY = target * 3;
  }
  else r.read_GY();
  r.read_MV();
  if (r.DIP2) r.printSensors();
  else r.printCam();
  if (r.SW2) r.set_GY();
  if (r.SW1 && r.Ball_In_Kicker) r.shoot();
  if (r.DIP2 && r.DIP1) r.spin(1);
  else if (r.DIP2) r.spin(0);
  if (r.DIP2) r.stop();
  else if (r.Ball_In_Kicker) {
    r.spin(r.DIP1);
    cnt_ball_in_kick++;
    if (cnt_ball_in_kick > 20) {
      r.moveAngle(180);
      if (cnt_ball_in_kick > 40) cnt_ball_in_kick = 0;
    } 
    // else if (r.is_yellow) {
    //   r.speed = 65000;
    //   if (target < 10 && target > -10) {
    //     r.shoot();
    //   } else r.move(0);
    // } 
    else {
      r.speed = 65000;
      r.move(0);
      r.shoot();
    }
  } else if (r.is_ball) {
    cnt_ball_in_kick = 0;
    cnt_back = 0;
    r.spin(r.DIP1);
    if (r.distance_ball < 95) {
      r.speed = 25000;
      if (r.ball_angle < 15 || r.ball_angle > 345) {
        r.moveAngle(0);
        r.speed = 40000;
      } else if (r.ball_angle < 20 || r.ball_angle > 340) {
        r.moveAngle(r.ball_angle);
        r.speed = 30000;
      } else if (r.ball_angle < 60) r.moveAngle(r.ball_angle + 40);
      else if (r.ball_angle > 300) r.moveAngle(r.ball_angle - 40);
      else if (r.ball_angle < 180) r.moveAngle(r.ball_angle + 90);
      else r.moveAngle(r.ball_angle - 90);
    } else {
      r.speed = 40000;
      if (r.ball_angle < 50 || r.ball_angle > 310) r.moveAngle(r.ball_angle);
      else if (r.ball_angle < 120) r.moveAngle(r.ball_angle + 30);
      else if (r.ball_angle > 240) r.moveAngle(r.ball_angle - 30);
      else if (r.ball_angle < 180) r.moveAngle(r.ball_angle + 60);
      else r.moveAngle(r.ball_angle - 60);
    }
  } else {
    r.speed = 30000;
    if (cnt_back < 50) r.moveAngle(180);
    else r.stop();
    r.spin(0);
    cnt_back++;
  }
}
