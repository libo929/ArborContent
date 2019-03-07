/*
 *
 * Cluster.h header template automatically generated by a class generator
 * Creation date : lun. mars 30 2015
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


#ifndef ARBORCLUSTER_H
#define ARBORCLUSTER_H

#include "Objects/Cluster.h"

#include "Api/PandoraApi.h"
#include "Api/PandoraContentApi.h"


#include "Pandora/PandoraInputTypes.h"
#include "ArborApi/ArborInputTypes.h"

#include "ArborApi/ArborContentApi.h"

//#include "ArborObjects/ArborMetaData.h"

namespace arbor_content
{

class ClusterFactory;

/** 
 *  @brief  Cluster class
 */ 
class ArborCluster : public pandora::Cluster
{
public:
	float GetMergedHadronicEnergy();

	const std::vector<ArborCluster*>& GetMotherCluster() const;
	const std::vector<ArborCluster*>& GetClustersToMerge() const;
        void  GetAllClustersToMerge(std::vector<ArborCluster*>& allClustersToMerge) const;
	const std::vector<ArborCluster*>& GetNearbyClusters() const;

	const pandora::CartesianVector& GetAxis() const;
	const pandora::CartesianVector& GetIntercept() const;
	const pandora::CartesianVector& GetCentroid() const;
	const pandora::CartesianVector& GetStartingPoint() const;
	const pandora::CartesianVector& GetEndpoint() const;

	bool IsPhoton();

	void SetMotherCluster(ArborCluster* cluster);
	void SetClustersToMerge(const std::vector<ArborCluster*>& clusterVector);
	void SetNearbyClusters(const std::vector<ArborCluster*>& clusterVector);

	void SetAxis(pandora::CartesianVector axis);
	void SetIntercept(pandora::CartesianVector intercept);
	void SetCentroid(pandora::CartesianVector centrod);
	void SetStartingPoint(pandora::CartesianVector startingPoint);
	void SetEndpoint(pandora::CartesianVector endpoint);

	void SetPhoton(bool isPhoton);

private:

    //
	ArborCluster(const PandoraContentApi::Cluster::Parameters &parameters);

	~ArborCluster();


protected:
	std::vector<ArborCluster*> m_motherCluster;

	std::vector<ArborCluster*> m_clustersToMerge;
	std::vector<ArborCluster*> m_nearbyClusters;

	pandora::CartesianVector m_axis;
	pandora::CartesianVector m_intercept;
	pandora::CartesianVector m_centroid;
	pandora::CartesianVector m_startingPoint;
	pandora::CartesianVector m_endpoint;

	bool m_isPhoton;

    friend class ClusterFactory;
};

} 

#endif 
