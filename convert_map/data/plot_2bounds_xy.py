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
def GetLen(x1, x2, y1, y2):
	len_2 = (x2-x1)*(x2-x1) + (y2-y1) * (y2-y1)
	return np.sqrt(len_2) 

filename1 = sys.argv[1]
filename2 = sys.argv[2]
# filename1 = "point.txt"
dat1 = np.loadtxt(filename1)
dat2 = np.loadtxt(filename2)

length1 = dat1.shape[0]
length2 = dat2.shape[0]

print("length1= {}".format(length1) )
for i in xrange(length1):
	x1 = dat1[i,0]
	y1 = dat1[i,1]
	x2 = dat1[i,3]
	y2 = dat1[i,4]
	print("len={}".format(GetLen(x1,x2,y1,y2)) )
	plt.plot([x1,x2], [y1,y2] , linewidth= 3)

print("length2= {}".format(length2) )
for i in xrange(length2):
	x1 = dat2[i,0]
	y1 = dat2[i,1]
	x2 = dat2[i,3]
	y2 = dat2[i,4]
	print("len={}".format(GetLen(x1,x2,y1,y2)) )
	plt.plot([x1,x2], [y1,y2] , linewidth= 3)
# plt.legend()
# plt.xlabel(" Time [second] ")
# plt.ylabel(" Acceleration [m/s^2] ")
# plt.grid()
# plt.savefig(filename1 + ".jpg")
plt.show()


