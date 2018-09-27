from streamplot import PlotManager
import numpy as np
import time

length = 100000
costs  = np.arange(length)

plt_mgr = PlotManager(
	title="plots", 
	nline=3)

for i in range(length):
	plt_mgr.add("time", time.time())
	plt_mgr.add("time2", time.time())
	plt_mgr.update()

plt_mgr.close()