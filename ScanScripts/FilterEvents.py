#!/usr/bin/env python
import os, sys, glob, re, shutil

def hasLepton(lines):
	for line in lines:
		if (line.find(' 11 ') > -1 or line.find(' -11 ') > -1 or
			line.find(' 13 ') > -1 or line.find(' -13 ') > -1 or
			line.find(' 15 ') > -1 or line.find(' -15 ') > -1):
			return True
	return False

def whichModel(model, oldModels):
	i = 0
	for oldModel in oldModels:
		if model == oldModel:
			return i
		i = i + 1
	return -1
def grabModelTag(lines):
	for line in lines:
		if line.find("# model ") > -1:
			modelTag = re.findall("# model [0-9a-zA-Z_.]{1,25}", line)[0]
	return modelTag
def main():
	fileName = sys.argv[1]
	NEventsPerPoint = sys.argv[2]

	
	
	shutil.move(fileName,fileName+"_orig")

	
	oldFile = open(fileName+"_orig", 'r')
	newFile = open(fileName, 'w')
	logFile = open(fileName.replace('.lhe', '.log'), 'w')

	modelNames = []
	numberWithModel = []
	numberWithLepton = []
	inEvent = False

	while True:
		line = oldFile.readline()
		if not line: break

		if line.find("<event") > -1:
			eventLines = ["<event>"]

			while not (line.find("</event") > -1):
				line = oldFile.readline()
				eventLines.append(line)

			modelTag = grabModelTag(eventLines)
			modelNumber =  whichModel(modelTag, modelNames)

			if modelNumber == -1:
				modelNames.append(modelTag)
				numberWithModel.append(0)
				numberWithLepton.append(0)
			modelNumber = whichModel(modelTag, modelNames)
			numberWithModel[modelNumber] = numberWithModel[modelNumber] + 1
			if hasLepton(eventLines):
				numberWithLepton[modelNumber] = numberWithLepton[modelNumber] + 1
				if numberWithLepton[modelNumber] < int(NEventsPerPoint) + 1:
					for eventLine in eventLines:
						newFile.write(eventLine)
		else:
			newFile.write(line)

	logFile.write("Model       Number of Events         Number of events with a lepton")
	i=-0
	for model in modelNames:
		logFile.write(model + ' ' + str(numberWithModel[i]) + ' ' +
					  str(numberWithLepton[i]) + '\n')
		i = i + 1

	oldFile.close()
	newFile.close()

#	os.system('mv temp.lhe ' + fileName + '.filtered')
	
if __name__ == "__main__":
	# Someone is launching this directly
	main()
