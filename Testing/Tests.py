import sys
import serial
from time import sleep

status_counter = 0
network_mode = 2
preferred_mode = 0

# Port will be given when starting the program
port = sys.argv[1].strip()
print(port)

# Current valid baudrate for S32K3XX implementation is 115200
baudrate = 115200

# Create serial port object to communicate via serial bus
try:
    SerialPort = serial.Serial(port, baudrate, timeout=None)
    print(SerialPort)
except:
    # In case that the serial port was not able to be created, then report the issue and quit the program
    print("Serial Port object creation failed, please check the inputs of the program and try again")
    #print(serial.tools.list_ports.comports())
    quit()

def Test_Impl(index):
    # Test out the functionality of the Wait Response Implementation in the device
    Responses = [b"OK\r\n", b"ERROR\r\n", b"OUTOFBOUNDS\r\n"]
    SerialPort.write(Responses[index])

def Test_AT():
    SerialPort.write(b'OK\r\n')

def SIM_Status(index):
    Status = [b"READY", b"SIM PIN", b"SIM PUK", b"NOT INSERTED", b"NOT READY"]
    SerialPort.write(b"+CPIN:" + Status[index] + b"\r\n")
    SerialPort.write(b"OK\r\n")

def SIM_SetNetworkMode(mode):
    network_mode = mode
    SerialPort.write(b"OK\r\n")

def SIM_GetNetworkMode():
    SerialPort.write(b"\r\n+CNMP:" + bytes(str(network_mode)) + b"\r\n")
    SerialPort.write(b"OK\r\n")

def SIM_SetPreferredMode(mode):
    preferred_mode = mode
    SerialPort.write(b"OK\r\n")

def SIM_GetPreferredMode(mode):
    SerialPort.write(b"\r\n+CMNB:" + bytes(str(preferred_mode)) + b"\r\n")
    SerialPort.write(b"OK\r\n")
    SerialPort.write(b"")

def SIM_GetLocalIP():
    SerialPort.write(b"\r\n+CNACT:")
    SerialPort.write(b"OK\r\n")
    
def attend_command(at_command):
    match at_command:
        case "Probando":
            print("Received Probando")
            sleep(0.5)
            SerialPort.write(b"OK\r\n")

        case "OK":
            print("Test: OK")
            sleep(0.5)
            Test_Impl(0)
        
        case "ERROR":
            print("Test: ERROR")
            sleep(0.5)
            Test_Impl(1)
        
        case "OUTOFBOUNDS":
            print("Test: OUT OF BOUNDS")
            sleep(0.5)
            Test_Impl(2)

        case "AT":
            print("Test: AT")
            sleep(0.5)
            # Execute testing of the TestAT case
            Test_AT()
        
        case "AT+CPIN?":
            SIM_Status(status_counter)
            # Increase the counter
            status_counter += 1
            if(status_counter > 4):
                status_counter = 0

        case "AT+CNMP=2":
            SIM_SetNetworkMode(2)

        case "AT+CNMP=13":
            SIM_SetNetworkMode(13)

        case "AT+CNMP=38":
            SIM_SetNetworkMode(38)

        case "AT+CNMP?":
            SIM_GetNetworkMode()

        case "AT+CMNB=1":
            SIM_SetPreferredMode(1)

        case "AT+CMNB=2":
            SIM_SetPreferredMode(2)

        case "AT+CMNB=3":
            SIM_SetPreferredMode(3)
        
        case "AT+CMNB?":
            SIM_GetPreferredMode()

        case "AT+CNACT?":
            SIM_GetLocalIP()

        case _:
            SerialPort.write(b'ERROR\r\n')
            print("ERROR\r\n")
            print("Received command is not recognized or is incorrect")

while True:
    # This is the main loop of execution where the test will take place depending on the AT command received

    # Read the serial terminal until \n (command received) or timeout
    print("waiting for input")
    input = SerialPort.read_until(b'\r')
    if input:
        command = input.decode('utf-8', errors='ignore').strip('\r').strip('\n').strip('\0')
        if command:
            print("Received command: " + command)
            attend_command(str(command))
            

