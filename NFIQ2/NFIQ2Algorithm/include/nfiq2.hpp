/*
 * This file is part of NIST Fingerprint Image Quality (NFIQ) 2. For more
 * information on this project, refer to:
 *   - https://nist.gov/services-resources/software/nfiq2
 *   - https://github.com/usnistgov/NFIQ2
 *
 * This work is in the public domain. For complete licensing details, refer to:
 *   - https://github.com/usnistgov/NFIQ2/blob/master/LICENSE.md
 */

#ifndef NFIQ2_HPP_
#define NFIQ2_HPP_

#include <nfiq2_algorithm.hpp>
#include <nfiq2_constants.hpp>
#include <nfiq2_data.hpp>
#include <nfiq2_exception.hpp>
#include <nfiq2_fingerprintimagedata.hpp>
#include <nfiq2_modelinfo.hpp>
#include <nfiq2_qualitymeasures.hpp>
#include <nfiq2_timer.hpp>
#include <nfiq2_version.hpp>

/**
 * @mainpage
 * @section Overview
 * NIST Fingerprint Image Quality (NFIQ) 2 is open source software that links
 * image quality of optical and ink 500 PPI plain impression fingerprints to
 * operational recognition performance. This allows quality values to be
 * tightly defined and then numerically calibrated, which in turn allows for
 * the standardization needed to support a worldwide deployment of fingerprint
 * sensors with universally interpretable image qualities. Quality measures
 * computed are formally standardized as part of [ISO/IEC 29794-4][1]. This
 * source serves as an officially-recognized reference implementation of the
 * standard.
 *
 * @section api Application Programming Interface
 * This webpage details the application programming interface (API) for NFIQ 2.
 * If you are not a software developer and only need to compute ISO/IEC 29794-4
 * unified quality scores---often colloquially referred to as "NFIQ 2 quality
 * scores" for 500 PPI optical or ink fingerprint images, **please** navigate to
 * the [releases page][2] and download a pre-compiled version of our example
 * application.
 *
 * @subsection v23_changes_intro v2.3 API Changes
 * Due to underlying terminology changes, the API changed in v2.3. A [crosswalk
 * of the new public API methods](#v22tov23) is available.
 *
 * @section quick_score Quick Start
 * Follow these high-level steps to compute an unified quality score in your own
 * application.
 *
 *   1. Include `nfiq2.hpp` in your source file.
 *      - NIST's [releases][2] install headers in `/usr/local/nfiq2/include` on
 *        macOS and Linux, `C:\Program Files\NFIQ 2\include` on Windows 64-bit,
 *        and `C:\Program Files (x86)\NFIQ 2\include` on Windows 32-bit.
 *   2. Instantiate a NFIQ2::ModelInfo that points to the included
 *      [random forest model parameters][3], and use it to instantiate an
 *      NFIQ2::Algorithm.
 *   3. Instantiate a NFIQ2::FingerprintImageData with a *decompressed*
 *      fingerprint image.
 *   4. Pass the NFIQ2::FingerprintImageData from 3 to
 *      NFIQ2::Algorithm::computeUnifiedQualityScore of the NFIQ2::Algorithm
 *      instantiated in 2. A unified quality score is returned or an
 *      NFIQ2::Exception is thrown.
 *      * **Note**: If the calling application needs to inspect individual
 *        native quality measures, computation speeds, or quality components
 *        compatible with ISO/IEC 39794-2 quality blocks, first call
 *        NFIQ2::QualityMeasures::computeNativeQualityMeasureAlgorithms or
 *        NFIQ2::QualityMeasures::computeNativeQualityMeasures with the image
 *        from 2 instead, and then pass the result of that method to
 *        NFIQ2::Algorithm::computeUnifiedQualityScore. Use the other methods
 *        within NFIQ2::QualityMeasures to isolate the individual values
 *        required.
 *
 * @section Examples
 * Some examples of using the API can be found in the [examples directory][4] in
 * the NFIQ 2 repository.
 *
 * @section Contact
 * If you found a bug and can provide steps to reliably reproduce it, or if you
 * have a feature request, please [open an issue on GitHub][5]. Other questions
 * may be addressed to the project maintainers by emailing nfiq2@nist.gov.
 *
 * @section License
 * This work is in the public domain. For complete licensing details, refer to
 * [LICENSE.md][6].
 *
 * [1]: https://www.iso.org/standard/83827.html
 * [2]: https://github.com/usnistgov/NFIQ2/releases
 * [3]:
 * https://github.com/usnistgov/NFIQ2/blob/master/NFIQ2/nist_plain_tir-ink.txt
 * [4]: https://github.com/usnistgov/NFIQ2/tree/master/examples
 * [5]: https://github.com/usnistgov/NFIQ2/issues
 * [6]: https://github.com/usnistgov/NFIQ2/blob/master/LICENSE.md
 */

/**
 * @brief
 * Classes and functions that contribute to the calculation of unified quality
 * scores.
 */
namespace NFIQ2 {
}

#endif /* NFIQ2_HPP_ */
