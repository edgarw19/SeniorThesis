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
gyroAverage = []
gyroAngle = []
rawAngle = []

derivForward = []
derivBackward = []
derivAngle = []

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
                                
                                
                                currentDutyRate.append(float(splitLine[1]))
                                totalDutyAverage.append(float(splitLine[2]))
                                lastNDutyAverage.append(float(splitLine[3]))
                                notCorrectedDutyRate.append(float(splitLine[4]))
                        if (splitLine[0] == "ANGLE"):
                                xNumber.append(i)
                                i += 1
                                currentAngle.append(float(splitLine[1]))
                                totalAngleAverage.append(float(splitLine[2])-2.45)
                                gyroAverage.append(float(splitLine[3]))
                                gyroAngle.append(float(splitLine[4]))
                                rawAngle.append(float(splitLine[5]))
                        if (splitLine[0] == "WEIGHTS"):
                                backwardWeights.append(float(splitLine[1])/6)
                                forwardWeights.append(float(splitLine[2])/6)

for i in range(0, len(currentAngle)):
        if (i < len(currentAngle) - 1):
                diff = currentAngle[i+1] - currentAngle[i]
                derivAngle.append(diff)

for i in range(0, len(backwardWeights)):
        if (i < len(backwardWeights) - 1):
                diff = backwardWeights[i+1] - backwardWeights[i]
                derivBackward.append(diff)

for i in range(0, len(forwardWeights)):
        if (i < len(forwardWeights) - 1):
                diff = forwardWeights[i+1] - forwardWeights[i]
                derivForward.append(diff)

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



plt.plot(np.array(xNumber), np.array(currentAngle),                             
        'green',                       # colour
        linestyle='--',                    # line style
        linewidth=1, label='Modified Angle')      # plot label

plt.plot(np.array(xNumber), np.array(rawAngle),                             
        'purple',                       # colour
        linestyle='--',                    # line style
        linewidth=1, label='Raw Angle')      # plot label

plt.plot(np.array(xNumber), np.array(totalAngleAverage),                             
        'blue',                       # colour
        linestyle='--',                    # line style
        linewidth=1, label='Average Angle')      # plot label

# plt.plot(np.array(xNumber), np.array(gyroAverage),                             
#         'red',                       # colour
#         linestyle='-',                    # line style
#         linewidth=2, label='Gyro Average')      # plot label

# plt.plot(np.array(xNumber), np.array(gyroAngle),                             
#         'black',                       # colour
#         linestyle='-',                    # line style
#         linewidth=2, label='Gyro Angle')      # plot label

plt.xlabel('Time', fontdict=labelfont)
plt.title('Angles', fontdict=titlefont) 
plt.ylabel('Angle Rate', fontdict=labelfont)
axes = plt.gca()
axes.set_xlim([0, len(xNumber)])            # x-axis bounds
axes.set_ylim([-10, 10])    

        # x-axis bounds
legend = plt.legend(loc='upper right', shadow=True, fontsize='small')





plt.show()





