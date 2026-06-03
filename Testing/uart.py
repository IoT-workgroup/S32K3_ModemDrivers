# Python driver for UART project
# Description:  This driver allows the transmission an reception by serial
# Version: 1.0
# Release notes:
# 		1.0: Initial version


import threading
import logging
import serial
import time
import math
import sys
import os
from collections import deque

TIMEOUT_SEND = 0.2

class UART:
    def __init__(self, port, br, parity):
        self._ser = serial.Serial(port, br, timeout=TIMEOUT_SEND, parity=parity)

    def send_byte(self, byte):
        self._ser.flush()
        if isinstance(byte, int):
            byte = byte.to_bytes(1, 'big')  # Convert int to bytes
        self._ser.write(byte)
        time.sleep(0.001)

    def send_bytes(self, buffer):
        self._ser.flush()
        self._ser.write(buffer)

    def get_byte(self):
        self._ser.flush()
        byte_val = self._ser.read()
        if byte_val:
            return int.from_bytes(byte_val, "big")
        return None  # Return None if no data received
