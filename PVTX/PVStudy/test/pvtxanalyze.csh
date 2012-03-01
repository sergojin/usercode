#! /bin/tcsh 

## argv[1] == 1 
# prepare cfg files

## argv[1] == 2
# submit cmsRun jobs


## argv[1] == 3 
# run the pvtxplot root macro for each sample
# publish plots in given webpage

## argv[1] == 4 
# run comptrkpu.C macro 
# Compare the trk/vertex parameters with and without PU

## argv[1] == 5
# run the comptrkpu.C macro



### Configuration ############################################################
set samples=("wmunu" "zee")
#("ttbar" "ttbar_156BxPU" "minbias" "zmumu_pt_20_300_lowlumipileup")  
#("qcd_pt_80_120")
#("qcd_pt_30")


set version=v4
# according to the cut 

set GlobalTag="MC_31X_V3"
set Events=10000
set cfg="PVStudy_cfg.py"

set cfgfiledir="work/cfgfiles/$version"
set outputdir="work/ntuple/$version"
set logdir="work/log/$version"

mkdir -p $cfgfiledir
mkdir -p $outputdir
mkdir -p $logdir


### Cut on the TrackSplitterProducer ########################
# minPvtxZsep: min zsep between secondary/leading pvtx  
# trkFilterZcut: zsep signficance of track dz w.r.t. leading pixelpvtx z

if ($version == "v3") then 
  set trkFilterZcut="999" 
  set minPvtxZsep="0"
endif

if ($version == "v4") then  
  set trkFilterZcut="10"
 set minPvtxZsep="0"    
endif 

if ($version == "v5") then
  set trkFilterZcut="5"
 set minPvtxZsep="0"    
endif 

if ($version == "v6") then
  set trkFilterZcut="10"
  set minPvtxZsep="0.5" # 5mm
endif 


if ($version == "v7") then
  set trkFilterZcut="10"
  set minPvtxZsep="0.15" # 1.5mm
endif   

### Cut on the Two-vertices pair method ######################## 
set ntrkdiffcut="0.1"


set macrodir="work/macro/$version"
set pngdir="/uscms_data/d2/ygao/pvtx/pngfiles/$version"
set epsdir="/uscms_data/d2/ygao/pvtx/epsfiles/$version"
set publishdir = "/afs/fnal.gov/files/home/room2/ygao/public_html/CMS/Tracking/pvtxstudy_$version"

mkdir -p $macrodir
mkdir -p $publishdir
mkdir -p $pngdir
mkdir -p $epsdir

set ifpublish="true"



##############################################################################
if ($1 == 1) then

echo "you chose option 1: prepare cfg files"

mkdir -p $cfgfiledir

foreach sample($samples)
   # set different cuts for different samples
    switch ($sample)
    case "ttbar":
 	set ntrkmin="10"
	set ntrkmax="100"
        set dataset="TTbar_Summer09_MC_31X_V3"
    breaksw
    case "ttbar_156BxPU":
 	set ntrkmin="10"
	set ntrkmax="100"
        set dataset="TTbar_Summer09_MC_31X_V3_156BxLumiPileUp"
    breaksw
    case "minbias":
 	set ntrkmin="0"
	set ntrkmax="50"        
	set dataset="MinBias_Summer09_MC_31X_V3"
    breaksw

    case "qcd_pt_30":
 	set ntrkmin="0"
	set ntrkmax="50"        
        set dataset="QCD_Pt_30_Summer09_MC_31X_V3"
    breaksw

    case "zmumu_pt_20_300*":
 	set ntrkmin="0"
	set ntrkmax="50"        
        set dataset="RelValZmumuJets_Pt_20_300_GEN_LowLumiPileUp_v1"
    breaksw  

    case "zee":
        set ntrkmin="0" 
        set ntrkmax="50"
        set dataset="Zee_Summer09_MC_31X_V3"
    breaksw
    
     case "wmunu":
        set ntrkmin="0"
        set ntrkmax="50"
        set dataset="Wmunu_Summer09_MC_31X_V3"                                                                                                                                     
    breaksw     


    default:
    set ntrkmin="10" 
    set ntrkmax="100" 
    set dataset=""
   endsw

   if(! -e ../python/${dataset}_cff.py) then 
      echo "Missing "$sample"_cff.py, skipping this sample..."
       continue  
    endif

    cat $cfg | sed \
      -e s@DATASET@$dataset@g \
      -e s@NEVENT@$Events@g \
      -e s@GLOBALTAG@$GlobalTag@g \
      -e s@SAMPLE@$sample@g \
      -e s@MINPVTXZSEP@$minPvtxZsep@g \
      -e s@TRKFILTERZCUT@$trkFilterZcut@g \
      -e s@NTRKMIN@$ntrkmin@g \
      -e s@NTRKMAX@$ntrkmax@g \
      -e s@NTRKDIFFCUT@$ntrkdiffcut@g \
      -e s@OUTPUTDIR@$outputdir@g \
      > ! $cfgfiledir/{$sample}_cfg.py
end


##############################################################################
else if ($1 == 2) then
echo "you chose option 2:  submit cmsRun jobs"
mkdir -p  $logdir
mkdir -p  $outputdir

eval `scramv1 run -csh`

foreach sample($samples)

  echo "submitting job for " $sample

  cmsRun $cfgfiledir/{$sample}_cfg.py >& ! $logdir/$sample.log < /dev/zero &

end
##############################################################################


##############################################################################
else if ($1 == 3) then

echo "you chose option 3:  run pvtxplot.C"
mkdir -p $macrodir

foreach sample($samples)

   echo "run the pvtxplot root macro for " $sample 

   mkdir -p $pngdir/$sample
   mkdir -p $epsdir/$sample

  cat pvtxplot.C | sed \
      -e s@SAMPLE@$sample@g \
      -e s@ROOTDIR@$outputdir@g \
      -e s@PNGDIR@$pngdir@g \
      -e s@EPSDIR@$epsdir@g \
      > ! $macrodir/$sample.C

  root -b -q -l $macrodir/$sample.C > ! $logdir/macro.$sample.log

  if ($ifpublish == "true") then  

     mkdir -p $publishdir
     mkdir -p $publishdir/$sample

     echo "publish pvtxplot in the web for " $sample

     rm -f $publishdir/$sample/*.png

     cp $pngdir/$sample/*.png $publishdir/$sample

     cp  convertplot.py   $publishdir/$sample

     $publishdir/$sample/convertplot.py $publishdir/$sample
  endif
end


else if ($1 == 4) then

echo "you chose option 4:  run comptrkpu.C"

set singlepvtxsample="ttbar"
set pusample="ttbar_156BxPU"

set comppngdir="$pngdir/comp_trk_pu_$singlepvtxsample"
mkdir -p $comppngdir

set compepsdir="$epsdir/comp_trk_pu_$singlepvtxsample"
mkdir -p $compepsdir


cat comptrkpu.C | sed \
      -e s@COMPPUPVTXSAMPLE@comp$singlepvtxsample@g \
      -e s@SINGLEPVTXSAMPLE@$singlepvtxsample@g \
      -e s@PUSAMPLE@$pusample@g \
      -e s@ROOTDIR@$outputdir@g \
      -e s@PNGDIR@$comppngdir@g \
      -e s@EPSDIR@$compepsdir@g \
      > ! $macrodir/comp$singlepvtxsample.C

 root -b -q -l $macrodir/comp$singlepvtxsample.C  > ! $logdir/macro.comp$singlepvtxsample.log


 if ($ifpublish == "true") then  

     mkdir -p $publishdir/comp_trk_pu_$singlepvtxsample

     echo "publish pvtxplot in the web for " $singlepvtxsample

     rm -f $publishdir/comp_trk_pu_$singlepvtxsample/*.png

     cp $comppngdir/*.png $publishdir/comp_trk_pu_$singlepvtxsample

     cp  convertplot.py  $publishdir/comp_trk_pu_$singlepvtxsample/ 

     $publishdir/comp_trk_pu_$singlepvtxsample/convertplot.py  $publishdir/comp_trk_pu_$singlepvtxsample/
  
endif


endif

