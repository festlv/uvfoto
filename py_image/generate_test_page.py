from PIL import Image

#in mm
stripe_width = 16
num_stripes = 6
dpi = 300.0

pix_per_stripe = 16/25.4 * 300

im = Image.new("RGB", (int(pix_per_stripe * num_stripes * 2), 40), "#fff")
pix = im.load()

j = 0
for i in range(num_stripes):

    for p in range(int(pix_per_stripe)):
        for z in range(40):
            pix[p + j, z] = (0, 0, 0)
    j+=int(pix_per_stripe)*2

im.save("pattern.png")
