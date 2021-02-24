/******************************************************************************
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 ******************************************************************************/

#ifdef _WIN32
#include <getopt.h>
#endif

#include <be_image_image.h>
#include <be_io_propertiesfile.h>
#include <be_io_recordstore.h>
#include <be_io_utility.h>
#include <be_sysdeps.h>
#include <be_text.h>
#include <nfiq2/modelinfo.hpp>
#include <nfiq2/nfiq2.hpp>
#include <nfiq2/timer.hpp>
#include <nfiq2/tool/nfiq2_ui_exception.h>
#include <nfiq2/tool/nfiq2_ui_image.h>
#include <nfiq2/tool/nfiq2_ui_log.h>
#include <nfiq2/tool/nfiq2_ui_refresh.h>
#include <nfiq2/tool/nfiq2_ui_threadedlog.h>
#include <nfiq2/tool/nfiq2_ui_types.h>
#include <nfiq2/tool/nfiq2_ui_utils.h>
#include <nfir_lib.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <ostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace BE = BiometricEvaluation;

std::mutex mutGray;

// Performs additional checks for an image before calculating an NFIQ2 score
void
NFIQ2UI::executeSingle(std::shared_ptr<BE::Image::Image> img,
    const std::string &name, const Flags &flags,
    const NFIQ::NFIQ2Algorithm &model, std::shared_ptr<NFIQ2UI::Log> logger,
    const bool singleImage, const bool interactive,
    const uint8_t fingerPosition, const std::string &warning)
{
	// Indicate whether it was quantized or re-sampled
	bool quantized = false;
	bool resampled = false;

	// Starting Checks for Image: 8 bit color and depth
	const uint16_t bitDepth = img->getBitDepth();
	const uint32_t colorDepth = img->getColorDepth();

	if ((bitDepth != colorDepth) || (bitDepth != 8)) {
		if (flags.force) {
			quantized = true;
			// quantize by force - gets handled below with
			// getRawGrayscaleData

		} else {
			if (interactive && !flags.force) {
				logger->debugMsg(
				    "Image is not 8 bit. Asking user to "
				    "quantize.");
				const std::string prompt =
				    "This Image is not 8 bit grayscale. "
				    "Would "
				    "you like to quantize/convert this "
				    "image to 8 bit color and depth?";
				const bool response = NFIQ2UI::yesOrNo(
				    prompt, false, true, true);

				if (response) {
					quantized = true;
					// Approved the quantize - gets handled
					// below with getRawGrayscaleData
					logger->debugMsg(
					    "User approved the quantize");
				} else {
					// Denied the quantize
					logger->debugMsg(
					    "User denied the quantize");
					logger->printError(name, fingerPosition,
					    255,
					    "'Error: User chose not to "
					    "quantize image'",
					    quantized, resampled);
					return;
				}
			} else {
				logger->printError(name, fingerPosition, 255,
				    "'Error: image is not 8 bit "
				    "depth and/or color'",
				    quantized, resampled);
				return;
			}
		}
	}
	logger->debugMsg(
	    "Successfully passed bit and color depth check. Quantized?: " +
	    std::to_string(quantized));
	// Now check for PPI

	BE::Memory::uint8Array grayscaleRawData {};

	try {
		if (img->getCompressionAlgorithm() ==
		    BE::Image::CompressionAlgorithm::WSQ20) {
			std::unique_lock<std::mutex> ulock(mutGray);
			grayscaleRawData = img->getRawGrayscaleData(8);
			ulock.unlock();

		} else {
			grayscaleRawData = img->getRawGrayscaleData(8);
		}

	} catch (const BE::Error::Exception &e) {
		logger->debugMsg(
		    "Could not get Grayscale raw data from image" + name);
		std::string error {
			"'Error: Could not get Grayscale raw data from image'"
		};
		logger->printError(name, fingerPosition, 255,
		    error.append(e.what()), quantized, resampled);
		return;
	}

	const BE::Image::Size dimensions = img->getDimensions();
	const uint32_t imageWidth = dimensions.xSize;
	const uint32_t imageHeight = dimensions.ySize;
	const BE::Image::Resolution resolution = img->getResolution().toUnits(
	    BE::Image::Resolution::Units::PPI);

	const uint16_t imageDPI = static_cast<uint16_t>(
	    std::round(resolution.xRes));

	cv::Mat postResample {};

	if ((resolution.xRes != resolution.yRes) || (imageDPI != 500)) {
		// Possible re-sampling
		if (flags.force) {
			resampled = true;
			// Re-sample by force
			try {
				cv::Mat preResample { static_cast<int>(
							  imageHeight),
					static_cast<int>(imageWidth), CV_8U,
					grayscaleRawData };
				NFIR::resample(preResample, postResample,
				    imageDPI, 500, "", "");

			} catch (const cv::Exception &e) {
				logger->printError(name, fingerPosition, 255,
				    "'Error: Matrix creation error: " + e.msg +
					"'",
				    quantized, resampled);
				return;

			} catch (const std::exception &e) {
				const std::string errStr { e.what() };
				logger->printError(name, fingerPosition, 255,
				    "'" + errStr + "'", quantized, resampled);
				return;
			}

		} else {
			if (interactive && !flags.force) {
				const std::string prompt =
				    "This Image is not 500 PPI. Would you "
				    "like "
				    "to re-sample this image to 500 PPI?";
				const bool response = NFIQ2UI::yesOrNo(
				    prompt, false, true, true);

				if (response) {
					resampled = true;
					// Re-sample the image
					try {
						cv::Mat preResample {
							static_cast<int>(
							    imageHeight),
							static_cast<int>(
							    imageWidth),
							CV_8U, grayscaleRawData
						};
						NFIR::resample(preResample,
						    postResample, imageDPI, 500,
						    "", "");

					} catch (const cv::Exception &e) {
						logger->printError(name,
						    fingerPosition, 255,
						    "'Error: Matrix creation error: " +
							e.msg + "'",
						    quantized, resampled);
						return;

					} catch (const std::exception &e) {
						const std::string errStr {
							e.what()
						};
						logger->printError(name,
						    fingerPosition, 255,
						    "'" + errStr + "'",
						    quantized, resampled);
						return;
					}

				} else {
					// User decided not to re-sample
					logger->debugMsg(
					    "User denied the re-sample");
					if (!singleImage) {
						logger->printError(name,
						    fingerPosition, 255,
						    "'Error: User chose not to "
						    "re-sample image'",
						    quantized, resampled);
					}
					return;
				}

			} else {
				logger->printError(name, fingerPosition, 255,
				    "'Error: Image is not 500PPI'", quantized,
				    resampled);
				return;
			}
		}
	}
	logger->debugMsg("Successfully passed 500 PPI check. Re-sampled?: " +
	    std::to_string(resampled));

	// At this point - all images are 500PPI or have been converted to that
	// resolution. Quantization will happen below if necessary.

	const NFIQ::FingerprintImageData wrappedImage = resampled ?
	    NFIQ::FingerprintImageData(postResample.data, postResample.total(),
		postResample.cols, postResample.rows, fingerPosition, 500) :
	    NFIQ::FingerprintImageData(grayscaleRawData,
		grayscaleRawData.size(), imageWidth, imageHeight,
		fingerPosition, imageDPI);

	NFIQ2UI::CoreReturn corereturn;

	try {
		corereturn = NFIQ2UI::coreCompute(wrappedImage, model);
	} catch (const NFIQ::NFIQException &e) {
		const std::string expMsg { e.what() };
		logger->printError(name, fingerPosition, 255,
		    "'NFIQ2 computeQualityScore returned an error code: " +
			expMsg + "'",
		    quantized, resampled);
		return;
	}

	// Print score:
	if (singleImage) {
		// print just the plain score to std::out
		logger->printSingle(corereturn.qualityScore);

	} else {
		// Print full score with optional headers
		logger->printScore(name, fingerPosition,
		    corereturn.qualityScore, warning, quantized, resampled,
		    corereturn.featureVector, corereturn.featureTimings,
		    corereturn.actionableQuality);
	}
}

void
NFIQ2UI::executeSingle(const NFIQ2UI::ImgCouple &couple, const Flags &flags,
    const NFIQ::NFIQ2Algorithm &model, std::shared_ptr<NFIQ2UI::Log> logger,
    const bool singleImage, const bool interactive)
{
	NFIQ2UI::executeSingle(couple.img, couple.imgName, flags, model, logger,
	    singleImage, interactive, couple.fingerPosition, couple.warning);
}

// Wrapper for NFIQ2 computeQualityScore()
NFIQ2UI::CoreReturn
NFIQ2UI::coreCompute(const NFIQ::FingerprintImageData &wrappedImage,
    const NFIQ::NFIQ2Algorithm &model)
{
	// compute quality now
	// call wrapper class with fingerprint image to get score
	// input is always raw image with set image parameters
	std::list<NFIQ::ActionableQualityFeedback> actionableQuality;
	std::list<NFIQ::QualityFeatureData> featureVector;
	std::list<NFIQ::QualityFeatureSpeed> featureTimings;

	unsigned int qualityScore = 255;

	try {
		qualityScore = model.computeQualityScore(wrappedImage, true,
		    actionableQuality, true, featureVector, true,
		    featureTimings);
	} catch (const NFIQ::NFIQException &e) {
		throw e;
	}

	const CoreReturn corereturn { featureVector, featureTimings,
		actionableQuality, qualityScore };
	return corereturn;
}

// Parsing a directory recursively finding all fingerprint images
void
NFIQ2UI::parseDirectory(const std::string &dirname, const Flags &flags,
    const NFIQ::NFIQ2Algorithm &model, std::shared_ptr<NFIQ2UI::Log> logger)
{
	// Uses dirent to iterate through a directory
	DIR *dr;
	struct dirent *en;
	dr = opendir(dirname.c_str());

	// Checks if opening the directory was successful
	if (dr != nullptr) {
		// Reading in each file within the directory, ignores "." and
		// ".."
		while ((en = readdir(dr)) != nullptr) {
			if (std::string(en->d_name) != "." &&
			    std::string(en->d_name) != "..") {
				// Checks for directories within directories and
				// recursively calls itself
				if (flags.recursion &&
				    BE::IO::Utility::pathIsDirectory(
					dirname + "/" + en->d_name)) {
					// need to also check for recordstores
					// here
					logger->debugMsg(
					    "Recursively scanning inner "
					    "directories to process "
					    "images");

					if (NFIQ2UI::isRecordStore(
						NFIQ2UI::removeSlash(dirname) +
						"/" + en->d_name)) {
						NFIQ2UI::executeRecordStore(
						    NFIQ2UI::removeSlash(
							dirname) +
							"/" + en->d_name,
						    flags, model, logger);
					} else {
						NFIQ2UI::parseDirectory(
						    NFIQ2UI::removeSlash(
							dirname) +
							"/" + en->d_name,
						    flags, model, logger);
					}
				} else {
					// Tries to executeSingle on each image
					// in the directory
					if (!BE::IO::Utility::pathIsDirectory(
						dirname + en->d_name)) {
						logger->debugMsg(
						    "Removing slashes from "
						    "filename");

						const auto images =
						    NFIQ2UI::getImages(
							NFIQ2UI::removeSlash(
							    dirname) +
							    "/" + en->d_name,
							logger);

						for (const auto &image :
						    images) {
							NFIQ2UI::executeSingle(
							    image, flags, model,
							    logger, false,
							    true);
						}
					}
				}
			}
		}
		closedir(dr);
	}
}

void
NFIQ2UI::batchConsume(NFIQ2UI::SafeSplitPathsQueue &splitQueue,
    SafeQueue<std::string> &printQueue, const Flags &flags,
    const NFIQ::NFIQ2Algorithm &model)
{
	std::shared_ptr<NFIQ2UI::ThreadedLog> threadedlogger =
	    std::make_shared<NFIQ2UI::ThreadedLog>(flags);

	while (!splitQueue.isEmpty()) {
		// Pop from the produced queue
		const auto item = splitQueue.pop();

		// Iterate through the vector and produce a score for each image
		for (const auto &j : item) {
			const auto images = NFIQ2UI::getImages(
			    j, threadedlogger);

			for (const auto &image : images) {
				NFIQ2UI::executeSingle(image, flags, model,
				    threadedlogger, false, false);
				// Push these scores to another queue that will
				// get processed by the printing thread
				printQueue.push(
				    threadedlogger->getAndClearLastScore());
			}
		}
	}
}

void
NFIQ2UI::executeBatch(const std::string &filename, const Flags &flags,
    const NFIQ::NFIQ2Algorithm &model, std::shared_ptr<NFIQ2UI::Log> logger)
{
	std::vector<std::string> content;
	unsigned int count;

	std::tie(content, count) = NFIQ2UI::getFileContent(filename);

	if (flags.numthreads == 1) {
		// Single Threaded:

		for (const auto &i : content) {
			const auto images = NFIQ2UI::getImages(i, logger);

			for (const auto &image : images) {
				executeSingle(
				    image, flags, model, logger, false, false);
			}
		}

	} else {
		// Multi Threaded:

		// Modify this to change the splitting factor
		const std::vector<std::string>::size_type splittingFactor = 2;

		// Split paths up into chunks using SafeQueue subclass
		NFIQ2UI::SafeSplitPathsQueue splitQueue(
		    content, splittingFactor);

		SafeQueue<std::string> printQueue;
		printQueue.setNumThreads(flags.numthreads);

		unsigned int upperThreadBound = flags.numthreads;

		if (flags.numthreads > (count / splittingFactor)) {
			upperThreadBound = static_cast<unsigned int>(
			    std::ceil(count / splittingFactor) + 1);
		}

		// Start consumer threads
		std::vector<std::thread> threads;
		for (unsigned int i { 0 }; i < upperThreadBound; ++i) {
			try {
				threads.emplace_back(std::bind(&batchConsume,
				    std::ref(splitQueue), std::ref(printQueue),
				    flags, std::cref(model)));
			} catch (const std::exception &e) {
				std::cerr << "Error during thread creation: "
					  << e.what() << "\n";
				return;
			}
		}

		// Start printing thread
		std::thread printThread(
		    threadedPrint, std::ref(printQueue), logger);

		// Join consumer threads
		for (auto &i : threads) {
			try {
				i.join();

			} catch (const std::exception &e) {
				std::cerr << "Error during thread joining: "
					  << e.what() << "\n";
				return;
			}
		}

		printQueue.setNumThreads(0);

		// Join printing thread
		printThread.join();
	}
}

void
NFIQ2UI::recordStoreConsume(const std::string &name,
    NFIQ2UI::SafeSplitPathsQueue &splitQueue,
    SafeQueue<std::string> &printQueue, const Flags &flags,
    const NFIQ::NFIQ2Algorithm &model)
{
	std::shared_ptr<BE::IO::RecordStore> rs {};

	std::shared_ptr<NFIQ2UI::ThreadedLog> threadedlogger =
	    std::make_shared<NFIQ2UI::ThreadedLog>(flags);

	threadedlogger->debugMsg("Attempting to parse RecordStore.");
	try {
		rs = BE::IO::RecordStore::openRecordStore(name);
	} catch (const BE::Error::Exception &e) {
		std::string error { "'Error: Could not open RecordStore'" };
		threadedlogger->printError(
		    name, 0, 255, error.append(e.what()), false, false);
		return;
	}

	while (!splitQueue.isEmpty()) {
		// Pop from the produced queue
		const auto item = splitQueue.pop();

		// Iterate through the vector and produce a score for each image
		for (const auto &j : item) {
			const auto data = rs->read(j);
			const auto images = NFIQ2UI::getImages(
			    data, j, threadedlogger);

			for (const auto &image : images) {
				NFIQ2UI::executeSingle(image, flags, model,
				    threadedlogger, false, false);
				// Push these scores to another queue that will
				// get processed by the printing thread
				printQueue.push(
				    threadedlogger->getAndClearLastScore());
			}
		}
	}
}

void
NFIQ2UI::executeRecordStore(const std::string &filename, const Flags &flags,
    const NFIQ::NFIQ2Algorithm &model, std::shared_ptr<NFIQ2UI::Log> logger)
{
	std::shared_ptr<BE::IO::RecordStore> rs {};

	logger->debugMsg("Attempting to parse RecordStore.");
	try {
		rs = BE::IO::RecordStore::openRecordStore(filename);
	} catch (const BE::Error::Exception &e) {
		std::string error { "'Error: Could not open RecordStore'" };
		logger->printError(
		    filename, 0, 255, error.append(e.what()), false, false);
		return;
	}

	// Single Threaded

	if (flags.numthreads == 1) {
		logger->debugMsg(
		    "Successfully parsed RecordStore: " + filename);

		for (const auto &rec : (*rs)) {
			logger->debugMsg(
			    "Getting Images from record: " + rec.key);

			const std::vector<NFIQ2UI::ImgCouple> images =
			    NFIQ2UI::getImages(rec.data, rec.key, logger);

			for (const auto &image : images) {
				logger->debugMsg(
				    "Iterating through images in record: " +
				    rec.key + ", calling executeSingle");

				NFIQ2UI::executeSingle(
				    image, flags, model, logger, false, false);
			}
		}
	} else {
		// Multi threaded

		// Modify this to change the splitting factor
		const std::vector<std::string>::size_type splittingFactor = 2;
		// This value needs to be the total images for each record
		const unsigned int count = rs->getCount();

		NFIQ2UI::SafeSplitPathsQueue splitQueue(rs, splittingFactor);

		// Can call threaded to print out scores

		SafeQueue<std::string> printQueue;
		printQueue.setNumThreads(flags.numthreads);

		unsigned int upperThreadBound = flags.numthreads;

		if (flags.numthreads > (count / splittingFactor)) {
			upperThreadBound = static_cast<unsigned int>(
			    std::ceil(count / splittingFactor) + 1);
		}

		// Start consumer threads
		std::vector<std::thread> threads;
		for (unsigned int i { 0 }; i < upperThreadBound; ++i) {
			try {
				threads.emplace_back(std::bind(
				    &recordStoreConsume, filename,
				    std::ref(splitQueue), std::ref(printQueue),
				    flags, std::cref(model)));
			} catch (const std::exception &e) {
				std::cerr << "Error during thread creation: "
					  << e.what() << "\n";
				return;
			}
		}

		// Start printing thread
		std::thread printThread(
		    threadedPrint, std::ref(printQueue), logger);

		// Join consumer threads
		for (auto &i : threads) {
			try {
				i.join();
			} catch (const std::exception &e) {
				std::cerr << "Error during thread joining: "
					  << e.what() << "\n";
				return;
			}
		}
		printQueue.setNumThreads(0);

		// Join printing thread
		printThread.join();
	}
}

void
NFIQ2UI::threadedPrint(
    SafeQueue<std::string> &printQueue, std::shared_ptr<NFIQ2UI::Log> logger)
{
	while (printQueue.getNumThreads() != 0) {
		if (!printQueue.isEmpty()) {
			const auto item = printQueue.pop();
			logger->printThreaded(item);
		}
	}
}

// Processes getopt arguments
NFIQ2UI::Arguments
NFIQ2UI::processArguments(int argc, char **argv)
{
	NFIQ2UI::Flags flags {};

	std::vector<std::string> vecSingle {};
	std::vector<std::string> vecDirs {};
	std::vector<std::string> vecBatch {};
	std::vector<std::string> vecRecordStore {};

	std::string output {};

	static const char options[] { "i:f:o:j:vqdFrm:a" };
	int c {};

	auto vecPush = [&](const std::string &m) {
		if (NFIQ2UI::isRecordStore(m)) {
			vecRecordStore.push_back(m);
		} else if (BE::IO::Utility::pathIsDirectory(m)) {
			vecDirs.push_back(m);
		} else {
			vecSingle.push_back(m);
		}
	};

	while ((c = getopt(argc, argv, options)) != -1)
		switch (c) {
		case 'i':
			vecPush(optarg);
			break;
		case 'f':
			vecBatch.push_back(optarg);
			break;
		case 'o':
			output = optarg;
			break;
		case 'j':
			flags.numthreads = checkThreads(optarg);
			break;
		case 'v':
			flags.verbose = true;
			break;
		case 'q':
			flags.speed = true;
			break;
		case 'd':
			flags.debug = true;
			break;
		case 'F':
			flags.force = true;
			break;
		case 'r':
			flags.recursion = true;
			break;
		case 'm':
			flags.model = optarg;
			break;
		case 'a':
			flags.actionable = true;
			break;
		case '?':
			NFIQ2UI::printUsage();
			throw NFIQ2UI::UndefinedFlagError(
			    "Undefined Flag Used");
		}

	for (auto &i = optind; i < argc; i++) {
		vecPush(argv[i]);
	}

	if (flags.numthreads != 1 &&
	    (vecBatch.empty() && vecRecordStore.empty())) {
		throw NFIQ2UI::InvalidArgumentError(
		    "User cannot use threading flag for single-threaded operations. "
		    "\nBatch "
		    "files and recordstores are the only multi-threaded operations.");
	}

	NFIQ2UI::Arguments arguments = { flags, argv[0], output, vecSingle,
		vecDirs, vecBatch, vecRecordStore };
	return arguments;
}

void
NFIQ2UI::procSingle(NFIQ2UI::Arguments arguments,
    const NFIQ::NFIQ2Algorithm &model, std::shared_ptr<NFIQ2UI::Log> logger)
{
	logger->debugMsg("Processing Singles: ");
	// If there is only one image being processed
	if (arguments.vecSingle.size() == 1 && arguments.vecDirs.size() == 0 &&
	    arguments.vecBatch.size() == 0 && !arguments.flags.verbose &&
	    !arguments.flags.speed && !arguments.flags.actionable) {
		const auto images = NFIQ2UI::getImages(
		    arguments.vecSingle[0], logger);

		for (const auto &image : images) {
			NFIQ2UI::executeSingle(
			    image, arguments.flags, model, logger, true, true);
		}
	} else {
		for (const auto &i : arguments.vecSingle) {
			const auto images = NFIQ2UI::getImages(i, logger);

			for (const auto &image : images) {
				NFIQ2UI::executeSingle(image, arguments.flags,
				    model, logger, false, true);
			}
		}
	}
}

// Only prints header if it is needed
void
NFIQ2UI::printHeader(
    NFIQ2UI::Arguments arguments, std::shared_ptr<NFIQ2UI::Log> logger)
{
	if ((arguments.vecSingle.size() == 1 &&
		(arguments.flags.verbose || arguments.flags.speed ||
		    arguments.flags.actionable)) ||
	    (arguments.vecSingle.size() == 1 &&
		NFIQ2UI::isAN2K(arguments.vecSingle[0])) ||
	    arguments.vecSingle.size() > 1 || arguments.vecDirs.size() != 0 ||
	    arguments.vecBatch.size() != 0 ||
	    arguments.vecRecordStore.size() != 0) {
		logger->printCSVHeader();
	}
}

NFIQ::ModelInfo
NFIQ2UI::parseModelInfo(const NFIQ2UI::Arguments &arguments)
{
	static const std::string DefaultModelInfoFilename {
		"nist_plain_tir-ink.txt"
	};
	static const std::string ShareDirLocalUnix { "/usr/local/nfiq2/share" };
	static const std::string ShareDirWin32 {
		"C:/Program Files (x86)/NFIQ 2/bin"
	};
	static const std::string ShareDirWin64 {
		"C:/Program Files/NFIQ 2/bin"
	};

	std::string modelInfoFilePath {};

	if (arguments.flags.model.empty()) {
		// Check common places for directory containing model
		for (const auto &dir : std::vector<std::string> { ".",
			 BE::Text::dirname(arguments.argv0), ShareDirLocalUnix,
			 ShareDirWin64, ShareDirWin32 }) {
			if (BE::IO::Utility::fileExists(
				dir + '/' + DefaultModelInfoFilename)) {
				modelInfoFilePath = dir + '/' +
				    DefaultModelInfoFilename;
				break;
			}
		}

		if (modelInfoFilePath.empty()) {
			throw NFIQ2UI::FileNotFoundError(
			    "No model info provided and default model info '" +
			    DefaultModelInfoFilename + "' not found");
		}
	} else {
		// Use -m defined path
		modelInfoFilePath = arguments.flags.model;
	}

	NFIQ::ModelInfo modelInfoObj {};
	try {
		modelInfoObj = NFIQ::ModelInfo(modelInfoFilePath);
	} catch (const NFIQ::NFIQException &e) {
		throw NFIQ2UI::ModelConstructionError(
		    "Could not construct ModelInfo Object from: " +
		    modelInfoFilePath + ". Error: " + e.what());
	}

	if (!BE::IO::Utility::fileExists(modelInfoObj.getModelPath())) {
		throw NFIQ2UI::PropertyParseError("Unable to parse '" +
		    NFIQ::ModelInfo::ModelInfoKeyPath + "' from '" +
		    modelInfoFilePath + "' (No file exists at '" +
		    modelInfoObj.getModelPath() + "')");
	}

	return modelInfoObj;
}

int
main(int argc, char **argv)
{
	if (argc < 2) {
		NFIQ2UI::printUsage();
		return EXIT_SUCCESS;
	}

	NFIQ2UI::Arguments arguments {};
	try {
		arguments = NFIQ2UI::processArguments(argc, argv);
	} catch (const NFIQ2UI::UndefinedFlagError &e) {
		std::cerr << e.what() << "\n";
		NFIQ2UI::printUndefinedFlag();
		return EXIT_FAILURE;
	} catch (const NFIQ2UI::InvalidArgumentError &e) {
		std::cerr << e.what() << "\n";
		return EXIT_FAILURE;
	}

	std::shared_ptr<NFIQ2UI::Log> logger {};
	try {
		logger = std::make_shared<NFIQ2UI::Log>(
		    arguments.flags, arguments.output);
	} catch (const NFIQ2UI::FileOpenError &e) {
		std::cerr << "Error: Could not create logger object. "
			  << e.what() << "\n";
		return EXIT_FAILURE;
	}

	// Initialize Model
	NFIQ::Timer timerInit;
	double timeInit = 0.0;
	timerInit.startTimer();

	NFIQ::ModelInfo modelInfoObj {};

	try {
		modelInfoObj = NFIQ::ModelInfo(
		    NFIQ2UI::parseModelInfo(arguments));

	} catch (const NFIQ2UI::Exception &e) {
		std::cerr << "Unable to extract model information. " << e.what()
			  << "\n";
		return EXIT_FAILURE;
	}

	logger->debugMsg("Model Name: " +
	    (modelInfoObj.getModelName().empty() ?
		    "<NA>" :
		    modelInfoObj.getModelName()));
	logger->debugMsg("Model Trainer: " +
	    (modelInfoObj.getModelTrainer().empty() ?
		    "<NA>" :
		    modelInfoObj.getModelTrainer()));
	logger->debugMsg("Model Description: " +
	    (modelInfoObj.getModelDescription().empty() ?
		    "<NA>" :
		    modelInfoObj.getModelDescription()));
	logger->debugMsg("Model Version: " +
	    (modelInfoObj.getModelVersion().empty() ?
		    "<NA>" :
		    modelInfoObj.getModelVersion()));
	logger->debugMsg("Model Path: " + modelInfoObj.getModelPath());
	logger->debugMsg("Model Hash: " + modelInfoObj.getModelHash());

	std::shared_ptr<NFIQ::NFIQ2Algorithm> model {};
	try {
		model = std::make_shared<NFIQ::NFIQ2Algorithm>(modelInfoObj);
	} catch (NFIQ::NFIQException &e) {
		std::cerr << "Model could not be constructed. " << e.what()
			  << "\n";
		return EXIT_FAILURE;
	}

	timeInit = timerInit.endTimerAndGetElapsedTime();

	std::stringstream loggerStream;
	loggerStream << "Model Initialization: " << std::setprecision(3)
		     << std::fixed << timeInit << " ms";

	logger->debugMsg(loggerStream.str());
	loggerStream.str("");
	loggerStream.clear();

	// Printing values of flags
	logger->debugMsg("Value of verbose flag: " +
	    std::to_string(arguments.flags.verbose));
	logger->debugMsg(
	    "Value of debug flag: " + std::to_string(arguments.flags.debug));
	logger->debugMsg(
	    "Value of speed flag: " + std::to_string(arguments.flags.speed));
	logger->debugMsg(
	    "Value of force flag: " + std::to_string(arguments.flags.force));
	logger->debugMsg("Value of model flag: " + arguments.flags.model);
	logger->debugMsg("Value of recursive flag: " +
	    std::to_string(arguments.flags.recursion));

	// Prints Header
	NFIQ2UI::printHeader(arguments, logger);

	// Process single images - includes AN2K files
	logger->debugMsg("Processing Singles and AN2K files:");
	NFIQ2UI::procSingle(arguments, *model, logger);

	logger->debugMsg("Processing Directories:");
	for (const auto &i : arguments.vecDirs) {
		NFIQ2UI::parseDirectory(i, arguments.flags, *model, logger);
	}

	logger->debugMsg("Processing Batch-files:");
	for (const auto &i : arguments.vecBatch) {
		NFIQ2UI::executeBatch(i, arguments.flags, *model, logger);
	}

	logger->debugMsg("Processing RecordStores:");
	for (const auto &i : arguments.vecRecordStore) {
		NFIQ2UI::executeRecordStore(i, arguments.flags, *model, logger);
	}

	return EXIT_SUCCESS;
}
