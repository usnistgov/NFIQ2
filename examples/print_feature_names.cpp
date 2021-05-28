#include <nfiq2.hpp>

#include <iostream>

int
main()
{
	std::cout << "NFIQ 2 Features:\n";
	for (const auto &features :
	    NFIQ2::QualityFeatures::getQualityFeatureIDs()) {
		std::cout << " * " << features << '\n';
	}

	return (EXIT_SUCCESS);
}
