#include "UserCode/PVStudy/interface/TrackHigherPt.h"

using namespace reco;


bool 
TrackHigherPt::operator() ( const Track & t1, 
				    const Track & t2) const
{
  return (t1.pt() > t2.pt());
}
