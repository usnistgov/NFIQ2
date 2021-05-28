/******************************************************************************
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 ******************************************************************************/

#include <be_data_interchange_an2k.h>
#include <be_data_interchange_ansi2004.h>
#include <be_image_image.h>
#include <be_image_raw.h>
#include <be_io_utility.h>
#include <tool/nfiq2_ui_image.h>
#include <tool/nfiq2_ui_log.h>
#include <tool/nfiq2_ui_types.h>
#include <tool/nfiq2_ui_utils.h>

#include <cmath>
#include <string>
#include <vector>

namespace BE = BiometricEvaluation;

// Returns images given a data-blob
std::vector<NFIQ2UI::ImgCouple>
NFIQ2UI::getImages(const BE::Memory::uint8Array &dataArray,
    const std::string &name, std::shared_ptr<NFIQ2UI::Log> logger)
{
	logger->debugMsg("Obtaining FileType from data: " + name);
	std::vector<NFIQ2UI::ImgCouple> vecCouple {};

	switch (NFIQ2UI::getFileType(dataArray)) {
	// Standard Image Case
	case NFIQ2UI::FileType::Standard:
		logger->debugMsg("FileType Standard: " + name);
		return NFIQ2UI::getImagesFromImage(dataArray, name, logger);

	// AN2K Case
	case NFIQ2UI::FileType::AN2K:
		logger->debugMsg("FileType AN2K: " + name);
		return NFIQ2UI::getImagesFromAN2K(dataArray, name, logger);

	// ANSI2004 Case
	case NFIQ2UI::FileType::ANSI2004:
		logger->debugMsg("FileType ANSI2004: " + name);
		return NFIQ2UI::getImagesFromANSI2004(dataArray, name, logger);

	default:
		logger->debugMsg("Image type could not be determined. " + name +
		    " will not be processed");
		logger->printError(
		    name, 0, "Error: Could not determine FileType", 0, 0);

		return vecCouple;
	}
}

// Returns images given a path
std::vector<NFIQ2UI::ImgCouple>
NFIQ2UI::getImages(
    const std::string &path, std::shared_ptr<NFIQ2UI::Log> logger)
{
	logger->debugMsg("Trying to obtain data from path: " + path);
	std::vector<NFIQ2UI::ImgCouple> vecCouple {};

	try {
		// Directory Paths do not contain images
		if (!BE::IO::Utility::pathIsDirectory(path)) {
			BE::Memory::uint8Array data = BE::IO::Utility::readFile(
			    path);
			logger->debugMsg("Obtained data from path: " + path);
			return NFIQ2UI::getImages(data, path, logger);
		}
	} catch (const BE::Error::Exception &e) {
		std::string error {
			"Error: Could not obtain data from path : "
		};
		logger->printError(
		    path, 0, error.append(e.what()), false, false);
	}

	return vecCouple;
}

// Returns Images given a Standard Image data-blob
std::vector<NFIQ2UI::ImgCouple>
NFIQ2UI::getImagesFromImage(const BE::Memory::uint8Array &dataArray,
    const std::string &name, std::shared_ptr<NFIQ2UI::Log> logger)
{
	std::vector<NFIQ2UI::ImgCouple> vecCouple {};

	try {
		std::shared_ptr<BE::Image::Image> img =
		    BE::Image::Image::openImage(dataArray, name);

		logger->debugMsg(
		    "Successfully parsed image from data-blob: " + name);

		vecCouple.emplace_back(img, 0, name, "NA");

	} catch (const BE::Error::Exception &e) {
		// Unable to open the image
		std::string error { "Error: Could not open image : " };
		logger->printError(name, 0, error.append(e.what()), 0, 0);
	}
	return vecCouple;
}

// Returns Images given a AN2K data-blob
std::vector<NFIQ2UI::ImgCouple>
NFIQ2UI::getImagesFromAN2K(const BE::Memory::uint8Array &dataArray,
    const std::string &name, std::shared_ptr<NFIQ2UI::Log> logger)
{
	logger->debugMsg(
	    "Trying to obtain images from ANSI/NIST Record: " + name);
	std::vector<NFIQ2UI::ImgCouple> vecCouple {};
	std::shared_ptr<BE::DataInterchange::AN2KRecord> an2k;

	try {
		an2k = std::make_shared<BE::DataInterchange::AN2KRecord>(
		    const_cast<BE::Memory::uint8Array &>(dataArray));

	} catch (const BE::Error::Exception &e) {
		std::string error {
			"Error AN2K Record could not be opened : "
		};
		logger->printError(name, 0, error.append(e.what()), 0, 0);
		return vecCouple;
	}

	logger->debugMsg("Successfully constructed AN2KRecord");

	const std::vector<BE::Finger::AN2KViewCapture> captures =
	    an2k->getFingerCaptures();

	for (const auto &cap : captures) {
		// Image is valid and in memory provided that the AN2K record
		// was constructed correctly
		std::shared_ptr<BE::Image::Image> img = cap.getImage();
		const auto fingerPosition =
		    BE::Framework::Enumeration::to_int_type(cap.getPosition());

		logger->debugMsg("Successfully parsed image from AN2KRecord: " +
		    name + "_" + std::to_string(fingerPosition));

		// Finger position must be 0-12 for reliable NFIQ2 score
		if (fingerPosition >= 0 && fingerPosition <= 12) {
			logger->debugMsg(
			    "Capture is a valid NFIQ2 fingerPosition");

			std::string warning = "NA";

			const uint16_t imagePPI = static_cast<uint16_t>(
			    std::round(
				img->getResolution()
				    .toUnits(BE::Image::Resolution::Units::PPI)
				    .xRes));
			const uint16_t an2kPPI = static_cast<uint16_t>(
			    std::round(
				cap.getImageResolution()
				    .toUnits(BE::Image::Resolution::Units::PPI)
				    .xRes));

			// Check if there is a mismatch in resolution between
			// the data-blob and the ANSI/NIST record
			if (imagePPI != an2kPPI) {
				logger->debugMsg(
				    "Resolution mismatch between data and "
				    "ANSI/NIST "
				    "Record. Using ANSI/NIST Resolution");

				warning = "Image Resolution does not match "
					  "resolution indicated "
					  "in ANSI/NIST file. Continuing NFIQ2 "
					  "computation with "
					  "ANSI/NIST resolution";

				try {
					// Create new Raw image with ANSI/NIST
					// Record resolution
					img = std::make_shared<BE::Image::Raw>(
					    img->getRawData(),
					    img->getDimensions(),
					    img->getColorDepth(),
					    img->getBitDepth(),
					    cap.getImageResolution(),
					    img->hasAlphaChannel(),
					    name + "_" +
						std::to_string(fingerPosition));
				} catch (const BE::Error::Exception &e) {
					// Could not construct revised image,
					// using original image
					warning = warning + e.what();
				}
			}

			vecCouple.emplace_back(img,
			    static_cast<uint8_t>(fingerPosition),
			    name + "_" + std::to_string(fingerPosition),
			    warning);

		} else {
			logger->debugMsg(
			    "Invalid fingerprint position provided: " + name +
			    "_" + std::to_string(fingerPosition));

			logger->printError(
			    name + "_" + std::to_string(fingerPosition),
			    static_cast<uint8_t>(fingerPosition),
			    "Error: Invalid FingerPosition for NFIQ2 (not "
			    "0-12)",
			    0, 0);
		}
	}
	return vecCouple;
}

// Work in Progress ///////////////////////////////////////////////////////////
// Returns Images given a ANSI2004 data-blob
std::vector<NFIQ2UI::ImgCouple>
NFIQ2UI::getImagesFromANSI2004(const BE::Memory::uint8Array &dataArray,
    const std::string &name, std::shared_ptr<NFIQ2UI::Log> logger)
{
	std::vector<NFIQ2UI::ImgCouple> vecCouple {};

	BiometricEvaluation::Memory::uint8Array empty {};

	std::shared_ptr<BE::DataInterchange::ANSI2004Record> ansi2004;

	logger->debugMsg("Trying to make ANSI2004 Record");

	try {
		// Make Ansi2004Record and collect fingerprint captures
		ansi2004 =
		    std::make_shared<BE::DataInterchange::ANSI2004Record>(
			empty, dataArray);
	} catch (const BE::Error::Exception &e) {
		// Could not make ANSI2004Record
		std::string error {
			"ERROR: ANSI2004 RECORD COULD NOT BE OPENED : "
		};
		logger->printError(name, 0, error.append(e.what()), 0, 0);
		return vecCouple;
	}

	logger->debugMsg("Successfully Made ANSI2004 Record");

	uint64_t printCount = ansi2004->getNumFingerViews();

	for (uint64_t i = 0; i < printCount; i++) {
		BE::Finger::ANSI2004View cap = ansi2004->getView(i);

		std::shared_ptr<BE::Image::Image> img = cap.getImage();
		const auto fingerPosition =
		    BE::Framework::Enumeration::to_int_type(cap.getPosition());

		logger->debugMsg(
		    "Successfully parsed image from ANSI2004 Record: " + name +
		    "_" + std::to_string(fingerPosition));

		// Finger position must be 0-12 for reliable NFIQ2 score
		if (fingerPosition >= 0 && fingerPosition <= 12) {
			logger->debugMsg(
			    "Capture is a valid NFIQ2 fingerPosition");

			std::string warning = "NA";

			const uint16_t imagePPI = static_cast<uint16_t>(
			    std::round(
				img->getResolution()
				    .toUnits(BE::Image::Resolution::Units::PPI)
				    .xRes));
			const uint16_t an2kPPI = static_cast<uint16_t>(
			    std::round(
				cap.getImageResolution()
				    .toUnits(BE::Image::Resolution::Units::PPI)
				    .xRes));

			// Check if there is a mismatch in resolution between
			// the data-blob and the ANSI2004 record
			if (imagePPI != an2kPPI) {
				logger->debugMsg(
				    "Resolution mismatch between data and "
				    "ANSI2004 "
				    "Record. Using ANSI2004 Resolution");

				warning = "Image Resolution does not match "
					  "resolution indicated "
					  "in ANSI2004 file. Continuing NFIQ2 "
					  "computation with "
					  "ANSI2004 resolution";

				try {
					// Create new Raw image with ANSI/NIST
					// Record resolution
					img = std::make_shared<BE::Image::Raw>(
					    img->getRawData(),
					    img->getDimensions(),
					    img->getColorDepth(),
					    img->getBitDepth(),
					    cap.getImageResolution(),
					    img->hasAlphaChannel(),
					    name + "_" +
						std::to_string(fingerPosition));
				} catch (const BE::Error::Exception &e) {
					// Could not construct revised image,
					// using original image
					warning = warning + e.what();
				}
			}

			vecCouple.emplace_back(img,
			    static_cast<uint8_t>(fingerPosition),
			    name + "_" + std::to_string(fingerPosition),
			    warning);

		} else {
			logger->debugMsg(
			    "Invalid fingerprint position provided: " + name +
			    "_" + std::to_string(fingerPosition));

			logger->printError(
			    name + "_" + std::to_string(fingerPosition),
			    static_cast<uint8_t>(fingerPosition),
			    "Error: Invalid finger print position for "
			    "NFIQ2",
			    0, 0);
		}
	}

	return vecCouple;
}
