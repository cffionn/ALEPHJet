//cpp dependencies
#include <iostream>
#include <string>

//ROOT dependencies
#include "TDatime.h"
#include "TFile.h"
#include "TMath.h"
#include "TTree.h"

//Fastjet dependencies
#include "fastjet/ClusterSequence.hh"
#include "fastjet/JetDefinition.hh"
#include "fastjet/PseudoJet.hh"

//Local dependencies
#include "include/checkMakeDir.h"
#include "include/plotUtilities.h"

int createAlephJet(const std::string inFileName)
{
  if(!checkFile(inFileName)){
    std::cout << "Given inFileName \'" << inFileName << "\' is invalid. return 1" << std::endl;
    return 1;
  }
  
  TDatime* date = new TDatime();
  const std::string dateStr = std::to_string(date->GetDate());
  delete date;

  std::string outFileName = inFileName;
  while(outFileName.find("/") != std::string::npos){outFileName.replace(0, outFileName.find("/")+1, "");}
  while(outFileName.find(".") != std::string::npos){outFileName.replace(outFileName.find("."), outFileName.size(), "");}

  outFileName = "output/" + outFileName + "_ALEPHJet_" + dateStr + ".root";

  TFile* outFile_p = new TFile(outFileName.c_str(), "RECREATE");
  TTree* alephJet_p = new TTree("globalJet", "");

  UChar_t lep_;
  UShort_t year_;
  UInt_t run_;
  UInt_t evt_;
  Float_t ecm_;

  alephJet_p->Branch("lep", &lep_, "lep/b");
  alephJet_p->Branch("year", &year_, "year/s");
  alephJet_p->Branch("run", &run_, "run/i");
  alephJet_p->Branch("evt", &evt_, "evt/i");
  alephJet_p->Branch("ecm", &ecm_, "ecm/F");
  
  const UChar_t nRParam = 2;
  const Double_t rParam[nRParam] = {0.4, 0.8};
  fastjet::JetDefinition jDef[nRParam];

  const UChar_t nPartMax_ = 255;
  UChar_t nJt_[nRParam];
  Float_t jtPx_[nRParam][nPartMax_];
  Float_t jtPy_[nRParam][nPartMax_];
  Float_t jtPz_[nRParam][nPartMax_];
  Float_t jtE_[nRParam][nPartMax_];
  Float_t jtPt_[nRParam][nPartMax_];
  
  TTree* inclusiveJet_p[nRParam];
  for(UChar_t rI = 0; rI < nRParam; ++rI){
    jDef[rI] = fastjet::JetDefinition(fastjet::ee_genkt_algorithm, rParam[rI], -1, fastjet::E_scheme);
    std::string treeName = "antiktESchemeR" + prettyString(rParam[rI], 1, true) + "Jet";
    inclusiveJet_p[rI] = new TTree(treeName.c_str(), "");

    inclusiveJet_p[rI]->Branch("nJt", &(nJt_[rI]), "nJt/b");
    inclusiveJet_p[rI]->Branch("jtPx", jtPx_[rI], "jtPx/F");
    inclusiveJet_p[rI]->Branch("jtPy", jtPy_[rI], "jtPy/F");
    inclusiveJet_p[rI]->Branch("jtPz", jtPz_[rI], "jtPz/F");
    inclusiveJet_p[rI]->Branch("jtE", jtE_[rI], "jtE/F");
    inclusiveJet_p[rI]->Branch("jtPt", jtPt_[rI], "jtPt/F");
  }

  TFile* inFile_p = new TFile(inFileName.c_str(), "READ");
  TTree* alephTree_p = (TTree*)inFile_p->Get("alephTree");

  UChar_t nPart_;
  Float_t px_[nPartMax_], py_[nPartMax_], pz_[nPartMax_], m_[nPartMax_];
  Short_t chg_[nPartMax_];
  UChar_t pwflag_[nPartMax_];

  alephTree_p->SetBranchAddress("lep", &lep_);
  alephTree_p->SetBranchAddress("year", &year_);
  alephTree_p->SetBranchAddress("run", &run_);
  alephTree_p->SetBranchAddress("evt", &evt_);
  alephTree_p->SetBranchAddress("ecm", &ecm_);  
  alephTree_p->SetBranchAddress("nPart", &nPart_);
  alephTree_p->SetBranchAddress("px", &px_);
  alephTree_p->SetBranchAddress("py", &py_);
  alephTree_p->SetBranchAddress("pz", &pz_);
  alephTree_p->SetBranchAddress("m", &m_);
  alephTree_p->SetBranchAddress("chg", &chg_);
  alephTree_p->SetBranchAddress("pwflag", &pwflag_);
  
  const ULong64_t nEntries = alephTree_p->GetEntries();
  const ULong64_t nDiv = TMath::Max((ULong64_t)1, (ULong64_t)(nEntries/20));

  std::cout << "Processing jets..." << std::endl;
  for(ULong64_t entry = 0; entry < nEntries; ++entry){
    if(entry%nDiv == 0) std::cout << " Entry " << entry << "/" << nEntries << std::endl;
    alephTree_p->GetEntry(entry);    

    std::vector<fastjet::PseudoJet> particles;
    
    for(UChar_t pI = 0; pI < nPart_; ++pI){
      double energy = TMath::Sqrt(px_[pI]*px_[pI] + py_[pI]*py_[pI] + pz_[pI]*pz_[pI] + m_[pI]*m_[pI]);
      particles.push_back(fastjet::PseudoJet(px_[pI], py_[pI], pz_[pI], energy));
    }    
    
    alephJet_p->Fill();
    for(UChar_t rI = 0; rI < nRParam; ++rI){
      fastjet::ClusterSequence cs(particles, jDef[rI]);
      std::vector<fastjet::PseudoJet> jets = fastjet::sorted_by_pt(cs.inclusive_jets());

      nJt_[rI] = 0;
      for(auto const& j : jets){
	jtPx_[rI][nJt_[rI]] = j.px();
	jtPy_[rI][nJt_[rI]] = j.py();
	jtPz_[rI][nJt_[rI]] = j.pz();
	jtE_[rI][nJt_[rI]] = j.e();
	jtPt_[rI][nJt_[rI]] = j.pt();
	++(nJt_[rI]);
      }      
      
      inclusiveJet_p[rI]->Fill();
    }
  }
  

  inFile_p->Close();
  delete inFile_p;
  
  outFile_p->cd();

  alephJet_p->Write("", TObject::kOverwrite);
  delete alephJet_p;

  for(UChar_t rI = 0; rI < nRParam; ++rI){
    inclusiveJet_p[rI]->Write("", TObject::kOverwrite);
    delete inclusiveJet_p[rI];
  }
  
  outFile_p->Close();
  delete outFile_p;
  
  return 0;
}

int main(int argc, char* argv[])
{
  if(argc != 2){
    std::cout << "Usage: ./bin/createAlephJet.exe <inFileName>" << std::endl;
    return 1;
  }

  int retVal = 0;
  retVal += createAlephJet(argv[1]);
  return retVal;
}
