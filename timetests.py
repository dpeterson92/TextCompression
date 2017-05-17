from subprocess import call
import time
import os

processTimes = []
threadTimes = []
files = ["test1000.txt"]

for f in files:
  i=0
  while i<100:
    start=time.time()*1000
    call(["./compressT_LOLS",f,'5'])
    finish=time.time()*1000
    threadTimes.append(finish-start)
    
    start=time.time()*1000
    call(["./compressR_LOLS",f,'5'])
    finish=time.time()*1000
    processTimes.append(finish-start)
    
    i+=1
    
  
  averageProcessTime=0;  
  for time in processTimes:
    averageProcessTime+=time
  averageProcessTime = averageProcessTime/len(processTimes)
  print "Average Process time (in ms) "+str(averageProcessTime)
  
  
  averageThreadTime=0;  
  for time in threadTimes:
    averageThreadTime+=time
  averageThreadTime = averageThreadTime/len(threadTimes)
  print "Average Thread time (in ms) "+str(averageThreadTime)
  differential = averageProcessTime-averageThreadTime
  
  if differential<0:
    print "For file "+f+" processes were faster, by an average of "+str(differential)+" ms"
  elif differential ==0:
    print "For file "+f+" on average threads and processes were equal speeds"
  else:
    print "For file "+f+" threads were faster, by an average of "+(str(differential))+" ms"