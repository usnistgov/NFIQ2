#include "LCSFeature.h"

#include <iostream>
#include <opencv2/core/core.hpp>
#include <sstream>

#include "FeatureFunctions.h"
#include "include/NFIQException.h"
#include "include/Timer.hpp"

#if defined WINDOWS || defined WIN32
#include <float.h>
#include <windows.h>
#endif

using namespace NFIQ;
using namespace cv;

double
loclar(Mat& block, const double orientation, const int v1sz_x, const int v1sz_y,
       const int scres, const bool padFlag);

#define HISTOGRAM_FEATURES 1

LCSFeature::~LCSFeature() {}

std::string
LCSFeature::getModuleID() {
    return "NFIQ2_LCS";
}

std::list<std::string>
LCSFeature::getAllFeatureIDs() {
    std::list<std::string> featureIDs;
#if HISTOGRAM_FEATURES
    addHistogramFeatureNames(featureIDs, "LCS_Bin10_", 10);
#endif
    return featureIDs;
}

const std::string LCSFeature::speedFeatureIDGroup = "Local clarity";

std::list<NFIQ::QualityFeatureResult>
LCSFeature::computeFeatureData(
    const NFIQ::FingerprintImageData& fingerprintImage) {
    std::list<NFIQ::QualityFeatureResult> featureDataList;

    // check if input image has 500 dpi
    if (fingerprintImage.m_ImageDPI != NFIQ::e_ImageResolution_500dpi) {
        throw NFIQ::NFIQException(
            NFIQ::e_Error_FeatureCalculationError,
            "Only 500 dpi fingerprint images are supported!");
    }

    Mat img;
    try {
        // get matrix from fingerprint image
        img = Mat(fingerprintImage.m_ImageHeight, fingerprintImage.m_ImageWidth,
                  CV_8UC1, (void*)fingerprintImage.data());
    } catch (cv::Exception& e) {
        std::stringstream ssErr;
        ssErr << "Cannot get matrix from fingerprint image: " << e.what();
        throw NFIQ::NFIQException(NFIQ::e_Error_FeatureCalculationError,
                                  ssErr.str());
    }

    NFIQ::Timer timerLCS;
    double timeLCS = 0.0;
    try {
        timerLCS.startTimer();

        int rows = img.rows;
        int cols = img.cols;

        const int v1sz_x = blocksize;
        const int v1sz_y = blocksize / 2;

        Mat maskim;
        ridgesegment(img, blocksize, threshold, noArray(), maskim, noArray());

        // ----------
        // compute LCS
        // ----------

        double blk = static_cast<double>(blocksize);

        double sumSQ =
            static_cast<double>((v1sz_x * v1sz_x) + (v1sz_y * v1sz_y));
        double eblksz =
            ceil(sqrt(sumSQ));  // block size for extraction of slanted block
        double diff = (eblksz - blk);
        int blkoffset = static_cast<int>(ceil(diff / 2));  // overlapping border

        int mapRows =
            static_cast<int>((static_cast<double>(rows) - diff) / blk);
        int mapCols =
            static_cast<int>((static_cast<double>(cols) - diff) / blk);

        Mat maskBseg = Mat::zeros(mapRows, mapCols, CV_8UC1);
        Mat blkorient = Mat::zeros(mapRows, mapCols, CV_64F);

        std::vector<double> dataVector;
        dataVector.reserve(mapRows * mapCols);

        Mat im_roi, blkwim;
        Mat maskB1;
        Mat lcs = Mat::zeros(mapRows, mapCols, CV_64F);
        double cova, covb, covc;
        // Image processed NOT from beg to end but with a border around
        // - can't be vectorized:(
        int br = 0;
        int bc = 0;

        for (int r = blkoffset; r < rows - (blocksize + blkoffset - 1);
             r += blocksize) {
            for (int c = blkoffset; c < cols - (blocksize + blkoffset - 1);
                 c += blocksize) {
                im_roi = img(Range(r, min(r + blocksize, img.rows)),
                             Range(c, min(c + blocksize, img.cols)));
                //      blkim = im(r:r+blksz-1, c:c+blksz-1);
                maskB1 = maskim(Range(r, min(r + blocksize, maskim.rows)),
                                Range(c, min(c + blocksize, maskim.cols)));
                //      maskB1 = maskim(r:r+blksz-1,
                //      c:c+blksz-1);
                maskBseg.at<uint8_t>(br, bc) = allfun(maskB1);
                covcoef(im_roi, cova, covb, covc, CENTERED_DIFFERENCES);

                // ridge ORIENT local
                blkorient.at<double>(br, bc) = ridgeorient(cova, covb, covc);
                // overlapping windows (border = blkoffset)
                blkwim = img(Range(r - blkoffset,
                                   min(r + blocksize + blkoffset, img.rows)),
                             Range(c - blkoffset,
                                   min(c + blocksize + blkoffset, img.cols)));
                lcs.at<double>(br, bc) =
                    loclar(blkwim, blkorient.at<double>(br, bc), v1sz_x, v1sz_y,
                           scannerRes, padFlag);
                if (maskBseg.at<uint8_t>(br, bc) == 1) {
                    dataVector.push_back(lcs.at<double>(br, bc));
                }
                bc = bc + 1;
            }
            br = br + 1;
            bc = 0;
        }

        Scalar localClarityScore = mean(lcs, maskBseg);

        timeLCS = timerLCS.endTimerAndGetElapsedTime();

#if HISTOGRAM_FEATURES
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
        addHistogramFeatures(featureDataList, "LCS_Bin10_", histogramBins10,
                             dataVector, 10);
#endif

        if (m_bOutputSpeed) {
            NFIQ::QualityFeatureSpeed speed;
            speed.featureIDGroup = LCSFeature::speedFeatureIDGroup;
#if HISTOGRAM_FEATURES
            addHistogramFeatureNames(speed.featureIDs, "LCS_Bin10_", 10);
#endif
            speed.featureSpeed = timeLCS;
            m_lSpeedValues.push_back(speed);
        }
    } catch (cv::Exception& e) {
        std::stringstream ssErr;
        ssErr << "Cannot compute LCS: " << e.what();
        throw NFIQ::NFIQException(NFIQ::e_Error_FeatureCalculationError,
                                  ssErr.str());
    } catch (NFIQ::NFIQException& e) {
        throw e;
    } catch (...) {
        throw NFIQ::NFIQException(NFIQ::e_Error_FeatureCalculationError,
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
loclar(Mat& block, const double orientation, const int v1sz_x, const int v1sz_y,
       const int screenRes, const bool padFlag) {
    // sanity check: check block size
    float cBlock = static_cast<float>(block.rows) / 2;  // square block
    int icBlock = static_cast<int>(cBlock);
    if (icBlock != cBlock) {
        std::cerr << "block rows = " << block.rows << std::endl;
        std::cerr << "warning: Wrong block size! Consider block with "
                     "size of even number"
                  << std::endl;
    }

    Mat blockRotated;
    getRotatedBlock(block, orientation, padFlag, blockRotated);

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
    Mat v2 = blockRotated(Range(rowstart, rowend), Range(colstart, colend));

    std::vector<uint8_t> ridval;
    std::vector<double> dt;
    getRidgeValleyStructure(v2, ridval, dt);

    // Ridge-valley thickness
    //  begrid = ridval(1); % begining with ridge?
    //  change = xor(ridval,circshift(ridval,1)); // find the bin change
    //  change(1) = []; % there can't be change in 1. element (circshift)
    //  change = find(change == 1);    % find indices where changes
    uint8_t begrid = ridval[0];  //% begining with ridge?
    std::vector<uint8_t> change;
    int j;
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
         i++)  // skip the first element, same effect
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
        double Wrmax125 = 5.0;  // max ridge for 125 ppi scanner
        double Wvmax125 = 5.0;  // max valley for 125 ppi scanner

        double Wrmin = 3.0;  // [px]
        double Wrmax = 10.0;
        double Wvmin = 2.0;
        double Wvmax = 10.0;

        double sc = static_cast<double>(screenRes);
        double RscaleNorm = (sc / 125.0) * Wrmax125;
        double VscaleNorm = (sc / 125.0) * Wvmax125;

        // normalized max/min
        double NWrmin = Wrmin / RscaleNorm;
        double NWrmax = Wrmax / RscaleNorm;
        double NWvmin =
            Wvmin / RscaleNorm;  // Should this be Wvmin/VscaleNorm???
        double NWvmax = Wvmax / RscaleNorm;

        std::vector<double> NWr, NWv;
        double rtemp, vtemp;
        if (begrid) {
            for (unsigned int i = 0; i < Wrv.size(); i += 2) {
                rtemp = static_cast<double>(Wrv[i]) / RscaleNorm;
                NWr.push_back(rtemp);  // Matlab "odd" indices
            }
            for (unsigned int i = 0; i < Wrv.size() - 1; i += 2) {
                vtemp = static_cast<double>(Wrv[i + 1]) / VscaleNorm;
                NWv.push_back(vtemp);  // Matlab "even" indices
            }
        } else {
            for (unsigned int i = 0; i < Wrv.size(); i += 2) {
                vtemp = static_cast<double>(Wrv[i]) / VscaleNorm;
                NWv.push_back(vtemp);  // Matlab "odd" indices
            }
            for (unsigned int i = 0; i < Wrv.size() - 1; i += 2) {
                rtemp = static_cast<double>(Wrv[i + 1]) / RscaleNorm;
                NWr.push_back(rtemp);  // Matlab "even" indices
            }
        }

        // Clarity test
        // NOTE: can be different strategy how to deal with out of limit
        // ridge-valley thickness: NOTE: first and last region can be
        // INCOMPLETE -should be somehow excluded from the test 1: all
        // should fall in (except first/last) 2: majority 3: mean/median
        // of all if all(NWr >= NWrmin) && all(NWr <= NWrmax) && all(NWv
        // >= NWvmin) && all(NWv <= NWvmax)

        Scalar muNWr{}, muNWv{};
        if (!NWr.empty()) {
            muNWr = mean(NWr, noArray());
        }
        if (!NWv.empty()) {
            muNWv = mean(NWv, noArray());
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
            Mat ridgecol, valleycol;
            int ridgeGood = 0, valleyGood = 0;
            int ridgePixelCount = 0, valleyPixelCount = 0;
            for (int i = 0; i < v2.cols; i++) {
                if (ridval[i] == 1)  // ridges
                {
                    ridgecol = (v2.col(i) >= dt[i]);
                    ridgeGood += countNonZero(ridgecol);  // no. pixels in
                                                          // column that are
                                                          // above the
                                                          // threshold
                    ridgePixelCount += v2.rows;           // number of pixels in
                                                          // the column
                } else                                    // valleys
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
