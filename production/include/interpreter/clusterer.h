#ifndef CLUSTERER_H_FEwmfiu3mg8u45jrtgiufjckldsmciowvurgjnv8943ojgklreglewflkwefkerjf
#define CLUSTERER_H_FEwmfiu3mg8u45jrtgiufjckldsmciowvurgjnv8943ojgklreglewflkwefkerjf

#include <vector>
#include <interpreter/svg/location.h>

namespace interpreter
{
	class clusterer
	{
	public:
		clusterer(const std::vector<svg::location>& data_);
        clusterer& operator=(const clusterer&) = delete;

        std::vector<svg::location> clusterize();

	private:
        const std::vector<svg::location>& data;
	};
}

#endif