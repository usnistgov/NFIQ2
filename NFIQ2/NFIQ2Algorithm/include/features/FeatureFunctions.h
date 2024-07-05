#ifndef FEATUREFUNCTIONS_H
#define FEATUREFUNCTIONS_H

#include <nfiq2_constants.hpp>
#include <opencv2/core.hpp>

#include <unordered_map>

namespace NFIQ2 { namespace QualityFeatures {

void ridgesegment(const cv::Mat &Image, int blksze, double thresh,
    cv::OutputArray NormImage, cv::Mat &MaskImage, cv::OutputArray MaskIndex);

double ridgeorient(double a, double b, double c);

void diffGrad(const cv::Mat &inBlock, cv::Mat &outBlock);

typedef enum {
	SOBEL_OPERATOR = 0,
	CENTERED_DIFFERENCES,
} ocl_type;

void covcoef(const cv::Mat &imblock, double &a, double &b, double &c,
    ocl_type compMethod);

double ocl(double a, double b, double c);

uint8_t allfun(const cv::Mat &image);

void getRotatedBlock(const cv::Mat &block, const double orientation,
    bool padFlag, cv::Mat &rotatedBlock);

void getRidgeValleyStructure(const cv::Mat &blockCropped,
    std::vector<uint8_t> &ridval, std::vector<double> &dt);
void Conv2D(const cv::Mat &im, const cv::Mat &filter, cv::Mat &ConvOut,
    const cv::Size &imageSize, const cv::Size &dftSize);
void GaborFilterCx(const int ksize, const double theta, const double freq,
    const int sigma, cv::Mat &FilterOut);

double calccoh(double gxx, double gyy, double gxy);

double calcof(double gsxavg, double gsyavg);

cv::Mat computeNumericalGradientX(const cv::Mat &mat);
void computeNumericalGradients(const cv::Mat &mat, cv::Mat &grad_x,
    cv::Mat &grad_y);

void
addHistogramFeatures(std::unordered_map<std::string, double> &featureDataList,
    std::string featurePrefix, std::vector<double> &binBoundaries,
    std::vector<double> &dataVector, int binCount);
void addSamplingFeatureNames(std::vector<std::string> &featureNames,
    const char *prefix);
void addHistogramFeatureNames(std::vector<std::string> &featureNames,
    const std::string &prefix, int binCount);

/**
 * @brief
 * Computation of sigmoid function defined in ISO/IEC 29794-4:2024,
 * Clause 6.1.1.1.
 *
 * @param nativeQuality
 * Native quality measure value.
 * @param inflectionPoint
 * Inflection point at which the function has the value 0.5.
 * @param scaling
 * Scaling parameter determining the width of the region.
 *
 * @return
 * Point on sigmoid curve.
 *
 * @see ISO/IEC 29794-4:2024, Clause 6.1.1.1.
 */
double sigmoid(const double nativeQuality, const double inflectionPoint,
    const double scaling);

/**
 * @brief
 * Map certain native quality measure values between 0-100 inclusive.
 *
 * @param nativeQuality
 * Native quality measure value.
 * @param minNativeQuality
 * Minimum observable value for the native quality value in question.
 * @param maxNativeQuality
 * Maximum observable value for the native quality value in question.
 *
 * @return
 * Mapping of `nativeQuality` between 0-100 inclusive.
 *
 * @see ISO/IEC 29794-4:2024, Clause 6.1.1.2.
 */
uint8_t knownRange(const double nativeQuality, const double minNativeQuality,
    const double maxNativeQuality);

/**
 * @brief
 * Obtain the [0, 100] mapped value of a native quality measure value.
 *
 * @param featureIdentifier
 * Identifier for the feature (from nfiq2_constants.hpp)
 * @param native
 * Native quality measure value.
 *
 * @return
 * Mapped quality component value.
 *
 * @throw Exception
 * Unrecognized `featureIdentifier`.
 */
uint8_t getQualityBlockValue(const std::string &featureIdentifier,
    const double native);

}}
#endif
