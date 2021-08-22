#!/usr/bin/env python3
"""Python script to create a virtual serial port and send radio_packet in it.

It currently only sends a single message, because I didn't want to reimplement
radio_compute_crc in Python.

When run, the script will display `remote={...}`. You will need to update the
rocketsd config so that the serial module's port matches the displayed value.

Requires 'socat' to be installed and available in PATH.
"""


import argparse
import serial
import subprocess
import time


class VirtualSerial:
    def __init__(self, baudrate: int = 115200):
        socat = subprocess.Popen(
            ['socat', '-d', '-d', f'pty,raw,echo=0,b{baudrate}', f'pty,raw,echo=0,b{baudrate}'],
            stderr=subprocess.PIPE
        )
        self.local = socat.stderr.readline().decode().strip().split(' ')[-1]
        self.remote = socat.stderr.readline().decode().strip().split(' ')[-1]

        self.serial = serial.Serial(self.local, baudrate)

    def write(self, data) -> int:
        return self.serial.write(data)

    def read(self, n=1) -> bytes:
        return self.serial.read(n)


def main(args) -> None or int:
    vcom = VirtualSerial()
    print(f'remote={vcom.remote}')

    while True:
        vcom.write(b'\x03\x0f\x00\x00\x20\x41\x00\x00\x00\x00\xd3')  # node=3, message=15, float=10
        time.sleep(args.interval)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('-B', '--baudrate', type=int, default=115200, help='Baudrate of virtual TTY')
    parser.add_argument('-n', '--interval', type=float, default=0.5, help='Write interval (in seconds)')
    exit(main(parser.parse_args()) or 0)
