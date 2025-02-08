#include <rcj_robot.h>
RCJ_Robot r;
void setup() {
  r.robot1_x = 158;
  r.robot1_y = 120;
  r.init();
  r.setBoostPWM(3000);
  r.buzzer(1);
  delay(100);
  r.buzzer(0);
}

void loop() {
  r.readSensors();
  r.readInputs();
  r.read_GY();
  r.read_MV();
  if (r.DIP2) r.printSensors();
  else r.printCam();
  if (r.SW2) r.set_GY();
  if (r.SW1 && r.Ball_In_Kicker) r.shoot();


  if (r.DIP2) r.stop();
  else if (r.Ball_In_Kicker) {
    r.spin(r.DIP1);
    if (r.is_yellow && r.DIP1) {
      if (r.lookToward(r.yellow_angle)) {
        r.stop();
        r.shoot();
      }
    } else {
      r.speed = 50000;
      r.move(0);
      r.shoot();
    }
  } else if (r.is_ball) {
    r.spin(r.DIP1);
    if (r.distance_ball < 95) {
      r.speed = 30000;
      if (r.ball_angle < 20 || r.ball_angle > 340) {
        r.moveAngle(r.ball_angle);
        r.speed = 40000;
      }
      else if (r.ball_angle < 60) r.moveAngle(r.ball_angle + 60);
      else if (r.ball_angle > 300) r.moveAngle(r.ball_angle - 60);
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
    r.spin(0);
    r.stop();
  }
}
