import pickle, optparse, os, sys
import ROOT
from ROOT import *

parser = optparse.OptionParser()
parser.add_option("--indir","--indir",dest="indir",default='/eos/cms/store/cmst3/group/exovv/VVtuple/FullRun2VVVHNtuple/2016_new/QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/',help="Directory with job files to hadd")
parser.add_option("--outdir","--outdir",dest="outdir",default='/eos/cms/store/cmst3/group/exovv/VVtuple/FullRun2VVVHNtuple/2016_new/',help="Directory to put the final merged file")
parser.add_option("--outfile","--outfile",dest="outfile",default='QCD_HT200to300.root',help="Final merged file")
parser.add_option("--merge_ext","--merge_ext",dest="merge_ext", action="store_true", help="Merge nominal+ext",default=False)
parser.add_option("--LO_weights","--LO_weights",dest="LO_weights", action="store_true", help="Merge nominal+ext",default=False)
(options,args) = parser.parse_args()

if os.path.isfile(options.outdir+"/"+options.outfile): 
 print "Final merged file",options.outdir+"/"+options.outfile,"already exist!"
 answer = raw_input('Do you want to remove it? (YES or NO) ')
 if answer=='YES': os.system('rm %s'%(options.outdir+"/"+options.outfile))
 
if options.merge_ext:

 outfile = options.outdir+"/"+options.outfile
 outfile_pck = outfile.replace('.root','.pck')

 nominal = outfile.replace('.root','_nominal.root')
 ext = outfile.replace('.root','_ext.root')
 cmd = 'hadd -f {outfile} {nominal} {ext}'.format(outfile=outfile,nominal=nominal,ext=ext)
 print cmd
 answer = raw_input('Do you want to execute the command above? ')
 if answer=='YES': os.system(cmd)
 
 fpck=open(outfile.replace('.root','_nominal.pck'))
 dpck=pickle.load(fpck)
 nominalEvts = float(dpck['events']) 
 fpck=open(outfile.replace('.root','_ext.pck'))
 dpck=pickle.load(fpck)
 extEvts = float(dpck['events']) 
 
 totalEvents=nominalEvts+extEvts
 weight = 1./totalEvents
 print "Total generated events",totalEvents,"Weight = ",weight

 data1 = {'sigma': 1.0,
         'events': totalEvents,
         'weight': weight}

 selfref_list = [1, 2, 3]
 selfref_list.append(selfref_list)

 if os.path.isfile(outfile_pck):
  print "Pck file",outfile_pck,"already exist! Removing it..."
  os.system('rm %s'%(outfile_pck))
 
 output = open(outfile_pck, 'wb')
 pickle.dump(data1, output)
 output.close() 
 
 answer = raw_input('Do you want to remove nominal+ext files? ')
 if answer=='YES':
  cmd = 'rm {nominal}.* {ext}.*'.format(nominal=nominal.replace('.root',''),ext=ext.replace('.root',''))
  os.system(cmd) 
 
 sys.exit() 
 
totalEvents = 0
totalGenEvents = 0

sample = options.indir.split('/')[-1]
i = 1
for d in os.listdir('./'):
 if not sample in d: continue
 if not 'VBF' in sample and 'VBF' in d: continue
 if not 'ext' in sample and 'ext' in d: continue
 #jobid = int(d.split('-')[-1])
 #if jobid <= 42: continue
 i+=1
 found = False
 for f in os.listdir(d):
  if '.out' in f:
   print f,d,
   for l in open(d+'/'+f,'r').readlines():
    if l.find('Total processed events') != -1:
     totalEvents+=float(l.split(' ')[-1].replace('\n',''))
     found = True
     print float(l.split(' ')[-1].replace('\n','')),
    if (options.LO_weights==False and l.find('Total generated events') != -1) or (options.LO_weights==True and l.find('Total generated events') != -1 and 'LO' in l):
     totalGenEvents+=float(l.split(' ')[-1].replace('\n',''))
     found = True
     print float(l.split(' ')[-1].replace('\n',''))
     break
 if not found:
  print "The job output file not found for directory",d
  sys.exit()

print totalEvents,totalGenEvents

'''
hadd_cmd = 'hadd -f %s '%(options.outdir+"/"+options.outfile)

nfiles = len(os.listdir(options.indir))
for i,f in enumerate(os.listdir(options.indir)):
 #jobid = int(f.split('-')[-1].replace('.root',''))
 #if jobid <= 42: continue
 #print f
 if i%50 == 0: print "File",i+1,"of",nfiles
 tf = ROOT.TFile.Open(options.indir+"/"+f)
 tree = tf.AnalysisTree
 if tree.GetEntries() != 0:
  hadd_cmd += options.indir+"/"+f+" "
 else: print "Found 0 entries for file",f
 tf.Close()

os.system(hadd_cmd)
'''

nfiles = len(os.listdir(options.indir))
ch = ROOT.TChain("AnalysisTree")
for i,f in enumerate(os.listdir(options.indir)):
 if i%50 == 0: print "File",i+1,"of",nfiles
 ch.Add(options.indir+"/"+f);
print "NOW MERGE..." 
ch.Merge(options.outdir+"/"+options.outfile)

infileName = options.outdir+"/"+options.outfile
outfileName = infileName.replace('.root','.pck')

infile = ROOT.TFile.Open(infileName,'READ')
tree = infile.AnalysisTree
print "Total generated events =",totalGenEvents
print "Total processed events =",totalEvents
print "The output tree has",tree.GetEntries(),"entries"
if tree.GetEntries() != 0: 
 tree.GetEntry(0)
 xsec = tree.xsec
else:
 print "The tree is empty!"
 sys.exit()
 
xsec = 1.0 
print "Xsec = ",xsec,"pb"
weight = 1./totalGenEvents
print "Weight = ",weight

data1 = {'sigma': 1.0,
         'events': totalGenEvents,
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

answer = raw_input('Do you want to remove the jobs folders? (YES or NO) (ps, if you are running nominal 17 do not do YES!!) ')
#answer='YES'
if answer == 'YES':
 os.system('rm -rf %s'%options.indir)
 os.system('rm -rf %s*'%sample)
