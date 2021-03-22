#include <nfiq2/nfiq2.hpp>

#include <iostream>

int
main(int argc, char **argv)
{
	if (argc != 3) {
		std::cout << "Incorrect Number of Arguments\n";
		return EXIT_FAILURE;
	}

	NFIQ::ModelInfo modelInfoObj {};
	try {
		modelInfoObj = NFIQ::ModelInfo(argv[1]);
	} catch (...) {
		std::cout
		    << "Could not parse model info file. Ensure it is the first argument on the cmd line\n";
		return EXIT_FAILURE;
	}

	std::shared_ptr<NFIQ::NFIQ2Algorithm> model {};
	try {
		model = std::make_shared<NFIQ::NFIQ2Algorithm>(modelInfoObj);
	} catch (...) {
		std::cout
		    << "Could not initialize model from model info file\n";
		return EXIT_FAILURE;
	}

	// Second arg passed in will be image to be decompressed (by opencv)

	// Will be populated by decompressed fingerprint image
	NFIQ::FingerprintImageData rawImage {};

	NFIQ::NFIQ2Results results = (*model).computeQualityFeaturesAndScore(
	    rawImage);

	std::cout << "NFIQ 2 Quality Score: " << results.getScore() << "\n";

	for (auto i : results.getActionable()) {
		std::cout << "Actionable Score: " << i.actionableQualityValue
			  << "\n";
	}

	for (auto i : results.getQuality()) {
		std::cout << "Feature Quality Score: " << i.featureDataDouble
			  << "\n";
	}

	for (auto i : results.getSpeed()) {
		std::cout << "Feature Quality Speed: " << i.featureSpeed
			  << "\n";
	}

	return (EXIT_SUCCESS);
}
