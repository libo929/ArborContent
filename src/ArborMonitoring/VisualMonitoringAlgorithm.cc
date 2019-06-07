/// \file VisualMonitoringAlgorithm.cc
/*
 *
 * VisualMonitoringAlgorithm.cc source template automatically generated by a class generator
 * Creation date : mar. avr. 14 2015
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

#include "ArborMonitoring/VisualMonitoringAlgorithm.h"

#include "ArborObjects/CaloHit.h"

#ifdef MONITORING
#include <TEveElement.h>
#include <TEveArrow.h>
#include <TEveManager.h>
#include <TGLViewer.h>
#endif

namespace arbor_content
{

  VisualMonitoringAlgorithm::VisualMonitoringAlgorithm() :
      m_showCurrentConnectors(true),
      m_connectorLevel("pfos"),
      m_showCurrentMCParticles(false),
      m_showCurrentCaloHits(false),
      m_showCurrentTracks(false),
      m_showCurrentClusters(true),
      m_showCurrentPfos(true),
      m_showCurrentVertices(false),
      m_displayEvent(true),
      m_showDetector(true),
      m_detectorView("xz"),
      m_showOnlyAvailable(false),
      m_showAssociatedTracks(false),
      m_hitColors("pfo"),
      m_thresholdEnergy(-1.f),
      m_transparencyThresholdE(-1.f),
      m_energyScaleThresholdE(1.f),
      m_showPfoVertices(true),
      m_showPfoHierarchy(true)
  {
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode VisualMonitoringAlgorithm::Run()
  {
    PANDORA_MONITORING_API(SetEveDisplayParameters(this->GetPandora(), m_showDetector, (m_detectorView.find("xz") != std::string::npos) ? ::DETECTOR_VIEW_XZ :
        (m_detectorView.find("xy") != std::string::npos) ? ::DETECTOR_VIEW_XY : ::DETECTOR_VIEW_DEFAULT, m_transparencyThresholdE, m_energyScaleThresholdE));

    // Show current mc particles
    if (m_showCurrentMCParticles)
    {
      this->VisualizeMCParticleList(std::string());
    }

    // Show specified lists of mc particles
    for (pandora::StringVector::const_iterator iter = m_mcParticleListNames.begin(), iterEnd = m_mcParticleListNames.end(); iter != iterEnd; ++iter)
    {
      this->VisualizeMCParticleList(*iter);
    }

    // Show current calo hit list
    if (m_showCurrentCaloHits)
    {
      this->VisualizeCaloHitList(std::string());
    }

    // Show specified lists of calo hits
    for (pandora::StringVector::const_iterator iter = m_caloHitListNames.begin(), iterEnd = m_caloHitListNames.end(); iter != iterEnd; ++iter)
    {
      this->VisualizeCaloHitList(*iter);
    }

    // Show current cluster list
    if (m_showCurrentClusters)
    {
      this->VisualizeClusterList(std::string());
    }

    // Show specified lists of clusters
    for (pandora::StringVector::const_iterator iter = m_clusterListNames.begin(), iterEnd = m_clusterListNames.end(); iter != iterEnd; ++iter)
    {
      this->VisualizeClusterList(*iter);
    }

    // Show current track list
    if (m_showCurrentTracks)
    {
      this->VisualizeTrackList(std::string());
    }

    // Show specified lists of tracks
    for (pandora::StringVector::const_iterator iter = m_trackListNames.begin(), iterEnd = m_trackListNames.end(); iter != iterEnd; ++iter)
    {
      this->VisualizeTrackList(*iter);
    }

    // Show current particle flow objects
    if (m_showCurrentPfos)
    {
      this->VisualizeParticleFlowList(std::string());
    }

    // Show specified lists of pfo
    for (pandora::StringVector::const_iterator iter = m_pfoListNames.begin(), iterEnd = m_pfoListNames.end(); iter != iterEnd; ++iter)
    {
      this->VisualizeParticleFlowList(*iter);
    }

    // Show current vertex objects
    if (m_showCurrentVertices)
    {
      this->VisualizeVertexList(std::string());
    }

    // Show specified lists of vertices
    for (pandora::StringVector::const_iterator iter = m_vertexListNames.begin(), iterEnd = m_vertexListNames.end(); iter != iterEnd; ++iter)
    {
      this->VisualizeVertexList(*iter);
    }
      
    // Set the background color. The one set in Pandora is white.
    if(m_darkBackground)
    {
#ifdef MONITORING
        if(this->GetPandora().GetSettings()->IsMonitoringEnabled())
	    {
            TEveManager* pEveManager = TEveManager::Create();
            if(NULL != pEveManager)
            {
                TGLViewer *pTGLViewer = pEveManager->GetDefaultGLViewer();
                if(NULL != pTGLViewer) 
                {
                    pTGLViewer->ColorSet().Background().SetColor(kBlack);
                    pTGLViewer->UpdateScene();
                }
            }
	    }
#endif
    }

    // Finally, display the event and pause application
    if (m_displayEvent)
    {
      PANDORA_MONITORING_API(ViewEvent(this->GetPandora()));
    }

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void VisualMonitoringAlgorithm::VisualizeMCParticleList(const std::string &listName) const
  {
    const pandora::MCParticleList *pMCParticleList = NULL;

    if (listName.empty())
    {
      if (pandora::STATUS_CODE_SUCCESS != PandoraContentApi::GetCurrentList(*this, pMCParticleList))
      {
        if (PandoraContentApi::GetSettings(*this)->ShouldDisplayAlgorithmInfo())
          //std::cout << "VisualMonitoringAlgorithm: mc particle list unavailable." << std::endl;
        return;
      }
    }
    else
    {
      if (pandora::STATUS_CODE_SUCCESS != PandoraContentApi::GetList(*this, listName, pMCParticleList))
      {
        if (PandoraContentApi::GetSettings(*this)->ShouldDisplayAlgorithmInfo())
          //std::cout << "VisualMonitoringAlgorithm: mc particle list unavailable." << std::endl;
        return;
      }
    }

	pandora::MCParticleList photonList;
	pandora::MCParticleList chargedList;
	pandora::MCParticleList neutralList;

	for( auto cluIter = pMCParticleList->begin(); cluIter != pMCParticleList->end(); ++cluIter )
	{
		if(*cluIter == nullptr) continue;

		try
		{
			int mcpPDG = (*cluIter)->GetParticleId();

		    if(mcpPDG == 22) 
		    {
		    	photonList.push_back( *cluIter );
		    }
		    else
		    {
		    	int mcpCharge = pandora::PdgTable::GetParticleCharge( (*cluIter)->GetParticleId() );
		    	bool isCharged = mcpCharge != 0;

		    	if(isCharged)
		    	{
		    		chargedList.push_back( *cluIter );
		    	}
		    	else
		    	{
		    		neutralList.push_back( *cluIter );
		    	}
		    }
		}
        catch (pandora::StatusCodeException &)
		{
		}
	}

    //PANDORA_MONITORING_API(VisualizeMCParticles(this->GetPandora(), pMCParticleList, listName.empty() ? "CurrentMCParticles" : listName.c_str(),
    PANDORA_MONITORING_API(VisualizeMCParticles(this->GetPandora(), &photonList, listName.empty() ? "CurrentPhotonMCParticles" : listName.c_str(),
        ::AUTO, &m_particleSuppressionMap));

    PANDORA_MONITORING_API(VisualizeMCParticles(this->GetPandora(), &chargedList, listName.empty() ? "CurrentChargedMCParticles" : listName.c_str(),
        ::AUTO, &m_particleSuppressionMap));

    PANDORA_MONITORING_API(VisualizeMCParticles(this->GetPandora(), &neutralList, listName.empty() ? "CurrentNeutralMCParticles" : listName.c_str(),
        ::AUTO, &m_particleSuppressionMap));
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void VisualMonitoringAlgorithm::VisualizeCaloHitList(const std::string &listName) const
  {
    const pandora::CaloHitList *pCaloHitList = NULL;

    if (listName.empty())
    {
      if (pandora::STATUS_CODE_SUCCESS != PandoraContentApi::GetCurrentList(*this, pCaloHitList))
      {
        if (PandoraContentApi::GetSettings(*this)->ShouldDisplayAlgorithmInfo())
          //std::cout << "VisualMonitoringAlgorithm: current calo hit list unavailable." << std::endl;
        return;
      }
    }
    else
    {
      if (pandora::STATUS_CODE_SUCCESS != PandoraContentApi::GetList(*this, listName, pCaloHitList))
      {
        if (PandoraContentApi::GetSettings(*this)->ShouldDisplayAlgorithmInfo())
          //std::cout << "VisualMonitoringAlgorithm: calo hit list " << listName << " unavailable." << std::endl;
        return;
      }
    }

    // Filter calo hit list
    pandora::CaloHitList caloHitList;
    pandora::CaloHitList isoCaloHitList;

    for (pandora::CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
      const pandora::CaloHit *const pCaloHit = *iter;

      if ((pCaloHit->GetElectromagneticEnergy() > m_thresholdEnergy) &&
          (!m_showOnlyAvailable || PandoraContentApi::IsAvailable(*this, pCaloHit)))
      {
		if(pCaloHit->IsIsolated())
		{
			isoCaloHitList.push_back(pCaloHit);
		}
		else
		{
			caloHitList.push_back(pCaloHit);
		}
      }
    }

#ifdef MONITORING
    if(this->GetPandora().GetSettings()->IsMonitoringEnabled())
    {
      if(m_showCurrentConnectors && m_connectorLevel == "calohits")
      {
        TEveElement *pParentElement = pandora_monitoring::PandoraMonitoring::GetInstance(this->GetPandora())->VisualizeCaloHits(&caloHitList, listName.empty() ? "CurrentCaloHits" : listName.c_str(),
            0, (m_hitColors.find("energy") != std::string::npos ? ::AUTOENERGY : ::RED));

        TEveElement *pIsoParentElement = pandora_monitoring::PandoraMonitoring::GetInstance(this->GetPandora())->VisualizeCaloHits(&isoCaloHitList, listName.empty() ? "CurrentCaloHits" : listName.c_str(),
            0, (m_hitColors.find("energy") != std::string::npos ? ::AUTOENERGY : ::YELLOW));

        this->VisualizeConnectors(&caloHitList, listName.empty() ? "CurrentCaloHitsConnectors" : (listName+"Connectors").c_str(), pParentElement,
            (m_hitColors.find("energy") != std::string::npos ? ::AUTOENERGY : ::GRAY));

        this->VisualizeConnectors(&isoCaloHitList, listName.empty() ? "CurrentIsoCaloHitsConnectors" : (listName+"Connectors").c_str(), pIsoParentElement,
            (m_hitColors.find("energy") != std::string::npos ? ::AUTOENERGY : ::GRAY));
      }
      else
      {
        PANDORA_MONITORING_API(VisualizeCaloHits(this->GetPandora(), &caloHitList, listName.empty() ? "CurrentCaloHits" : listName.c_str(),
            (m_hitColors.find("energy") != std::string::npos ? ::AUTOENERGY : ::RED)));

        PANDORA_MONITORING_API(VisualizeCaloHits(this->GetPandora(), &isoCaloHitList, listName.empty() ? "CurrentIsoCaloHits" : listName.c_str(),
            (m_hitColors.find("energy") != std::string::npos ? ::AUTOENERGY : ::YELLOW)));
      }
    }
#endif

  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void VisualMonitoringAlgorithm::VisualizeTrackList(const std::string &listName) const
  {
    const pandora::TrackList *pTrackList = NULL;

    if (listName.empty())
    {
      if (pandora::STATUS_CODE_SUCCESS != PandoraContentApi::GetCurrentList(*this, pTrackList))
      {
        if (PandoraContentApi::GetSettings(*this)->ShouldDisplayAlgorithmInfo())
          //std::cout << "VisualMonitoringAlgorithm: current track list unavailable." << std::endl;
        return;
      }
    }
    else
    {
      if (pandora::STATUS_CODE_SUCCESS != PandoraContentApi::GetList(*this, listName, pTrackList))
      {
        if (PandoraContentApi::GetSettings(*this)->ShouldDisplayAlgorithmInfo())
          //std::cout << "VisualMonitoringAlgorithm: track list " << listName << " unavailable." << std::endl;
        return;
      }
    }

    // Filter track list
    pandora::TrackList trackList;

    for (pandora::TrackList::const_iterator iter = pTrackList->begin(), iterEnd = pTrackList->end(); iter != iterEnd; ++iter)
    {
      const pandora::Track *const pTrack = *iter;

      if (!m_showOnlyAvailable || pTrack->IsAvailable())
        trackList.push_back(pTrack);
    }

    PANDORA_MONITORING_API(VisualizeTracks(this->GetPandora(), &trackList, listName.empty() ? "CurrentTracks" : listName.c_str(), ::MAGENTA));
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void VisualMonitoringAlgorithm::VisualizeClusterList(const std::string &listName) const
  {
    const pandora::ClusterList *pClusterList = NULL;

    if (listName.empty())
    {
      if (pandora::STATUS_CODE_SUCCESS != PandoraContentApi::GetCurrentList(*this, pClusterList))
      {
        if (PandoraContentApi::GetSettings(*this)->ShouldDisplayAlgorithmInfo())
          //std::cout << "VisualMonitoringAlgorithm: current cluster list unavailable." << std::endl;
        return;
      }
    }
    else
    {
      if (pandora::STATUS_CODE_SUCCESS != PandoraContentApi::GetList(*this, listName, pClusterList))
      {
        if (PandoraContentApi::GetSettings(*this)->ShouldDisplayAlgorithmInfo())
          //std::cout << "VisualMonitoringAlgorithm: cluster list " << listName << " unavailable." << std::endl;
        return;
      }
    }

    // Filter cluster list
    pandora::ClusterList clusterList;

    for (pandora::ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
      const pandora::Cluster *const pCluster = *iter;

      if (!m_showOnlyAvailable || PandoraContentApi::IsAvailable(*this, pCluster))
      {
        clusterList.push_back(pCluster);
      }
    }

#ifdef MONITORING
    if(this->GetPandora().GetSettings()->IsMonitoringEnabled())
    {
      if(m_showCurrentConnectors && m_connectorLevel == "clusters")
      {
           VisualizeClusterListWithConnectors(&clusterList,listName.empty() ? "CurrentClusters" : listName.c_str(), 0, 
				(m_hitColors.find("particleid") != std::string::npos) ? ::AUTOID :
				(m_hitColors.find("iterate") != std::string::npos) ? ::AUTOITER :
				(m_hitColors.find("energy") != std::string::npos) ? ::AUTOENERGY : ::AUTO,
			     m_showAssociatedTracks);	
#if 0
        TEveElement *pParentElement = pandora_monitoring::PandoraMonitoring::GetInstance(this->GetPandora())->VisualizeClusters(&clusterList, listName.empty() ? "CurrentClusters" : listName.c_str(), 0,
            (m_hitColors.find("particleid") != std::string::npos) ? ::AUTOID :
                (m_hitColors.find("iterate") != std::string::npos) ? ::AUTOITER :
                    (m_hitColors.find("energy") != std::string::npos) ? ::AUTOENERGY : ::AUTO,
                        m_showAssociatedTracks);

        TEveElement *pConnectorDirectory = new TEveElementList();
        pConnectorDirectory->SetElementNameTitle("ClusterConnectors", "ClusterConnectors");
        pParentElement->AddElement(pConnectorDirectory);

        for(pandora::ClusterList::const_iterator iter = clusterList.begin(), endIter = clusterList.end() ;
            endIter != iter ; ++iter)
        {
          pandora::CaloHitList clusterHits;
          (*iter)->GetOrderedCaloHitList().FillCaloHitList(clusterHits);

          this->VisualizeConnectors(&clusterHits, listName.empty() ? "CurrentClustersConnectors" : (listName+"Connectors").c_str(), pConnectorDirectory,
              (m_hitColors.find("particleid") != std::string::npos) ? ::AUTOID :
                  (m_hitColors.find("iterate") != std::string::npos) ? ::AUTOITER :
                      (m_hitColors.find("energy") != std::string::npos) ? ::AUTOENERGY : ::AUTO);
        }
#endif
      }
      else
      {
        PANDORA_MONITORING_API(VisualizeClusters(this->GetPandora(), &clusterList, listName.empty() ? "CurrentClusters" : listName.c_str(),
            (m_hitColors.find("particleid") != std::string::npos) ? ::AUTOID :
                (m_hitColors.find("iterate") != std::string::npos) ? ::AUTOITER :
                    (m_hitColors.find("energy") != std::string::npos) ? ::AUTOENERGY : ::AUTO,
                        m_showAssociatedTracks));
      }
    }
#endif
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  std::string VisualMonitoringAlgorithm::GetHitTypeString(const pandora::HitType hitType) const
  {
      switch (hitType)
      {
		  case pandora::TRACKER : return "TRACKER";
		  case pandora::ECAL : return "ECAL";
          case pandora::HCAL : return "HCAL";
          case pandora::MUON : return "MUON";
          case pandora::TPC_VIEW_U : return "TPC_VIEW_U";
          case pandora::TPC_VIEW_V : return "TPC_VIEW_V";
          case pandora::TPC_VIEW_W : return "TPC_VIEW_W";
          case pandora::TPC_3D : return "TPC_3D";
          case pandora::HIT_CUSTOM : return "HIT_CUSTOM";
          default : return "Unknown";
      }
  }

  //------------------------------------------------------------------------------------------------------------------------------------------
  
  // rewrite the VisualizeClusters function in Pandora for visualizing connectors
  TEveElement* VisualMonitoringAlgorithm::VisualizeClusterListWithConnectors(const pandora::ClusterList *const pClusterList, 
		  const std::string &name, TEveElement *parent, const Color color, bool showAssociatedTracks) const
  {
	  pandora::ClusterVector clusterVector(pClusterList->begin(), pClusterList->end());
      
	  std::sort(clusterVector.begin(), clusterVector.end(), pandora_monitoring::PandoraMonitoring::SortClustersByHadronicEnergy);
  
      const std::string starter("---");
      const std::string clusterListTitle(name.empty() ? "Clusters" : name);
      std::string clusterListName(clusterListTitle);
      if (clusterListName.find(starter) != std::string::npos)
          clusterListName.replace(clusterListName.find(starter), starter.length(), "Clusters/");
      std::replace_if(clusterListName.begin(), clusterListName.end(), std::bind2nd(std::equal_to<char>(),'\n'), '/');
  
      TEveElement *pClusterVectorElement = new TEveElementList();
      pClusterVectorElement->SetElementNameTitle(clusterListName.c_str(), clusterListTitle.c_str());
  
      for (const pandora::Cluster *const pCluster : clusterVector)
      {
          if (pCluster->GetNCaloHits() == 0)
              continue;
  
          std::stringstream sstr;
          sstr << starter << "Cluster\nEem(corr)=" << pCluster->GetElectromagneticEnergy() << "\nEhad(corr)=" << pCluster->GetHadronicEnergy()
              << "\nNHits=" << pCluster->GetNCaloHits()
			  << "\nInnerHitType=" << GetHitTypeString(pCluster->GetInnerLayerHitType())
              << "\nOuterHitType=" << GetHitTypeString(pCluster->GetOuterLayerHitType());
  
          const Color clusterColor((color != AUTO) ? color : (pCluster->GetAssociatedTrackList().empty()) ? LIGHTBLUE : MAGENTA);
  
		  pandora::CaloHitList caloHitList;
          pCluster->GetOrderedCaloHitList().FillCaloHitList(caloHitList);

		  std::string clusterName(sstr.str());

          TEveElement *pCaloHitsElement = 
			  pandora_monitoring::PandoraMonitoring::GetInstance(this->GetPandora())->VisualizeCaloHits(
			  &caloHitList, clusterName.c_str(), pClusterVectorElement, clusterColor);

          this->VisualizeConnectors(&caloHitList, "Connectors", pCaloHitsElement, MAGENTA);

          this->VisualizeClusterDirection(pCluster, "Direction", pCaloHitsElement, GREEN);

          if (showAssociatedTracks && !pCluster->GetAssociatedTrackList().empty())
          {
              TEveElement *pTrackParentElement = pCaloHitsElement;
          
			  pandora_monitoring::PandoraMonitoring::GetInstance(this->GetPandora())->VisualizeTracks(
			  &(pCluster->GetAssociatedTrackList()), "Tracks", pTrackParentElement, MAGENTA);
          }
      }
  

      if (parent)
      {
          parent->AddElement(pClusterVectorElement);
      }
      else
      {
          gEve->AddElement(pClusterVectorElement);
          gEve->Redraw3D();
      }
  
      return pClusterVectorElement;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void VisualMonitoringAlgorithm::VisualizeParticleFlowList(const std::string &listName) const
  {
    const pandora::PfoList *pPfoList = NULL;

    if (listName.empty())
    {
      if (pandora::STATUS_CODE_SUCCESS != PandoraContentApi::GetCurrentList(*this, pPfoList))
      {
        if (PandoraContentApi::GetSettings(*this)->ShouldDisplayAlgorithmInfo())
          //std::cout << "VisualMonitoringAlgorithm: current pfo list unavailable." << std::endl;
        return;
      }
    }
    else
    {
      if (pandora::STATUS_CODE_SUCCESS != PandoraContentApi::GetList(*this, listName, pPfoList))
      {
        if (PandoraContentApi::GetSettings(*this)->ShouldDisplayAlgorithmInfo())
          //std::cout << "VisualMonitoringAlgorithm: pfo list " << listName << " unavailable." << std::endl;
        return;
      }
    }

#ifdef MONITORING
    if(this->GetPandora().GetSettings()->IsMonitoringEnabled())
    {
      if(m_showCurrentConnectors && m_connectorLevel == "pfos")
      {
        TEveElement *pParentElement = pandora_monitoring::PandoraMonitoring::GetInstance(this->GetPandora())->VisualizeParticleFlowObjects(pPfoList, listName.empty() ? "CurrentPfos" : listName.c_str(), 0,
            (m_hitColors.find("particleid") != std::string::npos) ? ::AUTOID :
                (m_hitColors.find("iterate") != std::string::npos ? ::AUTOITER :
                    (m_hitColors.find("energy") != std::string::npos ? ::AUTOENERGY :
                        AUTO)), m_showPfoVertices, m_showPfoHierarchy);

        TEveElement *pConnectorDirectory = new TEveElementList();
        pConnectorDirectory->SetElementNameTitle("PfoConnectors", "PfoConnectors");
        pParentElement->AddElement(pConnectorDirectory);

        for(pandora::PfoList::const_iterator pfoIter = pPfoList->begin(), pfoEndIter = pPfoList->end() ;
            pfoEndIter != pfoIter ; ++pfoIter)
        {
          const pandora::Pfo *const pPfo = *pfoIter;

          const pandora::ClusterList &pfoClusterList(pPfo->GetClusterList());

          for(pandora::ClusterList::const_iterator iter = pfoClusterList.begin(), endIter = pfoClusterList.end() ;
              endIter != iter ; ++iter)
          {
            pandora::CaloHitList clusterHits;
            (*iter)->GetOrderedCaloHitList().FillCaloHitList(clusterHits);

            this->VisualizeConnectors(&clusterHits, listName.empty() ? "CurrentPfoConnectors" : (listName+"Connectors").c_str(), pConnectorDirectory,
                (m_hitColors.find("particleid") != std::string::npos) ? ::AUTOID :
                    (m_hitColors.find("iterate") != std::string::npos) ? ::AUTOITER :
                        (m_hitColors.find("energy") != std::string::npos) ? ::AUTOENERGY : ::AUTO);
          }
        }
      }
      else
      {
        PANDORA_MONITORING_API(VisualizeParticleFlowObjects(this->GetPandora(), pPfoList, listName.empty() ? "CurrentPfos" : listName.c_str(),
            (m_hitColors.find("particleid") != std::string::npos) ? ::AUTOID :
                (m_hitColors.find("iterate") != std::string::npos ? ::AUTOITER :
                    (m_hitColors.find("energy") != std::string::npos ? ::AUTOENERGY :
                        AUTO)), m_showPfoVertices, m_showPfoHierarchy));
      }
    }
#endif
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void VisualMonitoringAlgorithm::VisualizeVertexList(const std::string &listName) const
  {
    const pandora::VertexList *pVertexList = NULL;

    if (listName.empty())
    {
      if (pandora::STATUS_CODE_SUCCESS != PandoraContentApi::GetCurrentList(*this, pVertexList))
      {
        if (PandoraContentApi::GetSettings(*this)->ShouldDisplayAlgorithmInfo())
          //std::cout << "VisualMonitoringAlgorithm: current vertex list unavailable." << std::endl;
        return;
      }
    }
    else
    {
      if (pandora::STATUS_CODE_SUCCESS != PandoraContentApi::GetList(*this, listName, pVertexList))
      {
        if (PandoraContentApi::GetSettings(*this)->ShouldDisplayAlgorithmInfo())
          //std::cout << "VisualMonitoringAlgorithm: vertex list " << listName << " unavailable." << std::endl;
        return;
      }
    }

    // Filter vertex list
    pandora::VertexList vertexList;

    for (pandora::VertexList::const_iterator iter = pVertexList->begin(), iterEnd = pVertexList->end(); iter != iterEnd; ++iter)
    {
      const pandora::Vertex *const pVertex = *iter;

      if (!m_showOnlyAvailable || pVertex->IsAvailable())
        vertexList.push_back(pVertex);
    }

    PANDORA_MONITORING_API(VisualizeVertices(this->GetPandora(), &vertexList, listName.empty() ? "CurrentVertices" : listName.c_str(), ::AUTO));
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

#ifdef MONITORING
  void VisualMonitoringAlgorithm::VisualizeConnectors(const pandora::CaloHitList *const pCaloHitList, std::string name, TEveElement* parent, ::Color color) const
  {
    if(this->GetPandora().GetSettings()->IsMonitoringEnabled())
    {
      if(NULL == parent)
        return;

	  ConnectorList clusterConnectorList;

      for(pandora::CaloHitList::const_iterator iter = pCaloHitList->begin() , endIter = pCaloHitList->end() ;
          endIter != iter ; ++iter)
      {
        const arbor_content::CaloHit *const pCaloHit = dynamic_cast<const arbor_content::CaloHit *const>(*iter);

        if(NULL == pCaloHit)
          continue;

        const ConnectorList &connectorList(ArborContentApi::GetConnectorList(pCaloHit, FORWARD_DIRECTION));

        for(ConnectorList::const_iterator connectorIter = connectorList.begin(), connectorEndIter = connectorList.end() ;
            connectorEndIter != connectorIter ; ++connectorIter)
        {
          const Connector *const pConnector = *connectorIter;
		  clusterConnectorList.insert(pConnector);
		}
	  }

	  if(clusterConnectorList.empty()) return;

      TEveElement *pConnectorListElement = new TEveElementList();
      const std::string connectorListTitle(name.empty() ? "Connectors" : name);

      static int connectorColorIter = ::RED;
      if (++connectorColorIter >= ::AUTO)
        connectorColorIter = ::RED;

      if(color == ::AUTOITER)
      {
        color = ::Color(connectorColorIter);
      }
      else if(color == AUTO)
      {
        color = ::RED;
      }

      pConnectorListElement->SetElementNameTitle( connectorListTitle.c_str(), connectorListTitle.c_str() );
      //pConnectorListElement->SetRnrSelf(false);
      //pConnectorListElement->SetRnrChildren(false);
      pConnectorListElement->SetMainColor(GetROOTColor(color));


      for(ConnectorList::const_iterator connectorIter = clusterConnectorList.begin(), connectorEndIter = clusterConnectorList.end() ;
            connectorEndIter != connectorIter ; ++connectorIter)
	  {
          const Connector *const pConnector = *connectorIter;

          const CaloHit *const pCaloHitFrom = pConnector->GetFrom();
          const CaloHit *const pCaloHitTo = pConnector->GetTo();

          pandora::CartesianVector fromPosition(pCaloHitFrom->GetPositionVector());
          pandora::CartesianVector toPosition(pCaloHitTo->GetPositionVector());
          pandora::CartesianVector differenceVector(toPosition - fromPosition);

          const float scalingFactor(0.1f);

          TEveArrow *pConnectorArrow = new TEveArrow (
              differenceVector.GetX()*scalingFactor,
              differenceVector.GetY()*scalingFactor,
              differenceVector.GetZ()*scalingFactor,
              fromPosition.GetX()*scalingFactor,
              fromPosition.GetY()*scalingFactor,
              fromPosition.GetZ()*scalingFactor);

          pConnectorArrow->SetMainColor(GetROOTColor(color));
          pConnectorArrow->SetPickable(true);
          pConnectorListElement->AddElement(pConnectorArrow);
      }

      parent->AddElement(pConnectorListElement);
      gEve->Redraw3D();
    }
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  void VisualMonitoringAlgorithm::VisualizeClusterDirection(const pandora::Cluster* const pCluster, std::string /* name */, TEveElement* parent, ::Color color) const
  {
    if(this->GetPandora().GetSettings()->IsMonitoringEnabled())
    {
      if(NULL == parent)
        return;
	
	  const arbor_content::ArborCluster* const pArborCluster = dynamic_cast<const arbor_content::ArborCluster *const>(pCluster);

	  pandora::CartesianVector clusterCOG = pArborCluster->GetIntercept();
	  pandora::CartesianVector clusterAxis = pArborCluster->GetAxis();

	  //std::cout << "=================================== pArborCluster: " << pArborCluster << ", " << clusterCOG.GetX()
		//  << ", " << clusterAxis.GetX() << std::endl;

	  //if(clusterConnectorList.empty()) return;

      //const std::string connectorListTitle(name.empty() ? "Connectors" : name);

      //pConnectorListElement->SetElementNameTitle( connectorListTitle.c_str(), connectorListTitle.c_str() );
      //pConnectorListElement->SetRnrSelf(false);
      //pConnectorListElement->SetRnrChildren(false);
      //pConnectorListElement->SetMainColor(GetROOTColor(color));

      pandora::CartesianVector fromPosition(clusterCOG);
      pandora::CartesianVector differenceVector(clusterAxis);

	  if( !(differenceVector.GetMagnitude() > 1.e-3 && fromPosition.GetMagnitude() > 0) )
	  {
		  return;
	  }

      const float scalingFactor(0.1f);
      const float scalingFactorDir(10.f);

      TEveArrow *pConnectorArrow = new TEveArrow (
          differenceVector.GetX()*scalingFactorDir,
          differenceVector.GetY()*scalingFactorDir,
          differenceVector.GetZ()*scalingFactorDir,
          fromPosition.GetX()*scalingFactor,
          fromPosition.GetY()*scalingFactor,
          fromPosition.GetZ()*scalingFactor);

      std::stringstream sstr;
	  sstr << "Direction of cluster, Ehad=" << pCluster->GetHadronicEnergy();

	  std::string directionTitle(sstr.str());

      pConnectorArrow->SetMainColor(GetROOTColor(color));
	  pConnectorArrow->SetElementNameTitle("Direction", directionTitle.c_str());
      pConnectorArrow->SetPickable(true);

      parent->AddElement(pConnectorArrow);
      gEve->Redraw3D();
    }
  }

  //------------------------------------------------------------------------------------------------------------------------------------------
  EColor VisualMonitoringAlgorithm::GetROOTColor(::Color color) const
  {
    switch (color)
    {
    COLOR_TABLE(GET_ROOT_COLOR)
    default : return kGray;
    }
  }
#endif

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode VisualMonitoringAlgorithm::ReadSettings(const pandora::TiXmlHandle xmlHandle)
  {
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "ShowCurrentConnectors", m_showCurrentConnectors));

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "ConnectorLevel", m_connectorLevel));
    std::transform(m_connectorLevel.begin(), m_connectorLevel.end(), m_connectorLevel.begin(), ::tolower);

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "ShowCurrentMCParticles", m_showCurrentMCParticles));

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadVectorOfValues(xmlHandle,
        "MCParticleListNames", m_mcParticleListNames));

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "ShowCurrentCaloHits", m_showCurrentCaloHits));

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadVectorOfValues(xmlHandle,
        "CaloHitListNames", m_caloHitListNames));

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "ShowCurrentTracks", m_showCurrentTracks));

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadVectorOfValues(xmlHandle,
        "TrackListNames", m_trackListNames));

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "ShowCurrentClusters", m_showCurrentClusters));

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadVectorOfValues(xmlHandle,
        "ClusterListNames", m_clusterListNames));

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "ShowCurrentPfos", m_showCurrentPfos));

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadVectorOfValues(xmlHandle,
        "PfoListNames", m_pfoListNames));

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "ShowCurrentVertices", m_showCurrentVertices));

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadVectorOfValues(xmlHandle,
        "VertexListNames", m_vertexListNames));

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "DisplayEvent", m_displayEvent));

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "ShowDetector", m_showDetector));

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "DetectorView", m_detectorView));
    std::transform(m_detectorView.begin(), m_detectorView.end(), m_detectorView.begin(), ::tolower);

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "ShowOnlyAvailable", m_showOnlyAvailable));

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "ShowAssociatedTracks", m_showAssociatedTracks));

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "HitColors", m_hitColors));
    std::transform(m_hitColors.begin(), m_hitColors.end(), m_hitColors.begin(), ::tolower);

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "ThresholdEnergy", m_thresholdEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "TransparencyThresholdE", m_transparencyThresholdE));

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "EnergyScaleThresholdE", m_energyScaleThresholdE));

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "ShowPfoVertices", m_showPfoVertices));

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "ShowPfoHierarchy", m_showPfoHierarchy));
      
    m_darkBackground = true;
      
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "DarkBackground", m_darkBackground));

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadVectorOfValues(xmlHandle,
        "SuppressMCParticles", m_suppressMCParticles));

    for (pandora::StringVector::iterator iter = m_suppressMCParticles.begin(), iterEnd = m_suppressMCParticles.end(); iter != iterEnd; ++iter)
    {
      const std::string pdgEnergy(*iter);
      pandora::StringVector pdgEnergySeparated;
      const std::string delimiter = ":";
      pandora::XmlHelper::TokenizeString(pdgEnergy, pdgEnergySeparated, delimiter);

      if (pdgEnergySeparated.size() != 2)
        return pandora::STATUS_CODE_INVALID_PARAMETER;

      int pdgCode(0);
      float energy(0.f);

      if (!pandora::StringToType(pdgEnergySeparated.at(0), pdgCode) || !pandora::StringToType(pdgEnergySeparated.at(1), energy))
        return pandora::STATUS_CODE_INVALID_PARAMETER;

      m_particleSuppressionMap.insert(PdgCodeToEnergyMap::value_type(pdgCode, energy));
    }

    return pandora::STATUS_CODE_SUCCESS;
  }


} 

