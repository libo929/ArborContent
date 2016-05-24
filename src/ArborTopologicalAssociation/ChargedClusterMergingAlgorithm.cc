  /// \file ChargedClusterMergingAlgorithm.cc
/*
 *
 * ChargedClusterMergingAlgorithm.cc source template automatically generated by a class generator
 * Creation date : jeu. mai 19 2016
 *
 * This file is part of ArborContent libraries.
 * 
 * ArborContent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 * 
 * ArborContent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with ArborContent.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */

#include "Pandora/AlgorithmHeaders.h"

#include "ArborTopologicalAssociation/ChargedClusterMergingAlgorithm.h"

#include "ArborApi/ArborContentApi.h"
#include "ArborHelpers/ReclusterHelper.h"
#include "ArborHelpers/ClusterHelper.h"
#include "ArborHelpers/SortingHelper.h"
#include "ArborHelpers/GeometryHelper.h"

namespace arbor_content
{

pandora::StatusCode ChargedClusterMergingAlgorithm::Run()
{
	// get current cluster list
	const pandora::ClusterList *pClusterList = NULL;
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

	if(pClusterList->empty())
		return pandora::STATUS_CODE_SUCCESS;

	pandora::ClusterVector clusterVector(pClusterList->begin(), pClusterList->end());
	std::sort(clusterVector.begin(), clusterVector.end(), SortingHelper::SortClustersByInnerLayer);

	const unsigned int nClusters(clusterVector.size());

	for(unsigned int i=0 ; i<nClusters ; ++i)
	{
		const pandora::Cluster *const pParentCluster = clusterVector[i];

		if( NULL == pParentCluster )
			continue;

		const pandora::TrackList trackList(pParentCluster->GetAssociatedTrackList());
		unsigned int nTrackAssociations(trackList.size());

		if((nTrackAssociations < m_minTrackAssociations) || (nTrackAssociations > m_maxTrackAssociations))
			continue;

		const float chi = ReclusterHelper::GetTrackClusterCompatibility(this->GetPandora(), pParentCluster, trackList);

		// check for missing energy in charged cluster
		if((chi*chi < m_minChi2ToRunReclustering) && (chi < 0.f))
			continue;

	    float trackEnergySum(0.);

	    for (pandora::TrackList::const_iterator trackIter = trackList.begin(), trackIterEnd = trackList.end(); trackIter != trackIterEnd; ++trackIter)
	        trackEnergySum += (*trackIter)->GetEnergyAtDca();

		pandora::ClusterFitResult parentFitResult;
		pandora::CartesianVector parentCentroid(0.f, 0.f, 0.f);

		if(pandora::STATUS_CODE_SUCCESS != ClusterHelper::GetCentroid(pParentCluster, parentCentroid))
			continue;

		const unsigned int parentCentroidPseudoLayer(PandoraContentApi::GetPlugins(*this)->GetPseudoLayerPlugin()->GetPseudoLayer(parentCentroid));
		const unsigned int maxOccupiedLayers(parentCentroidPseudoLayer-pParentCluster->GetInnerPseudoLayer());

		if(pandora::STATUS_CODE_SUCCESS != pandora::ClusterFitHelper::FitStart(pParentCluster, maxOccupiedLayers, parentFitResult))
			continue;

		if(!parentFitResult.IsFitSuccessful())
			continue;

		pandora::ClusterVector daughterClusterVector;

		for(unsigned int j=0 ; j<nClusters ; ++j)
		{
			const pandora::Cluster *const pDaughterCluster = clusterVector[j];

			if( NULL == pDaughterCluster )
				continue;

			if( pParentCluster == pDaughterCluster )
				continue;

			if( ! pDaughterCluster->GetAssociatedTrackList().empty() )
				continue;

			if( parentCentroidPseudoLayer > pDaughterCluster->GetInnerPseudoLayer() )
				continue;

			if( pDaughterCluster->GetInnerPseudoLayer() - parentCentroidPseudoLayer > m_maxPseudoLayerAssociation )
				continue;

			pandora::CartesianVector daughterCentroid(0.f, 0.f, 0.f);

			if(pandora::STATUS_CODE_SUCCESS != ClusterHelper::GetCentroid(pDaughterCluster, daughterCentroid))
				continue;

			const float angle( parentFitResult.GetDirection().GetOpeningAngle( daughterCentroid - parentCentroid ) );

			if( angle > m_maxAngleAssociation )
				continue;
				
			daughterClusterVector.push_back(pDaughterCluster);
		}

		if(daughterClusterVector.empty())
			continue;

		// sort by corrected hadronic energy
		std::sort(daughterClusterVector.begin(), daughterClusterVector.end(), *this);

		const unsigned int nDaughterClusters(daughterClusterVector.size());

		for(unsigned int j=0 ; j<nDaughterClusters ; ++j)
		{
			const pandora::Cluster *const pDaughterCluster = daughterClusterVector[j];

			const float currentParentClusterEnergy(pParentCluster->GetCorrectedHadronicEnergy(this->GetPandora()));
			const float daughterClusterEnergy(pDaughterCluster->GetCorrectedHadronicEnergy(this->GetPandora()));

			const float clusterEnergySum(currentParentClusterEnergy + daughterClusterEnergy);

			float oldChi = ReclusterHelper::GetTrackClusterCompatibility(this->GetPandora(), currentParentClusterEnergy, trackEnergySum);
			float newChi = ReclusterHelper::GetTrackClusterCompatibility(this->GetPandora(), clusterEnergySum, trackEnergySum);

			if(oldChi*oldChi < newChi*newChi )
				break;

			pandora::ClusterVector::iterator iter = std::find(clusterVector.begin(), clusterVector.end(), pDaughterCluster);

			if(iter != clusterVector.end())
				(*iter) = NULL;

			PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pParentCluster, pDaughterCluster));
		}
	}

	return pandora::STATUS_CODE_SUCCESS;
}

bool ChargedClusterMergingAlgorithm::operator ()(const pandora::Cluster *const pLhs, const pandora::Cluster *const pRhs)
{
	return pLhs->GetCorrectedHadronicEnergy(this->GetPandora()) < pRhs->GetCorrectedHadronicEnergy(this->GetPandora());
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode ChargedClusterMergingAlgorithm::ReadSettings(const pandora::TiXmlHandle xmlHandle)
{
	m_minTrackAssociations = 1;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
			"MinTrackAssociations", m_minTrackAssociations));

	m_maxTrackAssociations = std::numeric_limits<unsigned int>::max();
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
			"MaxTrackAssociations", m_maxTrackAssociations));

	m_minChi2ToRunReclustering = 2.5;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
			"MinChi2ToRunReclustering", m_minChi2ToRunReclustering));

	m_maxAngleAssociation = 0.5f;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
			"MaxAngleAssociation", m_maxAngleAssociation));

	m_maxClusterDca = 30.f;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
			"MaxClusterDca", m_maxClusterDca));

	m_maxPseudoLayerAssociation = std::numeric_limits<unsigned int>::max();
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
			"MaxPseudoLayerAssociation", m_maxPseudoLayerAssociation));

	return pandora::STATUS_CODE_SUCCESS;
}

} 

