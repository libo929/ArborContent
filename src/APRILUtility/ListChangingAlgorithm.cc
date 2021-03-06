  /// \file ListChangingAlgorithm.cc
/*
 *
 * ListChangingAlgorithm.cc source template automatically generated by a class generator
 * Creation date : sam. mars 21 2015
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
 * @author John Marshall
 */

#include "Pandora/AlgorithmHeaders.h"

#include "APRILUtility/ListChangingAlgorithm.h"

namespace april_content
{

pandora::StatusCode ListChangingAlgorithm::Run()
{
    if (!m_caloHitListName.empty())
    {
        const pandora::StatusCode statusCode(PandoraContentApi::ReplaceCurrentList<pandora::CaloHit>(*this, m_caloHitListName));

        if ((pandora::STATUS_CODE_SUCCESS != statusCode) && (pandora::STATUS_CODE_NOT_FOUND != statusCode))
            return statusCode;

        if (pandora::STATUS_CODE_NOT_FOUND == statusCode)
            std::cout << "ListChangingAlgorithm: calohit list not found " << m_caloHitListName << std::endl;
    }

    if (!m_clusterListName.empty())
    {
        const pandora::StatusCode statusCode(PandoraContentApi::ReplaceCurrentList<pandora::Cluster>(*this, m_clusterListName));

        if ((pandora::STATUS_CODE_SUCCESS != statusCode) && (pandora::STATUS_CODE_NOT_FOUND != statusCode))
            return statusCode;

        if (pandora::STATUS_CODE_NOT_FOUND == statusCode)
            std::cout << "ListChangingAlgorithm: cluster list not found " << m_clusterListName << std::endl;
    }

    if (!m_vertexListName.empty())
    {
        const pandora::StatusCode statusCode(PandoraContentApi::ReplaceCurrentList<pandora::Vertex>(*this, m_vertexListName));

        if ((pandora::STATUS_CODE_SUCCESS != statusCode) && (pandora::STATUS_CODE_NOT_FOUND != statusCode))
            return statusCode;

        if (pandora::STATUS_CODE_NOT_FOUND == statusCode)
            std::cout << "ListChangingAlgorithm: vertex list not found " << m_vertexListName << std::endl;
    }

    if (!m_pfoListName.empty())
    {
        const pandora::StatusCode statusCode(PandoraContentApi::ReplaceCurrentList<pandora::Pfo>(*this, m_pfoListName));

        if ((pandora::STATUS_CODE_SUCCESS != statusCode) && (pandora::STATUS_CODE_NOT_FOUND != statusCode))
            return statusCode;

        if (pandora::STATUS_CODE_NOT_FOUND == statusCode)
            std::cout << "ListChangingAlgorithm: pfo list not found " << m_pfoListName << std::endl;
    }

    return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode ListChangingAlgorithm::ReadSettings(const pandora::TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle, "CaloHitListName", m_caloHitListName));
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle, "ClusterListName", m_clusterListName));
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle, "VertexListName", m_vertexListName));
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle, "PfoListName", m_pfoListName));

    return pandora::STATUS_CODE_SUCCESS;
}

}
