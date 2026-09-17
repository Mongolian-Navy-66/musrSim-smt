#ifndef PRIMARY_MUON_PLANE_TRUTH_HH
#define PRIMARY_MUON_PLANE_TRUTH_HH

#include <array>
#include <cmath>
#include <limits>
#include <set>
#include <stdexcept>

// Coordinates are Geant4 world coordinates in mm. The observer is passive: it
// never changes a track, a step, the geometry, or the random-number stream.
class PrimaryMuonPlaneTruth {
public:
  static const int kPlanes = 4;

  PrimaryMuonPlaneTruth() : enabled(false), primaryMuonCount(0) {
    referenceZ.fill(std::numeric_limits<double>::quiet_NaN());
    Reset();
  }

  void Configure(const std::array<double, kPlanes>& z) {
    for (int i = 0; i < kPlanes; ++i) {
      if (!std::isfinite(z[i]) || (i && !(z[i - 1] > z[i]))) {
        throw std::invalid_argument("truthPlaneZ requires four finite, strictly descending z values");
      }
    }
    referenceZ = z;
    enabled = true;
    Reset();
  }

  void Reset() {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    x.fill(nan);
    y.fill(nan);
    z.fill(nan);
    count.fill(0);
    primaryTrackIDs.clear();
    primaryMuonCount = 0;
  }

  void Observe(int trackID, int parentID, int pdg,
               double preX, double preY, double preZ,
               double postX, double postY, double postZ) {
    if (!enabled || parentID != 0 || (pdg != 13 && pdg != -13)) return;
    if (primaryTrackIDs.insert(trackID).second) ++primaryMuonCount;
    if (!std::isfinite(preX) || !std::isfinite(preY) || !std::isfinite(preZ) ||
        !std::isfinite(postX) || !std::isfinite(postY) || !std::isfinite(postZ) ||
        !(preZ > postZ)) return;

    for (int i = 0; i < kPlanes; ++i) {
      if (!(preZ > referenceZ[i] && postZ <= referenceZ[i])) continue;
      const double fraction = (preZ - referenceZ[i]) / (preZ - postZ);
      if (count[i] == 0) {
        x[i] = preX + fraction * (postX - preX);
        y[i] = preY + fraction * (postY - preY);
        z[i] = referenceZ[i];
      }
      ++count[i];
    }
  }

  bool enabled;
  std::array<double, kPlanes> x, y, z, referenceZ;
  std::array<int, kPlanes> count;
  int primaryMuonCount;

private:
  std::set<int> primaryTrackIDs;
};

#endif
