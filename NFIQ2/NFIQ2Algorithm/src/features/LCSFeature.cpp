#include <features/FeatureFunctions.h>
#include <features/LCSFeature.h>
#include <nfiq2_exception.hpp>
#include <nfiq2_timer.hpp>
#include <opencv2/core.hpp>

#include <sstream>

const char NFIQ2::Identifiers::QualityModules::LocalClarity[] {
	"LocalClarity"
};
static const char NFIQ2LCSFeaturePrefix[] { "LCS_Bin10_" };
const char
    NFIQ2::Identifiers::QualityFeatures::LocalClarity::Histogram::Bin0[] {
	    "LCS_Bin10_0"
    };
const char
    NFIQ2::Identifiers::QualityFeatures::LocalClarity::Histogram::Bin1[] {
	    "LCS_Bin10_1"
    };
const char
    NFIQ2::Identifiers::QualityFeatures::LocalClarity::Histogram::Bin2[] {
	    "LCS_Bin10_2"
    };
const char
    NFIQ2::Identifiers::QualityFeatures::LocalClarity::Histogram::Bin3[] {
	    "LCS_Bin10_3"
    };
const char
    NFIQ2::Identifiers::QualityFeatures::LocalClarity::Histogram::Bin4[] {
	    "LCS_Bin10_4"
    };
const char
    NFIQ2::Identifiers::QualityFeatures::LocalClarity::Histogram::Bin5[] {
	    "LCS_Bin10_5"
    };
const char
    NFIQ2::Identifiers::QualityFeatures::LocalClarity::Histogram::Bin6[] {
	    "LCS_Bin10_6"
    };
const char
    NFIQ2::Identifiers::QualityFeatures::LocalClarity::Histogram::Bin7[] {
	    "LCS_Bin10_7"
    };
const char
    NFIQ2::Identifiers::QualityFeatures::LocalClarity::Histogram::Bin8[] {
	    "LCS_Bin10_8"
    };
const char
    NFIQ2::Identifiers::QualityFeatures::LocalClarity::Histogram::Bin9[] {
	    "LCS_Bin10_9"
    };
const char NFIQ2::Identifiers::QualityFeatures::LocalClarity::Mean[] {
	"LCS_Bin10_Mean"
};
const char NFIQ2::Identifiers::QualityFeatures::LocalClarity::StdDev[] {
	"LCS_Bin10_StdDev"
};

double loclar(cv::Mat &block, const double orientation, const int v1sz_x,
    const int v1sz_y, const int scres, const bool padFlag);

NFIQ2::QualityFeatures::LCSFeature::LCSFeature(
    const NFIQ2::FingerprintImageData &fingerprintImage)
{
	this->setFeatures(computeFeatureData(fingerprintImage));
}

NFIQ2::QualityFeatures::LCSFeature::~LCSFeature() = default;

std::string
NFIQ2::QualityFeatures::LCSFeature::getModuleName() const
{
	return NFIQ2::Identifiers::QualityModules::LocalClarity;
}

std::vector<std::string>
NFIQ2::QualityFeatures::LCSFeature::getQualityFeatureIDs()
{
	return { Identifiers::QualityFeatures::LocalClarity::Histogram::Bin0,
		Identifiers::QualityFeatures::LocalClarity::Histogram::Bin1,
		Identifiers::QualityFeatures::LocalClarity::Histogram::Bin2,
		Identifiers::QualityFeatures::LocalClarity::Histogram::Bin3,
		Identifiers::QualityFeatures::LocalClarity::Histogram::Bin4,
		Identifiers::QualityFeatures::LocalClarity::Histogram::Bin5,
		Identifiers::QualityFeatures::LocalClarity::Histogram::Bin6,
		Identifiers::QualityFeatures::LocalClarity::Histogram::Bin7,
		Identifiers::QualityFeatures::LocalClarity::Histogram::Bin8,
		Identifiers::QualityFeatures::LocalClarity::Histogram::Bin9,
		Identifiers::QualityFeatures::LocalClarity::Mean,
		Identifiers::QualityFeatures::LocalClarity::StdDev };
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::LCSFeature::computeFeatureData(
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

	cv::Mat img;
	try {
		// get matrix from fingerprint image
		img = cv::Mat(fingerprintImage.height, fingerprintImage.width,
		    CV_8UC1, (void *)fingerprintImage.data());
	} catch (const cv::Exception &e) {
		std::stringstream ssErr;
		ssErr << "Cannot get matrix from fingerprint image: "
		      << e.what();
		throw NFIQ2::Exception(
		    NFIQ2::ErrorCode::FeatureCalculationError, ssErr.str());
	}

	NFIQ2::Timer timerLCS;
	try {
		timerLCS.start();

		int rows = img.rows;
		int cols = img.cols;

		const int v1sz_x = blocksize;
		const int v1sz_y = blocksize / 2;

		cv::Mat maskim;
		ridgesegment(img, blocksize, threshold, cv::noArray(), maskim,
		    cv::noArray());

		// ----------
		// compute LCS
		// ----------

		double blk = static_cast<double>(blocksize);

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

		cv::Mat maskBseg = cv::Mat::zeros(mapRows, mapCols, CV_8UC1);
		cv::Mat blkorient = cv::Mat::zeros(mapRows, mapCols, CV_64F);

		std::vector<double> dataVector;
		dataVector.reserve(mapRows * mapCols);

		cv::Mat im_roi, blkwim;
		cv::Mat maskB1;
		cv::Mat lcs = cv::Mat::zeros(mapRows, mapCols, CV_64F);
		double cova, covb, covc;
		// Image processed NOT from beg to end but with a border around
		// - can't be vectorized:(
		int br = 0;
		int bc = 0;

		for (int r = blkoffset; r < rows - (blocksize + blkoffset - 1);
		     r += blocksize) {
			for (int c = blkoffset;
			     c < cols - (blocksize + blkoffset - 1);
			     c += blocksize) {
				im_roi = img(
				    cv::Range(
					r, cv::min(r + blocksize, img.rows)),
				    cv::Range(
					c, cv::min(c + blocksize, img.cols)));
				//      blkim = im(r:r+blksz-1, c:c+blksz-1);
				maskB1 = maskim(
				    cv::Range(
					r, cv::min(r + blocksize, maskim.rows)),
				    cv::Range(c,
					cv::min(c + blocksize, maskim.cols)));
				//      maskB1 = maskim(r:r+blksz-1,
				//      c:c+blksz-1);
				maskBseg.at<uint8_t>(br, bc) = allfun(maskB1);
				covcoef(im_roi, cova, covb, covc,
				    CENTERED_DIFFERENCES);

				// ridge ORIENT local
				blkorient.at<double>(br, bc) = ridgeorient(
				    cova, covb, covc);
				// overlapping windows (border = blkoffset)
				blkwim = img(
				    cv::Range(r - blkoffset,
					cv::min(r + blocksize + blkoffset,
					    img.rows)),
				    cv::Range(c - blkoffset,
					cv::min(c + blocksize + blkoffset,
					    img.cols)));
				lcs.at<double>(br, bc) = loclar(blkwim,
				    blkorient.at<double>(br, bc), v1sz_x,
				    v1sz_y, scannerRes, padFlag);
				if (maskBseg.at<uint8_t>(br, bc) == 1) {
					dataVector.push_back(
					    lcs.at<double>(br, bc));
				}
				bc = bc + 1;
			}
			br = br + 1;
			bc = 0;
		}

		std::vector<double> histogramBins10;
		histogramBins10.push_back(LCSHISTLIMITS[0]);
		histogramBins10.push_back(LCSHISTLIMITS[1]);
		histogramBins10.push_back(LCSHISTLIMITS[2]);
		histogramBins10.push_back(LCSHISTLIMITS[3]);
		histogramBins10.push_back(LCSHISTLIMITS[4]);
		histogramBins10.push_back(LCSHISTLIMITS[5]);
		histogramBins10.push_back(LCSHISTLIMITS[6]);
		histogramBins10.push_back(LCSHISTLIMITS[7]);
		histogramBins10.push_back(LCSHISTLIMITS[8]);
		addHistogramFeatures(featureDataList, NFIQ2LCSFeaturePrefix,
		    histogramBins10, dataVector, 10);

		this->setSpeed(timerLCS.stop());
	} catch (const cv::Exception &e) {
		std::stringstream ssErr;
		ssErr << "Cannot compute LCS: " << e.what();
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
///////////////////////////////////////////////////////////////////////
/***
% LOCLAR Computes local clarity score (LCS) of ridges and valleys.
% Returns lcsISO [ 1(worst) - 0(best) ] by performing ridge-valley structure
analysis within % a block of FP image given as a parameter.
%
% Syntax:
%   lcs = loclar(blk, orang, v1sz, sc)
%
% Inputs:
%   blk         - square block of image (orientation block + border to fully
cover v1) %   orang       - angle of the orientation line perpendicular to the
ridge direction %               - within the block [rad] %   v1sz        - size
of slanted square to extract from block [px] (recommended 32x16) %   sc -
scanner resolution [ppi]
%
% Outputs:
%   lcs       - local clarity score (of the block) [ 0(worst) - 1(best) ]
%
% Examples:
%   lcs = loclar([36 36], ang_in_deg, [32 16], 500);
%

% 2011 Master Thesis, Vladimir Smida, vladimir.smida@[cased.de|gmail.com]
% FIT VUT, Czech Republic & CASED, Germany
***/

double
loclar(cv::Mat &block, const double orientation, const int v1sz_x,
    const int v1sz_y, const int screenRes, const bool padFlag)
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

	cv::Mat blockRotated;
	NFIQ2::QualityFeatures::getRotatedBlock(
	    block, orientation, padFlag, blockRotated);

	//% set x and y
	int xoff = v1sz_x / 2;
	int yoff = v1sz_y / 2;

	// extract slanted block by cropping the rotated image: To ensure that
	// rotated image does not contain any invalid regions.
	//     Matlab:  blockCropped =
	//     blockRotated(cBlock-(yoff-1):cBlock+yoff,cBlock-(xoff-1):cBlock+xoff);
	//     % v2
	// Note: Matlab uses matrix indices starting at 1, OpenCV starts at 0.
	// Also, OpenCV ranges are open-ended on the upper end.

	int rowstart = icBlock - (yoff - 1) - 1;
	int rowend = icBlock + yoff;
	int colstart = icBlock - (xoff - 1) - 1;
	int colend = icBlock + xoff;
	cv::Mat v2 = blockRotated(
	    cv::Range(rowstart, rowend), cv::Range(colstart, colend));

	std::vector<uint8_t> ridval;
	std::vector<double> dt;
	NFIQ2::QualityFeatures::getRidgeValleyStructure(v2, ridval, dt);

	// Ridge-valley thickness
	//  begrid = ridval(1); % begining with ridge?
	//  change = xor(ridval,circshift(ridval,1)); // find the bin change
	//  change(1) = []; % there can't be change in 1. element (circshift)
	//  change = find(change == 1);    % find indices where changes
	uint8_t begrid = ridval[0]; //% begining with ridge?
	std::vector<uint8_t> change;
	size_t j;
	for (unsigned int i = 0; i < ridval.size(); i++) {
		// circular shift from back to front
		if (i == 0) {
			j = ridval.size() - 1;
		} else {
			j = i - 1;
		}
		if (ridval[i] != ridval[j]) {
			change.push_back(1);
		} else {
			change.push_back(0);
		}
	}

	std::vector<uint8_t> changeIndex;
	for (unsigned int i = 1; i < change.size();
	     i++) // skip the first element, same effect
	{
		// as "change(1) = []" in Matlab.
		if (change[i] == 1) {
			changeIndex.push_back(i - 1);
		}
	}

	//  if ~isempty(changeIndex) ==> changes found = ridge-val structure
	double lcsNOTISO = 0.0;
	if (!changeIndex.empty()) {
		//    change1r = circshift(change,1); change1r(1) = 0;
		//    Wrv = change - change1r; % ridge and valley thickness
		std::vector<uint8_t> Wrv;
		Wrv.push_back(changeIndex[0]);
		for (unsigned int i = 1; i < changeIndex.size(); i++) {
			Wrv.push_back(changeIndex[i] - changeIndex[i - 1]);
		}

		// Matlab:
		// if begrid
		//     Wr = Wrv(1:2:end); % odd indeces
		//     Wv = Wrv(2:2:end); % even indeces
		// else
		//     Wv = Wrv(1:2:end); % odd indeces
		//     Wr = Wrv(2:2:end); % even indeces
		// end
		// NWr = Wr / ((sc/125)*Wrmax125);
		// NWv = Wv / ((sc/125)*Wvmax125);
		double Wrmax125 = 5.0; // max ridge for 125 ppi scanner
		double Wvmax125 = 5.0; // max valley for 125 ppi scanner

		double Wrmin = 3.0; // [px]
		double Wrmax = 10.0;
		double Wvmin = 2.0;
		double Wvmax = 10.0;

		double sc = static_cast<double>(screenRes);
		double RscaleNorm = (sc / 125.0) * Wrmax125;
		double VscaleNorm = (sc / 125.0) * Wvmax125;

		// normalized max/min
		double NWrmin = Wrmin / RscaleNorm;
		double NWrmax = Wrmax / RscaleNorm;
		double NWvmin = Wvmin /
		    RscaleNorm; // Should this be Wvmin/VscaleNorm???
		double NWvmax = Wvmax / RscaleNorm;

		std::vector<double> NWr, NWv;
		double rtemp, vtemp;
		if (begrid) {
			for (unsigned int i = 0; i < Wrv.size(); i += 2) {
				rtemp = static_cast<double>(Wrv[i]) /
				    RscaleNorm;
				NWr.push_back(rtemp); // Matlab "odd" indices
			}
			for (unsigned int i = 0; i < Wrv.size() - 1; i += 2) {
				vtemp = static_cast<double>(Wrv[i + 1]) /
				    VscaleNorm;
				NWv.push_back(vtemp); // Matlab "even" indices
			}
		} else {
			for (unsigned int i = 0; i < Wrv.size(); i += 2) {
				vtemp = static_cast<double>(Wrv[i]) /
				    VscaleNorm;
				NWv.push_back(vtemp); // Matlab "odd" indices
			}
			for (unsigned int i = 0; i < Wrv.size() - 1; i += 2) {
				rtemp = static_cast<double>(Wrv[i + 1]) /
				    RscaleNorm;
				NWr.push_back(rtemp); // Matlab "even" indices
			}
		}

		// Clarity test
		// NOTE: can be different strategy how to deal with out of limit
		// ridge-valley thickness: NOTE: first and last region can be
		// INCOMPLETE -should be somehow excluded from the test 1: all
		// should fall in (except first/last) 2: majority 3: mean/median
		// of all if all(NWr >= NWrmin) && all(NWr <= NWrmax) && all(NWv
		// >= NWvmin) && all(NWv <= NWvmax)

		cv::Scalar muNWr {}, muNWv {};
		if (!NWr.empty()) {
			muNWr = mean(NWr, cv::noArray());
		}
		if (!NWv.empty()) {
			muNWv = mean(NWv, cv::noArray());
		}

		if ((muNWr.val[0] >= NWrmin) && (muNWr.val[0] <= NWrmax) &&
		    (muNWv.val[0] >= NWvmin) && (muNWv.val[0] <= NWvmax)) {
			// ridge region
			//      ridmat = v2(:,ridval==1); % matrix of ridge pxs
			//      (according to v3 and dt1)
			// remove columns in v2 that do not correspond to ridges

			// Compute the number of pixels in ridge regions that
			// exceed their threshold Likewise, compute the number
			// of pixels in valley regions that are below their
			// threshold
			cv::Mat ridgecol, valleycol;
			int ridgeGood = 0, valleyGood = 0;
			int ridgePixelCount = 0, valleyPixelCount = 0;
			for (int i = 0; i < v2.cols; i++) {
				if (ridval[i] == 1) // ridges
				{
					ridgecol = (v2.col(i) >= dt[i]);
					ridgeGood += countNonZero(
					    ridgecol); // no. pixels in
						       // column that are
						       // above the
						       // threshold
					ridgePixelCount +=
					    v2.rows; // number of pixels in
						     // the column
				} else		     // valleys
				{
					valleycol = (v2.col(i) < dt[i]);
					valleyGood += countNonZero(valleycol);
					valleyPixelCount += v2.rows;
				}
			}
			// alpha = ratio of pixels below the threshold in valley
			// regions beta = ratio of pixels above the threshold in
			// ridge regions
			double alpha = static_cast<double>(valleyGood) /
			    static_cast<double>(valleyPixelCount);
			double beta = static_cast<double>(ridgeGood) /
			    static_cast<double>(ridgePixelCount);
			// lcsISO = mean([alpha beta]); % [ 1(worst) - 0(best) ]
			// lcsNOTISO = 1 - lcsISO; % [ 0(worst) - 1(best) ]
			lcsNOTISO = 1.0 - (alpha + beta) / 2.0;

		} else {
			lcsNOTISO = 0.0;
		}
	}

	return (lcsNOTISO);
}
