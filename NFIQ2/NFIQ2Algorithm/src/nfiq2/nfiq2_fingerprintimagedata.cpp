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

NFIQ2::FingerprintImageData::FingerprintImageData(uint32_t width,
    uint32_t height, uint8_t fingerCode, uint16_t ppi)
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
		img = cv::Mat(this->height, this->width, CV_8UC1,
		    (void *)this->data());
	} catch (const cv::Exception &e) {
		std::stringstream ssErr;
		ssErr << "Cannot get matrix from fingerprint image: "
		      << e.what();
		throw NFIQ2::Exception(
		    NFIQ2::ErrorCode::FeatureCalculationError, ssErr.str());
	}

	// start from top of image and find top row index that is already part
	// of the fingerprint image
	int topRowIndex { 0 }, bottomRowIndex { img.rows - 1 };
	for (; topRowIndex < img.rows; ++topRowIndex) {
		if (computeMuFromRow(topRowIndex, img) <= MU_THRESHOLD) {
			break;
		}
	}

	// If we traversed all rows and never found data, we can stop
	if (topRowIndex >= img.rows) {
		throw NFIQ2::Exception { NFIQ2::ErrorCode::InvalidImageSize,
			"All image rows appear to be blank" };
	} else {
		// start from bottom of image and find bottom row index that is
		// already part of the fingerprint image
		for (; bottomRowIndex >= topRowIndex; --bottomRowIndex) {
			if (computeMuFromRow(bottomRowIndex, img) <=
			    MU_THRESHOLD) {
				break;
			}
		}

		// topRowIndex was 0 and was the only row with pixels. for loop
		// made us go negative.
		if (bottomRowIndex <= 0)
			bottomRowIndex = 0;
	}

	// start from left of image and find left index that is already part of
	// the fingerprint image
	int leftIndex { 0 }, rightIndex { img.cols - 1 };
	for (; leftIndex < img.cols; ++leftIndex) {
		if (computeMuFromColumn(leftIndex, img) <= MU_THRESHOLD) {
			break;
		}
	}

	// If we traversed all the columns, then we don't need to check starting
	// from the other side.
	if (leftIndex >= img.cols) {
		// If we traversed all columns and never found data, we can stop
		throw NFIQ2::Exception { NFIQ2::ErrorCode::InvalidImageSize,
			"All image columns appear to be blank" };
	} else {
		// start from right of image and find right index that is
		// already part of the fingerprint image
		for (; rightIndex >= leftIndex; --rightIndex) {
			if (computeMuFromColumn(rightIndex, img) <=
			    MU_THRESHOLD) {
				break;
			}
		}
		// leftRow was 0 and was the only column with pixels. for loop
		// made us go negative.
		if (rightIndex <= 0)
			rightIndex = 0;
	}
	if ((rightIndex <= leftIndex) || (bottomRowIndex <= topRowIndex))
		throw NFIQ2::Exception { NFIQ2::ErrorCode::InvalidImageSize,
			"Asked to inclusively crop from (" +
			    std::to_string(leftIndex) + ',' +
			    std::to_string(topRowIndex) + ") to (" +
			    std::to_string(rightIndex) + ',' +
			    std::to_string(bottomRowIndex) + ')' };

	// OpenCV range upper boundaries are not included, so add 1 to index
	const cv::Mat roiImg = img(cv::Range(topRowIndex, bottomRowIndex + 1),
	    cv::Range(leftIndex, rightIndex + 1));

	static const uint16_t fingerJetMinWidth = 196;
	static const uint16_t fingerJetMaxWidth = 800;
	static const uint16_t fingerJetMinHeight = 196;
	static const uint16_t fingerJetMaxHeight = 1000;

	// Values are from FJFX image size thresholds
	if (roiImg.cols < fingerJetMinWidth) {
		throw NFIQ2::Exception(NFIQ2::ErrorCode::InvalidImageSize,
		    "Width is too small after trimming whitespace. WxH: " +
			std::to_string(roiImg.cols) + "x" +
			std::to_string(roiImg.rows) +
			", but minimum width is " +
			std::to_string(fingerJetMinWidth));
	} else if (roiImg.cols > fingerJetMaxWidth) {
		throw NFIQ2::Exception(NFIQ2::ErrorCode::InvalidImageSize,
		    "Width is too large after trimming whitespace. WxH: " +
			std::to_string(roiImg.cols) + "x" +
			std::to_string(roiImg.rows) +
			", but maximum width is " +
			std::to_string(fingerJetMaxWidth));
	} else if (roiImg.rows < fingerJetMinHeight) {
		throw NFIQ2::Exception(NFIQ2::ErrorCode::InvalidImageSize,
		    "Height is too small after trimming whitespace. WxH: " +
			std::to_string(roiImg.cols) + "x" +
			std::to_string(roiImg.rows) +
			", but minimum height is " +
			std::to_string(fingerJetMinHeight));
	} else if (roiImg.rows > fingerJetMaxHeight) {
		throw NFIQ2::Exception(NFIQ2::ErrorCode::InvalidImageSize,
		    "Height is too large after trimming whitespace. WxH: " +
			std::to_string(roiImg.cols) + "x" +
			std::to_string(roiImg.rows) +
			", but maximum height is " +
			std::to_string(fingerJetMaxHeight));
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
	// As of last test with OpenCV 4.5.3, this is significantly faster than
	// std::accumulate of img.row(rowIndex)
	unsigned int mu { 0 };
	for (int j = 0; j < img.cols; ++j) {
		// get gray value of image (0 = black, 255 = white)
		mu += *img.ptr<uchar>(rowIndex, j);
	}

	return static_cast<double>(
	    static_cast<double>(mu) / static_cast<double>(img.cols));
}

double
computeMuFromColumn(unsigned int columnIndex, const cv::Mat &img)
{
	// As of last test with OpenCV 4.5.3, this is significantly faster than
	// std::accumulate of img.col(columnIndex)
	unsigned int mu { 0 };
	for (int i = 0; i < img.rows; ++i) {
		// get gray value of image (0 = black, 255 = white)
		mu += *img.ptr<uchar>(i, columnIndex);
	}

	return static_cast<double>(
	    static_cast<double>(mu) / static_cast<double>(img.rows));
}
