/// \file ClusterFromTrackMergingAlgorithm5.cc
/*
 *
 * ClusterFromTrackMergingAlgorithm5.cc source template automatically generated by a class generator
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
#include "PandoraMonitoringApi.h"
#include "PandoraMonitoring.h"

#include "APRILTopologicalAssociation/ClusterFromTrackMergingAlgorithm5.h"

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

#define __DEBUG__ 0
#define __USEMCP__ 1

namespace april_content
{
  pandora::StatusCode ClusterFromTrackMergingAlgorithm5::Run()
  {
    // get candidate clusters for association
	std::vector<APRILCluster*> clusterVector;
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->GetEligibleClusters(clusterVector));

	// get cluster properties
	for(int i = 0; i < clusterVector.size(); ++i)
	{
		auto pCluster = clusterVector.at(i);
		
		pCluster->Reset();

		pandora::CartesianVector centroid(0., 0., 0);
		ClusterHelper::GetCentroid(pCluster, centroid);
		pCluster->SetCentroid(centroid);
		
		const pandora::Cluster* const pandoraClu = dynamic_cast<const pandora::Cluster* const>(pCluster);
		bool isPhoton = PandoraContentApi::GetPlugins(*this)->GetParticleId()->IsPhoton(pandoraClu);

        if(m_useMCForPhotonID)
		{
			try
		    {
		    	isPhoton = pandora::MCParticleHelper::GetMainMCParticle(pandoraClu)->GetParticleId() == 22;
		    }
		    catch(pandora::StatusCodeException &)
		    {
		    	std::cout << "MCP issue: " << pandoraClu << std::endl;
		    }
		}

		pCluster->SetPhoton(isPhoton);

#if __DEBUG__
		std::cout << " --- cluster : " << pCluster << ", energy: " << pCluster->GetHadronicEnergy() 
			      << ", COG: " << centroid.GetX() << ", " << centroid.GetY() << ", " << centroid.GetZ() << ", isPoton: " << isPhoton << std::endl;
#endif

		try
		{
		    pandora::ClusterFitResult clusterFitResult;
		    pandora::ClusterFitHelper::FitFullCluster(pCluster, clusterFitResult);
		    const pandora::CartesianVector& cluDirection = clusterFitResult.GetDirection();
		    const pandora::CartesianVector& cluIntercept = clusterFitResult.GetIntercept();
		    //std::cout << "  *** direction_f: " << cluDirection.GetX() << ", " << cluDirection.GetY() << ", " << cluDirection.GetZ() << std::endl;
		    //std::cout << "  *** intercept_f: " << cluIntercept.GetX() << ", " << cluIntercept.GetY() << ", " << cluIntercept.GetZ() << std::endl;
			
			pCluster->SetAxis(cluDirection);
			pCluster->SetIntercept(cluIntercept);
		}
		catch(pandora::StatusCodeException &)
		{
			//std::cout << "Fit failed, cluster: " << pCluster << ", E: " << pCluster->GetHadronicEnergy() << std::endl;
			//continue;
		}

		try
		{
		    pandora::ClusterFitResult clusterFitResult;
			pandora::ClusterFitHelper::FitStart(pCluster, 3, clusterFitResult);
		    const pandora::CartesianVector& startingPoint = clusterFitResult.GetIntercept();

		    //std::cout << "  *** startingPoint: " << startingPoint.GetX() << ", " << startingPoint.GetY() << ", " << startingPoint.GetZ() << std::endl;

			pCluster->SetStartingPoint(startingPoint);
		}
		catch(pandora::StatusCodeException &)
		{
			//std::cout << "Fit failed, cluster: " << pCluster << ", E: " << pCluster->GetHadronicEnergy() << std::endl;
			//continue;
		}

		try
		{
		    pandora::ClusterFitResult clusterFitResult;
			pandora::ClusterFitHelper::FitEnd(pCluster, 3, clusterFitResult);
		    const pandora::CartesianVector& endpoint = clusterFitResult.GetIntercept();

		    //std::cout << "  *** endpoint: " << endpoint.GetX() << ", " << endpoint.GetY() << ", " << endpoint.GetZ() << std::endl;

			pCluster->SetEndpoint(endpoint);
		}
		catch(pandora::StatusCodeException &)
		{
			//std::cout << "Fit failed, cluster: " << pCluster << ", E: " << pCluster->GetHadronicEnergy() << std::endl;
			continue;
		}
	}

	m_clustersToMerge.clear();

	pandora::ClusterVector photonCandidates;
	
	for(int i = 0; i < clusterVector.size(); ++i)
	{
		auto cluster = clusterVector.at(i);

		if( (cluster->GetAssociatedTrackList().size() == 0) && 
			(cluster->IsPhoton() == false) )
		{
			m_clustersToMerge.push_back(cluster);
		}

		if(cluster->IsPhoton())
		{
			cluster->SetRoot();
			photonCandidates.push_back(cluster);
		}
	}

	std::sort(photonCandidates.begin(), photonCandidates.end(), pandora_monitoring::PandoraMonitoring::SortClustersByHadronicEnergy);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<pandora::CartesianVector> m_clusterCentroids;

	for(auto clu : m_clustersToMerge)
	{
		auto& centroid = clu->GetCentroid();
		m_clusterCentroids.push_back(centroid);
	}

	CaloHitRangeSearchHelper::FillMatixByPoints(m_clusterCentroids, m_clusterCentroidsMatrix);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// search nearby clusters along track
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const pandora::TrackList *pTrackList = nullptr;
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pTrackList));

	// set the cluster with track as root cluster
	for(auto track : *pTrackList)
	{
		if( !(track->HasAssociatedCluster()) ) continue;

		auto clu = track->GetAssociatedCluster();
		auto associatedCluster = APRILContentApi::Modifiable(dynamic_cast<const april_content::APRILCluster*>(clu));
		associatedCluster->SetRoot();
	}

    if(m_mergeChargedClusters)
	{
		for(auto track : *pTrackList)
	    {
	    	// Reset the cluster for search
	        for(int i = 0; i < clusterVector.size(); ++i)
	    	{
	    		auto clu = clusterVector.at(i);
	    		clu->SetHasMotherAtSearch(false);
	    	}

	    	if( !(track->HasAssociatedCluster()) ) continue;

	    	auto clu = track->GetAssociatedCluster();
	    	auto associatedCluster = APRILContentApi::Modifiable(dynamic_cast<const april_content::APRILCluster*>(clu));

#if     __DEBUG__
	    	std::cout << "     ---> SearchProperClusters from starting cluster: " << clu 
	    		<< ", track E: " << track->GetEnergyAtDca() << std::endl;
#endif

	    	std::vector<APRILCluster*> properClusters;
	    	SearchProperClusters(track, associatedCluster, properClusters);
	    }
	}

	// clean clusters
	CleanClusterForMerging(clusterVector);
	
    return pandora::STATUS_CODE_SUCCESS;
  }

  void ClusterFromTrackMergingAlgorithm5::SearchProperClusters(const pandora::Track* pTrack, APRILCluster* startingCluster, 
		  std::vector<april_content::APRILCluster*>& properClusters)
  {
	  
#if __DEBUG__
	  const pandora::Cluster* const pandoraTrackStartClu = dynamic_cast<const pandora::Cluster* const>(startingCluster);
	  float startCluEnergy = startingCluster->GetHadronicEnergy();

	  auto pClusterMCParticle = pandora::MCParticleHelper::GetMainMCParticle(pandoraTrackStartClu);
	  std::cout << " SearchProperClusters for charged cluster: " << startingCluster << ", Ehad: " << startCluEnergy << ", MCP: " << pClusterMCParticle << std::endl;
#endif

	  std::vector<april_content::APRILCluster*> nearbyClusters;
	  GetNearbyClusters(startingCluster, m_clustersToMerge, nearbyClusters);

	  // map for sorting all nearby clusters by closest distance
	  std::multimap<float, APRILCluster*> clusterDistanceMap;

	  for(int i = 0; i < nearbyClusters.size(); ++i)
	  {
		  auto nearbyCluster = nearbyClusters.at(i);

		  if(nearbyCluster->HasMotherAtSearch() || nearbyCluster == startingCluster || nearbyCluster->IsRoot() || nearbyCluster->IsPhoton()) 
		  {
			  continue;
		  }

#if 0
		  // angle selection
		  pandora::CartesianVector trackPointAtCalo = pTrack->GetTrackStateAtCalorimeter().GetPosition();
		  pandora::CartesianVector trackMomentumAtCalo = pTrack->GetTrackStateAtCalorimeter().GetMomentum();

		  pandora::CartesianVector trackPointAtCaloClusterDistance = nearbyCluster->GetCentroid() - trackPointAtCalo;

		  float clusterTrackAngle = trackPointAtCaloClusterDistance.GetOpeningAngle(trackMomentumAtCalo);
		  float m_maxClusterTrackAngle = 0.3;
		  if(clusterTrackAngle > m_maxClusterTrackAngle || clusterTrackAngle < 0. || isnan(clusterTrackAngle)) continue;
#endif

#if __DEBUG__
		  std::cout << "nearbyClusters " << i << " : " << nearbyCluster << ", E: " << nearbyCluster->GetHadronicEnergy() << std::endl;
#endif


		  // GetClustersDistance
		  float closestDistance = 1.e6;

		  try
		  {
			  // false: use all hits for getting the distance (not only the connected hits)
			  ClusterHelper::GetClosestDistanceApproach(startingCluster, nearbyCluster, closestDistance, false);
		  }
          catch(pandora::StatusCodeException &)
		  {
			  std::cout << "GetClosestDistanceApproach failed" << std::endl;
		  }

		  float emEnergyInECAL = ClusterHelper::GetElectromagneticEnergyInECAL(nearbyCluster);
		  float emEnergyRatio  = emEnergyInECAL / nearbyCluster->GetElectromagneticEnergy();

		  float m_maxClosestDistance = 1.e6;
		  float meanDensity = 1.;

		  // FIXME
		  if(emEnergyRatio > 0.6) 
		  {
		      // mainly in ECAL 
		      m_maxClosestDistance = 50.;

			  ClusterHelper::GetMeanDensity(nearbyCluster, meanDensity);

			  // seems a hadronic fragment
			  if(meanDensity<0.3) m_maxClosestDistance = 100;
		  }
		  else
		  {
		      m_maxClosestDistance = 200.;
		  }
		      
		  // help by MC truth
		  if(m_useMCForChargeID)
		  {
			  try
		      {
		          const pandora::Cluster* const pandoraClu = dynamic_cast<const pandora::Cluster* const>(nearbyCluster);
		          auto pandoraCluMCP = pandora::MCParticleHelper::GetMainMCParticle(pandoraClu);

		          if( pandora::PdgTable::GetParticleCharge(pandoraCluMCP->GetParticleId()) == 0.)
		          {
		        	  continue;
		          }

		          if(nearbyCluster->GetHadronicEnergy() < 0.2 )
		          {
		        	  continue;
		          }
		      }
		      catch(pandora::StatusCodeException &)
		      {
		      }
		  }

		  //GetClustersDirection
		  auto& startingClusterAxis = startingCluster->GetAxis();

		  //startingCluster->GetAxis();
		  float angle = 1.e6;

		  auto& nearbyClusterCOG = nearbyCluster->GetCentroid();
		  auto& startingClusterCOG = startingCluster->GetCentroid();
		  auto directionOfCentroids = nearbyClusterCOG - startingClusterCOG;

		  if( directionOfCentroids.GetMagnitudeSquared() * startingClusterAxis.GetMagnitudeSquared() > std::numeric_limits<float>::epsilon() )
		  {
			  try
			  {
				  angle = directionOfCentroids.GetOpeningAngle(startingClusterAxis);
			  }
			  catch(pandora::StatusCodeException &)
			  {
				  std::cout << "GetOpeningAngle failed" << std::endl;
			  }
		  }

		  const float bField(PandoraContentApi::GetPlugins(*this)->GetBFieldPlugin()->GetBField( pandora::CartesianVector(0.f, 0.f, 0.f)));

		  const pandora::Helix helix(pTrack->GetTrackStateAtCalorimeter().GetPosition(),
		    	  pTrack->GetTrackStateAtCalorimeter().GetMomentum(), pTrack->GetCharge(), bField);
		
		  pandora::CartesianVector trackCluCentroidDistanceVec(0., 0., 0.);
		  float genericTime = 0.;

		  if(pandora::STATUS_CODE_SUCCESS != helix.GetDistanceToPoint(nearbyClusterCOG, trackCluCentroidDistanceVec, genericTime))
		  {
			std::cout << "helix.GetDistanceToPoint failed" << std::endl;
		  	continue;
		  }

#if __DEBUG__
		  float trackCluCentroidDistance = trackCluCentroidDistanceVec.GetMagnitude();
	      //const pandora::Cluster* const pandoraNearbyClu = dynamic_cast<const pandora::Cluster* const>(nearbyCluster);
	      //auto nearbyClusterMCParticle = pandora::MCParticleHelper::GetMainMCParticle(pandoraNearbyClu);
		  float nearbyCluEnergy = nearbyCluster->GetHadronicEnergy();

		  std::cout << " --- clu: " << nearbyCluster << ", E: " << nearbyCluEnergy
		   			<< ", trackCluCentroidDistance: " << trackCluCentroidDistance << ", angle: " << angle << std::endl;
#endif

#if 0
		  bool isGoodAngle = (angle < 0.3) ;
		  auto pseudoLayerPlugin = PandoraContentApi::GetPlugins(*this)->GetPseudoLayerPlugin();
		  const unsigned int startingLayer1(pseudoLayerPlugin->GetPseudoLayer(startingCluster->GetStartingPoint()));
		  const unsigned int startingLayer2(pseudoLayerPlugin->GetPseudoLayer(nearbyCluster->GetStartingPoint()));

		  if(isGoodAngle) 
		  if( (startingLayer1 < startingLayer2) && (trackCluCentroidDistance < 10. || isGoodAngle) ) 
#endif

		  auto& nearbyClusterAxis = nearbyCluster->GetAxis();
		  auto directionsCrossProd = nearbyClusterAxis.GetCrossProduct(startingClusterAxis);
		  float axisDistance = fabs(directionsCrossProd.GetDotProduct(directionOfCentroids)) / directionsCrossProd.GetMagnitude();

		  bool canMerge = (closestDistance < m_maxClosestDistance) || (angle < 1. && closestDistance < m_maxClosestDistance * 3.) ||
			              (axisDistance < 100. && closestDistance < m_maxClosestDistance * 4.) ;
				  

		  if(!canMerge) 
		  {
#if __DEBUG__
			  std::cout << "emEnergyRatio: " << emEnergyRatio << ", m_maxClosestDistance: " << m_maxClosestDistance 
				  << ", meanDensity: " << meanDensity << ", closestDistance: " << closestDistance << std::endl;
#endif

			  continue;
		  }

		  std::vector<float> clusterParameters;

		  clusterParameters.push_back(closestDistance);
		  clusterParameters.push_back(angle); // axis angle
		  clusterParameters.push_back(axisDistance);

		  std::vector<float> parameterPowers;
		  parameterPowers.push_back(5.);
		  parameterPowers.push_back(3.);
		  parameterPowers.push_back(1.);

		  ClustersOrderParameter orderParameter(clusterParameters, parameterPowers);
		  nearbyCluster->SetOrderParameterWithMother(startingCluster, orderParameter);

		  clusterDistanceMap.insert( std::pair<float, APRILCluster*>(closestDistance, nearbyCluster) );
      }
		  
	  for(auto it = clusterDistanceMap.begin(); it != clusterDistanceMap.end(); ++it)
	  {
		  auto nearbyCluster = it->second;

		  properClusters.push_back(nearbyCluster);
		  nearbyCluster->SetHasMotherAtSearch();
	  }

	  startingCluster->SetClustersToMerge(properClusters);
		
#if 0
	  // search proper cluster's proper cluster
	  for(int iClu = 0; iClu < properClusters.size(); ++iClu)
	  {
		  auto clu = properClusters.at(iClu);
		  
		  std::vector<APRILCluster*> clusters;
		  SearchProperClusters(pTrack, clu, clusters);
	  }
#endif
		  
#if __DEBUG__
	  std::cout << "-----------------------------------------------------------------------------------------------------------" << std::endl;
#endif
  }

  void ClusterFromTrackMergingAlgorithm5::GetNearbyClusters(pandora::Cluster* cluster, 
		  const std::vector<april_content::APRILCluster*>& clusterVector, std::vector<april_content::APRILCluster*>& clustersInRange)
  {
      pandora::CartesianVector centroid(0., 0., 0.);
	  ClusterHelper::GetCentroid(cluster, centroid);

	  const float distance = m_maxStartingClusterDistance;
	  const mlpack::math::Range range(0., distance);

      arma::mat testPoint(3, 1);
	  testPoint.col(0)[0] = centroid.GetX();
	  testPoint.col(0)[1] = centroid.GetY();
	  testPoint.col(0)[2] = centroid.GetZ();
	  
	  mlpack::range::RangeSearch<> rangeSearch(m_clusterCentroidsMatrix);
      std::vector<std::vector<size_t> > resultingNeighbors;
      std::vector<std::vector<double> > resultingDistances;
      rangeSearch.Search(testPoint, range, resultingNeighbors, resultingDistances);

      std::vector<size_t>& neighbors = resultingNeighbors.at(0);
      std::vector<double>& distances = resultingDistances.at(0);
	  
      for(size_t j=0; j < neighbors.size(); ++j)
      {
      	size_t neighbor = neighbors.at(j);
      	//double hitsDist = distances.at(j);

		clustersInRange.push_back( clusterVector.at(neighbor) );
	  }

	  // sort cluster by distance
	  std::multimap<float, APRILCluster*> clusterDistanceMap;

	  for(int i = 0; i < clustersInRange.size(); ++i)
	  {
		  auto clusterInRange = clustersInRange.at(i);

		  clusterDistanceMap.insert( std::pair<float, APRILCluster*>(distances.at(i), clusterInRange) );
	  }

	  clustersInRange.clear();

	  for(auto& mapIter : clusterDistanceMap)
	  {
		  auto clu = mapIter.second;
		  clustersInRange.push_back(clu);
	  }

	  ///////////////////////////////////////////////////////////////////////////////////////////////
		
#if 0
	  auto pClusterMCP = pandora::MCParticleHelper::GetMainMCParticle(cluster);

	  std::cout << "------------ cluster: " << cluster << ", energy: " << cluster->GetHadronicEnergy() 
		  << ", MCP: " << pClusterMCP << ", nearby clusters: " << clustersInRange.size() << std::endl;

	  for(auto it = clusterDistanceMap.begin(); it != clusterDistanceMap.end(); ++it)
	  {
		  //auto pCluster = clustersInRange.at(i);
		  auto distance = it->first;
		  auto pCluster = it->second;


		  const pandora::Cluster* const clu = dynamic_cast<const pandora::Cluster* const>(pCluster);
		  bool isPhoton = PandoraContentApi::GetPlugins(*this)->GetParticleId()->IsPhoton(clu);
		  auto pClusterMCParticle = pandora::MCParticleHelper::GetMainMCParticle(clu);

		  std::cout << " *clu: " << clu << ", dist: " << distance << ", nhits: " << 
			 pCluster->GetNCaloHits() << ", Ehad: " << pCluster->GetHadronicEnergy() 
			 << ", iL: " << pCluster->GetInnerPseudoLayer() << ": isPhoton: " << pCluster->IsPhoton() 
			 << ": MCP: " << pClusterMCParticle << std::endl;
	  }
#endif
  }

  pandora::StatusCode ClusterFromTrackMergingAlgorithm5::CleanClusterForMerging(std::vector<APRILCluster*>& clusterVector)
  {
	for(int i = 0; i < clusterVector.size(); ++i)
	{
		auto& cluster = clusterVector.at(i);

		auto& mothers = cluster->GetMotherCluster();

#if __DEBUG__
		std::cout << " --- cluster " << cluster << " mothers: " << mothers.size() << ", root?: " << cluster->IsRoot() << std::endl;
#endif

		// find the best one
		ClustersOrderParameter bestOrderParameter;
		APRILCluster* bestCluster;

		for(int iMother = 0; iMother < mothers.size(); ++iMother)
		{
			auto mother = mothers.at(iMother);
			ClustersOrderParameter orderParameter = cluster->GetOrderParameterWithMother(mother);

			if(orderParameter < bestOrderParameter)
			{
				bestOrderParameter = orderParameter;
				bestCluster = mother;
			}
		}

		// take the best one
		if(mothers.size() > 1)
		{
			for(int iMother = 0; iMother < mothers.size(); ++iMother)
			{
				auto mother = mothers.at(iMother);

				if(mother != bestCluster)
				{
#if __DEBUG__
				    std::cout << " !!! cluster: " << mother << " remove cluster to merge: " << cluster << std::endl;
#endif
					mother->RemoveFromClustersToMerge(cluster);
				}
			}

			mothers.clear();
			mothers.push_back(bestCluster);
		}
	}

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode ClusterFromTrackMergingAlgorithm5::GetEligibleClusters(std::vector<APRILCluster*>& clusterVector) const
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

	  auto aprilCluster = APRILContentApi::Modifiable(dynamic_cast<const april_content::APRILCluster*>(pCluster));
      clusterVector.push_back(aprilCluster);
    }

    // sort them by inner layer
    std::sort(clusterVector.begin(), clusterVector.end(), SortingHelper::SortClustersByInnerLayer);

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode ClusterFromTrackMergingAlgorithm5::ReadSettings(const pandora::TiXmlHandle xmlHandle)
  {
    m_maxStartingClusterDistance = 2000.;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "MaxStartingClusterDistance", m_maxStartingClusterDistance));

	m_maxClosestPhotonDistance = 80.;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "MaxClosestPhotonDistance", m_maxClosestPhotonDistance));

	m_mergeChargedClusters = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "MergeChargedClusters", m_mergeChargedClusters));

	m_useMCForPhotonID = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "UseMCForPhotonID", m_useMCForPhotonID));

	m_useMCForChargeID = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "UseMCForChargeID", m_useMCForChargeID));

    return pandora::STATUS_CODE_SUCCESS;
  }

} 
