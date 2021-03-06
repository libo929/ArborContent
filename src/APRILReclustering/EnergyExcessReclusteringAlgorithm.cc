/// \file EnergyExcessReclusteringAlgorithm.cc
/*
 *
 * EnergyExcessReclusteringAlgorithm.cc source template automatically generated by a class generator
 * Creation date : mar. d�c. 8 2015
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


#include "APRILReclustering/EnergyExcessReclusteringAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"

#include "APRILApi/APRILContentApi.h"
#include "APRILHelpers/ReclusterHelper.h"
#include "APRILHelpers/ClusterHelper.h"
#include "APRILHelpers/SortingHelper.h"

namespace april_content
{

  pandora::StatusCode EnergyExcessReclusteringAlgorithm::Run()
  {
    // start by recalculating track-cluster association
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_trackClusterAssociationAlgName));

    const pandora::ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

    if(pClusterList->empty())
      return pandora::STATUS_CODE_SUCCESS;

    pandora::ClusterVector clusterVector(pClusterList->begin(), pClusterList->end());
    std::sort(clusterVector.begin(), clusterVector.end(), SortingHelper::SortByTrackClusterCompatibility(&this->GetPandora()));

    for(pandora::ClusterVector::iterator iter = clusterVector.begin(), endIter = clusterVector.end() ;
        endIter != iter ; ++iter)
    {
      const pandora::Cluster *const pCluster = *iter;
      const pandora::TrackList &trackList(pCluster->GetAssociatedTrackList());

      // need exactly one track
      if(trackList.empty() || trackList.size() > 1)
        continue;

      // negative chi means missing energy in the cluster
      const float chi(ReclusterHelper::GetTrackClusterCompatibility(this->GetPandora(), pCluster, trackList));

      // check for chi2, energy excess and asymmetric cluster
      if( (chi*chi < m_minChi2ToRunReclustering || chi < 0.f)  || ((*trackList.begin())->GetEnergyAtDca() < m_minTrackMomentum))
        continue;

      // check for clusters that leave the detector
      if(ClusterHelper::IsClusterLeavingDetector(this->GetPandora(), pCluster))
        continue;

      // prepare clusters and tracks for reclustering
      pandora::ClusterList reclusterClusterList;
      reclusterClusterList.push_back(pCluster);
      pandora::TrackList reclusterTrackList(trackList);

      // initialize reclustering
      std::string originalClusterListName;
      PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, APRILContentApi::InitializeReclustering(*this,
          reclusterTrackList, reclusterClusterList, originalClusterListName));

      float bestChi(chi);
      std::string bestReclusterClusterListName(originalClusterListName);

      for(pandora::StringVector::const_iterator clusteringAlgIter = m_clusteringAlgorithmList.begin(), endClusteringAlgIter = m_clusteringAlgorithmList.end() ;
          endClusteringAlgIter != clusteringAlgIter ; ++clusteringAlgIter)
      {
        const pandora::ClusterList *pReclusterClusterList = NULL;
        std::string reclusterClusterListName;
        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, APRILContentApi::RunReclusteringAlgorithm(*this,
            *clusteringAlgIter, pReclusterClusterList, reclusterClusterListName));

        if(pReclusterClusterList->empty())
          continue;

        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this,
            m_associationAlgorithmName));

        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, APRILContentApi::PostRunReclusteringAlgorithm(*this, reclusterClusterListName));

        // run monitoring algorithm if provided
        if(!m_monitoringAlgorithmName.empty())
        {
          PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this,
              m_monitoringAlgorithmName));
        }

        bool shouldStopReclustering = false;

        // find the cluster associated with original track and look at the compatibility
        for(pandora::ClusterList::const_iterator reclusterIter = pReclusterClusterList->begin(), reclusterEndIter = pReclusterClusterList->end() ;
            reclusterEndIter != reclusterIter ; ++reclusterIter)
        {
          const pandora::Cluster *const pReclusterCluster = *reclusterIter;
          const pandora::TrackList &newTrackList(pReclusterCluster->GetAssociatedTrackList());

          if(newTrackList.empty())
            continue;

          const float newChi = ReclusterHelper::GetTrackClusterCompatibility(this->GetPandora(), pReclusterCluster, newTrackList);

          // if we see an improvement on separation update the best list
          if(newChi < bestChi && newChi*newChi < bestChi*bestChi/* && newClusterSymmetry < bestClusterSymmetry*/)
          {
            bestChi = newChi;
            bestReclusterClusterListName = reclusterClusterListName;

            if(newChi*newChi < m_maxChi2ToStopReclustering)
              shouldStopReclustering = true;
          }

          break;
        }

        if(shouldStopReclustering)
          break;
      }

      // Recreate track-cluster associations for chosen recluster candidates
      PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::TemporarilyReplaceCurrentList<pandora::Cluster>(*this, bestReclusterClusterListName));
      PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_trackClusterAssociationAlgName));

      PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, APRILContentApi::EndReclustering(*this, bestReclusterClusterListName));

      (*iter) = NULL;
    }

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode EnergyExcessReclusteringAlgorithm::ReadSettings(const pandora::TiXmlHandle xmlHandle)
  {
    m_minChi2ToRunReclustering = 1.8f;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "MinChi2ToRunReclustering", m_minChi2ToRunReclustering));

    m_maxChi2ToStopReclustering = 0.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "MaxChi2ToStopReclustering", m_maxChi2ToStopReclustering));

    m_minTrackMomentum = 0.8f;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "MinTrackMomentum", m_minTrackMomentum));

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ProcessAlgorithmList(*this, xmlHandle,
        "clusteringAlgorithms", m_clusteringAlgorithmList));

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ProcessAlgorithm(*this, xmlHandle,
        "ClusterAssociation", m_associationAlgorithmName));

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ProcessAlgorithm(*this, xmlHandle,
        "TrackClusterAssociation", m_trackClusterAssociationAlgName));

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ProcessAlgorithm(*this, xmlHandle,
        "Monitoring", m_monitoringAlgorithmName));

    return pandora::STATUS_CODE_SUCCESS;
  }

} 

