#ifndef MATERIAL_TRACKING_ACTION_HH
#define MATERIAL_TRACKING_ACTION_HH
#include "G4UserTrackingAction.hh"
#include "G4Track.hh"
#include "G4DynamicParticle.hh"
#include "G4PrimaryParticle.hh"
#include "musrRootOutput.hh"
class MaterialTrackingAction : public G4UserTrackingAction {
public:
  void PreUserTrackingAction(const G4Track* t) override {
    auto* out=musrRootOutput::GetRootInstance();
    if (!out->cryTruth.enabled || t->GetParentID()!=0) return;
    const auto* primary=t->GetDynamicParticle()->GetPrimaryParticle();
    const auto* info=primary ? dynamic_cast<const CryPrimaryInfo*>(primary->GetUserInformation()) : nullptr;
    if (!info) throw std::runtime_error("missing CRY primary metadata");
    out->cryTruth.Register(info->index,t->GetTrackID());
  }
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
