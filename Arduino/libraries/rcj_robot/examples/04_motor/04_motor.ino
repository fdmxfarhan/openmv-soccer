#include <rcj_robot.h>
RCJ_Robot myRobot;

void setup() {
  myRobot.init();
}

void loop() {
  myRobot.motor(30000, 30000, 30000, 30000);
}
