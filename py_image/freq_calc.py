import math
rev_per_s = 195

time_per_revolution = 1 / 195.0

useful_angle_part = 50 / 90.0

useful_time = useful_angle_part * time_per_revolution

freq = 1 / (useful_time / 4096.0)
print "Freq = %d kHz" % freq



useful_angle = 50
angular_resolution = 4096.0


def angle_position(angle):
    a = 8.0
    b = 8.0
    c = 78.0
    return a/math.cos(math.radians(angle)) - math.tan(math.radians(angle))*b + \
            math.tan(math.radians(90 - 2 * angle)) * c


pos23 = angle_position(20)
pos23_n = angle_position(20 + useful_angle/angular_resolution)

print "P23: %f" % pos23
print "P23_n: %f" % pos23_n
print "Diff: %f" % (pos23 - pos23_n)
