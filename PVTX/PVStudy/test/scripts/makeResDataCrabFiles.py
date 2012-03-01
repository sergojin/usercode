#! /usr/bin/env python

import string
import commands, os, re
import sys 
                   

cmsswSkelFile = 'pvres_Data_HLT_cfg.py'
dataSet = ''
lumimask = ''
nLumis = -1
nLumisPerJob = 25
histName = 'histo.root'
storageElement = 'T2_US_UCSD'
mode='glite'
report_every = 1000;
global_tag_flag = '';
crabdir = '/uscms_data/d1/ygao/pvtx7TeV/'
ptRange = '0'
hlttrigger = 'HLT_L1_BscMinBiasOR_BptxPlusORMinus'

def makeCrabConfig():
    outFileName = dataSet.split('/')[1]+'_'+dataSet.split('/')[2]
    if(str(ptRange) == '0'):
        outFileName = outFileName + '_fullRange';
    if(str(ptRange) != '0'):
        outFileName = outFileName + '_ptRange' + ptRange;
    outFile = open(outFileName + '.cfg', 'w')
    print 'Writing CRAB config file: ' + outFileName + '.cfg'
    outFile.write('[CRAB]\n')
    outFile.write('jobtype   = cmssw\n')
    outFile.write('scheduler = ' + mode + '\n')
    outFile.write('\n[CMSSW]\n')
    outFile.write('datasetpath             = ' + dataSet + '\n')
    outFile.write('pset                    = ' + outFileName + '_cfg.py \n')
    outFile.write('lumi_mask               = ' + lumimask + '\n')
    outFile.write('total_number_of_lumis   = ' + str(nLumis) + '\n')
    outFile.write('lumis_per_job           = ' + str(nLumisPerJob) + '\n')
    outFile.write('output_file             = ' + histName + '\n\n\n')
    outFile.write('[USER]\n')
    outFile.write('return_data             = 1\n')
    outFile.write('copy_data               = 0\n')
    outFile.write('ui_working_dir          = ' + crabdir + outFileName + '\n')
    outFile.write('publish_data            = 0\n')
    outFile.write('[GRID]\n')
    outFile.write('maxtarballsize = 20\n')
    outFile.write('##here are some default sites that we \n')
    outFile.write('##run at. Comment/Uncomment at will\n')
    outFile.write('##UCSD \n')
    outFile.write('#SE_white_list = T2_US_UCSD\n')
    outFile.write('##WISC\n')
    outFile.write('#SE_white_list = T2_US_Wisconsin\n')
    outFile.write('##DESY\n')
    outFile.write('#SE_white_list = T2_DE_DESY\n')
    outFile.write('##Purdue\n')
    outFile.write('#SE_white_list = T2_US_Purdue\n')
    outFile.write('##MIT\n')
    outFile.write('#SE_white_list = T2_US_MIT\n')
    outFile.write('##Nebraska\n')
    outFile.write('#SE_white_list = T2_US_Nebraska\n')
    outFile.write('##IFCA\n')
    outFile.write('#SE_white_list = T2_ES_IFCA\n')
    outFile.write('##Lyon\n')
    outFile.write('#SE_white_list = T2_FR_CCIN2P3\n')
    outFile.write('##CIEMAT\n')
    outFile.write('#SE_white_list = T2_ES_CIEMAT\n')
    outFile.write('##IIHE\n')
    outFile.write('#SE_white_list = T2_BE_IIHE\n')
    outFile.write('##Aachen\n')
    outFile.write('#SE_white_list = T2_DE_RWTH\n')
#
def makeCMSSWConfig(cmsswSkelFile):
    foundOutNtupleFile = False
    foundreportEvery = False
    inFile = open(cmsswSkelFile, 'r').read().split('\n')
    ptmin = 0.0
    ptmax = 999.0
    if(str(ptRange) == '0'):
        outFileName = dataSet.split('/')[1]+'_'+dataSet.split('/')[2] + '_fullRange_cfg.py'
        ptmin = 0.0;
        ptmax = 999.0;
    if(str(ptRange) == '1'):
        outFileName = dataSet.split('/')[1]+'_'+dataSet.split('/')[2] + '_ptRange1_cfg.py'
        ptmin = 0.0;
        ptmax = 0.6;
    if(str(ptRange) == '2'):
        outFileName = dataSet.split('/')[1]+'_'+dataSet.split('/')[2] + '_ptRange2_cfg.py'
        ptmin = 0.6;
        ptmax = 1.2;
    if(str(ptRange) == '3'):
        outFileName = dataSet.split('/')[1]+'_'+dataSet.split('/')[2] + '_ptRange3_cfg.py'
        ptmin = 1.2;
        ptmax = 999.0;
                                                                                                                        
    print 'Writing CMS2 CMSSW python config file : ' + outFileName
    outFile = open(outFileName, 'w')
    for i in inFile:

        if i.find('reportEvery') != -1:
            outFile.write('process.MessageLogger.cerr.FwkReport.reportEvery = ' + str(report_every) + '\n'); continue

        if i.find('globaltag') != -1:
            outFile.write('process.GlobalTag.globaltag = "' + global_tag + '"\n'); continue
        
        if i.find('process.ana.avgTrkPtInPVMin') != -1:
            outFile.write('process.ana.avgTrkPtInPVMin = ' + str(ptmin) + '\n'); continue
            
        if i.find('process.ana.avgTrkPtInPVMax') != -1:
            outFile.write('process.ana.avgTrkPtInPVMax = ' + str(ptmax) + '\n'); continue
            
        if i.find('HLTPaths = ') != -1:
            outFile.write('\t\t\t\t  HLTPaths = cms.vstring( "' + hlttrigger + '"), \n'); continue

        outFile.write(i+'\n')
        
    outFile.close()
       


if len(sys.argv) < 5 :
    print 'Usage: makeCrabFiles.py [OPTIONS]'
    print '\nWhere the required options are: '
    print '\t-d\t\tname of dataset'
    print '\t-l\t\tLumiMask file'
    print '\nOptional arguments:'
    print '\t-cfg\tname of the skeleton CMSSW config file '
    print '\t-nLumis\t\tTotal number of lumis you want to run on. Default is -1'
    print '\t-lumisPerJob\tNumber of lumis per job. Default is 15'
    print '\t-m\t\tsubmission mode (possible: condor_g, condor, glite). Default is glite'
    print '\t-gtag\t\tglobal tag. Default is one from DBS'
    print '\t-hlt\t\tHLTPath Default is HLT_L1_BscMinBiasOR_BptxPlusORMinus'
    print '\t-ptRange\t\tptRange. Default is 0 corresponding to no pT cut'
    sys.exit()


for i in range(0, len(sys.argv)):
    if sys.argv[i] == '-cfg':
        cmsswSkelFile = sys.argv[i+1]
    if sys.argv[i] == '-d':
        dataSet = sys.argv[i+1]
    if sys.argv[i] == '-m':
        mode = sys.argv[i+1]
    if sys.argv[i] == '-l':
        lumimask = str(sys.argv[i+1])
    if sys.argv[i] == '-nLumis':
        nLumis = sys.argv[i+1]
    if sys.argv[i] == '-lumisPerJob':
        nLumisPerJob = sys.argv[i+1]
    if sys.argv[i] == '-gtag':
        global_tag_flag = str(sys.argv[i+1])
    if sys.argv[i] == '-ptRange':
        ptRange  = sys.argv[i+1]
    if sys.argv[i] == '-hlt':
        hlttrigger  = sys.argv[i+1]

if os.path.exists(cmsswSkelFile) == False:
    print 'CMSSW skeleton file does not exist. Exiting'
    sys.exit()


#print '\nGetting global tag from DBS...'
if( global_tag_flag != '' ):
	print '\nUsing \'' + global_tag_flag + '\' specified by -gtag flag.\n'
	global_tag = global_tag_flag
else :
    global_tag = '';
    dbs_result = '';
    command = 'dbsql find config.name,config.content where dataset=' + dataSet + '>config.content; while read line; do globaltag=`echo $line | sed -n \'s/^.*process.GlobalTag.globaltag = \([^p]*\).*$/\\1/p\'`; if [ "$globaltag" != "" ]; then echo $globaltag; break; fi; done <config.content; rm config.content';
    lines = os.popen(command);
    for i in lines.readlines():
      dbs_result = re.sub('\n', '', i)
      global_tag = re.sub('#.+$', '', dbs_result)
      if( global_tag != '' and global_tag_flag == ''):
          print '\nDBS Query results:\t\'' + dbs_result + '\' ?\n'
          print 'Use global tag from DBS:\t\'' + global_tag + '\' ?\n'
          answer = raw_input('[y/n]?')
          while(answer != 'y' and answer != 'n'): 
              print 'Please pick either \'y\' or \'n\''
              answer = raw_input('[y/n]?')
          if answer == 'n':
              print 'Enter alternative Global Tag:'
              global_tag = raw_input('new global tag:')
                  
          if( global_tag == '' and global_tag_flag == '' ):
              print '\nGlobal tag not found in DBS. Use -gtag to set global tag. Exiting...\n'
              sys.exit()
makeCMSSWConfig(cmsswSkelFile)
makeCrabConfig()    

