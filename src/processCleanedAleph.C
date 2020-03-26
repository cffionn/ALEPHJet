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
  Float_t px_[nPartMax_], py_[nPartMax_], pz_[nPartMax_], m_[nPartMax_];
  Short_t chg_[nPartMax_];
  UChar_t pwflag_[nPartMax_];
  
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

  std::ifstream inFile(inFileName.c_str());
  std::string tempStr;

  bool lastLineEmpty = false;
  bool endOnFill = false;
  while(std::getline(inFile, tempStr)){
    std::vector<std::string> line = stringToVect(cleanString(tempStr));
    
    if(line.size() == 0){
      if(lastLineEmpty){
	endOnFill = true;
	break;
      }
      else alephTree_p->Fill();

      lastLineEmpty = true;
    }
    else if(line.size() == 3){
      lastLineEmpty = false;
      run_ = (UChar_t)(std::stoi(line.at(0)));
      evt_ = (UChar_t)(std::stoi(line.at(1)));
      ecm_ = std::stof(line.at(2));
      nPart_ = 0;
    }
    else if(line.size() == 5){
      lastLineEmpty = false;
      continue;
    }
    else if(line.size() == 11){
      lastLineEmpty = false;
      px_[nPart_] = std::stof(line.at(0));
      py_[nPart_] = std::stof(line.at(1));
      pz_[nPart_] = std::stof(line.at(2));
      m_[nPart_] = std::stof(line.at(3));
      chg_[nPart_] = (Short_t)(std::stoi(line.at(4)));
      pwflag_[nPart_] = (UChar_t)(std::stoi(line.at(5)));
      ++nPart_;
    }
  }
  
  if(!endOnFill) alephTree_p->Fill();

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
