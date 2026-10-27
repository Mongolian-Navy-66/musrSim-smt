#include "ROITruth.hh"
#include <cassert>
#include <stdexcept>

int main() {
  ROITruth truth;
  truth.Add("ECAL", "phys_LYSO_Crystal", 0);
  truth.Add("plastic", "phys_Plastic", 1);
  truth.enabled=true;
  truth.Observe(0,13,"outside",0,"phys_LYSO_Crystal",0,true,0.);
  assert(truth.entered[0]==1 && truth.deposited[0]==0 && truth.edep[0]==0.);
  truth.Observe(1,11,"phys_LYSO_Crystal",0,"phys_LYSO_Crystal",0,false,3.);
  assert(truth.deposited[0]==0);
  truth.Observe(0,-13,"phys_LYSO_Crystal",0,"phys_LYSO_Crystal",0,false,0.75);
  assert(truth.deposited[0]==1 && truth.edep[0]==0.75);
  truth.Observe(0,13,"phys_Plastic",0,"phys_Plastic",0,false,1.);
  assert(truth.entered[1]==0);
  truth.Observe(0,13,"phys_Plastic",1,"phys_Plastic",1,false,2.);
  assert(truth.entered[1]==1 && truth.edep[1]==2.);
  truth.Reset();
  assert(truth.entered[0]==0 && truth.deposited[1]==0 && truth.edep[0]==0.);
  truth.enabled=false;
  truth.Observe(0,13,"phys_LYSO_Crystal",0,"",0,false,5.);
  assert(truth.edep[0]==0.);
  bool duplicate=false;
  try { truth.Add("ECAL","other",0); } catch (const std::invalid_argument&) { duplicate=true; }
  assert(duplicate);
}
