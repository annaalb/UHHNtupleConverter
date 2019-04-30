#pragma once

#include "UHH2/core/include/fwd.h"
#include "UHH2/core/include/Selection.h"
#include "UHH2/core/include/Jet.h"
#include "UHH2/common/include/ObjectIdUtils.h"

namespace uhh2examples {
    
/* Select events with at least two jets in which the leading two jets have deltaphi > 2.7 and the third jet pt is
 * below 20% of the average of the leading two jets, where the minimum deltaphi and
 * maximum third jet pt fraction can be changed in the constructor.
 * The jets are assumed to be sorted in pt.
 */
class DijetSelection: public uhh2::Selection {
public:
    DijetSelection(float deta_max = 2.7f, float mjj_min = 0.0f);
    virtual bool passes(const uhh2::Event & event) override;
private:
    float deta_max, mjj_min;
};


class MuonVeto: public uhh2::Selection {
public:
  MuonVeto(float deltR_min = 0.8f, const boost::optional<MuonId> & muid = boost::none);
  virtual bool passes(const uhh2::Event & event) override;
private:
  float deltaR_min;
  boost::optional<MuonId> muid;
};

class ElectronVeto: public uhh2::Selection {
public:
  ElectronVeto(float deltR_min = 0.8f, const boost::optional<ElectronId> & eleid = boost::none);
  virtual bool passes(const uhh2::Event & event) override;
private:
  float deltaR_min;
  boost::optional<ElectronId> eleid;
};




class GenHbbEventSelection: public uhh2::Selection {
public:
    GenHbbEventSelection();
    virtual bool passes(const uhh2::Event & event) override {return false;};
    bool passes(const uhh2::Event & event, Jet & jet_);
    
private:    

};

class GenVqqEventSelection: public uhh2::Selection {
public:
    GenVqqEventSelection();
    virtual bool passes(const uhh2::Event & event) override {return false;};
    bool passes(const uhh2::Event & event, Jet & jet_);
    
private:    

};

}
