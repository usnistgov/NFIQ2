#include <nfiq2/nfiq2.hpp>

#include <iostream>

int
main()
{
	std::cout << "NFIQ 2 Features:\n";
	for (const auto &features :
	    NFIQ::NFIQ2Algorithm::getAllQualityFeatureIDs()) {
		std::cout << " * " << features << '\n';
	}

	return (EXIT_SUCCESS);
}
