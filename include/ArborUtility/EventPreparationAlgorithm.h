  /// \file EventPreparationAlgorithm.h
/*
 *
 * EventPreparationAlgorithm.h header template automatically generated by a class generator
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


#ifndef ARBOR_EVENT_PREPARATION_ALGORITHM_H
#define ARBOR_EVENT_PREPARATION_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace arbor_content
{

/**
 *  @brief  EventPreparationAlgorithm class
 */
class EventPreparationAlgorithm : public pandora::Algorithm
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

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    std::string     m_outputTrackListName;          ///< The output track list name
    std::string     m_outputCaloHitListName;        ///< The output calo hit list name
    std::string     m_outputMuonCaloHitListName;    ///< The output muon calo hit list name

    std::string     m_replacementTrackListName;     ///< The replacement track list name
    std::string     m_replacementCaloHitListName;   ///< The replacement calo hit list name
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *EventPreparationAlgorithm::Factory::CreateAlgorithm() const
{
    return new EventPreparationAlgorithm();
}

}

#endif // #ifndef ARBOR_EVENT_PREPARATION_ALGORITHM_H