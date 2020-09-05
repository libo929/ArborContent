  /// \file MatchTrackWithStartingClusterAlgorithm.h
/*
 *
 * MatchTrackWithStartingClusterAlgorithm.h header template automatically generated by a class generator
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


#ifndef MATCHTRACKWITHSTARTINGCLUSTER_H
#define MATCHTRACKWITHSTARTINGCLUSTER_H

#include "Pandora/Algorithm.h"
#include "Pandora/PandoraInputTypes.h"
#include "APRILApi/APRILInputTypes.h"

#include "APRILHelpers/CaloHitRangeSearchHelper.h"
#include "APRILHelpers/CaloHitNeighborSearchHelper.h"

namespace pandora { class ClusterFitResult; }

namespace april_content
{

/** 
 * @brief MatchTrackWithStartingClusterAlgorithm class
 */ 
class MatchTrackWithStartingClusterAlgorithm : public pandora::Algorithm
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
	pandora::StatusCode GetEligibleClusters(std::vector<APRILCluster*>& clusterVector) const;

private:
    bool            m_makeRecord;

};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *MatchTrackWithStartingClusterAlgorithm::Factory::CreateAlgorithm() const
{
    return new MatchTrackWithStartingClusterAlgorithm();
}

} 

#endif 