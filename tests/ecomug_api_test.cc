// SPDX-License-Identifier: GPL-3.0-only
#include "EcoMug.h"

#include <array>
#include <cassert>
#include <cmath>

int main() {
  EcoMug generator;
  generator.SetSeed(12345);
  generator.SetUseHSphere();
  generator.SetHSphereRadius(1000.);
  generator.SetHSphereCenterPosition({{0., 0., 0.}});
  generator.SetMinimumMomentum(0.01);
  generator.SetMaximumMomentum(10.);
  generator.SetMinimumTheta(0.);
  generator.SetMaximumTheta(1.57);
  generator.SetMinimumPhi(0.);
  generator.SetMaximumPhi(6.28);
  generator.Generate();
  const std::array<double, 3>& position = generator.GetGenerationPosition();
  for (double coordinate : position) assert(std::isfinite(coordinate));
  assert(std::isfinite(generator.GetGenerationMomentum()));
  assert(std::isfinite(generator.GetGenerationTheta()));
  assert(std::isfinite(generator.GetGenerationPhi()));
  assert(std::cos(generator.GetGenerationTheta()) < 0.);  // EcoMug's world z is downward.
  assert(generator.GetCharge() == -1 || generator.GetCharge() == 1);

  // A requested seed must reproduce the complete primary, including charge.
  EcoMug first;
  EcoMug second;
  for (EcoMug* value : {&first, &second}) {
    value->SetSeed(67890);
    value->SetUseHSphere();
    value->SetHSphereRadius(1000.);
    value->SetMinimumMomentum(0.01);
    value->SetMaximumMomentum(10.);
  }
  for (int index = 0; index < 32; ++index) {
    first.Generate();
    second.Generate();
    if (first.GetGenerationPosition() != second.GetGenerationPosition() ||
        first.GetGenerationMomentum() != second.GetGenerationMomentum() ||
        first.GetGenerationTheta() != second.GetGenerationTheta() ||
        first.GetGenerationPhi() != second.GetGenerationPhi() ||
        first.GetCharge() != second.GetCharge()) {
      return 1;
    }
  }
  return 0;
}
