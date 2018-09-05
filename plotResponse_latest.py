import numpy as np
#import matplotlib
#matplotlib.use('Agg')

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
    if not list_of_files:                # I prefer using the negation
        return None                      # because it behaves like a shortcut
    latest_file = max(list_of_files, key=os.path.getctime)
    _, filename = os.path.split(latest_file)
    return latest_file
	
filename = get_latest_file(path,'')

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
plt.ylim(1000, 4095) 
plt.xlim(t[-1]-5000,t[-1])
fig.savefig("test.png")
plt.show()
