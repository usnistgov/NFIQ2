/******************************************************************************
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 ******************************************************************************/

#include <nfiq2_qualityfeatures.hpp>
#include <tool/nfiq2_ui_exception.h>
#include <tool/nfiq2_ui_log.h>
#include <tool/nfiq2_ui_types.h>
#include <tool/nfiq2_ui_utils.h>

#include <cctype>
#include <iomanip>
#include <iostream>
#include <string>

// Responsible for all print outputs
NFIQ2UI::Log::Log(const Flags &flags, const std::string &path)
{
	this->verbose = flags.verbose;
	this->debug = flags.debug;
	this->speed = flags.speed;
	this->actionable = flags.actionable;

	if (path.empty()) {
		out = &std::cout;
	} else {
		this->logFile.open(path);
		if (!this->logFile) {
			throw NFIQ2UI::FileOpenError(
			    "Logger could not open file: " + path);
		}
		// Set output to designated file
		this->out = &(this->logFile);
	}
}

// Prints the qualityScore of the Image
void
NFIQ2UI::Log::printScore(const std::string &name, uint8_t fingerCode,
    unsigned int score, const std::string &errmsg, const bool quantized,
    const bool resampled,
    const std::unordered_map<std::string, double> &features,
    const std::unordered_map<std::string, double> &speed,
    const std::unordered_map<std::string, double> &actionable) const
{
	*(this->out) << "\"" << name << "\""
		     << "," << std::to_string(fingerCode) << "," << score << ","
		     << NFIQ2UI::sanitizeErrorMsg(errmsg) << "," << quantized
		     << "," << resampled;
	if (this->actionable || this->verbose || this->speed) {
		*(this->out) << ",";
	}

	// Print out actionable first
	if (this->actionable) {
		const auto actionableIDs =
		    NFIQ2::QualityFeatures::getActionableQualityFeedbackIDs();
		for (const auto &i : actionableIDs) {
			if (i != actionableIDs.front()) {
				*(this->out) << ",";
			}

			*(this->out)
			    << NFIQ2UI::formatDouble(actionable.at(i), 5);
		}
		if (this->verbose || this->speed) {
			*(this->out) << ",";
		}
	}

	if (this->verbose) {
		const auto featureIDs =
		    NFIQ2::QualityFeatures::getQualityFeatureIDs();
		for (const auto &i : featureIDs) {
			if (i != featureIDs.front()) {
				*(this->out) << ",";
			}

			*(this->out)
			    << NFIQ2UI::formatDouble(features.at(i), 5);
		}
		if (this->speed) {
			*(this->out) << ",";
		}
	}

	if (this->speed) {
		const auto moduleIDs =
		    NFIQ2::QualityFeatures::getQualityModuleIDs();
		for (const auto &i : moduleIDs) {
			if (i != moduleIDs.front()) {
				*(this->out) << ",";
			}

			*(this->out) << std::setprecision(5) << speed.at(i);
		}
	}
	*(this->out) << "\n";
}

// Pad CSV output with NAs for row consistency
std::string
NFIQ2UI::Log::padNA() const
{
	static std::string strNA {};
	if (!strNA.empty()) {
		return strNA;
	}

	static const unsigned int MIN_NUM_COLS { 6 };
	unsigned int numCols { MIN_NUM_COLS };

	if (this->actionable) {
		numCols += 4;
	}

	if (this->verbose) {
		numCols += 69;
	}

	if (this->speed) {
		numCols += 10;
	}

	const unsigned int padding = numCols - MIN_NUM_COLS;
	for (unsigned int i = 0; i < padding; i++) {
		strNA += ",NA";
	}

	return strNA;
}

// Prints out an error score if the image was unable to be processed correctly
void
NFIQ2UI::Log::printError(const std::string &name, uint8_t fingerCode,
    const std::string &errmsg, const bool quantized, const bool resampled) const
{
	static const std::string errscore { "NA" };
	*(this->out) << "\"" << name << "\""
		     << "," << std::to_string(fingerCode) << "," << errscore
		     << ","
		     << "\"" << NFIQ2UI::sanitizeErrorMsg(errmsg) << "\""
		     << "," << quantized << "," << resampled << padNA() << "\n";
}

// Wrapper for printError that takes in an ImageProps
void
NFIQ2UI::Log::printError(
    const std::string &errmsg, const ImageProps &imageProps) const
{
	NFIQ2UI::Log::printError(imageProps.name, imageProps.fingerPosition,
	    errmsg, imageProps.quantized, imageProps.resampled);
}

// Prints the quality score of a single image
void
NFIQ2UI::Log::printSingle(unsigned int qualityScore) const
{
	*(this->out) << qualityScore << "\n";
}

// Prints the error of a single image
void
NFIQ2UI::Log::printSingleError(const std::string &errmsg) const
{
	*(this->out) << NFIQ2UI::sanitizeErrorMsg(errmsg) << "\n";
}

// Prints output from Multi-threaded operations
void
NFIQ2UI::Log::printThreaded(const std::string &message) const
{
	*(this->out) << message;
}

// Prints debug messages to stdout
void
NFIQ2UI::Log::debugMsg(const std::string &message) const
{
	if (this->debug) {
		*(this->out) << "DEBUG 	| " << message << "\n";
	}
}

// Prints the CSV Header
void
NFIQ2UI::Log::printCSVHeader() const
{
	*(this->out) << "\"Filename\""
		     << ","
		     << "FingerCode"
		     << ","
		     << "QualityScore"
		     << ","
		     << "\"OptionalError\""
		     << ","
		     << "Quantized"
		     << ","
		     << "Resampled";
	if (this->actionable || this->verbose || this->speed) {
		*(this->out) << ",";
	}

	if (this->actionable) {
		std::vector<std::string> vHeaders =
		    NFIQ2::QualityFeatures::getActionableQualityFeedbackIDs();

		for (auto it = vHeaders.begin(); it != vHeaders.end(); ++it) {
			if (it != vHeaders.begin()) {
				*(this->out) << ',';
			}
			*(this->out) << *it;
		}

		if (this->verbose || this->speed) {
			*(this->out) << ',';
		}
	}

	if (this->verbose) {
		std::vector<std::string> vHeaders =
		    NFIQ2::QualityFeatures::getQualityFeatureIDs();

		for (auto it = vHeaders.begin(); it != vHeaders.end(); ++it) {
			if (it != vHeaders.begin()) {
				*(this->out) << ',';
			}
			*(this->out) << *it;
		}

		if (this->speed) {
			*(this->out) << ',';
		}
	}

	if (this->speed) {
		std::vector<std::string> sHeaders =
		    NFIQ2::QualityFeatures::getQualityModuleIDs();

		for (auto it = sHeaders.begin(); it != sHeaders.end(); ++it) {
			if (it != sHeaders.begin()) {
				*(this->out) << ',';
			}
			*(this->out) << *it << "Speed";
		}
	}
	*(this->out) << "\n";
}

NFIQ2UI::Log::~Log()
{
	this->out = nullptr;
}

// Sanitize invalid characters from error messages
std::string
NFIQ2UI::sanitizeErrorMsg(const std::string &errorMsg)
{
	std::string sanitized { errorMsg };

	auto it = sanitized.begin();
	while ((it = std::find_if_not(sanitized.begin(), sanitized.end(),
		    [](const char &c) -> bool {
			    return (std::isgraph(c) || c == ' ');
		    })) != sanitized.end()) {
		sanitized.replace(it, std::next(it), " ");
	}

	static const std::string from { "\"" };
	static const std::string to { "'" };
	std::string::size_type position { 0 };
	while (
	    (position = sanitized.find(from, position)) != std::string::npos) {
		sanitized.replace(position, from.length(), to);
		position += to.length();
	}
	return sanitized;
}
