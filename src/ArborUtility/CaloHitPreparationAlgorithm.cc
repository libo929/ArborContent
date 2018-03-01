/// \file CaloHitPreparationAlgorithm.cc
/*
 *
 * CaloHitPreparationAlgorithm.cc source template automatically generated by a class generator
 * Creation date : jeu. juin 2 2016
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


#include "ArborUtility/CaloHitPreparationAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"
#include "ArborApi/ArborContentApi.h"
#include "ArborObjects/CaloHit.h"
#include "ArborHelpers/GeometryHelper.h"

namespace arbor_content
{

  pandora::StatusCode CaloHitPreparationAlgorithm::Run()
  {
    const pandora::CaloHitList *pCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pCaloHitList));

	std::cout << "****************** pCaloHitList size: " << pCaloHitList->size() << std::endl;

    if(pCaloHitList->empty())
      return pandora::STATUS_CODE_SUCCESS;

    const pandora::GeometryManager *const pGeometryManager(PandoraContentApi::GetGeometry(*this));

    pandora::OrderedCaloHitList orderedCaloHitList;
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, orderedCaloHitList.Add(*pCaloHitList));

	int nHits(0);
    for(pandora::OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), endIter = orderedCaloHitList.end() ;
        endIter != iter ; ++iter)
    {
      const unsigned int pseudoLayer(iter->first);

	  //std::cout << "pseudoLayer: " << pseudoLayer << std::endl;

      for(pandora::CaloHitList::const_iterator hitIter = iter->second->begin(), hitEndIter = iter->second->end() ;
          hitEndIter != hitIter ; ++hitIter)
      {
        const arbor_content::CaloHit *const pCaloHit(dynamic_cast<const arbor_content::CaloHit *>(*hitIter));

        if(NULL == pCaloHit)
		{
			//std::cout << "pointer is null..."  << std::endl;
          return pandora::STATUS_CODE_FAILURE;
		}

		++nHits;
        const float energy(pCaloHit->GetHitType() == pandora::ECAL ? pCaloHit->GetElectromagneticEnergy() : pCaloHit->GetHadronicEnergy());

        const pandora::CartesianVector position(pCaloHit->GetPositionVector());
        const pandora::Granularity granularity(pGeometryManager->GetHitTypeGranularity(pCaloHit->GetHitType()));
        const float densityMaxPositionDifference(granularity <= pandora::FINE ? m_densityMaxSeparationDistanceFine : m_densityMaxSeparationDistanceCoarse);
        const float surroundingEnergyMaxPositionDifference(granularity <= pandora::FINE ? m_surroundingEnergyMaxSeparationDistanceFine : m_surroundingEnergyMaxSeparationDistanceCoarse);

        ArborContentApi::CaloHitMetadata caloHitMetadata;

        float density(0.f), surroundingEnergy(energy);
        unsigned int densityNeighbors(0);

        for(pandora::CaloHitList::const_iterator hitIter2 = iter->second->begin(), hitEndIter2 = iter->second->end() ;
            hitEndIter2 != hitIter2 ; ++hitIter2)
        {
          const pandora::CaloHit *const pCaloHit2(*hitIter2);

		  //std::cout << "loop..." << std::endl;

          if(pCaloHit == pCaloHit2)
            continue;

          const pandora::CartesianVector position2(pCaloHit2->GetPositionVector());

          // WTF ??
          // N.B. how about the case of hits in different time but the same position ?
          if(position == position2)
            continue;

          const float energy2(pCaloHit2->GetHitType() == pandora::ECAL ? pCaloHit2->GetElectromagneticEnergy() : pCaloHit2->GetHadronicEnergy());
          const float positionDifference((position2-position).GetMagnitude());

          if(positionDifference < densityMaxPositionDifference)
          {
            density += positionDifference/densityMaxPositionDifference;
            densityNeighbors++;
          }

          if(positionDifference < surroundingEnergyMaxPositionDifference)
            surroundingEnergy += energy2;
        }

        density /= (densityNeighbors + 1);

        caloHitMetadata.m_density = density;
        caloHitMetadata.m_surroundingEnergy= surroundingEnergy;

        if(pCaloHit->GetHitType() == pandora::MUON)
        {
          caloHitMetadata.m_isPossibleMip = true;
          PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ArborContentApi::AlterMetadata(*this, pCaloHit, caloHitMetadata));

          // continue -> never flag muon hits as isolated
          continue;
        }

        const float maxSeparationDistance(granularity <= pandora::FINE ? m_isolationMaxSeparationDistanceFine : m_isolationMaxSeparationDistanceCoarse);

        const unsigned int startLayer(pseudoLayer < m_isolationPseudoLayerDifference ? 0 : pseudoLayer-m_isolationPseudoLayerDifference);
        const unsigned int endLayer(pseudoLayer+m_isolationPseudoLayerDifference);

        unsigned int nNearbyHits(0);
        bool isIsolated = true;

        for(unsigned int iLayer = startLayer ; iLayer <= endLayer ; ++iLayer)
        {
          pandora::OrderedCaloHitList::const_iterator iIter = orderedCaloHitList.find(iLayer);

          if(orderedCaloHitList.end() == iIter)
            continue;

          if(!isIsolated)
            break;

          for(pandora::CaloHitList::const_iterator hitIter2 = iIter->second->begin(), hitEndIter2 = iIter->second->end() ;
              hitEndIter2 != hitIter2 ; ++hitIter2)
          {
            const pandora::CaloHit *const pCaloHit2(*hitIter2);

            if(pCaloHit == pCaloHit2)
              continue;

            if(pCaloHit2->GetHitType() == pandora::MUON)
              continue;

            const pandora::CartesianVector position2(pCaloHit2->GetPositionVector());

            // WTF ??
            if(position == position2)
              continue;

            const pandora::CartesianVector positionDifference(position2-position);
            const float openingAngle(position.GetOpeningAngle(positionDifference));
            const float sinOpeningAngle(std::sin(openingAngle));
            const float transverseDistance(positionDifference.GetMagnitude()*sinOpeningAngle);

            if(transverseDistance > maxSeparationDistance)
              continue;

            ++nNearbyHits;

            if(nNearbyHits > m_isolationMaxNNeighbors)
            {
              isIsolated = false;
              break;
            }
          }
        }

        caloHitMetadata.m_isIsolated = isIsolated;
        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ArborContentApi::AlterMetadata(*this, pCaloHit, caloHitMetadata));
      }
    }
	std::cout << "# of hits: " << nHits << std::endl;

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->FlagIsolatedHitsWithTracks(orderedCaloHitList));

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode CaloHitPreparationAlgorithm::FlagIsolatedHitsWithTracks(const pandora::OrderedCaloHitList &orderedCaloHitList)
  {
    const pandora::TrackList *pTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pTrackList));

    if(pTrackList->empty())
      return pandora::STATUS_CODE_SUCCESS;

    const pandora::GeometryManager *const pGeometryManager(PandoraContentApi::GetGeometry(*this));
    const pandora::BFieldPlugin *const pBFieldPlugin(PandoraContentApi::GetPlugins(*this)->GetBFieldPlugin());

    const float innerBField(pBFieldPlugin->GetBField(pandora::CartesianVector(0.f, 0.f, 0.f)));

    for(pandora::TrackList::const_iterator trackIter = pTrackList->begin(), trackEndIter = pTrackList->end() ;
        trackEndIter != trackIter ; ++trackIter)
    {
      const pandora::Track *const pTrack(*trackIter);

      if(!pTrack->ReachesCalorimeter())
        continue;

      if (!pTrack->GetDaughterList().empty())
        continue;

      const pandora::CartesianVector enteringPoint(pTrack->GetTrackStateAtCalorimeter().GetPosition());
      const pandora::CartesianVector direction(pTrack->GetTrackStateAtCalorimeter().GetMomentum());

      const pandora::Helix helix(enteringPoint, direction, pTrack->GetCharge(), innerBField);

      for(pandora::OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), endIter = orderedCaloHitList.end() ;
          endIter != iter ; ++iter)
      {
        for(pandora::CaloHitList::const_iterator hitIter = iter->second->begin(), hitEndIter = iter->second->end() ;
            hitEndIter != hitIter ; ++hitIter)
        {
          const pandora::CaloHit *const pCaloHit(*hitIter);

          if(pCaloHit->GetHitType() == pandora::MUON)
            continue;

          if(!pCaloHit->IsIsolated())
            continue;

          const pandora::Granularity granularity(pGeometryManager->GetHitTypeGranularity(pCaloHit->GetHitType()));
          const float maxDistanceToHelix(granularity <= pandora::FINE ? m_isolationMaxDistanceToHelixFine : m_isolationMaxDistanceToHelixCoarse);

          pandora::CartesianVector projectionOnHelix(0.f, 0.f, 0.f);

          if(pandora::STATUS_CODE_SUCCESS != GeometryHelper::GetProjectionOnHelix(helix, pCaloHit->GetPositionVector(), projectionOnHelix))
            continue;

          const float distanceToHelix((pCaloHit->GetPositionVector()-projectionOnHelix).GetMagnitude());

          // un-flag isolated hit if near helix
          if(distanceToHelix < maxDistanceToHelix)
          {
            object_creation::CaloHitMetadata caloHitMetadata;
            caloHitMetadata.m_isIsolated = false;

            //PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::AlterMetadata(*this, pCaloHit, caloHitMetadata));
            PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::CaloHit::AlterMetadata(*this, pCaloHit, caloHitMetadata));
          }
        }
      }
    }

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode CaloHitPreparationAlgorithm::ReadSettings(const pandora::TiXmlHandle xmlHandle)
  {
    m_isolationMaxNNeighbors = 1;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "IsolationMaxNNeighbors", m_isolationMaxNNeighbors));

    m_isolationPseudoLayerDifference = 2;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "IsolationPseudoLayerDifference", m_isolationPseudoLayerDifference));

    m_isolationMaxSeparationDistanceFine = 25.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "IsolationMaxSeparationDistanceFine", m_isolationMaxSeparationDistanceFine));

    m_isolationMaxSeparationDistanceCoarse = 40.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "IsolationMaxSeparationDistanceCoarse", m_isolationMaxSeparationDistanceCoarse));

    m_isolationMaxDistanceToHelixFine = 10.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "IsolationMaxDistanceToHelixFine", m_isolationMaxDistanceToHelixFine));

    m_isolationMaxDistanceToHelixCoarse = 20.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "IsolationMaxDistanceToHelixCoarse", m_isolationMaxDistanceToHelixCoarse));

    m_densityMaxSeparationDistanceFine = 10.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "DensityMaxSeparationDistanceFine", m_densityMaxSeparationDistanceFine));

    m_densityMaxSeparationDistanceCoarse = 20.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "DensityMaxSeparationDistanceCoarse", m_densityMaxSeparationDistanceCoarse));

    m_surroundingEnergyMaxSeparationDistanceFine = 10.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "SurroundingEnergyMaxSeparationDistanceFine", m_surroundingEnergyMaxSeparationDistanceFine));

    m_surroundingEnergyMaxSeparationDistanceCoarse = 20.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "SurroundingEnergyMaxSeparationDistanceCoarse", m_surroundingEnergyMaxSeparationDistanceCoarse));

    return pandora::STATUS_CODE_SUCCESS;
  }


} 
