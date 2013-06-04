#!/usr/bin/env python
# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

import usb.core
import usb.util
import sys


class PhoneKeys(object):
    KEY_0 = 0x00
    KEY_1 = 0x01
    KEY_2 = 0x02
    KEY_3 = 0x03
    KEY_4 = 0x04
    KEY_5 = 0x05
    KEY_6 = 0x06
    KEY_7 = 0x07
    KEY_8 = 0x08
    KEY_9 = 0x09
    KEY_R = 0x0A
    KEY_STAR = 0x0B
    KEY_HASH = 0x0C
    KEY_PICK = 0x0D
    KEY_DROP = 0x0E
    KEY_UP = 0x0F
    KEY_DOWN = 0x10
    KEY_NOOP = 0x11


class SkypePhoneDriver(object):

    PHONE_VENDOR_ID = 0x0451
    PHONE_PROD_ID = 0x1020

    KEY_INTERFACE = 0x03

    _usb_code_mapping = {
        0x38: PhoneKeys.KEY_0,
        0x10: PhoneKeys.KEY_1,
        0x20: PhoneKeys.KEY_2,
        0x30: PhoneKeys.KEY_3,
        0x40: PhoneKeys.KEY_4,
        0x50: PhoneKeys.KEY_5,
        0x60: PhoneKeys.KEY_6,
        0x70: PhoneKeys.KEY_7,
        0x08: PhoneKeys.KEY_8,
        0x18: PhoneKeys.KEY_9,
        0x28: PhoneKeys.KEY_STAR,
        0x48: PhoneKeys.KEY_HASH,
        0x03: PhoneKeys.KEY_R,
        0x01: PhoneKeys.KEY_UP,
        0x02: PhoneKeys.KEY_DOWN,
        0x68: PhoneKeys.KEY_PICK,
        0x78: PhoneKeys.KEY_DROP,
        0x00: PhoneKeys.KEY_NOOP
    }

    def __init__(self, timeout=1000):
        self._action_map = dict()
        self._init_usb()
        self._map_default_actions()
        self._timeout = timeout

    def _init_usb(self):
        dev = usb.core.find(idVendor=self.PHONE_VENDOR_ID,
                            idProduct=self.PHONE_PROD_ID)

        if dev is None:
            raise ValueError('Device not found')

        if dev.is_kernel_driver_active(self.KEY_INTERFACE):
            print "detaching kernel driver"
            dev.detach_kernel_driver(self.KEY_INTERFACE)

        dev.set_configuration()
        usb.util.claim_interface(dev, self.KEY_INTERFACE)
        self.dev = dev

    def _cleanup(self):
        usb.util.release_interface(self.dev, self.KEY_INTERFACE)
        usb.util.dispose_resources(self.dev)

    def _noop(self):
        """default action, does nothing
        """
        pass

    def _map_default_actions(self):
        phone_keys = [i for i in dir(PhoneKeys) if not i.startswith('__')]
        for p_key in phone_keys:
            attr_ndx = getattr(PhoneKeys, p_key)
            self._action_map[attr_ndx] = [self._noop]

    def map_action(self, key, action):
        self._action_map[key] = [action]

    def append_action(self, key, action):
        key_actions = self._action_map[key]
        if (len(key_actions) == 1 and key_actions[0] == self._noop):
            key_actions = []
        key_actions.append(action)
        self._action_map[key] = key_actions

    def clear_actions(self, key):
        self._action_map[key] = [self._noop]

    def _exec_action(self, key_code):
        action_id = self._usb_code_mapping[key_code]
        action_list = self._action_map[action_id]
        for action in action_list:
            action()

    def listen(self):
        read_bytes = 2
        endpoint_addr = 0x03 | usb.util.ENDPOINT_IN
        while True:
            try:
                # actual reading from device
                (key_code, _) = self.dev.read(endpoint_addr,
                                              read_bytes,
                                              self.KEY_INTERFACE,
                                              self._timeout)
                self._exec_action(key_code)
            except usb.core.USBError:
                pass
            except KeyboardInterrupt, k:
                self._cleanup()
                raise k


def print_hello():
    print "hello world"


def move(direction):
    def _move():
        print direction
    return _move


def main():
    phone = SkypePhoneDriver()
    phone.map_action(PhoneKeys.KEY_2, move('/\\'))
    phone.map_action(PhoneKeys.KEY_6, move('-->'))
    phone.map_action(PhoneKeys.KEY_4, move('<--'))
    phone.map_action(PhoneKeys.KEY_8, move('\/'))
    # phone.append_action(PhoneKeys.KEY_4, print_hello)
    # phone.clear_actions(PhoneKeys.KEY_2)
    phone.listen()


if __name__ == "__main__":
    try:
        main()
    except Exception, e:
        print str(e)
        sys.exit(1)
    except KeyboardInterrupt:
        print "interrupted by user"
        sys.exit(0)
