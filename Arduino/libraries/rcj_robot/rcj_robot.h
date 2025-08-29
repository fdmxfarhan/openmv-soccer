#ifndef RCJ_ROBOT_H
#define RCJ_RPBPT_H

#include <Arduino.h>
#include <Adafruit_SH1106_STM32.h>

#define openMV_Address 0x42
#define MAX_SPEED 65535
class RCJ_Robot
{
public:
    RCJ_Robot() : display(-1) {}
    Adafruit_SH1106 display;

    uint8_t L1_Pin = PB15;
    uint8_t L2_Pin = PB14;
    uint8_t R2_Pin = PB12;
    uint8_t R1_Pin = PB13;
    uint8_t L1_PWM = PB7;
    uint8_t L2_PWM = PB8;
    uint8_t R2_PWM = PA8;
    uint8_t R1_PWM = PB6;

    uint8_t SPIN_Pin = PC14;
    uint8_t SHOOT_Pin = PC15;

    uint8_t Buzzer_Pin = PB3;
    uint8_t LED_Pin = PC13;
    uint8_t SW1_Pin = PA15;
    uint8_t SW2_Pin = PA11;
    uint8_t DIP1_Pin = PB4;
    uint8_t DIP2_Pin = PB5;
    uint8_t BOOST_CTRL_Pin = PB9;
    uint8_t ID_Select_Pin = PA12;

    int16_t ball_x = 0;
    int16_t ball_y = 0;
    int16_t yellow_x = 0;
    int16_t yellow_y = 0;
    int16_t blue_x = 0;
    int16_t blue_y = 0;

    int ball_angle, direction_ball, distance_ball;
    int yellow_angle, direction_yellow, distance_yellow;
    int blue_angle, direction_blue, distance_blue;
    int look_sens = 8;
    int GY = 0;
    int GY_SENS = 100;
    bool is_ball = false;
    bool is_yellow = false;
    bool is_blue = false;
    bool reverse_GY = false;
    bool use_GY = true;
    bool SW1, SW2, DIP1, DIP2;
    uint16_t robot_x;
    uint16_t robot_y;
    uint16_t robot1_x = 155;
    uint16_t robot1_y = 90;
    uint16_t robot2_x = 155;
    uint16_t robot2_y = 90;
    uint8_t robot_id = 1;
    uint16_t speed = 30000;

    int shoot_sens, sensor[10];
    float v_bat;
    int shr, shl, shb, shf;
    bool Ball_In_Kicker = false;

    int get_angle(int x, int y);
    int get_direction(int angle);
    void init();
    void motor(int ML1, int ML2, int MR2, int MR1);
    void read_MV();
    void read_GY();
    void moveAngle(int a);
    void moveXY(int x, int y);
    void move(int direction);
    void spin(bool stat);
    void stop();
    void moveForSec(int dir, int sec);
    void readSensors();
    void setMotorPins(int ML1, int ML2, int MR2, int MR1);
    void setMotorPWMs(int ML1, int ML2, int MR2, int MR1);
    void buzzer(bool stat);
    void led(bool stat);
    void readInputs();
    void update();
    void printAll();
    void printCam();
    void printSensors();
    void setBoostPWM(int value);
    void shoot();
    void shoot(int delay_value);
    bool lookToward(int angle);
    void set_GY();
private:
    int buff[8];
    int counter;
    bool already_shooted = false;
    int shoot_cnt;
};

#endif
