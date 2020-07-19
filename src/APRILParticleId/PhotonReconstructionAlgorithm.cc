/// \file PhotonReconstructionAlgorithm.cc
/*
 *
 * PhotonReconstructionAlgorithm.cc source template automatically generated by a class generator
 * Creation date : lun. oct. 24 2016
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


#include "APRILParticleId/PhotonReconstructionAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"
#include "APRILHelpers/ClusterHelper.h"

#include "APRILHelpers/ClusterPropertiesHelper.h"
#include "APRILHelpers/HistogramHelper.h"
#include "APRILHelpers/BDTBasedClusterIdHelper.h"
#include "APRILHelpers/CaloHitRangeSearchHelper.h"

namespace april_content
{
  float m_minHitLayer, m_clusterVol, m_hitOutsideRatio, m_axisLengthRatio;

  TMVA::Reader* PhotonReconstructionAlgorithm::m_reader = NULL;

  pandora::StatusCode PhotonReconstructionAlgorithm::Run()
  {
	  // book MVA
	if(m_reader==NULL) 
	{
        m_reader = new TMVA::Reader( "!Color:!Silent" ); 

	    m_reader->AddVariable( "minHitLayer",         &m_minHitLayer);
	    m_reader->AddVariable( "clusterVol",          &m_clusterVol);
        m_reader->AddVariable( "hitOutsideRatio",     &m_hitOutsideRatio);
        m_reader->AddVariable( "axisLengthRatio",     &m_axisLengthRatio);

        // --- Book the MVA methods
        TString methodName = TString("ClusterTagging_BDT");
        TString weightfile = TString("/home/libo/workplace/APRILPFA/code/APRILContent/weights/ClusterTagging_BDT.weights.xml");

        try 
        {
	    	m_reader->BookMVA( methodName, weightfile ); 
        }
        catch (...)
        {
	    	std::cout << "Error: MVA reader !!!" << std::endl;
        }
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
    // If specified, change the current calo hit list, i.e. the input to the clustering algorithm
    std::string originalCaloHitListName;

    if (!m_inputCaloHitListName.empty())
    {
      PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentListName<pandora::CaloHit>(*this, originalCaloHitListName));
      PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<pandora::CaloHit>(*this, m_inputCaloHitListName));
    }

    {
      // First check if cluster list name already exists to assess pre-photon formation
      const pandora::ClusterList *pPhotonClusterList = NULL;
      const pandora::StatusCode statusCode(PandoraContentApi::GetList(*this, m_photonClusterListName, pPhotonClusterList));

      if(pandora::STATUS_CODE_SUCCESS == statusCode)
      {
        std::cout << "PhotonReconstructionAlgorithm: photon cluster list name '" << m_photonClusterListName << " already exists! Skipping photon cluster formation ..." << std::endl;
        return pandora::STATUS_CODE_SUCCESS;
      }
      else if(pandora::STATUS_CODE_NOT_INITIALIZED != statusCode)
      {
        return statusCode;
      }
    }

    // Run the initial cluster formation algorithm
    const pandora::ClusterList *pClusterList = NULL;
    std::string newClusterListName;
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, m_clusteringAlgorithmName,
        pClusterList, newClusterListName));
      
    // Select photons from created clusters
    pandora::ClusterList photonClusters;
    pandora::ClusterList nonPhotonClusters;
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, 
			                 this->SelectPhotonsAndRemoveOthers(pClusterList, photonClusters, nonPhotonClusters));
      
    // Save the new cluster list
    if (!photonClusters.empty())
    {
      std::cout << "  ---> The photon cluster size: " << photonClusters.size() << std::endl;
        
      PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList(*this, m_photonClusterListName, photonClusters));

      if (m_replaceCurrentClusterList)
        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<pandora::Cluster>(*this, m_photonClusterListName));
    }

    if (!nonPhotonClusters.empty())
    {
      std::cout << "  ---> The nonPhoton cluster size: " << nonPhotonClusters.size() << std::endl;
        
      PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList(*this, m_nonPhotonClusterListName, nonPhotonClusters));
	}

    // Unless specified, return current calo hit list to that when algorithm started
    if (m_restoreOriginalCaloHitList && !m_inputCaloHitListName.empty())
    {
      PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<pandora::CaloHit>(*this, originalCaloHitListName));
    }

#if 0
	// example of GetNeighbourHitsInRange
	{
       const pandora::CaloHitList *pCaloHitList = NULL;
       PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pCaloHitList));

	   float range = 200.;
	   pandora::CaloHitList hitsInRange;
	   const pandora::CaloHit* pCaloHit = *(pCaloHitList->begin());

	   CaloHitRangeSearchHelper::GetNeighbourHitsInRange(pCaloHitList, pCaloHit->GetPositionVector(), range, hitsInRange);

	   std::cout << "hits in range: " << hitsInRange.size() << std::endl;
	}
#endif

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode PhotonReconstructionAlgorithm::SelectPhotonsAndRemoveOthers(const pandora::ClusterList *const pInputClusterList, 
		                 pandora::ClusterList &photonClusters,
		                 pandora::ClusterList &nonPhotonClusters)
  {
    for(pandora::ClusterList::const_iterator iter = pInputClusterList->begin(), endIter = pInputClusterList->end() ;
        endIter != iter ; ++iter)
    {
#if 0
	   // TODO::
	   // put into a function
	   float minHitLayer, clusterVol, energyRatio, hitOutsideRatio, axisLengthRatio, shortAxisLengthRatio;

	   float nHit = (*iter)->GetNCaloHits();
	
	   ClusterPropertiesHelper::GetClusterProperties(*iter, minHitLayer, clusterVol, energyRatio, 
		                                     hitOutsideRatio, axisLengthRatio, shortAxisLengthRatio);
	   std::vector<float> vars;
	   vars.push_back( minHitLayer );
	   vars.push_back( clusterVol );
	   vars.push_back( energyRatio );
	   vars.push_back( hitOutsideRatio );
	   vars.push_back( axisLengthRatio );
	   vars.push_back( shortAxisLengthRatio );
	   vars.push_back( nHit );


	   const pandora::MCParticle *const pMCParticle(pandora::MCParticleHelper::GetMainMCParticle(*iter));
	   int clusterPID = pMCParticle->GetParticleId();

	   float clusterMCPEnergy = pMCParticle->GetEnergy();

	   if( clusterMCPEnergy > 1. )
	   { 
		   if( clusterPID == 22 )
	       {
			   HistogramManager::CreateFill("PhotonClustersVars", "minHitLayer:clusterVol:energyRatio:hitOutsideRatio:axisLengthRatio:shortAxisLengthRatio:nHit", vars);
	       }
	       else
	       { 
			   HistogramManager::CreateFill("NonPhotonClustersVars", "minHitLayer:clusterVol:energyRatio:hitOutsideRatio:axisLengthRatio:shortAxisLengthRatio:nHit", vars);
	       }
	   }

	   // TODO::
	   // use energyRatio;
	   ////////////////////////////////
	   m_minHitLayer = minHitLayer;
	   m_clusterVol = clusterVol;
	   m_hitOutsideRatio = hitOutsideRatio; 
	   m_axisLengthRatio = axisLengthRatio;
	
	   float photonLikelyhood = m_reader->EvaluateMVA( "ClusterTagging_BDT" );

	   std::cout << "photon likelyhood: " << photonLikelyhood << ", mcp PDG: " << clusterPID << ": cluster energy: "
		         << (*iter)->GetHadronicEnergy() << std::endl;
#endif

      if(PandoraContentApi::GetPlugins(*this)->GetParticleId()->IsPhoton(*iter))
      {
		object_creation::ClusterMetadata metadata;
        metadata.m_particleId = pandora::PHOTON;

		PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::AlterMetadata(*this, *iter, metadata));

        photonClusters.push_back(*iter);

#if 0
		if ( BDTBasedClusterIdHelper::BDTEvaluate(*iter, bdtVal) == pandora::STATUS_CODE_SUCCESS )
		{
			terstd::cout << " photon cluster energy: " << (*iter)->GetElectromagneticEnergy() << ",  BDT: " << bdtVal << std::endl;
		}
#endif
      }
      else
      {
        nonPhotonClusters.push_back(*iter);
#if 0
		if ( BDTBasedClusterIdHelper::BDTEvaluate(*iter, bdtVal) == pandora::STATUS_CODE_SUCCESS )
		{
			std::cout << " non-photon cluster energy: " << (*iter)->GetElectromagneticEnergy() << ",  BDT: " << bdtVal << std::endl;
		}
#endif
      }
    }

	//std::cout << "-------- Photon cluster: " << photonClusters.size() << std::endl;

    // Maybe we can leave the non-photon clusters there, and use them for the later algorithms
    //PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ClusterHelper::CleanAndDeleteClusters(*this, nonPhotonClusters));

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode PhotonReconstructionAlgorithm::ReadSettings(const pandora::TiXmlHandle xmlHandle)
  {
    // Daughter algorithm parameters
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ProcessAlgorithm(*this, xmlHandle,
        "ClusterFormation", m_clusteringAlgorithmName));

    // Input parameters: name of input calo hit list and whether it should persist as the current list after algorithm has finished
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "InputCaloHitListName", m_inputCaloHitListName));

    m_restoreOriginalCaloHitList = !m_inputCaloHitListName.empty();
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "RestoreOriginalCaloHitList", m_restoreOriginalCaloHitList));

    // Output parameters: name of output cluster list and whether it should subsequently be used as the current list
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "PhotonClusterListName", m_photonClusterListName));

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "NonPhotonClusterListName", m_nonPhotonClusterListName));

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "ReplaceCurrentClusterList", m_replaceCurrentClusterList));

    return pandora::STATUS_CODE_SUCCESS;
  }
} 

