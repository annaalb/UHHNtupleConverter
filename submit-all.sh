#!/bin/bash

TODO=$1
YEAR=$2
EXEC=run-all.sh

SAMPLES=(ZprimeWW WprimeWZ RadionWW RadionZZ BulkGWW BulkGZZ VBF_ZprimeWW VBF_WprimeWZ VBF_RadionZZ VBF_RadionWW VBF_BulkGZZ 
         VBF_BulkGWW ZprimeZH ZprimeZHinc WprimeWH WprimeWHinc VBF_WprimeWH VBF_WprimeWHinc VBF_ZprimeZH VBF_ZprimeZHinc 
         herwig pythia1 pythia2 pythia3 pythia4 pythia5 pythia6 pythia7 pythia8 pythia9 pythia10
         madgraph1 madgraph2 madgraph3 madgraph4 madgraph5 madgraph6 madgraph7 madgraph8
         TT1 TT2 ST1 ST2 WW WWTo4Q WZ ZZ WmH WpH ZH
         JetHT1 JetHT2 JetHT3 JetHT4) 
         
if [ $YEAR = "2016" ]; then SAMPLES+=(ZZTo4Q) ; fi
if [ $YEAR = "2017" -o $YEAR = "2018" ]; then SAMPLES+=(WJets1 WJets2 WJets3 ZJets1 ZJets2 ZJets3 TThadr) ; fi
if [ $YEAR = "2016" -o $YEAR = "2017" ]; then SAMPLES+=(JetHT5) ; fi
if [ $YEAR = "2016" ]; then SAMPLES+=(JetHT6 JetHT7) ; fi

for s in "${SAMPLES[@]}"
do
	./$EXEC $TODO $s $YEAR
done
