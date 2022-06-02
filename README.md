# UHHNtupleConverter

Convert UHH ntuple to 3D fit ntuple. It is based on UHH framework. To install it follow instructions [here](https://github.com/UHH2/UHH2/wiki/Installing,-Compiling,-Ntuples-(RunII-2016,17,18-datasets-in-CMSSW_10_2_X-v2))). Then clone this repository to run the converter as below.

```
cd CMSSW_10_2_17/src/UHH2/
git clone -b slim-ntuple https://github.com/jngadiub/UHHNtupleConverter.git
cd UHHNtupleConverter
cmsenv
source ../../../../SFrame/setup.sh
make
```

To run one test:

```
sframe_main config/test.xml
```

To submit condor jobs from lxplus for one single sample you must first add its name and folder to a txt file (see for example [samples-signals-2017-private.txt](https://github.com/jngadiub/UHHNtupleConverter/blob/slim-ntuple/samples-signals-2017-private.txt)).
Then you can submit jobs for one sample at the time as in this example:

```
python submit-jobs.py --sample test_ALPS_signal --nfiles_per_job 2 --outdir ./ --list samples-signals-2017-private.txt --xml config/config-2017.xml --queue longlunch
```

For large scale processing of a long list of samples follow the instructions below which use the 2016 year as an example (replace for the other years).

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

