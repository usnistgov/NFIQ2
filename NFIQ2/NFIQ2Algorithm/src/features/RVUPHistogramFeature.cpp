#include <features/FeatureFunctions.h>
#include <features/RVUPHistogramFeature.h>
#include <nfiq2_exception.hpp>
#include <nfiq2_timer.hpp>
#include <opencv2/core.hpp>

#include <cmath>
#include <sstream>

const char NFIQ2::Identifiers::QualityModules::RidgeValleyUniformity[] {
	"RidgeValleyUniformity"
};
const char NFIQ2RVUPFeaturePrefix[] { "RVUP_Bin10_" };
const char NFIQ2::Identifiers::QualityFeatures::RidgeValleyUniformity::
    Histogram::Bin0[] { "RVUP_Bin10_0" };
const char NFIQ2::Identifiers::QualityFeatures::RidgeValleyUniformity::
    Histogram::Bin1[] { "RVUP_Bin10_1" };
const char NFIQ2::Identifiers::QualityFeatures::RidgeValleyUniformity::
    Histogram::Bin2[] { "RVUP_Bin10_2" };
const char NFIQ2::Identifiers::QualityFeatures::RidgeValleyUniformity::
    Histogram::Bin3[] { "RVUP_Bin10_3" };
const char NFIQ2::Identifiers::QualityFeatures::RidgeValleyUniformity::
    Histogram::Bin4[] { "RVUP_Bin10_4" };
const char NFIQ2::Identifiers::QualityFeatures::RidgeValleyUniformity::
    Histogram::Bin5[] { "RVUP_Bin10_5" };
const char NFIQ2::Identifiers::QualityFeatures::RidgeValleyUniformity::
    Histogram::Bin6[] { "RVUP_Bin10_6" };
const char NFIQ2::Identifiers::QualityFeatures::RidgeValleyUniformity::
    Histogram::Bin7[] { "RVUP_Bin10_7" };
const char NFIQ2::Identifiers::QualityFeatures::RidgeValleyUniformity::
    Histogram::Bin8[] { "RVUP_Bin10_8" };
const char NFIQ2::Identifiers::QualityFeatures::RidgeValleyUniformity::
    Histogram::Bin9[] { "RVUP_Bin10_9" };
const char NFIQ2::Identifiers::QualityFeatures::RidgeValleyUniformity::Mean[] {
	"RVUP_Bin10_Mean"
};
const char
    NFIQ2::Identifiers::QualityFeatures::RidgeValleyUniformity::StdDev[] {
	    "RVUP_Bin10_StdDev"
    };

void rvuhist(cv::Mat block, const double orientation, const int v1sz_x,
    const int v1sz_y, bool padFlag, std::vector<double> &ratios,
    std::vector<uint8_t> &Nans);

NFIQ2::QualityFeatures::RVUPHistogramFeature::RVUPHistogramFeature(
    const NFIQ2::FingerprintImageData &fingerprintImage)
{
	this->setFeatures(computeFeatureData(fingerprintImage));
}

NFIQ2::QualityFeatures::RVUPHistogramFeature::~RVUPHistogramFeature() = default;

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::RVUPHistogramFeature::computeFeatureData(
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

	// ----------
	// compute RVU
	// ----------

	NFIQ2::Timer timerRVU;
	try {
		timerRVU.start();

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

		cv::Mat maskBseg = cv::Mat::zeros(mapRows, mapCols, CV_8UC1);
		cv::Mat blkorient = cv::Mat::zeros(mapRows, mapCols, CV_64F);

		cv::Mat im_roi, blkwim;
		cv::Mat maskB1;
		double cova, covb, covc;
		// Image processed NOT from beg to end but with a border around
		// - can't be vectorized:(
		int br = 0;
		int bc = 0;

		std::vector<double> rvures;
		std::vector<uint8_t> NanVec;
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
				maskBseg.at<uint8_t>(br, bc) = allfun(maskB1);
				covcoef(im_roi, cova, covb, covc,
				    CENTERED_DIFFERENCES);

				// ridge ORIENT local
				blkorient.at<double>(br, bc) = ridgeorient(
				    cova, covb, covc);
				// overlapping windows (border = blkoffset)
				blkwim = img(
				    cv::Range(r - blkoffset,
					cv::min(
					    r + blksize + blkoffset, img.rows)),
				    cv::Range(c - blkoffset,
					cv::min(c + blksize + blkoffset,
					    img.cols)));
				if (maskBseg.at<uint8_t>(br, bc) == 1) {
					rvuhist(blkwim,
					    blkorient.at<double>(br, bc),
					    v1sz_x, v1sz_y, this->padFlag,
					    rvures, NanVec);
				}
				bc = bc + 1;
			}
			br = br + 1;
			bc = 0;
		}

		// RIDGE-VALLEY UNIFORMITY
		std::vector<double> histogramBins10;
		histogramBins10.push_back(RVUPHISTLIMITS[0]);
		histogramBins10.push_back(RVUPHISTLIMITS[1]);
		histogramBins10.push_back(RVUPHISTLIMITS[2]);
		histogramBins10.push_back(RVUPHISTLIMITS[3]);
		histogramBins10.push_back(RVUPHISTLIMITS[4]);
		histogramBins10.push_back(RVUPHISTLIMITS[5]);
		histogramBins10.push_back(RVUPHISTLIMITS[6]);
		histogramBins10.push_back(RVUPHISTLIMITS[7]);
		histogramBins10.push_back(RVUPHISTLIMITS[8]);
		addHistogramFeatures(featureDataList, NFIQ2RVUPFeaturePrefix,
		    histogramBins10, rvures, 10);

		this->setSpeed(timerRVU.stop());
	} catch (const cv::Exception &e) {
		std::stringstream ssErr;
		ssErr << "Cannot compute RVU: " << e.what();
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

std::string
NFIQ2::QualityFeatures::RVUPHistogramFeature::getModuleName() const
{
	return NFIQ2::Identifiers::QualityModules::RidgeValleyUniformity;
}

std::vector<std::string>
NFIQ2::QualityFeatures::RVUPHistogramFeature::getQualityFeatureIDs()
{
	return { Identifiers::QualityFeatures::RidgeValleyUniformity::
		     Histogram::Bin0,
		Identifiers::QualityFeatures::RidgeValleyUniformity::Histogram::
		    Bin1,
		Identifiers::QualityFeatures::RidgeValleyUniformity::Histogram::
		    Bin2,
		Identifiers::QualityFeatures::RidgeValleyUniformity::Histogram::
		    Bin3,
		Identifiers::QualityFeatures::RidgeValleyUniformity::Histogram::
		    Bin4,
		Identifiers::QualityFeatures::RidgeValleyUniformity::Histogram::
		    Bin5,
		Identifiers::QualityFeatures::RidgeValleyUniformity::Histogram::
		    Bin6,
		Identifiers::QualityFeatures::RidgeValleyUniformity::Histogram::
		    Bin7,
		Identifiers::QualityFeatures::RidgeValleyUniformity::Histogram::
		    Bin8,
		Identifiers::QualityFeatures::RidgeValleyUniformity::Histogram::
		    Bin9,
		Identifiers::QualityFeatures::RidgeValleyUniformity::Mean,
		Identifiers::QualityFeatures::RidgeValleyUniformity::StdDev };
}
///////////////////////////////////////////////////////////////////////
/***
function [ratios, blockRotated, blockCropped, v3, x, dt1, dt, ridval, change,
ridvalComplete] = rvu(block, orientation, v1sz, scanResolution) % RVU computes
ridge/valley ratios for a given block.
%
% TODO:         - code could be refactored
%
% Syntax:       - ratios = rvu(block, orientation, v1sz, scanResolution)
%
% Inputs:
%   block           - square block of image (orientation block + border to fully
cover rotated img) %   orientation     - angle of the orientation line
perpendicular to the ridge direction %                     within the block
[rad] %   v1sz            - size of slanted square to extract from block [px]
(recommended 32x16) %   scanResolution  - scanner resolution [ppi]
%
% Outputs:
%   ratios          - local ratios (ridge/valley) of the ridge valley structure
%
% Examples:
%   ratios = rvu([36 36], ang_in_deg, [32 16], 500);
%
% Code parts by Vladimir Smida.
%
% 2011 Biometric Systems, Kenneth Skovhus Andersen & Lasse Bach Nielsen
% The Technical University of Denmark, DTU
***/

void
rvuhist(cv::Mat block, const double orientation, const int v1sz_x,
    const int v1sz_y, bool padFlag, std::vector<double> &rvures,
    std::vector<uint8_t> &NaNvec)
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

	cv::Mat blockCropped = blockRotated(
	    cv::Range((icBlock - (yoff - 1) - 1), (icBlock + yoff)),
	    cv::Range((icBlock - (xoff - 1) - 1), (icBlock + xoff))); // v2

	std::vector<uint8_t> ridval;
	std::vector<double> dt;
	NFIQ2::QualityFeatures::getRidgeValleyStructure(
	    blockCropped, ridval, dt);

	// Ridge-valley thickness
	//  change = xor(ridval,circshift(ridval,1)); // find the bin change
	// change(1) = []; % there can't be change in 1. element (circshift)
	// changeIndex = find(change == 1);    % find indices where changes
	std::vector<uint8_t> change;
	size_t j;
	for (unsigned int i = 0; i < ridval.size() - 1; i++) {
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
	if (!changeIndex.empty()) {
		//% non complete ridges/valleys are removed from ridval and
		// changeIndex
		//  ridvalComplete = ridval(changeIndex(1)+1:changeIndex(end));
		// That is, remove the first and last parts to remove incomplete
		// ridges/valleys occurring at the border of the original block.
		std::vector<uint8_t> ridvalComplete;
		for (int i = changeIndex[0] + 1;
		     i < changeIndex[changeIndex.size() - 1]; i++) {
			ridvalComplete.push_back(ridval[i]);
		}
		//  Likewise, remove corresponding changes from the change index
		//  vector. Matlab: changeIndexComplete = changeIndex -
		//  changeIndex(1);
		//          changeIndexComplete(1) = [];// % removing first
		//          value
		std::vector<uint8_t> changeIndexComplete;
		for (unsigned int i = 1; i < changeIndex.size();
		     i++) // skip the first value
		{
			changeIndexComplete.push_back(
			    changeIndex[i] - changeIndex[0]);
		}

		// if isempty(ridvalComplete)
		//% not ridge/valley structure, skip computation
		//     return
		// end;
		if (ridvalComplete.empty()) {
			return;
		} else {
			std::vector<double> ratios;
			uint8_t begrid =
			    ridvalComplete[0]; //% begining with ridge?

			//% do the magic
			//% changeIndex now represents the change values...
			// changeIndexComplete(end:-1:2) =
			// changeIndexComplete(end:-1:2)-changeIndexComplete(end-1:-1:1);

			auto changesize = changeIndexComplete.size();
			// If there is only one change index at this point, then
			// there aren't any ridge/valley structures to compare,
			// so leave the ratios vector empty. (This is the same
			// result that Matlab gives, even though it is handled
			// differently, i.e., Matlab handles this edge case
			// within the operations themselves, rather than testing
			// the length of changeIndexComplete directly.
			if (changesize > 1) {
				std::vector<uint8_t> changeComplete2;
				uint8_t tmpc;
				for (auto i = changesize - 1; i > 0; i--) {
					tmpc = changeIndexComplete[i] -
					    changeIndexComplete[i - 1];
					changeComplete2.push_back(tmpc);
				}
				// for m=1:length(changeIndexComplete)-1,
				//  ratios(m) =
				//  changeIndexComplete(m)/changeIndexComplete(m+1);
				// end;

				double r;
				for (unsigned int m = 0;
				     m < changeComplete2.size() - 1; m++) {
					r = static_cast<double>(
						changeComplete2[m]) /
					    static_cast<double>(
						changeComplete2[m + 1]);
					ratios.push_back(r);
					// Create a mask vector that is a 1 if r
					// is not a NaN, 0 if it is.
					if (std::isnan(r)) {
						NaNvec.push_back(0);
					} else {
						NaNvec.push_back(1);
					}
				}

				//    ratios(begrid+1:2:end) = 1 ./
				//    ratios(begrid+1:2:end);
				for (unsigned int i = begrid; i < ratios.size();
				     i += 2) {
					ratios[i] = 1 / ratios[i];
				}
			}

			if (!ratios.empty()) {
				for (unsigned int i = 0; i < ratios.size();
				     i++) {
					rvures.push_back(ratios[i]);
				}
			}
		}
	}

	return;
}
