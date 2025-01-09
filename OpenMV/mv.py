import sensor
import time
import pyb
import ustruct

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time=100)
sensor.set_auto_gain(False)  # must be turned off for color tracking
sensor.set_auto_whitebal(False)  # must be turned off for color tracking
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

threshold_Ball = [-30, 60, 20, 60, 40, 80]
threshold_Yellow = [60, 80, -20, 10, 40, 60]
threshold_Blue = [40, 60, -20, 0, -40, -30]
tr = [
    threshold_Ball,
    threshold_Yellow,
    threshold_Blue
]
def bigest(blobs):
    if not blobs:
        return None  # Return None if the list is empty
    return max(blobs, key=lambda blob: blob.area())  # Find the blob with the largest area

    # m = blobs[0].area()
    # b = blobs[0]
    # for blob in blobs:
    #     if blob.area() > m:
    #         m = blob.area()
    #         b = blob
    # return b
while True:
    clock.tick()
    img = sensor.snapshot()

    yellow_blobs = img.find_blobs([threshold_Yellow], pixels_threshold=3, area_threshold=3, merge=True, margin=10)
    blue_blobs = img.find_blobs([threshold_Blue], pixels_threshold=3, area_threshold=3, merge=True, margin=10)
    orenge_blobs = img.find_blobs([threshold_Ball], pixels_threshold=1, area_threshold=1, merge=True, margin=5)

    if len(yellow_blobs) > 0:
        goaly = bigest(yellow_blobs)
        img.draw_rectangle(goaly.rect(), (250,250,0), fill=True)
        x_yellow = goaly.x()
        y_yellow = goaly.y()
    else:
        x_yellow = 0
        y_yellow = 0
    if len(blue_blobs) > 0:
        goalb = bigest(blue_blobs)
        img.draw_rectangle(goalb.rect(), (0,0,250), fill=True)
        x_blue = goalb.x()
        y_blue = goalb.y()
    else:
        x_blue = 0
        y_blue = 0
    if len(orenge_blobs) > 0:
        ball = bigest(orenge_blobs)
        img.draw_rectangle(ball.rect(), (250,100,0), fill=True)
        x_ball = ball.x()
        y_ball = ball.y()
    else:
        x_ball = 0
        y_ball = 0

    try:
        data = ustruct.pack("<hhhhhh", x_ball, y_ball, x_yellow, y_yellow, x_blue, y_blue)
        # print(xb, yb)
        i2c.send(data)
    except OSError as e:
        print("I2C Error:", e)
