#include <features/FDAFeature.h>
#include <features/FeatureFunctions.h>
#include <nfiq2_nfiqexception.hpp>
#include <nfiq2_timer.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <sstream>

#if defined WINDOWS || defined WIN32
#include <float.h>
#include <windows.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace NFIQ;

double fda(const cv::Mat &block, const double orientation, const int v1sz_x,
    const int v1sz_y, const bool padFlag);

NFIQ::QualityFeatures::FDAFeature::FDAFeature(
    const NFIQ::FingerprintImageData &fingerprintImage)
{
	this->setFeatures(computeFeatureData(fingerprintImage));
}

NFIQ::QualityFeatures::FDAFeature::~FDAFeature() = default;

std::vector<std::string>
NFIQ::QualityFeatures::FDAFeature::getAllFeatureIDs()
{
	std::vector<std::string> featureIDs;
	addHistogramFeatureNames(featureIDs, "FDA_Bin10_", 10);

	return featureIDs;
}

const std::string NFIQ::QualityFeatures::FDAFeature::speedFeatureIDGroup =
    "Frequency domain";

const std::string NFIQ::QualityFeatures::FDAFeature::moduleName { "NFIQ2_FDA" };
std::string
NFIQ::QualityFeatures::FDAFeature::getModuleName() const
{
	return moduleName;
}

std::vector<NFIQ::QualityFeatureResult>
NFIQ::QualityFeatures::FDAFeature::computeFeatureData(
    const NFIQ::FingerprintImageData &fingerprintImage)
{
	std::vector<NFIQ::QualityFeatureResult> featureDataList;

	// check if input image has 500 dpi
	if (fingerprintImage.m_ImageDPI != NFIQ::e_ImageResolution_500dpi) {
		throw NFIQ::NFIQException(NFIQ::e_Error_FeatureCalculationError,
		    "Only 500 dpi fingerprint images are supported!");
	}

	// get matrix from fingerprint image
	cv::Mat img = cv::Mat(fingerprintImage.m_ImageHeight,
	    fingerprintImage.m_ImageWidth, CV_8UC1,
	    (void *)fingerprintImage.data());

	// ----------------------------
	// compute Fda (taken from Rvu)
	// ----------------------------

	NFIQ::Timer timer;
	double time = 0.0;
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
		addHistogramFeatures(featureDataList, "FDA_Bin10_",
		    histogramBins10, dataVector, 10);

		time = timer.stop();

		// Speed
		NFIQ::QualityFeatureSpeed speed;
		speed.featureIDGroup = FDAFeature::speedFeatureIDGroup;

		addHistogramFeatureNames(speed.featureIDs, "FDA_Bin10_", 10);

		speed.featureSpeed = time;
		this->setSpeed(speed);

	} catch (cv::Exception &e) {
		std::stringstream ssErr;
		ssErr << "Cannot compute Frequency Domain Analysis (FDA): "
		      << e.what();
		throw NFIQ::NFIQException(
		    NFIQ::e_Error_FeatureCalculationError, ssErr.str());
	} catch (NFIQ::NFIQException &e) {
		throw e;
	} catch (...) {
		throw NFIQ::NFIQException(NFIQ::e_Error_FeatureCalculationError,
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
		std::cerr << "block rows = " << block.rows << std::endl;
		std::cerr << "warning: Wrong block size! Consider block with "
			     "size of even number"
			  << std::endl;
	}

	// rotate image to get the ridges horizontal using nearest-neighbor
	// interpolation
	cv::Mat blockRotated;
	NFIQ::QualityFeatures::getRotatedBlock(
	    block, orientation + (M_PI / 2), padFlag, blockRotated);

	//% set x and y
	int xoff = v1sz_x / 2;
	int yoff = v1sz_y / 2;

	// extract slanted block by cropping the rotated image: To ensure that
	// rotated image does not contain any invalid regions.
	//     cv::Matlab:  blockCropped =
	//     blockRotated(cBlock-(xoff-1):cBlock+xoff,cBlock-(yoff-1):cBlock+yoff);
	//     % v2
	// Note: cv::Matlab uses matrix indices starting at 1, OpenCV starts at
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
	copyMakeBorder(t.t(), tmpM, 0, m - t.cols, 0, n - t.rows,
	    cv::BORDER_CONSTANT, cv::Scalar::all(0));
	// copy the source, on the border adding zero values
	cv::Mat planes[] = { tmpM, cv::Mat::zeros(tmpM.size(), CV_64F) };
	cv::Mat complex;
	merge(planes, 2, complex);
	dft(complex, complex,
	    cv::DFT_COMPLEX_OUTPUT | cv::DFT_ROWS); // fourier transform

	// Get Amplitude (Magnitude), cutting out DC (index 0,0)
	// dftAmp = abs(dft(1, 2:end));
	split(complex, planes);
	magnitude(planes[0], planes[1],
	    planes[0]); // sqrt(Re(DFT(I))^2 + Im(DFT(I))^2)
	cv::Mat absMag = abs(planes[0]);
	cv::Mat amp(absMag,
	    cv::Rect(1, 0, absMag.cols - 1, 1)); // set ROI, cutting out DC
	double mVal;
	cv::Point mLoc;
	minMaxLoc(amp, 0, &mVal, 0, &mLoc);
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
