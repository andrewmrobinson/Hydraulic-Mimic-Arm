import serial
import matplotlib.pyplot as plt
from drawnow import *

values = []

plt.ion()
cnt = 0

serialArduino = serial.Serial('COM100', 115200)


def plotValues():
    plt.title('Serial value from Arduino')
    plt.grid(True)
    plt.ylabel('Values')
    plt.plot(values, 'rx-', label='values')
    plt.legend(loc='upper right')


# pre-load dummy data
for i in range(0, 100):
    values.append(0)

while True:
    # while (serialArduino.inWaiting() == 0):
    #    pass
    valueRead = serialArduino.readline()
    valueRead = valueRead.split()
    # check if valid value can be casted
    try:
        valueInInt = int(valueRead[0])
        print(valueInInt)
        if valueInInt <= 4094:
            if valueInInt >= 0:
                values.append(valueInInt)
                values.pop(0)
                drawnow(plotValues)
            else:
                print("Invalid! negative number\n")
        else:
            print("Invalid! too large\n")
    except ValueError:
        print("Invalid! cannot cast\n")