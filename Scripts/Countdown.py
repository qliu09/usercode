#!/usr/bin/python

import os, time, sys, atexit, curses, traceback, select
from signal import signal, SIGTERM
from sys import exit
from sys import stdout

def Countdown(_time):
    TIME=float(_time)
    WholeString="\r\033[2K"
    HEADER = "   Next check will happen in "
    FLASH = False
    SLEEP = 0.1
    BAR = ['|', '-', '>', ':', '|']


    time_passed = bar_length = 0
    time_last = time.time()
    time_length = TIME
    
    done = False
    while True:
        bar_max_length = int(os.popen('stty size', 'r').read().split()[1])-12
        bar_max_length = int(0.5*bar_max_length)
        dbar = time_length/bar_max_length/SLEEP
        if time_length is 0:
            dbarl = bar_max_length - bar_length
        else:
            dbarl = bar_max_length/time_length*SLEEP
        
        secs = time_left = int(time_length - time_passed)
        mins = 0
        if time_left > 59:
            mins = int(time_left/60.0)
            secs = time_left % 60
        
        output = HEADER + ' '
        
        if mins <10:
                output += "0"+str(mins)+":"
        else:
                output += str(mins)+":"
        if secs <10:
                output += "0"+str(secs)
        else:
                output += str(secs)
        if time_length is not 0:
            percent = float(time_passed)/float(time_length)*100
        if percent > 100 or done:
            percent = 100
        output += '  '
        if   percent == 100: output += '('
        elif percent >=  10: output += '( '
        output += '%.2f%% ' % percent
        bar_length = percent * (bar_max_length/100.0)
        if bar_length > bar_max_length:
            bar_length = bar_max_length
        output = output+")  "+gen_progress_bar(BAR,bar_length, bar_max_length)
        WholeString+=str(output)
        
        time_delta = time.time() - time_last
        time_passed += float('%.4f' % time_delta)
        time_last = time.time()
        
        stdout.write(WholeString+"\r")
        stdout.flush()
        
        if done:
            break
        if time_passed >= time_length:
            done = True
            stdout.write("\r\033[2K Done !! \n")
            return
        
        else:
            WholeString="\r\033[2K"
            time.sleep(SLEEP)
    
    stdout.write("\r\033[2K \r")
    stdout.write(WholeString)
    stdout.flush()
    return

# generate the progress bar that will later be printed
def gen_progress_bar(BAR, bar_length, bar_max_length):
    output = BAR[0]
    for i in range(0, int(bar_length)):
        output += BAR[1]
    
    output += BAR[2]
    for i in range(abs(int(bar_length) - int(bar_max_length))):
        output += BAR[3]
    output += BAR[4]
    return output

        #if __name__ == "__main__":
#   Countdown(6)

