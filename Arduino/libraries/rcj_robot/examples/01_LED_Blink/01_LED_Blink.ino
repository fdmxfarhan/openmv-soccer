#include <rcj_robot.h>
RCJ_Robot myRobot;

void setup() {
  myRobot.init();
}

void loop() {
  myRobot.led(1);
  delay(500);
  myRobot.led(0);
  delay(500);
}
