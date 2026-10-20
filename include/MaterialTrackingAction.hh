#ifndef MATERIAL_TRACKING_ACTION_HH
#define MATERIAL_TRACKING_ACTION_HH
#include "G4UserTrackingAction.hh"
#include "G4Track.hh"
#include "musrRootOutput.hh"
class MaterialTrackingAction : public G4UserTrackingAction {
public:
  void PostUserTrackingAction(const G4Track* t) override {
    auto* out=musrRootOutput::GetRootInstance();
    if (!out->MaterialTruthEnabled()) return;
    const auto& x=t->GetPosition(); const auto& p=t->GetMomentum();
    out->materialTruth.Terminal(t->GetTrackID(),t->GetParentID(),t->GetDefinition()->GetPDGEncoding(),
      t->GetTrackStatus(),{{x.x()/CLHEP::mm,x.y()/CLHEP::mm,x.z()/CLHEP::mm}},
      {{p.x()/CLHEP::MeV,p.y()/CLHEP::MeV,p.z()/CLHEP::MeV}},t->GetGlobalTime()/CLHEP::ns);
  }
};
#endif
