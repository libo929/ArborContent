  /// \file TrackDrivenSeedingTool.h
/*
 *
 * TrackDrivenSeedingTool.h header template automatically generated by a class generator
 * Creation date : mer. f�vr. 24 2016
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


#ifndef TRACKDRIVENSEEDINGTOOL_H
#define TRACKDRIVENSEEDINGTOOL_H

#include "ArborTools/ConnectorAlgorithmTool.h"

namespace arbor_content
{

class CaloHit;
class Connector;

/** 
 * @brief TrackDrivenSeedingTool class
 */ 
class TrackDrivenSeedingTool : public ConnectorAlgorithmTool
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm tool
     */
    class Factory : public pandora::AlgorithmToolFactory
    {
    public:
        pandora::AlgorithmTool *CreateAlgorithmTool() const;
    };

	/**
	 *  @brief  Destructor
	 */
	~TrackDrivenSeedingTool() { /* nop */ }

	/**
	 *  @brief  Seed connectors from the calo hit list
	 *
	 *  @param  algorithm the parent algorithm to access pandora content
	 *  @param  pCaloHitList the calo hit list to process
	 */
	pandora::StatusCode Process(const pandora::Algorithm &algorithm, const pandora::CaloHitList *const pCaloHitList);

	/**
	 *  @brief  Read settings from the xml handle
	 *
	 *  @param  xmlHandle the xml handle to settings
	 */
	pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

private:
	/**
	 *  @brief  Find the initial calo hits close to track extrapolation
	 *
	 *  @param  algorithm the algorithm to access pandora content
	 *  @param  pTrack the track used for extrapolation
	 *  @param  pInputCaloHitList the input calo hit list to go all over
	 *  @param  caloHitVector the calo hits close to track extrapolation to receive
	 */
	pandora::StatusCode FindInitialCaloHits(const pandora::Algorithm &algorithm, const pandora::Track *pTrack, const pandora::CaloHitList *const pInputCaloHitList,
			pandora::CaloHitVector &caloHitVector);

	const pandora::CartesianVector GetFromHitDirection(const arbor_content::CaloHit *const pCaloHit);

	/**
	 *  @brief  Find and connect calo hits from the ones found close to the track extrapolation
	 *
	 *  @param  algorithm the algorithm to access pandora content
	 *  @param  pTrack the track used for extrapolation
	 *  @param  pInputCaloHitList the input calo hit list to go all over
	 *  @param  caloHitVector the calo hits close to track extrapolation / all connected calo hits to receive
	 */
	pandora::StatusCode ConnectCaloHits(const pandora::Algorithm &algorithm, const pandora::Track *pTrack,
			pandora::CaloHitVector &caloHitVector);

private:
	unsigned int                    m_maxInitialPseudoLayer;
	float                           m_maxInitialTrackDistance;
	float                           m_maxInitialTrackAngle;

	float                           m_maxConnectionPseudoLayer;
	float                           m_maxTransverseDistanceFine;
	float                           m_maxTransverseDistanceCoarse;
	float                           m_maxDistanceToTrackFine;
	float                           m_maxDistanceToTrackCoarse;

	bool                            m_shouldUseIsolatedHits;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::AlgorithmTool *TrackDrivenSeedingTool::Factory::CreateAlgorithmTool() const
{
	return new TrackDrivenSeedingTool();
}

} 

#endif  //  TRACKDRIVENSEEDINGTOOL_H
