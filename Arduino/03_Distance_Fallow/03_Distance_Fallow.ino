#include <rcj_robot.h>
RCJ_Robot r;
int cnt_back = 0;
int cnt_ball_in_kick = 0;
int target;
int shift = 60;
#define Look_At_Goal false
void setup() {
  r.robot1_x = 150;
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
      if(!r.DIP2) r.shoot();
    }
    else if(r.is_ball){
      r.speed = (bd-10) * 300;
      if(bd < 80) shift = 80; 
      else shift = 45;
      if(ba < 10 || ba > 350) r.moveAngle(ba);
      else if(ba < 180) r.moveAngle(ba + shift);
      else r.moveAngle(ba - shift);
    }
    else r.stop();
  }
  else r.stop();
}
