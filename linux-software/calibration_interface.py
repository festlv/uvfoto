#!/usr/bin/env python
import serial
import argparse
import threading
import Queue
import time

from py_usb import PhoneKeys, SkypePhoneDriver


class CalibrationInterface():
    serial = None
    angle = 19.0
    position = 0.0

    def run(self, port):
        self.queue = Queue.Queue()
        self.thread = threading.Thread(
            target=serial_worker,
            args=(port, self.queue))

        self.thread.start()
        self.phone = SkypePhoneDriver()
        self.phone.map_action(PhoneKeys.KEY_1, self._increase_angle)
        self.phone.map_action(PhoneKeys.KEY_4, self._decrease_angle)
        self.phone.map_action(PhoneKeys.KEY_2, self._increase_position)
        self.phone.map_action(PhoneKeys.KEY_5, self._decrease_position)
        self.phone.listen()

    def _increase_angle(self):
        self.angle += 0.1
        self._send_angle()

    def _decrease_angle(self):
        self.angle += 0.1
        self._send_angle()

    def _send_angle(self):
        self.queue.put("M2 A%f" % self.angle)

    def _increase_position(self):
        self.position += 0.1
        self._send_position()

    def _decrease_position(self):
        self.position -= 0.1
        self._send_position()

    def _send_position(self):
        self.queue.put("M1 Y%f" % self.position)


def serial_worker(port, queue):
    ser = serial.Serial(port, baudrate=115200, timeout=1)
    queue = queue
    try:
        while True:
            msg = queue.get(False)
            if msg:
                print("$ %s\n" % msg)
                ser.write("%s\n" % (msg.strip()))

            numchars = ser.inWaiting()
            if numchars > 0:
                received_line = ser.readline()
                print(received_line)

            time.sleep(0.1)
    except KeyboardInterrupt:
        pass

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("port", help="Serial port to use")
    args = parser.parse_args()
    c = CalibrationInterface()
    c.run(args.port)
