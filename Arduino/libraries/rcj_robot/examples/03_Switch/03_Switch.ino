#include <rcj_robot.h>
RCJ_Robot myRobot;

void setup() {
  myRobot.init();
}

void loop() {
  myRobot.readInputs();
  if (myRobot.SW1) {
    myRobot.led(1);
  } 
  else {
    myRobot.led(0);
  }
}
