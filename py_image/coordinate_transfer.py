from PIL import Image
import math
import pdb


def naive_sin(x):
    return x / (1.0+ (x*x / (2*3.0 - x*x + (2*3.0*x*x/(4*5.0 - x*x + (4*5.0*x*x)/1.0)))))

def naive_atan(x):
    return x / (1+((x*x) / (3 + ((2*x)*(2*x) / (5 + ((3*x)*(3*x)) / (7 + (((4*x)*(4*x))/(9+((5*x)*(5*x))))))))))

def run():
    IMAGE_WIDTH = 256 * 8
    IMAGE_HEIGHT = 2100
    im = Image.new("RGB", (IMAGE_WIDTH+2000, IMAGE_HEIGHT), "#fff")

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
    first_y = None
    for i in range(20):
        for j in range(20):
            pix[i + R - 10, j + R - 10] = (0, 0, 0)

    IMAGE_HALF = IMAGE_WIDTH / 2

    angular_offset = math.pi/2 - math.atan(IMAGE_HALF/R)

    for i in range(IMAGE_WIDTH):
        if pix[i, 0] == (0, 0, 0):
            import pdb
            #if i > IMAGE_HALF:
            #    pdb.set_trace()

            if i > IMAGE_HALF:
                p1 = float(i - IMAGE_HALF)
            else:
                p1 = float(IMAGE_HALF - i)

            a = math.atan(p1 / R)

            x = naive_sin(a) * R

            if i > IMAGE_HALF:
                a2 = a
                a1 =  math.pi/2 - angular_offset + a2
            else:
                a2 = math.pi/2 - a
                a1 = a2 - angular_offset

            xrot = a1 * R

            if i > IMAGE_HALF:
                x += IMAGE_HALF
            else:
                x = IMAGE_HALF - x

            y = math.cos(a) * R
            y = R + offset * 2 - y
            if first_y is None:
                first_y = y
            try:
                for z in range(10):
                    #pix[x, y + z] = (0, 0, 0)

                    pix[xrot, first_y] = (255, 0, 0)
            except IndexError:
                print "Failed to put pixel at: (%d, %d). i: %d" % (xrot, first_y, i)


    im.save("test.png")

if __name__ == "__main__":
    run()
