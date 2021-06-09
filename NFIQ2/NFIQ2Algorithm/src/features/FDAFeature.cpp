#include <features/FDAFeature.h>
#include <features/FeatureFunctions.h>
#include <nfiq2_exception.hpp>
#include <nfiq2_timer.hpp>
#include <opencv2/core.hpp>

#include <cmath>
#include <sstream>

const char NFIQ2::Identifiers::QualityModules::FrequencyDomainAnalysis[] {
	"FrequencyDomainAnalysis"
};
static const char NFIQ2FDAFeaturePrefix[] { "FDA_Bin10_" };
const char NFIQ2::Identifiers::QualityFeatures::FrequencyDomainAnalysis::
    Histogram::Bin0[] { "FDA_Bin10_0" };
const char NFIQ2::Identifiers::QualityFeatures::FrequencyDomainAnalysis::
    Histogram::Bin1[] { "FDA_Bin10_1" };
const char NFIQ2::Identifiers::QualityFeatures::FrequencyDomainAnalysis::
    Histogram::Bin2[] { "FDA_Bin10_2" };
const char NFIQ2::Identifiers::QualityFeatures::FrequencyDomainAnalysis::
    Histogram::Bin3[] { "FDA_Bin10_3" };
const char NFIQ2::Identifiers::QualityFeatures::FrequencyDomainAnalysis::
    Histogram::Bin4[] { "FDA_Bin10_4" };
const char NFIQ2::Identifiers::QualityFeatures::FrequencyDomainAnalysis::
    Histogram::Bin5[] { "FDA_Bin10_5" };
const char NFIQ2::Identifiers::QualityFeatures::FrequencyDomainAnalysis::
    Histogram::Bin6[] { "FDA_Bin10_6" };
const char NFIQ2::Identifiers::QualityFeatures::FrequencyDomainAnalysis::
    Histogram::Bin7[] { "FDA_Bin10_7" };
const char NFIQ2::Identifiers::QualityFeatures::FrequencyDomainAnalysis::
    Histogram::Bin8[] { "FDA_Bin10_8" };
const char NFIQ2::Identifiers::QualityFeatures::FrequencyDomainAnalysis::
    Histogram::Bin9[] { "FDA_Bin10_9" };
const char
    NFIQ2::Identifiers::QualityFeatures::FrequencyDomainAnalysis::Mean[] {
	    "FDA_Bin10_Mean"
    };
const char
    NFIQ2::Identifiers::QualityFeatures::FrequencyDomainAnalysis::StdDev[] {
	    "FDA_Bin10_StdDev"
    };

double fda(const cv::Mat &block, const double orientation, const int v1sz_x,
    const int v1sz_y, const bool padFlag);

NFIQ2::QualityFeatures::FDAFeature::FDAFeature(
    const NFIQ2::FingerprintImageData &fingerprintImage)
{
	this->setFeatures(computeFeatureData(fingerprintImage));
}

NFIQ2::QualityFeatures::FDAFeature::~FDAFeature() = default;

std::vector<std::string>
NFIQ2::QualityFeatures::FDAFeature::getQualityFeatureIDs()
{
	return { Identifiers::QualityFeatures::FrequencyDomainAnalysis::
		     Histogram::Bin0,
		Identifiers::QualityFeatures::FrequencyDomainAnalysis::
		    Histogram::Bin1,
		Identifiers::QualityFeatures::FrequencyDomainAnalysis::
		    Histogram::Bin2,
		Identifiers::QualityFeatures::FrequencyDomainAnalysis::
		    Histogram::Bin3,
		Identifiers::QualityFeatures::FrequencyDomainAnalysis::
		    Histogram::Bin4,
		Identifiers::QualityFeatures::FrequencyDomainAnalysis::
		    Histogram::Bin5,
		Identifiers::QualityFeatures::FrequencyDomainAnalysis::
		    Histogram::Bin6,
		Identifiers::QualityFeatures::FrequencyDomainAnalysis::
		    Histogram::Bin7,
		Identifiers::QualityFeatures::FrequencyDomainAnalysis::
		    Histogram::Bin8,
		Identifiers::QualityFeatures::FrequencyDomainAnalysis::
		    Histogram::Bin9,
		Identifiers::QualityFeatures::FrequencyDomainAnalysis::Mean,
		Identifiers::QualityFeatures::FrequencyDomainAnalysis::StdDev };
}

std::string
NFIQ2::QualityFeatures::FDAFeature::getModuleName() const
{
	return NFIQ2::Identifiers::QualityModules::FrequencyDomainAnalysis;
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::FDAFeature::computeFeatureData(
    const NFIQ2::FingerprintImageData &fingerprintImage)
{
	std::unordered_map<std::string, double> featureDataList;

	// check if input image has 500 dpi
	if (fingerprintImage.ppi !=
	    NFIQ2::FingerprintImageData::Resolution500PPI) {
		throw NFIQ2::Exception(
		    NFIQ2::ErrorCode::FeatureCalculationError,
		    "Only 500 dpi fingerprint images are supported!");
	}

	// get matrix from fingerprint image
	cv::Mat img = cv::Mat(fingerprintImage.height, fingerprintImage.width,
	    CV_8UC1, (void *)fingerprintImage.data());

	// ----------------------------
	// compute Fda (taken from Rvu)
	// ----------------------------

	NFIQ2::Timer timer;
	try {
		timer.start();

		cv::Mat maskim;
		const int blksize = this->blocksize;
		const int v1sz_x = this->slantedBlockSizeX;
		const int v1sz_y = this->slantedBlockSizeY;

		assert((blksize > 0) && (this->threshold > 0));

		ridgesegment(img, blksize, this->threshold, cv::noArray(),
		    maskim, cv::noArray());

		int rows = img.rows;
		int cols = img.cols;
		double blk = static_cast<double>(blksize);

		double sumSQ = static_cast<double>(
		    (v1sz_x * v1sz_x) + (v1sz_y * v1sz_y));
		double eblksz = ceil(
		    sqrt(sumSQ)); // block size for extraction of slanted block
		double diff = (eblksz - blk);
		int blkoffset = static_cast<int>(
		    ceil(diff / 2)); // overlapping border

		int mapRows = static_cast<int>(
		    (static_cast<double>(rows) - diff) / blk);
		int mapCols = static_cast<int>(
		    (static_cast<double>(cols) - diff) / blk);

		cv::Mat fdas = cv::Mat::zeros(mapRows, mapCols, CV_64F);
		cv::Mat blkorient = cv::Mat::zeros(mapRows, mapCols, CV_64F);
		cv::Mat im_roi, blkwim;
		cv::Mat maskB1;
		double cova, covb, covc;

		std::vector<double> dataVector;
		dataVector.reserve(mapRows * mapCols);

		// Image processed NOT from beg to end but with a border around
		// - can't be vectorized:(
		int br = 0;
		int bc = 0;
		for (int r = blkoffset; r < rows - (blksize + blkoffset - 1);
		     r += blksize) {
			for (int c = blkoffset;
			     c < cols - (blksize + blkoffset - 1);
			     c += blksize) {
				im_roi = img(
				    cv::Range(
					r, cv::min(r + blksize, img.rows)),
				    cv::Range(
					c, cv::min(c + blksize, img.cols)));
				maskB1 = maskim(
				    cv::Range(
					r, cv::min(r + blksize, maskim.rows)),
				    cv::Range(
					c, cv::min(c + blksize, maskim.cols)));
				uint8_t mask = allfun(maskB1);
				if (mask == 1) {
					covcoef(im_roi, cova, covb, covc,
					    CENTERED_DIFFERENCES);

					// ridge ORIENT local
					blkorient.at<double>(br,
					    bc) = ridgeorient(cova, covb, covc);
					// overlapping windows (border =
					// blkoffset)
					blkwim = img(
					    cv::Range(r - blkoffset,
						cv::min(r + blksize + blkoffset,
						    img.rows)),
					    cv::Range(c - blkoffset,
						cv::min(c + blksize + blkoffset,
						    img.cols)));
					fdas.at<double>(br, bc) = fda(blkwim,
					    blkorient.at<double>(br, bc),
					    v1sz_x, v1sz_y, this->padFlag);
					dataVector.push_back(
					    fdas.at<double>(br, bc));
				}
				bc = bc + 1;
			}
			br = br + 1;
			bc = 0;
		}

		std::vector<double> histogramBins10;
		histogramBins10.push_back(FDAHISTLIMITS[0]);
		histogramBins10.push_back(FDAHISTLIMITS[1]);
		histogramBins10.push_back(FDAHISTLIMITS[2]);
		histogramBins10.push_back(FDAHISTLIMITS[3]);
		histogramBins10.push_back(FDAHISTLIMITS[4]);
		histogramBins10.push_back(FDAHISTLIMITS[5]);
		histogramBins10.push_back(FDAHISTLIMITS[6]);
		histogramBins10.push_back(FDAHISTLIMITS[7]);
		histogramBins10.push_back(FDAHISTLIMITS[8]);
		addHistogramFeatures(featureDataList, NFIQ2FDAFeaturePrefix,
		    histogramBins10, dataVector, 10);

		this->setSpeed(timer.stop());
	} catch (const cv::Exception &e) {
		std::stringstream ssErr;
		ssErr << "Cannot compute Frequency Domain Analysis (FDA): "
		      << e.what();
		throw NFIQ2::Exception(
		    NFIQ2::ErrorCode::FeatureCalculationError, ssErr.str());
	} catch (const NFIQ2::Exception &) {
		throw;
	} catch (...) {
		throw NFIQ2::Exception(
		    NFIQ2::ErrorCode::FeatureCalculationError,
		    "Unknown exception occurred!");
	}

	return featureDataList;
}

/**
% FDA Computes image quality measure (IQM) for the Frequency Domain Analysis of
ridges and valleys. % Returns fdaIQM by performing ridge-valley periodical
signature analysis within % a block of fingerprint image given as a parameter.
%
% TODO:       - take scanner resolution into account (ppi <> 500 )
%
% Syntax:     - fdaIQM = fda(block, orientation, scanResolution)
%
% Inputs:
%   block           - square block of image (orientation block + border to fully
cover rotated img) %   orientation     - angle of the orientation line
perpendicular to the ridge direction %                     within the block
[rad] %   scanResolution  - scanner resolution [ppi]
%
% Outputs:
%   fdaIQM          - local quality score (of the block)
%
% Examples:
%   fdaIQM = fda([36 36], ang_in_deg, 500);
%
% Code parts by Vladimir Smida.
%
% 2011 Biometric Systems, Kenneth Skovhus Andersen & Lasse Bach Nielsen
% The Technical University of Denmark, DTU
*/
double
fda(const cv::Mat &block, const double orientation, const int v1sz_x,
    const int v1sz_y, const bool padFlag)
{
	// sanity check: check block size
	float cBlock = static_cast<float>(block.rows) / 2; // square block
	int icBlock = static_cast<int>(cBlock);
	if (icBlock != cBlock) {
		throw NFIQ2::Exception {
			NFIQ2::ErrorCode::FeatureCalculationError,
			"Wrong block size! Consider block with size of even number "
			"(block rows = " +
			    std::to_string(block.rows) + ')'
		};
	}

	// rotate image to get the ridges horizontal using nearest-neighbor
	// interpolation
	cv::Mat blockRotated;
	NFIQ2::QualityFeatures::getRotatedBlock(
	    block, orientation + (M_PI / 2), padFlag, blockRotated);

	//% set x and y
	int xoff = v1sz_x / 2;
	int yoff = v1sz_y / 2;

	// extract slanted block by cropping the rotated image: To ensure that
	// rotated image does not contain any invalid regions.
	//     Matlab:  blockCropped =
	//     blockRotated(cBlock-(xoff-1):cBlock+xoff,cBlock-(yoff-1):cBlock+yoff);
	//     % v2
	// Note: Matlab uses matrix indices starting at 1, OpenCV starts at
	// 0. Also, OpenCV ranges are open-ended on the upper end.

	cv::Mat blockCropped = blockRotated(
	    cv::Range((icBlock - (xoff - 1) - 1), (icBlock + xoff)),
	    cv::Range((icBlock - (yoff - 1) - 1), (icBlock + yoff))); // v2

	cv::Mat t = cv::Mat::zeros(blockCropped.rows, 1, CV_64F);
	for (int r = 0; r < blockCropped.rows; r++) {
		// get ROI for current row
		cv::Mat roi = cv::Mat(blockCropped,
		    cv::Rect(0, r, blockCropped.cols,
			1)); // x,y, width, height
		cv::Scalar s = mean(roi);
		t.at<double>(r, 0) = s.val[0];
		roi.release(); // is this required or does it auto clean when
			       // done?
	}

	// compute dft on transposed t (so using transposed dimensions)
	cv::Mat tmpM;
	int m = cv::getOptimalDFTSize(t.cols); // t' rows (t cols)
	int n = cv::getOptimalDFTSize(t.rows); // t' cols (t rows)
	// create output
	cv::copyMakeBorder(t.t(), tmpM, 0, m - t.cols, 0, n - t.rows,
	    cv::BORDER_CONSTANT, cv::Scalar::all(0));
	// copy the source, on the border adding zero values
	cv::Mat planes[] = { tmpM, cv::Mat::zeros(tmpM.size(), CV_64F) };
	cv::Mat complex;
	cv::merge(planes, 2, complex);
	cv::dft(complex, complex,
	    cv::DFT_COMPLEX_OUTPUT | cv::DFT_ROWS); // fourier transform

	// Get Amplitude (Magnitude), cutting out DC (index 0,0)
	// dftAmp = abs(cv::dft(1, 2:end));
	cv::split(complex, planes);
	cv::magnitude(planes[0], planes[1],
	    planes[0]); // sqrt(Re(DFT(I))^2 + Im(DFT(I))^2)
	cv::Mat absMag = abs(planes[0]);
	cv::Mat amp(absMag,
	    cv::Rect(1, 0, absMag.cols - 1, 1)); // set ROI, cutting out DC
	double mVal;
	cv::Point mLoc;
	cv::minMaxLoc(amp, 0, &mVal, 0, &mLoc);
	cv::Mat ampDenom(
	    amp, cv::Rect(0, 0, (int)floor((double)(amp.cols / 2)), 1));
	cv::Scalar iqmDenom = sum(ampDenom);
	if (mLoc.x == 0 || mLoc.x + 1 >= amp.cols) {
		// ?????? FIXME
		return 1.0;
	}
	return (mVal +
		   0.3 *
		       (amp.at<double>(0, mLoc.x - 1) +
			   amp.at<double>(0, mLoc.x + 1))) /
	    iqmDenom.val[0];
}
