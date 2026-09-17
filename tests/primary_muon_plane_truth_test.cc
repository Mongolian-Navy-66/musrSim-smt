#include "PrimaryMuonPlaneTruth.hh"

#include <cassert>
#include <cmath>

int main() {
  PrimaryMuonPlaneTruth truth;
  truth.Configure({{749., 448., -272., -574.}});
  truth.Reset();

  // Four crossings with a real kink between the upper and lower segments.
  truth.Observe(1, 0, -13, 0., 0., 760., 11., 0., 738.);
  truth.Observe(1, 0, -13, 30., 0., 460., 41., 0., 438.);
  truth.Observe(1, 0, -13, 100., 0., -260., 122., 0., -284.);
  truth.Observe(1, 0, -13, 140., 0., -560., 154., 0., -588.);
  for (int i = 0; i < 4; ++i) assert(truth.count[i] == 1);
  assert(truth.primaryMuonCount == 1);
  assert(std::abs(truth.x[0] - 5.5) < 1e-12);
  assert(std::abs(truth.x[1] - 36.) < 1e-12);
  assert(std::abs(truth.x[2] - 111.) < 1e-12);
  assert(std::abs(truth.x[3] - 147.) < 1e-12);
  assert(truth.z[0] == 749. && truth.z[3] == -574.);

  // The half-open interval records a step ending on the plane only once.
  truth.Reset();
  truth.Observe(1, 0, 13, 0., 0., 750., 1., 0., 749.);
  truth.Observe(1, 0, 13, 1., 0., 749., 2., 0., 748.);
  assert(truth.count[0] == 1);
  assert(truth.count[1] == 0 && std::isnan(truth.x[1]));

  // Upward and secondary tracks do not supply the primary-muon reference.
  truth.Observe(1, 0, 13, 0., 0., 748., 0., 0., 750.);
  truth.Observe(2, 1, 13, 0., 0., 750., 0., 0., 748.);
  assert(truth.count[0] == 1 && truth.primaryMuonCount == 1);

  // Real re-entry and a second primary remain visible as ambiguous states.
  truth.Observe(1, 0, 13, 3., 0., 750., 4., 0., 748.);
  truth.Observe(3, 0, -13, 5., 0., 750., 6., 0., 748.);
  assert(truth.count[0] == 3 && truth.primaryMuonCount == 2);
  assert(truth.x[0] == 1.);

  truth.Reset();
  assert(truth.primaryMuonCount == 0 && truth.count[0] == 0);
  assert(std::isnan(truth.x[0]));
  return 0;
}
