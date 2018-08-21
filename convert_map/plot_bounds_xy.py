#!/usr/bin/python

import matplotlib.pyplot as plt 
import numpy as np 
import sys

# #############################
import matplotlib.pylab as pylab
params = {'legend.fontsize': 'x-large',
          'figure.figsize': (10, 8),
         'axes.labelsize': 'x-large',
         'axes.titlesize':'x-large',
         'xtick.labelsize':'x-large',
         'ytick.labelsize':'x-large'}
pylab.rcParams.update(params)
# #############################


filename1 = sys.argv[1]
# filename1 = "point.txt"
dat1 = np.loadtxt(filename1)

length = dat1.shape[0]

print("length= {}".format(length) )
for i in xrange(length):
	x1 = dat1[i,0]
	y1 = dat1[i,1]
	x2 = dat1[i,3]
	y2 = dat1[i,4]
	plt.plot([x1,x2], [y1,y2] , linewidth= 3)
# plt.legend()
# plt.xlabel(" Time [second] ")
# plt.ylabel(" Acceleration [m/s^2] ")
# plt.grid()
# plt.savefig(filename1 + ".jpg")
plt.show()


