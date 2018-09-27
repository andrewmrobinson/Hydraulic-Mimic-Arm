import serial

from streamplot import PlotManager
import numpy as np

serialArduino = serial.Serial('COM100', 115200)

	
plt_mgr = PlotManager(
		title="adc values", 
		nline=2)

while True:
	# while (serialArduino.inWaiting() == 0):
	#	pass
	valueRead = serialArduino.readline()
	valueRead = valueRead.split()
	# check if valid value can be casted
	try:
		adc0 = int(valueRead[0])
		adc1 = int(valueRead[1])
		adc2 = int(valueRead[2])
		adc3 = int(valueRead[3])
		pos0 = int(valueRead[4])
		pos1 = int(valueRead[5])
		pos2 = int(valueRead[6])
		pos3 = int(valueRead[7])
		print(adc1)
		if adc1 <= 4096:
			if adc1 >= 0:
				plt_mgr.add("ADC 0", adc0,pos0)
				plt_mgr.add("ADC 1", adc1,pos1)
				plt_mgr.add("ADC 2", adc2,pos2)
				plt_mgr.add("ADC 3", adc3,pos3)
				plt_mgr.update()
			else:
				print("Invalid! negative number\n")
		else:
			print("Invalid! too large\n")
	except ValueError:
		print("Invalid! cannot cast\n")
plt_mgr.close()