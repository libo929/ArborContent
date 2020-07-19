  /// \file ClusterFromTrackMergingAlgorithm.h
/*
 *
 * ClusterFromTrackMergingAlgorithm.h header template automatically generated by a class generator
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


#ifndef CLUSTERFROMTRACKMERGINGALGORITHM_H 
#define CLUSTERFROMTRACKMERGINGALGORITHM_H

#include "Pandora/Algorithm.h"
#include "Pandora/PandoraInputTypes.h"
#include "APRILApi/APRILInputTypes.h"

#include "APRILHelpers/CaloHitRangeSearchHelper.h"
#include "APRILHelpers/CaloHitNeighborSearchHelper.h"

namespace pandora { class ClusterFitResult; }

namespace april_content
{

/** 
 * @brief ClusterFromTrackMergingAlgorithm class
 */ 
class ClusterFromTrackMergingAlgorithm : public pandora::Algorithm
{
public:
	/**
	 *  @brief  Factory class for instantiating algorithm
	 */
	class Factory : public pandora::AlgorithmFactory
	{
	public:
		pandora::Algorithm *CreateAlgorithm() const;
	};

private:
	pandora::StatusCode Run();
	pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);
  
    april_content::APRILCluster* GetMainCluster(const pandora::CaloHitVector& caloHitVector);
  
    void GetNearbyClusters(pandora::Cluster* cluster, const std::vector<april_content::APRILCluster*>& clusterVector, 
			std::vector<april_content::APRILCluster*>& clustersInRange);

    void SearchProperClusters(const pandora::Track* pTrack, APRILCluster* startingCluster, 
		  std::vector<april_content::APRILCluster*>& properClusters);

    pandora::StatusCode CleanClusterForMerging(std::vector<APRILCluster*>& clusterVector);

	/**
	 *  @brief  Get the eligible clusters for parent-daughter association
	 *
	 *  @param  clusterVector the cluster vector to receive
	 */
	pandora::StatusCode GetEligibleClusters(std::vector<APRILCluster*>& clusterVector) const;

	/**
	 *  @brief  Algorithm workhorse, find associations between daughter and parent cluster
	 *
	 *  @param  clusterVector the input cluster vector
	 *  @param  clusterToClusterMap the map of daughter to parent cluster to receive
	 */
	pandora::StatusCode FindClustersToMerge(const pandora::ClusterVector &clusterVector, ClusterToClusterMap &clusterToClusterMap) const;

	/**
	 *  @brief  Whether the cluster is eligible for association
	 *
	 *  @param  pCluster address of a candidate cluster for association
	 *
	 *  @return  boolean
	 */
	bool CanMergeCluster(const pandora::Cluster *const pCluster) const;

	/**
	 *  @brief  Find the best parent cluster to merge a daughter one
	 *
	 *  @param  pDaughterCluster address of the daughter cluster
	 *  @param  clusterVector a cluster vector
	 *  @param  pBestParentCluster address of the best parent cluster to receive
	 */
	pandora::StatusCode FindBestParentCluster(const pandora::Cluster *const pDaughterCluster, const pandora::ClusterVector &clusterVector,
			const pandora::Cluster *&pBestParentCluster) const;

	/**
	 *  @brief  Get the cluster backward direction and the inner cluster position using a cluster fit of the n first layers
	 *
	 *  @param  pCluster the input cluster address
	 *  @param  backwardDirection the backward direction cartesian vector to receive
	 *  @param  innerPosition the inner cluster position to receive
	 */
	pandora::StatusCode GetClusterBackwardDirection(const pandora::Cluster *const pCluster, pandora::CartesianVector &backwardDirection, pandora::CartesianVector &innerPosition) const;

private:
	bool                             m_discriminatePhotonPid;            ///< Whether to discriminate photons
	bool                             m_allowNeutralParentMerging;
	unsigned int                     m_minNCaloHits;
	unsigned int                     m_maxNCaloHits;
	unsigned int                     m_minNPseudoLayers;
	unsigned int                     m_maxNPseudoLayers;
	float                            m_chi2AssociationCut;
	unsigned int                     m_nBackwardLayersFit;
	float                            m_maxBackwardAngle;
	float                            m_maxBackwardDistanceFine;
	float                            m_maxBackwardDistanceCoarse;
	unsigned int                     m_maxBackwardPseudoLayer;
	unsigned int                     m_minParentClusterBackwardNHits;
	float                            m_maxClusterFitDca;
	float                            m_minClusterCosOpeningAngle;
	float                            m_minClusterFitCosOpeningAngle;
	float                            m_minClusterFitCosOpeningAngle2;

	float                            m_maxStartingClusterDistance;
	float                            m_maxClusterTrackAngle;
	float                            m_maxClusterDistanceToMerge;

	bool                             m_resetCluster;

	arma::mat m_clusterCentroidsMatrix;
	std::vector<APRILCluster*> m_clustersToMerge;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ClusterFromTrackMergingAlgorithm::Factory::CreateAlgorithm() const
{
    return new ClusterFromTrackMergingAlgorithm();
}

} 

#endif
