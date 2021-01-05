/******************************************************************************
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 ******************************************************************************/

#ifndef NFIQ2_UI_IMAGE_H_
#define NFIQ2_UI_IMAGE_H_

#include <be_io_utility.h>

#include "nfiq2_ui_log.h"
#include "nfiq2_ui_types.h"

#include <string>
#include <vector>

namespace NFIQ2UI {

/**
 *  @brief
 *  Obtains Images from image paths and records within recordstores.
 *
 *  @details
 *  Functions in this file all work to return a vector of ImgCouple.
 *
 *  This struct contains all pertinent information for each image needed
 *  for NFIQ2 computation.
 *
 *  These functions also log error scores if an data-blob or a image within
 *  a data-blob is unable to be parsed.
 */

/**
 *  @brief
 *  Core getImages function.
 *
 *  @details
 *  Switches on the FileType of a data-blob.
 *
 *  Calls the respective getImages function depending on the FileType
 *  of the data-blob.
 *
 *  @param[in] dataArray
 *    The data-blob containing images.
 *  @param[in] name
 *    The name of the data-blob.
 *  @param[in] logger
 *    The logger used for printing scores and debug statements.
 *
 *  @return
 *    Vector of ImgCouple containing pertinent information for all
 *    images contained in a data-blob.
 */

std::vector<NFIQ2UI::ImgCouple> getImages(
    const BiometricEvaluation::Memory::uint8Array &dataArray,
    const std::string &name, std::shared_ptr<NFIQ2UI::Log> logger);

/**
 *  @brief
 *  Converts file path to a data-blob.
 *
 *  @details
 *  Calls getImages using the data-blob produced.
 *
 *  @param[in] path
 *    The path containing images.
 *  @param[in] logger
 *    The logger used for printing scores and debug statements.
 *
 *  @return
 *    Vector of ImgCouple containing pertinent information for all
 *    images contained in a data-blob.
 */
std::vector<NFIQ2UI::ImgCouple> getImages(
    const std::string &path, std::shared_ptr<NFIQ2UI::Log> logger);

/**
 *  @brief
 *  Return images from Standard Images
 *
 *  @details
 *  Standard images include images with the file-extensions
 *  .png, .jpeg, .wsq, etc. This does not include AN2K or ANSI2004
 *  records.
 *
 *  @param[in] imageData
 *    The data-blob containing images.
 *  @param[in] name
 *    The name of the data-blob.
 *  @param[in] logger
 *    The logger used for printing scores and debug statements.
 *
 *  @return
 *    Vector of ImgCouple containing pertinent information for all
 *    images contained in a data-blob.
 */
std::vector<NFIQ2UI::ImgCouple> getImagesFromImage(
    const BiometricEvaluation::Memory::uint8Array &imageData,
    const std::string &name, std::shared_ptr<NFIQ2UI::Log> logger);

/**
 *  @brief
 *  Return images from AN2K Records
 *
 *  @param[in] an2kData
 *    The data-blob containing an AN2K record.
 *  @param[in] name
 *    The name of the data-blob.
 *  @param[in] logger
 *    The logger used for printing scores and debug statements.
 *
 *  @return
 *    Vector of ImgCouple containing pertinent information for all
 *    images contained in a data-blob.
 */
std::vector<NFIQ2UI::ImgCouple> getImagesFromAN2K(
    const BiometricEvaluation::Memory::uint8Array &an2kData,
    const std::string &name, std::shared_ptr<NFIQ2UI::Log> logger);

/**
 *  @brief
 *  Return images from ANSI2004 Records
 *
 *  @param[in] ansiData
 *    The data-blob containing an ANSI 2004 file.
 *  @param[in] name
 *    The name of the data-blob.
 *  @param[in] logger
 *    The logger used for printing scores and debug statements.
 *
 *  @return
 *    Vector of ImgCouple containing pertinent information for all
 *    images contained in a data-blob.
 */
std::vector<NFIQ2UI::ImgCouple> getImagesFromANSI2004(
    const BiometricEvaluation::Memory::uint8Array &ansiData,
    const std::string &name, std::shared_ptr<NFIQ2UI::Log> logger);

} // namespace NFIQ2UI

#endif /* NFIQ2_UI_IMAGE_H_ */
