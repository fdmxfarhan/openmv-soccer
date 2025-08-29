#include <rcj_robot.h>
RCJ_Robot r;
int cnt_back = 0;
int cnt_ball_in_kick = 0;
int target;
int shift = 45;
#define Look_At_Goal false
void setup() {
  r.robot1_x = 161;
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

  if (r.SW2) r.set_GY();
  if (r.SW1) r.shoot();
  int ba = r.ball_angle;
  int bd = r.distance_ball;
  int xb = r.ball_x - r.robot1_x;
  int yb = r.ball_y - r.robot1_y;
  int v = 400;
  // r.display.clearDisplay();
  // r.display.setTextSize(2);
  // r.display.setCursor(0, 0);
  // r.display.print("x: ");
  // r.display.println(xb);
  // r.display.print("y: ");
  // r.display.println(yb);
  // r.display.display();
  if(!r.DIP1){
    if(r.Ball_In_Kicker){
      r.speed = 65535;
      r.moveAngle(0);
      r.shoot();
    }
    else if(r.is_ball){
      if(ba < 20 || ba >340) {
        r.speed = 30000;
        r.moveAngle(ba);
      }
      else if(yb < 40){
        if(abs(xb) > 50) r.moveXY(0, 2*v * (yb - 45));
        else r.moveXY(30000, 0);
      }
      else r.moveXY(v * (xb), v * (yb - 45));
    }
    else r.stop();
  }
  else r.stop();
}
