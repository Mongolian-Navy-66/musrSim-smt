#ifndef CRY_TRUTH_HH
#define CRY_TRUTH_HH
#include "ROITruth.hh"
#include <string>
#include <cmath>
#include <vector>
#include <stdexcept>
#include "G4VUserPrimaryParticleInformation.hh"

class CryPrimaryInfo : public G4VUserPrimaryParticleInformation {
public:
  explicit CryPrimaryInfo(int i) : index(i) {}
  void Print() const override {}
  int index;
};

struct CryTruth {
  bool enabled=false;
  double elapsed=0;
  int roiCount=0, nMuon=0;
  std::vector<int> pdg, trackID, roiEntered;
  std::vector<double> energy, x, y, z, u, v, w, time, roiEdep;
  void Reset(int nr) {
    roiCount=nr; nMuon=0; pdg.clear(); trackID.clear(); energy.clear(); x.clear(); y.clear(); z.clear();
    u.clear(); v.clear(); w.clear(); time.clear(); roiEntered.clear(); roiEdep.clear(); elapsed=0;
  }
  int Add(int code,double ke,double xx,double yy,double zz,double ux,double uy,double uz,double t) {
    int index=pdg.size(); if (std::abs(code)==13) ++nMuon; pdg.push_back(code); trackID.push_back(0); energy.push_back(ke);
    x.push_back(xx); y.push_back(yy); z.push_back(zz); u.push_back(ux); v.push_back(uy); w.push_back(uz); time.push_back(t);
    roiEntered.insert(roiEntered.end(),roiCount,0); roiEdep.insert(roiEdep.end(),roiCount,0.);
    return index;
  }
  void Register(int index,int track) {
    if (index<0 || static_cast<std::size_t>(index)>=trackID.size() || trackID[index]!=0)
      throw std::runtime_error("invalid CRY primary track mapping");
    trackID[index]=track;
  }
  void Observe(int track,int parent,int code,const std::string& before,int beforeCopy,
               const std::string& after,int afterCopy,bool boundary,double e,const ROITruth& roi) {
    if (!enabled || parent!=0 || std::abs(code)!=13) return;
    for (std::size_t i=0;i<trackID.size();++i) if (trackID[i]==track) {
      for (int j=0;j<roiCount;++j) {
        const bool inside=before==roi.volumes[j] && beforeCopy==roi.copies[j];
        const bool entering=boundary && after==roi.volumes[j] && afterCopy==roi.copies[j];
        if (inside || entering) roiEntered[i*roiCount+j]=1;
        if (inside && e>0) roiEdep[i*roiCount+j]+=e;
      }
      return;
    }
    throw std::runtime_error("unmapped CRY primary muon");
  }
};
#endif
