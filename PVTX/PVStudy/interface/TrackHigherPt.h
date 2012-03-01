#ifndef TrackHigherPt_H
#define TrackHigherPt_H

#include "DataFormats/TrackReco/interface/Track.h"

#include <vector>

/** \class TrackHigherPt
 * operator for sorting Track
 * in decreasing order of the Pt
 */
struct TrackHigherPt {

  bool operator() ( const reco::Track & v1, const reco::Track & v2) const;

};



#endif
