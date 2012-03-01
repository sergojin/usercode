#ifndef TrackSorter_H
#define TrackSorter_H

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include <vector>



/** \class TrackSorter
 * class to sort TrackCollection
 * in decreasing order of the track pT
 */
struct TrackSorter {

  std::vector<reco::Track> sortedList(const reco::TrackCollection & tracks) const;
 
};



#endif
