from PIL import Image
import math


def run():
    IMAGE_WIDTH = 256 * 8
    IMAGE_HEIGHT = 500
    im = Image.new("RGB", (IMAGE_WIDTH * 2, IMAGE_HEIGHT), "#fff")

    START_ANGLE = 18
    USEFUL_ANGLE = 54
    ANGULAR_RESOLUTION = 4096.0

    i = 0
    pix = im.load()

    while i < IMAGE_WIDTH:
        for j in range(160):
            if i + j < IMAGE_WIDTH:
                for z in range(10):
                    pix[j + i, z] = (0, 0, 0)
        i += 160 * 2

    offset = 10

    A = 80.0
    B = 80.0
    C = 840.0
    TIME_PER_REVOLUTION = 1 / 195.0
    USEFUL_ANGLE_PART = 1
    USEFUL_TIME = USEFUL_ANGLE_PART * TIME_PER_REVOLUTION
    FREQ = 1 / (USEFUL_TIME / ANGULAR_RESOLUTION)

    IMAGE_HALF = IMAGE_WIDTH / 2

    ANGULAR_RESOLUTION_DIFF = USEFUL_ANGLE / ANGULAR_RESOLUTION

    for i in range(int(ANGULAR_RESOLUTION)):
        angle = START_ANGLE + i * ANGULAR_RESOLUTION_DIFF
        position = A / math.cos(math.radians(angle)) - \
            math.tan(math.radians(angle)) * B + \
            math.tan(math.radians(90 - 2 * angle)) * C

        position = position

        if position > 0:
            offset = int(IMAGE_HALF - position)
        else:
            offset = int(IMAGE_HALF + math.fabs(position))

        print "Angle: %f, position: %f, offset: %d" % (angle, position, offset)
        if offset > 0 and pix[offset, 0] == (0, 0, 0):
            pix[i, 50] = (255, 0, 0)

    im.save("test.png")
    print "Calculated freq: %d" % FREQ
if __name__ == "__main__":
    run()
