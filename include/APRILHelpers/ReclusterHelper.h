/*
 *
 * ReclusterHelper.h header template automatically generated by a class generator
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


#ifndef RECLUSTERHELPER_H
#define RECLUSTERHELPER_H

#include "Pandora/PandoraInputTypes.h"
#include "Pandora/PandoraInternal.h"

namespace april_content
{

class CaloHit;

/**
 *  @brief  ReclusterResult class
 */
class ReclusterResult
{
public:
	/**
	 *
	 */
	float GetChi() const;

	/**
	 *
	 */
	float GetChi2() const;

	/**
	 *
	 */
	float GetChiPerDof() const;

	/**
	 *
	 */
	float GetChi2PerDof() const;

	/**
	 *
	 */
	float GetNeutralEnergy() const;

	/**
	 *
	 */
	float GetChargedEnergy() const;

	/**
	 *
	 */
	float GetChiWorstAssociation() const;

	/**
	 *
	 */
	void SetChi(float chi);

	/**
	 *
	 */
	void SetChi2(float chi2);

	/**
	 *
	 */
	void SetChiPerDof(float chiPerDof);

	/**
	 *
	 */
	void SetChi2PerDof(float chi2PerDof);

	/**
	 *
	 */
	void SetNeutralEnergy(float neutralEnergy);

	/**
	 *
	 */
	void SetChargedEnergy(float chargedEnergy);

	/**
	 *
	 */
	void SetChiWorstAssociation(float chiWorstAssociation);

private:
	float                  m_chi;
	float                  m_chi2;
	float                  m_chiPerDof;
	float                  m_chi2PerDof;
	float                  m_neutralEnergy;
	float                  m_chargedEnergy;
	float                  m_chiWorstAssociation;
};

/** 
 * @brief ReclusterHelper class
 */ 
class ReclusterHelper 
{
public:
    /**
     *  @brief  Evaluate the compatibility of a cluster with its associated tracks. Reclustering can be used to split up a
     *          cluster and produce more favourable track/cluster matches.
     *
     *  @param  pandora the associated pandora instance
     *  @param  pCluster address of the cluster
     *  @param  trackList address of the list of tracks associated with the cluster
     *  @param  energyResolutionFactory the number of sigmas of resolution in the chi computation
     *
     *  @return the chi value for the suitability of the track/cluster associations
     */
    static float GetTrackClusterCompatibility(const pandora::Pandora &pandora, const pandora::Cluster *const pCluster,
        const pandora::TrackList &trackList);
  
	static float GetTrackClusterCompatibility(const pandora::Pandora &pandora, const pandora::Cluster *const pClusterToEnlarge, const pandora::Cluster *const pClusterToMerge, const pandora::TrackList &trackList);

    /**
     *  @brief  Evaluate the compatibility of a cluster with its associated tracks. Reclustering can be used to split up a
     *          cluster and produce more favourable track/cluster matches.
     *
     *  @param  pandora the associated pandora instance
     *  @param  clusterEnergy the cluster energy
     *  @param  trackEnergy the sum of the energies of the associated tracks
     *  @param  energyResolutionFactory the number of sigmas of resolution in the chi computation
     *
     *  @return the chi value for the suitability of the track/cluster associations
     */
    static float GetTrackClusterCompatibility(const pandora::Pandora &pandora, const float clusterEnergy, const float trackEnergy);

    /**
     *  @brief  Extract the recluster result
     *
     *  @param  pandora the pandora instance to access content
     *  @param  clusterList the newly created clusters after a reclustering step
     *  @param  reclusterResult the recluster results
     */
    static pandora::StatusCode ExtractReclusterResults(const pandora::Pandora &pandora, const pandora::ClusterList* clusterList, ReclusterResult &reclusterResult);

    /**
     *  @brief  Create a separated tree cluster from a seed calo hit contained in an original cluster.
     *          Possible only if the original cluster contains more than one tree (more than one seed calo hit).
     *          Calo hits of the newly created tree cluster are removed from the original one
     *
     *  @param  algorithm the algorithm instance used to access pandora content
     *  @param  pSeedCaloHit the seed calo hit pointer to build the separated tree from
     *  @param  pOriginalCluster the original cluster pointer from which to separate the tree
     */
    static pandora::StatusCode SplitTreeFromCluster(const pandora::Algorithm &algorithm, const april_content::CaloHit *const pSeedCaloHit,
    		const pandora::Cluster *const pOriginalCluster, const pandora::Cluster *&pSeparatedTreeCluster, const std::string &originalClusterListName = "");

    /**
     *  @brief  Create a cluster list for each tree contained in the cluster
     *          Possible only if the cluster contains more than one tree (more than one seed calo hit)
     *
     *  @param  algorithm the algorithm instance used to access pandora content
     *  @param  pCluster the cluster to split
     *  @param  treeClusterVector the split list of tree clusters
     */
    static pandora::StatusCode SplitClusterIntoTreeClusters(const pandora::Algorithm &algorithm, const pandora::Cluster *const pCluster,
    		pandora::ClusterVector &treeClusterVector);
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline float ReclusterResult::GetChi() const
{
	return m_chi;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ReclusterResult::GetChi2() const
{
	return m_chi2;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ReclusterResult::GetChiPerDof() const
{
	return m_chiPerDof;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ReclusterResult::GetChi2PerDof() const
{
	return m_chi2PerDof;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ReclusterResult::GetNeutralEnergy() const
{
	return m_neutralEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ReclusterResult::GetChargedEnergy() const
{
	return m_chargedEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ReclusterResult::GetChiWorstAssociation() const
{
	return m_chiWorstAssociation;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ReclusterResult::SetChi(float chi)
{
	m_chi = chi;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ReclusterResult::SetChi2(float chi2)
{
	m_chi2 = chi2;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ReclusterResult::SetChiPerDof(float chiPerDof)
{
	m_chiPerDof = chiPerDof;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ReclusterResult::SetChi2PerDof(float chi2PerDof)
{
	m_chi2PerDof = chi2PerDof;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ReclusterResult::SetNeutralEnergy(float neutralEnergy)
{
	m_neutralEnergy = neutralEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ReclusterResult::SetChargedEnergy(float chargedEnergy)
{
	m_chargedEnergy = chargedEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ReclusterResult::SetChiWorstAssociation(float chiWorstAssociation)
{
	m_chiWorstAssociation = chiWorstAssociation;
}

} 

#endif  //  RECLUSTERHELPER_H
