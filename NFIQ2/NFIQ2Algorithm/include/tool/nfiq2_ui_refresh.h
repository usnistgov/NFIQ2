/******************************************************************************
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 ******************************************************************************/

#ifndef NFIQ2_UI_REFRESH_H_
#define NFIQ2_UI_REFRESH_H_

#include <be_image_image.h>
#include <be_io_utility.h>
#include <nfiq2_algorithm.hpp>
#include <nfiq2_modelinfo.hpp>
#include <opencv2/core.hpp>

#include "nfiq2_ui_log.h"
#include "nfiq2_ui_types.h"

#include <iostream>
#include <string>
#include <vector>

namespace NFIQ2UI {

/**
 * @brief
 *  Prompts the user if the supplied image is not 8 bit color depth, allowing
 *  them to quantize the image to 8 bit.
 *
 * @return
 *  Yes/No response to quantize prompt
 */
bool askIfQuantize();

/**
 * @brief
 *  Prompts the user if supplied a 72 PPI (default resolution) image.
 *
 * @details
 *  Allows the user to select whether they wish to stick with the original
 *  image resolution or if they wish to resample it.
 *
 * @param[in] name
 *  The images name
 * @param[in] imagePPI
 *  The image resolution
 * @param[in] requiredPPI
 *  The required PPI for NFIQ 2 to produce an accurate score (500PPI)
 *
 * @return
 *  Yes/No response to resample prompt
 */
bool askIfDefaultResolution(const std::string &name, const uint16_t imagePPI,
    const uint16_t requiredPPI);

/**
 * @brief
 *  Prompts the user if supplied a non 500 PPI image.
 *
 * @details
 *  Allows the user to select whether they wish to stick with the original
 *  image resolution or if they wish to resample it.
 *
 * @param[in] name
 *  The images name
 * @param[in] imagePPI
 *  The image resolution
 * @param[in] requiredPPI
 *  The required PPI for NFIQ 2 to produce an accurate score (500PPI)
 *
 * @return
 *  Yes/No response to resample prompt
 */
bool askIfResample(const std::string &name, const uint16_t imagePPI,
    const uint16_t requiredPPI);

/**
 * @brief
 * Resampling helper function that performs the resampling
 * operation on a fingerprint image
 *
 * @details
 * Has the ability to log errors if a logger is passed in,
 * if a logger is not passed in then it will throw an exception
 * to the calling function
 *
 * @param[in] grayscaleRawData
 *  The raw quantized bytes of a fingerprint image
 * @param[in] dimensionInfo
 *  A struct containing image dimension information
 * @param[in] imageProps
 *  A struct containing image property information
 * @param[in] logger
 *  Default to nullptr if there is no logger object
 *
 * @return
 *  The resampled matrix
 */
cv::Mat resampleAndLogError(
    BiometricEvaluation::Memory::uint8Array &grayscaleRawData,
    const NFIQ2UI::DimensionInfo &dimensionInfo,
    const NFIQ2UI::ImageProps &imageProps,
    std::shared_ptr<NFIQ2UI::Log> logger = nullptr);

/**
 *  @brief
 *  Executes a single Image and prints its computed NFIQ2 score.
 *
 *  @details
 *  Uses the initialized NFIQ2 model to call coreCompute and generate
 *  Image scores.
 *
 *  Uses parsed flags to modify the behavior of the function to either
 *  force operations or print additional Feature Scores, Timings and
 *  debug statements.
 *
 *  @param[in] img
 *      Image to be processed.
 *  @param[in] name
 *      Name of Image.
 *  @param[in] flags
 *      Values of optional flag command line arguments.
 *  @param[in] model
 *      Shared pointer to the Machine learning model used with NFIQ2.
 *  @param[in] logger
 *      Logger used to print scores and error codes.
 *  @param[in] singleImage
 *      Indicates whether a single Image was passed to the command line.
 *  @param[in] interactive
 *      Indicates whether yes/no prompts will be active.
 *  @param[in] fingerPosition
 *      Indicates the finger position of a particular Image (default is 0).
 *  @param[in] warning
 *      Optional warning message used in AN2K and ANSI2004 Records.
 */
void executeSingle(std::shared_ptr<BiometricEvaluation::Image::Image> img,
    const std::string &name, const Flags &flags, const NFIQ2::Algorithm &model,
    std::shared_ptr<NFIQ2UI::Log> logger, const bool singleImage,
    const bool interactive, const uint8_t fingerPosition = 0,
    const std::string &warning = "NA");

/**
 *  @brief
 *  Version of executeSingle that takes an ImgCouple.
 *
 *  @details
 *  This version of executeSingle takes a ImgCouple as an argument and
 *  calls the above executeSingle.
 *
 *  @param[in] couple
 *      Struct that contains a shared_ptr to an img, the image name, its finger
 *      position (if applicable) and an optional warning message.
 *  @param[in] flags
 *      Contains information from command line arguments.
 *  @param[in] model
 *      Machine learning model that NFIQ2 relies on for score generation.
 *  @param[in] logger
 *      Prints scores, errors and debug messages to an output stream.
 *  @param[in] singleImage
 *      Indicates whether a single Image was passed to the command line.
 *  @param[in] interactive
 *      Indicates whether yes/no prompts will be active.
 */
void executeSingle(const NFIQ2UI::ImgCouple &couple, const Flags &flags,
    const NFIQ2::Algorithm &model, std::shared_ptr<NFIQ2UI::Log> logger,
    const bool singleImage, const bool interactive);

/**
 *  @brief
 *  Uses executeSingle to iterate through a directory.
 *
 *  @details
 *  Can recursively go through directories with -r flag,
 *  otherwise it will only scan the current directory given though
 *  the command line.
 *
 *  @param[in] dirname
 *      Directory path name that will be scanned.
 *  @param[in] flags
 *      Contains information from command line arguments.
 *  @param[in] model
 *      Machine learning model that NFIQ2 relies on for score generation.
 *  @param[in] logger
 *      Prints scores, errors and debug messages to an output stream.
 */
void parseDirectory(const std::string &dirname, const Flags &flags,
    const NFIQ2::Algorithm &model, std::shared_ptr<NFIQ2UI::Log> logger);

/**
 *  @brief
 *  Batch Multi-threaded consume function.
 *
 *  @details
 *  Consumes chunked up paths. Threads will continuously take from
 *  splitQueue until empty and will push print results to printQueue.
 *
 *  @param[in] splitQueue
 *      Thread-safe Queue containing chunks of data needing to be
 *      processed by worker threads.
 *  @param[in] printQueue
 *      Thread-safe Queue containing all scores that need to be printed
 *      in a Multi-threaded operation.
 *  @param[in] flags
 *      Contains information from command line arguments.
 *  @param[in] model
 *      Machine learning model that NFIQ2 relies on for score generation.
 */
void batchConsume(SafeSplitPathsQueue &splitQueue,
    SafeQueue<std::string> &printQueue, const Flags &flags,
    const NFIQ2::Algorithm &model);

/**
 *  @brief
 *  Iterates through the lines of a given batch file, calling
 *  executeSingle.
 *
 *  @details
 *  Obtains its images by calling getImages
 *
 *  @param[in] filename
 *      Path to the batch file being processed.
 *  @param[in] flags
 *      Contains information from command line arguments.
 *  @param[in] model
 *      Machine learning model that NFIQ2 relies on for score generation.
 *  @param[in] logger
 *      Prints scores, errors and debug messages to an output stream.
 */
void executeBatch(const std::string &filename, const Flags &flags,
    const NFIQ2::Algorithm &model, std::shared_ptr<NFIQ2UI::Log> logger);

/**
 *  @brief
 *  RecordStore Multi-threaded consume function.
 *
 *  @details
 *  Consumes chunked up Records (stored as record keys). Threads will
 *  continuously take from splitQueue until empty and will push print
 *  results to printQueue.
 *
 *  @param[in] name
 *      Name of the RecordStore.
 *  @param[in] splitQueue
 *      Thread-safe Queue containing chunks of data needing to be
 *      processed by worker threads.
 *  @param[in] printQueue
 *      Thread-safe Queue containing all scores that need to be printed
 *      in a Multi-threaded operation.
 *  @param[in] flags
 *      Contains information from command line arguments.
 *  @param[in] model
 *      Machine learning model that NFIQ2 relies on for score generation.
 */
void recordStoreConsume(const std::string &name,
    NFIQ2UI::SafeSplitPathsQueue &splitQueue,
    SafeQueue<std::string> &printQueue, const Flags &flags,
    const NFIQ2::Algorithm &model);

/**
 *  @brief
 *  Opens a RecordStore and iterates through it, finding all images in each
 *  record and executes those images.
 *
 *  @details
 *  Obtains its images by calling getImages
 *
 *  @param[in] filename
 *      Name of the RecordStore.
 *  @param[in] flags
 *      Contains information from command line arguments.
 *  @param[in] model
 *      Machine learning model that NFIQ2 relies on for score generation.
 *  @param[in] logger
 *      Prints scores, errors and debug messages to an output stream.
 */
void executeRecordStore(const std::string &filename, const Flags &flags,
    const NFIQ2::Algorithm &model, std::shared_ptr<NFIQ2UI::Log> logger);

/**
 *  @brief
 *  Multi-Threaded print function.
 *
 *  @details
 *  Obtains scores from Multi-threaded operations and prints it to the
 *  output stream specified in the logger.
 *
 *  @param[in] printQueue
 *      Thread-safe Queue containing all scores that need to be printed
 *      in a Multi-threaded operation.
 *  @param[in] logger
 *      Prints scores, errors and debug messages to an output stream.
 */
void threadedPrint(
    SafeQueue<std::string> &printQueue, std::shared_ptr<NFIQ2UI::Log> logger);

/**
 *  @brief
 *  Iterates through command line arguments.
 *
 *  @details
 *  Creates vectors for each type of argument entered on the command line.
 *  This includes single image files, directories, batch files, and
 *  RecordStores. It also constructs the flag object from optional flags
 *  inputted into the command line.
 *
 *  @param[in] argc
 *      Count of arguments obtained from main.
 *  @param[in] argv
 *      Array of arguments obtained from main.
 *
 *  @return
 *      Arguments contains a collection of parsed command line arguments.
 */
NFIQ2UI::Arguments processArguments(int argc, char **argv);

/**
 *  @brief
 *  Processes single images, AN2K and ANSI2004 files.
 *
 *  @details
 *  Calls getImages and executeSingle on each path inside of
 *  arguments.vecSingle.
 *
 *  Calls executeSingle with different arguments if only one image
 *  is provided to the command line in order to simply print just
 *  the score and not CSV format.
 *
 *  @param[in] arguments
 *      Contains information about provided command line arguments, as well
 *      as all of the paths of the files that will be processed through NFIQ2.
 *  @param[in] model
 *      Machine learning model that NFIQ2 relies on for score generation.
 *  @param[in] logger
 *      Prints scores, errors and debug messages to an output stream.
 */
void procSingle(NFIQ2UI::Arguments arguments, const NFIQ2::Algorithm &model,
    std::shared_ptr<NFIQ2UI::Log> logger);

/**
 *  @brief
 *  Prints CSV Header
 *
 *  @details
 *  Checks certain conditions to make sure that printing the CSV Header
 *  is appropriate.
 *
 *  @param[in] arguments
 *      Contains information about provided command line arguments, as well
 *      as all of the paths of the files that will be processed through NFIQ2.
 *  @param[in] logger
 *      Prints scores, errors and debug messages to an output stream.
 */
void printHeader(
    NFIQ2UI::Arguments arguments, std::shared_ptr<NFIQ2UI::Log> logger);

/**
 *  @brief
 *  Extracts the model path and model hash from the NFIQ2 Random Forest model
 *
 *  @details
 *  Tries to locate the default model info file in numerous common install paths
 *  and parses it to obtain model information.
 *
 *  @param[in] arguments
 *      Contains information about provided command line arguments, as well
 *      as all of the paths of the files that will be processed through NFIQ2.
 *
 *  @return
 *      Returns a tuple containing the model path and the models hash.
 */
NFIQ2::ModelInfo parseModelInfo(const NFIQ2UI::Arguments &arguments);

} // namespace NFIQ2UI

#endif /* NFIQ2_UI_REFRESH_H_ */
