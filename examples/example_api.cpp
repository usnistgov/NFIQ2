#include <nfiq2/nfiq2.hpp>
#include <nfiq2/nfiq2_qualityfeatures.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>

#include <iostream>
#include <memory>

int
main(int argc, char **argv)
{
	if (argc != 3) {
		std::cout
		    << "NFIQ 2 API Example\n\nThis is a sample program that "
		       "shows how to use the NFIQ 2 API on an image.\n\nThis command line "
		       "tool takes two arguments.\nThe first is the path to a NFIQ 2 "
		       "RandomForest model information file.\nThe second is the path to a "
		       "single fingerprint image.\n\nPlease provide arguments to the binary "
		       "in the designated order.\n";
		return (EXIT_FAILURE);
	}

	// Note: NFIQ 2 only operates on images captured at a resolution of
	// 500 PPI. Unfortunately, the images contained within this repository
	// are encoded in PGM format, which does not encode resolution
	// information. ALWAYS make use of image libraries to decode image
	// resolution information and convert to PPI to avoid erroneous NFIQ 2
	// results.
	static const uint16_t PPI = 500;

	NFIQ::ModelInfo modelInfoObj {};
	try {
		modelInfoObj = NFIQ::ModelInfo(argv[1]);
	} catch (...) {
		std::cout
		    << "Could not parse model info file. "
		       "Ensure it is the first argument on the cmd line\n";
		return (EXIT_FAILURE);
	}

	std::unique_ptr<NFIQ::NFIQ2Algorithm> model {};
	try {
		model.reset(new NFIQ::NFIQ2Algorithm(modelInfoObj));
	} catch (...) {
		std::cout
		    << "Could not initialize model from model info file\n";
		return (EXIT_FAILURE);
	}

	cv::Mat imgMat {};
	try {
		imgMat = cv::imread(argv[2], cv::IMREAD_GRAYSCALE);
	} catch (...) {
		std::cout
		    << "Could not parse image file. Ensure it is the second "
		       "argument on the cmd line\n";
		return (EXIT_FAILURE);
	}

	NFIQ::FingerprintImageData rawImage = NFIQ::FingerprintImageData(
	    imgMat.data, static_cast<uint32_t>(imgMat.total()),
	    static_cast<uint32_t>(imgMat.cols),
	    static_cast<uint32_t>(imgMat.rows), 0, PPI);

	NFIQ::NFIQ2Results results {};
	try {
		results = (*model).computeQualityFeaturesAndScore(rawImage);
	} catch (...) {
		std::cout << "Error in calculating quality score\n";
		return (EXIT_FAILURE);
	}

	std::cout << "QualityScore: " << results.getScore() << "\n";

	std::vector<std::string> actionableHeaders =
	    NFIQ::QualityFeatures::getAllActionableIdentifiers();
	std::vector<NFIQ::ActionableQualityFeedback> actionableResults =
	    results.getActionable();

	for (size_t i = 0; i < actionableHeaders.size(); i++) {
		std::cout << actionableHeaders.at(i) << ": "
			  << actionableResults.at(i).actionableQualityValue
			  << "\n";
	}

	std::vector<std::string> featureHeaders =
	    NFIQ::QualityFeatures::getAllQualityFeatureIDs();
	std::vector<NFIQ::QualityFeatureData> featureResults =
	    results.getQuality();

	for (size_t i = 0; i < featureHeaders.size(); i++) {
		std::cout << featureHeaders.at(i) << ": "
			  << featureResults.at(i).featureDataDouble << "\n";
	}

	std::vector<std::string> speedHeaders =
	    NFIQ::QualityFeatures::getAllSpeedFeatureGroups();
	std::vector<NFIQ::QualityFeatureSpeed> speedResults =
	    results.getSpeed();

	for (size_t i = 0; i < speedHeaders.size(); i++) {
		std::cout << speedHeaders.at(i) << ": "
			  << speedResults.at(i).featureSpeed << "\n";
	}

	return (EXIT_SUCCESS);
}
