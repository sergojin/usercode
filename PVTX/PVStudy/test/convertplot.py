#!/usr/bin/env python

import sys
import optparse
import commands
import os
import glob

#def insertPlot(base, findex):
    
    
    
    
#######################
# Get options
#######################

parser = optparse.OptionParser("usage: %prog [options]\
<input directory> \n")
parser.add_option ('--o', dest='outdir', type='string',
                   default = './',
                   help="directory for output png and eps")
parser.add_option ('--t', dest='title', type='string',
                   default = 'Primary Vertex Resolution Studies',
                   help="Title of html page")
options, args = parser.parse_args()

if len(args) != 1:
    print "Please specify input dir.  Exiting."
    sys.exit()

indir  = args[0]+"/"
outdir = options.outdir+"/"
title = options.title

##############################################
# Check dir
##############################################
if not os.path.isdir(indir) :
    print "Cannot find %s.  Exiting." % infile
    sys.exit()

if os.path.isfile(indir+"index.html") :
    print "%s already exists.  Exiting." % indir+"index.html"

files = glob.glob(indir+'*png')

for file in files:
    base = os.path.splitext(file)[0]
    print file, base
    command = "convert %s.png -resize 300x300 %s_small.png" % (base, base)
    os.system(command)

