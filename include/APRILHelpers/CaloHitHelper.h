  /// \file CaloHitHelper.h
/*
 *
 * CaloHitHelper.h header template automatically generated by a class generator
 * Creation date : lun. mars 30 2015
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


#ifndef CALOHITHELPER_H
#define CALOHITHELPER_H

#include "Pandora/PandoraInternal.h"
#include "Pandora/StatusCodes.h"

#include "APRILObjects/CaloHit.h"
#include "APRILObjects/Connector.h"

namespace pandora { class Algorithm; class CaloHit; class Track; }

namespace april_content
{

/** 
 * @brief CaloHitHelper class
 */ 
class CaloHitHelper 
{
public:
	/**
	 *  @brief  Remove the connections of the current calo hit list
	 *
	 *  @param  algorithm the algorithm to access the content
	 */
	static pandora::StatusCode RemoveCurrentConnections(const pandora::Algorithm &algorithm);

	/**
	 *  @brief Remove the connections of the calo hit list
	 *
	 *  @param  pCaloHitList the calo hit list to remove the connections
	 */
	static pandora::StatusCode RemoveConnections(const pandora::CaloHitList *const pCaloHitList);

	/**
	 *  @brief  Extract the calo hit seed list from the current calo hit list
	 *
	 *  @param  algorithm the algorithm to access the content
	 *  @param  seedCaloHitList the calo hit seed list to receive
	 *  @param  discriminateLeafHits whether the leaf/seed hits have to be put in the extracted list
	 */
	static pandora::StatusCode ExtractCurrentSeedCaloHitList(const pandora::Algorithm &algorithm, pandora::CaloHitList &seedCaloHitList, bool discriminateLeafHits = false);

	/**
	 *  @brief  Extract the calo hit seed list from the calo hit list
	 *
	 *  @param  pCaloHitList the list to extract the sub calo hit seed list
	 *  @param  seedCaloHitList the calo hit seed list to receive
	 *  @param  discriminateLeafHits whether the leaf/seed hits have to be put in the extracted list
	 */
	static pandora::StatusCode ExtractSeedCaloHitList(const pandora::CaloHitList *const pCaloHitList, pandora::CaloHitList &seedCaloHitList, bool discriminateLeafHits = false);

	/**
	 *  @brief  Extract the calo hit leaf list from the current calo hit list
	 *
	 *  @param  algorithm the algorithm to access the content
	 *  @param  leafCaloHitList the calo hit leaf list to receive
	 *  @param  discriminateLeafHits whether the leaf/seed hits have to be put in the extracted list
	 */
	static pandora::StatusCode ExtractCurrentLeafCaloHitList(const pandora::Algorithm &algorithm, pandora::CaloHitList &leafCaloHitList, bool discriminateSeedHits = false);

	/**
	 *  @brief  Extract the calo hit leaf list from the calo hit list
	 *
	 *  @param  pCaloHitList the list to extract the sub calo hit leaf list
	 *  @param  leafCaloHitList the calo hit leaf list to receive
	 *  @param  discriminateLeafHits whether the leaf/seed hits have to be put in the extracted list
	 */
	static pandora::StatusCode ExtractLeafCaloHitList(const pandora::CaloHitList *const pCaloHitList, pandora::CaloHitList &leafCaloHitList, bool discriminateSeedHits = false);

	/**
	 *  @brief  Build the calo hit list starting from a calo hit by looking recursively in the connector list (forward or backward)
	 *
	 *  @param  pCaloHit the calo hit to start from
	 *  @param  direction the direction to look recursively for
	 *  @param  caloHitList the calo hit list recursively built to receive
	 */
	static pandora::StatusCode BuildCaloHitList(const CaloHit *const pCaloHit, ConnectorDirection direction, pandora::CaloHitList &calohitList);

	/**
	 *	@brief  Build the calo hit list starting from a calo hit by looking recursively in the connector list (forward or backward).
	 *	        Stops after n recursive steps from the initial calo hit or after reaching a pseudo layer limit
	 *
	 *  @param  pCaloHit the calo hit to start from
	 *  @param  direction the direction to look recursively for
	 *  @param  caloHitList the calo hit list recursively built to receive
	 *  @param  connectionLimit the maximum number of recursive steps from the initial calo hit
	 *  @param  pseudoLayerLimit the pseudo layer limit to reach for stopping the recursive list building
	 */
	static pandora::StatusCode BuildCaloHitList(const CaloHit *const pCaloHit, ConnectorDirection direction, pandora::CaloHitList &calohitList,
			unsigned int connectionLimit, unsigned int pseudoLayerLimit);

	/**
	 *  @brief  Get the mean direction of the calo hit in a given direction
	 *
	 *  @param  pCaloHit the calo hit to evaluate the mean direction
	 *  @param  connectorDirection the connector direction to evaluate the mean direction
	 *  @param  direction the mean direction to receive
	 *  @param  depth the maximum connector depth to reach for the direction evaluation
	 */
	static pandora::StatusCode GetMeanDirection(const CaloHit *const pCaloHit, ConnectorDirection connectorDirection,
			 pandora::CartesianVector &direction, unsigned int depth);

	/**
	 * @brief  Whether the two calo hits can be connected.
	 *         The valid region to connect two hits is contained within a cone
	 *
	 * @param  pCaloHit1 a backward calo hit to connect
	 * @param  pCaloHit2 a forward calo hit to connect
	 * @param  exceptedDirection the expected of the shower development from pCaloHit1
	 * @param  normaleMaxAngle the maximum angle between the two hits when the expected direction is colinear to the cell direction of pCaloHit1
	 * @param  normaleMaxDistance the maximum distance between the two hits when the expected direction is colinear to the cell direction of pCaloHit1
	 * @param  transverseMaxAngle the maximum angle between the two hits when the expected direction is perpendicular to the cell direction of pCaloHit1
	 * @param  transverseMaxDistance the maximum distance between the two hits when the expected direction is perpendicular to the cell direction of pCaloHit1
	 */
	static bool CanConnect(const CaloHit *const pCaloHit1, const CaloHit *const pCaloHit2, const pandora::CartesianVector &expectedDirection,
			const float normaleMaxAngle, const float normaleMaxDistance,
			const float transverseMaxAngle, const float transverseMaxDistance);

	/**
	 * @brief  Whether the track and the calo hit can be connected.
	 *         The valid region is contained within a cone
	 *
	 * @param  pTrack a track with a valid calorimeter projection
	 * @param  pCaloHit a forward calo hit to connect
	 * @param  normaleMaxAngle the maximum angle between the two hits when the expected direction is colinear to the cell direction of pCaloHit1
	 * @param  normaleMaxDistance the maximum distance between the two hits when the expected direction is colinear to the cell direction of pCaloHit1
	 * @param  transverseMaxAngle the maximum angle between the two hits when the expected direction is perpendicular to the cell direction of pCaloHit1
	 * @param  transverseMaxDistance the maximum distance between the two hits when the expected direction is perpendicular to the cell direction of pCaloHit1
	 */
	static bool CanConnect(const pandora::Track *const pTrack, const CaloHit *const pCaloHit,
			const float normaleMaxAngle, const float normaleMaxDistance,
			const float transverseMaxAngle, const float transverseMaxDistance);

	/**
	 * @brief  Whether the tested position is in the region of interest.
	 *         The valid region is contained within a cone
	 *
	 * @param  startRegionPosition the position where the region of interest starts
	 * @param  testPosition the position to test inside the region of interest
	 * @param  exceptedDirection the expected of the shower development from startRegionPosition
	 * @param  normaleMaxAngle the maximum angle between the two hits when the expected direction is colinear to the cell direction of startRegionPosition
	 * @param  normaleMaxDistance the maximum distance between the two hits when the expected direction is colinear to the cell direction of startRegionPosition
	 * @param  transverseMaxAngle the maximum angle between the two hits when the expected direction is perpendicular to the cell direction of startRegionPosition
	 * @param  transverseMaxDistance the maximum distance between the two hits when the expected direction is perpendicular to the cell direction of startRegionPosition
	 */
	static bool IsInRegionOfInterest(const pandora::CartesianVector &startRegionPosition, const pandora::CartesianVector &testPosition, const pandora::CartesianVector &expectedDirection,
			const pandora::CartesianVector &normaleVector, const float normaleMaxAngle, const float normaleMaxDistance, const float transverseMaxAngle, const float transverseMaxDistance);
};

} 

#endif  //  CALOHITHELPER_H