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
class Ak4RemovalModule: public uhh2::AnalysisModule {
 public:
    
  explicit Ak4RemovalModule(uhh2::Context & ctx, float mindeta_ = 1.2, string const & VBFjets = "jetsAk4Puppi");
  virtual bool process(uhh2::Event & event) override;
    
 private:
  float mindeta;
  uhh2::Event::Handle<std::vector<Jet>> hndlvbf;
 };
