#! /bin/tcsh 

set samples=("ttbar")
#("qcd_pt_80_120")
#("qcd_pt_30")
#("ttbar")
#("qcd_pt_30")
#("qcd_pt_80_120")
set ntupledir = "/uscms/home/ygao/pvtxstudy/CMSSW_3_1_2/src/UserCode/PVStudy/test/ntuple"
set pngdir="/uscms/home/ygao/pvtxstudy/CMSSW_3_1_2/src/UserCode/PVStudy/test/pngfiles/"
set epsdir="/uscms/home/ygao/pvtxstudy/CMSSW_3_1_2/src/UserCode/PVStudy/test/epsfiles/"
set publishdir = "/afs/fnal.gov/files/home/room2/ygao/public_html/CMS/Tracking/pvtxstudy"

mkdir -p $pngdir
mkdir -p $epsdir



foreach sample($samples)

    mkdir -p $pngdir/$samples
    mkdir -p $epsdir/$samples
       
    cat pvtxplot.C | sed \
      -e s@SAMPLE@$samples@g \
      -e s@ROOTDIR@$ntupledir@g \
      -e s@PNGDIR@$pngdir@g \
      -e s@EPSDIR@$epsdir@g \
      > ! macro/$samples.C

    root -b -q -l macro/$samples.C > ! macro/macro.$samples.log
    
    echo "publish png files in the web: " $sample
    rm -f $publishdir/$samples/*.png
    cp $pngdir/$samples/*.png $publishdir/$samples
    
    $publishdir/$samples/convertplot.py $publishdir/$samples

end


