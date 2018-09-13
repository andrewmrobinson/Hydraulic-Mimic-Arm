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
actual_0 = []
desired_0 = []
actual_1 = []
desired_1 = []
pid = []

lines.remove(lines[-1])

for n in lines:
	data.append(n.split())
	if len(data[-1])!=9:
		data.remove(data[-1])
	else:
		actual_0.append(float(data[-1][0]))
		desired_0.append(float(data[-1][7]))
		actual_1.append(float(data[-1][2]))
		desired_1.append(float(data[-1][8]))
		pid.append(data[-1][4] + " " + data[-1][5] + " " + data[-1][6])


text_file.close()

t = np.linspace(0.0, len(actual_0) - 1, len(actual_0))

fig, ax = plt.subplots()
ax.plot(t, actual_0)
ax.plot(t, desired_0)
ax.plot(t, actual_1)
ax.plot(t, desired_1)

#ctr = 0;
#for n in actual_0:
#	ctr += 1
#	if ctr % 100 == 0:
#		ax.annotate(pid[ctr], (t[ctr], actual_0[ctr]))
#		ax.annotate(pid[ctr], (t[ctr], actual_1[ctr]))

plt.ylim(1000, 4095)
plt.xlim(t[-1] - 5000, t[-1])
fig.savefig(filename + ".png")
plt.title(filename)
plt.show()

