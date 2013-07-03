#!/usr/bin/env python
import argparse
from serial_interface import PrinterSerialInterface
import cStringIO
from cairosvg.surface import PNGSurface
from PIL import Image
import pdb
import numpy

MM_TO_PIX = (
    (0, 18.0,  36.0,  51.0,  63.0,  74.0,  83.0,  93.5,   102.5,  111.0,  119.0,  128.0,  137.0,  147.0,  158.0,  187.0),
    (0, 160.0, 320.0, 480.0, 640.0, 800.0, 960.0, 1120.0, 1280.0, 1440.0, 1600.0, 1760.0, 1920.0, 2080.0, 2240.0, 2400.0)
)


class PrinterInterface():
    serial = None
    angle = 17.0
    position = 150.0
    bytestr_length = 400

    def _setup_serial(self, args):
        self.serial = PrinterSerialInterface(args.port)

    def home(self, args):
        self._setup_serial(args)
        self.serial.send_command("G28", block=True)
        self.serial.quit()

    def _source_to_target_pixel(self, pixel_num, dpi):
        #first, convert the source pixel num to mm
        source_pixel_mm = (pixel_num / float(dpi)) * 25.4
        #then, perform a linear interpolation
        return numpy.interp(source_pixel_mm, MM_TO_PIX[0], MM_TO_PIX[1])

    def process_file(self, args):
        """Processes printable file, returns G-code-like file in memory"""
        filename = args.filename
        converted_image = cStringIO.StringIO()
        dpi = 400

        PNGSurface.convert(dpi=dpi, url=filename, write_to=converted_image)
        converted_image.reset()

        img = Image.open(converted_image)
        pix = img.load()

        img_size_x = img.size[0]
        img_size_y = img.size[1]

        converted_image.close()

        gcode_output = cStringIO.StringIO()
        gcode_x_increment_mm = (1.0 / dpi) * 25.4

        for i in xrange(img_size_y):
            row = "M3 B"
            buf = [0] * self.bytestr_length
            use_row = False

            for j in xrange(img_size_x):
                p = pix[j, i]
                if p[0] == 0 and p[3] == 255:
                    use_row = True
                    #we have a black pixel
                    target_pixel = int(self._source_to_target_pixel(j, dpi))
                    target_byte = target_pixel / 8
                    target_mask = 1 << (target_pixel % 8)
                    buf[target_byte] = buf[target_byte] | target_mask

            if use_row:
                row += ''.join([chr(b) for b in buf]) + '\n'

                gcode_output.write("G1 X%f\n" % (i * gcode_x_increment_mm))
                gcode_output.write(row)



        return gcode_output

    def run_print(self, args):
        gcode_f = self.process_file(args)
        print(gcode_f.getvalue())

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", help="Serial port to use")

    p = PrinterInterface()

    subparsers = parser.add_subparsers(title="commands")
    parser_home = subparsers.add_parser('home', help="Move to home location")
    parser_home.set_defaults(func=p.home)

    parser_print = subparsers.add_parser('print', help="Print an image")
    parser_print.set_defaults(func=p.run_print)
    parser_print.add_argument("filename", help="Image file to print")
    parser_print.add_argument(
        "--dpi",
        help="DPI of the image file (only relevant for non-SVG files")

    parser_print.add_argument(
        "--intensity",
        help="Intensity of laser beam (0-255)")

    parser_print.add_argument(
        "--offset-left",
        help="Offset from left side, mm")

    parser_print.add_argument(
        "--offset-top",
        help="Offset from top, mm")

    parser_print.add_argument(
        "--center",
        help="Center the image from left-to-right")

    args = parser.parse_args()
    args.func(args)
    #c.run(args.port)
