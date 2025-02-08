import sensor
import time
import pyb
import ustruct

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time=100)
# sensor.set_auto_gain(False)  # must be turned off for color tracking
sensor.set_auto_whitebal(True)  # must be turned off for color tracking
sensor.set_auto_exposure(False, exposure_us=5000)  # Adjust exposure time in microseconds
sensor.set_auto_gain(False, gain_db=10)
# Initialize I2C
i2c = pyb.I2C(2, pyb.I2C.SLAVE, addr=0x42)

# Values to send
x_ball = 0
y_ball = 0
x_yellow = 0
y_yellow = 0
x_blue = 0
y_blue = 0


clock = time.clock()

# threshold_Ball = [40, 60, 10, 50, 20, 70]
# threshold_Yellow = [20, 70, -30, 10, 30, 60]
# threshold_Blue = [30, 60, -20, 10, -50, -20]

threshold_Ball = [-30, 60, 20, 60, 10, 80]
threshold_Yellow = [60, 80, -30, 10, 50, 100]
threshold_Blue = [30, 30, -20, 10, -50, -20]
tr = [
    threshold_Ball,
    threshold_Yellow,
    threshold_Blue
]

def bigest(blobs):
    m = blobs[0].area()
    b = blobs[0]
    for blob in blobs:
        if blob.area() > m:
            m = blob.area()
            b = blob
    return b

while True:
    clock.tick()
    img = sensor.snapshot()

    yellow_blobs = img.find_blobs([threshold_Yellow], pixels_threshold=3, area_threshold=3, merge=True, margin=10)
    blue_blobs = img.find_blobs([threshold_Blue], pixels_threshold=3, area_threshold=3, merge=True, margin=10)
    orenge_blobs = img.find_blobs([threshold_Ball], pixels_threshold=5, area_threshold=10, merge=True, margin=10)

    # if img.():
    #     print('clicked')
    if len(yellow_blobs) > 0:
        goaly = bigest(yellow_blobs)
        img.draw_rectangle(goaly.rect(), (250,250,0), fill=True)
        x_yellow = int(goaly.x() + goaly.w()/2)
        y_yellow = int(goaly.y() + goaly.h()/2)
    else:
        x_yellow = 0
        y_yellow = 0
    if len(blue_blobs) > 0:
        goalb = bigest(blue_blobs)
        img.draw_rectangle(goalb.rect(), (0,0,250), fill=True)
        x_blue = int(goalb.x() + goalb.w()/2)
        y_blue = int(goalb.y() + goalb.h()/2)
    else:
        x_blue = 0
        y_blue = 0
    if len(orenge_blobs) > 0:
        ball = bigest(orenge_blobs)
        r = int(ball.w()/2)
        x_ball = int(ball.x() + ball.w()/2)
        y_ball = int(ball.y() + ball.h()/2)
        img.draw_circle(x_ball, y_ball, r, (250,100,0), fill=True)
    else:
        x_ball = 0
        y_ball = 0

    try:
        data = ustruct.pack("<hhhhhh", x_ball, y_ball, x_yellow, y_yellow, x_blue, y_blue)
        # print(xb, yb)
        i2c.send(data)
    except OSError as e:
        print("I2C Error:", e)
