  /// \file PerfectPfoCreationAlgorithm.cc
/*
 *
 * PerfectPfoCreationAlgorithm.cc source template automatically generated by a class generator
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
#include "Api/PandoraContentApi.h"

#include "ArborCheating/PerfectPfoCreationAlgorithm.h"
#include "ArborHelpers/ClusterHelper.h"
#include "ArborHelpers/CaloHitHelper.h"
#include "ArborHelpers/ReclusterHelper.h"

#include "ArborHelpers/HistogramHelper.h"


namespace arbor_content
{

PerfectPfoCreationAlgorithm::PerfectPfoCreationAlgorithm()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PerfectPfoCreationAlgorithm::Run()
{
    const pandora::PfoList *pPfoList = NULL; std::string pfoListName;
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::CreateTemporaryListAndSetCurrent(*this, pPfoList, pfoListName));

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->CreateTrackBasedPfos());

    if (!pPfoList->empty())
    {
        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList<pandora::Pfo>(*this, m_outputPfoListName));
        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<pandora::Pfo>(*this, m_outputPfoListName));
    }

    return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PerfectPfoCreationAlgorithm::TrackCollection(const pandora::MCParticle *const pPfoTarget, PfoParameters &pfoParameters) const
{
    const pandora::TrackList *pTrackList = NULL;
    PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pTrackList));
	//std::cout << "---track size: " << pTrackList->size() << std::endl;

    for (pandora::TrackList::const_iterator iter = pTrackList->begin(), iterEnd = pTrackList->end(); iter != iterEnd; ++iter)
    {
        try
        {
            const pandora::Track *const pTrack = *iter;
            const pandora::MCParticle *const pTrkMCParticle(pandora::MCParticleHelper::GetMainMCParticle(pTrack));
            const pandora::MCParticle *const pTrkPfoTarget(pTrkMCParticle->GetPfoTarget());

            if (pTrkPfoTarget != pPfoTarget)
                continue;

            pfoParameters.m_trackList.push_back(pTrack);

			std::cout << "found a track: " << pTrack << " for MCP: " << pTrkPfoTarget << std::endl;
        }
        catch (pandora::StatusCodeException &e)
        {
			//std::cout << e.ToString() << std::endl;
			// skip this event
			return pandora::STATUS_CODE_FAILURE;
        }
    }

    return pandora::STATUS_CODE_SUCCESS;
}

void PerfectPfoCreationAlgorithm::SetPfoParametersFromTracks(int &nTracksUsed, PfoParameters &pfoParameters) const
{
    if (!pfoParameters.m_trackList.empty())
    {
        int charge(0);
        float energyWithPionMass(0.f), energyWithElectronMass(0.f);
		pandora::CartesianVector momentum(0.f, 0.f, 0.f);
		pandora::ClusterList clusters;

        for (pandora::TrackList::const_iterator iter = pfoParameters.m_trackList.begin(), iterEnd = pfoParameters.m_trackList.end(); iter != iterEnd; ++iter)
        {
            const pandora::Track *const pTrack = *iter;

            if (!pTrack->CanFormPfo() && !pTrack->CanFormClusterlessPfo())
            {
                std::cout << " Drop track, E: " << pTrack->GetEnergyAtDca() << " cfp: " << pTrack->CanFormPfo() << " cfcp: " << pTrack->CanFormClusterlessPfo() << std::endl;
		std::cout << "ReachesCalorimeter: " << pTrack->ReachesCalorimeter() << std::endl;
                continue;
            }

            if (!pTrack->GetParentList().empty())
            {
                std::cout << " Drop track, E: " << pTrack->GetEnergyAtDca() << " nParents: " << pTrack->GetParentList().size() << std::endl;
                continue;
            }

            ++nTracksUsed;

            // ATTN Assume neutral track-based pfos represent pair-production
            const float electronMass(pandora::PdgTable::GetParticleMass(pandora::E_MINUS));

            charge += pTrack->GetCharge();
            momentum += pTrack->GetMomentumAtDca();
            energyWithPionMass += pTrack->GetEnergyAtDca();
            energyWithElectronMass += std::sqrt(electronMass * electronMass + pTrack->GetMomentumAtDca().GetMagnitudeSquared());

			// track-cluster
			if(pTrack->HasAssociatedCluster())
			{
				const pandora::Cluster* pCluster = pTrack->GetAssociatedCluster();

                if (PandoraContentApi::IsAvailable(*this, pCluster))
				{
					clusters.push_back(pCluster);
				}

				std::cout << "track: " << pTrack << " --- cluster: " << pCluster << std::endl;
				std::cout << "track energy: " << pTrack->GetEnergyAtDca() << ", cluster energy: " << pCluster->GetHadronicEnergy() 
					<< std::endl;
			}
        }

        if (0 == nTracksUsed)
            return;

		pfoParameters.m_clusterList = clusters;
        pfoParameters.m_charge = charge;
        pfoParameters.m_momentum = momentum;
        pfoParameters.m_particleId = (pfoParameters.m_charge.Get() == 0) ? pandora::PHOTON : (pfoParameters.m_charge.Get() < 0) ? pandora::PI_MINUS : pandora::PI_PLUS;
        pfoParameters.m_energy = (pfoParameters.m_particleId.Get() == pandora::PHOTON) ? energyWithElectronMass : energyWithPionMass;
        pfoParameters.m_mass = std::sqrt(std::max(pfoParameters.m_energy.Get() * pfoParameters.m_energy.Get() - pfoParameters.m_momentum.Get().GetDotProduct(pfoParameters.m_momentum.Get()), 0.f));
    }
}

/////////

// FIXME
float neutralPfoEnergy;

pandora::StatusCode PerfectPfoCreationAlgorithm::SetPfoParametersFromClusters() const
{
    const pandora::ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));
	std::cout << "check the cluster again: " << pClusterList->size() << std::endl;


	neutralPfoEnergy = 0.;

    // Examine clusters with no associated tracks to form neutral pfos
    for (pandora::ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        const pandora::Cluster *const pCluster = *iter;
        if (!PandoraContentApi::IsAvailable(*this, pCluster)) continue;

		//std::cout << "cluster: " << pCluster->GetAssociatedTrackList().size() << std::endl;

#if 0
        if (!pCluster->GetAssociatedTrackList().empty())
		{
			continue;

#if 0
			auto tracks = pCluster->GetAssociatedTrackList();
			bool canFormPFO = true;

			for(auto trackIter=tracks.begin(); trackIter!=tracks.end(); ++trackIter)
			{
				auto pTrack = *trackIter;
                if (!PandoraContentApi::IsAvailable(*this, pTrack)) canFormPFO = false;
			}

			if (canFormPFO==false) continue;
#endif
		}
#endif

		bool clusterHasAssociatedTrack = !( pCluster->GetAssociatedTrackList().empty() );

		// associated track can form Pfo ?
		bool canFormPfo = false;
		bool noParentTrack = false;

		float clusterHadEnergy = pCluster->GetHadronicEnergy();

#if 1
        if (pCluster->GetAssociatedTrackList().size()!=0)
		{
			auto tracks = pCluster->GetAssociatedTrackList();

#if 1
			for(auto trackIter=tracks.begin(); trackIter!=tracks.end(); ++trackIter)
			{
				auto pTrack = *trackIter;
				canFormPfo = pTrack->CanFormPfo() || pTrack->CanFormClusterlessPfo();
				noParentTrack = pTrack->GetParentList().empty();
				if(canFormPfo && noParentTrack) break;
			}
#endif

			//if(canFormPfo && noParentTrack) continue;
		}
		else
		{
			//std::cout << " ---- this is a neutral cluster..." << std::endl;
		}
#endif

		// the meaning should be clarified
		bool usedInPFO = !(canFormPfo && noParentTrack);

		std::vector<float> vars;
		vars.push_back( clusterHasAssociatedTrack );
		vars.push_back( canFormPfo );
		vars.push_back( noParentTrack );
		vars.push_back( usedInPFO );
		vars.push_back( clusterHadEnergy );

        extern HistogramManager AHM;
		AHM.CreateFill("PerfectPfoCreation_NeutralClusters", "clusterHasAssociatedTrack:canFormPfo:noParentTrack:usedInPFO:clusterHadEnergy", vars);

		if(canFormPfo && noParentTrack) continue;


#if 0
        if (pCluster->GetNCaloHits() < m_minHitsInCluster)
            continue;
#endif

        //const bool isPhoton(pCluster->PassPhotonId(this->GetPandora()));
        const pandora::MCParticle *const pMCParticle(pandora::MCParticleHelper::GetMainMCParticle(pCluster));
        const bool isPhoton(pandora::PHOTON == pMCParticle->GetParticleId());
        float clusterEnergy(isPhoton ? pCluster->GetCorrectedElectromagneticEnergy(this->GetPandora()) : pCluster->GetCorrectedHadronicEnergy(this->GetPandora()));

		//std::cout << "----cluster energy: " << pCluster->GetElectromagneticEnergy() << std::endl;
		//std::cout << "----- h_energy: " << pCluster->GetCorrectedHadronicEnergy(this->GetPandora()) << std::endl;

        // Veto non-photon clusters below hadronic energy threshold and those occupying a single layer
#if 0
        if (!isPhoton)
        {
            if (clusterEnergy < m_minClusterHadronicEnergy)
                continue;

            if (!m_allowSingleLayerClusters && (pCluster->GetInnerPseudoLayer() == pCluster->GetOuterPseudoLayer()))
                continue;
        }
        else
        {
            if (clusterEnergy < m_minClusterElectromagneticEnergy)
                continue;
        }
#endif

        // Specify the pfo parameters
        PandoraContentApi::ParticleFlowObject::Parameters pfoParameters;
        pfoParameters.m_particleId = (isPhoton ? pandora::PHOTON : pandora::NEUTRON);
        pfoParameters.m_charge = 0;
        pfoParameters.m_mass = (isPhoton ? pandora::PdgTable::GetParticleMass(pandora::PHOTON) : pandora::PdgTable::GetParticleMass(pandora::NEUTRON));
        pfoParameters.m_energy = clusterEnergy;

		//std::cout << "----- energy: " << clusterEnergy << std::endl;
        pfoParameters.m_clusterList.push_back(pCluster);


        // Photon position: 0) unweighted inner centroid, 1) energy-weighted inner centroid, 2+) energy-weighted centroid for all layers
        pandora::CartesianVector positionVector(0.f, 0.f, 0.f);

#if 0
        const unsigned int clusterInnerLayer(pCluster->GetInnerPseudoLayer());


        if (!isPhoton)
        {
            positionVector = pCluster->GetCentroid(clusterInnerLayer);
        }
        else if (1 == m_photonPositionAlgorithm)
        {
            positionVector = this->GetEnergyWeightedCentroid(pCluster, clusterInnerLayer, clusterInnerLayer);
        }
        else
        {
            positionVector = this->GetEnergyWeightedCentroid(pCluster, clusterInnerLayer, pCluster->GetOuterPseudoLayer());
        }

        const pandora::CartesianVector momentum(positionVector.GetUnitVector() * clusterEnergy);
#endif
        const pandora::CartesianVector momentum(1., 1., 1.);
        pfoParameters.m_momentum = momentum.GetUnitVector() * clusterEnergy;

        const pandora::ParticleFlowObject *pPfo(NULL);
		try 
		{
			PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::ParticleFlowObject::Create(*this, pfoParameters, pPfo));

			neutralPfoEnergy += pfoParameters.m_energy.Get();
		}
        catch (pandora::StatusCodeException &)
		{
			std::cout << "Create cluster failed..." << std::endl;
		}
    }

    return pandora::STATUS_CODE_SUCCESS;
}

/////////

pandora::StatusCode PerfectPfoCreationAlgorithm::CreateTrackBasedPfos() const
{
    const pandora::MCParticleList *pMCParticleList = NULL;
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pMCParticleList));

    if (pMCParticleList->empty())
        return pandora::STATUS_CODE_SUCCESS;

	int nPFO = 0;
	int nTrack = 0;

	float chargedPfoEnergy = 0.;

    for (pandora::MCParticleList::const_iterator iterMC = pMCParticleList->begin(), iterMCEnd = pMCParticleList->end(); iterMC != iterMCEnd; ++iterMC)
    {
        try
        {
            const pandora::MCParticle *const pPfoTarget = *iterMC;
			//std::cout << "pfoTarget: " << pPfoTarget << std::endl;
            PfoParameters pfoParameters;

            if( TrackCollection(pPfoTarget, pfoParameters) != pandora::STATUS_CODE_SUCCESS )
				return pandora::STATUS_CODE_FAILURE;

			/////
            int nTracksUsed(0);
            this->SetPfoParametersFromTracks(nTracksUsed, pfoParameters);

#if 1
            if ((0 == nTracksUsed) && pfoParameters.m_clusterList.empty())
            {
                std::cout << pPfoTarget << " No energy deposits for pfo target " << pPfoTarget->GetParticleId() 
					      << ", E: " << pPfoTarget->GetEnergy() << std::endl;

				continue;
                //throw pandora::StatusCodeException(pandora::STATUS_CODE_INVALID_PARAMETER);
            }
#endif

			const pandora::ParticleFlowObject *pPfo(NULL);
            //PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, 
			//PandoraContentApi::ParticleFlowObject::Create(*this, pfoParameters, pPfo));

            auto createStatus = PandoraContentApi::ParticleFlowObject::Create(*this, pfoParameters, pPfo);
			chargedPfoEnergy += pfoParameters.m_energy.Get();
			nTrack += nTracksUsed;

			std::cout << "up to here, pfo: " << ++nPFO << ", track: " << nTrack << std::endl;

			if(createStatus != pandora::STATUS_CODE_SUCCESS)
			{
				std::cout << "PFO creation error: " << createStatus << std::endl;
			}
		}
        catch (pandora::StatusCodeException &)
		{
			continue;
		}
	}


#if 1
    const pandora::PfoList *pPfoList = NULL;
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pPfoList));

	std::cout << "--- PFO size: " << pPfoList->size() << std::endl;

	// check track in pfo list
    for(pandora::PfoList::const_iterator iter = pPfoList->begin(), endIter = pPfoList->end(); endIter != iter ; ++iter)
    {
		const pandora::ParticleFlowObject* pfo = *iter;

		const pandora::TrackList& trackList = pfo->GetTrackList();

		std::cout << "pfo " << pfo << " has " << trackList.size() << " tracks" << std::endl;
		for(auto trackIter = trackList.begin(); trackIter != trackList.end(); ++trackIter)
		{
			std::cout << "  track " << *trackIter << " energy " << (*trackIter)->GetEnergyAtDca() << std::endl;
		}
	}
    

	// for the neutral cluster 
    try
    {
		//std::cout << "====== SetPfoParametersFromClusters ====== " << std::endl;
		this->SetPfoParametersFromClusters();
	}
    catch (pandora::StatusCodeException &)
	{
		std::cout << "StatusCodeException" << std::endl;
	}

	std::vector<float> vars;
	vars.push_back( chargedPfoEnergy );
	vars.push_back( neutralPfoEnergy );

    extern HistogramManager AHM;
	AHM.CreateFill("PerfectPfoCreation_PfoEnergy", "chargedPfoEnergy:neutralPfoEnergy", vars);

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pPfoList));
	std::cout << "--- PFO size after making neutral PFO: " << pPfoList->size() << std::endl;
#endif

    return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const pandora::CartesianVector PerfectPfoCreationAlgorithm::GetEnergyWeightedCentroid(const pandora::Cluster *const pCluster, const unsigned int innerPseudoLayer,
    const unsigned int outerPseudoLayer) const
{
    float energySum(0.f);
    pandora::CartesianVector energyPositionSum(0.f, 0.f, 0.f);
    const pandora::OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (pandora::OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        if (iter->first > outerPseudoLayer)
            break;

        if (iter->first < innerPseudoLayer)
            continue;

        for (pandora::CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const float electromagneticEnergy((*hitIter)->GetElectromagneticEnergy());
            energySum += electromagneticEnergy;
            energyPositionSum += ((*hitIter)->GetPositionVector() * electromagneticEnergy);
        }
    }

    if (energySum < std::numeric_limits<float>::epsilon())
        throw pandora::StatusCodeException(pandora::STATUS_CODE_NOT_INITIALIZED);

    return (energyPositionSum * (1.f / energySum));
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PerfectPfoCreationAlgorithm::ReadSettings(const pandora::TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "OutputPfoListName", m_outputPfoListName));

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ProcessAlgorithm(*this, xmlHandle,
        "ClusterAssociation", m_associationAlgorithmName));

    return pandora::STATUS_CODE_SUCCESS;
}

}
