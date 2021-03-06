  /// \file PerfectPfoCreationAlgorithm.cc
/*
 *
 * PerfectPfoCreationAlgorithm.cc source template automatically generated by a class generator
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
#include "Api/PandoraContentApi.h"

#include "APRILCheating/PerfectPfoCreationAlgorithm.h"
#include "APRILHelpers/ClusterHelper.h"
#include "APRILHelpers/CaloHitHelper.h"
#include "APRILHelpers/ReclusterHelper.h"

#include "APRILHelpers/HistogramHelper.h"


namespace april_content
{

PerfectPfoCreationAlgorithm::PerfectPfoCreationAlgorithm()
{
}

const pandora::MCParticle* GetMCParticle(const pandora::Track *const pTrack)
{
    float bestWeight(0.f);
    const pandora::MCParticle *pBestMCParticle(nullptr);
    const pandora::MCParticleWeightMap &hitMCParticleWeightMap(pTrack->GetMCParticleWeightMap());

	if( hitMCParticleWeightMap.empty() )
	{
		//std::cout << "track mcp error: hitMCParticleWeightMap is empty " << std::endl;
	}

	pandora::MCParticleVector mcParticleVector;
    for (const pandora::MCParticleWeightMap::value_type &mapEntry : hitMCParticleWeightMap) mcParticleVector.push_back(mapEntry.first);
    //std::sort(mcParticleVector.begin(), mcParticleVector.end(), PointerLessThan<MCParticle>());

    for (const pandora::MCParticle *const pMCParticle : mcParticleVector)
    {
        const float weight(hitMCParticleWeightMap.at(pMCParticle));

        if (weight > bestWeight)
        {
            bestWeight = weight;
            pBestMCParticle = pMCParticle;
        }
    }

    if (!pBestMCParticle)
    {
		//std::cout << "track mcp error " << std::endl;

        //throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);
    }
	else
	{
		//std::cout << pBestMCParticle << std::endl;
	}

    return pBestMCParticle;
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
        const pandora::Track *const pTrack = *iter;

        try
        {
			//GetMCParticle( pTrack );

            const pandora::MCParticle *const pTrkMCParticle(pandora::MCParticleHelper::GetMainMCParticle(pTrack));
            const pandora::MCParticle *const pTrkPfoTarget(pTrkMCParticle->GetPfoTarget());

            if (pTrkPfoTarget != pPfoTarget)
                continue;

			// FIXME: this may be good for real clustering
#if 0
			float trackEnergy = pTrack->GetEnergyAtDca();
			float trackMCPEnergy = pTrkMCParticle->GetEnergy();

			if(trackMCPEnergy > 5. && fabs(trackEnergy-trackMCPEnergy)/trackMCPEnergy > 0.5) 
			{
				std::cout << "error: track energy: " << trackEnergy << ", mcp energy: " << trackMCPEnergy << std::endl;
				continue;
			}
#endif

            pfoParameters.m_trackList.push_back(pTrack);

			//std::cout << "found a track: " << pTrack << " for MCP: " << pTrkPfoTarget << std::endl;
        }
        catch (pandora::StatusCodeException &e)
        {
			///// std::cout << e.ToString() << std::endl;
			///// skip this event
			//std::cout << " track " << pTrack << " mcp is not found " << std::endl;
			///// return pandora::STATUS_CODE_FAILURE;
			///// continue;
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

			// track-cluster
			if(pTrack->HasAssociatedCluster())
			{
				const pandora::Cluster* pCluster = pTrack->GetAssociatedCluster();

                if (PandoraContentApi::IsAvailable(*this, pCluster))
				{
					clusters.push_back(pCluster);
				}

				//std::cout << "track: " << pTrack << " --- cluster: " << pCluster << std::endl;
				//std::cout << "track energy: " << pTrack->GetEnergyAtDca() << ", cluster energy: " << pCluster->GetHadronicEnergy() 
				//	<< std::endl;
			}

            if (!pTrack->CanFormPfo() && !pTrack->CanFormClusterlessPfo())
            {
                //std::cout << " Drop track, E: " << pTrack->GetEnergyAtDca() << " cfp: " << pTrack->CanFormPfo() << " cfcp: " << pTrack->CanFormClusterlessPfo() << std::endl;
		        //std::cout << "ReachesCalorimeter: " << pTrack->ReachesCalorimeter() << std::endl;

                continue;
            }

            if (!pTrack->GetParentList().empty())
            {
                //std::cout << " Drop track, E: " << pTrack->GetEnergyAtDca() << " nParents: " << pTrack->GetParentList().size() << std::endl;
                continue;
            }

            ++nTracksUsed;

            // ATTN Assume neutral track-based pfos represent pair-production
            const float electronMass(pandora::PdgTable::GetParticleMass(pandora::E_MINUS));

            charge += pTrack->GetCharge();
            momentum += pTrack->GetMomentumAtDca();
            energyWithPionMass += pTrack->GetEnergyAtDca();
            energyWithElectronMass += std::sqrt(electronMass * electronMass + pTrack->GetMomentumAtDca().GetMagnitudeSquared());
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

    for (pandora::ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
	{
		auto pCluster = *iter;

		std::vector<float> vars;
		vars.push_back( pCluster->GetNCaloHits() );
		vars.push_back( pCluster->GetHadronicEnergy() );
		HistogramManager::CreateFill("PerfectPfoCreation_cluster", "clusterSize:hadEnergy", vars);
        //int showerStartLayer(pCluster->GetShowerStartLayer(this->GetPandora()));
		//std::cout << "showerStartLayer: " << showerStartLayer << std::endl;
	}

	neutralPfoEnergy = 0.;

    // Examine clusters with no associated tracks to form neutral pfos
    for (pandora::ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        const pandora::Cluster *const pCluster = *iter;
        if (!PandoraContentApi::IsAvailable(*this, pCluster)) continue;

		//std::cout << "cluster: " << pCluster->GetAssociatedTrackList().size() << std::endl;

		bool clusterHasAssociatedTrack = !( pCluster->GetAssociatedTrackList().empty() );

		// the properties of associated track 
		bool canFormPfo = false;
		bool noParentTrack = false;
		bool trakEnergyError = false;

		float clusterHadEnergy = pCluster->GetHadronicEnergy();

        if (pCluster->GetAssociatedTrackList().size()!=0)
		{
			auto tracks = pCluster->GetAssociatedTrackList();

			for(auto trackIter=tracks.begin(); trackIter!=tracks.end(); ++trackIter)
			{
				auto pTrack = *trackIter;
				canFormPfo = pTrack->CanFormPfo() || pTrack->CanFormClusterlessPfo();
				noParentTrack = pTrack->GetParentList().empty();

				try
				{
			        float trackEnergy = pTrack->GetEnergyAtDca();
                    const pandora::MCParticle *const pTrkMCParticle(pandora::MCParticleHelper::GetMainMCParticle(pTrack));
			        float trackMCPEnergy = pTrkMCParticle->GetEnergy();

					// some reconstructed track has problem on energy ...
			        trakEnergyError = (trackMCPEnergy > 5. && fabs(trackEnergy-trackMCPEnergy)/trackMCPEnergy > 0.5);
					if(trakEnergyError) 
					{
						//std::cout << "track energy: " <<  trackEnergy << " is not OK, mcp energy: " << trackMCPEnergy << std::endl;
					}
				}
                catch (pandora::StatusCodeException &e)
				{
				}

				if(canFormPfo && noParentTrack && (!trakEnergyError) ) break;
			}

			//if(canFormPfo && noParentTrack) continue;
		}
		else
		{
			//std::cout << " ---- this is a neutral cluster..." << std::endl;
		}

		// the usage of cluster here is determined by its associated tracks
		bool usedInPFO = !(canFormPfo && noParentTrack);

		std::vector<float> vars;
		vars.push_back( clusterHasAssociatedTrack );
		vars.push_back( canFormPfo );
		vars.push_back( noParentTrack );
		vars.push_back( usedInPFO );
		vars.push_back( clusterHadEnergy );

        HistogramManager::CreateFill("PerfectPfoCreation_NeutralClusters", 
				"clusterHasAssociatedTrack:canFormPfo:noParentTrack:usedInPFO:clusterHadEnergy", vars);

		if(canFormPfo && noParentTrack && (!trakEnergyError) ) continue;
		//if( !usedInPFO ) continue;


#if 0
        if (pCluster->GetNCaloHits() < m_minHitsInCluster)
            continue;
#endif

        //const bool isPhoton(pCluster->PassPhotonId(this->GetPandora()));
		const pandora::MCParticle *pMCParticle = nullptr;
		try
		{
			pMCParticle = pandora::MCParticleHelper::GetMainMCParticle(pCluster);
		}
        catch (pandora::StatusCodeException &e)
		{
			//std::cout << "failure: getting cluster mcp..." << std::endl;
		}

		//std::cout << "cluster MCParticle: " << pMCParticle << endl;
        //const bool isPhoton(pandora::PHOTON == pMCParticle->GetParticleId());
        bool isPhoton;

		if(pMCParticle!=nullptr)
		{
			isPhoton = pandora::PHOTON == pMCParticle->GetPfoTarget()->GetParticleId();
		}
		else
		{
			isPhoton = true;
		}
		//std::cout << "cluster is photon ?: " << isPhoton  << endl;
        float clusterEnergy(isPhoton ? pCluster->GetCorrectedElectromagneticEnergy(this->GetPandora()) : pCluster->GetCorrectedHadronicEnergy(this->GetPandora()));
		//std::cout << "cluster energy: " << clusterEnergy << endl;

		// if cluster has associated tracks, use the energy from tracks
		// N.B. if the varible 'ShouldCollapseMCParticlesToPfoTarget' in the steering file is false, should be set to false now
		if( !m_collapsedMCParticlesToPfoTarget )
		{
			const pandora::TrackList& clusterTrackList = pCluster->GetAssociatedTrackList();

		    if(!clusterTrackList.empty())
		    {
		    	float orignalClusterEnergy = clusterEnergy;
		    	clusterEnergy = 0.;

		    	float trackNum = 0.;
		    	float trackParentSize = 0.;

		    	for(auto trk : clusterTrackList)
		        {
		    		//auto startPos = trk->GetTrackStateAtStart().GetPosition();
		    		//auto endPos = trk->GetTrackStateAtEnd().GetPosition();
		    		//auto posDiff = (endPos - startPos).GetMagnitude();

		    		//if( ! ( (trk->CanFormPfo() || trk->CanFormClusterlessPfo() ) && trk->GetParentList().empty() ) )
		    		//if( ( (trk->CanFormPfo() || trk->CanFormClusterlessPfo() ) && trk->GetParentList().empty() ) )
		    		//if( ( (trk->CanFormPfo() || trk->CanFormClusterlessPfo() ) ) )
		    		//if( trk->GetParentList().empty() )
                    //if ( PandoraContentApi::IsAvailable(*this, trk) )
		    		//if( posDiff > 400 )
		    		{
		    			clusterEnergy += trk->GetEnergyAtDca();
		    			//std::cout << "track energy: " << trk->GetEnergyAtDca() << ", track length: " << posDiff << std::endl;
		    			++trackNum;
		    		}
		    	    
		    		trackParentSize += trk->GetParentList().size();
		        }

		    	///////////////////////////////////////
		    	// cluster - track energy comparison
		    	///////////////////////////////////////
		    	if( fabs(clusterEnergy-orignalClusterEnergy)/orignalClusterEnergy > 1.5)
		    	{
		    		clusterEnergy = orignalClusterEnergy;
		    		trackNum = 0.;
		    	}

		    	float mcpEnergy = pMCParticle->GetEnergy();

		        std::vector<float> trkVars;
		        trkVars.push_back( mcpEnergy );
		        trkVars.push_back( orignalClusterEnergy );
		        trkVars.push_back( clusterEnergy );
		        trkVars.push_back( trackNum );
		        trkVars.push_back( trackParentSize );
		    
				HistogramManager::CreateFill("PerfectPfoCreation_Track4Neutral", 
						"mcpEnergy:orignalClusterEnergy:clusterEnergy:trackNum:trackParentSize", trkVars);
		    }
	    }


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
			//std::cout << "neutral pfo energy: " << pfoParameters.m_energy.Get() << std::endl;
		}
        catch (pandora::StatusCodeException &)
		{
			//std::cout << "Create cluster failed..." << std::endl;
		}
    }

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));
	std::cout << "check the cluster again and again: " << pClusterList->size() << std::endl;

    return pandora::STATUS_CODE_SUCCESS;
}

/////////

pandora::StatusCode PerfectPfoCreationAlgorithm::CreateTrackBasedPfos() const
{
    const pandora::MCParticleList *pMCParticleList = NULL;
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pMCParticleList));

    if (pMCParticleList->empty())
        return pandora::STATUS_CODE_SUCCESS;

	//int nPFO = 0;
	int nTrack = 0;

	float chargedPfoEnergy = 0.;

    for (pandora::MCParticleList::const_iterator iterMC = pMCParticleList->begin(), iterMCEnd = pMCParticleList->end(); iterMC != iterMCEnd; ++iterMC)
    {
        try
        {
			if( !(*iterMC)->IsPfoTarget() ) continue;

            const pandora::MCParticle *const pPfoTarget = *iterMC;
			//std::cout << "pfoTarget: " << pPfoTarget << std::endl;
            PfoParameters pfoParameters;

            //TrackCollection(pPfoTarget, pfoParameters);
            if( TrackCollection(pPfoTarget, pfoParameters) != pandora::STATUS_CODE_SUCCESS )
			{
			//	return pandora::STATUS_CODE_FAILURE;
			}

			/////
            int nTracksUsed(0);
            this->SetPfoParametersFromTracks(nTracksUsed, pfoParameters);

            if ((0 == nTracksUsed) && pfoParameters.m_clusterList.empty())
            {
                //std::cout << pPfoTarget << " No energy deposits for pfo target " << pPfoTarget->GetParticleId() 
				//	      << ", E: " << pPfoTarget->GetEnergy() << std::endl;

				continue;
                //throw pandora::StatusCodeException(pandora::STATUS_CODE_INVALID_PARAMETER);
            }

			const pandora::ParticleFlowObject *pPfo(NULL);
            //PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, 
			//PandoraContentApi::ParticleFlowObject::Create(*this, pfoParameters, pPfo));

            auto createStatus = PandoraContentApi::ParticleFlowObject::Create(*this, pfoParameters, pPfo);
			chargedPfoEnergy += pfoParameters.m_energy.Get();
			nTrack += nTracksUsed;

			//std::cout << "up to here, pfo: " << ++nPFO << ", track: " << nTrack << std::endl;

			if(createStatus != pandora::STATUS_CODE_SUCCESS)
			{
				//std::cout << "PFO creation error: " << createStatus << std::endl;
			}
		}
        catch (pandora::StatusCodeException &)
		{
			continue;
		}
	}

#if 0
	//////////////////////////////////////////////////////////////////////
	// check track in pfo list
    const pandora::PfoList *pPfoList = NULL;
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pPfoList));

	std::cout << "--- PFO size: " << pPfoList->size() << std::endl;

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
#endif
    
	// for the neutral cluster 
    try
    {
		//std::cout << "====== SetPfoParametersFromClusters ====== " << std::endl;
		this->SetPfoParametersFromClusters();
	}
    catch (pandora::StatusCodeException &)
	{
		//std::cout << "StatusCodeException" << std::endl;
	}

	std::vector<float> vars;
	vars.push_back( chargedPfoEnergy );
	vars.push_back( neutralPfoEnergy );

	//std::cout << "chargedPfoEnergy: " << chargedPfoEnergy << ", neutralPfoEnergy: " << neutralPfoEnergy << std::endl;

    HistogramManager::CreateFill("PerfectPfoCreation_PfoEnergy", "chargedPfoEnergy:neutralPfoEnergy", vars);

#if 0
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

    m_collapsedMCParticlesToPfoTarget = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "CollapsedMCParticlesToPfoTarget", m_collapsedMCParticlesToPfoTarget));

    return pandora::STATUS_CODE_SUCCESS;
}

}
