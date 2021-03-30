#include <nfiq2_qualityfeatures.hpp>

#include <iostream>

int
main()
{
	std::cout << "NFIQ 2 Features:\n";
	for (const auto &features :
	    NFIQ::QualityFeatures::getAllQualityFeatureIDs()) {
		std::cout << " * " << features << '\n';
	}

	return (EXIT_SUCCESS);
}
