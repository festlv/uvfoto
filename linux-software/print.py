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


class PrinterException(Exception):
    pass


class PrinterInterface():
    serial = None
    angle = 17.0
    position = 150.0
    bytestr_length = 400

    def _setup_serial(self, args):
        if not args.port:
            raise PrinterException("Serial port is required!")

        self.serial = PrinterSerialInterface(args.port)

    def home(self, args):
        self._setup_serial(args)
        self.serial.send_command("G28", block=True)
        self.serial.quit()

    def _source_to_target_pixel(self, pixel_num, dpi, offset):
        #first, convert the source pixel num to mm
        source_pixel_mm = (pixel_num / float(dpi)) * 25.4 + offset
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
        offset_top = args.offset_top or 0.0
        offset_left = args.offset_left or 0.0

        for i in xrange(img_size_y):
            row = "M3 B"
            buf = [0] * self.bytestr_length
            use_row = False

            for j in xrange(img_size_x):
                p = pix[j, i]
                if p[0] == 0 and p[3] == 255:
                    use_row = True
                    #we have a black pixel
                    target_pixel = int(
                        self._source_to_target_pixel(j, dpi, offset_left)
                    )
                    target_byte = target_pixel / 8
                    target_mask = 1 << (target_pixel % 8)
                    buf[target_byte] = buf[target_byte] | target_mask

            if use_row:
                row += ''.join([chr(b) for b in buf]) + '\n'

                gcode_output.write("G1 X%f\n" % (
                    offset_top + i * gcode_x_increment_mm))
                gcode_output.write(row)

        return gcode_output

    def prepare(self, args):
        gcode_f = self.process_file(args)
        if args.output:
            f = open(args.output, 'w')
            f.write(gcode_f.getvalue())
        else:
            print(gcode_f.getvalue())

    def send(self, args):
        self._setup_serial(args)
        gcode_f = open(args.filename, 'r')

        gcode_lines = gcode_f.read().split('\n')
        i = 0
        for l in gcode_lines:
            i += 1
            print("Sending: %d/%d\n" % (i, len(gcode_lines)))
            self.serial.send_command(l + '\n', block=True)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", help="Serial port to use")

    p = PrinterInterface()

    subparsers = parser.add_subparsers(title="commands")
    parser_home = subparsers.add_parser('home', help="Move to home location")
    parser_home.set_defaults(func=p.home)

    parser_print = subparsers.add_parser(
        'prepare', help="Prepare an image for printing")
    parser_print.set_defaults(func=p.prepare)

    parser_send = subparsers.add_parser(
        'send', help="Sends G-code file to exponer")

    parser_send.set_defaults(func=p.send)
    parser_send.add_argument("filename", help="G-code file to send to exponer")

    parser_print.add_argument("filename", help="Image file to print")

    parser_print.add_argument(
        "--output",
        help="Filename to save G-code to (defaults to stdout)")

    parser_print.add_argument(
        "--dpi", type=int,
        help="DPI of the image file (only relevant for non-SVG files")

    parser_print.add_argument(
        "--intensity", type=int,
        help="Intensity of laser beam (0-255)")

    parser_print.add_argument(
        "--offset-left", type=float,
        help="Offset from left side, mm")

    parser_print.add_argument(
        "--offset-top", type=float,
        help="Offset from top, mm")

    parser_print.add_argument(
        "--center", type=bool,
        help="Center the image from left-to-right")

    args = parser.parse_args()
    args.func(args)
