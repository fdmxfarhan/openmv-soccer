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
xb = 0  # Example value 1
yb = 0  # Example value 2


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
    orenge_blobs = img.find_blobs([threshold_Ball], pixels_threshold=1, area_threshold=1, merge=True, margin=5)

    if len(yellow_blobs) > 0:
        goaly = bigest(yellow_blobs)
        img.draw_rectangle(goaly.rect(), (250,250,0), fill=True)
    if len(blue_blobs) > 0:
        goalb = bigest(blue_blobs)
        img.draw_rectangle(goalb.rect(), (0,0,250), fill=True)
    if len(orenge_blobs) > 0:
        ball = bigest(orenge_blobs)
        img.draw_rectangle(ball.rect(), (250,100,0), fill=True)

    try:
        data = ustruct.pack("<hh", xb, yb)
        # print(xb, yb)
        # i2c.send(data)
    except OSError as e:
        print("I2C Error:", e)
