// Passive material-boundary observer. Units: world mm, MeV/c, ns.
#ifndef MATERIAL_BOUNDARY_TRUTH_HH
#define MATERIAL_BOUNDARY_TRUTH_HH
#include <vector>
#include <string>
#include <set>
#include <array>
#include <stdexcept>
#include <cmath>
#include <limits>

inline long MaterialEventSeed(long offset, int event) {
  if (offset < 0 || event < 0 || offset > 2147483646L - event)
    throw std::invalid_argument("eventSeedOffset + EventID + 1 must fit positive 32-bit seed");
  return offset + event + 1;
}
inline double MaterialFixedMomentum(double sampled, double fixed) {
  if (!std::isfinite(fixed) || fixed < 0) throw std::invalid_argument("invalid fixedMomentum");
  return fixed > 0 ? fixed : sampled;
}
class MaterialBoundaryTruth {
public:
  bool enabled = false;
  std::vector<int> track, kind, terminalTrack, terminalStatus;
  std::vector<std::string> preVolume, postVolume;
  std::vector<double> x,y,z,prePx,prePy,prePz,postPx,postPy,postPz,time;
  std::vector<double> terminalX,terminalY,terminalZ,terminalPx,terminalPy,terminalPz,terminalTime;
  void Reset() {
    track.clear(); kind.clear(); preVolume.clear(); postVolume.clear();
    x.clear(); y.clear(); z.clear(); prePx.clear(); prePy.clear(); prePz.clear();
    postPx.clear(); postPy.clear(); postPz.clear(); time.clear();
    terminalTrack.clear(); terminalStatus.clear(); terminalX.clear(); terminalY.clear(); terminalZ.clear();
    terminalPx.clear(); terminalPy.clear(); terminalPz.clear(); terminalTime.clear();
  }
  void Observe(int id,int parent,int pdg,bool boundary,bool prePb,bool postPb,
               const std::string& before,const std::string& after,
               const std::array<double,3>& pos,const std::array<double,3>& p0,
               const std::array<double,3>& p1,double t) {
    if (!enabled || parent != 0 || std::abs(pdg)!=13 || !boundary || !(prePb || postPb)) return;
    track.push_back(id); kind.push_back(prePb==postPb ? 0 : (postPb ? 1 : -1));
    preVolume.push_back(before); postVolume.push_back(after);
    x.push_back(pos[0]); y.push_back(pos[1]); z.push_back(pos[2]);
    prePx.push_back(p0[0]); prePy.push_back(p0[1]); prePz.push_back(p0[2]);
    postPx.push_back(p1[0]); postPy.push_back(p1[1]); postPz.push_back(p1[2]); time.push_back(t);
  }
  void Terminal(int id,int parent,int pdg,int status,const std::array<double,3>& pos,
                const std::array<double,3>& p,double t) {
    if (!enabled || parent!=0 || std::abs(pdg)!=13) return;
    terminalTrack.push_back(id); terminalStatus.push_back(status);
    terminalX.push_back(pos[0]); terminalY.push_back(pos[1]); terminalZ.push_back(pos[2]);
    terminalPx.push_back(p[0]); terminalPy.push_back(p[1]); terminalPz.push_back(p[2]); terminalTime.push_back(t);
  }
};
#endif
