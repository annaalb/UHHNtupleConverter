#include "UHH2/core/include/Event.h"
#include "UHH2/UHHNtupleConverter/include/Ak4RemovalModule.h"
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
    cerr << "VBF: Handle not valid!\n";
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

  /////////////////AK4 cleaning end ////////////////

  event.set(hndlvbf, vbfjet_collection);
  return true;
}
