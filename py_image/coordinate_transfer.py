from PIL import Image
import math
import pdb

def run():
    IMAGE_WIDTH = 256 * 8
    IMAGE_HEIGHT = 2100
    im = Image.new("RGB", (IMAGE_WIDTH, IMAGE_HEIGHT), "#fff")

    i = 0
    pix = im.load()

    while i < IMAGE_WIDTH:
        for j in range(160):
            if i + j < IMAGE_WIDTH:
                for z in range(10):
                    pix[j + i, z] = (0, 0, 0)
        i += 160 * 2

    offset = 10

    R = 1000.0

    #create a center point
    for i in range(20):
        for j in range(20):
            pix[i + R - 10, j + R - 10] = (0, 0, 0)

    IMAGE_HALF = IMAGE_WIDTH / 2
    for i in range(IMAGE_WIDTH):
        if pix[i, 0] == (0, 0, 0):
            if i > IMAGE_HALF:
                p1 = float(i - IMAGE_HALF)
            else:
                p1 = float(IMAGE_HALF - i)

            a = math.atan(p1 / R)
            x = math.sin(a) * R
            if i > IMAGE_HALF:
                x += IMAGE_HALF
            else:
                x = IMAGE_HALF - x

            y = math.cos(a) * R
            y = R + offset * 2 - y
            try:
                for z in range(10):
                    pix[x, y + z] = (0, 0, 0)
            except IndexError:
                print "Failed to put pixel at: (%d, %d). i: %d" % (x, y + z, i)

            for z in range(5):
                pix[x, 500] = (255, 0, 0)

    im.save("test.png")

if __name__ == "__main__":
    run()
