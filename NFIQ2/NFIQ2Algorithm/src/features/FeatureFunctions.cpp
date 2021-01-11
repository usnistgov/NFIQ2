#include <nfiq2/features/FeatureFunctions.h>
#include <nfiq2/nfiqexception.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define USE_MATH_DEFINES
#include <math.h>

#include <cstring>
#include <iostream>
#include <limits>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static const int maxSampleCount = 50;

using namespace NFIQ;
using namespace cv;
/***
From the matlab code:
% RIDGESEGMENT - Normalises fingerprint image and segments ridge region
%
% Function identifies ridge regions of a fingerprint image and returns a
% mask identifying this region.  It also normalises the intesity values of
% the image so that the ridge regions have zero mean, unit standard
% deviation.
%
% This function breaks the image up into blocks of size blksze x blksze and
% evaluates the standard deviation in each region.  If the standard
% deviation is above the threshold it is deemed part of the fingerprint.
% Note that the image is normalised to have zero mean, unit standard
% deviation prior to performing this process so that the threshold you
% specify is relative to a unit standard deviation.
***/

void
ridgesegment(const Mat &img, int blksze, double thresh, OutputArray _normImage,
    Mat &maskImage, OutputArray _maskIndex)

{
	/***Convert the input image to double.
	Matlab: im = double(im);
	***/
	Mat double_im;
	img.convertTo(double_im, CV_64F);

	/***Normalize the image to have zero mean, unit standard deviation
	Matlab: im = (im-mean(im(:))) ./ std(im(:));
	***/
	Scalar imMean = 0, imStd = 0;
	meanStdDev(double_im, imMean, imStd, noArray());
	double_im = (double_im - imMean.val[0]) / imStd.val[0];
	/***For each block in the image, compute the standard deviation. Replace
	each element of the block with its standard deviation value. Matlab: fun
	= inline('std(x(:))*ones(size(x))'); stddevim = blkproc(im, [blksze
	blksze], fun);
	***/

	Mat stddevim = double_im.clone();
	Mat im_roi;

	for (int r = 0; r < stddevim.rows; r += blksze) {
		for (int c = 0; c < stddevim.cols; c += blksze) {
			// Range is open-ended on the upper end: r <= i < r +
			// blksze
			im_roi = stddevim(
			    Range(r, min(r + blksze, stddevim.rows)),
			    Range(c, min(c + blksze, stddevim.cols)));
			meanStdDev(im_roi, imMean, imStd, noArray());
			im_roi = imStd.val[0];
		}
	}

	maskImage.create(img.size(), CV_8UC1);
	// OpenCV: Result of comparison is an 8-bit single channel mask whose
	// elements are set to 255 (if the particular element satisfies the
	// condition) or 0. Matlab: result of comparison is 1 or 0;
	maskImage = (stddevim > thresh);

	if (_maskIndex.needed()) {
		/***Create the mask vector indicating ridge-like regions: get
		the linear indices of non-zero elements of the matrix Matlab:
		maskind = find(mask);
		***/
		std::vector<int32_t> maskIndex;
		int32_t vec_index = 1;
		int cols = maskImage.cols, rows = maskImage.rows;

		for (int j = 0; j < cols; j++) {
			for (int i = 0; i < rows; i++) {
				if (maskImage.at<uint8_t>(i, j) > 0) {
					maskIndex.push_back(vec_index);
				}
				vec_index++;
			}
		}
		_maskIndex.create(1, maskIndex.size(), CV_32SC1);
		_maskIndex.getMat() = Mat(maskIndex);
	}

	if (_normImage.needed()) {
		_normImage.create(double_im.size(), double_im.type());
		Mat normImage = _normImage.getMat();

		/***Renormalise image so that the *ridge regions* have zero
		mean, unit standard deviation. Matlab: im = im -
		mean(im(maskind)); normim = im/std(im(maskind));
		***/
		meanStdDev(double_im, imMean, imStd, maskImage);
		normImage = (double_im - imMean.val[0]) / imStd.val[0];
	}

	return;
}

/////////////////////////////////////////////////////////////////////////
/***function [a b c] = covcoef(blk)
%COVCOEF Computes covariance coefficients of grey level gradients
%   Computes coefficients of covariance matrix [a c; c b] of grey level
gradients inside %   a block specified as a parameter
%
% Syntax:
%   [a b c] = covcoef(blk)
%
% Inputs:
%   blk         - block of the image
%
% Outputs:
%   a         - param a of covariance matix [a c; c b]
%   b         - param b of covariance matix [a c; c b]
%   c         - param c of covariance matix [a c; c b]
%
***/
void
covcoef(const cv::Mat &imblock, double &a, double &b, double &c,
    ocl_type compMethod)
{
	/*** Compute the gradient of the input block.  In Matlab, this is done
	as follows:
	[fx fy] = gradient(double(blk));
	This function estimates the gradient using finite differences.

	In OpenCV, the gradient is estimated using the Sobel filter:
	gradient in X direction:
	Sobel( src_gray, grad_x, ddepth, 1, 0, 3 );
	gradient in Y direction:
	Sobel( src_gray, grad_y, ddepth, 0, 1, 3 );

	Here, we implement both, since the NFIQ2 documentation specifies Sobel
	operators, but the matlab implementation uses centered differences.  The
	comMethod parameter controls which gradient estimation method is used.
	***/

	Mat dfx, dfy, dfxT;
	Mat doubleIm;

	imblock.convertTo(doubleIm, CV_64F);

	if (compMethod == CENTERED_DIFFERENCES) {
		/* estimate the gradient in the y direction (across the rows) */
		diffGrad(doubleIm, dfy);
		/* estimate the gradient in the x direction (across the columns)
		 * by transposing the matrix */
		diffGrad(doubleIm.t(), dfxT);
		dfx = dfxT.t();
	} else // Sobel operator
	{
		try {
			Sobel(doubleIm, dfx, CV_64F, 1, 0, 3, 1, 0,
			    BORDER_REFLECT_101);
			Sobel(doubleIm, dfy, CV_64F, 0, 1, 3, 1, 0,
			    BORDER_REFLECT_101);
		} catch (cv::Exception &e) {
			std::stringstream ssErr;
			ssErr << "Call to OpenCV Sobel operator function "
				 "failed: "
			      << e.what();
			throw NFIQ::NFIQException(
			    NFIQ::e_Error_FeatureCalculationError, ssErr.str());
		}
	}

	/* Matlab:
	a = mean(fx(:).^2);
	b = mean(fy(:).^2);
	*/
	Scalar fxMean, fyMean, ProdMean;
	Mat dfx2 = dfx.mul(dfx);
	fxMean = mean(dfx2);
	Mat dfy2 = dfy.mul(dfy);
	fyMean = mean(dfy2);

	a = fxMean.val[0];
	b = fyMean.val[0];

	/*Matlab: c = fx.*fy; c = mean(c(:)); (Per-element multiplication) */
	Mat gradProd = dfx.mul(dfy);
	ProdMean = mean(gradProd);
	c = ProdMean.val[0];

	return;
}
/////////////////////////////////////////////////////////////////////////
/***
function orientang = ridgeorient(a, b, c)
% of block
%RIDGEORIENT Computes orientation angles
%    Computes angles of orientation lines perpendiular to the ridge direction
%
% Syntax:
%   orientang = ridgeorient(a, b, c)
%
% Inputs:
%   a         - param a of covariance matix [a c; c b]
%   b         - param b of covariance matix [a c; c b]
%   c         - param c of covariance matix [a c; c b]
%
% Outputs:
%   orientang - orientation angle of line perpendicular to the ridge flow

% 2011 Master Thesis, Vladimir Smida, vladimir.smida@[cased.de|gmail.com]
% FIT VUT, Czech Republic & CASED, Germany
***/

double
ridgeorient(double a, double b, double c)
{
	double denom, sin2theta, cos2theta, orientang;
	double temp;

	temp = a - b;
	denom = (c * c + temp * temp) + DBL_EPSILON;
	sin2theta = c / denom;
	cos2theta = temp / denom;
	orientang = atan2(sin2theta, cos2theta) / 2;

	return (orientang);
}

/////////////////////////////////////////////////////////////////////////

uint8_t
allfun(const Mat &Image)
/*** Returns 1 if all elements are nonzero, 0 otherwise.
% Equivalent to matlab:
% all elements of x should be nonzero, otherwise flag a
allfun = inline('all(x(:))');
***/
{
	uint8_t allNonZero = 1;
	for (int r = 0; r < Image.rows; r++) {
		for (int c = 0; c < Image.cols; c++) {
			if (Image.at<uint8_t>(r, c) > 0) {
				continue;
			} else {
				allNonZero = 0;
				break;
			}
		}
		if (allNonZero == 0) {
			break;
		}
	}
	return (allNonZero);
}

//////////////////////////////////////////////////////////////
/* This function computes the gradient across the rows of a 2D matrix using
forward differences at the edges and central differences elsewhere.
Spacing is 1.  The input matrix is assumed to be 64-bit floating point
*/
void
diffGrad(const Mat &inBlock, Mat &outBlock)
{
	outBlock.create(inBlock.size(), CV_64F);

	int nrows = inBlock.rows;
	/* Compute the forward differences at the edges. */

	outBlock.row(0) = inBlock.row(1) - inBlock.row(0);
	outBlock.row(nrows - 1) = inBlock.row(nrows - 1) -
	    inBlock.row(nrows - 2);

	/* Compute central differences elsewhere */

	for (int r = 1; r < nrows - 1; r++) {
		outBlock.row(r) = (inBlock.row(r + 1) - inBlock.row(r - 1)) /
		    2.0;
	}

	return;
}

//////////////////////////////////////////////////////////////
void
getRotatedBlock(
    const Mat &block, const double orientation, bool padFlag, Mat &rotatedBlock)
{
	const double Rad2Deg = 180.0 / M_PI;
	Mat rot_mat(2, 3, CV_64F);
	Mat Inblock;

	// sanity check: check block size
	float cBlock = static_cast<float>(block.rows) / 2; // square block
	int icBlock = static_cast<int>(cBlock);
	if (icBlock != cBlock) {
		std::cerr << "block rows = " << block.rows << std::endl;
		std::cerr << "warning: Wrong block size! Consider block with "
			     "size of even number"
			  << std::endl;
	}

	if (padFlag) {
		copyMakeBorder(block, Inblock, 2, 2, 2, 2, BORDER_CONSTANT, 0);
	} else {
		Inblock = block;
	}
	try {
		// rotate image to get the ridges vertical
		//   Matlab:  blockRotated = imrotate(block,
		//   rad2deg(orientation), 'nearest', 'crop');
		rotatedBlock.create(block.rows, block.cols, block.type());
		double orientDegrees = orientation * Rad2Deg;
		Point2f center(
		    ((float)Inblock.cols / 2.0f), ((float)Inblock.rows / 2.0f));
		rot_mat = getRotationMatrix2D(center, orientDegrees, 1);
		warpAffine(Inblock, rotatedBlock, rot_mat, rotatedBlock.size(),
		    INTER_NEAREST);
	} catch (cv::Exception &e) {
		std::stringstream ssErr;
		ssErr << "Exception during block rotation: " << e.what();
		throw NFIQ::NFIQException(
		    NFIQ::e_Error_FeatureCalculationError, ssErr.str());
	}

	return;
}
//////////////////////////////////////////////////////////////////////////////
void
getRidgeValleyStructure(const Mat &blockCropped, std::vector<uint8_t> &ridval,
    std::vector<double> &dt)
{
	// average profile of blockCropped: Compute average of each column to
	// get a projection of the grey values down the ridges.
	//    Matlab:  v3 = mean(blockCropped);
	//    Note: If A is a matrix, mean(A) treats the columns of A as
	//    vectors, returning
	//          a row vector of mean values.
	Mat v3 = Mat::zeros(blockCropped.cols, 1, CV_64F);
	Mat blockCol;
	Scalar colMean;
	for (int i = 0; i < blockCropped.cols; i++) {
		// extract a column from blockCropped
		blockCol = blockCropped.col(i);
		colMean = mean(blockCol, noArray());
		v3.at<double>(i, 0) = colMean.val[0];
	}

	// %% Linear regression using least square
	// % output = input * coefficients
	// % operator / "divide" the output by the input to get the linear
	// coefficients
	//   x = 1:length(v3);
	// % Append a column of ones before dividing to include an intercept,
	// dt1 = [intercept coefficient]
	//  dt1 = [ones(length(x),1) x'] \ v3';
	Mat dttemp(v3.rows, 2, CV_64F);
	for (int i = 0; i < v3.rows; i++) {
		dttemp.at<double>(i, 0) = 1;
		dttemp.at<double>(i, 1) = i + 1;
	}

	Mat dt1;
	try {
		solve(dttemp, v3, dt1, DECOMP_QR);
	} catch (cv::Exception &e) {
		std::stringstream ssErr;
		ssErr << "Exception during ridge/valley processing: "
			 "solve(DECOMP_QR) "
		      << e.what();
		throw NFIQ::NFIQException(
		    NFIQ::e_Error_FeatureCalculationError, ssErr.str());
	}

	// Round to 11 decimal points to preserve score consistency across
	// platforms (10^11)
#if CV_MAJOR_VERSION <= 2
	for (int i = 0; i < dt1.rows; i++) {
		for (int j = 0; j < dt1.cols; j++) {
			dt1.at<double>(
			    i, j) = round(dt1.at<double>(i, j) * 100000000000) /
			    100000000000;
		}
	}
#else
	dt1.forEach<double>([&](double &val, const int *position) {
		val = round(val * 100000000000) / 100000000000;
	});
#endif /* CV_MAJOR_VERSION */

	//%% Block segmentation into ridge and valley regions
	//  dt = x*dt1(2) + dt1(1);
	double tmpx, tmpi;
	for (int i = 0; i < v3.rows; i++) {
		tmpi = static_cast<double>(i + 1);
		tmpx = tmpi * dt1.at<double>(1, 0) + dt1.at<double>(0, 0);
		dt.push_back(tmpx);
	}
	// ridval = (v3 < dt)'; % ridges = 1, valleys = 0

	for (unsigned int i = 0; i < dt.size(); i++) {
		if (v3.at<double>(i, 0) < dt[i]) {
			ridval.push_back(1);
		} else {
			ridval.push_back(0);
		}
	}
	return;
}
//////////////////////////////////////////////////////////////////////////////

/**
 *
 * @param ks    kernel size
 * @param sig   sigma of the Gaussian envelope
 * @param th    theta (in degrees) represents the orientation of the normal to
 * the parallel stripes of the function
 * @param lm    lambda represents the wavelength of the sinusoidal factor
 * @param ps    psi (in degrees) is the phase offset
 * @return
 */
void
GaborFilterCx(const int ksize, const double theta, const double freq,
    const int sigma, Mat &FilterOut)
{
	int ks2 = (ksize - 1) / 2;
	double x1, y1;
	double tmp, tmpreal, tmpimag;
	double dsigma = static_cast<double>(sigma);
	double scale = 1.0 / pow(dsigma, 2);
	for (int i = -ks2; i <= ks2; i++) {
		for (int j = -ks2; j <= ks2; j++) {
			x1 = static_cast<double>(i) * sin(theta) +
			    static_cast<double>(j) * cos(theta);
			y1 = static_cast<double>(i) * cos(theta) -
			    static_cast<double>(j) * sin(theta);
			tmp = scale * (pow(x1, 2) + pow(y1, 2));
			tmp = exp(-0.5 * tmp);
			tmpreal = tmp * cos(2 * CV_PI * freq * x1);
			tmpimag = tmp * sin(2 * CV_PI * freq * x1);
			std::complex<double> fourier(tmpreal, tmpimag);
			FilterOut.at<std::complex<double>>(
			    j + ks2, i + ks2) = fourier;
		}
	}
	return;
}

///////////////////////////////////////////////////////////////////////////////

void
Conv2D(const Mat &imDFT, const Mat &filter, Mat &ConvOut, const Size &imageSize,
    const Size &dftSize, bool imDFTFlag)
{
	int kernSize = filter.rows;
	int OutType = filter.type();
	ConvOut.create(imageSize, OutType);

	// Input is the DFT of an image, and a complex-valued filter.  We
	// compute the DFT of the filter, multiply the two DFTs, then compute
	// the inverse transform.

	Mat kernTmp(dftSize, filter.type(), Scalar::all(0));
	// Copy the filter to the upper left corner of the tmp array
	Mat kernROI(kernTmp, Range(0, filter.rows), Range(0, filter.cols));
	filter.copyTo(kernROI);
	dft(kernTmp, kernTmp, DFT_COMPLEX_OUTPUT);

	// Multiply the two DFTs
	bool conjugateFlag = false;
	Mat MulOut;
	mulSpectrums(imDFT, kernTmp, MulOut, 0, conjugateFlag);

	Mat invDFT;
	// Get the inverse DFT
	dft(MulOut, invDFT, DFT_INVERSE + DFT_SCALE, 0);
	invDFT(Rect(0, 0, ConvOut.cols, ConvOut.rows)).copyTo(ConvOut);

	return;
}

////////////////////////////////////////
// compute coherence for COH/CS
double
calccoh(double gxx, double gyy, double gxy)
{
	return sqrt((gxx - gyy) * (gxx - gyy) + 4 * gxy * gxy) / (gxx + gyy);
}

double
calcof(double gsxavg, double gsyavg)
{
	double theta = 0;
	double phi = atan2(gsyavg, gsxavg);
	if (phi <= 0) {
		theta = phi + M_PI / 2;
	} else {
		theta = phi - M_PI / 2;
	}
	return theta;
}

cv::Mat
computeNumericalGradientX(const cv::Mat &mat)
{
	cv::Mat out(mat.rows, mat.cols, CV_64F);

	for (int y = 0; y < mat.rows; ++y) {
		const uchar *in_r = mat.ptr<uchar>(y);
		double *out_r = out.ptr<double>(y);

		out_r[0] = in_r[1] - in_r[0];
		for (int x = 1; x < mat.cols - 1; ++x) {
			out_r[x] = (in_r[x + 1] - in_r[x - 1]) / 2.0;
		}
		out_r[mat.cols - 1] = in_r[mat.cols - 1] - in_r[mat.cols - 2];
	}
	return out;
}

void
computeNumericalGradients(const cv::Mat &mat, cv::Mat &grad_x, cv::Mat &grad_y)
{
	// get x-gradient
	grad_x = computeNumericalGradientX(mat);
	// to get the y-gradient: take the x-gradient of the transpose matrix
	// and transpose it again
	grad_y = computeNumericalGradientX(mat.t()).t();
}

void
addSamplingFeatures(std::list<NFIQ::QualityFeatureResult> &featureDataList,
    std::string featurePrefix, std::vector<double> &dataVector)
{
	const int sampleSize = dataVector.size();

	// randomize data
	std::random_shuffle(dataVector.begin(), dataVector.end());

	for (int i = 0; i < maxSampleCount; i++) {
		NFIQ::QualityFeatureData fd;

		std::stringstream s;
		s << featurePrefix << i;

		fd.featureID = s.str();
		fd.featureDataType = NFIQ::e_QualityFeatureDataTypeDouble;
		bool canComputeValue = true;
		if (i < sampleSize) {
			fd.featureDataDouble = dataVector.at(i);
		} else {
			canComputeValue = false;
		}

		NFIQ::QualityFeatureResult result;
		result.featureData = fd;
		if (canComputeValue) {
			result.returnCode = 0;
		} else {
			result.returnCode =
			    NFIQ::e_Error_FeatureCalculationError;
		}

		featureDataList.push_back(result);
	}
}

void
addHistogramFeatures(std::list<NFIQ::QualityFeatureResult> &featureDataList,
    std::string featurePrefix, std::vector<double> &binBoundaries,
    std::vector<double> &dataVector, int binCount)
{
	binBoundaries.push_back(std::numeric_limits<double>::infinity());

	const int myBinCount = binBoundaries.size();

	if (myBinCount != binCount) {
		std::stringstream s;
		s << "Wrong histogram bin count for " << featurePrefix
		  << ". Should be " << binCount << " but is " << myBinCount;
		throw NFIQ::NFIQException(
		    NFIQ::e_Error_FeatureCalculationError, s.str());
	}

	std::sort(dataVector.begin(), dataVector.end());

	int *bins = new int[binCount];
	for (int i = 0; i < binCount; i++) {
		bins[i] = 0;
	}
	int currentBucket = 0;
	double currentBound = binBoundaries.at(currentBucket);

	for (std::vector<double>::iterator it = dataVector.begin();
	     it != dataVector.end(); ++it) {
		while (!cvIsInf(*it) && *it >= currentBound) {
			currentBucket++;
			currentBound = binBoundaries.at(currentBucket);
		}
		bins[currentBucket]++;
	}

	for (int i = 0; i < binCount; i++) {
		NFIQ::QualityFeatureData fd;

		std::stringstream s;
		s << featurePrefix << i;

		fd.featureID = s.str();
		fd.featureDataType = NFIQ::e_QualityFeatureDataTypeDouble;
		fd.featureDataDouble = bins[i];

		NFIQ::QualityFeatureResult result;
		result.featureData = fd;
		result.returnCode = 0;

		featureDataList.push_back(result);
	}

	Mat dataMat(dataVector);
	Scalar mean, stdDev;
	meanStdDev(dataMat, mean, stdDev);

	NFIQ::QualityFeatureData meanFD, stdDevFD;
	NFIQ::QualityFeatureResult meanFR, stdDevFR;
	std::stringstream meanSs, stdDevSs;

	meanSs << featurePrefix << "Mean";
	stdDevSs << featurePrefix << "StdDev";

	meanFD.featureID = meanSs.str();
	meanFD.featureDataType = NFIQ::e_QualityFeatureDataTypeDouble;
	meanFD.featureDataDouble = mean.val[0];
	meanFR.featureData = meanFD;
	meanFR.returnCode = 0;

	stdDevFD.featureID = stdDevSs.str();
	stdDevFD.featureDataType = NFIQ::e_QualityFeatureDataTypeDouble;
	stdDevFD.featureDataDouble = stdDev.val[0];
	stdDevFR.featureData = stdDevFD;
	stdDevFR.returnCode = 0;

	featureDataList.push_back(meanFR);
	featureDataList.push_back(stdDevFR);

	if (bins) {
		delete[] bins;
	}
}

void
addSamplingFeatureNames(
    std::list<std::string> &featureNames, const char *prefix)
{
	for (int i = 0; i < maxSampleCount; i++) {
		std::stringstream s;
		s << prefix << i;
		featureNames.push_back(s.str());
	}
}

void
addHistogramFeatureNames(
    std::list<std::string> &featureNames, const char *prefix, int binCount)
{
	for (int i = 0; i < binCount; i++) {
		std::stringstream s;
		s << prefix << i;
		featureNames.push_back(s.str());
	}
	std::stringstream meanSs, stdDevSs;
	meanSs << prefix << "Mean";
	stdDevSs << prefix << "StdDev";
	featureNames.push_back(meanSs.str());
	featureNames.push_back(stdDevSs.str());
}
