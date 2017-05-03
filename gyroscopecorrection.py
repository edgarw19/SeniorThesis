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
averageAngle = 0
averageGyroScope = 0

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

gyroFraction = 50

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
                                curAngle = float(splitLine[1])
                                averageGyroScope = averageGyroScope * (1-1/gyroFraction) + float(splitLine[4]) * 1/gyroFraction
                                averageAngle = averageAngle * 9.0/10 + curAngle * 1.0/10
                                currentAngle.append(float(splitLine[1]))
                                
                                totalAngleAverage.append(averageAngle)
                                gyroAverage.append(averageGyroScope)
                                gyroAngle.append(float(splitLine[4]))
                                rawAngle.append(float(splitLine[5]) - 2.45)
                        if (splitLine[0] == "WEIGHTS"):
                                backwardWeights.append(float(splitLine[1])/6)
                                forwardWeights.append(float(splitLine[2])/6)

for x in range(0, 10):
        rawAngleReadings = []
        avgAngleReadings = []
        averageAngle = 0
        multiplicativeValue = x/5.0
        for i in range(0, len(rawAngle)):
                curAngle = rawAngle[i] + gyroAverage[i] * multiplicativeValue
                rawAngleReadings.append(curAngle)
                averageAngle = averageAngle * 9.0/10 + curAngle * 1.0/10
                avgAngleReadings.append(averageAngle)


        rmseRawAngle = 0
        for i in range(0, len(rawAngleReadings)):
                rmseRawAngle += math.sqrt(rawAngleReadings[i]*rawAngleReadings[i])
        print("FRACTION IS: " + str(multiplicativeValue))
        print("RAW ANGLE RMSE: " + str(rmseRawAngle/len(currentAngle)))

        rmseAvgAngle = 0
        for i in range(0, len(avgAngleReadings)):
                rmseAvgAngle += math.sqrt(avgAngleReadings[i]*avgAngleReadings[i])
        print("AVERAGE ANGLE RMSE: " + str(rmseAvgAngle/len(currentAngle)))
        print

