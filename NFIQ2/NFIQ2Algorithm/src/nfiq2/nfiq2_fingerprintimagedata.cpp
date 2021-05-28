#include <nfiq2_fingerprintimagedata.hpp>

int debug = 0;

#include <opencv2/imgproc.hpp>

#include <cstring>

static double computeMuFromRow(unsigned int rowIndex, cv::Mat &img);
static double computeMuFromColumn(unsigned int columnIndex, cv::Mat &img);

NFIQ2::FingerprintImageData::FingerprintImageData()
    : Data()
    , m_ImageWidth(0)
    , m_ImageHeight(0)
    , m_FingerCode(0)
    , m_ImagePPI(NFIQ2::FingerprintImageData::Resolution500PPI)
{
}

NFIQ2::FingerprintImageData::FingerprintImageData(uint32_t imageWidth,
    uint32_t imageHeight, uint8_t fingerCode, uint16_t imagePPI)
    : Data()
    , m_ImageWidth(imageWidth)
    , m_ImageHeight(imageHeight)
    , m_FingerCode(fingerCode)
    , m_ImagePPI(imagePPI)
{
}

NFIQ2::FingerprintImageData::FingerprintImageData(const uint8_t *pData,
    uint32_t dataSize, uint32_t imageWidth, uint32_t imageHeight,
    uint8_t fingerCode, uint16_t imagePPI)
    : Data(pData, dataSize)
    , m_ImageWidth(imageWidth)
    , m_ImageHeight(imageHeight)
    , m_FingerCode(fingerCode)
    , m_ImagePPI(imagePPI)
{
}

NFIQ2::FingerprintImageData::FingerprintImageData(
    const FingerprintImageData &otherData)
    : Data(otherData)
{
	m_ImageWidth = otherData.m_ImageWidth;
	m_ImageHeight = otherData.m_ImageHeight;
	m_FingerCode = otherData.m_FingerCode;
	m_ImagePPI = otherData.m_ImagePPI;
}

NFIQ2::FingerprintImageData::~FingerprintImageData()
{
}

NFIQ2::FingerprintImageData
NFIQ2::FingerprintImageData::removeWhiteFrameAroundFingerprint() const
{
	// make local copy of internal fingerprint image
	NFIQ2::FingerprintImageData localFingerprintImage(this->m_ImageWidth,
	    this->m_ImageHeight, this->m_FingerCode, this->m_ImagePPI);
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
	} catch (const cv::Exception &e) {
		std::stringstream ssErr;
		ssErr << "Cannot get matrix from fingerprint image: "
		      << e.what();
		throw NFIQ2::Exception(
		    NFIQ2::ErrorCode::FeatureCalculationError, ssErr.str());
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

	static const uint16_t fingerJetMinWidth = 196;
	static const uint16_t fingerJetMaxWidth = 800;
	static const uint16_t fingerJetMinHeight = 196;
	static const uint16_t fingerJetMaxHeight = 1000;

	// Values are from FJFX image size thresholds
	if (roiImg.cols <= fingerJetMinWidth) {
		throw NFIQ2::Exception(NFIQ2::ErrorCode::InvalidImageSize,
		    "Width is too small after trimming whitespace. WxH: " +
			std::to_string(roiImg.cols) + "x" +
			std::to_string(roiImg.rows) +
			", but minimum width is " +
			std::to_string(fingerJetMinWidth + 1));
	} else if (roiImg.cols >= fingerJetMaxWidth) {
		throw NFIQ2::Exception(NFIQ2::ErrorCode::InvalidImageSize,
		    "Width is too large after trimming whitespace. WxH: " +
			std::to_string(roiImg.cols) + "x" +
			std::to_string(roiImg.rows) +
			", but maximum width is " +
			std::to_string(fingerJetMaxWidth - 1));
	} else if (roiImg.rows <= fingerJetMinHeight) {
		throw NFIQ2::Exception(NFIQ2::ErrorCode::InvalidImageSize,
		    "Height is too small after trimming whitespace. WxH: " +
			std::to_string(roiImg.cols) + "x" +
			std::to_string(roiImg.rows) +
			", but minimum height is " +
			std::to_string(fingerJetMinHeight + 1));
	} else if (roiImg.rows >= fingerJetMaxHeight) {
		throw NFIQ2::Exception(NFIQ2::ErrorCode::InvalidImageSize,
		    "Height is too large after trimming whitespace. WxH: " +
			std::to_string(roiImg.cols) + "x" +
			std::to_string(roiImg.rows) +
			", but maximum height is " +
			std::to_string(fingerJetMaxHeight - 1));
	}

	NFIQ2::FingerprintImageData croppedImage;
	croppedImage.m_ImageHeight = roiImg.rows;
	croppedImage.m_ImageWidth = roiImg.cols;
	croppedImage.m_FingerCode = this->m_FingerCode;
	croppedImage.m_ImagePPI = this->m_ImagePPI;
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
