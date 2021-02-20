#include "UHH2/core/include/Event.h"
#include "UHH2/UHHNtupleConverter/include/VBFModule.h"
#include "UHH2/common/include/Utils.h"


using namespace uhh2;
using namespace std;


Ak4RemovalModule::Ak4RemovalModule(Context & ctx, float mindeta_, string const & VBFjets): mindeta(mindeta_)
{
  hndlvbf = ctx.get_handle<vector<Jet>>(VBFjets);
  year = extract_year(ctx);
}

bool Ak4RemovalModule::process(Event & event){
  assert(event.jets);
  if (!event.is_valid(hndlvbf)) {
    cerr << "VBF cleaning: Handle not valid!\n";
    assert(false);
  }
  vector<Jet> & vbfjet_collection = event.get(hndlvbf);

  //Cleaning(removing) AK4 if overlapping with AK8
  std::vector<Jet>* AK4Jets(new std::vector<Jet> (vbfjet_collection));
  const Jet & j_0 = event.jets->at(0);
  const Jet & j_1 = event.jets->at(1);

  AK4Jets->clear();
  AK4Jets->reserve(vbfjet_collection.size());

  for(const Jet ak4:vbfjet_collection)
    {
      bool bdeltaR=true;
      double deltar_0 = deltaR(ak4,j_0);
      double deltar_1 = deltaR(ak4,j_1);
      if( !passes_jetid(ak4) ) continue;
      if((deltar_0 < mindeta) || (deltar_1 < mindeta)) bdeltaR=false;
      if(bdeltaR) AK4Jets->push_back(ak4);
    }

  sort_by_pt<Jet>(*AK4Jets);
  ////put cleaned AK4 jets in event.jet
  vbfjet_collection.clear();
  vbfjet_collection.reserve(AK4Jets->size());
  for(const auto & j : *AK4Jets) vbfjet_collection.push_back(j);
  sort_by_pt<Jet>(*event.jets);

  event.set(hndlvbf, vbfjet_collection);
  return true;
}

bool Ak4RemovalModule::passes_jetid(const Jet & jet){

 if( year==Year::is2017v2 || year == Year::is2016v2 || year == Year::is2016v3 ){

  if( abs(jet.eta())<=2.4 && jet.neutralHadronEnergyFraction()<0.90 && jet.neutralEmEnergyFraction()<0.90 && jet.numberOfDaughters()>1 && jet.chargedHadronEnergyFraction()>0 && jet.chargedMultiplicity()>0 ) return true;
  else if( abs(jet.eta())>2.4 && abs(jet.eta())<=2.7 && jet.neutralHadronEnergyFraction()<0.90 && jet.neutralEmEnergyFraction()<0.90 && jet.numberOfDaughters()>1 ) return true;
  else if( abs(jet.eta())>2.7 && abs(jet.eta())<=3.0 && jet.neutralHadronEnergyFraction()<0.9999 ) return true;
  else if( abs(jet.eta())>3.0 && jet.neutralEmEnergyFraction()<0.90 && jet.neutralHadronEnergyFraction()>0.02 ) return true;
  
 }
 else if( year == Year::is2018 ){
  
  if( abs(jet.eta())<=2.6 && jet.neutralHadronEnergyFraction()<0.9 && jet.neutralEmEnergyFraction()<0.9 && jet.numberOfDaughters()>1 && jet.chargedHadronEnergyFraction()>0 && jet.chargedMultiplicity()>0 ) return true;
  else if( abs(jet.eta())>2.6 && abs(jet.eta())<=2.7 && jet.neutralHadronEnergyFraction()<0.9 && jet.neutralEmEnergyFraction()<0.99  ) return true;
  else if( abs(jet.eta())>2.7 && abs(jet.eta())<=3.0 && jet.neutralHadronEnergyFraction()<0.9999 ) return true;
  else if( abs(jet.eta()) > 3.0 && jet.neutralHadronEnergyFraction()>0.02 && jet.neutralEmEnergyFraction()<0.90 ) return true; 
  
 }
 
 return false;

}

VBFvariable::VBFvariable(Context & ctx, string const & VBFjets)
{
  hndlvbf = ctx.get_handle<vector<Jet>>(VBFjets);
  
  //reco puppi VBF jet variables                                                                                                                                                                                                                                                
  m_o_vbf_mjj = ctx.declare_event_output<float>("vbf_jj_LV_mass");
  m_o_vbf_ptjj = ctx.declare_event_output<float>("vbf_jj_LV_pt");
  m_o_vbf_etajj = ctx.declare_event_output<float>("vbf_jj_LV_eta");
  m_o_vbf_phijj = ctx.declare_event_output<float>("vbf_jj_LV_phi");
  m_o_vbf_pt_jet1 = ctx.declare_event_output<float>("vbf_jj_l1_pt");
  m_o_vbf_pt_jet2 = ctx.declare_event_output<float>("vbf_jj_l2_pt");
  m_o_vbf_eta_jet1 = ctx.declare_event_output<float>("vbf_jj_l1_eta");
  m_o_vbf_eta_jet2 = ctx.declare_event_output<float>("vbf_jj_l2_eta");
  m_o_vbf_phi_jet1 = ctx.declare_event_output<float>("vbf_jj_l1_phi");
  m_o_vbf_phi_jet2 = ctx.declare_event_output<float>("vbf_jj_l2_phi");
  m_o_vbf_mass_jet1 = ctx.declare_event_output<float>("vbf_jj_l1_mass");
  m_o_vbf_mass_jet2 = ctx.declare_event_output<float>("vbf_jj_l2_mass");
  
  m_o_vbf_Nconst_jet1  = ctx.declare_event_output<float>("vbf_jj_l1_Nconst");
  m_o_vbf_Nconst_jet2  = ctx.declare_event_output<float>("vbf_jj_l2_Nconst");
  m_o_vbf_nef_jet1     = ctx.declare_event_output<float>("vbf_jj_l1_nef");   
  m_o_vbf_nef_jet2     = ctx.declare_event_output<float>("vbf_jj_l2_nef");   
  m_o_vbf_nhf_jet1     = ctx.declare_event_output<float>("vbf_jj_l1_nhf");   
  m_o_vbf_nhf_jet2     = ctx.declare_event_output<float>("vbf_jj_l2_nhf");   
  m_o_vbf_cef_jet1     = ctx.declare_event_output<float>("vbf_jj_l1_cef");   
  m_o_vbf_cef_jet2     = ctx.declare_event_output<float>("vbf_jj_l2_cef");   
  m_o_vbf_chf_jet1     = ctx.declare_event_output<float>("vbf_jj_l1_chf");   
  m_o_vbf_chf_jet2     = ctx.declare_event_output<float>("vbf_jj_l2_chf");   
  m_o_vbf_mf_jet1      = ctx.declare_event_output<float>("vbf_jj_l1_mf");    
  m_o_vbf_mf_jet2      = ctx.declare_event_output<float>("vbf_jj_l2_mf");    
  m_o_vbf_phf_jet1     = ctx.declare_event_output<float>("vbf_jj_l1_phf");   
  m_o_vbf_phf_jet2     = ctx.declare_event_output<float>("vbf_jj_l2_phf");   
  m_o_vbf_cmult_jet1   = ctx.declare_event_output<float>("vbf_jj_l1_cmult"); 
  m_o_vbf_cmult_jet2   = ctx.declare_event_output<float>("vbf_jj_l2_cmult"); 
  m_o_vbf_nmult_jet1   = ctx.declare_event_output<float>("vbf_jj_l1_nmult"); 
  m_o_vbf_nmult_jet2   = ctx.declare_event_output<float>("vbf_jj_l2_nmult"); 
  m_o_vbf_mumult_jet1  = ctx.declare_event_output<float>("vbf_jj_l1_mumult");
  m_o_vbf_mumult_jet2  = ctx.declare_event_output<float>("vbf_jj_l2_mumult");
  m_o_vbf_elmult_jet1  = ctx.declare_event_output<float>("vbf_jj_l1_elmult");
  m_o_vbf_elmult_jet2  = ctx.declare_event_output<float>("vbf_jj_l2_elmult");
  m_o_vbf_phmult_jet1  = ctx.declare_event_output<float>("vbf_jj_l1_phmult");
  m_o_vbf_phmult_jet2  = ctx.declare_event_output<float>("vbf_jj_l2_phmult");
  m_o_vbf_puppimult_jet1      = ctx.declare_event_output<float>("vbf_jj_l1_puppimult");    
  m_o_vbf_puppimult_jet2      = ctx.declare_event_output<float>("vbf_jj_l2_puppimult");    
  m_o_vbf_npuppimult_jet1     = ctx.declare_event_output<float>("vbf_jj_l1_npuppimult");   
  m_o_vbf_npuppimult_jet2     = ctx.declare_event_output<float>("vbf_jj_l2_npuppimult");   
  m_o_vbf_nhpuppimult_jet1    = ctx.declare_event_output<float>("vbf_jj_l1_nhpuppimult");  
  m_o_vbf_nhpuppimult_jet2    = ctx.declare_event_output<float>("vbf_jj_l2_nhpuppimult");  
  m_o_vbf_phpuppimult_jet1    = ctx.declare_event_output<float>("vbf_jj_l1_phpuppimult");  
  m_o_vbf_phpuppimult_jet2    = ctx.declare_event_output<float>("vbf_jj_l2_phpuppimult");  
  m_o_vbf_HFhpuppimult_jet1   = ctx.declare_event_output<float>("vbf_jj_l1_HFhpuppimult"); 
  m_o_vbf_HFhpuppimult_jet2   = ctx.declare_event_output<float>("vbf_jj_l2_HFhpuppimult"); 
  m_o_vbf_HFempuppimult_jet1  = ctx.declare_event_output<float>("vbf_jj_l1_HFempuppimult");
  m_o_vbf_HFempuppimult_jet2  = ctx.declare_event_output<float>("vbf_jj_l2_HFempuppimult");
  
}
bool VBFvariable::process(Event & event){
  if (!event.is_valid(hndlvbf)) {
    cerr << "VBF cleaning: Handle not valid!\n";
    assert(false);
  }
  vector<Jet> & vbfjets = event.get(hndlvbf);
  int VBFsize = vbfjets.size() ;

  //reco puppi vbf jet variables                                                                                                                                                                                                                                                  
  event.set(m_o_vbf_mjj,VBFsize > 1 ? inv_mass_safe( vbfjets.at(0).v4()+vbfjets.at(1).v4()) : -9999);
  event.set(m_o_vbf_ptjj,VBFsize > 1 ? (vbfjets.at(0).v4()+vbfjets.at(1).v4()).Pt() : -9999);
  event.set(m_o_vbf_etajj,VBFsize > 1 ? (vbfjets.at(0).v4()+vbfjets.at(1).v4()).Eta() : -9999);
  event.set(m_o_vbf_phijj,VBFsize > 1 ? (vbfjets.at(0).v4()+vbfjets.at(1).v4()).Phi() : -9999);
  event.set(m_o_vbf_pt_jet1,VBFsize > 0 ? vbfjets.at(0).pt() : -9999);
  event.set(m_o_vbf_pt_jet2,VBFsize > 1 ? vbfjets.at(1).pt() : -9999);
  event.set(m_o_vbf_eta_jet1,VBFsize > 0 ? vbfjets.at(0).eta() : -9999);
  event.set(m_o_vbf_eta_jet2,VBFsize > 1 ? vbfjets.at(1).eta() : -9999);
  event.set(m_o_vbf_phi_jet1,VBFsize > 0 ? vbfjets.at(0).phi() : -9999);
  event.set(m_o_vbf_phi_jet2,VBFsize > 1 ? vbfjets.at(1).phi() : -9999);
  event.set(m_o_vbf_mass_jet1,VBFsize > 0 ? vbfjets.at(0).v4().M() : -9999);
  event.set(m_o_vbf_mass_jet2,VBFsize > 1 ? vbfjets.at(1).v4().M() : -9999);
  
  event.set(m_o_vbf_Nconst_jet1,VBFsize > 0 ? vbfjets.at(0).numberOfDaughters() : -9999);
  event.set(m_o_vbf_Nconst_jet2,VBFsize > 1 ? vbfjets.at(1).numberOfDaughters() : -9999);
  event.set(m_o_vbf_nef_jet1,VBFsize > 0 ? vbfjets.at(0).neutralEmEnergyFraction() : -9999);
  event.set(m_o_vbf_nef_jet2,VBFsize > 1 ? vbfjets.at(1).neutralEmEnergyFraction() : -9999);
  event.set(m_o_vbf_nhf_jet1,VBFsize > 0 ? vbfjets.at(0).neutralHadronEnergyFraction() : -9999);
  event.set(m_o_vbf_nhf_jet2,VBFsize > 1 ? vbfjets.at(1).neutralHadronEnergyFraction() : -9999);
  event.set(m_o_vbf_cef_jet1,VBFsize > 0 ? vbfjets.at(0).chargedEmEnergyFraction() : -9999);
  event.set(m_o_vbf_cef_jet2,VBFsize > 1 ? vbfjets.at(1).chargedEmEnergyFraction() : -9999);
  event.set(m_o_vbf_chf_jet1,VBFsize > 0 ? vbfjets.at(0).chargedHadronEnergyFraction() : -9999);
  event.set(m_o_vbf_chf_jet2,VBFsize > 1 ? vbfjets.at(1).chargedHadronEnergyFraction() : -9999);
  event.set(m_o_vbf_mf_jet1,VBFsize > 0 ? vbfjets.at(0).muonEnergyFraction() : -9999);
  event.set(m_o_vbf_mf_jet2,VBFsize > 1 ? vbfjets.at(1).muonEnergyFraction() : -9999);
  event.set(m_o_vbf_phf_jet1,VBFsize > 0 ? vbfjets.at(0).photonEnergyFraction() : -9999);
  event.set(m_o_vbf_phf_jet2,VBFsize > 1 ? vbfjets.at(1).photonEnergyFraction() : -9999);
  event.set(m_o_vbf_cmult_jet1,VBFsize > 0 ? vbfjets.at(0).chargedMultiplicity() : -9999);
  event.set(m_o_vbf_cmult_jet2,VBFsize > 1 ? vbfjets.at(1).chargedMultiplicity() : -9999);
  event.set(m_o_vbf_nmult_jet1,VBFsize > 0 ? vbfjets.at(0).neutralMultiplicity() : -9999);
  event.set(m_o_vbf_nmult_jet2,VBFsize > 1 ? vbfjets.at(1).neutralMultiplicity() : -9999);
  event.set(m_o_vbf_mumult_jet1,VBFsize > 0 ? vbfjets.at(0).muonMultiplicity() : -9999);
  event.set(m_o_vbf_mumult_jet2,VBFsize > 1 ? vbfjets.at(1).muonMultiplicity() : -9999);
  event.set(m_o_vbf_elmult_jet1,VBFsize > 0 ? vbfjets.at(0).electronMultiplicity() : -9999);
  event.set(m_o_vbf_elmult_jet2,VBFsize > 1 ? vbfjets.at(1).electronMultiplicity() : -9999);
  event.set(m_o_vbf_phmult_jet1,VBFsize > 0 ? vbfjets.at(0).photonMultiplicity() : -9999);
  event.set(m_o_vbf_phmult_jet2,VBFsize > 1 ? vbfjets.at(1).photonMultiplicity() : -9999);
  event.set(m_o_vbf_puppimult_jet1,VBFsize > 0 ? vbfjets.at(0).puppiMultiplicity() : -9999);
  event.set(m_o_vbf_puppimult_jet2,VBFsize > 1 ? vbfjets.at(1).puppiMultiplicity() : -9999);  
  event.set(m_o_vbf_npuppimult_jet1,VBFsize > 0 ? vbfjets.at(0).neutralPuppiMultiplicity() : -9999);
  event.set(m_o_vbf_npuppimult_jet2,VBFsize > 1 ? vbfjets.at(1).neutralPuppiMultiplicity() : -9999);
  event.set(m_o_vbf_nhpuppimult_jet1,VBFsize > 0 ? vbfjets.at(0).neutralHadronPuppiMultiplicity() : -9999);
  event.set(m_o_vbf_nhpuppimult_jet2,VBFsize > 1 ? vbfjets.at(1).neutralHadronPuppiMultiplicity() : -9999);
  event.set(m_o_vbf_phpuppimult_jet1,VBFsize > 0 ? vbfjets.at(0).photonPuppiMultiplicity() : -9999);
  event.set(m_o_vbf_phpuppimult_jet2,VBFsize > 1 ? vbfjets.at(1).photonPuppiMultiplicity() : -9999);
  event.set(m_o_vbf_HFhpuppimult_jet1,VBFsize > 0 ? vbfjets.at(0).HFHadronPuppiMultiplicity() : -9999);
  event.set(m_o_vbf_HFhpuppimult_jet2,VBFsize > 1 ? vbfjets.at(1).HFHadronPuppiMultiplicity() : -9999);
  event.set(m_o_vbf_HFempuppimult_jet1,VBFsize > 0 ? vbfjets.at(0).HFEMPuppiMultiplicity() : -9999);
  event.set(m_o_vbf_HFempuppimult_jet2,VBFsize > 1 ? vbfjets.at(1).HFEMPuppiMultiplicity() : -9999);
      
  return true;
}
