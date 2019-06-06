#include "UHH2/core/include/Event.h"
#include "UHH2/UHHNtupleConverter/include/VBFModule.h"
#include "UHH2/common/include/Utils.h"


using namespace uhh2;
using namespace std;


Ak4RemovalModule::Ak4RemovalModule(Context & ctx, float mindeta_, string const & VBFjets): mindeta(mindeta_)
{
  hndlvbf = ctx.get_handle<vector<Jet>>(VBFjets);
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



  return true;
}
