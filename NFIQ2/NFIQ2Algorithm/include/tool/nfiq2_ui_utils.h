/******************************************************************************
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 ******************************************************************************/

#ifndef NFIQ2_UI_UTILS_H_
#define NFIQ2_UI_UTILS_H_

#include <be_io_utility.h>
#include <tool/nfiq2_ui_types.h>

#include <cmath>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

namespace NFIQ2UI {

/**
 *  @brief
 *  This file contains utility functions used throughout this NFIQ2 tool.
 */

/**
 *  @brief
 *  Returns the version of the Biometric Evaluation Framework Library.
 *
 *  @details
 *  Returns the version of the Biomeval library being used by CLI tool.
 *
 *  @return
 *    A string containing version information.
 */
std::string getBiomevalVersion();

/**
 *  @brief
 *  Identifies a Standard Image from other FileTypes.
 *
 *  @details
 *  Used to determine how Images will be gathered from a data-blob.
 *
 *  @param[in] dataArray
 *      The FileType of this dataArray is the return type.
 *
 *  @return
 *    The enumerated type of the data-blob.
 */
NFIQ2UI::FileType getFileType(
    const BiometricEvaluation::Memory::uint8Array &dataArray);

/**
 *  @brief
 *  Converts AN2K file path to a data-blob. Calls isAN2K on data-blob.
 *
 *  @param[in] path
 *      Path to AN2K file.
 *
 *  @return
 *    Boolean indicating whether the file is an AN2K file.
 */
bool isAN2K(const std::string &path);

/**
 *  @brief
 *  Checks the starting bytes to determine if the data-blob is an AN2K
 *  Record.
 *
 *  @param[in] dataArray
 *      Data-blob to be checked.
 *
 *  @return
 *    Boolean indicating whether the data-blob is of AN2K format.
 */
bool isAN2K(const BiometricEvaluation::Memory::uint8Array &dataArray);

/**
 *  @brief
 *  Checks if a path is a RecordStore.
 *
 *  @param[in] path
 *      Path to be checked.
 *
 *  @return
 *    Boolean indicating whether the path is a RecordStore.
 */
bool isRecordStore(const std::string &path);

/**
 *  @brief
 *  Returns the content of a batch file along with its line count.
 *
 *  @param[in] filename
 *      Batch file to be read.
 *
 *  @return
 *    Tuple containing all of the content from the batch file and
 *    the line count.
 */
std::tuple<std::vector<std::string>, int> getFileContent(
    const std::string &filename);

/**
 *  @brief
 *  Removes an extra slash in a path to preserve original argument.
 *
 *  @param[in] path
 *      Path string to be modified.
 *
 *  @return
 *    The modified pathname in string form.
 */
std::string removeSlash(std::string path);

/**
 *  @brief
 *  Yes or No prompt.
 *
 *  @details
 *  Used for when the User can choose whether they would
 *  Like an image to be quantized or re-sampled.
 *
 *  Default options are "no"
 *
 *  @param[in] prompt
 *    String prompt asking the user a question.
 *  @param[in] default_answer
 *    Default answer if user does not provide a response
 *  @param[in] show_options
 *    Shows the possible options (yes/no)
 *  @param[in] allow_default_answer
 *    Determines if the default answer will be accepted
 *
 *  @return
 *    Boolean indicating the user's choice (Y/N, T/F)
 */
bool yesOrNo(const std::string &prompt, bool default_answer = true,
    bool show_options = true, bool allow_default_answer = true);

/**
 *  @brief
 *  Checks the given thread amount against the physical cores
 *  available on the system.
 *
 *  @details
 *  If the inputted amount is greater
 *  than the total amount of physical cores, the user is
 *  asked if they would like to change to using n cores
 *  to maintain system stability.
 *
 *  @param[in] threadArg
 *    The number of desired threads to create for Multi-Threaded
 *    operations.
 *
 *  @return
 *    The number of threads to be used by Multi-threaded operations.
 */
unsigned int checkThreads(const std::string &threadArg);

std::string formatDouble(const double &d, const uint8_t precision);

/**
 *  @brief
 *  Prints information on how to use this NFIQ2 tool
 */
void printUsage();

/**
 *  @brief
 *  Prints warning message if undefined flag is given
 */
void printUndefinedFlag();

// WIP ////////////////////////////////////////////////////////////////////////
bool isANSI2004(); // const BiometricEvaluation::Memory::uint8Array &dataArray

} // namespace NFIQ2UI

#endif /* NFIQ2_UI_UTILS_H_ */
