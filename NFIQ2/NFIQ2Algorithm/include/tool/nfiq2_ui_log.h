/******************************************************************************
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 ******************************************************************************/

#ifndef NFIQ2_UI_LOG_H_
#define NFIQ2_UI_LOG_H_

#include <nfiq2_algorithm.hpp>

#include "nfiq2_ui_types.h"

#include <fstream>
#include <iostream>
#include <string>

namespace NFIQ2UI {

/**
 *  @brief
 *  Logging class
 *
 *  @details
 *  Logger is used to print debug, verbose and speed statements.
 *  It is also used to print Scores to either stdout or a designated file.
 */
class Log {
    public:
	/**
	 *  @brief
	 *    Construct a Log object with passed in flags and optional path.
	 *
	 *    @details
	 *    Default path will print to stdout.
	 *
	 *    @param[in] flags
	 *    Argument flags passed into the command line.
	 *    @param[in] path
	 *    Path to the output file.
	 */
	Log(const Flags &flags, const std::string &path = {});

	/**
	 *  @brief
	 *    Print the score for an Image in CSV format.
	 *
	 *  @details
	 *    Prints score generated from NFIQ2.
	 *
	 *  @param[in] name
	 *    The name of the image.
	 *  @param[in] fingerCode
	 *    Finger position of the image. Valid values: 0-12.
	 *  @param[in] score
	 *    Calculated NFIQ2 Score.
	 *  @param[in] errmsg
	 *    Error message if applicable. Will be "NA" otherwise.
	 *  @param[in] quantized
	 *    If the image was quantized 0 = not quantized, 1 = quantized.
	 *  @param[in] resampled
	 *    If the image was resampled 0 = not resampled, 1 = resampled.
	 *  @param[in] featureVector
	 *    Prints featureVector information if verbose flag is enabled.
	 *  @param[in] featureTimings
	 *    Prints featureTimings information if verbose flag is enabled.
	 */
	void printScore(const std::string &name, uint8_t fingerCode,
	    unsigned int score, const std::string &errmsg, const bool quantized,
	    const bool resampled,
	    const std::unordered_map<std::string, double> &features,
	    const std::unordered_map<std::string, double> &speed,
	    const std::unordered_map<std::string, double> &actionable) const;

	/**
	 *  @brief
	 *  Pads an error score with NA values to provide consistency when CSV
	 * output is used in an outside data processing application
	 *
	 *  @details
	 *  Checks to see whether Verbose/Quality and/or Speed flags have been
	 *  enabled. If so, the amount of padding will vary.
	 *
	 *  @return
	 *    The padded NA string.
	 */
	std::string padNA() const;

	/**
	 *  @brief
	 *    Print the score for an Image in CSV format.
	 *
	 *  @details
	 *    Prints Error scores generated from NFIQ2 for images that failed
	 *    processing.
	 *    Error 'NA' is printed if image could not be evaluated and
	 * padding is added to ensure consistency.
	 *
	 *  @param[in] name
	 *    The name of the image.
	 *  @param[in] fingerCode
	 *    Finger position of the image. Valid values: 0-12.
	 *  @param[in] errmsg
	 *    Error message if applicable. Will be "NA" otherwise
	 *  @param[in] quantized
	 *    If the image was quantized 0 = not quantized, 1 = quantized.
	 *  @param[in] resampled
	 *    If the image was resampled 0 = not resampled, 1 = resampled.
	 */
	void printError(const std::string &name, uint8_t fingerCode,
	    const std::string &errmsg, const bool quantized,
	    const bool resampled) const;

	/**
	 *  @brief
	 *    Wrapper around original printError method that takes an
	 * 	  imageProps struct
	 *
	 *  @param[in] errmsg
	 *    Error message if applicable. Will be "NA" otherwise
	 *  @param[in] imageProps
	 *    A struct containing image property information
	 */
	void printError(
	    const std::string &errmsg, const ImageProps &imageProps) const;

	/**
	 *  @brief
	 *  Prints a single score.
	 *
	 *  @details
	 *  Used for Single independent Image.
	 *
	 *  @param[in] qualityScore
	 *    The qualityScore to be printed out.
	 */
	void printSingle(unsigned int qualityScore) const;

	/**
	 *  @brief
	 *  Prints a single error.
	 *
	 *  @details
	 *  Used for Single independent Image.
	 *
	 *  @param[in] errmsg
	 *    The error message to be printed out.
	 */
	void printSingleError(const std::string &errmsg) const;
	/**
	 *  @brief
	 *  Prints a score produced by a Multi-Threaded operation.
	 *
	 *  @param[in] message
	 *    The score to be printed to the output stream.
	 */
	void printThreaded(const std::string &message) const;

	/**
	 *  @brief
	 *  Prints a debug message to the output stream.
	 *
	 *  @param[in] message
	 *    The message to be printed.
	 */
	void debugMsg(const std::string &message) const;

	/**
	 *  @brief
	 *  Prints CSV headers.
	 *
	 *  @details
	 *  CSV headers are dependent on:
	 *  Whether more than one image was provided to the command line.
	 *  Whether the Verbose or Speed flags were provided.
	 */
	void printCSVHeader() const;

	virtual ~Log();

    protected:
	/** output stream for scores to be printed to */
	std::ostream *out { nullptr };

    private:
	/** Value of verbose flag */
	bool verbose;
	/** Value of debug flag */
	bool debug;
	/** Value of speed flag */
	bool speed;
	/** Value of the actionable flag */
	bool actionable;
	/** Used if a specified file will be the output stream */
	std::ofstream logFile {};
};

/**
 *  @brief
 *  Sanitizes input before being sent to the output buffer.
 *
 *  @details
 *  Since this tool produces a CSV output, sanitizing new lines and quotes
 *	is critical
 *
 *  @param[in] errorMsg
 *    The string to be sanitized
 *
 *  @return
 *    The sanitized error message string
 */
std::string sanitizeErrorMsg(const std::string &errorMsg);

} // namespace NFIQ2UI

#endif /* NFIQ2_UI_LOG_H_ */
