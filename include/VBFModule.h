#pragma once


/* This file define 'cleaning' modules, i.e. modules which throw away
 * parts of some collection in the event by applying kinematic or id criteria.
 */
#include <iostream>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/ObjectIdUtils.h"

using namespace std;

/// keep only Ak4 jets that to not overlap with the leading ak8 chs jets
class Ak4RemovalModule: public uhh2::AnalysisModule {
 public:
    
  explicit Ak4RemovalModule(uhh2::Context & ctx, float mindeta_ = 1.2, string const & VBFjets = "jetsAk4Puppi");
  virtual bool process(uhh2::Event & event) override;
    
 private:
  float mindeta;
  uhh2::Event::Handle<std::vector<Jet>> hndlvbf;
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

};
