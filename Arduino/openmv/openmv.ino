#include <rcj_robot.h>
RCJ_Robot r;
int cnt_aim = 0;
int cnt_ball_in_kick = 0;
int target;
int shift = 60;
bool arrived_to_goal = false;
#define Look_At_Goal false
void setup() {
  r.robot1_x = 159;
  r.robot1_y = 110;
  r.GY_SENS = 100;
  r.init();
  r.buzzer(1);
  delay(100);
  r.buzzer(0);
}

void loop() {
  r.readSensors();
  r.readInputs();
  r.read_GY();
  r.read_MV();
  r.printCam();
  r.display.setCursor(0, 50);
  r.display.print("dy:");
  r.display.println(r.distance_yellow);
  r.display.display();

  if (r.SW2) r.set_GY();
  if (r.SW1) r.shoot();
  int ba = r.ball_angle;
  int bd = r.distance_ball;
  int xb = r.ball_x - r.robot1_x;
  int yb = r.ball_y - r.robot1_y;
  int v = 400;
  r.GY_SENS = 100;
  if (!r.DIP1) {
    if (r.Ball_In_Kicker) {
      r.reverse_GY = true;
      r.spin(1);
      r.stop();
      if (!r.DIP2) {
        r.reverse_GY = false;
        r.speed = 60000;
        r.moveAngle(0);
        r.shoot();
      } else {
        r.speed = 15000;
        // r.GY_SENS = 50;
        if (cnt_ball_in_kick < 10) {
          r.reverse_GY = false;
          r.moveAngle(0);
          cnt_ball_in_kick += 1;
          cnt_aim = 0;
          arrived_to_goal = false;
        } else if (!r.is_yellow) {
          if (r.GY > 30 || r.GY < -30) r.stop();
          else r.moveAngle(180);
        } else if (r.distance_yellow < 120 && !arrived_to_goal ) {
          cnt_aim = 0;
          r.moveAngle(r.yellow_angle + 180);
        } else if (r.distance_yellow > 170 && !arrived_to_goal) {
          cnt_aim = 0;
          r.moveAngle(r.yellow_angle);
        } else {
          arrived_to_goal = true;
          if (r.lookToward(r.yellow_angle)) {
            if (cnt_aim < 3) r.stop();
            else r.shoot();
            cnt_aim += 1;
          }
        }
      }
    } else if (r.is_ball) {
      cnt_ball_in_kick = 0;
      r.reverse_GY = false;
      r.spin(1);
      r.speed = (bd - 10) * 280;
      if (bd < 80) shift = 80;
      else shift = 45;
      if (ba < 10 || ba > 350) r.moveAngle(ba);
      else if (ba < 35) r.moveAngle(ba + shift / 2);
      else if (ba > 360 - 35) r.moveAngle(ba - shift / 2);
      else if (ba < 180) r.moveAngle(ba + shift);
      else r.moveAngle(ba - shift);
    } else {
      r.reverse_GY = false;
      r.spin(false);
      r.stop();
    }
  } else {
    r.reverse_GY = false;
    r.spin(false);
    r.stop();
  }
}
