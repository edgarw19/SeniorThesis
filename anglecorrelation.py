# -*- coding: utf-8 -*-
import pylab
import numpy as np
import math
import random
import scipy.stats as sp
import matplotlib.cm as cm
import matplotlib.pyplot as plt

currentAngle = []
totalAngleAverage = []
lastNAngleAverage = []
notCorrectedAngle = []

currentDutyRate = []
totalDutyAverage = []
lastNDutyAverage = []
notCorrectedDutyRate = []

xNumber = []
forwardWeights = []
backwardWeights = []
printWeights = False
printAngles = False
printDutyRates = False

i = 0
# Add values to the training bag
with open("test.txt") as f:
        for line in f:
                if (len(line) > 1):
                        splitLine = line.split(",")
                        if (splitLine[0] == "DUTY"):
                                xNumber.append(i)
                                i += 1
                                currentDutyRate.append(float(splitLine[1]))
                                totalDutyAverage.append(float(splitLine[2]))
                                lastNDutyAverage.append(float(splitLine[3]))
                                notCorrectedDutyRate.append(float(splitLine[4]))
                        if (splitLine[0] == "ANGLE"):
                                currentAngle.append(float(splitLine[1]))
                                totalAngleAverage.append(float(splitLine[2]))
                                lastNAngleAverage.append(float(splitLine[3]))
                        if (splitLine[0] == "WEIGHTS"):
                                backwardWeights.append(float(splitLine[1])/20)
                                forwardWeights.append(float(splitLine[2])/20)


#Q1a
plt.clf() #Clear the current figure (prevents multiple labels)

labelfont = {
        'family' : 'sans-serif',  # (cursive, fantasy, monospace, serif)
        'color'  : 'black',       # html hex or colour name
        'weight' : 'normal',      # (normal, bold, bolder, lighter)
        'size'   : 14,            # default value:12
        }

titlefont = {
        'family' : 'serif',
        'color'  : 'black',
        'weight' : 'bold',
        'size'   : 16,
        }


# FIRST ONE
plt.figure(0)



plt.plot(np.array(xNumber), np.array(currentDutyRate),                             
        'green',                       # colour
        linestyle='--',                    # line style
        linewidth=3, label='Current')      # plot label

plt.plot(np.array(xNumber), np.array(totalDutyAverage),                             
        'blue',                       # colour
        linestyle='--',                    # line style
        linewidth=3, label='Average')      # plot label

plt.plot(np.array(xNumber), np.array(lastNDutyAverage),                             
        'red',                       # colour
        linestyle='--',                    # line style
        linewidth=3, label='Last N Average')      # plot label

plt.plot(np.array(xNumber), np.array(notCorrectedDutyRate),                             
        'black',                       # colour
        linestyle='--',                    # line style
        linewidth=3, label='Uncorrected')      # plot label

plt.xlabel('Time', fontdict=labelfont)
plt.title('Duty Rates', fontdict=titlefont) 
plt.ylabel('Duty Rate', fontdict=labelfont)
axes = plt.gca()
axes.set_xlim([0, len(xNumber)])            # x-axis bounds
axes.set_ylim([55, 105])    

        # x-axis bounds
legend = plt.legend(loc='upper right', shadow=True, fontsize='small')


# SECOND ONE
plt.figure(1)



# plt.plot(np.array(xNumber), np.array(currentAngle),                             
#         'green',                       # colour
#         linestyle='--',                    # line style
#         linewidth=3, label='Current')      # plot label

plt.plot(np.array(xNumber), np.array(totalAngleAverage),                             
        'blue',                       # colour
        linestyle='--',                    # line style
        linewidth=3, label='Average')      # plot label

# plt.plot(np.array(xNumber), np.array(lastNAngleAverage),                             
#         'red',                       # colour
#         linestyle='--',                    # line style
#         linewidth=3, label='Last N Average')      # plot label

plt.plot(np.array(xNumber), np.array(notCorrectedAngle),                             
        'black',                       # colour
        linestyle='--',                    # line style
        linewidth=3, label='Uncorrected Angle')      # plot label


# plt.plot(np.array(xNumber), np.array(backwardWeights),                             
#         'purple',                       # colour
#         linestyle=':',                    # line style
#         linewidth=8, label='backward')      # plot label

plt.plot(np.array(xNumber), np.array(forwardWeights),                             
        'orange',                       # colour
        linestyle=':',                    # line style
        linewidth=8, label='forward')      # plot label

plt.xlabel('Time', fontdict=labelfont)
plt.title('Angles', fontdict=titlefont) 
plt.ylabel('Angle', fontdict=labelfont)
axes = plt.gca()
axes.set_xlim([0, len(xNumber)])            # x-axis bounds
axes.set_ylim([-10, 10])   


        # x-axis bounds
legend = plt.legend(loc='upper right', shadow=True, fontsize='small')


# THIRD ONE

plt.figure(2)


                #Transmission Powers
plt.plot(np.array(xNumber), np.array(backwardWeights),                             
        'green',                       # colour
        linestyle='--',                    # line style
        linewidth=3, label='backward')      # plot label

plt.plot(np.array(xNumber), np.array(forwardWeights),                             
        'blue',                       # colour
        linestyle='--',                    # line style
        linewidth=3, label='forward')      # plot label

plt.xlabel('Time', fontdict=labelfont)
plt.title('Weights', fontdict=titlefont) 
plt.ylabel('Weights', fontdict=labelfont)
axes = plt.gca()
axes.set_xlim([0, len(xNumber)])            # x-axis bounds
axes.set_ylim([0, 175])    



        # x-axis bounds
legend = plt.legend(loc='upper right', shadow=True, fontsize='small')


plt.show()





