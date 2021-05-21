//cpp dependencies
#include <fstream>
#include <iostream>
#include <string>

//ROOT dependencies
#include "TDatime.h"
#include "TFile.h"
#include "TMath.h"
#include "TTree.h"

//Local dependencies
#include "include/checkMakeDir.h"

std::string cleanString(std::string inStr)
{
  const std::string valid = "0123456789.- ";

  unsigned int pos = 0;
  while(pos < inStr.size()){
    if(valid.find(inStr.substr(pos,1)) != std::string::npos) ++pos;
    else if(pos == inStr.size()-1) inStr = inStr.substr(0, inStr.size()-1);
    else inStr = inStr.substr(0, pos) + " " + inStr.substr(pos+1, inStr.size());
  }
    
  if(inStr.size() == 0) return inStr;
  while(inStr.find("  ") != std::string::npos){inStr = inStr.substr(0, inStr.find("  ")) + inStr.substr(inStr.find("  ")+1, inStr.size());}
  if(inStr.size() == 0) return inStr;
  while(inStr.substr(0, 1).find(" ") != std::string::npos){inStr = inStr.substr(1,inStr.size());}
  if(inStr.size() == 0) return inStr;
  while(inStr.substr(inStr.size()-1, 1).find(" ") != std::string::npos){inStr = inStr.substr(0, inStr.size()-1);}

  return inStr;
}

std::vector<std::string> stringToVect(std::string inStr)
{
  std::vector<std::string> outVect;
  if(inStr.size() == 0) return outVect;
  
  while(inStr.substr(0,1).find(" ") != std::string::npos){
    inStr = inStr.substr(1,inStr.size()-1);
    if(inStr.size() == 0) break;
  }
  if(inStr.size() == 0) return outVect;

  while(inStr.substr(inStr.size()-1,1).find(" ") != std::string::npos){
    inStr = inStr.substr(0, inStr.size()-1);
    if(inStr.size() == 0) break;
  }
  if(inStr.size() == 0) return outVect;
  
  inStr = inStr + " ";
  
  while(inStr.find(" ") != std::string::npos){
    outVect.push_back(inStr.substr(0, inStr.find(" ")));
    inStr = inStr.substr(inStr.find(" ")+1, inStr.size());
  }  
  
  return outVect;
}

int processCleanedAleph(const std::string inFileName)
{
  TDatime* date = new TDatime();
  const std::string dateStr = std::to_string(date->GetDate());
  delete date;

  const Int_t nValidYears = 10;
  const std::string validYears[nValidYears] = {"1992", "1993", "1994", "1995", "1996", "1996", "1997", "1998", "1999", "2000"};
  const UShort_t validYearsUShort[nValidYears] = {1992, 1993, 1994, 1995, 1996, 1996, 1997, 1998, 1999, 2000};
  const Int_t nValidRunPeriods = 2;
  const std::string validRunPeriods[nValidRunPeriods] = {"LEP1", "LEP2"};
  const UChar_t validRunPeriodsUChar[nValidRunPeriods] = {1, 2};
  
  std::string year = "";
  UShort_t yearUShort = 0;
  std::string runPeriod = "";  
  UShort_t runPeriodUChar = 0;
  
  for(Int_t vI = 0; vI < nValidYears; ++vI){
    if(inFileName.find(validYears[vI]) != std::string::npos){
      year = validYears[vI];
      yearUShort = validYearsUShort[vI];
      break;
    }    
  }

  for(Int_t vI = 0; vI < nValidRunPeriods; ++vI){
    if(inFileName.find(validRunPeriods[vI]) != std::string::npos){
      runPeriod = validRunPeriods[vI];
      runPeriodUChar = validRunPeriodsUChar[vI];
      break;
    }    
  }

  if(year.size() == 0){
    std::cout << "inFileName \'" << inFileName << "\' contains no valid year. Pick from: ";
    for(auto const & p : validYears){
      std::cout << "\'" << p << "\', ";
    }
    std::cout << std::endl;
    return 1;
  }

  if(runPeriod.size() == 0){
    std::cout << "inFileName \'" << inFileName << "\' contains no valid run period. Pick from: ";
    for(auto const & p : validRunPeriods){
      std::cout << "\'" << p << "\', ";
    }
    std::cout << std::endl;
    return 1;
  }

  checkMakeDir("output");
  std::string outFileName = inFileName;
  while(outFileName.find("/") != std::string::npos){outFileName.replace(0, outFileName.find("/")+1, "");}
  while(outFileName.find(".") != std::string::npos){outFileName = outFileName.substr(0, outFileName.find("."));}
  outFileName = "output/" + outFileName + "_CleanedAleph_" + dateStr + ".root";
  
  UChar_t lep_ = runPeriodUChar;
  UShort_t year_ = yearUShort;
  UInt_t run_;
  UInt_t evt_;
  Float_t ecm_;

  UChar_t nPart_;
  const UChar_t nPartMax_ = 255;
  Float_t px_[nPartMax_], py_[nPartMax_], pz_[nPartMax_], m_[nPartMax_], dEdXCode_[nPartMax_], dEdXElectron_[nPartMax_], dEdXPion_[nPartMax_], dEdXKaon_[nPartMax_], dEdXProton_[nPartMax_]; 
  Short_t chg_[nPartMax_];
  UChar_t pwflag_[nPartMax_];

  UChar_t nV0_;
  Float_t v0Vx_[nPartMax_], v0Vy_[nPartMax_], v0Vz_[nPartMax_];
  Int_t v0TrkPos1_[nPartMax_], v0TrkPos2_[nPartMax_];
  Float_t v0Px1_[nPartMax_], v0Py1_[nPartMax_], v0Pz1_[nPartMax_], v0Px2_[nPartMax_], v0Py2_[nPartMax_], v0Pz2_[nPartMax_];
  
  TFile* outFile_p = new TFile(outFileName.c_str(), "RECREATE");
  TTree* alephTree_p = new TTree("alephTree", "");
  alephTree_p->Branch("lep", &lep_, "lep/b");
  alephTree_p->Branch("year", &year_, "year/s");
  alephTree_p->Branch("run", &run_, "run/i");
  alephTree_p->Branch("evt", &evt_, "evt/i");
  alephTree_p->Branch("ecm", &ecm_, "ecm/F");
  
  alephTree_p->Branch("nPart", &nPart_, "nPart/b");
  alephTree_p->Branch("px", px_, "px[nPart]/F");
  alephTree_p->Branch("py", py_, "py[nPart]/F");
  alephTree_p->Branch("pz", pz_, "pz[nPart]/F");
  alephTree_p->Branch("m", m_, "m[nPart]/F");
  alephTree_p->Branch("chg", chg_, "chg[nPart]/S");
  alephTree_p->Branch("pwflag", pwflag_, "pwflag[nPart]/b");
  alephTree_p->Branch("dEdXCode", dEdXCode_, "dEdXCode[nPart]/F");
  alephTree_p->Branch("dEdXElectron", dEdXElectron_, "dEdXElectron[nPart]/F");
  alephTree_p->Branch("dEdXPion", dEdXPion_, "dEdXPion[nPart]/F");
  alephTree_p->Branch("dEdXKaon", dEdXKaon_, "dEdXKaon[nPart]/F");
  alephTree_p->Branch("dEdXProton", dEdXProton_, "dEdXProton[nPart]/F");

  alephTree_p->Branch("nV0", &nV0_, "nV0/b");

  alephTree_p->Branch("v0Vx", v0Vx_, "v0Vx[nV0]/F");
  alephTree_p->Branch("v0Vy", v0Vy_, "v0Vy[nV0]/F");
  alephTree_p->Branch("v0Vz", v0Vz_, "v0Vz[nV0]/F");
  
  alephTree_p->Branch("v0Px1", v0Px1_, "v0Px1[nV0]/F");
  alephTree_p->Branch("v0Py1", v0Py1_, "v0Py1[nV0]/F");
  alephTree_p->Branch("v0Pz1", v0Pz1_, "v0Pz1[nV0]/F");
  alephTree_p->Branch("v0TrkPos1", v0TrkPos1_, "v0TrkPos1[nV0]/I");

  alephTree_p->Branch("v0Px2", v0Px2_, "v0Px2[nV0]/F");
  alephTree_p->Branch("v0Py2", v0Py2_, "v0Py2[nV0]/F");
  alephTree_p->Branch("v0Pz2", v0Pz2_, "v0Pz2[nV0]/F");
  alephTree_p->Branch("v0TrkPos2", v0TrkPos2_, "v0TrkPos2[nV0]/I");
  

  std::ifstream inFile(inFileName.c_str());
  std::string tempStr;

  bool prevLineEmpty = false;
  bool endOnFill = false;

  bool hitEighteen = true;
  while(std::getline(inFile, tempStr)){
    std::vector<std::string> line = stringToVect(cleanString(tempStr));

    /*
    std::cout << line.size() << ", " << std::endl;
    if(line.size() == 20){
      for(unsigned int i = 0; i < line.size(); ++i){
	std::cout << "'" << line[i] << "' ";       
      }
      std::cout << std::endl;

      return 1;
    }
    */
    
    if(line.size() == 0){
      if(prevLineEmpty){
	endOnFill = true;
      	break;
      }
      else alephTree_p->Fill();

      prevLineEmpty = true;
    }
    else if(line.size() == 3){
      prevLineEmpty = false;
      run_ = (UChar_t)(std::stoi(line.at(0)));
      evt_ = (UChar_t)(std::stoi(line.at(1)));
      ecm_ = std::stof(line.at(2));
      nPart_ = 0;
      nV0_ = 0;
      hitEighteen = false;
    }
    else if(line.size() == 5){
      prevLineEmpty = false;
      continue;
    }
    else if(line.size() == 18){
      prevLineEmpty = false;
      px_[nPart_] = std::stof(line.at(0));
      py_[nPart_] = std::stof(line.at(1));
      pz_[nPart_] = std::stof(line.at(2));
      m_[nPart_] = std::stof(line.at(3));
      chg_[nPart_] = (Short_t)(std::stoi(line.at(4)));
      pwflag_[nPart_] = (UChar_t)(std::stoi(line.at(5)));

      dEdXCode_[nPart_] = std::stof(line.at(13));
      dEdXElectron_[nPart_] = std::stof(line.at(14));
      dEdXPion_[nPart_] = std::stof(line.at(15));
      dEdXKaon_[nPart_] = std::stof(line.at(16));
      dEdXProton_[nPart_] = std::stof(line.at(17));
      
      ++nPart_;

      hitEighteen = true;
    }
    else if(line.size() == 6 && hitEighteen){
      v0Vx_[nV0_] = std::stof(line.at(0));
      v0Vy_[nV0_] = std::stof(line.at(1));
      v0Vz_[nV0_] = std::stof(line.at(2));

      std::getline(inFile, tempStr);
      line = stringToVect(cleanString(tempStr));
      v0TrkPos1_[nV0_] = std::stoi(line.at(0));
      v0Px1_[nV0_] = std::stof(line.at(1));
      v0Py1_[nV0_] = std::stof(line.at(2));
      v0Pz1_[nV0_] = std::stof(line.at(3));

      std::getline(inFile, tempStr);
      line = stringToVect(cleanString(tempStr));
      v0TrkPos2_[nV0_] = std::stoi(line.at(0));
      v0Px2_[nV0_] = std::stof(line.at(1));
      v0Py2_[nV0_] = std::stof(line.at(2));
      v0Pz2_[nV0_] = std::stof(line.at(3));
      
      ++nV0_;
    }    
  }
  
  //  if(endOnFill) alephTree_p->Fill();

  inFile.close();

  outFile_p->cd();

  alephTree_p->Write("", TObject::kOverwrite);
  delete alephTree_p;

  outFile_p->Close();
  delete outFile_p;
  
  return 0;
}

int main(int argc, char* argv[])
{
  if(argc != 2){
    std::cout << "Usage: ./bin/processCleanedAleph.exe <inFileName>" << std::endl;
    return 1;
  }

  int retVal = 0;
  retVal += processCleanedAleph(argv[1]);
  return retVal;
}
