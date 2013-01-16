////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "CommonTools/Utils/interface/StringCutObjectSelector.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include <memory>
#include <vector>
#include <sstream>


////////////////////////////////////////////////////////////////////////////////
// class definition
////////////////////////////////////////////////////////////////////////////////
class TrackFromPVSelector : public edm::EDProducer
{
public:
  // construction/destruction
  TrackFromPVSelector(const edm::ParameterSet& iConfig);
  virtual ~TrackFromPVSelector();
  
  // member functions
  void produce(edm::Event& iEvent,const edm::EventSetup& iSetup);
  void endJob();

private:  
  // member data
  edm::InputTag     srcPart_ ;  
  edm::InputTag     srcPV_   ;
  double            max_dxy_ ;
  double            max_dz_  ;
};



////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
TrackFromPVSelector::TrackFromPVSelector(const edm::ParameterSet& iConfig)
  : srcPart_(iConfig.getParameter<edm::InputTag>("srcTrack"))
  , srcPV_  (iConfig.getParameter<edm::InputTag>("srcVertex"))
  , max_dxy_(iConfig.getParameter<double>("max_dxy"))
  , max_dz_ (iConfig.getParameter<double>("max_dz"))
{
  produces<std::vector<reco::Track> >();
}


//______________________________________________________________________________
TrackFromPVSelector::~TrackFromPVSelector(){}

////////////////////////////////////////////////////////////////////////////////
// implementation of member functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void TrackFromPVSelector::produce(edm::Event& iEvent,const edm::EventSetup& iSetup)
{  
  std::auto_ptr<std::vector<reco::Track> > goodTracks(new std::vector<reco::Track >);
  
  edm::Handle< std::vector<reco::Vertex> > VertexHandle;
  iEvent.getByLabel(srcPV_,VertexHandle);

  edm::Handle< std::vector<reco::Track> > TrackHandle;
  iEvent.getByLabel(srcPart_,TrackHandle);
  
  if( (VertexHandle->size() == 0) || (TrackHandle->size() == 0) ) 
  {
    iEvent.put(goodTracks);
    return ;
  }
  
  reco::Vertex PV = VertexHandle->front();   
  //typename std::vector<reco::Track>::const_iterator TrackIt ;
  std::vector<reco::Track>::const_iterator TrackIt ;

  for (TrackIt = TrackHandle->begin(); TrackIt != TrackHandle->end(); ++TrackIt) {
    if ( fabs(TrackIt->dxy(PV.position())) < max_dxy_ &&
         fabs(TrackIt->dz(PV.position()))  < max_dz_  ){
      goodTracks -> push_back(*TrackIt) ;
    }
  }  
  
  iEvent.put(goodTracks);
  
}

void TrackFromPVSelector::endJob()
{
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(TrackFromPVSelector);
