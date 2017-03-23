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

namespace arbor_content
{

pandora::StatusCode ClusterPreparationAlgorithm::Run()
{
    // Merge all candidate clusters in specified input lists, to create e.g. final pfo cluster list
    for (pandora::StringVector::const_iterator iter = m_candidateListNames.begin(), iterEnd = m_candidateListNames.end(); iter != iterEnd; ++iter)
    {
        const pandora::ClusterList *pClusterList = NULL;

        if (pandora::STATUS_CODE_SUCCESS != PandoraContentApi::GetList(*this, *iter, pClusterList))
            continue;

        pandora::ClusterList clustersToSave;

		//std::cout << "the name of clusters: " << *iter << ", size: " << pClusterList->size() << std::endl;
        for (pandora::ClusterList::const_iterator clusterIter = pClusterList->begin(), clusterIterEnd = pClusterList->end(); clusterIter != clusterIterEnd; ++clusterIter)
        {
            if ((*clusterIter)->IsAvailable())
                clustersToSave.insert(*clusterIter);
			//else
				//std::cout << "a cluster is not available..." << std::endl;
        }

        PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_INITIALIZED, !=, PandoraContentApi::SaveList(*this,
            *iter, m_mergedCandidateListName, clustersToSave));
    }

    // Save the merged list and set it to be the current list for future algorithms
    if (pandora::STATUS_CODE_SUCCESS != PandoraContentApi::ReplaceCurrentList<pandora::Cluster>(*this, m_mergedCandidateListName))
    {
        std::cout << "ClusterPreparationAlgorithm: empty cluster list for subsequent pfo construction." << std::endl;
        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::DropCurrentList<pandora::Cluster>(*this));
    }
#if 0
    const pandora::ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));
	std::cout << "check the pClusterList: " << pClusterList->size() << std::endl;
#endif

    return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode ClusterPreparationAlgorithm::ReadSettings(const pandora::TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ReadVectorOfValues(xmlHandle,
        "CandidateListNames", m_candidateListNames));

    if (m_candidateListNames.empty())
        return pandora::STATUS_CODE_INVALID_PARAMETER;

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "MergedCandidateListName", m_mergedCandidateListName));

    return pandora::STATUS_CODE_SUCCESS;
}

}
