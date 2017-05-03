# -*- coding: utf-8 -*-
import pylab
import numpy as np
import math
import random
import scipy.stats as sp
import matplotlib.cm as cm
import matplotlib.pyplot as plt
import sys

def calcAngle(ax, ay, az):
        pitch = math.atan2(-ax, math.sqrt(ay*ay+az*az))
        pitch *= 180.0/math.pi
        return pitch

# Generic Filter
# Takes in a differential limit (last v current)
# Takes in a Lower Bound
# Takes up an upper bound
# Takes in the average
def correctValue(currentValue, lastValue, differential, lowerBound, upperBound, average, errorMargin):
        differentialCorrection = 0
        lowerBoundCorrection = 0
        upperBoundCorrection = 0
        errorMarginCorrection = 0
        if (abs(currentValue - lastValue) > differential):
                currentValue = average
                differentialCorrection += 1

        # if (currentValue < lowerBound):
        #         currentValue = average
        #         lowerBoundCorrection += 1

        if (currentValue > upperBound):
                currentValue = upperBound
                upperBoundCorrection += 1

        if (abs(currentValue - average) > errorMargin):
                currentValue = average
                errorMarginCorrection += 1



        return currentValue

def lowPassFilter(currentValue, average, fraction):
        averagedValue = (1 - 1.0/fraction) * average + (1.0/fraction) * currentValue
        return averagedValue


def generateAverages(startingAverage, values, weighting, differential, lowerBound, upperBound, errorMargin, subtractNorm):
        average = startingAverage
        averageValues = []

        # Check corrections
        differentialCorrection = 0
        lowerBoundCorrection = 0
        upperBoundCorrection = 0
        errorMarginCorrection = 0


        for i in range(0, len(values)):
                currentValue = values[i]
                lastValue = currentValue
                if (i > 0):
                        lastValue = values[i-1]
                # correctedValue = correctValue(currentValue, lastValue, differential, lowerBound, upperBound, average, errorMargin)
                
                if (abs(currentValue - lastValue) > differential):
                        currentValue = average
                        differentialCorrection += 1

                # if (abs(currentValue-subtractNorm) < lowerBound):
                #         currentValue = average
                #         lowerBoundCorrection += 1

                if (abs(currentValue-subtractNorm) > upperBound):
                        currentValue = upperBound
                        upperBoundCorrection += 1

                if (abs(currentValue - average) > errorMargin):
                        if (currentValue - average) > 0:
                            currentValue  = average + errorMargin
                        else:
                            currentValue = average- errorMargin
                        errorMarginCorrection += 1

                average = lowPassFilter(currentValue, average, weighting)
                averageValues.append(average)


        print("DIFFERENTIAL CORRECTION: " + str(differentialCorrection))
        print("LOWERBOUND CORRECTION: " + str(lowerBoundCorrection))
        print("UPPERBOUND CORRECTION: " + str(upperBoundCorrection))
        print("ERRORMARGIN CORRECTION: " + str(errorMarginCorrection))
        return averageValues

def generateAveragesAngle(startingAverage, values, weighting, differential, lowerBound, upperBound, errorMargin, subtractNorm):
        average = startingAverage
        averageValues = []

        # Check corrections
        differentialCorrection = 0
        lowerBoundCorrection = 0
        upperBoundCorrection = 0
        errorMarginCorrection = 0


        for i in range(0, len(values)):
                currentValue = values[i]
                lastValue = currentValue
                if (i > 0):
                        lastValue = values[i-1]
                # correctedValue = correctValue(currentValue, lastValue, differential, lowerBound, upperBound, average, errorMargin)
                
                if (abs(currentValue - lastValue) > differential):
                        currentValue = average
                        differentialCorrection += 1

                # if (abs(currentValue-subtractNorm) < lowerBound):
                #         currentValue = average
                #         lowerBoundCorrection += 1

                if (abs(currentValue-subtractNorm) > upperBound):
                        currentValue = upperBound
                        upperBoundCorrection += 1

                if (abs(currentValue - average) > errorMargin):
                        if (currentValue - average) > 0:
                            currentValue  = average + errorMargin
                        else:
                            currentValue = average- errorMargin
                        errorMarginCorrection += 1

                average = lowPassFilter(currentValue, average, weighting)
                averageValues.append(average)


        print("DIFFERENTIAL CORRECTION: " + str(differentialCorrection))
        print("LOWERBOUND CORRECTION: " + str(lowerBoundCorrection))
        print("UPPERBOUND CORRECTION: " + str(upperBoundCorrection))
        print("ERRORMARGIN CORRECTION: " + str(errorMarginCorrection))
        return averageValues

def generateAveragesX(startingAverage, values, weighting, differentialUP, differentialDown, upperBound, errorMargin, subtractNorm):
        average = startingAverage
        averageValues = []

        # Check corrections
        differentialCorrection = 0
        lowerBoundCorrection = 0
        upperBoundCorrection = 0
        errorMarginCorrection = 0


        for i in range(0, len(values)):
                currentValue = values[i]
                lastValue = currentValue
                if (i > 0):
                        lastValue = values[i-1]
                # correctedValue = correctValue(currentValue, lastValue, differential, lowerBound, upperBound, average, errorMargin)
                
                if (abs(currentValue - lastValue) > differentialUP):
                        if (currentValue - lastValue) > 0:
                            currentValue = average
                            differentialCorrection += 1
                elif (abs(currentValue - lastValue) > differentialDown):
                        if (currentValue - lastValue) < 0:
                            currentValue = average
                            differentialCorrection += 1

                # if (abs(currentValue-subtractNorm) < lowerBound):
                #         currentValue = average
                #         lowerBoundCorrection += 1

                if (abs(currentValue-subtractNorm) > upperBound):
                        currentValue = upperBound
                        upperBoundCorrection += 1

                if (abs(currentValue - average) > errorMargin):
                        currentValue = average
                        errorMarginCorrection += 1

                average = lowPassFilter(currentValue, average, weighting)
                averageValues.append(average)


        print("DIFFERENTIAL CORRECTION: " + str(differentialCorrection))
        print("LOWERBOUND CORRECTION: " + str(lowerBoundCorrection))
        print("UPPERBOUND CORRECTION: " + str(upperBoundCorrection))
        print("ERRORMARGIN CORRECTION: " + str(errorMarginCorrection))
        return averageValues

def generateAveragesV2(startingAverage, values, weighting, differential, differentialScaling, lowerBound, upperBound, errorMargin, subtractNorm):
        average = startingAverage
        averageValues = []

        # Check corrections
        differentialCorrection = 0
        lowerBoundCorrection = 0
        upperBoundCorrection = 0
        errorMarginCorrection = 0


        for i in range(0, len(values)):
                currentValue = values[i]
                lastValue = currentValue
                if (i > 0):
                        lastValue = values[i-1]
                # correctedValue = correctValue(currentValue, lastValue, differential, lowerBound, upperBound, average, errorMargin)
                
                if (abs(currentValue - lastValue) > differential):
                        lastValue += (currentValue-lastValue) * differentialScaling
                        differentialCorrection += 1

                # if (abs(currentValue-subtractNorm) < lowerBound):
                #         currentValue = startingAverage
                #         lowerBoundCorrection += 1

                if (abs(currentValue-subtractNorm) > upperBound):
                        currentValue = upperBound
                        upperBoundCorrection += 1

                if (abs(currentValue - average) > errorMargin):
                        if (currentValue - average) > 0:
                            currentValue  = average + errorMargin
                        else:
                            currentValue = average- errorMargin
                        errorMarginCorrection += 1

                average = lowPassFilter(currentValue, average, weighting)
                averageValues.append(average)


        print("DIFFERENTIAL CORRECTION: " + str(differentialCorrection))
        print("LOWERBOUND CORRECTION: " + str(lowerBoundCorrection))
        print("UPPERBOUND CORRECTION: " + str(upperBoundCorrection))
        print("ERRORMARGIN CORRECTION: " + str(errorMarginCorrection))
        return averageValues







def main(argv):

        xWeighting = 5
        differentialUPX = .1
        differentialDownX = .015
        errorMarginX = .1
        upperBoundX = .5
        subtractNormX = 0

        # left right turn
        axValues = []
        axAverage = []
        # Barrel Roll
        ayValues = []
        ayAverage = []
        # Incline Decline
        azValues = []
        azAverage = []

        #Barrel Roll
        gyValues = []
        gyAverage = []
        # Incline/Decline
        gzValues = []
        gzAverage = []

        angleValues = []
        angleAverage = []

        backwardWeights = []
        forwardWeights = []

        xNumber = []



        i = 0
        # Grab the values
        with open("finalFlatground1.txt") as f:
                for line in f:
                        if (len(line) > 1):
                                splitLine = line.split(",")
                                if (splitLine[0] == "ANGLE"):
                                        xNumber.append(i)
                                        i += 1
                                        axValues.append(float(splitLine[1]) * 0.000732)
                                        ayValues.append(float(splitLine[2]) * 0.000732)
                                        azValues.append(float(splitLine[3]) * 0.000732)
                                        gyValues.append(float(splitLine[4]))
                                        gzValues.append(float(splitLine[5]))
                                        angleValues.append(float(splitLine[6])/15)
                                if (splitLine[0] == "WEIGHTS"):
                                    backwardWeights.append(float(splitLine[1])/20)
                                    forwardWeights.append(float(splitLine[2])/20)

        # Calculate the averages
       
        # def generateAverages(average, values, weighting, differential, lowerBound, upperBound, errorMargin, subtractNorm):
        # def generateAveragesV2(startingAverage, values, weighting, differential, differentialScaling, lowerBound, upperBound, errorMargin, subtractNorm):

        totalAcceleration = []
        totalAccelerationAvg = []
        for i in range(0, len(axValues)):
            ax = axValues[i]
            ay = ayValues[i]
            az = azValues[i]
            totalAcceleration.append(math.sqrt(az*az+ay*ay+az*az))


        azAverage = generateAveragesV2(1.2, azValues, 15, .5, .5, .1, 3, .5, 1.2)
        # azAverage = generateAverages(1.2, azValues, 10, 100, 0, 100, 100, 1.2)
        # axAverage = generateAverages(0, axValues, 10, 100, 0, 100, 100, 0)
        print("AX FIXES")
        # axAverage = generateAverages(0, axValues, 10, .015, 0, .5, .5, 0)
        # def generateAveragesX(startingAverage, values, weighting, differentialUP, differentialDown, upperBound, errorMargin, subtractNorm):



        axAverage = generateAveragesX(0, axValues, xWeighting, differentialUPX, differentialDownX, upperBoundX, errorMarginX, 0)
       
        print
        ayAverage = generateAverages(.5, ayValues, 10, 100, 0, 100, 100, .5)
        backwardWeightAvg = generateAverages(80, backwardWeights, 10, 50, 0, 200, 200, 80)



        calcRawAngleValues = []
        for i in range(0, len(ayAverage)):
                angle = calcAngle(axValues[i], ayValues[i], azValues[i])
                calcRawAngleValues.append(-(angle+1))

        calcAvgAngleValues = []
        for i in range(0, len(ayAverage)):
                angle = calcAngle(axAverage[i], ayAverage[i], azAverage[i])
                calcAvgAngleValues.append(-(angle+1))

        # calcAvgAngleValues = generateAveragesV2(0, calcAvgAngleValues, 15, )

        print("ANGLE AVERAGE")
        angleAverage = generateAveragesAngle(0, calcAvgAngleValues, 5, 1.5, .5, 10, 1.5, 0)
        totalAccelerationAvg = generateAverages(1.2, totalAcceleration, 10, 2, .1, 5, 1, 1)


        for x in range(0, len(angleAverage)):
            print(str(axAverage[x]) + ", " + str(ayAverage[x]) + ", " + str(azAverage[x])  + ", " + str(calcAvgAngleValues[x]) + ", " + str(angleAverage[x]))

        # Calculate RMSE FOR VARIOUS ACCOUNTS
        rmseRawAngle = 0
        for i in range(0, len(calcAvgAngleValues)):
                rmseRawAngle += calcAvgAngleValues[i]
        print("FLAT GROUND")
        print
        print("AVG ANGLE RMSE: " + str(rmseRawAngle/len(calcAvgAngleValues)))
        plt.clf() #Clear the current figure (prevents multiple labels)

        plt.figure(0)

        legend = plt.legend(loc='upper right', shadow=True, fontsize='small')

                # PLOT THE SHIZ
                #Q1a
        

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

        plt.xlabel('Time', fontdict=labelfont)
        plt.title('G readings Flat Ground', fontdict=titlefont) 
        plt.ylabel('G', fontdict=labelfont)
        axes = plt.gca()
        axes.set_xlim([0, len(xNumber)])            # x-axis bounds
        axes.set_ylim([-6, 6])    

        plt.plot(np.array(xNumber), np.array(axAverage),                             
                'green',                       # colour
                linestyle='--',                    # line style
                linewidth=1, label='Ax Avg')      # plot label

        plt.plot(np.array(xNumber), np.array(ayAverage),                             
                'purple',                       # colour
                linestyle='--',                    # line style
                linewidth=1, label='Ay Avg')      # plot label

        plt.plot(np.array(xNumber), np.array(azAverage),                             
            'black',                       # colour
            linestyle='--',                    # line style
            linewidth=1, label='Az Avg')      # plot label

        plt.plot(np.array(xNumber), np.array(totalAccelerationAvg),                             
            'red',                       # colour
            linestyle='--',                    # line style
            linewidth=2, label='Accel Avg')      # plot label

        plt.plot(np.array(xNumber), np.array(angleAverage),                             
                'orange',                       # colour
                linestyle='--',                    # line style
                linewidth=1, label='Avg Angle Readings')      # plot label




# ANGLE VALUES






                # x-axis bounds


        # DO IT FOR DOWNHILL

         # left right turn
        axValues = []
        axAverage = []
        # Barrel Roll
        ayValues = []
        ayAverage = []
        # Incline Decline
        azValues = []
        azAverage = []

        #Barrel Roll
        gyValues = []
        gyAverage = []
        # Incline/Decline
        gzValues = []
        gzAverage = []

        angleValues = []
        angleAverage = []

        backwardWeights = []
        forwardWeights = []

        xNumber = []



        i = 0
        # Grab the values
        with open("finalDownhill4.txt") as f:
                for line in f:
                        if (len(line) > 1):
                                splitLine = line.split(",")
                                if (splitLine[0] == "ANGLE"):
                                        xNumber.append(i)
                                        i += 1
                                        axValues.append(float(splitLine[1]) * 0.000732)
                                        ayValues.append(float(splitLine[2]) * 0.000732)
                                        azValues.append(float(splitLine[3]) * 0.000732)
                                        gyValues.append(float(splitLine[4]))
                                        gzValues.append(float(splitLine[5]))
                                        angleValues.append(float(splitLine[6])/15)
                                if (splitLine[0] == "WEIGHTS"):
                                    backwardWeights.append(float(splitLine[1])/20)
                                    forwardWeights.append(float(splitLine[2])/20)

        # Calculate the averages
       
        # def generateAverages(average, values, weighting, differential, lowerBound, upperBound, errorMargin, subtractNorm):
        # def generateAveragesV2(startingAverage, values, weighting, differential, differentialScaling, lowerBound, upperBound, errorMargin, subtractNorm):

        totalAcceleration = []
        totalAccelerationAvg = []
        for i in range(0, len(axValues)):
            ax = axValues[i]
            ay = ayValues[i]
            az = azValues[i]
            totalAcceleration.append(math.sqrt(az*az+ay*ay+az*az))


        azAverage = generateAveragesV2(1.2, azValues, 15, .5, .5, .1, 3, .5, 1.2)
        # azAverage = generateAverages(1.2, azValues, 10, 100, 0, 100, 100, 1.2)
        # axAverage = generateAverages(0, axValues, 10, 100, 0, 100, 100, 0)
        print("AX FIXES")
        # axAverage = generateAverages(0, axValues, 10, .015, 0, .5, .5, 0)
        # def generateAveragesX(startingAverage, values, weighting, differentialUP, differentialDown, upperBound, errorMargin, subtractNorm):

        axAverage = generateAveragesX(0, axValues, xWeighting, differentialUPX, differentialDownX, upperBoundX, errorMarginX, 0)
       
        print
        ayAverage = generateAverages(.5, ayValues, 10, 100, 0, 100, 100, .5)
        backwardWeightAvg = generateAverages(80, backwardWeights, 10, 50, 0, 200, 200, 80)



        calcRawAngleValues = []
        for i in range(0, len(ayAverage)):
                angle = calcAngle(axValues[i], ayValues[i], azValues[i])
                calcRawAngleValues.append(-(angle+1))

        calcAvgAngleValues = []
        for i in range(0, len(ayAverage)):
                angle = calcAngle(axAverage[i], ayAverage[i], azAverage[i])
                calcAvgAngleValues.append(-(angle+1))

        # calcAvgAngleValues = generateAveragesV2(0, calcAvgAngleValues, 15, )

        print("ANGLE AVERAGE")
        angleAverage = generateAveragesAngle(0, calcAvgAngleValues, 5, 1.5, .5, 10, 1.5, 0)
        totalAccelerationAvg = generateAverages(1.2, totalAcceleration, 10, 2, .1, 5, 1, 1)


        for x in range(0, len(angleAverage)):
            print(str(axAverage[x]) + ", " + str(ayAverage[x]) + ", " + str(azAverage[x])  + ", " + str(calcAvgAngleValues[x]) + ", " + str(angleAverage[x]))

        # Calculate RMSE FOR VARIOUS ACCOUNTS
        rmseRawAngle = 0
        for i in range(0, len(calcAvgAngleValues)):
                rmseRawAngle += calcAvgAngleValues[i]
        print("DOWNHILL")
        print("AVG ANGLE RMSE: " + str(rmseRawAngle/len(calcAvgAngleValues)))

        plt.figure(1)

        plt.plot(np.array(xNumber), np.array(axAverage),                             
                'green',                       # colour
                linestyle='--',                    # line style
                linewidth=1, label='Ax Avg')      # plot label

        plt.plot(np.array(xNumber), np.array(ayAverage),                             
                'purple',                       # colour
                linestyle='--',                    # line style
                linewidth=1, label='Ay Avg')      # plot label

        plt.plot(np.array(xNumber), np.array(azAverage),                             
            'black',                       # colour
            linestyle='--',                    # line style
            linewidth=1, label='Az Avg')      # plot label

        plt.plot(np.array(xNumber), np.array(totalAccelerationAvg),                             
            'red',                       # colour
            linestyle='--',                    # line style
            linewidth=2, label='Accel Avg')      # plot label

        plt.plot(np.array(xNumber), np.array(angleAverage),                             
                'orange',                       # colour
                linestyle='--',                    # line style
                linewidth=1, label='Avg Angle Readings')      # plot label




# # ANGLE VALUES




        plt.xlabel('Time', fontdict=labelfont)
        plt.title('G readings Downhill', fontdict=titlefont) 
        plt.ylabel('G', fontdict=labelfont)
        axes = plt.gca()
        axes.set_xlim([0, len(xNumber)])            # x-axis bounds
        axes.set_ylim([-8, 8])    

                # x-axis bounds
        legend = plt.legend(loc='upper right', shadow=True, fontsize='small')


        # DO IT FOR UPHILL

         # left right turn
        axValues = []
        axAverage = []
        # Barrel Roll
        ayValues = []
        ayAverage = []
        # Incline Decline
        azValues = []
        azAverage = []

        #Barrel Roll
        gyValues = []
        gyAverage = []
        # Incline/Decline
        gzValues = []
        gzAverage = []

        angleValues = []
        angleAverage = []

        backwardWeights = []
        forwardWeights = []

        xNumber = []



        i = 0
        # Grab the values
        with open("finalUphill4.txt") as f:
                for line in f:
                        if (len(line) > 1):
                                splitLine = line.split(",")
                                if (splitLine[0] == "ANGLE"):
                                        xNumber.append(i)
                                        i += 1
                                        axValues.append(float(splitLine[1]) * 0.000732)
                                        ayValues.append(float(splitLine[2]) * 0.000732)
                                        azValues.append(float(splitLine[3]) * 0.000732)
                                        gyValues.append(float(splitLine[4]))
                                        gzValues.append(float(splitLine[5]))
                                        angleValues.append(float(splitLine[6])/15)
                                if (splitLine[0] == "WEIGHTS"):
                                    backwardWeights.append(float(splitLine[1])/20)
                                    forwardWeights.append(float(splitLine[2])/20)

        # Calculate the averages
       
        # def generateAverages(average, values, weighting, differential, lowerBound, upperBound, errorMargin, subtractNorm):
        # def generateAveragesV2(startingAverage, values, weighting, differential, differentialScaling, lowerBound, upperBound, errorMargin, subtractNorm):

        totalAcceleration = []
        totalAccelerationAvg = []
        for i in range(0, len(axValues)):
            ax = axValues[i]
            ay = ayValues[i]
            az = azValues[i]
            totalAcceleration.append(math.sqrt(az*az+ay*ay+az*az))


        azAverage = generateAveragesV2(1.2, azValues, 15, .5, .5, .1, 3, .5, 1.2)
        # azAverage = generateAverages(1.2, azValues, 10, 100, 0, 100, 100, 1.2)
        # axAverage = generateAverages(0, axValues, 10, 100, 0, 100, 100, 0)
        print("AX FIXES")
        # axAverage = generateAverages(0, axValues, 10, .015, 0, .5, .5, 0)
        # def generateAveragesX(startingAverage, values, weighting, differentialUP, differentialDown, upperBound, errorMargin, subtractNorm):

        axAverage = generateAveragesX(0, axValues, xWeighting, differentialUPX, differentialDownX, upperBoundX, errorMarginX, 0)
       
        print
        ayAverage = generateAverages(.5, ayValues, 10, 100, 0, 100, 100, .5)
        backwardWeightAvg = generateAverages(80, backwardWeights, 10, 50, 0, 200, 200, 80)



        calcRawAngleValues = []
        for i in range(0, len(ayAverage)):
                angle = calcAngle(axValues[i], ayValues[i], azValues[i])
                calcRawAngleValues.append(-(angle+1))

        calcAvgAngleValues = []
        for i in range(0, len(ayAverage)):
                angle = calcAngle(axAverage[i], ayAverage[i], azAverage[i])
                calcAvgAngleValues.append(-(angle+1))

        # calcAvgAngleValues = generateAveragesV2(0, calcAvgAngleValues, 15, )

        print("ANGLE AVERAGE")
        angleAverage = generateAveragesAngle(0, calcAvgAngleValues, 5, 1.5, .5, 10, 1.5, 0)
        totalAccelerationAvg = generateAverages(1.2, totalAcceleration, 10, 2, .1, 5, 1, 1)


        for x in range(0, len(angleAverage)):
            print(str(axAverage[x]) + ", " + str(ayAverage[x]) + ", " + str(azAverage[x])  + ", " + str(calcAvgAngleValues[x]) + ", " + str(angleAverage[x]))

        # Calculate RMSE FOR VARIOUS ACCOUNTS
        rmseRawAngle = 0
        for i in range(0, len(calcAvgAngleValues)):
                rmseRawAngle += calcAvgAngleValues[i]
        print("UPHILL")
        print("AVG ANGLE RMSE: " + str(rmseRawAngle/len(calcAvgAngleValues)))





        # # FIRST ONE

        min = 10000
        max = -10000
        for x in totalAcceleration:
                if x > max:
                        max = x
                if x < min:
                        min = x


# RAW ACCEL VALUES
        # plt.plot(np.array(xNumber), np.array(axValues),                             
        #         'green',                       # colour
        #         linestyle='--',                    # line style
        #         linewidth=1, label='Ax Values')      # plot label

        # plt.plot(np.array(xNumber), np.array(ayValues),                             
        #         'purple',                       # colour
        #         linestyle='--',                    # line style
        #         linewidth=1, label='Ay Values')      # plot label

        # plt.plot(np.array(xNumber), np.array(azValues),                             
        #     'black',                       # colour
        #     linestyle='--',                    # line style
        #     linewidth=1, label='Az Values')      # plot label

        # # plt.plot(np.array(xNumber), np.array(calcRawAngleValues),                             
        # #         'orange',                       # colour
        # #         linestyle='--',                    # line style
        # #         linewidth=1, label='Angle Readings')      # plot label

        # plt.plot(np.array(xNumber), np.array(totalAcceleration),                             
        #     'red',                       # colour
        #     linestyle='--',                    # line style
        #     linewidth=2, label='Accel Value')      # plot label

# AVG ACCEL VALUES
        plt.figure(2)

        plt.plot(np.array(xNumber), np.array(axAverage),                             
                'green',                       # colour
                linestyle='--',                    # line style
                linewidth=1, label='Ax Avg')      # plot label

        plt.plot(np.array(xNumber), np.array(ayAverage),                             
                'purple',                       # colour
                linestyle='--',                    # line style
                linewidth=1, label='Ay Avg')      # plot label

        plt.plot(np.array(xNumber), np.array(azAverage),                             
            'black',                       # colour
            linestyle='--',                    # line style
            linewidth=1, label='Az Avg')      # plot label

        plt.plot(np.array(xNumber), np.array(totalAccelerationAvg),                             
            'red',                       # colour
            linestyle='--',                    # line style
            linewidth=2, label='Accel Avg')      # plot label

        plt.plot(np.array(xNumber), np.array(angleAverage),                             
                'orange',                       # colour
                linestyle='--',                    # line style
                linewidth=1, label='Avg Angle Readings')      # plot label




# ANGLE VALUES




        plt.xlabel('Time', fontdict=labelfont)
        plt.title('G readings Uphill', fontdict=titlefont) 
        plt.ylabel('G', fontdict=labelfont)
        axes = plt.gca()
        axes.set_xlim([0, len(xNumber)])            # x-axis bounds
        axes.set_ylim([min-2, max+2])    

                # x-axis bounds
        legend = plt.legend(loc='upper right', shadow=True, fontsize='small')


        plt.show()



if __name__ == "__main__":
  main(sys.argv[1:])