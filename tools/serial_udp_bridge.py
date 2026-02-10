#!/usr/bin/env python3
"""
Serial-to-UDP bridge for HIL simulation.

Relays MAVLink traffic between an STM32 UART (via USB-serial adapter)
and PX4 SITL's UDP port.

Usage:
    python3 serial_udp_bridge.py /dev/ttyUSB0
    python3 serial_udp_bridge.py /dev/ttyUSB0 --baud 921600 --udp-port 14540
"""

import argparse
import socket
import sys
import threading
import serial


def serial_to_udp(ser: serial.Serial, sock: socket.socket, udp_addr: tuple):
    """Forward bytes from serial port to UDP socket."""
    while True:
        data = ser.read(ser.in_waiting or 1)
        if data:
            sock.sendto(data, udp_addr)


def udp_to_serial(ser: serial.Serial, sock: socket.socket):
    """Forward bytes from UDP socket to serial port."""
    while True:
        data, _ = sock.recvfrom(4096)
        if data:
            ser.write(data)


def main():
    parser = argparse.ArgumentParser(description="Serial <-> UDP MAVLink bridge")
    parser.add_argument("port", help="Serial port (e.g. /dev/ttyUSB0)")
    parser.add_argument("--baud", type=int, default=115200, help="Baud rate (default: 115200)")
    parser.add_argument("--udp-host", default="127.0.0.1", help="PX4 SITL UDP host")
    parser.add_argument("--udp-port", type=int, default=14540, help="PX4 SITL UDP port")
    args = parser.parse_args()

    ser = serial.Serial(args.port, args.baud, timeout=0.01)
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("0.0.0.0", 0))  # Bind to any available local port
    udp_addr = (args.udp_host, args.udp_port)

    print(f"Bridge: {args.port} @ {args.baud} <-> {udp_addr[0]}:{udp_addr[1]}")

    t1 = threading.Thread(target=serial_to_udp, args=(ser, sock, udp_addr), daemon=True)
    t2 = threading.Thread(target=udp_to_serial, args=(ser, sock), daemon=True)
    t1.start()
    t2.start()

    try:
        t1.join()
    except KeyboardInterrupt:
        print("\nStopped.")
        sys.exit(0)


if __name__ == "__main__":
    main()
