#include "UHH2/core/include/Event.h"
#include "UHH2/UHHNtupleConverter/include/NLOweightsModule.h"
#include "UHH2/common/include/Utils.h"


using namespace uhh2;
using namespace std;

bool print = false;

NLOweight::NLOweight(Context & ctx,
                     const std::string & NLOWeightsFilename,
                     const std::string & s_jj_l1_mergedVTruth,
                     const std::string & s_jj_l1_gen_pt,
                     const std::string & s_jj_l2_gen_pt)
{
  jj_l1_mergedVTruth = ctx.get_handle<bool>(s_jj_l1_mergedVTruth);
  jj_l1_gen_pt = ctx.get_handle<float>(s_jj_l1_gen_pt);
  jj_l2_gen_pt = ctx.get_handle<float>(s_jj_l2_gen_pt);
  NLOWeightsFileName_ = NLOWeightsFilename;
  NLOWeightsFile.reset(TFile::Open(locate_file(NLOWeightsFilename).c_str()));
  //h_kfactor.reset((TH1F*) NLOWeightsFile->Get("kfactor"));
  h_ewcorr.reset((TH1F*) NLOWeightsFile->Get("kfactor_monojet_ewk"));

  funcQCD_ = new TF1("funcQCD","[0]*exp([1]*x)+[2]",0,3600);
  funcEW_ = new TF1("funcEW","[0]*exp([1]*x)+[2]",0,3600);
  if( NLOWeightsFileName_.Contains("WJets") ){
   funcQCD_->SetParameter(0,1.053);
   funcQCD_->SetParameter(1,-3.163/1000.);
   funcQCD_->SetParameter(2,0.746);
   funcEW_->SetParameter(0,0.523418);
   funcEW_->SetParameter(1,-0.00073779);
   funcEW_->SetParameter(2,0.501753);
  }
  else{
   funcQCD_->SetParameter(0,1.434);
   funcQCD_->SetParameter(1,-2.210/1000.);
   funcQCD_->SetParameter(2,0.443);
   funcEW_->SetParameter(0,0.371264);
   funcEW_->SetParameter(1,-0.000859976);
   funcEW_->SetParameter(2,0.648457);
  }
  
  m_o_kfactor = ctx.declare_event_output<float>("kfactor");
  m_o_kfactor_ew = ctx.declare_event_output<float>("kfactor_ew");
  m_o_kfactor_qcd = ctx.declare_event_output<float>("kfactor_qcd");
  
}

bool NLOweight::process(Event & event){
  if(print) cout << "process " << endl;
  float w = 1.;
  float w_ew = 1.;

  bool b_jj_l1_mergedVTruth;
  float gen_pt_1;
  float gen_pt_2;
  
  if (event.is_valid(jj_l1_mergedVTruth)) b_jj_l1_mergedVTruth = &event.get(jj_l1_mergedVTruth);
  else throw std::runtime_error("NLOweights::process -- invalid handle to jj_l1_mergedVTruth");
  if (event.is_valid(jj_l1_gen_pt)) gen_pt_1 = event.get(jj_l1_gen_pt);
  else throw std::runtime_error("NLOweights::process -- invalid handle to jj_l1_gen_pt");
  if (event.is_valid(jj_l2_gen_pt)) gen_pt_2 = event.get(jj_l2_gen_pt);
  else throw std::runtime_error("NLOweights::process -- invalid handle to jj_l2_gen_pt");


  if(print) cout << "jj_l1_mergedVTruth " << b_jj_l1_mergedVTruth << endl; 
  if( b_jj_l1_mergedVTruth ==1){
    w = getWeight(gen_pt_1);
    w_ew = getEWCorr(gen_pt_1);
  }
  else{
    w = getWeight(gen_pt_2);
    w_ew = getEWCorr(gen_pt_2);
  }
  float kfactor = w * w_ew; 
  if(print) cout << "kfactor " << kfactor << " = w " << w << " * w_ew " << w_ew << endl; 
  
  event.set(m_o_kfactor, kfactor);
  event.set(m_o_kfactor_ew, w_ew);
  event.set(m_o_kfactor_qcd, w);

  return true;
}



float NLOweight::getWeight(float gen_pt){
  if(print) cout << "getWeight " << endl;

  /*if(print) cout << "gen pt = " << gen_pt << endl;
  if( gen_pt > 3000 ) gen_pt = 2800;
  if( gen_pt < 200 ) gen_pt = 205;
  if(print) cout << "gen pt = " << gen_pt << endl;

  float binw = h_kfactor->GetXaxis()->FindBin(gen_pt);
  if(print) cout << "bin width  = " << binw << endl;

  float reweight= h_kfactor->GetBinContent(binw);
  if(print) cout << "reweight = " << reweight << endl;*/

  //return reweight;
  
  if(print) cout << "gen pt = " << gen_pt << " QCD NLO SF = " << funcQCD_->Eval(gen_pt) << endl;
  
  return funcQCD_->Eval(gen_pt);

}

float NLOweight::getEWCorr(float gen_pt){
  if(print) cout << "getEWCorr " << endl;
  
  /*if(print) cout << "gen pt = " << gen_pt << endl;
  
  if( gen_pt > 1250 ) gen_pt = 1205;
  if( gen_pt < 150 ) gen_pt = 160;
  if(print) cout << "gen pt = " << gen_pt << endl;

  float binw = h_ewcorr->GetXaxis()->FindBin(gen_pt);
  if(print) cout << "bin width  = " << binw << endl;

  float reweight= h_ewcorr->GetBinContent(binw);
  if(print) cout << "reweight = " << reweight << endl;

  return reweight;*/

  if(print) cout << "gen pt = " << gen_pt << " EW NLO SF = " << funcEW_->Eval(gen_pt) << endl;
  
  return funcEW_->Eval(gen_pt);
  
}
