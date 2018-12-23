  /// \file ArborClusteringAlgorithm.h
/*
 *
 * ArborClusteringAlgorithm.h header template automatically generated by a class generator
 * Creation date : ven. d�c. 4 2015
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


#ifndef ARBORCLUSTERINGALGORITHM_H
#define ARBORCLUSTERINGALGORITHM_H

#include "Pandora/Algorithm.h"
#include "Pandora/PandoraInternal.h"
#include "Pandora/PandoraInputTypes.h"
#include "ArborApi/ArborInputTypes.h"

namespace arbor_content
{

/** 
 * @brief ArborClusteringAlgorithm class
 */ 
class ArborClusteringAlgorithm : public pandora::Algorithm
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

    static const pandora::CaloHitList* GetCaloHitList() { return m_pCaloHitList; }
    static const pandora::CaloHitList* GetEcalHitList() { return &m_hcalCaloHitList; }
    static const pandora::CaloHitList* GetHcalHitList() { return &m_hcalCaloHitList; }
    static const pandora::CaloHitList* GetMuonHitList() { return &m_muonCaloHitList; }

private:
	pandora::StatusCode Run();
	pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

	/**
	 *  @brief  Split the input calo hit list into three lists for ecal, hcal and muon.
	 *
	 *  @param  pCaloHitList the input calo hit list to split
	 *  @param  ecalCaloHitList the ecal calo hit list to receive
	 *  @param  hcalCaloHitList the hcal calo hit list to receive
	 *  @param  muonCaloHitList the muon calo hit list to receive
	 */
	pandora::StatusCode SplitCaloHitList(const pandora::CaloHitList *const pCaloHitList, pandora::CaloHitList &ecalCaloHitList,
			pandora::CaloHitList &hcalCaloHitList, pandora::CaloHitList &muonCaloHitList) const;

	/**
	 *  @brief  Process calo hit list connection using the algorithm tool list
	 *
	 *  @param  caloHitList the input calo hit list to connect
	 *  @param  toolVector the list of algorithm tool used for connecting calo hits
	 */
    pandora::StatusCode ConnectCaloHits(const ConnectorAlgorithmToolVector &toolVector) const;

	/**
	 *  @brief  Create clusters from the previously connected calo hits
	 */
	pandora::StatusCode CreateClusters() const;


private:
	ConnectorAlgorithmToolVector      m_toolList;        ///< The algorithm tool list

    static const pandora::CaloHitList*       m_pCaloHitList; 
    static pandora::CaloHitList              m_ecalCaloHitList; 
	static pandora::CaloHitList              m_hcalCaloHitList; 
	static pandora::CaloHitList              m_muonCaloHitList;

	bool                              m_allowSingleHitClusters;    ///< Whether to allow single calo hit clusters
	bool                              m_useMultithread;            ///< Whether to use multithreading with ecal-hcal-muon tools
    bool                              m_useAsIndependent;          ///< Whether to use this algorithm as a mother algorithm
    
    // It seems that we can not add any member of type ConnectorAlgorithmToolVector or string more (?), otherwise it will crash.
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ArborClusteringAlgorithm::Factory::CreateAlgorithm() const
{
    return new ArborClusteringAlgorithm();
}

} 

#endif  //  ARBORCLUSTERINGALGORITHM_H
