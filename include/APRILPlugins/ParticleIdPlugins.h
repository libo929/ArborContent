/// \file ParticleIdPlugins.h
/*
 *
 * ParticleIdPlugins.h header template automatically generated by a class generator
 * Creation date : lun. oct. 17 2016
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


#ifndef PARTICLEIDPLUGINS_H
#define PARTICLEIDPLUGINS_H

#include "Plugins/ParticleIdPlugin.h"
#include "Pandora/PandoraInputTypes.h"

namespace april_content
{
  /**
   *  @brief  ParticleIdHelper class
   */
  class ParticleIdHelper
  {
  public:
    /**
     *  @brief  StartingLayerSettings class
     */
    class StartingLayerSettings
    {
    public:
      /**
       *  @brief  Default constructor
       */
      StartingLayerSettings();

      pandora::InputUInt         m_maxForwardPseudoLayer;
      pandora::InputUInt         m_minPseudoLayerNHits;
      pandora::InputFloat        m_minFitHitDistanceFine;
      pandora::InputFloat        m_minFitHitDistanceCoarse;
      pandora::InputUInt         m_minNInteractingLayers;
      pandora::InputFloat        m_lowEnergyClusterCut;
    };

    /**
     *  @brief  Get the starting pseudo layer of the target cluster
     *
     *  @param  pandora the pandora instance to access content
     *  @param  pCluster the target cluster address
     *  @param  settings the input settings to calculate the starting pseudo layer
     *  @param  startingPseudoLayer the starting pseudo layer to receive
     */
    static pandora::StatusCode GetStartingPseudoLayer(const pandora::Pandora &pandora, const pandora::Cluster *const pCluster,
        const StartingLayerSettings &settings, unsigned int &startingPseudoLayer);
  
	static pandora::StatusCode GetClusterVolume(const pandora::Cluster *const pCluster, float& clusterVolume);
  };

  //------------------------------------------------------------------------------------------------------------------------------------------

/**
*  @brief  LCParticleIdPlugins class
*/
class APRILParticleIdPlugins
{
public:

	  /**
	   *  @brief  APRILEmShowerId class
	   */
	  class APRILEmShowerId : public pandora::ParticleIdPlugin
	  {
	  public:
	    /**
	     *  @brief  Default constructor
	     */
	    APRILEmShowerId();
	
	    bool IsMatch(const pandora::Cluster *const pCluster) const;
		bool IsMatch(const pandora::ParticleFlowObject *const pPfo) const;
	    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);
	
	    /**
	     *
	     */
	    const pandora::CartesianVector GetEnergyWeightedCentroid(const pandora::Cluster *const pCluster, const unsigned int innerPseudoLayer,
	        const unsigned int outerPseudoLayer) const;

		static void SetRecord(bool canRecord);
	
	  private:
	    ParticleIdHelper::StartingLayerSettings      m_startingLayerSettings;
	    unsigned int                                 m_maxStartPseudoLayer;
	    unsigned int                                 m_maxPseudoLayerExtension;
	    float                                        m_maxAngleWithOrigin;
	    float                                        m_maxHadronicEnergyFraction;
	    unsigned int                                 m_minOuterEcalPseudoLayerEnergyCut;
	    float                                        m_maxOuterEcalEnergyFraction;

		static bool                                  m_canRecord;
	  };
	
	  //------------------------------------------------------------------------------------------------------------------------------------------
	  //------------------------------------------------------------------------------------------------------------------------------------------
	
	  /**
	   *  @brief  APRILPhotonId class
	   */
	  class APRILPhotonId : public pandora::ParticleIdPlugin
	  {
	    public:
	       bool IsMatch(const pandora::Cluster *const pCluster) const;
		   bool IsMatch(const pandora::ParticleFlowObject *const pPfo) const;

		   static void SetRecord(bool canRecord);
	       pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

	    private:
		   static bool                               m_canRecord;
	  };
	
	  //------------------------------------------------------------------------------------------------------------------------------------------
	  //------------------------------------------------------------------------------------------------------------------------------------------
	
	  /**
	   *  @brief  APRILElectronId class
	   */
	  class APRILElectronId : public pandora::ParticleIdPlugin
	  {
	  public:
	    bool IsMatch(const pandora::Cluster *const pCluster) const;
		bool IsMatch(const pandora::ParticleFlowObject *const pPfo) const;
	    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);
	  };
	
	  //------------------------------------------------------------------------------------------------------------------------------------------
	  //------------------------------------------------------------------------------------------------------------------------------------------
	
	  /**
	   *  @brief  APRILMuonId class
	   */
	  class APRILMuonId : public pandora::ParticleIdPlugin
	  {
	  public:
	    /**
	     *  @brief  Default constructor
	     */
	    APRILMuonId();
	
	    bool IsMatch(const pandora::Cluster *const pCluster) const;
		bool IsMatch(const pandora::ParticleFlowObject *const pPfo) const;
	    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);
	
	  private:
	    float                     m_lowMomentumMuonCut;
	    unsigned int              m_maxNCaloHitsInPseudoLayerFine;
	    unsigned int              m_maxNCaloHitsInPseudoLayerCoarse;
	    unsigned int              m_maxNHighDensityPseudoLayer;
	    unsigned int              m_minEcalPseudoLayers;
	    unsigned int              m_minEcalLayers;
	    unsigned int              m_minHcalPseudoLayers;
	    unsigned int              m_minHcalLayers;
	  };
};

} 

#endif  //  PARTICLEIDPLUGINS_H
