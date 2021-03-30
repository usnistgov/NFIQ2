#include <nfiq2_fingerprintimagedata.hpp>

int debug = 0;

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdlib.h>
#include <string.h>

using namespace NFIQ;

static double computeMuFromRow(unsigned int rowIndex, cv::Mat &img);
static double computeMuFromColumn(unsigned int columnIndex, cv::Mat &img);

FingerprintImageData::FingerprintImageData()
    : Data()
    , m_ImageWidth(0)
    , m_ImageHeight(0)
    , m_FingerCode(0)
    , m_ImageDPI(NFIQ::e_ImageResolution_500dpi)
{
}

FingerprintImageData::FingerprintImageData(uint32_t imageWidth,
    uint32_t imageHeight, uint8_t fingerCode, uint16_t imageDPI)
    : Data()
    , m_ImageWidth(imageWidth)
    , m_ImageHeight(imageHeight)
    , m_FingerCode(fingerCode)
    , m_ImageDPI(imageDPI)
{
}

FingerprintImageData::FingerprintImageData(const uint8_t *pData,
    uint32_t dataSize, uint32_t imageWidth, uint32_t imageHeight,
    uint8_t fingerCode, uint16_t imageDPI)
    : Data(pData, dataSize)
    , m_ImageWidth(imageWidth)
    , m_ImageHeight(imageHeight)
    , m_FingerCode(fingerCode)
    , m_ImageDPI(imageDPI)
{
}

FingerprintImageData::FingerprintImageData(
    const FingerprintImageData &otherData)
    : Data(otherData)
{
	m_ImageWidth = otherData.m_ImageWidth;
	m_ImageHeight = otherData.m_ImageHeight;
	m_FingerCode = otherData.m_FingerCode;
	m_ImageDPI = otherData.m_ImageDPI;
}

FingerprintImageData::~FingerprintImageData()
{
}

NFIQ::FingerprintImageData
FingerprintImageData::removeWhiteFrameAroundFingerprint() const
{
	// make local copy of internal fingerprint image
	NFIQ::FingerprintImageData localFingerprintImage(this->m_ImageWidth,
	    this->m_ImageHeight, this->m_FingerCode, this->m_ImageDPI);
	// copy data now
	localFingerprintImage.resize(this->size());
	memcpy(
	    (void *)localFingerprintImage.data(), this->data(), this->size());

	cv::Mat img;
	try {
		// get matrix from fingerprint image
		img = cv::Mat(localFingerprintImage.m_ImageHeight,
		    localFingerprintImage.m_ImageWidth, CV_8UC1,
		    (void *)localFingerprintImage.data());
	} catch (cv::Exception &e) {
		std::stringstream ssErr;
		ssErr << "Cannot get matrix from fingerprint image: "
		      << e.what();
		throw NFIQ::NFIQException(
		    NFIQ::e_Error_FeatureCalculationError, ssErr.str());
	}

	// start from top of image and find top row index that is already part
	// of the fingerprint image
	int topRowIndex = 0;
	for (int i = 0; i < img.rows; i++) {
		double mu = computeMuFromRow(i, img);
		if (mu <= MU_THRESHOLD) {
			// Mu is not > threshold anymore -> top row index found
			if (i == 0) {
				topRowIndex = i;
			} else {
				topRowIndex = (i - 1);
			}
			break;
		}
	}

	// start from bottom of image and find bottom row index that is already
	// part of the fingerprint image
	int bottomRowIndex = (img.rows - 1);
	for (int i = (img.rows - 1); i >= 0; i--) {
		double mu = computeMuFromRow(i, img);
		if (mu <= MU_THRESHOLD) {
			// Mu is not > threshold anymore -> bottom row index
			// found
			if (i == (img.rows - 1)) {
				bottomRowIndex = i;
			} else {
				bottomRowIndex = (i + 1);
			}
			break;
		}
	}

	// start from left of image and find left index that is already part of
	// the fingerprint image
	int leftIndex = 0;
	for (int j = 0; j < img.cols; j++) {
		double mu = computeMuFromColumn(j, img);
		if (mu <= MU_THRESHOLD) {
			// Mu is not > threshold anymore -> left index found
			if (j == 0) {
				leftIndex = j;
			} else {
				leftIndex = (j - 1);
			}
			break;
		}
	}

	// start from right of image and find right index that is already part
	// of the fingerprint image
	int rightIndex = (img.cols - 1);
	for (int j = (img.cols - 1); j >= 0; j--) {
		double mu = computeMuFromColumn(j, img);
		if (mu <= MU_THRESHOLD) {
			// Mu is not > threshold anymore -> right index found
			if (j == (img.cols - 1)) {
				rightIndex = j;
			} else {
				rightIndex = (j + 1);
			}
			break;
		}
	}

	// now crop image according to detected border indices
	int width = rightIndex - leftIndex + 1;
	if (width <= 0) {
		leftIndex = 0;
		width = img.cols;
	}
	int height = bottomRowIndex - topRowIndex + 1;
	if (height <= 0) {
		topRowIndex = 0;
		height = img.rows;
	}
	cv::Rect roi(leftIndex, topRowIndex, width, height);
	cv::Mat roiImg = img(roi);

	NFIQ::FingerprintImageData croppedImage;
	croppedImage.m_ImageHeight = roiImg.rows;
	croppedImage.m_ImageWidth = roiImg.cols;
	croppedImage.m_FingerCode = this->m_FingerCode;
	croppedImage.m_ImageDPI = this->m_ImageDPI;
	// copy data now
	unsigned int size = roiImg.rows * roiImg.cols;
	croppedImage.resize(size);
	unsigned int counter = 0;
	for (int i = 0; i < roiImg.rows; i++) {
		for (int j = 0; j < roiImg.cols; j++) {
			croppedImage.at(counter) = roiImg.at<uchar>(i, j);
			counter++;
		}
	}

	return croppedImage;
}

double
computeMuFromRow(unsigned int rowIndex, cv::Mat &img)
{
	double mu = 0.0;
	for (int j = 0; j < img.cols; j++) {
		// get gray value of image (0 = black, 255 = white)
		mu += (double)img.at<uchar>(rowIndex, j);
	}

	mu /= img.cols;
	return mu;
}

double
computeMuFromColumn(unsigned int columnIndex, cv::Mat &img)
{
	double mu = 0.0;
	for (int i = 0; i < img.rows; i++) {
		// get gray value of image (0 = black, 255 = white)
		mu += (double)img.at<uchar>(i, columnIndex);
	}

	mu /= img.rows;
	return mu;
}
