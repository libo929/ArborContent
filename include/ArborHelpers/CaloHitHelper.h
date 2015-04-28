  /// \file CaloHitHelper.h
/*
 *
 * CaloHitHelper.h header template automatically generated by a class generator
 * Creation date : lun. mars 30 2015
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


#ifndef CALOHITHELPER_H
#define CALOHITHELPER_H

#include "Pandora/PandoraInternal.h"
#include "Pandora/StatusCodes.h"

#include "ArborObjects/CaloHit.h"
#include "ArborObjects/Connector.h"

namespace pandora { class Algorithm; class CaloHit; }

namespace arbor_content
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
	 */
	static pandora::StatusCode ExtractCurrentLeafCaloHitList(const pandora::Algorithm &algorithm, pandora::CaloHitList &leafCaloHitList);

	/**
	 *  @brief  Extract the calo hit leaf list from the calo hit list
	 *
	 *  @param  pCaloHitList the list to extract the sub calo hit leaf list
	 *  @param  leafCaloHitList the calo hit leaf list to receive
	 */
	static pandora::StatusCode ExtractLeafCaloHitList(const pandora::CaloHitList *const pCaloHitList, pandora::CaloHitList &leafCaloHitList);

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
};

} 

#endif  //  CALOHITHELPER_H
