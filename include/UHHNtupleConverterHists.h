#pragma once

#include "UHH2/core/include/Hists.h"
#include "UHH2/common/include/TriggerSelection.h"

namespace uhh2examples {

/**  \brief Example class for booking and filling histograms
 * 
 * NOTE: This class uses the 'hist' method to retrieve histograms.
 * This requires a string lookup and is therefore slow if you have
 * many histograms. Therefore, it is recommended to use histogram
 * pointers as member data instead, like in 'common/include/ElectronHists.h'.
 */
class UHHNtupleConverterHists: public uhh2::Hists {
public:
    // use the same constructor arguments as Hists for forwarding:
    UHHNtupleConverterHists(uhh2::Context & ctx, const std::string & dirname);

    virtual void fill(const uhh2::Event & ev) override;
    virtual ~UHHNtupleConverterHists();
};

class TriggerHists: public uhh2::Hists {
public:
    // use the same constructor arguments as Hists for forwarding:
    TriggerHists(uhh2::Context & ctx, const std::string & dirname, std::vector<std::string> jet_triggers_n);

    virtual void fill(const uhh2::Event & ev) override {return;};
    void fill(const uhh2::Event & ev,std::vector<TriggerSelection> jet_triggers, std::vector<std::string> jet_triggers_n);
    virtual ~TriggerHists();
};

}
