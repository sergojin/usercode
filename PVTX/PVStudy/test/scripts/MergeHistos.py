#!/usr/bin/env python
#____________________________________________________________
#
#  Merge histo root files from different runs
#
# Geng-yuan Jeng
# Geng-yuan.Jeng@cern.ch
#
# Fermilab, 2009
#
#____________________________________________________________
import sys,os

def main():
    workdir=os.environ['PWD']
    ## create temp dir to store root files
    tmpDir = "/tmp/pvtxana/"
    os.system("mkdir -p "+tmpDir)

    fileHandle = open("JobsList.txt")
    lineList = fileHandle.readlines()
    for line in lineList:
        npos=0
        for i in line.split():
            npos+=1
            if npos == 1:
                d="Run"+str(i)+"/"
                if os.path.exists(d):
                    copycmd = "cp -rf "+d+"pvtxana_hist_Run*.root "+tmpDir
                    os.system(copycmd)
    ## Merge files
    haddcmd = "hadd "+workdir+"/pvtxana_hist_all.root "+tmpDir+"*.root"
##    print haddcmd
    os.system(haddcmd)
    fileHandle.close()
    os.system("rm -rf "+tmpDir)
#_________________________________    
if __name__ =='__main__':
    sys.exit(main())
