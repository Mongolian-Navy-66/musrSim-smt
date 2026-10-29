#include "CryTruth.hh"
#include <cassert>
int main() {
  ROITruth roi; roi.Add("ECAL","phys_LYSO_Crystal",0); roi.enabled=true;
  CryTruth cry; cry.enabled=true; cry.Reset(1);
  int a=cry.Add(13,1000,1,2,3,0,0,-1,0), b=cry.Add(-13,2000,4,5,6,0,0,-1,0), c=cry.Add(22,10,0,0,0,0,0,-1,0);
  cry.Register(a,1); cry.Register(b,2); cry.Register(c,3);
  cry.Observe(1,0,13,"outside",0,"phys_LYSO_Crystal",0,true,0,roi);
  cry.Observe(1,0,13,"phys_LYSO_Crystal",0,"phys_LYSO_Crystal",0,false,2,roi);
  cry.Observe(2,0,-13,"phys_LYSO_Crystal",0,"phys_LYSO_Crystal",0,false,3,roi);
  cry.Observe(3,0,22,"phys_LYSO_Crystal",0,"phys_LYSO_Crystal",0,false,9,roi);
  cry.Observe(4,1,13,"phys_LYSO_Crystal",0,"phys_LYSO_Crystal",0,false,9,roi);
  assert(cry.roiEntered[0]==1 && cry.roiEntered[1]==1 && cry.roiEntered[2]==0);
  assert(cry.roiEdep[0]==2 && cry.roiEdep[1]==3 && cry.roiEdep[2]==0);
  cry.Reset(1); assert(cry.pdg.empty() && cry.roiEdep.empty());
}
