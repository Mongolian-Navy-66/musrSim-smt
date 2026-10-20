#include "../include/MaterialBoundaryTruth.hh"
#include "../third_party/EcoMug/EcoMug.h"
#include <cassert>
int main() {
  MaterialBoundaryTruth b; std::array<double,3> pos={{0,0,138}}, p={{0,0,-3000}};
  b.Observe(1,0,-13,true,false,true,"World","Pb",pos,p,p,0); assert(b.track.empty());
  b.enabled=true;
  b.Observe(2,1,13,true,false,true,"World","Pb",pos,p,p,0);
  b.Observe(3,0,11,true,false,true,"World","Pb",pos,p,p,0); assert(b.track.empty());
  b.Observe(1,0,-13,true,false,true,"World","Pb",pos,p,p,0);
  b.Observe(1,0,-13,true,true,true,"PbA","PbB",pos,p,p,1);
  b.Observe(1,0,-13,true,true,false,"Pb","World",pos,p,p,2);
  assert((b.kind==std::vector<int>{1,0,-1}));
  b.Observe(1,0,-13,false,true,true,"Pb","Pb",pos,p,p,3); assert(b.kind.size()==3);
  b.Observe(1,0,-13,true,false,true,"World","Pb",pos,p,p,4);
  b.Terminal(1,0,-13,2,pos,{{0,0,0}},5); assert(b.kind.back()==1 && b.terminalStatus[0]==2);
  b.Reset(); assert(b.track.empty() && b.terminalTrack.empty());
  assert(MaterialEventSeed(7100000,99999)==7200000);
  assert(MaterialEventSeed(7200000,0)==7200001);
  bool threw=false; try {MaterialEventSeed(2147483647L,0);} catch (const std::invalid_argument&) {threw=true;} assert(threw);
  assert(MaterialFixedMomentum(10,0)==10 && MaterialFixedMomentum(10,3000)==3000);
  EcoMug a,bg;
  for (auto* g : {&a,&bg}) {
    g->SetSeed(7100); g->SetUseHSphere(); g->SetHSphereRadius(1000);
    g->SetHSphereCenterPosition({{0,0,0}});
  }
  for (int i=0;i<1000;++i) {
    a.Generate(); bg.Generate();
    const double fixed=MaterialFixedMomentum(a.GetGenerationMomentum()*1000,3000);
    assert(fixed==3000);
    assert(a.GetGenerationPosition()==bg.GetGenerationPosition());
    assert(a.GetGenerationTheta()==bg.GetGenerationTheta());
    assert(a.GetGenerationPhi()==bg.GetGenerationPhi());
    assert(a.GetCharge()==bg.GetCharge());
    assert(a.GetGenerationMomentum()==bg.GetGenerationMomentum());
  }
}
