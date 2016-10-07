  /// \file EnergyDrivenTrackClusterAssociationAlgorithm.cc
/*
 *
 * EnergyDrivenTrackClusterAssociationAlgorithm.cc source template automatically generated by a class generator
 * Creation date : jeu. avr. 9 2015
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

#include "ArborTrackClusterAssociation/EnergyDrivenTrackClusterAssociationAlgorithm.h"

#include "ArborTools/EnergyEstimateTools.h"
#include "ArborHelpers/SortingHelper.h"

namespace arbor_content
{

pandora::StatusCode EnergyDrivenTrackClusterAssociationAlgorithm::Run()
{
	const pandora::TrackList *pTrackList = NULL;
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pTrackList));

	if(pTrackList->empty())
		return pandora::STATUS_CODE_SUCCESS;

	const pandora::ClusterList *pClusterList = NULL;
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

	if(pClusterList->empty())
		return pandora::STATUS_CODE_SUCCESS;

	// reset the track-cluster associations
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::RemoveCurrentTrackClusterAssociations(*this));

	pandora::ClusterVector clusterVector(pClusterList->begin(), pClusterList->end());
	std::sort(clusterVector.begin(), clusterVector.end(), SortingHelper::SortClustersByInnerLayer);

	for(pandora::TrackList::const_iterator trackIter = pTrackList->begin() , trackEndIter = pTrackList->end() ;
			trackEndIter != trackIter ; ++trackIter)
	{
		const pandora::Track *const pTrack = *trackIter;

		pandora::ClusterVector clusterAssociationVector;
		PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->FindClustersForAssociation(pTrack, clusterVector, clusterAssociationVector));
		PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->PerformTrackClusterAssociation(pTrack, clusterAssociationVector, clusterVector));
	}

    return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode EnergyDrivenTrackClusterAssociationAlgorithm::FindClustersForAssociation(const pandora::Track *const pTrack, const pandora::ClusterVector &clusterVector,
		pandora::ClusterVector &clusterAssociationVector) const
{
	const pandora::CartesianVector &trackProjectionPosition(pTrack->GetTrackStateAtCalorimeter().GetPosition());
	pandora::ClusterVector firstInteractingClusterVector;
	pandora::ClusterVector closebyClusterVector;

	for(pandora::ClusterVector::const_iterator iter = clusterVector.begin() , endIter = clusterVector.end() ;
			endIter != iter ; ++iter)
	{
		const pandora::Cluster *pCluster = *iter;

		if(!pCluster->GetAssociatedTrackList().empty())
			continue;

		unsigned int innerPseudoLayer = pCluster->GetInnerPseudoLayer();
		const pandora::CartesianVector innerClusterPosition = pCluster->GetCentroid(innerPseudoLayer);

		const float trackClusterDistance((trackProjectionPosition - innerClusterPosition).GetMagnitude());

		// since clusters are sorted by inner layer no need to continue
		if(innerPseudoLayer <= m_trackClusterNLayersCut)
			break;

		if(trackClusterDistance < m_trackClusterDistanceCut1)
			firstInteractingClusterVector.push_back(pCluster);

		const pandora::CartesianVector differenceVector(innerClusterPosition - trackProjectionPosition);
		const float projectionDistance(trackClusterDistance * std::sin(differenceVector.GetOpeningAngle(differenceVector)));

		if(trackClusterDistance < m_trackClusterDistanceCut2 && projectionDistance < m_trackClusterProjectionCut)
			closebyClusterVector.push_back(pCluster);
	}

	if(firstInteractingClusterVector.size() >= m_firstInteractingLayerNSeedCut)
		clusterAssociationVector = firstInteractingClusterVector;
	else
		clusterAssociationVector = closebyClusterVector;

    return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode EnergyDrivenTrackClusterAssociationAlgorithm::RemoveClustersFromList(pandora::ClusterVector &inputClusterVector,
		const pandora::ClusterVector &clusterToRemoveVector) const
{
	for(pandora::ClusterVector::const_iterator clusterIterI = clusterToRemoveVector.begin(), clusterEndIterI = clusterToRemoveVector.end() ;
			clusterEndIterI != clusterIterI ; ++clusterIterI)
	{
		for(pandora::ClusterVector::iterator clusterIterJ = inputClusterVector.begin(), clusterEndIterJ = inputClusterVector.end() ;
				clusterEndIterJ != clusterIterJ ; ++clusterIterJ)
		{
			if(*clusterIterI == *clusterIterJ)
			{
				inputClusterVector.erase(clusterIterJ);
				break;
			}
		}
	}

    return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode EnergyDrivenTrackClusterAssociationAlgorithm::PerformTrackClusterAssociation(const pandora::Track *const pTrack,
		const pandora::ClusterVector &clusterAssociationVector, pandora::ClusterVector &inputClusterVector) const
{
	float currentChi2 = std::numeric_limits<float>::max();
	float totalClusterEnergy = 0.f;

	pandora::ClusterVector finalClusterAssociationVector;
	pandora::CaloHitList combinedClustersCaloHitList;

	for(pandora::ClusterVector::const_iterator iter = clusterAssociationVector.begin() , endIter = clusterAssociationVector.end() ;
			endIter != iter ; ++iter)
	{
		const pandora::Cluster *const pCluster = *iter;

		pandora::CaloHitList clusterCaloHitList;
		pCluster->GetOrderedCaloHitList().GetCaloHitList(clusterCaloHitList);
		combinedClustersCaloHitList.insert(clusterCaloHitList.begin(), clusterCaloHitList.end());

		PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, m_pEnergyEstimateTool->ComputeEnergy(combinedClustersCaloHitList, totalClusterEnergy));
		const float chi = m_pEnergyEstimateTool->GetTrackClusterCompatibility(totalClusterEnergy, pTrack->GetEnergyAtDca(), m_chi2SigmaFactor);

		if(chi * chi < currentChi2)
		{
			currentChi2 = chi * chi;
			finalClusterAssociationVector.push_back(pCluster);
		}
	}

	if(finalClusterAssociationVector.empty())
	    return pandora::STATUS_CODE_SUCCESS;

	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->RemoveClustersFromList(inputClusterVector, finalClusterAssociationVector));

	if(1 == finalClusterAssociationVector.size())
	{
		PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation(*this, pTrack, *finalClusterAssociationVector.begin()));
	}
	else
	{
		const pandora::Cluster *const pClusterToEnlarge = *finalClusterAssociationVector.begin();

		for(pandora::ClusterVector::const_iterator iter = finalClusterAssociationVector.begin() , endIter = finalClusterAssociationVector.end() ;
				endIter != iter ; ++iter)
		{
			if((*iter) == pClusterToEnlarge)
				continue;

			PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pClusterToEnlarge, *iter));
		}

		PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation(*this, pTrack, pClusterToEnlarge));
	}

    return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode EnergyDrivenTrackClusterAssociationAlgorithm::ReadSettings(const pandora::TiXmlHandle xmlHandle)
{
	pandora::AlgorithmTool *pAlgorithmTool = NULL;
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ProcessAlgorithmTool(*this, xmlHandle,
			"EnergyEstimate", pAlgorithmTool));

	m_pEnergyEstimateTool = dynamic_cast<EnergyEstimateToolBase *>(pAlgorithmTool);

	if(NULL == m_pEnergyEstimateTool)
		return pandora::STATUS_CODE_INVALID_PARAMETER;

	m_chi2SigmaFactor = 2.f;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
			"Chi2SigmaFactor", m_chi2SigmaFactor));

	m_trackClusterChi2Cut = 1.f;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
			"TrackClusterChi2Cut", m_trackClusterChi2Cut));

	m_trackClusterNLayersCut = 2;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
			"TrackClusterNLayersCut", m_trackClusterNLayersCut));

	if(0 == m_trackClusterNLayersCut)
		return pandora::STATUS_CODE_INVALID_PARAMETER;

	m_trackClusterDistanceCut1 = 75.f;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
			"TrackClusterDistanceCut1", m_trackClusterDistanceCut1));

	m_trackClusterDistanceCut2 = 40.f;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
			"TrackClusterDistanceCut2", m_trackClusterDistanceCut2));

	if(m_trackClusterDistanceCut1 < m_trackClusterDistanceCut2)
		return pandora::STATUS_CODE_INVALID_PARAMETER;

	m_trackClusterProjectionCut = 30.f;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
			"TrackClusterProjectionCut", m_trackClusterProjectionCut));

	m_firstInteractingLayerNSeedCut = 5;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
			"FirstInteractingLayerNSeedCut", m_firstInteractingLayerNSeedCut));

    return pandora::STATUS_CODE_SUCCESS;
}

} 
