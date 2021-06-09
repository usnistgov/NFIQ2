#include <nfiq2_fingerprintimagedata.hpp>

int debug = 0;

#include <opencv2/imgproc.hpp>

#include <cstring>

static double computeMuFromRow(unsigned int rowIndex, const cv::Mat &img);
static double computeMuFromColumn(unsigned int columnIndex, const cv::Mat &img);

NFIQ2::FingerprintImageData::FingerprintImageData()
    : Data()
    , width(0)
    , height(0)
    , fingerCode(0)
    , ppi(NFIQ2::FingerprintImageData::Resolution500PPI)
{
}

NFIQ2::FingerprintImageData::FingerprintImageData(
    uint32_t width, uint32_t height, uint8_t fingerCode, uint16_t ppi)
    : Data()
    , width(width)
    , height(height)
    , fingerCode(fingerCode)
    , ppi(ppi)
{
}

NFIQ2::FingerprintImageData::FingerprintImageData(const uint8_t *pData,
    uint32_t dataSize, uint32_t width_, uint32_t height_, uint8_t fingerCode_,
    uint16_t ppi_)
    : Data(pData, dataSize)
    , width(width_)
    , height(height_)
    , fingerCode(fingerCode_)
    , ppi(ppi_)
{
}

NFIQ2::FingerprintImageData::FingerprintImageData(
    const FingerprintImageData &otherData)
    : Data(otherData)
{
	width = otherData.width;
	height = otherData.height;
	fingerCode = otherData.fingerCode;
	ppi = otherData.ppi;
}

NFIQ2::FingerprintImageData::~FingerprintImageData() = default;

NFIQ2::FingerprintImageData
NFIQ2::FingerprintImageData::copyRemovingNearWhiteFrame() const
{
	/**
	 * Pixel intensity threshold used for determining whitespace
	 * around fingerprint. Consecutive rows <= this value starting on each
	 * edge shall be removed.
	 */
	static const double MU_THRESHOLD { 250 };

	cv::Mat img;
	try {
		// get matrix from fingerprint image
		img = cv::Mat(
		    this->height, this->width, CV_8UC1, (void *)this->data());
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
	const cv::Rect roi(leftIndex, topRowIndex, width, height);
	const cv::Mat roiImg = img(roi);

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
	croppedImage.height = roiImg.rows;
	croppedImage.width = roiImg.cols;
	croppedImage.fingerCode = this->fingerCode;
	croppedImage.ppi = this->ppi;
	// copy data now
	const unsigned int size = roiImg.rows * roiImg.cols;
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
computeMuFromRow(unsigned int rowIndex, const cv::Mat &img)
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
computeMuFromColumn(unsigned int columnIndex, const cv::Mat &img)
{
	double mu = 0.0;
	for (int i = 0; i < img.rows; i++) {
		// get gray value of image (0 = black, 255 = white)
		mu += (double)img.at<uchar>(i, columnIndex);
	}

	mu /= img.rows;
	return mu;
}
