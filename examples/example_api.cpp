#include <nfiq2/nfiq2.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>

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

	cv::Mat imgMat {};
	try {
		imgMat = cv::imread(argv[2], cv::IMREAD_GRAYSCALE);
	} catch (...) {
		std::cout
		    << "Could not parse image file. Ensure it is the second argument on the cmd line\n";
		return EXIT_FAILURE;
	}

	// Will be populated by decompressed fingerprint image
	NFIQ::FingerprintImageData rawImage = NFIQ::FingerprintImageData(
	    imgMat.data, static_cast<uint32_t>(imgMat.total()),
	    static_cast<uint32_t>(imgMat.cols),
	    static_cast<uint32_t>(imgMat.rows), 0, 500);

	NFIQ::NFIQ2Results results {};
	try {
		results = (*model).computeQualityFeaturesAndScore(rawImage);
	} catch (...) {
		std::cout << "Error in calculating quality score\n";
		return EXIT_FAILURE;
	}

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
