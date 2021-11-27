import subprocess
import time
import os
from datetime import datetime

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def printmsg(text, color = ''):
	curtime = datetime.now().strftime("%H:%M")
	print("[{:s}] {:s}{:s}\033[0m".format(curtime, color, text))

def main():
	firstrun = False;	
	lasterrormsg = ''
	runningprc: subprocess.Popen[bytes] | str = ''
	outfile = open('out.txt', 'w')
	os.system('clear')
	printmsg("Starting compilation in watch mode", bcolors.OKCYAN)
	while 1:
		prc = subprocess.Popen(['make'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		stdout, stderr = prc.communicate()
		rcode = prc.poll()
		stdout = stdout.decode("utf-8")
		if stderr:
			stderr = stderr.decode("utf-8")
		if rcode != 0 and lasterrormsg != stderr:
			os.system('clear')
			printmsg("Compilation error", bcolors.FAIL)
			print("---\n"+stderr+"\n----")
			lasterrormsg = stderr
			if runningprc != '':
				runningprc.terminate()
		elif rcode == 0 and (stdout != "make[1]: Nothing to be done for `all'.\n" or firstrun == False):
			os.system('clear')
			printmsg("Compilation successful", bcolors.OKGREEN)
			firstrun = True
			if runningprc != '':
				runningprc.terminate()
			outfile.truncate(0)
			runningprc = subprocess.Popen(['./democheck', 'samples/wallhack1.dem'], stdout=outfile, stderr=outfile)
		time.sleep(5)

main()