#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/CommonModules.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/ElectronHists.h"
#include "UHH2/common/include/NSelections.h"
#include "UHH2/common/include/TriggerSelection.h"
#include "UHH2/UHHNtupleConverter/include/UHHNtupleConverterSelections.h"
#include "UHH2/UHHNtupleConverter/include/UHHNtupleConverterHists.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/common/include/ObjectIdUtils.h"

#include "UHH2/UHHNtupleConverter/include/LumiWeight.h"

using namespace std;
using namespace uhh2;

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

private:
    
    std::unique_ptr<CommonModules> common;
    std::unique_ptr<JetCleaner> jetcleaner;
    std::unique_ptr<AnalysisModule> massCalc;
   
    // declare the Selections to use. Use unique_ptr to ensure automatic call of delete in the destructor,
    // to avoid memory leaks.
    std::unique_ptr<Selection> njet_sel, dijet_sel;
    std::unique_ptr<GenHbbEventSelection> genHbbEvent_sel;
    std::unique_ptr<GenVqqEventSelection> genVqqEvent_sel;
    std::vector<TriggerSelection> trigger_selection; 
    std::vector<TriggerSelection> metfilters;
    std::unique_ptr<NPVSelection> pvfilter;
    
    
    // store the Hists collection as member variables. Again, use unique_ptr to avoid memory leaks.
    std::unique_ptr<Hists> h_nocuts, h_njet, h_dijet, h_ele;
    
    //some other useful objects
    Year year;
    LumiWeight m_xSec;
    double xSec_;
    bool isMC;
    
    //output tree variables
    
    //event variables
    uhh2::Event::Handle<bool>   b_isData;
    uhh2::Event::Handle<int>    b_lumi;
    uhh2::Event::Handle<int>    b_run;
    uhh2::Event::Handle<int>    b_event;
    uhh2::Event::Handle<double> b_xSec;
    uhh2::Event::Handle<float>  b_weightGen;
    uhh2::Event::Handle<float>  b_weightPU;
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
    uhh2::Event::Handle<bool>    jj_mergedVTruth_jet1;
    uhh2::Event::Handle<bool>    jj_mergedVTruth_jet2;
    uhh2::Event::Handle<bool>    jj_mergedHTruth_jet1;
    uhh2::Event::Handle<bool>    jj_mergedHTruth_jet2;
            
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
    uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_bbvsLight_jet1; 
    uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_bbvsLight_jet2; 
    uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_ZHbbvsQCD_jet1; 
    uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_ZHbbvsQCD_jet2; 
    uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_HbbvsQCD_jet1; 
    uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_HbbvsQCD_jet2; 
    uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_probHbb_jet1; 
    uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_probHbb_jet2; 
    uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_probQCDbb_jet1; 
    uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_probQCDbb_jet2; 
    uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_WvsQCD_jet1; 
    uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_WvsQCD_jet2; 
    uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_probWqq_jet1; 
    uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_probWqq_jet2;	  
    uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_proWcq_jet1; 
    uhh2::Event::Handle<float>  m_o_MassDecorrelatedDeepBoosted_proWcq_jet2;		 
    uhh2::Event::Handle<float>  m_o_MassIndependentDeepDoubleBvLJet_probHbb_jet1; 
    uhh2::Event::Handle<float>  m_o_MassIndependentDeepDoubleBvLJet_probHbb_jet2; 
    uhh2::Event::Handle<float>  m_o_MassIndependentDeepDoubleBvLJet_probQCD_jet1; 
    uhh2::Event::Handle<float>  m_o_MassIndependentDeepDoubleBvLJet_probQCD_jet2;     
    uhh2::Event::Handle<float>  m_o_DeepBoosted_WvsQCD_jet1; 
    uhh2::Event::Handle<float>  m_o_DeepBoosted_WvsQCD_jet2;  
    uhh2::Event::Handle<float>  m_o_DeepBoosted_probWqq_jet1; 
    uhh2::Event::Handle<float>  m_o_DeepBoosted_probWqq_jet2;  
    uhh2::Event::Handle<float>  m_o_DeepBoosted_proWcq_jet1; 
    uhh2::Event::Handle<float>  m_o_DeepBoosted_proWcq_jet2;  
    uhh2::Event::Handle<float>  m_o_DeepBoosted_ZbbvsQCD_jet1;
    uhh2::Event::Handle<float>  m_o_DeepBoosted_ZbbvsQCD_jet2;
    uhh2::Event::Handle<float>  m_o_DeepBoosted_HbbvsQCD_jet1; 
    uhh2::Event::Handle<float>  m_o_DeepBoosted_HbbvsQCD_jet2;  
    uhh2::Event::Handle<float>  m_o_DeepBoosted_probHbb_jet1; 
    uhh2::Event::Handle<float>  m_o_DeepBoosted_probHbb_jet2;  
    uhh2::Event::Handle<float>  m_o_DeepBoosted_probQCDbb_jet1; 
    uhh2::Event::Handle<float>  m_o_DeepBoosted_probQCDbb_jet2;  
    uhh2::Event::Handle<float>  m_o_DeepDoubleBvLJet_probHbb_jet1; 
    uhh2::Event::Handle<float>  m_o_DeepDoubleBvLJet_probHbb_jet2;  
    uhh2::Event::Handle<float>  m_o_DeepDoubleBvLJet_probQCD_jet1; 
    uhh2::Event::Handle<float>  m_o_DeepDoubleBvLJet_probQCD_jet2;  
                                            
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
    
    year = extract_year(ctx);
    isMC = ctx.get("dataset_type") == "MC";
    
    if (isMC) xSec_ = m_xSec.getLumiWeight( ctx.get("sample_name") );//to be checked
    cout << "Cross section set to " << xSec_ << " for sample " << ctx.get("sample_name") << endl;
    
    // 1. setup other modules. CommonModules and the JetCleaner:
    common.reset(new CommonModules());
    // TODO: configure common here, e.g. by 
    // calling common->set_*_id or common->disable_*
    // note that the JetCleaner is only kept for the sake of example;
    // instead of constructing a jetcleaner explicitly,
    // the cleaning can also be achieved with less code via CommonModules with:
    // common->set_jet_id(PtEtaCut(30.0, 2.4));
    // before the 'common->init(ctx)' line.
    common->switch_jetPtSorter(true);
    common->disable_metfilters();
    common->disable_pvfilter();
    common->init(ctx);
    jetcleaner.reset(new JetCleaner(ctx, 200.0, 2.4)); //automatically run PFJetID Tight for CHS in the Common modules unless disable_jetpfidfilter() is run
    massCalc.reset(new SoftDropMassCalculator(ctx, true, "common/data/2018/puppiCorr.root"));

    //branches for output tree
    
    //event variables
    ctx.undeclare_all_event_output();

    b_isData = ctx.declare_event_output<bool>("isData");
    b_lumi = ctx.declare_event_output<int>("lumi");
    b_run = ctx.declare_event_output<int>("run");
    b_event = ctx.declare_event_output<int>("evt");
    b_xSec = ctx.declare_event_output<double>("xsec");
    b_weightGen = ctx.declare_event_output<float>("genWeight");
    b_weightPU = ctx.declare_event_output<float>("puWeight");
    b_weightBTag = ctx.declare_event_output<float>("btagWeight");
    b_nTrueInt = ctx.declare_event_output<float>("nTrueInt");
    b_rho = ctx.declare_event_output<float>("rho");
    b_nVert = ctx.declare_event_output<int>("nVert");
    HLT_JJ = ctx.declare_event_output<bool>("HLT_JJ");
    b_passed_MET_filters = ctx.declare_event_output<bool>("passed_METfilters");
    b_passed_PV_filter = ctx.declare_event_output<bool>("passed_PVfilter");
    m_o_njj = ctx.declare_event_output<int>("njj");
        
    /* some filters and triggers*/	
    std::vector<std::string> trigNames;
    std::vector<std::string> metFilters;
    	
    //nb, not sure we need all the thresolds. Better to choose only one that was always unprescaled. To be checked.
    if( year == Year::is2018 || year == Year::is2017v2 || year == Year::is2017v1 ){//from b2g-18-002
     trigNames = {"HLT_PFHT1050_v*" ,"HLT_AK8PFJet500_v*",
                                 "HLT_AK8PFJet360_TrimMass30_v*","HLT_AK8PFJet380_TrimMass30_v*","HLT_AK8PFJet400_TrimMass30_v*","HLT_AK8PFJet420_TrimMass30_v*", //pt=400 first always unprescaled. To be checked.
				 "HLT_AK8PFHT750_TrimMass50_v*","HLT_AK8PFHT800_TrimMass50_v*","HLT_AK8PFHT850_TrimMass50_v*","HLT_AK8PFHT900_TrimMass50_v*"}; //ht=800 first always unprescaled. To be checked.
     if(isMC) metFilters = {"Flag_goodVertices","Flag_globalSuperTightHalo2016Filter","Flag_HBHENoiseFilter","Flag_HBHENoiseIsoFilter","Flag_EcalDeadCellTriggerPrimitiveFilter","Flag_BadPFMuonFilter"};     
     else metFilters = {"Flag_goodVertices","Flag_globalSuperTightHalo2016Filter","Flag_HBHENoiseFilter","Flag_HBHENoiseIsoFilter","Flag_EcalDeadCellTriggerPrimitiveFilter","Flag_BadPFMuonFilter","Flag_eeBadScFilter"};          
    }
    else if( year == Year::is2016v2 || year == Year::is2016v3 ){//from b2g-17-001     
     trigNames = {"HLT_PFHT800_v*","HLT_PFHT900_v*",
                                  "HLT_PFJet450_v*","HLT_PFJet500_v*","HLT_PFJet450_v*",
				  "HLT_AK8PFJet450_v*","HLT_AK8PFJet500_v*",
				  "HLT_PFHT650_WideJetMJJ900DEtaJJ1p5_v*","HLT_PFHT650_WideJetMJJ950DEtaJJ1p5_v*",
				  "HLT_AK8PFJet360_TrimMass30_v*","HLT_AK8PFHT700_TrimR0p1PT0p03Mass50_v*"}; 
     if(isMC) metFilters = {"Flag_goodVertices","Flag_globalSuperTightHalo2016Filter","Flag_HBHENoiseFilter","Flag_HBHENoiseIsoFilter","Flag_EcalDeadCellTriggerPrimitiveFilter"};
     else metFilters = {"Flag_goodVertices","Flag_globalSuperTightHalo2016Filter","Flag_HBHENoiseFilter","Flag_HBHENoiseIsoFilter","Flag_EcalDeadCellTriggerPrimitiveFilter","Flag_eeBadScFilter"};
    }

    std::cout << "USING " << trigNames.size() << " TRIGGER PATHS:" << std::endl;
    for(int i=0; i<trigNames.size(); ++i){
      std::cout << trigNames[i] << std::endl;
      trigger_selection.push_back(TriggerSelection(trigNames[i])); 
      HLT_all.push_back( ctx.declare_event_output<bool>(trigNames[i].replace(trigNames[i].end()-3,trigNames[i].end(),"")) );
    }    
    std::cout << "----------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "USING " << metFilters.size()+1 << " MET FILTERS:" << std::endl;                
    for(int i=0; i<metFilters.size(); ++i){ std::cout << metFilters[i] << std::endl; metfilters.push_back( TriggerSelection(metFilters[i]) ); b_MET_filters_all.push_back(ctx.declare_event_output<bool>(metFilters[i])); }
    std::cout << "Flag_EcalBadCalibSelection (for 2016 this is always = 1)" << std::endl;
    b_MET_filters_all.push_back( ctx.declare_event_output<bool>("Flag_EcalBadCalibSelection") );  
    /*done with triggers and filters*/        
	     
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
    jj_mergedHTruth_jet1 = ctx.declare_event_output<bool>("jj_l1_mergedHTruth");
    jj_mergedHTruth_jet2 = ctx.declare_event_output<bool>("jj_l2_mergedHTruth");
              
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
    m_o_ecfN3_beta1_jet1 = ctx.declare_event_output<float>("jj_l1_ecfN2_beta1");
    m_o_ecfN3_beta2_jet1 = ctx.declare_event_output<float>("jj_l1_ecfN2_beta2");
    m_o_ecfN3_beta1_jet2 = ctx.declare_event_output<float>("jj_l2_ecfN2_beta1");
    m_o_ecfN3_beta2_jet2 = ctx.declare_event_output<float>("jj_l2_ecfN2_beta2");    
    m_o_MassDecorrelatedDeepBoosted_bbvsLight_jet1 = ctx.declare_event_output<float>("jj_l1_MassDecorrelatedDeepBoosted_bbvsLight"); 
    m_o_MassDecorrelatedDeepBoosted_bbvsLight_jet2 = ctx.declare_event_output<float>("jj_l2_MassDecorrelatedDeepBoosted_bbvsLight"); 
    m_o_MassDecorrelatedDeepBoosted_ZHbbvsQCD_jet1 = ctx.declare_event_output<float>("jj_l1_MassDecorrelatedDeepBoosted_ZHbbvsQCD"); 
    m_o_MassDecorrelatedDeepBoosted_ZHbbvsQCD_jet2 = ctx.declare_event_output<float>("jj_l2_MassDecorrelatedDeepBoosted_ZHbbvsQCD"); 
    m_o_MassDecorrelatedDeepBoosted_HbbvsQCD_jet1 = ctx.declare_event_output<float>("jj_l1_MassDecorrelatedDeepBoosted_HbbvsQCD"); 
    m_o_MassDecorrelatedDeepBoosted_HbbvsQCD_jet2 = ctx.declare_event_output<float>("jj_l2_MassDecorrelatedDeepBoosted_HbbvsQCD"); 
    m_o_MassDecorrelatedDeepBoosted_probHbb_jet1 = ctx.declare_event_output<float>("jj_l1_MassDecorrelatedDeepBoosted_probHbb"); 
    m_o_MassDecorrelatedDeepBoosted_probHbb_jet2 = ctx.declare_event_output<float>("jj_l2_MassDecorrelatedDeepBoosted_probHbb"); 
    m_o_MassDecorrelatedDeepBoosted_probQCDbb_jet1 = ctx.declare_event_output<float>("jj_l1_MassDecorrelatedDeepBoosted_probQCDbb"); 
    m_o_MassDecorrelatedDeepBoosted_probQCDbb_jet2 = ctx.declare_event_output<float>("jj_l2_MassDecorrelatedDeepBoosted_probQCDbb"); 
    m_o_MassDecorrelatedDeepBoosted_WvsQCD_jet1 = ctx.declare_event_output<float>("jj_l1_MassDecorrelatedDeepBoosted_WvsQCD"); 
    m_o_MassDecorrelatedDeepBoosted_WvsQCD_jet2 = ctx.declare_event_output<float>("jj_l2_MassDecorrelatedDeepBoosted_WvsQCD"); 
    m_o_MassDecorrelatedDeepBoosted_probWqq_jet1 = ctx.declare_event_output<float>("jj_l1_MassDecorrelatedDeepBoosted_probWqq"); 
    m_o_MassDecorrelatedDeepBoosted_probWqq_jet2 = ctx.declare_event_output<float>("jj_l2_MassDecorrelatedDeepBoosted_probWqq");     
    m_o_MassDecorrelatedDeepBoosted_proWcq_jet1 = ctx.declare_event_output<float>("jj_l1_MassDecorrelatedDeepBoosted_proWcq"); 
    m_o_MassDecorrelatedDeepBoosted_proWcq_jet2 = ctx.declare_event_output<float>("jj_l2_MassDecorrelatedDeepBoosted_proWcq");             
    m_o_MassIndependentDeepDoubleBvLJet_probHbb_jet1 = ctx.declare_event_output<float>("jj_l1_MassIndependentDeepDoubleBvLJetprobHbb_"); 
    m_o_MassIndependentDeepDoubleBvLJet_probHbb_jet2 = ctx.declare_event_output<float>("jj_l2_MassIndependentDeepDoubleBvLJet_probHbb"); 
    m_o_MassIndependentDeepDoubleBvLJet_probQCD_jet1 = ctx.declare_event_output<float>("jj_l1_MassIndependentDeepDoubleBvLJet_probQCD"); 
    m_o_MassIndependentDeepDoubleBvLJet_probQCD_jet2 = ctx.declare_event_output<float>("jj_l2_MassIndependentDeepDoubleBvLJet_probQCD");     
    m_o_DeepBoosted_WvsQCD_jet1 = ctx.declare_event_output<float>("jj_l1_DeepBoosted_WvsQCD");  
    m_o_DeepBoosted_WvsQCD_jet2 = ctx.declare_event_output<float>("jj_l2_DeepBoosted_WvsQCD"); 
    m_o_DeepBoosted_probWqq_jet1 = ctx.declare_event_output<float>("jj_l1_DeepBoosted_probWqq");  
    m_o_DeepBoosted_probWqq_jet2 = ctx.declare_event_output<float>("jj_l2_DeepBoosted_probWqq"); 
    m_o_DeepBoosted_proWcq_jet1 = ctx.declare_event_output<float>("jj_l1_DeepBoosted_proWcq");  
    m_o_DeepBoosted_proWcq_jet2 = ctx.declare_event_output<float>("jj_l2_DeepBoosted_proWcq"); 
    m_o_DeepBoosted_ZbbvsQCD_jet1 = ctx.declare_event_output<float>("jj_l2_DeepBoosted_ZbbvsQCD"); 
    m_o_DeepBoosted_ZbbvsQCD_jet2 = ctx.declare_event_output<float>("jj_l2_DeepBoosted_ZbbvsQCD");     
    m_o_DeepBoosted_HbbvsQCD_jet1 = ctx.declare_event_output<float>("jj_l1_DeepBoosted_HbbvsQCD"); 
    m_o_DeepBoosted_HbbvsQCD_jet2 = ctx.declare_event_output<float>("jj_l2_DeepBoosted_HbbvsQCD");  
    m_o_DeepBoosted_probHbb_jet1 = ctx.declare_event_output<float>("jj_l1_DeepBoosted_probHbb"); 
    m_o_DeepBoosted_probHbb_jet2 = ctx.declare_event_output<float>("jj_l2_DeepBoosted_probHbb");  
    m_o_DeepBoosted_probQCDbb_jet1 = ctx.declare_event_output<float>("jj_l1_DeepBoosted_probQCDbb"); 
    m_o_DeepBoosted_probQCDbb_jet2 = ctx.declare_event_output<float>("jj_l2_DeepBoosted_probQCDbb");  
    m_o_DeepDoubleBvLJet_probHbb_jet1 = ctx.declare_event_output<float>("jj_l1_DeepDoubleBvLJet_probHbb"); 
    m_o_DeepDoubleBvLJet_probHbb_jet2 = ctx.declare_event_output<float>("jj_l2_DeepDoubleBvLJet_probHbb");  
    m_o_DeepDoubleBvLJet_probQCD_jet1 = ctx.declare_event_output<float>("jj_l1_DeepDoubleBvLJet_probQCD"); 
    m_o_DeepDoubleBvLJet_probQCD_jet2 = ctx.declare_event_output<float>("jj_l2_DeepDoubleBvLJet_probQCD");
    
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
                
    // 2. set up selections
    njet_sel.reset(new NJetSelection(2)); // see common/include/NSelections.h
    dijet_sel.reset(new DijetSelection(1.3,700)); // see UHHNtupleConverterSelections
    
    PrimaryVertexId pvid=StandardPrimaryVertexId();
    pvfilter.reset(new NPVSelection(1,-1,pvid) );

    // 3. Set up Hists classes:
    h_nocuts.reset(new UHHNtupleConverterHists(ctx, "NoCuts"));
    h_njet.reset(new UHHNtupleConverterHists(ctx, "Njet"));
    h_dijet.reset(new UHHNtupleConverterHists(ctx, "Dijet"));    
    genHbbEvent_sel.reset(new GenHbbEventSelection());
    genVqqEvent_sel.reset(new GenVqqEventSelection());
    

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
    
    //cout << "UHHNtupleConverterModule: Starting to process event (runid, eventid) = (" << event.run << ", " << event.event << "); weight = " << event.weight << endl;

    // 2. test selections and fill histograms
    h_nocuts->fill(event);
        
    // 1. run all modules other modules.
    common->process(event)   

    bool passedMETFilters = true;
    for(unsigned int i=0; i<metfilters.size(); ++i){
     if(!metfilters[i].passes(event)) passedMETFilters = false;
     event.set(b_MET_filters_all[i],metfilters[i].passes(event));
    }
    if( !event.passEcalBadCalib ) passedMETFilters = false;
    event.set(b_MET_filters_all[metfilters.size()],event.passEcalBadCalib);
    event.set(b_passed_MET_filters,passedMETFilters);
    event.set(b_passed_PV_filter,pvfilter->passes(event));
    	
    jetcleaner->process(event);
    massCalc->process(event);    
          
    //set event variables/triggers/weights  
    event.set(b_isData, !isMC); 
    event.set(b_lumi, event.luminosityBlock); 
    event.set(b_run, event.run);  
    event.set(b_event, event.event);
    event.set(b_weightGen,event.genInfo->weights().at(0));
    event.set(b_weightPU,event.weight/event.genInfo->weights().at(0));
    event.set(b_weightBTag,1);
    event.set(b_xSec, isMC ? xSec_ : 1);
    event.set(b_nTrueInt,event.genInfo->pileup_TrueNumInteractions());
    event.set(b_rho,event.rho);
    event.set(b_nVert,event.pvs->size());
        
    bool passedTriggers=false;				     
    for(unsigned int i=0; i<trigger_selection.size(); ++i){
     bool isfired = trigger_selection[i].passes(event);
     if(isfired) passedTriggers=true;
     event.set(HLT_all[i], isfired);
    }
    event.set(HLT_JJ, passedTriggers);
    
    bool njet_selection = njet_sel->passes(event);
    if(njet_selection){
        h_njet->fill(event);
    }    
    if(!njet_selection) return false;
    
    bool dijet_selection = dijet_sel->passes(event);
    if(!dijet_selection) return false;
    
    h_dijet->fill(event);

    //now random sorting    
    Jet jet1 = event.jets->at(0);
    Jet jet2 = event.jets->at(1);
    if( event.event%2 != 0 ){
     jet1 = event.jets->at(1);
     jet2 = event.jets->at(0);
    }
    
    auto closest_puppijet1 = closestParticle(jet1, *(event.topjets));
    auto closest_puppijet2 = closestParticle(jet2, *(event.topjets)); 
    auto closest_softdrop_genjet1 = closestParticle(jet1, *(event.gentopjets));
    auto closest_softdrop_genjet2 = closestParticle(jet2, *(event.gentopjets));
    auto closest_genjet1 = closestParticle(jet1, *(event.genjets));
    auto closest_genjet2 = closestParticle(jet2, *(event.genjets));

    if( !closest_puppijet1 || !closest_puppijet2 ) return false;  
          
    //event variables		   
    event.set(m_o_njj,1);     
        
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
    
    event.set(jj_mergedHTruth_jet1,genHbbEvent_sel->passes(event,jet1));
    event.set(jj_mergedHTruth_jet2,genHbbEvent_sel->passes(event,jet2));
    event.set(jj_mergedVTruth_jet1,genVqqEvent_sel->passes(event,jet1));
    event.set(jj_mergedVTruth_jet2,genVqqEvent_sel->passes(event,jet2));
                
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
    event.set(m_o_MassDecorrelatedDeepBoosted_bbvsLight_jet1,closest_puppijet1->btag_MassDecorrelatedDeepBoosted_bbvsLight());
    event.set(m_o_MassDecorrelatedDeepBoosted_bbvsLight_jet2,closest_puppijet2->btag_MassDecorrelatedDeepBoosted_bbvsLight());
    event.set(m_o_MassDecorrelatedDeepBoosted_ZHbbvsQCD_jet1,closest_puppijet1->btag_MassDecorrelatedDeepBoosted_ZHbbvsQCD()); 
    event.set(m_o_MassDecorrelatedDeepBoosted_ZHbbvsQCD_jet2,closest_puppijet2->btag_MassDecorrelatedDeepBoosted_ZHbbvsQCD()); 
    event.set(m_o_MassDecorrelatedDeepBoosted_HbbvsQCD_jet1,closest_puppijet1->btag_MassDecorrelatedDeepBoosted_HbbvsQCD());  
    event.set(m_o_MassDecorrelatedDeepBoosted_HbbvsQCD_jet2,closest_puppijet2->btag_MassDecorrelatedDeepBoosted_HbbvsQCD()); 
    event.set(m_o_MassDecorrelatedDeepBoosted_probHbb_jet1,closest_puppijet1->btag_MassDecorrelatedDeepBoosted_probHbb()); 
    event.set(m_o_MassDecorrelatedDeepBoosted_probHbb_jet2,closest_puppijet2->btag_MassDecorrelatedDeepBoosted_probHbb()); 
    event.set(m_o_MassDecorrelatedDeepBoosted_probQCDbb_jet1,closest_puppijet1->btag_MassDecorrelatedDeepBoosted_probQCDbb()); 
    event.set(m_o_MassDecorrelatedDeepBoosted_probQCDbb_jet2,closest_puppijet2->btag_MassDecorrelatedDeepBoosted_probQCDbb()); 
    event.set(m_o_MassDecorrelatedDeepBoosted_WvsQCD_jet1,closest_puppijet1->btag_MassDecorrelatedDeepBoosted_WvsQCD());  
    event.set(m_o_MassDecorrelatedDeepBoosted_WvsQCD_jet2,closest_puppijet2->btag_MassDecorrelatedDeepBoosted_WvsQCD()); 
    event.set(m_o_MassDecorrelatedDeepBoosted_probWqq_jet1,closest_puppijet1->btag_MassDecorrelatedDeepBoosted_probWqq()); 
    event.set(m_o_MassDecorrelatedDeepBoosted_probWqq_jet2,closest_puppijet2->btag_MassDecorrelatedDeepBoosted_probWqq());     
    event.set(m_o_MassDecorrelatedDeepBoosted_proWcq_jet1,closest_puppijet1->btag_MassDecorrelatedDeepBoosted_proWcq()); 
    event.set(m_o_MassDecorrelatedDeepBoosted_proWcq_jet2,closest_puppijet2->btag_MassDecorrelatedDeepBoosted_proWcq());	     
    event.set(m_o_MassIndependentDeepDoubleBvLJet_probHbb_jet1,closest_puppijet1->btag_MassIndependentDeepDoubleBvLJet_probHbb()); 
    event.set(m_o_MassIndependentDeepDoubleBvLJet_probHbb_jet2,closest_puppijet2->btag_MassIndependentDeepDoubleBvLJet_probHbb()); 
    event.set(m_o_MassIndependentDeepDoubleBvLJet_probQCD_jet1,closest_puppijet1->btag_MassIndependentDeepDoubleBvLJet_probQCD()); 
    event.set(m_o_MassIndependentDeepDoubleBvLJet_probQCD_jet2,closest_puppijet2->btag_MassIndependentDeepDoubleBvLJet_probQCD()); 
    event.set(m_o_DeepBoosted_WvsQCD_jet1,closest_puppijet1->btag_DeepBoosted_WvsQCD());  
    event.set(m_o_DeepBoosted_WvsQCD_jet2,closest_puppijet2->btag_DeepBoosted_WvsQCD());    
    event.set(m_o_DeepBoosted_probWqq_jet1,closest_puppijet1->btag_DeepBoosted_probWqq());  
    event.set(m_o_DeepBoosted_probWqq_jet2,closest_puppijet2->btag_DeepBoosted_probWqq());  
    event.set(m_o_DeepBoosted_proWcq_jet1,closest_puppijet1->btag_DeepBoosted_probWcq());  
    event.set(m_o_DeepBoosted_proWcq_jet2,closest_puppijet2->btag_DeepBoosted_probWcq());
    event.set(m_o_DeepBoosted_ZbbvsQCD_jet1,closest_puppijet1->btag_DeepBoosted_ZbbvsQCD()); 
    event.set(m_o_DeepBoosted_ZbbvsQCD_jet2,closest_puppijet2->btag_DeepBoosted_ZbbvsQCD());	 
    event.set(m_o_DeepBoosted_HbbvsQCD_jet1,closest_puppijet1->btag_DeepBoosted_HbbvsQCD()); 
    event.set(m_o_DeepBoosted_HbbvsQCD_jet2,closest_puppijet2->btag_DeepBoosted_HbbvsQCD());   
    event.set(m_o_DeepBoosted_probHbb_jet1,closest_puppijet1->btag_DeepBoosted_probHbb()); 
    event.set(m_o_DeepBoosted_probHbb_jet2,closest_puppijet2->btag_DeepBoosted_probHbb()); 
    event.set(m_o_DeepBoosted_probQCDbb_jet1,closest_puppijet1->btag_DeepBoosted_probQCDbb());  
    event.set(m_o_DeepBoosted_probQCDbb_jet2,closest_puppijet2->btag_DeepBoosted_probQCDbb());  
    event.set(m_o_DeepDoubleBvLJet_probHbb_jet1,closest_puppijet1->btag_DeepDoubleBvLJet_probHbb());  
    event.set(m_o_DeepDoubleBvLJet_probHbb_jet2,closest_puppijet2->btag_DeepDoubleBvLJet_probHbb()); 
    event.set(m_o_DeepDoubleBvLJet_probQCD_jet1,closest_puppijet1->btag_DeepDoubleBvLJet_probQCD()); 
    event.set(m_o_DeepDoubleBvLJet_probQCD_jet2,closest_puppijet2->btag_DeepDoubleBvLJet_probQCD());  
    	
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

    //puppi met variables	
    event.set(m_o_met_pt,event.met->pt());
    event.set(m_o_met_eta,event.met->v4().Eta());
    event.set(m_o_met_phi,event.met->phi());
    event.set(m_o_met_mass,event.met->v4().M());
    event.set(m_o_met_sumEt,event.met->sumEt());
                                         
    // 3. decide whether or not to keep the current event in the output:
    return njet_selection && dijet_selection;
}

// as we want to run the UHHNtupleConverterCycleNew directly with AnalysisModuleRunner,
// make sure the UHHNtupleConverterModule is found by class name. This is ensured by this macro:
UHH2_REGISTER_ANALYSIS_MODULE(UHHNtupleConverterModule)

}
