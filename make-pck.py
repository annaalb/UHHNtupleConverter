import pickle, optparse, os
import ROOT
from ROOT import *

parser = optparse.OptionParser()
parser.add_option("--indir","--indir",dest="indir",default='/eos/cms/store/cmst3/group/exovv/VVtuple/FullRun2VVVHNtuple/2016_new/QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/',help="Directory with job files to hadd")
parser.add_option("--outdir","--outdir",dest="outdir",default='/eos/cms/store/cmst3/group/exovv/VVtuple/FullRun2VVVHNtuple/2016_new/',help="Directory to put the final merged file")
parser.add_option("--outfile","--outfile",dest="outfile",default='QCD_HT200to300.root',help="Final merged file")
parser.add_option("-d", "--data", dest="isdata", action="store_true", help="If is data do not use genEvents histo")
(options,args) = parser.parse_args()

if os.path.isfile(options.outdir+"/"+options.outfile): 
 print "Final merged file",options.outdir+"/"+options.outfile,"already exist! Removing it..."
 os.system('rm %s'%(options.outdir+"/"+options.outfile))

hadd_cmd = 'hadd -f %s %s/*.root'%(options.outdir+"/"+options.outfile.replace('.root','_tmp.root'),options.indir)
os.system(hadd_cmd)
infileName = options.outdir+"/"+options.outfile.replace('.root','_tmp.root')
infile = ROOT.TFile.Open(infileName,'READ')
ref_hist = infile.Get("NoCuts/genEvents")
totalEvents = ref_hist.GetBinContent(2)+ref_hist.GetBinContent(3)
underflowEntries = eventsWithNegWeight = ref_hist.GetBinContent(0)
eventsWithFractionalWeight = ref_hist.GetBinContent(1)
eventsWithWeightOneToTwo = ref_hist.GetBinContent(2)
overflowEntries = eventsWithWeightAboveTwo = ref_hist.GetBinContent(3)
totalEntries = ref_hist.GetEntries()
infile.Close()
os.system('rm %s'%infileName)

hadd_cmd = 'hadd -f %s '%(options.outdir+"/"+options.outfile)

counter = 0
counter2 = 0
counter3 = 0
nfiles = len(os.listdir(options.indir))
for i,f in enumerate(os.listdir(options.indir)):
 if i%50 == 0: print "File",i+1,"of",nfiles
 tf = ROOT.TFile.Open(options.indir+"/"+f)
 tree = tf.AnalysisTree
 hist = tf.Get("NoCuts/genEvents")
 counter+=hist.GetBinContent(2)
 counter2+=hist.GetEntries()
 counter3+=hist.Integral()
 if hist.GetEntries() != hist.Integral(): print "WHAT A FUCK!",f
 if tree.GetEntries() != 0:
  hadd_cmd += options.indir+"/"+f+" "
 else: print "YOOOOO",f 
 tf.Close()

print counter,counter2,counter3 
#print hadd_cmd
os.system(hadd_cmd)
 
infileName = options.outdir+"/"+options.outfile
outfileName = infileName.replace('.root','.pck')

infile = ROOT.TFile.Open(infileName,'READ')
print "Total generated events:",totalEvents
print "Events with weight < 0:",eventsWithNegWeight,"(underflow); 0 < weight < 1:",eventsWithFractionalWeight,"(bin=1); 1 < weight < 2:",eventsWithWeightOneToTwo,"(bin=2); weight > 1:",eventsWithWeightAboveTwo,"(overflow)"
print "Sum = ",totalEvents+eventsWithNegWeight+eventsWithFractionalWeight
print "Entries = ",totalEntries
#print "If sum != entries --> some events have negative weights! Cross check with the input file!"
tree = infile.AnalysisTree
print "The output tree has",tree.GetEntries(),"entries"
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

answer = raw_input('Do you want to remove the job folder? (YES or NO) ')
if answer == 'YES':
 os.system('rm -rf %s'%options.indir)
