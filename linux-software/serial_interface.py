import serial
import Queue
import time
import threading


class PrinterSerialInterface(object):

    def __init__(self, port):
        self.sending_queue = Queue.Queue()
        self.receiving_queue = Queue.Queue()
        self.port = port
        self._quit_flag = False

        self.thread = threading.Thread(
            target=self.serial_worker)
        self.thread.setDaemon(True)
        self.thread.start()


    def send_command(self, cmd, block=False):
        if not cmd.endswith('\n'):
            cmd = cmd + '\n'

        if block:
            #wait for queue to become empty
            while not self.sending_queue.empty():
                pass
        #send command
        self.sending_queue.put(cmd)

        if block:
            #wait for queue to become empty
            while not self.sending_queue.empty():
                pass

    def quit(self):
        self._quit_flag = True
        self.thread.join()

    def serial_worker(self):
        ser = serial.Serial(self.port, baudrate=115200, timeout=1)
        try:
            while not self._quit_flag:
                try:
                    msg = self.sending_queue.get(False)
                    #print("$ %s" % msg)
                    ser.write(msg)
                except Queue.Empty:
                    pass

                numchars = ser.inWaiting()
                if numchars > 0:
                    received_line = ser.readline()
                    print(received_line)
                    print("\n\n")

                time.sleep(0.1)
        except KeyboardInterrupt:
            pass
