# UHHNtupleConverter

Convert UHH ntuple to 3D fit ntuple. It is based on UHH framework. To install it follow instructions [here](https://github.com/UHH2/UHH2/wiki/Installing,-Compiling,-Ntuples-(RunII-2016,17,18-datasets-in-CMSSW_10_2_X-v2))). Then clone this repository to run the converter as below.

```
cd CMSSW_10_2_17/src/UHH2/
git clone https://github.com/Diboson3D/UHHNtupleConverter.git
cd UHHNtupleConverter
cmsenv
source ../../../../SFrame/setup.sh
make
```

To run one test:

```
sframe_main sframe_main config/test.xml
```

For large scale processing follow the instructions below which use the 2016 year as an example (change for the other years).

To submit condor jobs from lxplus:

```
./submit-all.sh submit 2016
```

To monitor the status (running, done, or failed) of the jobs:

```
./submit-all.sh check 2016
```

In case of failed jobs, it will be asked if you wish to resubmit.

To merge the job files:
 
```
./submit-all.sh merge 2016
```

nb, some of the 2017 MC files have buggy pileup profile as described [here](https://github.com/UHH2/UHH2/wiki/2017-MC-samples-with-buggy-pileup).
To run on them without a crash you would first need to link this folder in your local folder as:

```
ln -s /eos/cms/store/cmst3/group/exovv/VVtuple/FullRun2VVVHNtuple/deepAK8V2/Pileup_QCD_PtBinned/ Pileup_QCD_PtBinned
```

