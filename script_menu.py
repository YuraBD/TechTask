import serial
import struct
import threading
from math import floor
import time
import argparse


SERIAL_PORT = 'COM4'
BAUD_RATE = 115200
lock = threading.Lock()


def change_mode():
    print()
    print('Led modes:')
    print('1. On')
    print('2. Off')
    print('3. Blink')

    try:
        user_choice = int(input('Choose mode: ')) - 1
    except ValueError:
        print('Wrong input: Choose from 1, 2 and 3\n')
        return
    if user_choice not in [0, 1, 2]:
        print('Wrong input: Choose from 1, 2 and 3\n')
        return
    print('Please wait...')
    print()
    print_to_UART(mode = user_choice)

def change_frequency():
    print()
    print('Change blinking frequency: 0.5Hz to 10Hz (enter just number)')
    try:
        user_choice = float(input('Enter frequency: '))
    except ValueError:
        print('Wrong input: Enter float value from 0.5 to 10\n')
        return
    if user_choice < 0.5 or user_choice > 10:
        print('Wrong input: Enter float value from 0.5 to 10\n')
        return
    print('Please wait...')
    print()
    period = floor(10000 / user_choice / 2)
    print_to_UART(period = period)

def change_brightness():
    print()
    print('Change brightness of the LED. From 0%-100% (Enter just number)')
    try:
        user_choice = int(input('Enter brightness: '))
    except ValueError:
        print('Wrong input: Enter int value from 0 to 100\n')
        return
    if user_choice < 0 or user_choice > 100:
        print('Wrong input: Enter int value from 0 to 100\n')
        return
    print('Please wait...')
    print()
    print_to_UART(brightness = user_choice)

def print_to_UART(mode = 3, brightness = 101, period = 0):
    opt_string = struct.pack('iii', mode, brightness, period)
    with lock:
        with serial.Serial(SERIAL_PORT, BAUD_RATE) as ser:
            ser.write(opt_string)

def wait_set_to_default():
    while True:
        with lock:
            with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1) as ser:
                read_char = ser.read(1)
            if read_char == b'D':
                print('\n\nDevice set to default value: 100% intensity, 2 Hz blink\n')
                print('Continue your input: ', end='', flush=True)
        time.sleep(0)

def show_menu():
    menu = {
        1: change_mode,
        2: change_frequency,
        3: change_brightness,
        4: exit
    }
    print(flush=True)
    print('1. Change led mode')
    print('2. Change led blinking frequency')
    print('3. Change led brightness')
    print('4. Exit')

    try:
        user_choice = int(input('Choose an option: '))
    except ValueError:
        print("Wrong input: Choose from 1, 2, 3 and 4")
        return
    try:
        menu[user_choice]()
    except KeyError:
        print("Wrong input: Choose from 1, 2, 3 and 4")
        return

if __name__ == "__main__":
    formatter = lambda prog: argparse.HelpFormatter(prog, max_help_position=52)
    parser = argparse.ArgumentParser(formatter_class=formatter)
    parser.add_argument('-p', '--port', help='Change serial port (Default COM4)',
                        type=str, metavar='VALUE')
    parser.add_argument('-br', '--baudrate', help='Change baud rate (Default 115200)',
                        type=int, metavar='VALUE')
    args = parser.parse_args()
    if (args.port != None):
        SERIAL_PORT = args.port
    if (args.baudrate != None):
        BAUD_RATE = args.baudrate

    uart_listener = threading.Thread(target=wait_set_to_default, daemon=True)
    uart_listener.start()
    while True:
        show_menu()