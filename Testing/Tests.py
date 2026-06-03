import sys
import serial


# Port will be given when starting the program
port = sys.argv[1].strip()
print(port)

# Current valid baudrate for S32K3XX implementation is 115200
baudrate = 115200

# Create serial port object to communicate via serial bus
try:
    SerialPort = serial.Serial(port, baudrate, timeout=None)
except:
    # In case that the serial port was not able to be created, then report the issue and quit the program
    print("Serial Port object creation failed, please check the inputs of the program and try again")
    print(serial.tools.list_ports.comports())
    quit()

def TestAT(void):
    SerialPort.write("OK\n\r")
    
def attend_command(at_command):
    match at_command:
        case "AT":
            # Execute testing of the TestAT case
            TestAT()

while True:
    # This is the main loop of execution where the test will take place depending on the AT command received

    # Read the serial terminal until \n (command received) or timeout
    print("waiting for input")
    input = SerialPort.readline()
    if input:
        command = input.decode('utf-8', errors='ignore').strip()
        if command:
            print("Received command: " + command)
            attend_command(command)
            

