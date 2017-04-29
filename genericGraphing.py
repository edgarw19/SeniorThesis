# -*- coding: utf-8 -*-
import pylab
import numpy as np
import math
import random
import scipy.stats as sp
import matplotlib.cm as cm
import matplotlib.pyplot as plt

current = []
totalAverage = []
lastNAverage = []
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
                        xNumber.append(i)
                        splitLine = line.split(",")
                        if printDutyRates:
                                if (splitLine[0] == "DUTY"):
                                        current.append(float(splitLine[1]))
                                        totalAverage.append(float(splitLine[2]))
                                        lastNAverage.append(float(splitLine[3]))
                        elif printAngles:
                                if (splitLine[0] == "ANGLE"):
                                        current.append(float(splitLine[1]))
                                        totalAverage.append(float(splitLine[2]))
                                        lastNAverage.append(float(splitLine[3]))
                        else:
                                if (splitLine[0] == "WEIGHTS"):
                                        backwardWeights.append(float(splitLine[1]))
                                        forwardWeights.append(float(splitLine[2]))
                        i += 1


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


if printDutyRates or printAngles:
                #Transmission Powers
        plt.plot(np.array(xNumber), np.array(current),                             
                'green',                       # colour
                linestyle='--',                    # line style
                linewidth=3, label='Current')      # plot label

        plt.plot(np.array(xNumber), np.array(totalAverage),                             
                'blue',                       # colour
                linestyle='--',                    # line style
                linewidth=3, label='Average')      # plot label

        plt.plot(np.array(xNumber), np.array(lastNAverage),                             
                'red',                       # colour
                linestyle='--',                    # line style
                linewidth=3, label='Last N Average')      # plot label

else:

                        #Transmission Powers
        plt.plot(np.array(xNumber), np.array(backwardWeights),                             
                'green',                       # colour
                linestyle='--',                    # line style
                linewidth=3, label='backward')      # plot label

        plt.plot(np.array(xNumber), np.array(forwardWeights),                             
                'blue',                       # colour
                linestyle='--',                    # line style
                linewidth=3, label='forward')      # plot label


        

axes = plt.gca()
        # x-axis bounds
legend = plt.legend(loc='upper right', shadow=True, fontsize='small')

if printDutyRates:
        plt.title('Duty Rates', fontdict=titlefont) 
        plt.ylabel('Duty Rate', fontdict=labelfont)
        axes.set_xlim([0, len(xNumber)])            # x-axis bounds
        axes.set_ylim([55, 105])    

elif printAngles:
        plt.title('Angles', fontdict=titlefont) 
        plt.ylabel('Angle', fontdict=labelfont)
        axes.set_xlim([0, len(xNumber)])            # x-axis bounds
        axes.set_ylim([-10, 10])    
else:
        plt.title('Weights', fontdict=titlefont) 
        plt.ylabel('Weights', fontdict=labelfont)
        axes.set_xlim([0, len(xNumber)])            # x-axis bounds
        axes.set_ylim([0, 175])    

plt.xlabel('Time', fontdict=labelfont)

plt.show()





