#include <nfiq2.hpp>

#include <iostream>

int
main()
{
	std::cout << "NFIQ 2 Native Quality Measures:\n";
	for (const auto &features :
	    NFIQ2::QualityMeasures::getQualityFeatureIDs()) {
		std::cout << " * " << features << '\n';
	}

	return (EXIT_SUCCESS);
}
