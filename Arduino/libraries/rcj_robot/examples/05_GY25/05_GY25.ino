#include <rcj_robot.h>
RCJ_Robot myRobot;

void setup() {
  myRobot.init();
}

void loop() {
  myRobot.read_GY();
  myRobot.motor(GY*200, GY*200, GY*200, GY*200);
}
