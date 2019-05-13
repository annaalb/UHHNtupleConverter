#pragma once


/* This file define 'cleaning' modules, i.e. modules which throw away
 * parts of some collection in the event by applying kinematic or id criteria.
 */
#include <iostream>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/ObjectIdUtils.h"

using namespace std;

/// Keep only jets fulfilling a certain JetId / with a minimum pt and maximum |eta|
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

 };
