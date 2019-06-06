import pickle, optparse, os
import ROOT
from ROOT import *

parser = optparse.OptionParser()
parser.add_option("--indir","--indir",dest="indir",default='/eos/cms/store/cmst3/group/exovv/VVtuple/FullRun2VVVHNtuple/2016_new/QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/',help="Directory with job files to hadd")
parser.add_option("--outdir","--outdir",dest="outdir",default='/eos/cms/store/cmst3/group/exovv/VVtuple/FullRun2VVVHNtuple/2016_new/',help="Directory to put the final merged file")
parser.add_option("--outfile","--outfile",dest="outfile",default='QCD_HT200to300.root',help="Final merged file")
(options,args) = parser.parse_args()

if os.path.isfile(options.outdir+"/"+options.outfile): 
 print "Final merged file",options.outdir+"/"+options.outfile,"already exist! Removing it..."
 os.system('rm %s'%(options.outdir+"/"+options.outfile))
 
#hadd_cmd = 'hadd -f %s %s/*.root'%(options.outdir+"/"+options.outfile,options.indir)
hadd_cmd = 'hadd -f %s '%(options.outdir+"/"+options.outfile)

nfiles = len(os.listdir(options.indir))
for i,f in enumerate(os.listdir(options.indir)):
 if i%50 == 0: print "File",i+1,"of",nfiles
 tf = ROOT.TFile.Open(options.indir+"/"+f)
 tree = tf.AnalysisTree
 if tree.GetEntries() != 0:
  hadd_cmd += options.indir+"/"+f+" "
 tf.Close()
 
#print hadd_cmd
os.system(hadd_cmd)

infileName = options.outdir+"/"+options.outfile
outfileName = infileName.replace('.root','.pck')

infile = ROOT.TFile.Open(infileName,'READ')
ref_hist = infile.Get("NoCuts/genEvents")
totalEvents = ref_hist.GetBinContent(2)
print "Total generated events:",totalEvents
print "Events with weight != 1",ref_hist.GetBinContent(1)
print "Sum = ",totalEvents+ref_hist.GetBinContent(1)
print "Entries = ",ref_hist.GetEntries()
print "If sum != entries --> some events have negative weights! Cross check with the input file!"
tree = infile.AnalysisTree
print tree.GetEntries()
if tree.GetEntries() != 0: 
 tree.GetEntry(0)
 xsec = tree.xsec
else:
 print "The tree is empty!"
 xsec = 1.0
 
print "Xsec = ",xsec,"pb"
weight = 1./totalEvents
print "Weight = ",weight

data1 = {'sigma': 1.0,
         'events': totalEvents,
         'weight': weight}

selfref_list = [1, 2, 3]
selfref_list.append(selfref_list)

if os.path.isfile(outfileName):
 print "Pck file",outfileName,"already exist! Removing it..."
 os.system('rm %s'%(outfileName))
 
output = open(outfileName, 'wb')

pickle.dump(data1, output)


output.close()
infile.Close()

