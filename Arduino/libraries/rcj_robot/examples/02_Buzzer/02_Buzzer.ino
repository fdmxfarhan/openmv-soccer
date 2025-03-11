#include <rcj_robot.h>
RCJ_Robot myRobot;

void setup() {
  myRobot.init();
}

void loop() {
  myRobot.buzzer(1);
  delay(500);
  myRobot.buzzer(0);
  delay(500);
}
