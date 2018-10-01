import numpy as np
# import matplotlib
# matplotlib.use('Agg')

import matplotlib.pyplot as plt
import os
import re
import datetime
import glob

path = 'PC_project_arm/project_arm/responseData/*.txt'


def get_latest_file(path, *paths):
    """Returns the name of the latest (most recent) file 
    of the joined path(s)"""
    fullpath = os.path.join(path, *paths)
    list_of_files = glob.glob(path)  # You may use iglob in Python3
    if not list_of_files:  # I prefer using the negation
        return None  # because it behaves like a shortcut
    latest_file = max(list_of_files, key=os.path.getctime)
    _, filename = os.path.split(latest_file)
    return latest_file


filename = get_latest_file(path, '')

print(filename)

text_file = open(filename, "r")
lines = text_file.read().split('\n')
data = []
actual_0 = []
desired_0 = []
actual_1 = []
desired_1 = []
actual_2 = []
desired_2 = []
actual_3 = []
desired_3 = []
pid = []

lines.remove(lines[-1])

for n in lines:
	data.append(n.split())
	if len(data[-1])!=15:
		data.remove(data[-1])
	else:
		actual_0.append(int(data[-1][0]))
		desired_0.append(int(data[-1][4]))
		actual_1.append(int(data[-1][1]))
		desired_1.append(int(data[-1][5]))
		actual_2.append(int(data[-1][2]))
		desired_2.append(int(data[-1][6]))
		actual_3.append(int(data[-1][3]))
		desired_3.append(int(data[-1][7]))
		
text_file.close()

t = np.linspace(0.0, len(actual_0) - 1, len(actual_0))

fig, axes = plt.subplots(2,2)
axes[0,0].set_title("ADC 0")
axes[0,0].plot(t, actual_0)
axes[0,0].plot(t, desired_0)
axes[0,1].set_title("ADC 1")
axes[0,1].plot(t, actual_1)
axes[0,1].plot(t, desired_1)
axes[1,0].set_title("ADC 2")
axes[1,0].plot(t, actual_2)
axes[1,0].plot(t, desired_2)
axes[1,1].set_title("ADC 3")
axes[1,1].plot(t, actual_3)
axes[1,1].plot(t, desired_3)

#ctr = 0;
#for n in actual_0:
#	ctr += 1
#	if ctr % 100 == 1:
#		ax.annotate(pid[ctr], (t[ctr], actual_0[ctr]))
#		ax.annotate(pid[ctr], (t[ctr], actual_1[ctr]))

plt.ylim(1000, 4095)
axes[0,0].set_xlim(t[-1] - 1000, t[-1])
axes[1,0].set_xlim(t[-1] - 1000, t[-1])
axes[1,1].set_xlim(t[-1] - 1000, t[-1])
axes[0,1].set_xlim(t[-1] - 1000, t[-1])
fig.savefig(filename + ".png")
fig.suptitle(filename)
plt.show()
