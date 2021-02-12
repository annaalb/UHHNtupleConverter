#pragma once


/* This file define 'cleaning' modules, i.e. modules which throw away
 * parts of some collection in the event by applying kinematic or id criteria.
 */
#include <iostream>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/ObjectIdUtils.h"
#include "UHH2/common/include/Utils.h"


using namespace std;

/// keep only Ak4 jets that to not overlap with the leading ak8 chs jets
class Ak4RemovalModule: public uhh2::AnalysisModule {
 public:
    
  explicit Ak4RemovalModule(uhh2::Context & ctx, float mindeta_ = 1.2, string const & VBFjets = "jetsAk4Puppi");
  virtual bool process(uhh2::Event & event) override;
  bool passes_jetid(const Jet & jet);
    
 private:
  float mindeta;
  uhh2::Event::Handle<std::vector<Jet>> hndlvbf;
  Year year;

 };
 
/// saving variables for VBF jets to the output tree
class VBFvariable: public uhh2::AnalysisModule {
 public:

  explicit VBFvariable(uhh2::Context & ctx, string const & VBFjets = "jetsAk4Puppi");
  virtual bool process(uhh2::Event & event) override;

 private:
  uhh2::Event::Handle<std::vector<Jet>> hndlvbf;
  //reco puppi VBF jet variables                                                                                                                                                                                                                                                
  uhh2::Event::Handle<float>  m_o_vbf_mjj;
  uhh2::Event::Handle<float>  m_o_vbf_ptjj;
  uhh2::Event::Handle<float>  m_o_vbf_etajj;
  uhh2::Event::Handle<float>  m_o_vbf_phijj;
  uhh2::Event::Handle<float>  m_o_vbf_pt_jet1;
  uhh2::Event::Handle<float>  m_o_vbf_pt_jet2;
  uhh2::Event::Handle<float>  m_o_vbf_eta_jet1;
  uhh2::Event::Handle<float>  m_o_vbf_eta_jet2;
  uhh2::Event::Handle<float>  m_o_vbf_phi_jet1;
  uhh2::Event::Handle<float>  m_o_vbf_phi_jet2;
  uhh2::Event::Handle<float>  m_o_vbf_mass_jet1;
  uhh2::Event::Handle<float>  m_o_vbf_mass_jet2;
  uhh2::Event::Handle<float>  m_o_vbf_Nconst_jet1; //m_numberOfDaughters
  uhh2::Event::Handle<float>  m_o_vbf_Nconst_jet2;
  uhh2::Event::Handle<float>  m_o_vbf_nef_jet1; //m_neutralEmEnergyFraction
  uhh2::Event::Handle<float>  m_o_vbf_nef_jet2;  
  uhh2::Event::Handle<float>  m_o_vbf_nhf_jet1; //m_neutralHadronEnergyFraction
  uhh2::Event::Handle<float>  m_o_vbf_nhf_jet2;
  uhh2::Event::Handle<float>  m_o_vbf_cef_jet1; //m_chargedEmEnergyFraction
  uhh2::Event::Handle<float>  m_o_vbf_cef_jet2;
  uhh2::Event::Handle<float>  m_o_vbf_chf_jet1; //m_chargedHadronEnergyFraction
  uhh2::Event::Handle<float>  m_o_vbf_chf_jet2;  
  uhh2::Event::Handle<float>  m_o_vbf_mf_jet1; //m_muonEnergyFraction
  uhh2::Event::Handle<float>  m_o_vbf_mf_jet2;
  uhh2::Event::Handle<float>  m_o_vbf_phf_jet1; //m_photonEnergyFraction
  uhh2::Event::Handle<float>  m_o_vbf_phf_jet2;
  uhh2::Event::Handle<float>  m_o_vbf_cmult_jet1; //m_chargedMultiplicity
  uhh2::Event::Handle<float>  m_o_vbf_cmult_jet2;
  uhh2::Event::Handle<float>  m_o_vbf_nmult_jet1; //m_neutralMultiplicity
  uhh2::Event::Handle<float>  m_o_vbf_nmult_jet2;
  uhh2::Event::Handle<float>  m_o_vbf_mumult_jet1; //m_muonMultiplicity
  uhh2::Event::Handle<float>  m_o_vbf_mumult_jet2;
  uhh2::Event::Handle<float>  m_o_vbf_elmult_jet1; //m_electronMultiplicity
  uhh2::Event::Handle<float>  m_o_vbf_elmult_jet2;
  uhh2::Event::Handle<float>  m_o_vbf_phmult_jet1; //m_photonMultiplicity
  uhh2::Event::Handle<float>  m_o_vbf_phmult_jet2;
  uhh2::Event::Handle<float>  m_o_vbf_puppimult_jet1; //m_puppiMultiplicity
  uhh2::Event::Handle<float>  m_o_vbf_puppimult_jet2;
  uhh2::Event::Handle<float>  m_o_vbf_npuppimult_jet1; //m_neutralPuppiMultiplicity
  uhh2::Event::Handle<float>  m_o_vbf_npuppimult_jet2;
  uhh2::Event::Handle<float>  m_o_vbf_nhpuppimult_jet1; //m_neutralHadronPuppiMultiplicity
  uhh2::Event::Handle<float>  m_o_vbf_nhpuppimult_jet2;
  uhh2::Event::Handle<float>  m_o_vbf_phpuppimult_jet1; //m_photonPuppiMultiplicity
  uhh2::Event::Handle<float>  m_o_vbf_phpuppimult_jet2;
  uhh2::Event::Handle<float>  m_o_vbf_HFhpuppimult_jet1; //m_HFHadronPuppiMultiplicity
  uhh2::Event::Handle<float>  m_o_vbf_HFhpuppimult_jet2;
  uhh2::Event::Handle<float>  m_o_vbf_HFempuppimult_jet1; //m_HFEMPuppiMultiplicity
  uhh2::Event::Handle<float>  m_o_vbf_HFempuppimult_jet2;
};
