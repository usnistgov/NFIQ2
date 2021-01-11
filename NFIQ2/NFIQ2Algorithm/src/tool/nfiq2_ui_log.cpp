/******************************************************************************
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 ******************************************************************************/

#include <NFIQ2Algorithm.h>

#include "nfiq2_ui_exception.h"
#include "nfiq2_ui_log.h"
#include "nfiq2_ui_types.h"
#include "nfiq2_ui_utils.h"

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
    const std::list<NFIQ::QualityFeatureData> &featureVector,
    const std::list<NFIQ::QualityFeatureSpeed> &featureTimings,
    const std::list<NFIQ::ActionableQualityFeedback> &actionableQuality) const
{
	*(this->out) << name << "," << std::to_string(fingerCode) << ","
		     << score << "," << errmsg << "," << quantized << ","
		     << resampled;
	if (this->actionable || this->verbose || this->speed) {
		*(this->out) << ",";
	}

	// Print out actionable first
	if (this->actionable) {
		for (auto i = actionableQuality.begin();
		     i != actionableQuality.end(); ++i) {
			if (i != actionableQuality.begin()) {
				*(this->out) << ",";
			}

			*(this->out) << NFIQ2UI::formatDouble(
			    i->actionableQualityValue, 5);
		}
		if (this->verbose || this->speed) {
			*(this->out) << ",";
		}
	}

	if (this->verbose) {
		for (auto i = featureVector.begin(); i != featureVector.end();
		     ++i) {
			if (i != featureVector.begin()) {
				*(this->out) << ",";
			}

			*(this->out)
			    << NFIQ2UI::formatDouble(i->featureDataDouble, 5);
		}
		if (this->speed) {
			*(this->out) << ",";
		}
	}

	if (this->speed) {
		for (auto i = featureTimings.begin(); i != featureTimings.end();
		     ++i) {
			if (i != featureTimings.begin()) {
				*(this->out) << ",";
			}

			*(this->out) << std::setprecision(5) << i->featureSpeed;
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
	for (auto i = 0; i < padding; i++) {
		strNA += ",NA";
	}

	return strNA;
}

// Prints out an error score if the image was unable to be processed correctly
void
NFIQ2UI::Log::printError(const std::string &name, uint8_t fingerCode,
    unsigned int score, const std::string &errmsg, const bool quantized,
    const bool resampled) const
{
	*(this->out) << name << "," << std::to_string(fingerCode) << ","
		     << score << "," << errmsg << "," << quantized << ","
		     << resampled << padNA() << "\n";
}

// Prints the quality score of a single image
void
NFIQ2UI::Log::printSingle(unsigned int qualityScore) const
{
	*(this->out) << qualityScore << "\n";
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
	*(this->out) << "Filename"
		     << ","
		     << "FingerCode"
		     << ","
		     << "QualityScore"
		     << ","
		     << "OptionalError"
		     << ","
		     << "Quantized"
		     << ","
		     << "Resampled";
	if (this->actionable || this->verbose || this->speed) {
		*(this->out) << ",";
	}

	if (this->actionable) {
		std::vector<std::string> vHeaders =
		    NFIQ::NFIQ2Algorithm::getAllActionableIdentifiers();

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
		    NFIQ::NFIQ2Algorithm::getAllQualityFeatureIDs();

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
		    NFIQ::NFIQ2Algorithm::getAllSpeedFeatureGroups();

		for (auto it = sHeaders.begin(); it != sHeaders.end(); ++it) {
			if (it != sHeaders.begin()) {
				*(this->out) << ',';
			}
			std::replace(it->begin(), it->end(), ' ', '_');
			*(this->out) << *it;
		}
	}
	*(this->out) << "\n";
}

NFIQ2UI::Log::~Log()
{
	this->out = nullptr;
}
