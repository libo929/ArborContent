/// \file MatchTrackWithStartingClusterAlgorithm.cc
/*
 *
 * MatchTrackWithStartingClusterAlgorithm.cc source template automatically generated by a class generator
 * Creation date : jeu. avr. 9 2015
 *
 * This file is part of APRILContent libraries.
 * 
 * APRILContent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 * 
 * APRILContent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with APRILContent.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */

#include "Pandora/AlgorithmHeaders.h"

#include "APRILTopologicalAssociation/MatchTrackWithStartingClusterAlgorithm.h"

#include "APRILHelpers/SortingHelper.h"
#include "APRILHelpers/GeometryHelper.h"
#include "APRILHelpers/ReclusterHelper.h"
#include "APRILHelpers/ClusterHelper.h"
#include "APRILHelpers/HistogramHelper.h"
#include "APRILHelpers/ClusterPropertiesHelper.h"

#include "APRILApi/APRILContentApi.h"

#include "APRILUtility/EventPreparationAlgorithm.h"

#include "APRILTools/TrackDrivenSeedingTool.h"
#include "APRILObjects/CaloHit.h"

#include <algorithm>

namespace april_content
{
  pandora::StatusCode MatchTrackWithStartingClusterAlgorithm::Run()
  {
	std::vector<APRILCluster*> clusterVector;
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->GetEligibleClusters(clusterVector));

	TrackCaloHitVector& trackCaloHits = TrackDrivenSeedingTool::GetTrackAndInitCaloHits();

	pandora::TrackVector trackWithCaloHitsVector;

	for(auto trackCaloHitsIter = trackCaloHits.begin(); trackCaloHitsIter != trackCaloHits.end(); ++trackCaloHitsIter)
	{
		auto pTrack = trackCaloHitsIter->first;
		auto& caloHits = trackCaloHitsIter->second;

		if(caloHits.size() > 0)
		{
			trackWithCaloHitsVector.push_back(pTrack);
		}
	}

    std::sort(trackWithCaloHitsVector.begin(), trackWithCaloHitsVector.end(), SortingHelper::SortTracksByMomentum);

	std::vector<APRILCluster*> trackStartingClusters;

	for(int iTrack = trackWithCaloHitsVector.size() - 1; iTrack >= 0; --iTrack)
	{
		auto pTrack = trackWithCaloHitsVector.at(iTrack);
		pandora::CaloHitVector& caloHits = trackCaloHits.find(pTrack)->second;

		//std::cout << " **************************** track p: " << pTrack->GetMomentumAtDca().GetMagnitude() << std::endl;

		auto mainCluster = GetMainCluster(caloHits);
		if(mainCluster == nullptr) continue;
		//std::cout << "main cluster: " << mainCluster << std::endl;
		
		// make track-cluster association
		PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation(*this, pTrack, mainCluster));

		trackStartingClusters.push_back(mainCluster);

		// ---------- make the record of track-cluster association

		if(m_makeRecord)
		{
			const pandora::MCParticle *pTrackMCParticle = nullptr;
            const pandora::MCParticle *pClusterMCParticle = nullptr;

            try
            {
		        pTrackMCParticle = pandora::MCParticleHelper::GetMainMCParticle( pTrack );
            }
            catch (pandora::StatusCodeException &)
            {
		    	continue;
            }

            try
            {
				const pandora::Cluster* const pCluster = dynamic_cast<const pandora::Cluster* const>(mainCluster);
		        pClusterMCParticle = pandora::MCParticleHelper::GetMainMCParticle( pCluster );
            }
            catch (pandora::StatusCodeException &)
            {
		    	continue;
            }

		    if(pTrackMCParticle == nullptr || pClusterMCParticle == nullptr) continue;

		    ///
	        std::vector<float> vars;
	        vars.push_back( float(EventPreparationAlgorithm::GetEventNumber()) );
	        vars.push_back( pTrack->GetMomentumAtDca().GetMagnitude() );
		    vars.push_back( float(pTrack->IsProjectedToEndCap()) );
		    vars.push_back( float(caloHits.size()) );
		    vars.push_back( float(pTrackMCParticle == pClusterMCParticle) );
	
		    HistogramManager::CreateFill("MatchTrackWithStartingCluster_TrackCheck", "evtNum:trackMomentum:reachEndcap:nCaloHit:isRightMatch", vars);
		}

	}

    return pandora::STATUS_CODE_SUCCESS;
  }

  april_content::APRILCluster* MatchTrackWithStartingClusterAlgorithm::GetMainCluster(const pandora::CaloHitVector& caloHitVector)
  {
      //get the cluster which contains most of calo hits in the vector
      std::map<april_content::APRILCluster*, int> clusterTimes;
      
      for(int iHit = 0; iHit < caloHitVector.size(); ++iHit)
      {
      	const pandora::CaloHit* const pCaloHit = caloHitVector.at(iHit);
      
        const april_content::CaloHit *const pAPRILCaloHit = dynamic_cast<const april_content::CaloHit *const>(pCaloHit);
      	//std::cout << " --- calo hit: " << pAPRILCaloHit << ", cluster: " << pAPRILCaloHit->GetMother() << std::endl;
      
	    auto cluster = APRILContentApi::Modifiable(dynamic_cast<const april_content::APRILCluster*>(pAPRILCaloHit->GetMother()));
      
      	if(cluster != nullptr)
      	{
      	    if(clusterTimes.find(cluster) == clusterTimes.end())
      	    {
      	    	clusterTimes[cluster] = 1;
      	    }
      	    else
      	    {
      	    	clusterTimes[cluster] += 1;
      	    }
      	}
      }
      
	  april_content::APRILCluster* mainCluster = nullptr;
      int mainClusterTimes = 0;
      
      for(auto iter = clusterTimes.begin(); iter != clusterTimes.end(); ++iter)
      {
      	const auto clu = iter->first;
      	int cluTimes = iter->second;
      
      	if(cluTimes > mainClusterTimes)
      	{
      		mainCluster = clu;
      		mainClusterTimes = cluTimes;
      	}
      }
      
      return mainCluster;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode MatchTrackWithStartingClusterAlgorithm::GetEligibleClusters(std::vector<APRILCluster*>& clusterVector) const
  {
	clusterVector.clear();

    const pandora::ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

	//std::cout << ">>>cluster number: " << pClusterList->size() << std::endl;

    if(pClusterList->empty())
      return pandora::STATUS_CODE_SUCCESS;

    for(pandora::ClusterList::const_iterator clusterIter = pClusterList->begin(), clusterEndIter = pClusterList->end() ;
        clusterEndIter != clusterIter ; ++clusterIter)
    {
      const pandora::Cluster *const pCluster = *clusterIter;

      //if(!this->CanMergeCluster(pCluster))
      //  continue;

	  auto aprilCluster = APRILContentApi::Modifiable(dynamic_cast<const april_content::APRILCluster*>(pCluster));
      clusterVector.push_back(aprilCluster);
    }

    // sort them by inner layer
    std::sort(clusterVector.begin(), clusterVector.end(), SortingHelper::SortClustersByInnerLayer);

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode MatchTrackWithStartingClusterAlgorithm::ReadSettings(const pandora::TiXmlHandle xmlHandle)
  {
    m_makeRecord = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "MakeRecord", m_makeRecord));

    return pandora::STATUS_CODE_SUCCESS;
  }


} 

