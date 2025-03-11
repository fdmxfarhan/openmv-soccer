#include <rcj_robot.h>
RCJ_Robot r;
void setup() {
  r.init();
  r.setBoostPWM(2000);
}

void loop() {
  r.readSensors();
  r.readInputs();
  r.read_GY();
  r.read_MV();
  // r.printSensors();
  r.printCam();

  if (r.DIP2) r.stop();
  else if (r.Ball_In_Kicker) {
    r.spin(1);
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
    r.spin(1);
    if(r.distance_ball < 70){
      r.speed = 20000;
      if (r.ball_angle < 50 || r.ball_angle > 300) {r.speed = 30000; r.moveAngle(r.ball_angle);}
      else if (r.ball_angle < 120) r.moveAngle(r.ball_angle + 60);
      else if (r.ball_angle > 240) r.moveAngle(r.ball_angle - 60);
      else if (r.ball_angle < 180) r.moveAngle(r.ball_angle + 90);
      else  r.moveAngle(r.ball_angle - 90);
    }else{
      r.speed = 40000;
      if (r.ball_angle < 25 || r.ball_angle > 335) r.moveAngle(0);
      else if (r.ball_angle < 120) r.moveAngle(r.ball_angle + 30);
      else if (r.ball_angle > 240) r.moveAngle(r.ball_angle - 30);
      else if (r.ball_angle < 180) r.moveAngle(r.ball_angle + 60);
      else  r.moveAngle(r.ball_angle - 60);
    }
  } else {
    r.spin(0);
    r.stop();
  }
}
