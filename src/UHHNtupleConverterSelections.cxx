#include "UHH2/UHHNtupleConverter/include/UHHNtupleConverterSelections.h"

#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Particle.h"
#include "UHH2/common/include/Utils.h"

#include <stdexcept>
#include <iostream>

using namespace uhh2examples;
using namespace uhh2;
using namespace std;
bool PRINT = false;

DijetSelection::DijetSelection(float deta_max_, float mjj_min_): deta_max(deta_max_), mjj_min(mjj_min_){}
    
bool DijetSelection::passes(const Event & event){
    assert(event.jets); // if this fails, it probably means jets are not read in
    if(event.jets->size() < 2) return false;
    const auto & jet0 = event.jets->at(0);
    const auto & jet1 = event.jets->at(1);
    auto deta = fabs(jet0.eta() - jet1.eta());
    if(deta > deta_max) return false;
    if(inv_mass_safe(jet0.v4()+jet1.v4()) < mjj_min ) return false;
    return true;
}

MuonVeto::MuonVeto(const MuonId & muid_, float deltaR_min_): deltaR_min(deltaR_min_), muid(muid_){}

bool MuonVeto::passes(const Event & event){
  assert(event.jets); // if this fails, it probably means jets are not read in                                                                                                                                                                                             
  assert(event.muons); // if this fails, it probably means jets are not read in                                                                                                                                                                                               
 
  for(const auto & muon : *event.muons) {
    if(muid(muon,event)) {
      for(const auto & jet : *event.jets){
	if(deltaR(jet,muon)  < deltaR_min ) return false;
      }
    }
  }

  return true;

}

ElectronVeto::ElectronVeto(const ElectronId & eleid_, float deltaR_min_): deltaR_min(deltaR_min_), eleid(eleid_){}

bool ElectronVeto::passes(const Event & event){
  assert(event.jets); // if this fails, it probably means jets are not read in                                                                                                                                                                                             
  assert(event.electrons); // if this fails, it probably means jets are not read in                                                                                                                                                                                           

  for(const auto & electron : *event.electrons){
    if(eleid(electron,event)) {
	for(const auto & jet : *event.jets){
	  if(deltaR(jet,electron)  < deltaR_min) return false;
	}
      }
  }
  return true;
  
}



GenHbbEventSelection::GenHbbEventSelection(){}

bool GenHbbEventSelection::passes(const Event & event, Jet & jet){
  std::vector<GenParticle> genQuarks;
  int associatedQuarks=0;
  //bool Higgsboson = false;
  if(PRINT) cout << " GenHbbEventSelection" << endl;
  for(auto genp:*event.genparticles){
    if(PRINT) cout << "  genparticle pdg id = " << genp.pdgId() << endl; 
    if(abs(genp.pdgId())==25){
      if(PRINT) cout << "    I have a Higgs boson!" << endl;
      //Higgsboson = true;
      if(abs(event.genparticles->at(genp.daughter1()).pdgId())==5 && abs(event.genparticles->at(genp.daughter2()).pdgId())==5){
       genQuarks.push_back( event.genparticles->at(genp.daughter1()) ); 
       genQuarks.push_back( event.genparticles->at(genp.daughter2()) );
      }
    }
    
    if(genQuarks.size()>1)
    {
      associatedQuarks=0;
      for(unsigned int i=0; i<genQuarks.size(); ++i){
         if( deltaR(jet.v4(),genQuarks[i].v4()) < 0.8) associatedQuarks +=1;
      } 
      if(associatedQuarks != 2){
         genQuarks.clear();
         associatedQuarks=0;
         continue;
      }
      else break;
    }
  }
  
  //if(Higgsboson && genQuarks.size()>2) cout << "WARNING: found more than two daughters for Higgs boson!" << endl;
  //if(Higgsboson && genQuarks.size()<2) cout << "WARNING: found less than two daughters for Higgs boson!" << endl;
  
  if (associatedQuarks == 2) return true;
  else return false;
}


GenVqqEventSelection::GenVqqEventSelection(){}

bool GenVqqEventSelection::passes(const Event & event, Jet & jet){
  assert(event.genparticles); // if this fails, it probably means genparticles are not read in    
  std::vector<GenParticle> genQuarks;
  int associatedQuarks=0;
  //bool vectorboson = false;
  if(PRINT) cout << " GenVqqEventSelection" << endl;
  for(auto genp:*event.genparticles){
    if(PRINT) cout << "  genparticle pdg id = " << genp.pdgId() << endl;   
    if(abs(genp.pdgId())==24 || abs(genp.pdgId())==23){
       if(PRINT) cout << "    I have a vector boson!" << endl;
       //vectorboson = true;
       int dau1 = abs(event.genparticles->at(genp.daughter1()).pdgId());
       if(PRINT) cout << "    I have one daughter" << endl;
       int dau2 = abs(event.genparticles->at(genp.daughter2()).pdgId());
       if(PRINT) cout << "    I have 2 daughters" << endl;
       if(PRINT) cout << "      daughters: " << event.genparticles->at(genp.daughter1()).pdgId() << " and " << event.genparticles->at(genp.daughter2()).pdgId() << endl;
       if( (dau1 >=1 && dau1 <=6) && (dau2 >=1 && dau2 <=6) ){
          genQuarks.push_back( event.genparticles->at(genp.daughter1()) ); 
          genQuarks.push_back( event.genparticles->at(genp.daughter2()) );
       }
    }

    if(genQuarks.size()>1)
    {
      associatedQuarks=0;
      for(unsigned int i=0; i<genQuarks.size(); ++i){
         if( deltaR(jet.v4(),genQuarks[i].v4()) < 0.8) associatedQuarks +=1;
      } 
      if(associatedQuarks != 2){
         genQuarks.clear();
         associatedQuarks=0;
         continue;
      }
      else break;
    }
  }
  
  //if(vectorboson && genQuarks.size()>2) cout << "WARNING: found more than two daughters for Vector boson!" << endl;
  //if(vectorboson && genQuarks.size()<2) cout << "WARNING: found less than two daughters for Vector boson! Are you using an inclusive sample?" << endl;

  if (associatedQuarks == 2) return true;
  else return false;
}   


VBFjetSelection::VBFjetSelection(Context & ctx, string const & VBFjet_, float deta_min_, float mjj_min_ ):VBFjet(VBFjet_), deta_min(deta_min_), mjj_min(mjj_min_){
  h_VBFjet = ctx.get_handle<vector<Jet>>(VBFjet);
}

bool VBFjetSelection::passes(const Event & event){
  if (!event.is_valid(h_VBFjet)) {
    cerr << "VBF selections: Handle not valid!\n";
    assert(false);
  }

  vector<Jet> vbfjets = event.get(h_VBFjet);
  if(vbfjets.size() < 2) return false;
  const auto & jet0 = vbfjets.at(0);
  const auto & jet1 = vbfjets.at(1);
  auto deta = fabs(jet0.eta() - jet1.eta());
  if(deta < deta_min) return false;
  if(jet0.eta() * jet1.eta() > 0) return false;
  if(inv_mass_safe(jet0.v4()+jet1.v4()) < mjj_min ) return false;
  return true;
}


///Brute Force Decorrelation
BruteForceDecorrelation::BruteForceDecorrelation( uhh2::Context & ctx, string percentage_){

  percentage = percentage_;
  string h_name = "DeepBoosted_ZHbbvsQCD_"+percentage;
  string h_name2 = "DeepBoosted_WvsQCD_"+percentage;
  h_DeepBoosted_ZHbbvsQCD= ctx.declare_event_output<float>(h_name);
  h_DeepBoosted_WvsQCD= ctx.declare_event_output<float>(h_name2);

  std::string filename_zh = "UHHNtupleConverter/data/DDTMap_ZHbbvsQCD/myDeepBoostedMap"+percentage+".root" ;
  std::string filename_w = "UHHNtupleConverter/data/DDTMap_WvsQCD/myDeepBoostedMap"+percentage+".root"; 
  infile_ZHbbvsQCD.reset(TFile::Open(locate_file(filename_zh).c_str()));
  infile_WvsQCD.reset(TFile::Open(locate_file(filename_w).c_str()));

  map_ZHbbvsQCD = (TH2F*)infile_ZHbbvsQCD->Get("DeepBoosted_ZHbbvsQCD_v_rho_v_pT_yx");
  map_WvsQCD = (TH2F*)infile_WvsQCD->Get("DeepBoosted_WvsQCD_v_rho_v_pT_yx");
}
    
bool BruteForceDecorrelation::process(Event & event){
  assert(event.topjets);


  for(auto jet:*event.topjets){
    //// find pT bin
    pt_bin = map_ZHbbvsQCD->GetYaxis()->FindFixBin(jet.pt());
    if(pt_bin > map_ZHbbvsQCD->GetYaxis()->GetNbins()){
      pt_bin = map_ZHbbvsQCD->GetYaxis()->GetNbins();
    }else if(pt_bin <=0){
      pt_bin = 1;
    }

    /// find rho bin
    x = 2 * TMath::Log(jet.softdropmass()/ jet.pt());
    x_bin = map_ZHbbvsQCD->GetXaxis()->FindFixBin(x);
    if(x_bin > map_ZHbbvsQCD->GetXaxis()->GetNbins()){
      x_bin = map_ZHbbvsQCD->GetXaxis()->GetNbins();
    }else if(x_bin <= 0){
      x_bin = 1;
    }
    
  }
  cut_ZHbbvsQCD = map_ZHbbvsQCD->GetBinContent(x_bin,pt_bin);
  cut_WvsQCD = map_WvsQCD->GetBinContent(x_bin,pt_bin);

  event.set(h_DeepBoosted_ZHbbvsQCD,cut_ZHbbvsQCD);
  event.set(h_DeepBoosted_WvsQCD,cut_WvsQCD);

  return true;
}






