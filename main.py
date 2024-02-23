from inputs import devices, get_gamepad
import serial
import threading
import time
import go2me_devices
import usb1
from importlib import import_module
from os import path
from subprocess import check_call, CalledProcessError

def main():
    device_name = "thrustmaster_enzo"
    _context = usb1.USBContext()

    try:
        device = import_module('go2me_devices.' + device_name)
    except ModuleNotFoundError:
        print('Device name "' + device_name + '" is invalid.')
        raise

    try:
        device
    except UnboundLocalError:
        print('Device name "' + device_name + '" is invalid.')
        raise

    if device.jscal is not None:
        dev_path = '/dev/input/by-id/' + device.dev_by_id
        if path.exists(dev_path):
            _jscal(device.jscal, dev_path)

    # Replace '/dev/ttyACM0' with the appropriate port for your Bluetooth module
    bluetooth_port = '/dev/rfcomm0'  # Example port, replace with your Bluetooth module port
    bluetooth_baudrate = 9600
    ser = serial.Serial(bluetooth_port, bluetooth_baudrate)

    thread = threading.Thread(target=read_gamepad, args=(ser,), daemon=True)
    thread.start()

    # Main thread can do other tasks or wait for user input
    while True:
        user_input = input("Press 'q' to quit: ")
        if user_input.lower() == 'q':
            break

    # Wait for all threads to finish
    thread.join()

def _jscal(configuration, device_file):
    try:
        check_call(['jscal', '-s', configuration, device_file])
        print("Success in calibration")
    except FileNotFoundError:
        print('jscal not found, skipping device calibration.')
    except CalledProcessError as err:
        print('jscal non-zero exit code {}, device may not be calibrated'.format(str(err)[-1]))

def read_gamepad(ser):
    oldCode = "ABS_X"
    oldState = 128
    lastAbsX = oldState
    while True:
        events = get_gamepad()
        for event in events:
            data = f"{event.code} {event.state}\n"
            if((oldCode == event.code and oldState == event.state) or event.code == "MSC_SCAN" or event.code == "SYN_REPORT"):
                continue
            if(event.code == "ABS_X"):
                if(event.state > 255 or event.state < 0 or (event.state < lastAbsX + 10 and event.state > lastAbsX - 10)):
                    continue
                lastAbsX = event.state

            print(f"{oldState}\t{lastAbsX}\t{data}")
            oldState = event.state
            oldCode = event.code
            
            ser.write(data.encode())

if __name__ == '__main__':
    main()