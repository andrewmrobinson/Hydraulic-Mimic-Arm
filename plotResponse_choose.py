import numpy as np
#import matplotlib
#matplotlib.use('Agg')

import matplotlib.pyplot as plt
import os
import re
import datetime
import glob
import easygui

filename = easygui.fileopenbox(msg="Choose Data Set",default="PC_project_arm/project_arm/responseData/*.txt")

print(filename)
	
text_file = open(filename, "r")
lines = text_file.read().split('\n')
data = []
actual = []
desired = []

lines.remove(lines[-1])

for n in lines:
	data.append(n.split('\t'))
	if len(data[-1][2].split('|'))>1:
		data[-1][2] = data[-1][2].split('|')[1]
	else:
		data.remove(data[-1])
	actual.append(int(data[-1][0]))
	desired.append(int(data[-1][2]))
		
	
#print(len(lines))
text_file.close()


t = np.linspace(0.0, len(actual)-1, len(actual))

fig, ax = plt.subplots()
ax.plot(t, actual)
ax.plot(t, desired)
plt.ylim(0, 4095) 
plt.xlim(0,2500)
plt.title("filename")
fig.savefig("test.png")
plt.show()
