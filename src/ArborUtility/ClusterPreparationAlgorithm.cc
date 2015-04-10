  /// \file ListChangingAlgorithm.cc
/*
 *
 * ListChangingAlgorithm.cc source template automatically generated by a class generator
 * Creation date : sam. mars 21 2015
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
 * @author John Marshall
 */

#include "Pandora/AlgorithmHeaders.h"

#include "ArborUtility/ClusterPreparationAlgorithm.h"

using namespace pandora;

namespace arbor_content
{

StatusCode ClusterPreparationAlgorithm::Run()
{
    // Merge all candidate clusters in specified input lists, to create e.g. final pfo cluster list
    for (StringVector::const_iterator iter = m_candidateListNames.begin(), iterEnd = m_candidateListNames.end(); iter != iterEnd; ++iter)
    {
        const ClusterList *pClusterList = NULL;

        if (STATUS_CODE_SUCCESS != PandoraContentApi::GetList(*this, *iter, pClusterList))
            continue;

        ClusterList clustersToSave;

        for (ClusterList::const_iterator clusterIter = pClusterList->begin(), clusterIterEnd = pClusterList->end(); clusterIter != clusterIterEnd; ++clusterIter)
        {
            if ((*clusterIter)->IsAvailable())
                clustersToSave.insert(*clusterIter);
        }

        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_INITIALIZED, !=, PandoraContentApi::SaveList(*this,
            *iter, m_mergedCandidateListName, clustersToSave));
    }

    // Save the merged list and set it to be the current list for future algorithms
    if (STATUS_CODE_SUCCESS != PandoraContentApi::ReplaceCurrentList<Cluster>(*this, m_mergedCandidateListName))
    {
        std::cout << "ClusterPreparationAlgorithm: empty cluster list for subsequent pfo construction." << std::endl;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::DropCurrentList<Cluster>(*this));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterPreparationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "CandidateListNames", m_candidateListNames));

    if (m_candidateListNames.empty())
        return STATUS_CODE_INVALID_PARAMETER;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "MergedCandidateListName", m_mergedCandidateListName));

    return STATUS_CODE_SUCCESS;
}

}
