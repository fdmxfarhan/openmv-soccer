#include <Wire.h>
#include <Adafruit_SH1106_STM32.h>
Adafruit_SH1106 display(-1);
TwoWire newWire(2, I2C_FAST_MODE);
#define Wire newWire
int16_t ball_x = 0;
int16_t ball_y = 0;
int16_t yellow_x = 0;
int16_t yellow_y = 0;
int16_t blue_x = 0;
int16_t blue_y = 0;
const int slaveAddress = 0x42;
int angle_ball, direction_ball, distance_ball;
bool is_ball = false;
#define robot_x 154
#define robot_y 94


void setup() {
  pinMode(PC13, OUTPUT);
  delay(500);
  Wire.begin();
  delay(500);
  display.begin(0x2, 0x3C);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  // -------------------- Loading
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print("Wellcome");
  display.display();
  delay(500);
  display.clearDisplay();
  display.setTextSize(1);
}

void loop() {

  Wire.requestFrom(slaveAddress, 12);  // Request 12 bytes (6 values * 2 bytes each)

  if (Wire.available() == 12) {
    uint8_t buffer[12];
    for (int i = 0; i < 12; i++) {
      buffer[i] = Wire.read();  // Read each byte into buffer
    }

    // Unpack the data
    ball_x = (int16_t)(buffer[0] | (buffer[1] << 8));
    ball_y = (int16_t)(buffer[2] | (buffer[3] << 8));
    yellow_x = (int16_t)(buffer[4] | (buffer[5] << 8));
    yellow_y = (int16_t)(buffer[6] | (buffer[7] << 8));
    blue_x = (int16_t)(buffer[8] | (buffer[9] << 8));
    blue_y = (int16_t)(buffer[10] | (buffer[11] << 8));

    angle_ball = get_angle(ball_x, ball_y);
    direction_ball = get_direction(angle_ball);
    distance_ball = sqrt(pow(ball_x - robot_x, 2) + pow(ball_y - robot_y, 2));
    is_ball = true;
  }



  display.clearDisplay();
  display.drawCircle(100, 32, 12, WHITE);
  if (is_ball)
    display.fillCircle(100 + sin(angle_ball * PI / 180) * 20, 32 - cos(angle_ball * PI / 180) * 20, 3, WHITE);

  display.setCursor(0, 20);
  display.print("a:");
  display.println(angle_ball);

  // display.setCursor(0, 20);
  // display.print("ball_x:");
  // display.println(ball_x);
  // display.print("ball_y:");
  // display.println(ball_y);
  // display.print("yellow_x:");
  // display.println(yellow_x);
  // display.print("yellow_y:");
  // display.println(yellow_y);
  // display.print("blue_x:");
  // display.println(blue_x);
  // display.print("blue_y:");
  // display.println(blue_y);
  display.display();
}

int get_angle(int x, int y) {
  int angle = atan2(x - robot_x, y - robot_y) * 180 / PI;
  if (angle < 0) angle += 360;
  return angle;
}
int get_direction(int angle) {
  int direction;
  for (int i = 0; i < 16; i++) {
    if ((angle - 11.25 >= i * 22.5) && (angle - 11.25 < (i + 1) * 22.5)) direction = i + 1;
  }
  if (angle <= 11.25 || angle >= 348.5) direction = 0;
  return direction;
}
