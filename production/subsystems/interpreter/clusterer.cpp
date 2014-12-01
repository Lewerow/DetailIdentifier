#include <interpreter/clusterer.h>
#include <algorithm>
#include <cassert>
#include <map>
#include <cmath>

#include <GClasses/GCluster.h>

namespace
{
	const double ACCEPTABLE_RATIO = 1.75;
	const double MIN_ACCEPTABLE_DISTANCE = 0.5;
	const std::size_t ATTEMPT_COUNT = 10;

	double waffles_distance(double* l1, double* l2)
	{
		return std::sqrt(std::pow(l1[0] - l2[0], 2) + std::pow(l1[1] - l2[1], 2));
	}

	bool roughly_equal_clusters(GClasses::GKMeans& kmeans, GClasses::GMatrix& locations)
	{
		GClasses::GMatrix& centroids = *kmeans.centroids();
		double total_distances = kmeans.assignClusters(&locations);
		
		std::map<std::size_t, std::pair<std::size_t, double> > distances;

		for (std::size_t i = 0; i < locations.rows(); ++i)
		{
			std::size_t cluster = kmeans.whichCluster(i);
			++distances[cluster].first;
			distances[cluster].second += waffles_distance(locations[i], centroids[cluster]);
		}
		
		std::vector<double> average_in_cluster;
		for (auto& d : distances)
		{
			if (d.second.first == 1)
				continue;

			double average_distance = d.second.second / d.second.first;
			if(average_distance > MIN_ACCEPTABLE_DISTANCE)
        		average_in_cluster.push_back(average_distance);
		}

		std::sort(average_in_cluster.begin(), average_in_cluster.end()); // order is ASCENDING

		if (average_in_cluster.size() < 2)
			return true;

		return average_in_cluster.back() / average_in_cluster.front() < ACCEPTABLE_RATIO;
	}

	GClasses::GMatrix setup_data(const std::vector<svg::location>& data)
	{
		GClasses::GMatrix matrix(data.size(), 2);
		for (std::size_t i = 0; i < data.size(); ++i)
		{
			matrix[i][0] = data[i].x;
			matrix[i][1] = data[i].y;
		}

		return matrix;
	}

	std::vector<svg::location> read_clusters(GClasses::GMatrix& matrix)
	{
		assert(matrix.cols() == 2);
		std::vector<svg::location> clusters;
		for (std::size_t i = 0; i < matrix.rows(); ++i)
			clusters.push_back(svg::location(matrix[i][0], matrix[i][1]));

		return clusters;
	}
}

namespace interpreter
{
	clusterer::clusterer(const std::vector<svg::location>& data_) : data(data_)
	{}
	
	std::vector<svg::location> clusterer::clusterize()
	{
		GClasses::GRandMersenneTwister random(11);
		GClasses::GMatrix data_matrix = setup_data(data);

		std::size_t cluster_count = 1;
		bool clusters_roughly_equal = false;
		std::vector<svg::location> clusters;
		while(true)
		{
			GClasses::GKMeans kmeans(++cluster_count, &random);
			kmeans.setReps(ATTEMPT_COUNT);
			kmeans.init(&data_matrix);
			kmeans.cluster(&data_matrix);

			if (roughly_equal_clusters(kmeans, data_matrix))
			{
				clusters = read_clusters(*kmeans.centroids());
				break;
			}
		}

		std::sort(clusters.begin(), clusters.end(), [](svg::location l1, svg::location l2) {return svg::cartesian_distance(l1, svg::location(0, 0)) < svg::cartesian_distance(l2, svg::location(0, 0)); });

		return clusters;
	}
}