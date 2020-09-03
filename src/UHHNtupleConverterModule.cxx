#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/CommonModules.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/MuonIds.h"
#include "UHH2/common/include/ElectronIds.h"
#include "UHH2/common/include/ObjectIdUtils.h"
#include "UHH2/common/include/ElectronHists.h"
#include "UHH2/common/include/NSelections.h"
#include "UHH2/common/include/TriggerSelection.h"
#include "UHH2/UHHNtupleConverter/include/UHHNtupleConverterSelections.h"
#include "UHH2/UHHNtupleConverter/include/UHHNtupleConverterHists.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/common/include/PrintingModules.h"
#include "UHH2/UHHNtupleConverter/include/LumiWeight.h"
#include "UHH2/UHHNtupleConverter/include/VBFModule.h"
#include "UHH2/UHHNtupleConverter/include/NLOweightsModule.h"
#include "UHH2/common/include/TopPtReweight.h"
#include "boost/algorithm/string.hpp"

using namespace std;
using namespace uhh2;

#define DEEPCSVLOOSE 0.2217
#define DEEPCSVMEDIUM 0.6321
#define DEEPCSVTIGHT 0.8953
#define DEEPJETLOOSE 0.0614
#define DEEPJETMEDIUM 0.3093
#define DEEPJETTIGHT 0.7221

namespace uhh2examples {

/** \brief Basic analysis example of an AnalysisModule (formerly 'cycle') in UHH2
 * 
 * This is the central class which calls other AnalysisModules, Hists or Selection classes.
 * This AnalysisModule, in turn, is called (via AnalysisModuleRunner) by SFrame.
 */
class UHHNtupleConverterModule: public AnalysisModule {
public:
    
    explicit UHHNtupleConverterModule(Context & ctx);
    virtual bool process(Event & event) override;
    ~UHHNtupleConverterModule();

private:

    bool PRINT = false;

    std::unique_ptr<AnalysisModule> Gen_printer;    
    std::unique_ptr<CommonModules> common;
    std::unique_ptr<JetCleaner> jetcleaner;
    std::unique_ptr<JetCleaner> vbfjetcleaner;
    std::unique_ptr<AnalysisModule> massCalc;

    std::string jec_tag, jec_ver, jec_jet_coll_AK8chs, jec_jet_coll_AK4puppi,Vcorr;
    bool isVjet;
    JERSmearing::SFtype1 JER_sf = {};
    std::string sfFilename = "";
    TString ResolutionFileName;   
  
    std::unique_ptr<JetCorrector> jet_corrector;
    std::unique_ptr<GenericJetCorrector> jet_corrector_puppi; //need an additional corrector for Puppi AK4, for VBF jets

    std::unique_ptr<JetCorrector> jet_corrector_2016_B;
    std::unique_ptr<JetCorrector> jet_corrector_2016_C;
    std::unique_ptr<JetCorrector> jet_corrector_2016_D;
    std::unique_ptr<JetCorrector> jet_corrector_2016_E;
    std::unique_ptr<JetCorrector> jet_corrector_2016_F;
    std::unique_ptr<JetCorrector> jet_corrector_2016_G;
    std::unique_ptr<JetCorrector> jet_corrector_2016_H;

    std::unique_ptr<JetCorrector> jet_corrector_2017_B;
    std::unique_ptr<JetCorrector> jet_corrector_2017_C;
    std::unique_ptr<JetCorrector> jet_corrector_2017_D;
    std::unique_ptr<JetCorrector> jet_corrector_2017_E;
    std::unique_ptr<JetCorrector> jet_corrector_2017_F;

    std::unique_ptr<JetCorrector> jet_corrector_2018_A;
    std::unique_ptr<JetCorrector> jet_corrector_2018_B;
    std::unique_ptr<JetCorrector> jet_corrector_2018_C;
    std::unique_ptr<JetCorrector> jet_corrector_2018_D;

    //for AK4 puppi VBF jets
    std::unique_ptr<GenericJetCorrector> jet_corrector_puppi_2016_B;
    std::unique_ptr<GenericJetCorrector> jet_corrector_puppi_2016_C;
    std::unique_ptr<GenericJetCorrector> jet_corrector_puppi_2016_D;
    std::unique_ptr<GenericJetCorrector> jet_corrector_puppi_2016_E;
    std::unique_ptr<GenericJetCorrector> jet_corrector_puppi_2016_F;
    std::unique_ptr<GenericJetCorrector> jet_corrector_puppi_2016_G;
    std::unique_ptr<GenericJetCorrector> jet_corrector_puppi_2016_H;

    std::unique_ptr<GenericJetCorrector> jet_corrector_puppi_2017_B;
    std::unique_ptr<GenericJetCorrector> jet_corrector_puppi_2017_C;
    std::unique_ptr<GenericJetCorrector> jet_corrector_puppi_2017_D;
    std::unique_ptr<GenericJetCorrector> jet_corrector_puppi_2017_E;
    std::unique_ptr<GenericJetCorrector> jet_corrector_puppi_2017_F;

    std::unique_ptr<GenericJetCorrector> jet_corrector_puppi_2018_A;
    std::unique_ptr<GenericJetCorrector> jet_corrector_puppi_2018_B;
    std::unique_ptr<GenericJetCorrector> jet_corrector_puppi_2018_C;
    std::unique_ptr<GenericJetCorrector> jet_corrector_puppi_2018_D;

    std::unique_ptr<JetResolutionSmearer> jet_EResSmearer;                                                                                                                                                                                                              
    std::unique_ptr<GenericJetResolutionSmearer> jetpuppi_EResSmearer;                                                                                                                                                                                                    
    std::unique_ptr<AnalysisModule> Ak4OverlapCleaner;    

    // declare the Selections to use. Use unique_ptr to ensure automatic call of delete in the destructor,
    // to avoid memory leaks.
    std::unique_ptr<Selection> muon_sel, electron_sel, njet_sel, dijet_sel,dijet_sel_loose, vbf_sel;
    std::unique_ptr<GenTopHadrEventSelection> genTopHadrEvent_sel;
    std::unique_ptr<GenHbbEventSelection> genHbbEvent_sel;
    std::unique_ptr<GenVqqEventSelection> genVqqEvent_sel;
    std::vector<TriggerSelection> trigger_selection, ref_triggers; 
    std::vector<TriggerSelection> metfilters;
    std::unique_ptr<NPVSelection> pvfilter;
    std::unique_ptr<TopPtReweight> top_pt_reweight;
    uhh2::Event::Handle<float> top_pt_weight;
    
    // store the Hists collection as member variables. Again, use unique_ptr to avoid memory leaks.
    std::unique_ptr<Hists> h_nocuts, h_njet, h_dijet, h_ele;
    std::unique_ptr<TriggerHists> h_trig;
    
    //some other useful objects
    Year year;
    LumiWeight m_xSec;
    double xSec_;
    bool isMC;
    bool isTTbar;
    bool isSignal;
    bool hasBuggyPU2017;
    float totalEvents;
    float totalGenEvents;
    float totalGenEvents_LO;
    std::vector<std::string> trigNames;

    bool printGenparticle;    

    enum Sorting{
     SORTING_RANDOM,
     SORTING_BYBTAG
    };
    Sorting sorting;
        
    //output tree variables
    
    //event variables
    uhh2::Event::Handle<bool>   b_isData;
    uhh2::Event::Handle<int>    b_lumi;
    uhh2::Event::Handle<int>    b_run;
    uhh2::Event::Handle<int>    b_event;
    uhh2::Event::Handle<double> b_xSec;
    uhh2::Event::Handle<float>  b_pdf_x1;
    uhh2::Event::Handle<float>  b_pdf_x2;
    uhh2::Event::Handle<float>  b_pdf_scalePDF;
    uhh2::Event::Handle<float>  b_weightGen;
    uhh2::Event::Handle<float>  b_weightGen_LO;
    uhh2::Event::Handle<float>  b_weightPU;
    uhh2::Event::Handle<float>  b_weightTop;
    uhh2::Event::Handle<float>  b_weightBTag;
    uhh2::Event::Handle<float>  b_nTrueInt;
    uhh2::Event::Handle<float>  b_rho;
    uhh2::Event::Handle<int>    b_nVert;
    uhh2::Event::Handle<bool>   HLT_JJ;
    uhh2::Event::Handle<bool>   b_passed_MET_filters;
    uhh2::Event::Handle<bool>   b_passed_PV_filter;
    std::vector< uhh2::Event::Handle<bool> > HLT_all;
    std::vector< uhh2::Event::Handle<bool> > b_MET_filters_all;
    uhh2::Event::Handle<int>    m_o_njj;  
    uhh2::Event::Handle<int>    m_o_njj_vbf;  

    //reco CHS jet variables
    uhh2::Event::Handle<float>  m_o_mjj; 
    uhh2::Event::Handle<float>  m_o_ptjj; 
    uhh2::Event::Handle<float>  m_o_etajj; 
    uhh2::Event::Handle<float>  m_o_phijj; 
    uhh2::Event::Handle<float>  m_o_pt_jet1;
    uhh2::Event::Handle<float>  m_o_pt_jet2;
    uhh2::Event::Handle<float>  m_o_eta_jet1;
    uhh2::Event::Handle<float>  m_o_eta_jet2;
    uhh2::Event::Handle<float>  m_o_phi_jet1;
    uhh2::Event::Handle<float>  m_o_phi_jet2;
    uhh2::Event::Handle<float>  m_o_mass_jet1;
    uhh2::Event::Handle<float>  m_o_mass_jet2; 
    uhh2::Event::Handle<bool>   jj_mergedVTruth_jet1;
    uhh2::Event::Handle<bool>   jj_mergedVTruth_jet2;
    uhh2::Event::Handle<bool>   jj_mergedZbbTruth_jet1;
    uhh2::Event::Handle<bool>   jj_mergedZbbTruth_jet2;
    uhh2::Event::Handle<bool>   jj_mergedHTruth_jet1;
    uhh2::Event::Handle<bool>   jj_mergedHTruth_jet2;
    uhh2::Event::Handle<bool>   jj_mergedTopTruth_jet1;
    uhh2::Event::Handle<bool>   jj_mergedTopTruth_jet2;
                
    //reco puppi softdrop variables
    uhh2::Event::Handle<float>  m_o_pt_softdrop_jet1;
    uhh2::Event::Handle<float>  m_o_pt_softdrop_jet2;
    uhh2::Event::Handle<float>  m_o_eta_softdrop_jet1;
    uhh2::Event::Handle<float>  m_o_eta_softdrop_jet2;
    uhh2::Event::Handle<float>  m_o_phi_softdrop_jet1;
    uhh2::Event::Handle<float>  m_o_phi_softdrop_jet2;
    uhh2::Event::Handle<float>  m_o_mpuppisoftdrop_jet1; 
    uhh2::Event::Handle<float>  m_o_mpuppisoftdrop_jet2; 
    uhh2::Event::Handle<int>    m_o_nSubJets_softdrop_jet1;
    uhh2::Event::Handle<int>    m_o_nSubJets_softdrop_jet2;
    uhh2::Event::Handle<float>  m_o_pt_softdrop_s1_jet1;
    uhh2::Event::Handle<float>  m_o_eta_softdrop_s1_jet1;
    uhh2::Event::Handle<float>  m_o_phi_softdrop_s1_jet1;
    uhh2::Event::Handle<float>  m_o_mass_softdrop_s1_jet1;
    uhh2::Event::Handle<float>  m_o_pt_softdrop_s2_jet1;
    uhh2::Event::Handle<float>  m_o_eta_softdrop_s2_jet1;
    uhh2::Event::Handle<float>  m_o_phi_softdrop_s2_jet1;
    uhh2::Event::Handle<float>  m_o_mass_softdrop_s2_jet1;
    uhh2::Event::Handle<float>  m_o_pt_softdrop_s1_jet2;
    uhh2::Event::Handle<float>  m_o_eta_softdrop_s1_jet2;
    uhh2::Event::Handle<float>  m_o_phi_softdrop_s1_jet2;
    uhh2::Event::Handle<float>  m_o_mass_softdrop_s1_jet2;
    uhh2::Event::Handle<float>  m_o_pt_softdrop_s2_jet2;
    uhh2::Event::Handle<float>  m_o_eta_softdrop_s2_jet2;
    uhh2::Event::Handle<float>  m_o_phi_softdrop_s2_jet2;
    uhh2::Event::Handle<float>  m_o_mass_softdrop_s2_jet2;
    uhh2::Event::Handle<float>  m_o_tau1_jet1; 
    uhh2::Event::Handle<float>  m_o_tau1_jet2; 
    uhh2::Event::Handle<float>  m_o_tau2_jet1; 
    uhh2::Event::Handle<float>  m_o_tau2_jet2; 
    uhh2::Event::Handle<float>  m_o_tau3_jet1; 
    uhh2::Event::Handle<float>  m_o_tau3_jet2; 
    uhh2::Event::Handle<float>  m_o_tau4_jet1; 
    uhh2::Event::Handle<float>  m_o_tau4_jet2; 
    uhh2::Event::Handle<float>  m_o_ecfN2_beta1_jet1; 
    uhh2::Event::Handle<float>  m_o_ecfN2_beta2_jet1; 
    uhh2::Event::Handle<float>  m_o_ecfN2_beta1_jet2; 
    uhh2::Event::Handle<float>  m_o_ecfN2_beta2_jet2; 
    uhh2::Event::Handle<float>  m_o_ecfN3_beta1_jet1; 
    uhh2::Event::Handle<float>  m_o_ecfN3_beta2_jet1; 
    uhh2::Event::Handle<float>  m_o_ecfN3_beta1_jet2; 
    uhh2::Event::Handle<float>  m_o_ecfN3_beta2_jet2;
    //uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_bbvsLight_jet1; 
    //uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_bbvsLight_jet2; 
    uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_ZHbbvsQCD_jet1; 
    uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_ZHbbvsQCD_jet2; 
    uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_HbbvsQCD_jet1; 
    uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_HbbvsQCD_jet2; 
    //uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_probHbb_jet1; 
    //uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_probHbb_jet2; 
    //uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_probQCDbb_jet1; 
    //uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_probQCDbb_jet2; 
    uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_WvsQCD_jet1; 
    uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_WvsQCD_jet2; 
    //uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_probWqq_jet1; 
    //uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_probWqq_jet2;	  
    //uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_proWcq_jet1; 
    //uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_proWcq_jet2;		 
    uhh2::Event::Handle<float>  m_o_MassIndependentDeepDoubleBvLJet_probHbb_jet1; 
    uhh2::Event::Handle<float>  m_o_MassIndependentDeepDoubleBvLJet_probHbb_jet2; 
    //uhh2::Event::Handle<float>  m_o_MassIndependentDeepDoubleBvLJet_probQCD_jet1; 
    //uhh2::Event::Handle<float>  m_o_MassIndependentDeepDoubleBvLJet_probQCD_jet2;     
    uhh2::Event::Handle<float>  m_o_DeepBoosted_WvsQCD_jet1; 
    uhh2::Event::Handle<float>  m_o_DeepBoosted_WvsQCD_jet2;  
    //uhh2::Event::Handle<float>  m_o_DeepBoosted_probWqq_jet1; 
    //uhh2::Event::Handle<float>  m_o_DeepBoosted_probWqq_jet2;  
    //uhh2::Event::Handle<float>  m_o_DeepBoosted_proWcq_jet1; 
    //uhh2::Event::Handle<float>  m_o_DeepBoosted_proWcq_jet2;  
    //uhh2::Event::Handle<float>  m_o_DeepBoosted_ZbbvsQCD_jet1;
    //uhh2::Event::Handle<float>  m_o_DeepBoosted_ZbbvsQCD_jet2;
    //uhh2::Event::Handle<float>  m_o_DeepBoosted_HbbvsQCD_jet1; 
    //uhh2::Event::Handle<float>  m_o_DeepBoosted_HbbvsQCD_jet2;  
    uhh2::Event::Handle<float>  m_o_DeepBoosted_ZHbbvsQCD_jet1; 
    uhh2::Event::Handle<float>  m_o_DeepBoosted_ZHbbvsQCD_jet2;
    //uhh2::Event::Handle<float>  m_o_DeepBoosted_probHbb_jet1; 
    //uhh2::Event::Handle<float>  m_o_DeepBoosted_probHbb_jet2;  
    //uhh2::Event::Handle<float>  m_o_DeepBoosted_probQCDbb_jet1; 
    //uhh2::Event::Handle<float>  m_o_DeepBoosted_probQCDbb_jet2;  
    uhh2::Event::Handle<float>  m_o_DeepDoubleBvLJet_probHbb_jet1; 
    uhh2::Event::Handle<float>  m_o_DeepDoubleBvLJet_probHbb_jet2;  
    //uhh2::Event::Handle<float>  m_o_DeepDoubleBvLJet_probQCD_jet1; 
    //uhh2::Event::Handle<float>  m_o_DeepDoubleBvLJet_probQCD_jet2; 
    uhh2::Event::Handle<float>  m_o_btag_DeepCSV_jet1;
    uhh2::Event::Handle<float>  m_o_btag_DeepCSV_jet2;
    uhh2::Event::Handle<float>  m_o_btag_DeepCSV_softdrop_s1_jet1;
    uhh2::Event::Handle<float>  m_o_btag_DeepCSV_softdrop_s2_jet1;
    uhh2::Event::Handle<float>  m_o_btag_DeepCSV_softdrop_s1_jet2;
    uhh2::Event::Handle<float>  m_o_btag_DeepCSV_softdrop_s2_jet2;
    uhh2::Event::Handle<int>    m_o_nSubJets_softdrop_btag_DeepCSVL_jet1;
    uhh2::Event::Handle<int>    m_o_nSubJets_softdrop_btag_DeepCSVL_jet2;
    uhh2::Event::Handle<int>    m_o_nSubJets_softdrop_btag_DeepCSVM_jet1;
    uhh2::Event::Handle<int>    m_o_nSubJets_softdrop_btag_DeepCSVM_jet2;
    uhh2::Event::Handle<int>    m_o_nSubJets_softdrop_btag_DeepCSVT_jet1;
    uhh2::Event::Handle<int>    m_o_nSubJets_softdrop_btag_DeepCSVT_jet2;    
    /*uhh2::Event::Handle<float>  m_o_btag_DeepFlavour_bb_jet1;
    uhh2::Event::Handle<float>  m_o_btag_DeepFlavour_bb_jet2;
    uhh2::Event::Handle<float>  m_o_btag_DeepFlavour_bb_softdrop_s1_jet1;
    uhh2::Event::Handle<float>  m_o_btag_DeepFlavour_bb_softdrop_s2_jet1;
    uhh2::Event::Handle<float>  m_o_btag_DeepFlavour_bb_softdrop_s1_jet2;
    uhh2::Event::Handle<float>  m_o_btag_DeepFlavour_bb_softdrop_s2_jet2;
    uhh2::Event::Handle<float>  m_o_btag_DeepFlavour_b_jet1;
    uhh2::Event::Handle<float>  m_o_btag_DeepFlavour_b_jet2;
    uhh2::Event::Handle<float>  m_o_btag_DeepFlavour_b_softdrop_s1_jet1;
    uhh2::Event::Handle<float>  m_o_btag_DeepFlavour_b_softdrop_s2_jet1;
    uhh2::Event::Handle<float>  m_o_btag_DeepFlavour_b_softdrop_s1_jet2;
    uhh2::Event::Handle<float>  m_o_btag_DeepFlavour_b_softdrop_s2_jet2;
    uhh2::Event::Handle<float>  m_o_btag_DeepJet_jet1;
    uhh2::Event::Handle<float>  m_o_btag_DeepJet_jet2;
    uhh2::Event::Handle<float>  m_o_btag_DeepJet_softdrop_s1_jet1;
    uhh2::Event::Handle<float>  m_o_btag_DeepJet_softdrop_s2_jet1;
    uhh2::Event::Handle<float>  m_o_btag_DeepJet_softdrop_s1_jet2;
    uhh2::Event::Handle<float>  m_o_btag_DeepJet_softdrop_s2_jet2;
    uhh2::Event::Handle<int>    m_o_nSubJets_softdrop_btag_DeepJetL_jet1;
    uhh2::Event::Handle<int>    m_o_nSubJets_softdrop_btag_DeepJetL_jet2;
    uhh2::Event::Handle<int>    m_o_nSubJets_softdrop_btag_DeepJetM_jet1;
    uhh2::Event::Handle<int>    m_o_nSubJets_softdrop_btag_DeepJetM_jet2;
    uhh2::Event::Handle<int>    m_o_nSubJets_softdrop_btag_DeepJetT_jet1;
    uhh2::Event::Handle<int>    m_o_nSubJets_softdrop_btag_DeepJetT_jet2;*/
                                                                     
    //reco puppi VBF jet variables                                                                                                                                                                                                                                             
    std::unique_ptr<AnalysisModule> VBFvariables;    

    //NLO weights for Vjets background samples                                                                                                                                                                                                                                            
    std::unique_ptr<AnalysisModule> NLOweights;    

    //gen CHS jets variable    
    uhh2::Event::Handle<float>  m_o_genmjj; 
    uhh2::Event::Handle<float>  m_o_genptjj; 
    uhh2::Event::Handle<float>  m_o_genetajj; 
    uhh2::Event::Handle<float>  m_o_genphijj; 
    uhh2::Event::Handle<float>  m_o_genpt_jet1;
    uhh2::Event::Handle<float>  m_o_genpt_jet2;
    uhh2::Event::Handle<float>  m_o_geneta_jet1;
    uhh2::Event::Handle<float>  m_o_geneta_jet2;
    uhh2::Event::Handle<float>  m_o_genphi_jet1;
    uhh2::Event::Handle<float>  m_o_genphi_jet2;
    uhh2::Event::Handle<float>  m_o_genmass_jet1;
    uhh2::Event::Handle<float>  m_o_genmass_jet2;
            
    //gen softdrop jets variable
    uhh2::Event::Handle<float>  m_o_pt_gen_softdrop_jet1;
    uhh2::Event::Handle<float>  m_o_pt_gen_softdrop_jet2;
    uhh2::Event::Handle<float>  m_o_eta_gen_softdrop_jet1;
    uhh2::Event::Handle<float>  m_o_eta_gen_softdrop_jet2;
    uhh2::Event::Handle<float>  m_o_phi_gen_softdrop_jet1;
    uhh2::Event::Handle<float>  m_o_phi_gen_softdrop_jet2;
    uhh2::Event::Handle<float>  m_o_mgensoftdrop_jet1; 
    uhh2::Event::Handle<float>  m_o_mgensoftdrop_jet2; 
    uhh2::Event::Handle<int>    m_o_nSubJets_gen_softdrop_jet1;
    uhh2::Event::Handle<int>    m_o_nSubJets_gen_softdrop_jet2;
    uhh2::Event::Handle<float>  m_o_tau1_gen_jet1; 
    uhh2::Event::Handle<float>  m_o_tau1_gen_jet2; 
    uhh2::Event::Handle<float>  m_o_tau2_gen_jet1; 
    uhh2::Event::Handle<float>  m_o_tau2_gen_jet2; 
    uhh2::Event::Handle<float>  m_o_tau3_gen_jet1; 
    uhh2::Event::Handle<float>  m_o_tau3_gen_jet2; 
    uhh2::Event::Handle<float>  m_o_tau4_gen_jet1; 
    uhh2::Event::Handle<float>  m_o_tau4_gen_jet2;     
    uhh2::Event::Handle<float>  m_o_ecfN2_beta1_gen_jet1; 
    uhh2::Event::Handle<float>  m_o_ecfN2_beta2_gen_jet1; 
    uhh2::Event::Handle<float>  m_o_ecfN2_beta1_gen_jet2; 
    uhh2::Event::Handle<float>  m_o_ecfN2_beta2_gen_jet2; 
    uhh2::Event::Handle<float>  m_o_ecfN3_beta1_gen_jet1; 
    uhh2::Event::Handle<float>  m_o_ecfN3_beta2_gen_jet1; 
    uhh2::Event::Handle<float>  m_o_ecfN3_beta1_gen_jet2; 
    uhh2::Event::Handle<float>  m_o_ecfN3_beta2_gen_jet2;     
    uhh2::Event::Handle<float>  m_o_pt_gen_softdrop_s1_jet1;
    uhh2::Event::Handle<float>  m_o_eta_gen_softdrop_s1_jet1;
    uhh2::Event::Handle<float>  m_o_phi_gen_softdrop_s1_jet1;
    uhh2::Event::Handle<float>  m_o_mass_gen_softdrop_s1_jet1;
    uhh2::Event::Handle<float>  m_o_pt_gen_softdrop_s2_jet1;
    uhh2::Event::Handle<float>  m_o_eta_gen_softdrop_s2_jet1;
    uhh2::Event::Handle<float>  m_o_phi_gen_softdrop_s2_jet1;
    uhh2::Event::Handle<float>  m_o_mass_gen_softdrop_s2_jet1;
    uhh2::Event::Handle<float>  m_o_pt_gen_softdrop_s1_jet2;
    uhh2::Event::Handle<float>  m_o_eta_gen_softdrop_s1_jet2;
    uhh2::Event::Handle<float>  m_o_phi_gen_softdrop_s1_jet2;
    uhh2::Event::Handle<float>  m_o_mass_gen_softdrop_s1_jet2;
    uhh2::Event::Handle<float>  m_o_pt_gen_softdrop_s2_jet2;
    uhh2::Event::Handle<float>  m_o_eta_gen_softdrop_s2_jet2;
    uhh2::Event::Handle<float>  m_o_phi_gen_softdrop_s2_jet2;
    uhh2::Event::Handle<float>  m_o_mass_gen_softdrop_s2_jet2;
    
    //puppi met variables
    uhh2::Event::Handle<float> m_o_met_pt;
    uhh2::Event::Handle<float> m_o_met_eta;
    uhh2::Event::Handle<float> m_o_met_phi;
    uhh2::Event::Handle<float> m_o_met_mass;
    uhh2::Event::Handle<float> m_o_met_sumEt;

    //variables for spike killer
    uhh2::Event::Handle<float> m_o_genHT;
    uhh2::Event::Handle<float> m_o_qscale;
    uhh2::Event::Handle<float> m_o_PU_pThat;
    uhh2::Event::Handle<bool> b_pt_over_genHT;
    uhh2::Event::Handle<bool> b_pt_over_qscale;
    uhh2::Event::Handle<bool> b_PUpthat_over_genHT;
    uhh2::Event::Handle<bool> b_spikekiller;
    
    // //cut values for decorrelation
   std::vector< std::unique_ptr<BruteForceDecorrelation> > bruteForce_Decorrelation_default_16;
   std::vector< std::unique_ptr<BruteForceDecorrelation> > bruteForce_Decorrelation_default_17;
   std::vector< std::unique_ptr<BruteForceDecorrelation> > bruteForce_Decorrelation_default_18;
   std::vector< std::unique_ptr<BruteForceDecorrelation> > bruteForce_Decorrelation_default_1617;
   std::vector< std::unique_ptr<BruteForceDecorrelation> > bruteForce_Decorrelation_default_161718;
   //std::vector< std::unique_ptr<BruteForceDecorrelation> > bruteForce_Decorrelation_smooth_16;
   //std::vector< std::unique_ptr<BruteForceDecorrelation> > bruteForce_Decorrelation_smooth_17;
   //std::vector< std::unique_ptr<BruteForceDecorrelation> > bruteForce_Decorrelation_smooth_1617;   
   
    //run numbers to apply vorrect JEC
    const int runnr_2016_Ab = 271036;
    const int runnr_2016_Ae = 271658;
    const int runnr_2016_Bb = 272007;
    const int runnr_2016_Be = 275376;
    const int runnr_2016_Cb = 275657;
    const int runnr_2016_Ce = 276283;
    const int runnr_2016_Db = 276315;
    const int runnr_2016_De = 276811;
    const int runnr_2016_Eb = 276831;
    const int runnr_2016_Ee = 277420;
    const int runnr_2016_Fb = 277772;
    const int runnr_2016_Fe = 278808;
    const int runnr_2016_Gb = 278820;
    const int runnr_2016_Ge = 280385;
    const int runnr_2016_Hb = 280919;
    const int runnr_2016_He = 284044;

    const int runnr_2017_Ab = 294927;
    const int runnr_2017_Ae = 297019;
    const int runnr_2017_Bb = 297046;
    const int runnr_2017_Be = 299329;
    const int runnr_2017_Cb = 299368;
    const int runnr_2017_Ce = 302029;
    const int runnr_2017_Db = 302030;
    const int runnr_2017_De = 303434;
    const int runnr_2017_Eb = 303824;
    const int runnr_2017_Ee = 304797;
    const int runnr_2017_Fb = 305040;
    const int runnr_2017_Fe = 306462;

    const int runnr_2018_Ab = 315252;
    const int runnr_2018_Ae = 316995;
    const int runnr_2018_Bb = 317080;
    const int runnr_2018_Be = 319310;
    const int runnr_2018_Cb = 319337;
    const int runnr_2018_Ce = 320065;
    const int runnr_2018_Db = 320673;
    const int runnr_2018_De = 325175;

    MuonId     MuId;
    ElectronId EleId;

};


UHHNtupleConverterModule::UHHNtupleConverterModule(Context & ctx){
    // In the constructor, the typical tasks are to initialize the
    // member variables, in particular the AnalysisModules such as
    // CommonModules or some cleaner module, Selections and Hists.
    // But you can do more and e.g. access the configuration, as shown below.
    
    cout << "Hello World from UHHNtupleConverterModule!" << endl;
    
    // If needed, access the configuration of the module here, e.g.:
    string testvalue = ctx.get("TestKey", "<not set>");
    cout << "TestKey in the configuration was: " << testvalue << endl;
    
    // If running in SFrame, the keys "dataset_version", "dataset_type", "dataset_lumi",
    // and "target_lumi" are set to the according values in the xml file. For CMSSW, these are
    // not set automatically, but can be set in the python config file.
    for(auto & kv : ctx.get_all()){
        cout << " " << kv.first << " = " << kv.second << endl;
    }
  

    Gen_printer.reset(new GenParticlesPrinter(ctx));

    std::cout << "----------------------------------------------------------------------------------------------------" << std::endl;

    year = extract_year(ctx);
    isMC = ctx.get("dataset_type") == "MC";
    totalEvents=0;
    totalGenEvents=0;
    totalGenEvents_LO=0;

    printGenparticle = false;
  
    isSignal = false;
    isVjet = false;
    Vcorr = "";
    TString sample = ctx.get("sample_name");
    if( sample.Contains("BulkGrav") or sample.Contains("Qstar") or sample.Contains("Wprime") or sample.Contains("Zprime") or sample.Contains("Radion")) isSignal = true;

    if(ctx.get("jet_sorting")=="random") sorting = Sorting::SORTING_RANDOM;
    else if(ctx.get("jet_sorting")=="btag") sorting = Sorting::SORTING_BYBTAG;
    else{
     std::cout << "WARNING: jet_sorting=="<<ctx.get("jet_sorting")<<" not among the options (btag,random). Set to random by default!" << std::endl;
     sorting = Sorting::SORTING_RANDOM;
    }
    
    if (isMC) xSec_ = m_xSec.getLumiWeight( ctx.get("sample_name") );//to be checked
    else xSec_ = 1;
    std::cout << "----------------------------------------------------------------------------------------------------" << std::endl;
    cout << "Cross section set to " << xSec_ << " for sample " << ctx.get("sample_name") << endl;
    
    hasBuggyPU2017 = false;
    TString pudirectory = ctx.get("pileup_directory");
    if(isMC && year == Year::is2017v2 && pudirectory.Contains("Pileup_QCD_PtBinned")){
     hasBuggyPU2017 = true;
     std::cout << "Sample " << sample << " has buggy PU: will use PU distribution " << pudirectory << std::endl;
    }
    
    MuId  = AndId<Muon>(MuonID(Muon::CutBasedIdTight), PtEtaCut(30., 2.4), MuonID(Muon::TkIsoLoose));
    EleId = AndId<Electron>(ElectronID_HEEP_RunII_25ns, PtEtaCut(35., 2.5));
    

    // 1. setup other modules. CommonModules and the JetCleaner:
    common.reset(new CommonModules());
    // TODO: configure common here, e.g. by 
    // calling common->set_*_id or common->disable_*
    // note that the JetCleaner is only kept for the sake of example;
    // instead of constructing a jetcleaner explicitly,
    // the cleaning can also be achieved with less code via CommonModules with:
    // common->set_jet_id(PtEtaCut(30.0, 2.4));
    // before the 'common->init(ctx)' line.
    common->disable_jersmear(); //JER are done manually
    common->disable_jec(); //JEC are done manually
    common->switch_jetPtSorter(true);
    common->disable_metfilters();
    common->disable_pvfilter();
    common->init(ctx);
    jetcleaner.reset(new JetCleaner(ctx, 200.0, 2.4)); //automatically run PFJetID Tight for CHS in the Common modules unless disable_jetpfidfilter() is run
    massCalc.reset(new SoftDropMassCalculator(ctx, true, "common/data/2018/puppiCorr.root"));

    vbfjetcleaner.reset(new JetCleaner(ctx, 30.0, 5.0, "jetsAk4Puppi")); //automatically run PFJetID Tight for CHS in the Common modules unless disable_jetpfidfilter() is run
    Ak4OverlapCleaner.reset(new Ak4RemovalModule(ctx,1.2,"jetsAk4Puppi"));
    //branches for output tree
    
    //event variables
    ctx.undeclare_all_event_output();

    b_isData = ctx.declare_event_output<bool>("isData");
    b_lumi = ctx.declare_event_output<int>("lumi");
    b_run = ctx.declare_event_output<int>("run");
    b_event = ctx.declare_event_output<int>("evt");
    b_xSec = ctx.declare_event_output<double>("xsec");
    b_pdf_x1 = ctx.declare_event_output<float>("pdf_x1");
    b_pdf_x2 = ctx.declare_event_output<float>("pdf_x2");
    b_pdf_scalePDF = ctx.declare_event_output<float>("pdf_scalePDF");
    b_weightGen = ctx.declare_event_output<float>("genWeight");
    b_weightGen_LO = ctx.declare_event_output<float>("genWeight_LO");
    b_weightPU = ctx.declare_event_output<float>("puWeight");
    b_weightTop = ctx.declare_event_output<float>("TopPTWeight");
    b_weightBTag = ctx.declare_event_output<float>("btagWeight");
    b_nTrueInt = ctx.declare_event_output<float>("nTrueInt");
    b_rho = ctx.declare_event_output<float>("rho");
    b_nVert = ctx.declare_event_output<int>("nVert");
    HLT_JJ = ctx.declare_event_output<bool>("HLT_JJ");
    b_passed_MET_filters = ctx.declare_event_output<bool>("passed_METfilters");
    b_passed_PV_filter = ctx.declare_event_output<bool>("passed_PVfilter");
    m_o_njj = ctx.declare_event_output<int>("njj");
    m_o_njj_vbf = ctx.declare_event_output<int>("njj_vbf");
        
    /* some filters and triggers*/	
    std::vector<std::string> metFilters;
    	
    //nb, not sure we need all the thresolds. Better to choose only one that was always unprescaled. To be checked.
    if( year == Year::is2018 || ((year == Year::is2017v2 || year == Year::is2017v1) && !sample.Contains("Run2017B")) ){//from b2g-18-002
     trigNames = {"HLT_PFHT1050_v*" ,"HLT_AK8PFJet500_v*",
                  "HLT_AK8PFJet360_TrimMass30_v*", "HLT_AK8PFJet380_TrimMass30_v*","HLT_AK8PFJet400_TrimMass30_v*","HLT_AK8PFJet420_TrimMass30_v*", //pt=400 first always unprescaled. To be checked.
		  "HLT_AK8PFHT750_TrimMass50_v*","HLT_AK8PFHT800_TrimMass50_v*","HLT_AK8PFHT850_TrimMass50_v*","HLT_AK8PFHT900_TrimMass50_v*"}; //ht=800 first always unprescaled. To be checked.
     if(isMC) metFilters = {"Flag_goodVertices","Flag_globalSuperTightHalo2016Filter","Flag_HBHENoiseFilter","Flag_HBHENoiseIsoFilter","Flag_EcalDeadCellTriggerPrimitiveFilter","Flag_BadPFMuonFilter"};     
     else metFilters = {"Flag_goodVertices","Flag_globalSuperTightHalo2016Filter","Flag_HBHENoiseFilter","Flag_HBHENoiseIsoFilter","Flag_EcalDeadCellTriggerPrimitiveFilter","Flag_BadPFMuonFilter","Flag_eeBadScFilter"};          
    }
    if( (year == Year::is2017v2 || year == Year::is2017v1) && sample.Contains("Run2017B") ){//from b2g-18-002
     trigNames = {"HLT_PFHT1050_v*" ,"HLT_AK8PFJet500_v*"};
     if(isMC) metFilters = {"Flag_goodVertices","Flag_globalSuperTightHalo2016Filter","Flag_HBHENoiseFilter","Flag_HBHENoiseIsoFilter","Flag_EcalDeadCellTriggerPrimitiveFilter","Flag_BadPFMuonFilter"};     
     else metFilters = {"Flag_goodVertices","Flag_globalSuperTightHalo2016Filter","Flag_HBHENoiseFilter","Flag_HBHENoiseIsoFilter","Flag_EcalDeadCellTriggerPrimitiveFilter","Flag_BadPFMuonFilter","Flag_eeBadScFilter"};          
    }
    else if( year == Year::is2016v2 || year == Year::is2016v3 ){//from b2g-17-001 
     if(sample.Contains("Run2016H")){
      trigNames = {"HLT_PFHT900_v*",
                   "HLT_PFJet400_v*","HLT_PFJet500_v*","HLT_PFJet450_v*",
		   "HLT_AK8PFJet400_v*","HLT_AK8PFJet450_v*","HLT_AK8PFJet500_v*",
		   "HLT_PFHT650_WideJetMJJ900DEtaJJ1p5_v*","HLT_PFHT650_WideJetMJJ950DEtaJJ1p5_v*",
		   "HLT_AK8PFJet360_TrimMass30_v*","HLT_AK8PFHT700_TrimR0p1PT0p03Mass50_v*"}; 
     }
     else{
      trigNames = {"HLT_PFHT800_v*","HLT_PFHT900_v*",
                   "HLT_PFJet400_v*","HLT_PFJet450_v*","HLT_PFJet500_v*",
		   "HLT_PFHT650_WideJetMJJ900DEtaJJ1p5_v*","HLT_PFHT650_WideJetMJJ950DEtaJJ1p5_v*",
		   "HLT_AK8PFJet360_TrimMass30_v*","HLT_AK8PFHT700_TrimR0p1PT0p03Mass50_v*"};		
     }				  
     if(isMC) metFilters = {"Flag_goodVertices","Flag_globalSuperTightHalo2016Filter","Flag_HBHENoiseFilter","Flag_HBHENoiseIsoFilter","Flag_EcalDeadCellTriggerPrimitiveFilter"};
     else metFilters = {"Flag_goodVertices","Flag_globalSuperTightHalo2016Filter","Flag_HBHENoiseFilter","Flag_HBHENoiseIsoFilter","Flag_EcalDeadCellTriggerPrimitiveFilter","Flag_eeBadScFilter"};
    }

    std::cout << "USING " << trigNames.size() << " SIGNAL TRIGGER PATHS:" << std::endl;
    for (auto it = trigNames.begin(), end = trigNames.end(); it != end; ++it){
      std::cout << *it << std::endl;
      trigger_selection.push_back(TriggerSelection(*it)); 
      HLT_all.push_back( ctx.declare_event_output<bool>((*it).replace((*it).end()-3,(*it).end(),"")) );
    }    
    std::cout << "----------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "USING " << metFilters.size()+1 << " MET FILTERS:" << std::endl;                
    for (auto it = metFilters.begin(), end = metFilters.end(); it != end; ++it){std::cout << *it << std::endl; metfilters.push_back( TriggerSelection(*it) ); b_MET_filters_all.push_back(ctx.declare_event_output<bool>(*it)); }
    std::cout << "Flag_EcalBadCalibSelection (for 2016 this is always = 1)" << std::endl;
    b_MET_filters_all.push_back( ctx.declare_event_output<bool>("Flag_EcalBadCalibSelection") );  
    
    ref_triggers.push_back(TriggerSelection("HLT_IsoMu27_v*"));
    ref_triggers.push_back(TriggerSelection("HLT_Mu50_v*"));

    /*done with triggers and filters*/        

    /* jec year dependent initialization */ 
    std::cout << "----------------------------------------------------------------------------------------------------" << std::endl;
    jec_jet_coll_AK8chs   = "AK8PFchs";
    jec_jet_coll_AK4puppi = "AK4PFPuppi";

    if(year == Year::is2016v2 || year == Year::is2016v3){
      jec_tag = "Summer16_07Aug2017";
      jec_ver = "11";
      JER_sf  = JERSmearing::SF_13TeV_Summer16_25nsV1;
      ResolutionFileName = "2016/Summer16_25nsV1_MC_PtResolution_AK4PFPuppi.txt";
    }
    else if(year == Year::is2017v1 || year == Year::is2017v2){
      jec_tag = "Fall17_17Nov2017";
      jec_ver = "32";
      JER_sf  = JERSmearing::SF_13TeV_Fall17_V3;
      ResolutionFileName = "2017/Fall17_V3_MC_PtResolution_AK4PFPuppi.txt";
    }
    else if(year == Year::is2018 ){
      jec_tag = "Autumn18";
      jec_ver = "19";
      sfFilename = "common/data/2018/Autumn18_V7_MC_SF_AK4PFPuppi.txt";
      ResolutionFileName = "2018/Autumn18_V7_MC_PtResolution_AK4PFPuppi.txt";
    }
    
    if(isMC){
      std::cout << "USING "<< year_str_map.at(year) << " MC JEC: "<< jec_tag << " V" << jec_ver << std::endl;
      std::cout << "for the following jet collections: " << jec_jet_coll_AK8chs << " " << jec_jet_coll_AK4puppi << std::endl;     
      std::cout << "Smearing: " << jec_jet_coll_AK8chs << " with year default " << std::endl;
      std::cout << "Smearing: " << jec_jet_coll_AK4puppi << " with "<< ResolutionFileName << std::endl;     
      jet_corrector.reset(new JetCorrector(ctx, JERFiles::JECFilesMC(jec_tag, jec_ver, jec_jet_coll_AK8chs)));
      jet_corrector_puppi.reset(new GenericJetCorrector(ctx, JERFiles::JECFilesMC(jec_tag, jec_ver, jec_jet_coll_AK4puppi),"jetsAk4Puppi"));
      jet_EResSmearer.reset(new JetResolutionSmearer(ctx));
      if (sfFilename != "") {                                                                                                                                                                                                           
	jetpuppi_EResSmearer.reset(new GenericJetResolutionSmearer(ctx,"jetsAk4Puppi","slimmedGenJets",sfFilename,ResolutionFileName));
      } else if (JER_sf.size() > 0) {
	jetpuppi_EResSmearer.reset(new GenericJetResolutionSmearer(ctx,"jetsAk4Puppi","slimmedGenJets",JER_sf,ResolutionFileName));
      } else {
	throw runtime_error("No valid JER SF either as text file nor JERSmearing::SFtype1");
      }
    }
    else{
      std::cout << "USING " << year_str_map.at(year) << " DATA JEC: "<< jec_tag << " V" << jec_ver << std::endl;
      if(year == Year::is2016v2 || year == Year::is2016v3){
	jet_corrector_2016_B.reset(new JetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK8chs, "B")));
	jet_corrector_2016_C.reset(new JetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK8chs, "C")));
	jet_corrector_2016_D.reset(new JetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK8chs, "D")));
	jet_corrector_2016_E.reset(new JetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK8chs, "E")));
	jet_corrector_2016_F.reset(new JetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK8chs, "F")));
	jet_corrector_2016_G.reset(new JetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK8chs, "G")));
	jet_corrector_2016_H.reset(new JetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK8chs, "H")));

	jet_corrector_puppi_2016_B.reset(new GenericJetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK4puppi, "B"), "jetsAk4Puppi"));
	jet_corrector_puppi_2016_C.reset(new GenericJetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK4puppi, "C"), "jetsAk4Puppi"));
	jet_corrector_puppi_2016_D.reset(new GenericJetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK4puppi, "D"), "jetsAk4Puppi"));
	jet_corrector_puppi_2016_E.reset(new GenericJetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK4puppi, "E"), "jetsAk4Puppi"));
	jet_corrector_puppi_2016_F.reset(new GenericJetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK4puppi, "F"), "jetsAk4Puppi"));
	jet_corrector_puppi_2016_G.reset(new GenericJetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK4puppi, "G"), "jetsAk4Puppi"));
	jet_corrector_puppi_2016_H.reset(new GenericJetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK4puppi, "H"), "jetsAk4Puppi"));
 
      }
      else if(year == Year::is2017v1 || year == Year::is2017v2){
	jet_corrector_2017_B.reset(new JetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK8chs, "B")));
	jet_corrector_2017_C.reset(new JetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK8chs, "C")));
	jet_corrector_2017_D.reset(new JetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK8chs, "D")));
	jet_corrector_2017_E.reset(new JetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK8chs, "E")));
	jet_corrector_2017_F.reset(new JetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK8chs, "F")));

	jet_corrector_puppi_2017_B.reset(new GenericJetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK4puppi, "B"), "jetsAk4Puppi"));
	jet_corrector_puppi_2017_C.reset(new GenericJetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK4puppi, "C"), "jetsAk4Puppi"));
	jet_corrector_puppi_2017_D.reset(new GenericJetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK4puppi, "D"), "jetsAk4Puppi"));
	jet_corrector_puppi_2017_E.reset(new GenericJetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK4puppi, "E"), "jetsAk4Puppi"));
	jet_corrector_puppi_2017_F.reset(new GenericJetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK4puppi, "F"), "jetsAk4Puppi"));
 
      }
      else if(year == Year::is2018 ){
        jet_corrector_2018_A.reset(new JetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK8chs, "A")));
        jet_corrector_2018_B.reset(new JetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK8chs, "B")));
        jet_corrector_2018_C.reset(new JetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK8chs, "C")));
        jet_corrector_2018_D.reset(new JetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK8chs, "D")));

        jet_corrector_puppi_2018_A.reset(new GenericJetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK4puppi, "A"), "jetsAk4Puppi"));
        jet_corrector_puppi_2018_B.reset(new GenericJetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK4puppi, "B"), "jetsAk4Puppi"));
        jet_corrector_puppi_2018_C.reset(new GenericJetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK4puppi, "C"), "jetsAk4Puppi"));
        jet_corrector_puppi_2018_D.reset(new GenericJetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll_AK4puppi, "D"), "jetsAk4Puppi"));
      }
    }
    std::cout << "----------------------------------------------------------------------------------------------------" << std::endl;
    /*end of year dependent initialization*/

	     

    //reco CHS jet variables
    m_o_mjj = ctx.declare_event_output<float>("jj_LV_mass"); 
    m_o_ptjj = ctx.declare_event_output<float>("jj_LV_pt"); 
    m_o_etajj = ctx.declare_event_output<float>("jj_LV_eta"); 
    m_o_phijj = ctx.declare_event_output<float>("jj_LV_phi"); 
    m_o_pt_jet1 = ctx.declare_event_output<float>("jj_l1_pt");
    m_o_pt_jet2 = ctx.declare_event_output<float>("jj_l2_pt");
    m_o_eta_jet1 = ctx.declare_event_output<float>("jj_l1_eta");
    m_o_eta_jet2 = ctx.declare_event_output<float>("jj_l2_eta");
    m_o_phi_jet1 = ctx.declare_event_output<float>("jj_l1_phi");
    m_o_phi_jet2 = ctx.declare_event_output<float>("jj_l2_phi");
    m_o_mass_jet1 = ctx.declare_event_output<float>("jj_l1_mass");
    m_o_mass_jet2 = ctx.declare_event_output<float>("jj_l2_mass");
    jj_mergedVTruth_jet1 = ctx.declare_event_output<bool>("jj_l1_mergedVTruth");
    jj_mergedVTruth_jet2 = ctx.declare_event_output<bool>("jj_l2_mergedVTruth");
    jj_mergedZbbTruth_jet1 = ctx.declare_event_output<bool>("jj_l1_mergedZbbTruth");
    jj_mergedZbbTruth_jet2 = ctx.declare_event_output<bool>("jj_l2_mergedZbbTruth");
    jj_mergedHTruth_jet1 = ctx.declare_event_output<bool>("jj_l1_mergedHTruth");
    jj_mergedHTruth_jet2 = ctx.declare_event_output<bool>("jj_l2_mergedHTruth");
    jj_mergedTopTruth_jet1 = ctx.declare_event_output<bool>("jj_l1_mergedTopTruth");
    jj_mergedTopTruth_jet2 = ctx.declare_event_output<bool>("jj_l2_mergedTopTruth");
                  
    //reco puppi softdrop variables    
    m_o_pt_softdrop_jet1 = ctx.declare_event_output<float>("jj_l1_softDrop_pt");
    m_o_pt_softdrop_jet2 = ctx.declare_event_output<float>("jj_l2_softDrop_pt");
    m_o_eta_softdrop_jet1 = ctx.declare_event_output<float>("jj_l1_softDrop_eta");
    m_o_eta_softdrop_jet2 = ctx.declare_event_output<float>("jj_l2_softDrop_eta");
    m_o_phi_softdrop_jet1 = ctx.declare_event_output<float>("jj_l1_softDrop_phi");
    m_o_phi_softdrop_jet2 = ctx.declare_event_output<float>("jj_l2_softDrop_phi");
    m_o_mpuppisoftdrop_jet1 = ctx.declare_event_output<float>("jj_l1_softDrop_mass");   
    m_o_mpuppisoftdrop_jet2 = ctx.declare_event_output<float>("jj_l2_softDrop_mass");  
    m_o_nSubJets_softdrop_jet1 = ctx.declare_event_output<int>("jj_l1_softDrop_nSubJets");
    m_o_nSubJets_softdrop_jet2 = ctx.declare_event_output<int>("jj_l2_softDrop_nSubJets");    
    m_o_pt_softdrop_s1_jet1 = ctx.declare_event_output<float>("jj_l1_softDrop_s1_pt");
    m_o_eta_softdrop_s1_jet1 = ctx.declare_event_output<float>("jj_l1_softDrop_s1_eta");
    m_o_phi_softdrop_s1_jet1 = ctx.declare_event_output<float>("jj_l1_softDrop_s1_phi");
    m_o_mass_softdrop_s1_jet1 = ctx.declare_event_output<float>("jj_l1_softDrop_s1_mass");
    m_o_pt_softdrop_s2_jet1 = ctx.declare_event_output<float>("jj_l1_softDrop_s2_pt");
    m_o_eta_softdrop_s2_jet1 = ctx.declare_event_output<float>("jj_l1_softDrop_s2_eta");
    m_o_phi_softdrop_s2_jet1 = ctx.declare_event_output<float>("jj_l1_softDrop_s2_phi");
    m_o_mass_softdrop_s2_jet1 = ctx.declare_event_output<float>("jj_l1_softDrop_s2_mass");    
    m_o_pt_softdrop_s1_jet2 = ctx.declare_event_output<float>("jj_l2_softDrop_s1_pt");
    m_o_eta_softdrop_s1_jet2 = ctx.declare_event_output<float>("jj_l2_softDrop_s1_eta");
    m_o_phi_softdrop_s1_jet2 = ctx.declare_event_output<float>("jj_l2_softDrop_s1_phi");
    m_o_mass_softdrop_s1_jet2 = ctx.declare_event_output<float>("jj_l2_softDrop_s1_mass");
    m_o_pt_softdrop_s2_jet2 = ctx.declare_event_output<float>("jj_l2_softDrop_s2_pt");
    m_o_eta_softdrop_s2_jet2 = ctx.declare_event_output<float>("jj_l2_softDrop_s2_eta");
    m_o_phi_softdrop_s2_jet2 = ctx.declare_event_output<float>("jj_l2_softDrop_s2_phi");
    m_o_mass_softdrop_s2_jet2 = ctx.declare_event_output<float>("jj_l2_softDrop_s2_mass");  
    m_o_tau1_jet1 = ctx.declare_event_output<float>("jj_l1_tau1");
    m_o_tau2_jet1 = ctx.declare_event_output<float>("jj_l1_tau2");
    m_o_tau3_jet1 = ctx.declare_event_output<float>("jj_l1_tau3");
    m_o_tau4_jet1 = ctx.declare_event_output<float>("jj_l1_tau4");
    m_o_tau1_jet2 = ctx.declare_event_output<float>("jj_l2_tau1");
    m_o_tau2_jet2 = ctx.declare_event_output<float>("jj_l2_tau2");
    m_o_tau3_jet2 = ctx.declare_event_output<float>("jj_l2_tau3");
    m_o_tau4_jet2 = ctx.declare_event_output<float>("jj_l2_tau4");
    m_o_ecfN2_beta1_jet1 = ctx.declare_event_output<float>("jj_l1_ecfN2_beta1");
    m_o_ecfN2_beta2_jet1 = ctx.declare_event_output<float>("jj_l1_ecfN2_beta2");
    m_o_ecfN2_beta1_jet2 = ctx.declare_event_output<float>("jj_l2_ecfN2_beta1");
    m_o_ecfN2_beta2_jet2 = ctx.declare_event_output<float>("jj_l2_ecfN2_beta2");
    m_o_ecfN3_beta1_jet1 = ctx.declare_event_output<float>("jj_l1_ecfN3_beta1");
    m_o_ecfN3_beta2_jet1 = ctx.declare_event_output<float>("jj_l1_ecfN3_beta2");
    m_o_ecfN3_beta1_jet2 = ctx.declare_event_output<float>("jj_l2_ecfN3_beta1");
    m_o_ecfN3_beta2_jet2 = ctx.declare_event_output<float>("jj_l2_ecfN3_beta2");
    //m_o_MassDecorrelatedDeepBoosted_bbvsLight_jet1 = ctx.declare_event_output<float>("jj_l1_MassDecorrelatedDeepBoosted_bbvsLight"); 
    //m_o_MassDecorrelatedDeepBoosted_bbvsLight_jet2 = ctx.declare_event_output<float>("jj_l2_MassDecorrelatedDeepBoosted_bbvsLight"); 
    m_o_MassDecorrelatedDeepBoosted_ZHbbvsQCD_jet1 = ctx.declare_event_output<float>("jj_l1_MassDecorrelatedDeepBoosted_ZHbbvsQCD"); 
    m_o_MassDecorrelatedDeepBoosted_ZHbbvsQCD_jet2 = ctx.declare_event_output<float>("jj_l2_MassDecorrelatedDeepBoosted_ZHbbvsQCD"); 
    m_o_MassDecorrelatedDeepBoosted_HbbvsQCD_jet1 = ctx.declare_event_output<float>("jj_l1_MassDecorrelatedDeepBoosted_HbbvsQCD"); 
    m_o_MassDecorrelatedDeepBoosted_HbbvsQCD_jet2 = ctx.declare_event_output<float>("jj_l2_MassDecorrelatedDeepBoosted_HbbvsQCD"); 
    //m_o_MassDecorrelatedDeepBoosted_probHbb_jet1 = ctx.declare_event_output<float>("jj_l1_MassDecorrelatedDeepBoosted_probHbb"); 
    //m_o_MassDecorrelatedDeepBoosted_probHbb_jet2 = ctx.declare_event_output<float>("jj_l2_MassDecorrelatedDeepBoosted_probHbb"); 
    //m_o_MassDecorrelatedDeepBoosted_probQCDbb_jet1 = ctx.declare_event_output<float>("jj_l1_MassDecorrelatedDeepBoosted_probQCDbb"); 
    //m_o_MassDecorrelatedDeepBoosted_probQCDbb_jet2 = ctx.declare_event_output<float>("jj_l2_MassDecorrelatedDeepBoosted_probQCDbb"); 
    m_o_MassDecorrelatedDeepBoosted_WvsQCD_jet1 = ctx.declare_event_output<float>("jj_l1_MassDecorrelatedDeepBoosted_WvsQCD"); 
    m_o_MassDecorrelatedDeepBoosted_WvsQCD_jet2 = ctx.declare_event_output<float>("jj_l2_MassDecorrelatedDeepBoosted_WvsQCD"); 
    //m_o_MassDecorrelatedDeepBoosted_probWqq_jet1 = ctx.declare_event_output<float>("jj_l1_MassDecorrelatedDeepBoosted_probWqq"); 
    //m_o_MassDecorrelatedDeepBoosted_probWqq_jet2 = ctx.declare_event_output<float>("jj_l2_MassDecorrelatedDeepBoosted_probWqq");     
    //m_o_MassDecorrelatedDeepBoosted_proWcq_jet1 = ctx.declare_event_output<float>("jj_l1_MassDecorrelatedDeepBoosted_proWcq"); 
    //m_o_MassDecorrelatedDeepBoosted_proWcq_jet2 = ctx.declare_event_output<float>("jj_l2_MassDecorrelatedDeepBoosted_proWcq");             
    m_o_MassIndependentDeepDoubleBvLJet_probHbb_jet1 = ctx.declare_event_output<float>("jj_l1_MassIndependentDeepDoubleBvLJetprobHbb_"); 
    m_o_MassIndependentDeepDoubleBvLJet_probHbb_jet2 = ctx.declare_event_output<float>("jj_l2_MassIndependentDeepDoubleBvLJet_probHbb"); 
    //m_o_MassIndependentDeepDoubleBvLJet_probQCD_jet1 = ctx.declare_event_output<float>("jj_l1_MassIndependentDeepDoubleBvLJet_probQCD"); 
    //m_o_MassIndependentDeepDoubleBvLJet_probQCD_jet2 = ctx.declare_event_output<float>("jj_l2_MassIndependentDeepDoubleBvLJet_probQCD");     
    m_o_DeepBoosted_WvsQCD_jet1 = ctx.declare_event_output<float>("jj_l1_DeepBoosted_WvsQCD");  
    m_o_DeepBoosted_WvsQCD_jet2 = ctx.declare_event_output<float>("jj_l2_DeepBoosted_WvsQCD"); 
    //m_o_DeepBoosted_probWqq_jet1 = ctx.declare_event_output<float>("jj_l1_DeepBoosted_probWqq");  
    //m_o_DeepBoosted_probWqq_jet2 = ctx.declare_event_output<float>("jj_l2_DeepBoosted_probWqq"); 
    //m_o_DeepBoosted_proWcq_jet1 = ctx.declare_event_output<float>("jj_l1_DeepBoosted_proWcq");  
    //m_o_DeepBoosted_proWcq_jet2 = ctx.declare_event_output<float>("jj_l2_DeepBoosted_proWcq"); 
    //m_o_DeepBoosted_ZbbvsQCD_jet1 = ctx.declare_event_output<float>("jj_l1_DeepBoosted_ZbbvsQCD"); 
    //m_o_DeepBoosted_ZbbvsQCD_jet2 = ctx.declare_event_output<float>("jj_l2_DeepBoosted_ZbbvsQCD");     
    //m_o_DeepBoosted_HbbvsQCD_jet1 = ctx.declare_event_output<float>("jj_l1_DeepBoosted_HbbvsQCD"); 
    //m_o_DeepBoosted_HbbvsQCD_jet2 = ctx.declare_event_output<float>("jj_l2_DeepBoosted_HbbvsQCD");  
    m_o_DeepBoosted_ZHbbvsQCD_jet1 = ctx.declare_event_output<float>("jj_l1_DeepBoosted_ZHbbvsQCD"); 
    m_o_DeepBoosted_ZHbbvsQCD_jet2 = ctx.declare_event_output<float>("jj_l2_DeepBoosted_ZHbbvsQCD");  
    //m_o_DeepBoosted_probHbb_jet1 = ctx.declare_event_output<float>("jj_l1_DeepBoosted_probHbb"); 
    //m_o_DeepBoosted_probHbb_jet2 = ctx.declare_event_output<float>("jj_l2_DeepBoosted_probHbb");  
    //m_o_DeepBoosted_probQCDbb_jet1 = ctx.declare_event_output<float>("jj_l1_DeepBoosted_probQCDbb"); 
    //m_o_DeepBoosted_probQCDbb_jet2 = ctx.declare_event_output<float>("jj_l2_DeepBoosted_probQCDbb");  
    m_o_DeepDoubleBvLJet_probHbb_jet1 = ctx.declare_event_output<float>("jj_l1_DeepDoubleBvLJet_probHbb"); 
    m_o_DeepDoubleBvLJet_probHbb_jet2 = ctx.declare_event_output<float>("jj_l2_DeepDoubleBvLJet_probHbb");  
    //m_o_DeepDoubleBvLJet_probQCD_jet1 = ctx.declare_event_output<float>("jj_l1_DeepDoubleBvLJet_probQCD"); 
    //m_o_DeepDoubleBvLJet_probQCD_jet2 = ctx.declare_event_output<float>("jj_l2_DeepDoubleBvLJet_probQCD");
    m_o_btag_DeepCSV_jet1 = ctx.declare_event_output<float>("jj_l1_btag_DeepCSV");
    m_o_btag_DeepCSV_jet2 = ctx.declare_event_output<float>("jj_l2_btag_DeepCSV");
    m_o_btag_DeepCSV_softdrop_s1_jet1 = ctx.declare_event_output<float>("jj_l1_softDrop_s1_btag_DeepCSV");
    m_o_btag_DeepCSV_softdrop_s2_jet1 = ctx.declare_event_output<float>("jj_l1_softDrop_s2_btag_DeepCSV");
    m_o_btag_DeepCSV_softdrop_s1_jet2 = ctx.declare_event_output<float>("jj_l2_softDrop_s1_btag_DeepCSV");
    m_o_btag_DeepCSV_softdrop_s2_jet2 = ctx.declare_event_output<float>("jj_l2_softDrop_s2_btag_DeepCSV");
    m_o_nSubJets_softdrop_btag_DeepCSVL_jet1 = ctx.declare_event_output<int>("jj_l1_softDrop_nSubJets_btag_DeepCSVL");
    m_o_nSubJets_softdrop_btag_DeepCSVL_jet2 = ctx.declare_event_output<int>("jj_l2_softDrop_nSubJets_btag_DeepCSVL");
    m_o_nSubJets_softdrop_btag_DeepCSVM_jet1 = ctx.declare_event_output<int>("jj_l1_softDrop_nSubJets_btag_DeepCSVM");
    m_o_nSubJets_softdrop_btag_DeepCSVM_jet2 = ctx.declare_event_output<int>("jj_l2_softDrop_nSubJets_btag_DeepCSVM");
    m_o_nSubJets_softdrop_btag_DeepCSVT_jet1 = ctx.declare_event_output<int>("jj_l1_softDrop_nSubJets_btag_DeepCSVT");
    m_o_nSubJets_softdrop_btag_DeepCSVT_jet2 = ctx.declare_event_output<int>("jj_l2_softDrop_nSubJets_btag_DeepCSVT");    
    /*m_o_btag_DeepFlavour_bb_jet1 = ctx.declare_event_output<float>("jj_l1_btag_DeepFlavour_bb");
    m_o_btag_DeepFlavour_bb_jet2 = ctx.declare_event_output<float>("jj_l2_btag_DeepFlavour_bb");
    m_o_btag_DeepFlavour_bb_softdrop_s1_jet1 = ctx.declare_event_output<float>("jj_l1_softDrop_s1_btag_DeepFlavour_bb");
    m_o_btag_DeepFlavour_bb_softdrop_s2_jet1 = ctx.declare_event_output<float>("jj_l1_softDrop_s2_btag_DeepFlavour_bb");
    m_o_btag_DeepFlavour_bb_softdrop_s1_jet2 = ctx.declare_event_output<float>("jj_l2_softDrop_s1_btag_DeepFlavour_bb");
    m_o_btag_DeepFlavour_bb_softdrop_s2_jet2 = ctx.declare_event_output<float>("jj_l2_softDrop_s2_btag_DeepFlavour_bb");
    m_o_btag_DeepFlavour_b_jet1 = ctx.declare_event_output<float>("jj_l1_btag_DeepFlavour_b");
    m_o_btag_DeepFlavour_b_jet2 = ctx.declare_event_output<float>("jj_l2_btag_DeepFlavour_b");
    m_o_btag_DeepFlavour_b_softdrop_s1_jet1 = ctx.declare_event_output<float>("jj_l1_softDrop_s1_btag_DeepFlavour_b");
    m_o_btag_DeepFlavour_b_softdrop_s2_jet1 = ctx.declare_event_output<float>("jj_l1_softDrop_s2_btag_DeepFlavour_b");
    m_o_btag_DeepFlavour_b_softdrop_s1_jet2 = ctx.declare_event_output<float>("jj_l2_softDrop_s1_btag_DeepFlavour_b");
    m_o_btag_DeepFlavour_b_softdrop_s2_jet2 = ctx.declare_event_output<float>("jj_l2_softDrop_s2_btag_DeepFlavour_b"); 
    m_o_btag_DeepJet_jet1 = ctx.declare_event_output<float>("jj_l1_btag_DeepJet");
    m_o_btag_DeepJet_jet2 = ctx.declare_event_output<float>("jj_l2_btag_DeepJet");
    m_o_btag_DeepJet_softdrop_s1_jet1 = ctx.declare_event_output<float>("jj_l1_softDrop_s1_btag_DeepJet");
    m_o_btag_DeepJet_softdrop_s2_jet1 = ctx.declare_event_output<float>("jj_l1_softDrop_s2_btag_DeepJet");
    m_o_btag_DeepJet_softdrop_s1_jet2 = ctx.declare_event_output<float>("jj_l2_softDrop_s1_btag_DeepJet");
    m_o_btag_DeepJet_softdrop_s2_jet2 = ctx.declare_event_output<float>("jj_l2_softDrop_s2_btag_DeepJet");
    m_o_nSubJets_softdrop_btag_DeepJetL_jet1 = ctx.declare_event_output<int>("jj_l1_softDrop_nSubJets_btag_DeepJetL");
    m_o_nSubJets_softdrop_btag_DeepJetL_jet2 = ctx.declare_event_output<int>("jj_l2_softDrop_nSubJets_btag_DeepJetL");
    m_o_nSubJets_softdrop_btag_DeepJetM_jet1 = ctx.declare_event_output<int>("jj_l1_softDrop_nSubJets_btag_DeepJetM");
    m_o_nSubJets_softdrop_btag_DeepJetM_jet2 = ctx.declare_event_output<int>("jj_l2_softDrop_nSubJets_btag_DeepJetM");
    m_o_nSubJets_softdrop_btag_DeepJetT_jet1 = ctx.declare_event_output<int>("jj_l1_softDrop_nSubJets_btag_DeepJetT");
    m_o_nSubJets_softdrop_btag_DeepJetT_jet2 = ctx.declare_event_output<int>("jj_l2_softDrop_nSubJets_btag_DeepJetT"); */  
                
    //reco puppi VBF jet variables
    VBFvariables.reset(new VBFvariable(ctx,"jetsAk4Puppi"));

    //NLO weights
    if( sample.Contains("WJets") or sample.Contains("ZJets")) { 
      if( sample.Contains("WJets")) {
	Vcorr="WJets";  
      }
      if( sample.Contains("ZJets")){
	Vcorr="ZJets";
      } 
      isVjet = true;
      if(PRINT) cout << "********* isVjet " << isVjet << " " << Vcorr << endl; 
      NLOweights.reset(new NLOweight(ctx,"NLOweights/"+Vcorr+"Corr.root"));
    }

    //gen CHS variable         
    m_o_genmjj = ctx.declare_event_output<float>("jj_gen_partialMass");  
    m_o_genptjj = ctx.declare_event_output<float>("jj_gen_LV_pt");  
    m_o_genetajj = ctx.declare_event_output<float>("jj_gen_LV_eta"); 
    m_o_genphijj = ctx.declare_event_output<float>("jj_gen_LV_phi");
    m_o_genpt_jet1 = ctx.declare_event_output<float>("jj_l1_gen_pt");
    m_o_genpt_jet2 = ctx.declare_event_output<float>("jj_l2_gen_pt");
    m_o_geneta_jet1 = ctx.declare_event_output<float>("jj_l1_gen_eta");
    m_o_geneta_jet2 = ctx.declare_event_output<float>("jj_l2_gen_eta");
    m_o_genphi_jet1 = ctx.declare_event_output<float>("jj_l1_gen_phi");
    m_o_genphi_jet2 = ctx.declare_event_output<float>("jj_l2_gen_phi");
    m_o_genmass_jet1 = ctx.declare_event_output<float>("jj_l1_gen_mass");
    m_o_genmass_jet2 = ctx.declare_event_output<float>("jj_l2_gen_mass");
            
    //gen softdrop jets variable        
    m_o_pt_gen_softdrop_jet1 = ctx.declare_event_output<float>("jj_l1_gen_softDrop_pt");
    m_o_pt_gen_softdrop_jet2 = ctx.declare_event_output<float>("jj_l2_gen_softDrop_pt");
    m_o_eta_gen_softdrop_jet1 = ctx.declare_event_output<float>("jj_l1_gen_softDrop_eta");
    m_o_eta_gen_softdrop_jet2 = ctx.declare_event_output<float>("jj_l2_gen_softDrop_eta");
    m_o_phi_gen_softdrop_jet1 = ctx.declare_event_output<float>("jj_l1_gen_softDrop_phi");
    m_o_phi_gen_softdrop_jet2 = ctx.declare_event_output<float>("jj_l2_gen_softDrop_phi");
    m_o_mgensoftdrop_jet1 = ctx.declare_event_output<float>("jj_l1_gen_softDrop_mass");
    m_o_mgensoftdrop_jet2 = ctx.declare_event_output<float>("jj_l2_gen_softDrop_mass");
    m_o_nSubJets_gen_softdrop_jet1 = ctx.declare_event_output<int>("jj_l1_gen_softDrop_nSubJets");
    m_o_nSubJets_gen_softdrop_jet2 = ctx.declare_event_output<int>("jj_l2_gen_softDrop_nSubJets");
    m_o_pt_gen_softdrop_s1_jet1 = ctx.declare_event_output<float>("jj_l1_gen_softDrop_s1_pt");
    m_o_eta_gen_softdrop_s1_jet1 = ctx.declare_event_output<float>("jj_l1_gen_softDrop_s1_eta");
    m_o_phi_gen_softdrop_s1_jet1 = ctx.declare_event_output<float>("jj_l1_gen_softDrop_s1_phi");
    m_o_mass_gen_softdrop_s1_jet1 = ctx.declare_event_output<float>("jj_l1_gen_softDrop_s1_mass");
    m_o_pt_gen_softdrop_s2_jet1 = ctx.declare_event_output<float>("jj_l1_gen_softDrop_s2_pt");
    m_o_eta_gen_softdrop_s2_jet1 = ctx.declare_event_output<float>("jj_l1_gen_softDrop_s2_eta");
    m_o_phi_gen_softdrop_s2_jet1 = ctx.declare_event_output<float>("jj_l1_gen_softDrop_s2_phi");
    m_o_mass_gen_softdrop_s2_jet1 = ctx.declare_event_output<float>("jj_l1_gen_softDrop_s2_mass");    
    m_o_pt_gen_softdrop_s1_jet2 = ctx.declare_event_output<float>("jj_l2_gen_softDrop_s1_pt");
    m_o_eta_gen_softdrop_s1_jet2 = ctx.declare_event_output<float>("jj_l2_gen_softDrop_s1_eta");
    m_o_phi_gen_softdrop_s1_jet2 = ctx.declare_event_output<float>("jj_l2_gen_softDrop_s1_phi");
    m_o_mass_gen_softdrop_s1_jet2 = ctx.declare_event_output<float>("jj_l2_gen_softDrop_s1_mass");
    m_o_pt_gen_softdrop_s2_jet2 = ctx.declare_event_output<float>("jj_l2_gen_softDrop_s2_pt");
    m_o_eta_gen_softdrop_s2_jet2 = ctx.declare_event_output<float>("jj_l2_gen_softDrop_s2_eta");
    m_o_phi_gen_softdrop_s2_jet2 = ctx.declare_event_output<float>("jj_l2_gen_softDrop_s2_phi");
    m_o_mass_gen_softdrop_s2_jet2 = ctx.declare_event_output<float>("jj_l2_gen_softDrop_s2_mass");      
    m_o_tau1_gen_jet1 = ctx.declare_event_output<float>("jj_l1_gen_tau1");
    m_o_tau2_gen_jet1 = ctx.declare_event_output<float>("jj_l1_gen_tau2");
    m_o_tau3_gen_jet1 = ctx.declare_event_output<float>("jj_l1_gen_tau3");
    m_o_tau4_gen_jet1 = ctx.declare_event_output<float>("jj_l1_gen_tau4");
    m_o_tau1_gen_jet2 = ctx.declare_event_output<float>("jj_l2_gen_tau1");
    m_o_tau2_gen_jet2 = ctx.declare_event_output<float>("jj_l2_gen_tau2");
    m_o_tau3_gen_jet2 = ctx.declare_event_output<float>("jj_l2_gen_tau3");
    m_o_tau4_gen_jet2 = ctx.declare_event_output<float>("jj_l2_gen_tau4");
    m_o_ecfN2_beta1_gen_jet1 = ctx.declare_event_output<float>("jj_l1_gen_ecfN2_beta1");
    m_o_ecfN2_beta2_gen_jet1 = ctx.declare_event_output<float>("jj_l1_gen_ecfN2_beta2");
    m_o_ecfN2_beta1_gen_jet2 = ctx.declare_event_output<float>("jj_l2_gen_ecfN2_beta1");
    m_o_ecfN2_beta2_gen_jet2 = ctx.declare_event_output<float>("jj_l2_gen_ecfN2_beta2");
    m_o_ecfN3_beta1_gen_jet1 = ctx.declare_event_output<float>("jj_l1_gen_ecfN2_beta1");
    m_o_ecfN3_beta2_gen_jet1 = ctx.declare_event_output<float>("jj_l1_gen_ecfN2_beta2");
    m_o_ecfN3_beta1_gen_jet2 = ctx.declare_event_output<float>("jj_l2_gen_ecfN2_beta1");
    m_o_ecfN3_beta2_gen_jet2 = ctx.declare_event_output<float>("jj_l2_gen_ecfN2_beta2"); 

    //puppi met variables            
    m_o_met_pt = ctx.declare_event_output<float>("met_pt");
    m_o_met_eta = ctx.declare_event_output<float>("met_eta");
    m_o_met_phi = ctx.declare_event_output<float>("met_phi");
    m_o_met_mass = ctx.declare_event_output<float>("met_mass");
    m_o_met_sumEt = ctx.declare_event_output<float>("met_sumEt");
         
    //variables for spike killer
    m_o_genHT = ctx.declare_event_output<float>("genHT");
    m_o_qscale = ctx.declare_event_output<float>("qscale");
    m_o_PU_pThat = ctx.declare_event_output<float>("PU_pThat");
    b_pt_over_genHT = ctx.declare_event_output<bool>("b_pt_over_genHT");
    b_pt_over_qscale = ctx.declare_event_output<bool>("b_pt_over_qscale");
    b_PUpthat_over_genHT = ctx.declare_event_output<bool>("b_PUpthat_over_genHT");
    b_spikekiller = ctx.declare_event_output<bool>("b_spikekiller");
    
    // //cut values for decorrelation
    string percentage[8] = { "_0p02", "_0p03", "_0p05", "_0p10", "_0p15", "_0p20", "_0p30", "_0p50" };
    for(int i=0; i<8; ++i){
     bruteForce_Decorrelation_default_16.emplace_back(new BruteForceDecorrelation(ctx, percentage[i], "default_16"));
     bruteForce_Decorrelation_default_17.emplace_back(new BruteForceDecorrelation(ctx, percentage[i], "default_17"));
     bruteForce_Decorrelation_default_18.emplace_back(new BruteForceDecorrelation(ctx, percentage[i], "default_18"));
     bruteForce_Decorrelation_default_1617.emplace_back(new BruteForceDecorrelation(ctx, percentage[i], "default_1617")); 
     bruteForce_Decorrelation_default_161718.emplace_back(new BruteForceDecorrelation(ctx, percentage[i], "default_161718")); 
     //bruteForce_Decorrelation_smooth_16.emplace_back(new BruteForceDecorrelation(ctx, percentage[i], "smooth_16"));
     //bruteForce_Decorrelation_smooth_17.emplace_back(new BruteForceDecorrelation(ctx, percentage[i], "smooth_17"));
     //bruteForce_Decorrelation_smooth_1617.emplace_back(new BruteForceDecorrelation(ctx, percentage[i], "smooth_1617"));
    }    
        
    // 2. set up selections
    muon_sel.reset(new MuonVeto(MuId,0.8)); // see UHHNtupleConverterSelections
    electron_sel.reset(new ElectronVeto(EleId,0.8)); // see UHHNtupleConverterSelections
    njet_sel.reset(new NJetSelection(2)); // see common/include/NSelections.h
    dijet_sel.reset(new DijetSelection(1.3,700)); // see UHHNtupleConverterSelections
    dijet_sel_loose.reset(new DijetSelection(1.3,0.0)); // see UHHNtupleConverterSelections
    vbf_sel.reset(new VBFjetSelection(ctx,"jetsAk4Puppi",4.5f,800.0f)); // see UHHNtupleConverterSelections
    genHbbEvent_sel.reset(new GenHbbEventSelection());
    genVqqEvent_sel.reset(new GenVqqEventSelection());
    genTopHadrEvent_sel.reset(new GenTopHadrEventSelection());
    
    PrimaryVertexId pvid=StandardPrimaryVertexId();
    pvfilter.reset(new NPVSelection(1,-1,pvid) );
    
    isTTbar = isMC && boost::algorithm::contains(ctx.get("dataset_version", ""),"ttbar");
    if( isTTbar ){
     cout << "Running over ttbar sample -> calculate top pt weight" << endl;
     top_pt_weight = ctx.get_handle<float>("weight_ttbar");
     top_pt_reweight.reset(new TopPtReweight(ctx,0.0615,-0.0005,"","weight_ttbar",false,1.));
    }

    // 3. Set up Hists classes:
    h_nocuts.reset(new UHHNtupleConverterHists(ctx, "NoCuts"));
    h_njet.reset(new UHHNtupleConverterHists(ctx, "Njet"));
    h_dijet.reset(new UHHNtupleConverterHists(ctx, "Dijet")); 
    h_trig.reset(new TriggerHists(ctx,"Trigger",trigNames));
       

    
}


bool UHHNtupleConverterModule::process(Event & event) {
    // This is the main procedure, called for each event. Typically,
    // do some pre-processing by calling the modules' process method
    // of the modules constructed in the constructor (1).
    // Then, test whether the event passes some selection and -- if yes --
    // use it to fill the histograms (2).
    // Finally, decide whether or not to keep the event in the output (3);
    // this is controlled by the return value of this method: If it
    // returns true, the event is kept; if it returns false, the event
    // is thrown away.
               
    if(PRINT) cout << "UHHNtupleConverterModule: Starting to process event (runid, eventid) = (" << event.run << ", " << event.event << "); weight = " << event.weight << endl;
    if(printGenparticle)    Gen_printer->process(event);     
    
    float genWeight = 1;
    // the LO PDF generator weight is better initialized to an unphysical number to make more clear when the LO PDF reweighting does not work
    float genWeight_LO = -99.;
    if( isMC ){
      genWeight = event.genInfo->weights().at(0);
      // In many 2017 and 2018 MG (LO) signal samples, reweight to LO PDF set since central NNLO PDF set (NNPDF31_nnlo_hessian_pdfas or NNPDF31_nnlo_as_0118_nf_4) is not positive definite
      if( isSignal && (year == Year::is2018 || year == Year::is2017v2 || year == Year::is2017v1) ){
        if(event.genInfo->systweights().size()==0){
            // Pure Pythia samples that don't need reweighting (already LO PDF)
            genWeight_LO = genWeight;
        }
        else if(event.genInfo->systweights().size()==882){
            // Scenario 1: reweight central PDF NNPDF31_nnlo_as_0118_nf_4 (LHAPDF ID: 320900) to NNPDF30_lo_as_0130_nf_4 (LHAPDF ID: 263400) -- 4FS
            // 9 scale variations incl. nominal, assuming central PDF 320900
            // PDF set list ~close to https://github.com/cms-sw/genproductions/blob/18702a67988278c7f34cecdbd398f83684076bb0/MetaData/pdflist_4f_2017.dat, but different first set 
            genWeight_LO = (event.genInfo->systweights()[777] / event.genInfo->systweights()[0]) * genWeight;
        }
        else if(event.genInfo->systweights().size()==918){
            // Scenario 2: central PDF already LO: NNPDF30_lo_as_0130_nf_4 (LHAPDF ID: 263400) -- 4FS
            // 45 scale variations incl. nominal, assuming central PDF 263400
            // PDF set list ~from https://github.com/cms-sw/genproductions/blob/18702a67988278c7f34cecdbd398f83684076bb0/MetaData/pdflist_4f_2017.dat
            genWeight_LO = genWeight;
        }
        else if(event.genInfo->systweights().size()==919){
            // Scenario 3: reweight central PDF NNPDF31_nnlo_as_0118_nf_4 (LHAPDF ID: 320900) to NNPDF30_lo_as_0130_nf_4 (LHAPDF ID: 263400) -- 4FS
            // 45 scale variations incl. nominal, assuming central PDF 320900
            // PDF set list ~from https://github.com/cms-sw/genproductions/blob/18702a67988278c7f34cecdbd398f83684076bb0/MetaData/pdflist_4f_2017.dat
            genWeight_LO = (event.genInfo->systweights()[814] / event.genInfo->systweights()[0]) * genWeight;
        }
        else if(event.genInfo->systweights().size()==1019){
            // Scenario 4: central PDF already LO: NNPDF30_lo_as_0130_nf_4 (LHAPDF ID: 263400) -- 4FS
            // 45 scale variations incl. nominal, assuming central PDF 263400
            // PDF set list ~from https://github.com/cms-sw/genproductions/blob/e32b6350765c614c155f2ab5c17547eedfdeab43/MetaData/pdflist_4f_2017.dat
            genWeight_LO = genWeight;
        }
        else if(event.genInfo->systweights().size()==1115){
            // Scenario 5: central PDF already LO: NNPDF30_lo_as_0130 (LHAPDF ID: 263000) -- 5FS because of proton definition in MG proc card
            // 45 scale variations incl. nominal, assuming central PDF 263000
            // PDF set list ~from https://github.com/cms-sw/genproductions/blob/18702a67988278c7f34cecdbd398f83684076bb0/MetaData/pdflist_5f_2017.dat
            genWeight_LO = genWeight;
        }
        else if(event.genInfo->systweights().size()==1116){
            // Scenario 6: reweight central PDF NNPDF31_nnlo_hessian_pdfas (LHAPDF ID: 306000) to NNPDF30_lo_as_0130 (LHAPDF ID: 263000) -- 5FS because of proton definition in MG proc card
            // 45 scale variations incl. nominal, assuming central PDF 306000
            // PDF set list ~from https://github.com/cms-sw/genproductions/blob/18702a67988278c7f34cecdbd398f83684076bb0/MetaData/pdflist_5f_2017.dat
            genWeight_LO = (event.genInfo->systweights()[1115] / event.genInfo->systweights()[0]) * genWeight;
	    }
        else{
            cout << "WARNING: encountered unexpected LHE weight scenario, with number of weights: " << event.genInfo->systweights().size() << endl;
            cout << "         Please check which LHE weight to take to reweight to LO PDF" << endl;
        }
      }
      else{
        // In 2016 signal samples (or background), no LO PDF reweighting should be done; set LO weights to default weights for simplicity
        genWeight_LO = genWeight;
      }
    }
    totalGenEvents+=genWeight;
    totalGenEvents_LO+=genWeight_LO;
    totalEvents+=1;
    
    //PU filter for 2017 buggy samples
    if( hasBuggyPU2017 && event.genInfo->pileup_TrueNumInteractions() < 10 && event.genInfo->pileup_TrueNumInteractions() > 75){
     return false;
    }
    
    // 1. run all modules other modules.
    bool common_sel = common->process(event);
    if(!common_sel) return false;
    h_nocuts->fill(event);       
    if(PRINT)   cout << " common modules done " << endl; 
    
    //calculate top pt weight
    if( isTTbar ) top_pt_reweight->process(event); 

    bool passedMETFilters = true;
    for(unsigned int i=0; i<metfilters.size(); ++i){
     if(!metfilters[i].passes(event)) passedMETFilters = false;
     event.set(b_MET_filters_all[i],metfilters[i].passes(event));
    }
    if( !event.passEcalBadCalib ) passedMETFilters = false;
    event.set(b_MET_filters_all[metfilters.size()],event.passEcalBadCalib);
    event.set(b_passed_MET_filters,passedMETFilters);
    event.set(b_passed_PV_filter,pvfilter->passes(event));

    if(PRINT)     cout << " met filters done" << endl;
    	
    /* Apply JEC */
    if(isMC){
      jet_corrector->process(event);
      jet_corrector_puppi->process(event);
      jet_corrector->correct_met(event);
      jet_EResSmearer->process(event);
      jetpuppi_EResSmearer->process(event);
    }
    else{
      //2016                                                                                                                                                                                                                                                                   
      if(event.run >= runnr_2016_Bb && event.run <= runnr_2016_Be){
	jet_corrector_2016_B->process(event);
	jet_corrector_2016_B->correct_met(event);
      }
      else if(event.run >= runnr_2016_Cb && event.run <= runnr_2016_Ce){
	jet_corrector_2016_C->process(event);
	jet_corrector_puppi_2016_C->process(event);
	jet_corrector_2016_C->correct_met(event);
      }
      else if(event.run >= runnr_2016_Db && event.run <= runnr_2016_De){
	jet_corrector_2016_D->process(event);
	jet_corrector_puppi_2016_D->process(event);
	jet_corrector_2016_D->correct_met(event);
      }
      else if(event.run >= runnr_2016_Eb && event.run <= runnr_2016_Ee){
        jet_corrector_2016_E->process(event);
        jet_corrector_puppi_2016_E->process(event);
	jet_corrector_2016_E->correct_met(event);
      }
      else if(event.run >= runnr_2016_Fb && event.run <= runnr_2016_Fe){
	jet_corrector_2016_F->process(event);
	jet_corrector_puppi_2016_F->process(event);
	jet_corrector_2016_F->correct_met(event);
      }
      else if(event.run >= runnr_2016_Gb && event.run <= runnr_2016_Ge){
	jet_corrector_2016_G->process(event);
	jet_corrector_puppi_2016_G->process(event);
	jet_corrector_2016_G->correct_met(event);
      }
      else if(event.run >= runnr_2016_Hb && event.run <= runnr_2016_He){
	jet_corrector_2016_H->process(event);
	jet_corrector_puppi_2016_H->process(event);
	jet_corrector_2016_H->correct_met(event);
      }
      //2017                                                                                                                                                                                                                                                                 
      if(event.run >= runnr_2017_Bb && event.run <= runnr_2017_Be){
	jet_corrector_2017_B->process(event);
	jet_corrector_puppi_2017_B->process(event);
	jet_corrector_2017_B->correct_met(event);
      }
      else if(event.run >= runnr_2017_Cb && event.run <= runnr_2017_Ce){
	jet_corrector_2017_C->process(event);
	jet_corrector_puppi_2017_C->process(event);
	jet_corrector_2017_C->correct_met(event);
      }
      else if(event.run >= runnr_2017_Db && event.run <= runnr_2017_De){
	jet_corrector_2017_D->process(event);
	jet_corrector_puppi_2017_D->process(event);
	jet_corrector_2017_D->correct_met(event);
      }
      else if(event.run >= runnr_2017_Eb && event.run <= runnr_2017_Ee){
	jet_corrector_2017_E->process(event);
	jet_corrector_puppi_2017_E->process(event);
	jet_corrector_2017_E->correct_met(event);
      }
      else if(event.run >= runnr_2017_Fb && event.run <= runnr_2017_Fe){
	jet_corrector_2017_F->process(event);
	jet_corrector_puppi_2017_F->process(event);
	jet_corrector_2017_F->correct_met(event);
      }
      //2018                                                                                                                                                                                                                                                                   
      if(event.run >= runnr_2018_Ab && event.run <= runnr_2018_Ae){
	jet_corrector_2018_A->process(event);
	jet_corrector_puppi_2018_A->process(event);
	jet_corrector_2018_A->correct_met(event);
      }
      else if(event.run >= runnr_2018_Bb && event.run <= runnr_2018_Be){
	jet_corrector_2018_B->process(event);
	jet_corrector_puppi_2018_B->process(event);
	jet_corrector_2018_B->correct_met(event);
      }
      else if(event.run >= runnr_2018_Cb && event.run <= runnr_2018_Ce){
	jet_corrector_2018_C->process(event);
	jet_corrector_puppi_2018_C->process(event);
	jet_corrector_2018_C->correct_met(event);
      }
      else if(event.run >= runnr_2018_Db && event.run <= runnr_2018_De){
        jet_corrector_2018_D->process(event);
        jet_corrector_puppi_2018_D->process(event);
	jet_corrector_2018_D->correct_met(event);
      }
    }//else on MC or data
    /* JEC are now applied */

    if(PRINT)     cout << " JEC/JER done" << endl;

    jetcleaner->process(event);
    massCalc->process(event);    

    if(PRINT)     cout << " cleaner & SD mass  done" << endl;

    bool muon_selection = muon_sel->passes(event);
    if(!muon_selection) return false;
    bool electron_selection = electron_sel->passes(event);
    if(!electron_selection) return false;
    if(PRINT)     cout << " leptons done" << endl;
    
    bool njet_selection = njet_sel->passes(event);
    if(njet_selection){    
        h_njet->fill(event);	
    }    
    if(!njet_selection) return false;
    if(PRINT)     cout << " njet sel done" << endl;

    bool dijet_loose_selection = dijet_sel_loose->passes(event);
    if(!dijet_loose_selection) return false;
    if(PRINT)     cout << " dijet sel loose done" << endl;
    
    //fill histograms for trigger turn on
    bool passedTriggers = false;
    for(unsigned int i=0; i<ref_triggers.size(); ++i){
      bool isfired = ref_triggers[i].passes(event);
      if(isfired){ passedTriggers = true; }
    }    
    
    if(passedTriggers) h_trig->fill(event,trigger_selection,trigNames);
    
    //set event variables/triggers/weights  
    event.set(b_isData, !isMC); 
    event.set(b_lumi, event.luminosityBlock); 
    event.set(b_run, event.run);  
    event.set(b_event, event.event);
    event.set(b_weightGen, genWeight);
    event.set(b_weightGen_LO, genWeight_LO);
    event.set(b_weightPU, isMC ? event.weight/event.genInfo->weights().at(0) : 1);
    event.set(b_weightTop, isTTbar ? event.get(top_pt_weight) : 1);
    event.set(b_weightBTag,1);
    event.set(b_xSec, isMC ? xSec_ : 1);
    event.set(b_pdf_x1, isMC ? event.genInfo->pdf_x1() : -9999);
    event.set(b_pdf_x2, isMC ? event.genInfo->pdf_x2() : -9999);
    event.set(b_pdf_scalePDF, isMC ? event.genInfo->pdf_scalePDF() : -9999);
    event.set(b_nTrueInt,isMC ? event.genInfo->pileup_TrueNumInteractions() : 1);
    event.set(b_rho,event.rho);
    event.set(b_nVert,event.pvs->size());
        
    passedTriggers=false;				     
    for(unsigned int i=0; i<trigger_selection.size(); ++i){
     bool isfired = trigger_selection[i].passes(event);
     if(isfired) passedTriggers=true;
     event.set(HLT_all[i], isfired);
    }
    event.set(HLT_JJ, passedTriggers);

    if(PRINT)     cout << " triggers done" << endl;

    bool dijet_selection = dijet_sel->passes(event);
    if(!dijet_selection) return false;
    
    h_dijet->fill(event);
    if(PRINT)     cout << " dijet sel done" << endl;

    vbfjetcleaner->process(event);
    Ak4OverlapCleaner->process(event);
    if(PRINT)     cout << " VBF jet cleaner and overlap done" << endl;
    //need to sort the jets  first  
    Jet jet1 = event.jets->at(0);
    Jet jet2 = event.jets->at(1);
    
    auto closest_puppijet1 = closestParticle(jet1, *(event.topjets));
    auto closest_puppijet2 = closestParticle(jet2, *(event.topjets)); 
    if( !closest_puppijet1 || !closest_puppijet2 ) return false;  
    if(PRINT)     cout << " closest puppi jets done" << endl;
    if(sorting == Sorting::SORTING_RANDOM){
     if( event.event%2 != 0 ){
      jet1 = event.jets->at(1);
      jet2 = event.jets->at(0);
     }
    } 
    else if(sorting == Sorting::SORTING_BYBTAG){
     double btag1 = closest_puppijet1->btag_MassDecorrelatedDeepBoosted_ZHbbvsQCD();
     double btag2 = closest_puppijet2->btag_MassDecorrelatedDeepBoosted_ZHbbvsQCD(); 
     if(btag2 > btag1){
      jet1 = event.jets->at(1);
      jet2 = event.jets->at(0);
     }     
    } 
    if(PRINT)     cout << " sorting done" << endl;
    closest_puppijet1 = closestParticle(jet1, *(event.topjets));
    closest_puppijet2 = closestParticle(jet2, *(event.topjets)); 
    auto closest_softdrop_genjet1 = isMC ? closestParticle(jet1, *(event.gentopjets)) : 0;
    auto closest_softdrop_genjet2 = isMC ? closestParticle(jet2, *(event.gentopjets)) : 0;
    auto closest_genjet1 = isMC ? closestParticle(jet1, *(event.genjets)) : 0;
    auto closest_genjet2 = isMC ? closestParticle(jet2, *(event.genjets)) : 0;
              
    //event variables		   
    event.set(m_o_njj,1);     
    bool vbf_selection = vbf_sel->passes(event);
    event.set(m_o_njj_vbf,vbf_selection);     
    if(PRINT)     cout << " vbf done" << endl;    
    //reco CHS jet variables	     
    event.set(m_o_mjj,inv_mass_safe(jet1.v4()+jet2.v4()));
    event.set(m_o_ptjj, (jet1.v4()+jet2.v4()).Pt());
    event.set(m_o_etajj, (jet1.v4()+jet2.v4()).Eta());
    event.set(m_o_phijj, (jet1.v4()+jet2.v4()).Phi());
    event.set(m_o_pt_jet1,jet1.pt());
    event.set(m_o_pt_jet2,jet2.pt());
    event.set(m_o_eta_jet1,jet1.eta());
    event.set(m_o_eta_jet2,jet2.eta());
    event.set(m_o_phi_jet1,jet1.phi());
    event.set(m_o_phi_jet2,jet2.phi());
    event.set(m_o_mass_jet1,jet1.v4().M());
    event.set(m_o_mass_jet2,jet2.v4().M());
    
    auto [ isVqq1, isZbb1 ] =  isMC ? genVqqEvent_sel->passes(event,jet1) : std::make_tuple(false,false);
    auto [ isVqq2, isZbb2 ] =  isMC ? genVqqEvent_sel->passes(event,jet2) : std::make_tuple(false,false);
    event.set(jj_mergedHTruth_jet1,isMC ? genHbbEvent_sel->passes(event,jet1) : 0);
    event.set(jj_mergedHTruth_jet2,isMC ? genHbbEvent_sel->passes(event,jet2) : 0);
    event.set(jj_mergedVTruth_jet1,isVqq1);
    event.set(jj_mergedVTruth_jet2,isVqq2);
    event.set(jj_mergedZbbTruth_jet1,isZbb1);
    event.set(jj_mergedZbbTruth_jet2,isZbb2);
    event.set(jj_mergedTopTruth_jet1,isMC ? genTopHadrEvent_sel->passes(event,jet1) : 0);
    event.set(jj_mergedTopTruth_jet2,isMC ? genTopHadrEvent_sel->passes(event,jet2) : 0);
            
    if(PRINT)     cout << " reco chs variable  done" << endl;

    //reco puppi softdrop variables	  
    event.set(m_o_pt_softdrop_jet1,closest_puppijet1->pt());
    event.set(m_o_pt_softdrop_jet2,closest_puppijet2->pt());
    event.set(m_o_eta_softdrop_jet1,closest_puppijet1->eta());
    event.set(m_o_eta_softdrop_jet2,closest_puppijet2->eta());
    event.set(m_o_phi_softdrop_jet1,closest_puppijet1->phi());
    event.set(m_o_phi_softdrop_jet2,closest_puppijet2->phi());
    event.set(m_o_mpuppisoftdrop_jet1,closest_puppijet1->softdropmass());
    event.set(m_o_mpuppisoftdrop_jet2,closest_puppijet2->softdropmass());
    int nsubjets1 = closest_puppijet1->subjets().size();
    int nsubjets2 = closest_puppijet2->subjets().size();    
    event.set(m_o_nSubJets_softdrop_jet1,nsubjets1);
    event.set(m_o_nSubJets_softdrop_jet2,nsubjets2);	   
    event.set(m_o_pt_softdrop_s1_jet1,nsubjets1 > 0 ? closest_puppijet1->subjets()[0].pt() : -9999);
    event.set(m_o_eta_softdrop_s1_jet1,nsubjets1 > 0 ? closest_puppijet1->subjets()[0].eta() : -9999); 
    event.set(m_o_phi_softdrop_s1_jet1,nsubjets1 > 0 ? closest_puppijet1->subjets()[0].phi() : -9999); 
    event.set(m_o_mass_softdrop_s1_jet1,nsubjets1 > 0 ? closest_puppijet1->subjets()[0].v4().M() : -9999);
    event.set(m_o_pt_softdrop_s2_jet1,nsubjets1 > 1 ? closest_puppijet1->subjets()[1].pt() : -9999);
    event.set(m_o_eta_softdrop_s2_jet1,nsubjets1 > 1 ? closest_puppijet1->subjets()[1].eta() : -9999); 
    event.set(m_o_phi_softdrop_s2_jet1,nsubjets1 > 1 ? closest_puppijet1->subjets()[1].phi() : -9999); 
    event.set(m_o_mass_softdrop_s2_jet1,nsubjets1 > 1 ? closest_puppijet1->subjets()[1].v4().M() : -9999);
    event.set(m_o_pt_softdrop_s1_jet2,nsubjets2 > 0 ? closest_puppijet2->subjets()[0].pt() : -9999);
    event.set(m_o_eta_softdrop_s1_jet2,nsubjets2 > 0 ? closest_puppijet2->subjets()[0].eta() : -9999); 
    event.set(m_o_phi_softdrop_s1_jet2,nsubjets2 > 0 ? closest_puppijet2->subjets()[0].phi() : -9999); 
    event.set(m_o_mass_softdrop_s1_jet2,nsubjets2 > 0 ? closest_puppijet2->subjets()[0].v4().M() : -9999);
    event.set(m_o_pt_softdrop_s2_jet2,nsubjets2 > 1 ? closest_puppijet2->subjets()[1].pt() : -9999);
    event.set(m_o_eta_softdrop_s2_jet2,nsubjets2 > 1 ? closest_puppijet2->subjets()[1].eta() : -9999); 
    event.set(m_o_phi_softdrop_s2_jet2,nsubjets2 > 1 ? closest_puppijet2->subjets()[1].phi() : -9999); 
    event.set(m_o_mass_softdrop_s2_jet2,nsubjets2 > 1 ? closest_puppijet2->subjets()[1].v4().M() : -9999);	 
    event.set(m_o_tau1_jet1,closest_puppijet1->tau1()); 		    
    event.set(m_o_tau2_jet1,closest_puppijet1->tau2()); 		    
    event.set(m_o_tau3_jet1,closest_puppijet1->tau3()); 		    
    event.set(m_o_tau4_jet1,closest_puppijet1->tau4()); 		    
    event.set(m_o_tau1_jet2,closest_puppijet2->tau1()); 		    
    event.set(m_o_tau2_jet2,closest_puppijet2->tau2()); 		    
    event.set(m_o_tau3_jet2,closest_puppijet2->tau3()); 		    
    event.set(m_o_tau4_jet2,closest_puppijet2->tau4()); 
    event.set(m_o_ecfN2_beta1_jet1,closest_puppijet1->ecfN2_beta1()); 
    event.set(m_o_ecfN2_beta2_jet1,closest_puppijet1->ecfN2_beta2()); 
    event.set(m_o_ecfN2_beta1_jet2,closest_puppijet2->ecfN2_beta1()); 
    event.set(m_o_ecfN2_beta2_jet2,closest_puppijet2->ecfN2_beta2()); 
    event.set(m_o_ecfN3_beta1_jet1,closest_puppijet1->ecfN3_beta1()); 
    event.set(m_o_ecfN3_beta2_jet1,closest_puppijet1->ecfN3_beta2()); 
    event.set(m_o_ecfN3_beta1_jet2,closest_puppijet2->ecfN3_beta1()); 
    event.set(m_o_ecfN3_beta2_jet2,closest_puppijet2->ecfN3_beta2()); 
    //event.set(m_o_MassDecorrelatedDeepBoosted_bbvsLight_jet1,closest_puppijet1->btag_MassDecorrelatedDeepBoosted_bbvsLight());
    //event.set(m_o_MassDecorrelatedDeepBoosted_bbvsLight_jet2,closest_puppijet2->btag_MassDecorrelatedDeepBoosted_bbvsLight());
    event.set(m_o_MassDecorrelatedDeepBoosted_ZHbbvsQCD_jet1,closest_puppijet1->btag_MassDecorrelatedDeepBoosted_ZHbbvsQCD()); 
    event.set(m_o_MassDecorrelatedDeepBoosted_ZHbbvsQCD_jet2,closest_puppijet2->btag_MassDecorrelatedDeepBoosted_ZHbbvsQCD()); 
    event.set(m_o_MassDecorrelatedDeepBoosted_HbbvsQCD_jet1,closest_puppijet1->btag_MassDecorrelatedDeepBoosted_HbbvsQCD());  
    event.set(m_o_MassDecorrelatedDeepBoosted_HbbvsQCD_jet2,closest_puppijet2->btag_MassDecorrelatedDeepBoosted_HbbvsQCD()); 
    //event.set(m_o_MassDecorrelatedDeepBoosted_probHbb_jet1,closest_puppijet1->btag_MassDecorrelatedDeepBoosted_probHbb()); 
    //event.set(m_o_MassDecorrelatedDeepBoosted_probHbb_jet2,closest_puppijet2->btag_MassDecorrelatedDeepBoosted_probHbb()); 
    //event.set(m_o_MassDecorrelatedDeepBoosted_probQCDbb_jet1,closest_puppijet1->btag_MassDecorrelatedDeepBoosted_probQCDbb()); 
    //event.set(m_o_MassDecorrelatedDeepBoosted_probQCDbb_jet2,closest_puppijet2->btag_MassDecorrelatedDeepBoosted_probQCDbb()); 
    event.set(m_o_MassDecorrelatedDeepBoosted_WvsQCD_jet1,closest_puppijet1->btag_MassDecorrelatedDeepBoosted_WvsQCD());  
    event.set(m_o_MassDecorrelatedDeepBoosted_WvsQCD_jet2,closest_puppijet2->btag_MassDecorrelatedDeepBoosted_WvsQCD()); 
    //event.set(m_o_MassDecorrelatedDeepBoosted_probWqq_jet1,closest_puppijet1->btag_MassDecorrelatedDeepBoosted_probWqq()); 
    //event.set(m_o_MassDecorrelatedDeepBoosted_probWqq_jet2,closest_puppijet2->btag_MassDecorrelatedDeepBoosted_probWqq());     
    //event.set(m_o_MassDecorrelatedDeepBoosted_proWcq_jet1,closest_puppijet1->btag_MassDecorrelatedDeepBoosted_proWcq()); 
    //event.set(m_o_MassDecorrelatedDeepBoosted_proWcq_jet2,closest_puppijet2->btag_MassDecorrelatedDeepBoosted_proWcq());	     
    event.set(m_o_MassIndependentDeepDoubleBvLJet_probHbb_jet1,closest_puppijet1->btag_MassIndependentDeepDoubleBvLJet_probHbb()); 
    event.set(m_o_MassIndependentDeepDoubleBvLJet_probHbb_jet2,closest_puppijet2->btag_MassIndependentDeepDoubleBvLJet_probHbb()); 
    //event.set(m_o_MassIndependentDeepDoubleBvLJet_probQCD_jet1,closest_puppijet1->btag_MassIndependentDeepDoubleBvLJet_probQCD()); 
    //event.set(m_o_MassIndependentDeepDoubleBvLJet_probQCD_jet2,closest_puppijet2->btag_MassIndependentDeepDoubleBvLJet_probQCD()); 
    event.set(m_o_DeepBoosted_WvsQCD_jet1,closest_puppijet1->btag_DeepBoosted_WvsQCD());  
    event.set(m_o_DeepBoosted_WvsQCD_jet2,closest_puppijet2->btag_DeepBoosted_WvsQCD());    
    //event.set(m_o_DeepBoosted_probWqq_jet1,closest_puppijet1->btag_DeepBoosted_probWqq());  
    //event.set(m_o_DeepBoosted_probWqq_jet2,closest_puppijet2->btag_DeepBoosted_probWqq());  
    //event.set(m_o_DeepBoosted_proWcq_jet1,closest_puppijet1->btag_DeepBoosted_probWcq());  
    //event.set(m_o_DeepBoosted_proWcq_jet2,closest_puppijet2->btag_DeepBoosted_probWcq());
    //event.set(m_o_DeepBoosted_ZbbvsQCD_jet1,closest_puppijet1->btag_DeepBoosted_ZbbvsQCD()); 
    //event.set(m_o_DeepBoosted_ZbbvsQCD_jet2,closest_puppijet2->btag_DeepBoosted_ZbbvsQCD());	 
    //event.set(m_o_DeepBoosted_HbbvsQCD_jet1,closest_puppijet1->btag_DeepBoosted_HbbvsQCD()); 
    //event.set(m_o_DeepBoosted_HbbvsQCD_jet2,closest_puppijet2->btag_DeepBoosted_HbbvsQCD());   
    event.set(m_o_DeepBoosted_ZHbbvsQCD_jet1,closest_puppijet1->btag_DeepBoosted_ZHbbvsQCD()); 
    event.set(m_o_DeepBoosted_ZHbbvsQCD_jet2,closest_puppijet2->btag_DeepBoosted_ZHbbvsQCD());  
    //event.set(m_o_DeepBoosted_probHbb_jet1,closest_puppijet1->btag_DeepBoosted_probHbb()); 
    //event.set(m_o_DeepBoosted_probHbb_jet2,closest_puppijet2->btag_DeepBoosted_probHbb()); 
    //event.set(m_o_DeepBoosted_probQCDbb_jet1,closest_puppijet1->btag_DeepBoosted_probQCDbb());  
    //event.set(m_o_DeepBoosted_probQCDbb_jet2,closest_puppijet2->btag_DeepBoosted_probQCDbb());  
    event.set(m_o_DeepDoubleBvLJet_probHbb_jet1,closest_puppijet1->btag_DeepDoubleBvLJet_probHbb());  
    event.set(m_o_DeepDoubleBvLJet_probHbb_jet2,closest_puppijet2->btag_DeepDoubleBvLJet_probHbb()); 
    //event.set(m_o_DeepDoubleBvLJet_probQCD_jet1,closest_puppijet1->btag_DeepDoubleBvLJet_probQCD()); 
    //event.set(m_o_DeepDoubleBvLJet_probQCD_jet2,closest_puppijet2->btag_DeepDoubleBvLJet_probQCD());  
    event.set(m_o_btag_DeepCSV_jet1,closest_puppijet1->btag_DeepCSV());
    event.set(m_o_btag_DeepCSV_jet2,closest_puppijet2->btag_DeepCSV());
    event.set(m_o_btag_DeepCSV_softdrop_s1_jet1,nsubjets1 > 0 ? closest_puppijet1->subjets()[0].btag_DeepCSV() : -9999);
    event.set(m_o_btag_DeepCSV_softdrop_s2_jet1,nsubjets1 > 1 ? closest_puppijet1->subjets()[1].btag_DeepCSV() : -9999);
    event.set(m_o_btag_DeepCSV_softdrop_s1_jet2,nsubjets2 > 0 ? closest_puppijet2->subjets()[0].btag_DeepCSV() : -9999);
    event.set(m_o_btag_DeepCSV_softdrop_s2_jet2,nsubjets2 > 1 ? closest_puppijet2->subjets()[1].btag_DeepCSV() : -9999);
    
    int nsubjetsBTaggedLoose = 0;
    int nsubjetsBTaggedMedium = 0;
    int nsubjetsBTaggedTight = 0;
    for(int s=0; s<nsubjets1; ++s){
     if(closest_puppijet1->subjets()[s].btag_DeepCSV()>DEEPCSVLOOSE) nsubjetsBTaggedLoose++;
     if(closest_puppijet1->subjets()[s].btag_DeepCSV()>DEEPCSVMEDIUM) nsubjetsBTaggedMedium++;
     if(closest_puppijet1->subjets()[s].btag_DeepCSV()>DEEPCSVTIGHT) nsubjetsBTaggedTight++;
    }
    event.set(m_o_nSubJets_softdrop_btag_DeepCSVL_jet1,nsubjetsBTaggedLoose);
    event.set(m_o_nSubJets_softdrop_btag_DeepCSVM_jet1,nsubjetsBTaggedMedium);
    event.set(m_o_nSubJets_softdrop_btag_DeepCSVT_jet1,nsubjetsBTaggedTight);
    
    nsubjetsBTaggedLoose = 0;
    nsubjetsBTaggedMedium = 0;
    nsubjetsBTaggedTight = 0;
    for(int s=0; s<nsubjets2; ++s){
     if(closest_puppijet2->subjets()[s].btag_DeepCSV()>DEEPCSVLOOSE) nsubjetsBTaggedLoose++;
     if(closest_puppijet2->subjets()[s].btag_DeepCSV()>DEEPCSVMEDIUM) nsubjetsBTaggedMedium++;
     if(closest_puppijet2->subjets()[s].btag_DeepCSV()>DEEPCSVTIGHT) nsubjetsBTaggedTight++;
    }
    event.set(m_o_nSubJets_softdrop_btag_DeepCSVL_jet2,nsubjetsBTaggedLoose);
    event.set(m_o_nSubJets_softdrop_btag_DeepCSVM_jet2,nsubjetsBTaggedMedium);
    event.set(m_o_nSubJets_softdrop_btag_DeepCSVT_jet2,nsubjetsBTaggedTight);
        
    /*event.set(m_o_btag_DeepFlavour_bb_jet1,closest_puppijet1->btag_DeepFlavour_bb());
    event.set(m_o_btag_DeepFlavour_bb_jet2,closest_puppijet2->btag_DeepFlavour_bb());
    event.set(m_o_btag_DeepFlavour_bb_softdrop_s1_jet1,nsubjets1 > 0 ? closest_puppijet1->subjets()[0].btag_DeepFlavour_bb() : -9999);
    event.set(m_o_btag_DeepFlavour_bb_softdrop_s2_jet1,nsubjets1 > 1 ? closest_puppijet1->subjets()[1].btag_DeepFlavour_bb() : -9999);
    event.set(m_o_btag_DeepFlavour_bb_softdrop_s1_jet2,nsubjets2 > 0 ? closest_puppijet2->subjets()[0].btag_DeepFlavour_bb() : -9999);
    event.set(m_o_btag_DeepFlavour_bb_softdrop_s2_jet2,nsubjets2 > 1 ? closest_puppijet2->subjets()[1].btag_DeepFlavour_bb() : -9999);
    event.set(m_o_btag_DeepFlavour_b_jet1,closest_puppijet1->btag_DeepFlavour_b());
    event.set(m_o_btag_DeepFlavour_b_jet2,closest_puppijet2->btag_DeepFlavour_b());
    event.set(m_o_btag_DeepFlavour_b_softdrop_s1_jet1,nsubjets1 > 0 ? closest_puppijet1->subjets()[0].btag_DeepFlavour_b() : -9999);
    event.set(m_o_btag_DeepFlavour_b_softdrop_s2_jet1,nsubjets1 > 1 ? closest_puppijet1->subjets()[1].btag_DeepFlavour_b() : -9999);
    event.set(m_o_btag_DeepFlavour_b_softdrop_s1_jet2,nsubjets2 > 0 ? closest_puppijet2->subjets()[0].btag_DeepFlavour_b() : -9999);
    event.set(m_o_btag_DeepFlavour_b_softdrop_s2_jet2,nsubjets2 > 1 ? closest_puppijet2->subjets()[1].btag_DeepFlavour_b() : -9999);
    event.set(m_o_btag_DeepJet_jet1,closest_puppijet1->btag_DeepJet());
    event.set(m_o_btag_DeepJet_jet2,closest_puppijet2->btag_DeepJet());
    event.set(m_o_btag_DeepJet_softdrop_s1_jet1,nsubjets1 > 0 ? closest_puppijet1->subjets()[0].btag_DeepJet() : -9999);
    event.set(m_o_btag_DeepJet_softdrop_s2_jet1,nsubjets1 > 1 ? closest_puppijet1->subjets()[1].btag_DeepJet() : -9999);
    event.set(m_o_btag_DeepJet_softdrop_s1_jet2,nsubjets2 > 0 ? closest_puppijet2->subjets()[0].btag_DeepJet() : -9999);
    event.set(m_o_btag_DeepJet_softdrop_s2_jet2,nsubjets2 > 1 ? closest_puppijet2->subjets()[1].btag_DeepJet() : -9999);

    nsubjetsBTaggedLoose = 0;
    nsubjetsBTaggedMedium = 0;
    nsubjetsBTaggedTight = 0;
    for(int s=0; s<nsubjets1; ++s){
     if(closest_puppijet1->subjets()[s].btag_DeepJet()>DEEPJETLOOSE) nsubjetsBTaggedLoose++;
     if(closest_puppijet1->subjets()[s].btag_DeepJet()>DEEPJETMEDIUM) nsubjetsBTaggedMedium++;
     if(closest_puppijet1->subjets()[s].btag_DeepJet()>DEEPJETTIGHT) nsubjetsBTaggedTight++;
    }
    event.set(m_o_nSubJets_softdrop_btag_DeepJetL_jet1,nsubjetsBTaggedLoose);
    event.set(m_o_nSubJets_softdrop_btag_DeepJetM_jet1,nsubjetsBTaggedMedium);
    event.set(m_o_nSubJets_softdrop_btag_DeepJetT_jet1,nsubjetsBTaggedTight);
    
    nsubjetsBTaggedLoose = 0;
    nsubjetsBTaggedMedium = 0;
    nsubjetsBTaggedTight = 0;
    for(int s=0; s<nsubjets2; ++s){
     if(closest_puppijet2->subjets()[s].btag_DeepJet()>DEEPJETLOOSE) nsubjetsBTaggedLoose++;
     if(closest_puppijet2->subjets()[s].btag_DeepJet()>DEEPJETMEDIUM) nsubjetsBTaggedMedium++;
     if(closest_puppijet2->subjets()[s].btag_DeepJet()>DEEPJETTIGHT) nsubjetsBTaggedTight++;
    }
    event.set(m_o_nSubJets_softdrop_btag_DeepJetL_jet2,nsubjetsBTaggedLoose);
    event.set(m_o_nSubJets_softdrop_btag_DeepJetM_jet2,nsubjetsBTaggedMedium);
    event.set(m_o_nSubJets_softdrop_btag_DeepJetT_jet2,nsubjetsBTaggedTight);*/
                    
    if(PRINT)     cout << " reco puppi SD variable  done" << endl;
    //reco puppi VBF jet variables 
    VBFvariables->process(event);
    if(PRINT)     cout << " reco vbf variable  done" << endl;
    //gen CHS variable  			  
    event.set(m_o_genmjj,(closest_genjet1 && closest_genjet2) ? inv_mass_safe(closest_genjet1->v4()+closest_genjet2->v4()) : -9999);		  
    event.set(m_o_genptjj,(closest_genjet1 && closest_genjet2) ? (closest_genjet1->v4()+closest_genjet2->v4()).Pt() : -9999);  	    
    event.set(m_o_genetajj,(closest_genjet1 && closest_genjet2) ? (closest_genjet1->v4()+closest_genjet2->v4()).Eta() : -9999);	      
    event.set(m_o_genphijj,(closest_genjet1 && closest_genjet2) ? (closest_genjet1->v4()+closest_genjet2->v4()).Phi() : -9999);	      
    event.set(m_o_genpt_jet1,closest_genjet1 ? closest_genjet1->pt() : -9999);
    event.set(m_o_genpt_jet2,closest_genjet2 ? closest_genjet2->pt() : -9999);
    event.set(m_o_geneta_jet1,closest_genjet1 ? closest_genjet1->eta() : -9999);
    event.set(m_o_geneta_jet2,closest_genjet2 ? closest_genjet2->eta() : -9999);
    event.set(m_o_genphi_jet1,closest_genjet1 ? closest_genjet1->phi() : -9999);
    event.set(m_o_genphi_jet2,closest_genjet2 ? closest_genjet2->phi() : -9999);
    event.set(m_o_genmass_jet1,closest_genjet1 ? closest_genjet1->v4().M() : -9999);
    event.set(m_o_genmass_jet2,closest_genjet2 ? closest_genjet2->v4().M() : -9999);
    if(PRINT)     cout << " gen chs variable  done" << endl;
    //gen softdrop jets variable	    
    event.set(m_o_pt_gen_softdrop_jet1,closest_softdrop_genjet1 ? closest_softdrop_genjet1->pt() : -9999);
    event.set(m_o_pt_gen_softdrop_jet2,closest_softdrop_genjet2 ? closest_softdrop_genjet2->pt() : -9999);
    event.set(m_o_eta_gen_softdrop_jet1,closest_softdrop_genjet1 ? closest_softdrop_genjet1->eta() : -9999);
    event.set(m_o_eta_gen_softdrop_jet2,closest_softdrop_genjet2 ? closest_softdrop_genjet2->eta() : -9999);
    event.set(m_o_phi_gen_softdrop_jet1,closest_softdrop_genjet1 ? closest_softdrop_genjet1->phi() : -9999);
    event.set(m_o_phi_gen_softdrop_jet2,closest_softdrop_genjet2 ? closest_softdrop_genjet2->phi() : -9999);
    event.set(m_o_mgensoftdrop_jet1,closest_softdrop_genjet1 ? closest_softdrop_genjet1->softdropmass() : -9999);//if there are no subjets sdmass should be 0 ?
    event.set(m_o_mgensoftdrop_jet2,closest_softdrop_genjet2 ? closest_softdrop_genjet2->softdropmass() : -9999);//if there are no subjets sdmass should be 0 ?	   
    nsubjets1 = closest_softdrop_genjet1 ? closest_softdrop_genjet1->subjets().size() : 0;
    nsubjets2 = closest_softdrop_genjet2 ? closest_softdrop_genjet2->subjets().size() : 0;
    event.set(m_o_nSubJets_gen_softdrop_jet1,nsubjets1);
    event.set(m_o_nSubJets_gen_softdrop_jet2,nsubjets2);      
    event.set(m_o_pt_gen_softdrop_s1_jet1,nsubjets1 > 0 ? closest_softdrop_genjet1->subjets()[0].pt() : -9999);
    event.set(m_o_eta_gen_softdrop_s1_jet1,nsubjets1 > 0 ? closest_softdrop_genjet1->subjets()[0].eta() : -9999); 
    event.set(m_o_phi_gen_softdrop_s1_jet1,nsubjets1 > 0 ? closest_softdrop_genjet1->subjets()[0].phi() : -9999); 
    event.set(m_o_mass_gen_softdrop_s1_jet1,nsubjets1 > 0 ? closest_softdrop_genjet1->subjets()[0].v4().M() : -9999);
    event.set(m_o_pt_gen_softdrop_s2_jet1,nsubjets1 > 1 ? closest_softdrop_genjet1->subjets()[1].pt() : -9999);
    event.set(m_o_eta_gen_softdrop_s2_jet1,nsubjets1 > 1 ? closest_softdrop_genjet1->subjets()[1].eta() : -9999); 
    event.set(m_o_phi_gen_softdrop_s2_jet1,nsubjets1 > 1 ? closest_softdrop_genjet1->subjets()[1].phi() : -9999); 
    event.set(m_o_mass_gen_softdrop_s2_jet1,nsubjets1 > 1 ? closest_softdrop_genjet1->subjets()[1].v4().M() : -9999);
    event.set(m_o_pt_gen_softdrop_s1_jet2,nsubjets2 > 0 ? closest_softdrop_genjet2->subjets()[0].pt() : -9999);
    event.set(m_o_eta_gen_softdrop_s1_jet2,nsubjets2 > 0 ? closest_softdrop_genjet2->subjets()[0].eta() : -9999); 
    event.set(m_o_phi_gen_softdrop_s1_jet2,nsubjets2 > 0 ? closest_softdrop_genjet2->subjets()[0].phi() : -9999); 
    event.set(m_o_mass_gen_softdrop_s1_jet2,nsubjets2 > 0 ? closest_softdrop_genjet2->subjets()[0].v4().M() : -9999);
    event.set(m_o_pt_gen_softdrop_s2_jet2,nsubjets2 > 1 ? closest_softdrop_genjet2->subjets()[1].pt() : -9999);
    event.set(m_o_eta_gen_softdrop_s2_jet2,nsubjets2 > 1 ? closest_softdrop_genjet2->subjets()[1].eta() : -9999); 
    event.set(m_o_phi_gen_softdrop_s2_jet2,nsubjets2 > 1 ? closest_softdrop_genjet2->subjets()[1].phi() : -9999); 
    event.set(m_o_mass_gen_softdrop_s2_jet2,nsubjets2 > 1 ? closest_softdrop_genjet2->subjets()[1].v4().M() : -9999);	     
    event.set(m_o_tau1_gen_jet1,closest_softdrop_genjet1 ? closest_softdrop_genjet1->tau1() : -9999);		       
    event.set(m_o_tau2_gen_jet1,closest_softdrop_genjet1 ? closest_softdrop_genjet1->tau2() : -9999);		       
    event.set(m_o_tau3_gen_jet1,closest_softdrop_genjet1 ? closest_softdrop_genjet1->tau3() : -9999);		       
    event.set(m_o_tau4_gen_jet1,closest_softdrop_genjet1 ? closest_softdrop_genjet1->tau4() : -9999);		       
    event.set(m_o_tau1_gen_jet2,closest_softdrop_genjet2 ? closest_softdrop_genjet2->tau1() : -9999);		       
    event.set(m_o_tau2_gen_jet2,closest_softdrop_genjet2 ? closest_softdrop_genjet2->tau2() : -9999);		       
    event.set(m_o_tau3_gen_jet2,closest_softdrop_genjet2 ? closest_softdrop_genjet2->tau3() : -9999);		       
    event.set(m_o_tau4_gen_jet2,closest_softdrop_genjet2 ? closest_softdrop_genjet2->tau4() : -9999); 
    event.set(m_o_ecfN2_beta1_gen_jet1,closest_softdrop_genjet1 ? closest_softdrop_genjet1->ecfN2_beta1() : -9999); 
    event.set(m_o_ecfN2_beta2_gen_jet1,closest_softdrop_genjet1 ? closest_softdrop_genjet1->ecfN2_beta2() : -9999); 
    event.set(m_o_ecfN2_beta1_gen_jet2,closest_softdrop_genjet2 ? closest_softdrop_genjet2->ecfN2_beta1() : -9999); 
    event.set(m_o_ecfN2_beta2_gen_jet2,closest_softdrop_genjet2 ? closest_softdrop_genjet2->ecfN2_beta2() : -9999); 
    event.set(m_o_ecfN3_beta1_gen_jet1,closest_softdrop_genjet1 ? closest_softdrop_genjet1->ecfN3_beta1() : -9999); 
    event.set(m_o_ecfN3_beta2_gen_jet1,closest_softdrop_genjet1 ? closest_softdrop_genjet1->ecfN3_beta2() : -9999); 
    event.set(m_o_ecfN3_beta1_gen_jet2,closest_softdrop_genjet2 ? closest_softdrop_genjet2->ecfN3_beta1() : -9999); 
    event.set(m_o_ecfN3_beta2_gen_jet2,closest_softdrop_genjet2 ? closest_softdrop_genjet2->ecfN3_beta2() : -9999); 
   if(PRINT)     cout << " gen puppi variable  done" << endl;
    //puppi met variables	
    event.set(m_o_met_pt,event.met->pt());
    event.set(m_o_met_eta,event.met->v4().Eta());
    event.set(m_o_met_phi,event.met->phi());
    event.set(m_o_met_mass,event.met->v4().M());
    event.set(m_o_met_sumEt,event.met->sumEt());

    //variables for spike killer
    float genHT = 0.;

    if (!event.isRealData) {
      for (const auto & itr: *event.genparticles) {
	if (abs(itr.status()) != 23) continue;
	uint pdg = abs(itr.pdgId());
	if (( pdg <= 6 && pdg >= 2) || pdg == 21) {
	  genHT += itr.pt();
	}
      }
    }

    event.set(m_o_genHT,!event.isRealData ? genHT : -9999 );
    event.set(m_o_qscale, !event.isRealData ? event.genInfo->qScale() : -9999);
    event.set(m_o_PU_pThat, !event.isRealData ? event.genInfo->PU_pT_hat_max() : -9999);
    event.set(b_pt_over_genHT, !event.isRealData && genHT > 0 && (event.jets->size() && ((event.jets->at(0).pt() / genHT) > 2)) ? false : true); 
    event.set(b_pt_over_qscale, !event.isRealData && event.jets->size() && ((event.jets->at(0).pt() / event.genInfo->qScale()) > 2) ? false: true);
    event.set(b_PUpthat_over_genHT, !event.isRealData && genHT > 0 && event.jets->size() && ((event.genInfo->PU_pT_hat_max() / genHT) > 1) ? false: true);
    event.set(b_spikekiller, (!event.isRealData && ((genHT > 0 && (event.jets->size() && ((event.jets->at(0).pt() / genHT) > 2)) ) || ((event.jets->at(0).pt() / event.genInfo->qScale()) > 2) || ((event.genInfo->PU_pT_hat_max() / genHT) > 1) ) )? false : true);
    
    for(unsigned int i=0; i<bruteForce_Decorrelation_default_16.size(); i++){
     bruteForce_Decorrelation_default_16[i]->process(event,closest_puppijet1,closest_puppijet2);
     bruteForce_Decorrelation_default_17[i]->process(event,closest_puppijet1,closest_puppijet2);
     bruteForce_Decorrelation_default_18[i]->process(event,closest_puppijet1,closest_puppijet2);
     bruteForce_Decorrelation_default_1617[i]->process(event,closest_puppijet1,closest_puppijet2);
     bruteForce_Decorrelation_default_161718[i]->process(event,closest_puppijet1,closest_puppijet2);
     //bruteForce_Decorrelation_smooth_16[i]->process(event,closest_puppijet1,closest_puppijet2);
     //bruteForce_Decorrelation_smooth_17[i]->process(event,closest_puppijet1,closest_puppijet2);
     //bruteForce_Decorrelation_smooth_1617[i]->process(event,closest_puppijet1,closest_puppijet2);
    }

    //NLO weights                                                                                                                                                                                          
    if(isVjet)      NLOweights->process(event);
   

  
    if(PRINT)     cout << " done!!! " << endl;
     
    // 3. decide whether or not to keep the current event in the output:
    return njet_selection && dijet_selection;
}

UHHNtupleConverterModule::~UHHNtupleConverterModule(){

    std::cout.precision(10); 
    std::cout << "Total processed events = " << totalEvents << std::endl;
    std::cout << "Total generated events = " << totalGenEvents << std::endl;
    std::cout << "Total generated events (LO PDF reweighted) = " << totalGenEvents_LO << std::endl;

}

// as we want to run the UHHNtupleConverterCycleNew directly with AnalysisModuleRunner,
// make sure the UHHNtupleConverterModule is found by class name. This is ensured by this macro:
UHH2_REGISTER_ANALYSIS_MODULE(UHHNtupleConverterModule)

}
