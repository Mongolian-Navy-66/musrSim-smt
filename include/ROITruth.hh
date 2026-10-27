#ifndef ROI_TRUTH_HH
#define ROI_TRUTH_HH

#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

// Passive, event-level scoring for explicitly configured physical volumes.
// Energy is in MeV. No Geant4 state or random numbers are modified.
class ROITruth {
public:
  bool enabled = false;
  std::vector<std::string> names, volumes;
  std::vector<int> copies, entered, deposited;
  std::vector<double> edep;

  void Add(const std::string& name, const std::string& volume, int copy) {
    if (name.empty() || volume.empty() || copy < 0)
      throw std::invalid_argument("ROI name, physical volume and nonnegative copy required");
    for (std::size_t i=0; i<names.size(); ++i) {
      if (names[i] == name || (volumes[i] == volume && copies[i] == copy))
        throw std::invalid_argument("duplicate ROI name or physical volume instance");
    }
    names.push_back(name);
    volumes.push_back(volume);
    copies.push_back(copy);
    Reset();
  }

  void Reset() {
    entered.assign(names.size(), 0);
    deposited.assign(names.size(), 0);
    edep.assign(names.size(), 0.0);
  }

  void Observe(int parent, int pdg, const std::string& before, int beforeCopy,
               const std::string& after, int afterCopy, bool boundary, double energyMeV) {
    if (!enabled || parent != 0 || std::abs(pdg) != 13) return;
    for (std::size_t i=0; i<names.size(); ++i) {
      bool inside = before == volumes[i] && beforeCopy == copies[i];
      bool entering = boundary && after == volumes[i] && afterCopy == copies[i];
      if (inside || entering) entered[i] = 1;
      if (inside && energyMeV > 0) {
        edep[i] += energyMeV;
        deposited[i] = 1;
      }
    }
  }
};
#endif
