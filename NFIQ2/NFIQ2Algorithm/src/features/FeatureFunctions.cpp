
#include <features/FeatureFunctions.h>
#include <nfiq2_exception.hpp>
#include <opencv2/imgproc.hpp>

#include <cmath>
#include <cstring>
#include <limits>

static const int maxSampleCount = 50;

static const char FeaturFunctionsStdDevSuffix[] = "StdDev";
static const char FeaturFunctionsMeanSuffix[] = "Mean";

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
NFIQ2::QualityFeatures::ridgesegment(const cv::Mat &img, int blksze,
    double thresh, cv::OutputArray _normImage, cv::Mat &maskImage,
    cv::OutputArray _maskIndex)

{
	/***Convert the input image to double.
	Matlab: im = double(im);
	***/
	cv::Mat double_im;
	img.convertTo(double_im, CV_64F);

	/***Normalize the image to have zero mean, unit standard deviation
	Matlab: im = (im-mean(im(:))) ./ std(im(:));
	***/
	cv::Scalar imMean = 0, imStd = 0;
	cv::meanStdDev(double_im, imMean, imStd, cv::noArray());
	double_im = (double_im - imMean.val[0]) / imStd.val[0];
	/***For each block in the image, compute the standard deviation. Replace
	each element of the block with its standard deviation value. Matlab: fun
	= inline('std(x(:))*ones(size(x))'); stddevim = blkproc(im, [blksze
	blksze], fun);
	***/

	cv::Mat stddevim = double_im.clone();
	cv::Mat im_roi;

	for (int r = 0; r < stddevim.rows; r += blksze) {
		for (int c = 0; c < stddevim.cols; c += blksze) {
			// cv::Range is open-ended on the upper end: r <= i < r
			// + blksze
			im_roi = stddevim(
			    cv::Range(r, cv::min(r + blksze, stddevim.rows)),
			    cv::Range(c, cv::min(c + blksze, stddevim.cols)));
			cv::meanStdDev(im_roi, imMean, imStd, cv::noArray());
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
		_maskIndex.getMat() = cv::Mat(maskIndex);
	}

	if (_normImage.needed()) {
		_normImage.create(double_im.size(), double_im.type());
		cv::Mat normImage = _normImage.getMat();

		/***Renormalise image so that the *ridge regions* have zero
		mean, unit standard deviation. Matlab: im = im -
		mean(im(maskind)); normim = im/std(im(maskind));
		***/
		cv::meanStdDev(double_im, imMean, imStd, maskImage);
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
NFIQ2::QualityFeatures::covcoef(const cv::Mat &imblock, double &a, double &b,
    double &c, ocl_type compMethod)
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

	cv::Mat dfx, dfy, dfxT;
	cv::Mat doubleIm;

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
			cv::Sobel(doubleIm, dfx, CV_64F, 1, 0, 3, 1, 0,
			    cv::BORDER_REFLECT_101);
			cv::Sobel(doubleIm, dfy, CV_64F, 0, 1, 3, 1, 0,
			    cv::BORDER_REFLECT_101);
		} catch (const cv::Exception &e) {
			std::stringstream ssErr;
			ssErr << "Call to OpenCV Sobel operator function "
				 "failed: "
			      << e.what();
			throw NFIQ2::Exception(
			    NFIQ2::ErrorCode::FeatureCalculationError,
			    ssErr.str());
		}
	}

	/* Matlab:
	a = mean(fx(:).^2);
	b = mean(fy(:).^2);
	*/
	cv::Scalar fxMean, fyMean, ProdMean;
	cv::Mat dfx2 = dfx.mul(dfx);
	fxMean = cv::mean(dfx2);
	cv::Mat dfy2 = dfy.mul(dfy);
	fyMean = cv::mean(dfy2);

	a = fxMean.val[0];
	b = fyMean.val[0];

	/*Matlab: c = fx.*fy; c = mean(c(:)); (Per-element multiplication) */
	cv::Mat gradProd = dfx.mul(dfy);
	ProdMean = cv::mean(gradProd);
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
NFIQ2::QualityFeatures::ridgeorient(double a, double b, double c)
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
NFIQ2::QualityFeatures::allfun(const cv::Mat &Image)
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
NFIQ2::QualityFeatures::diffGrad(const cv::Mat &inBlock, cv::Mat &outBlock)
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
NFIQ2::QualityFeatures::getRotatedBlock(const cv::Mat &block,
    const double orientation, bool padFlag, cv::Mat &rotatedBlock)
{
	const double Rad2Deg = 180.0 / M_PI;
	cv::Mat rot_mat(2, 3, CV_64F);
	cv::Mat Inblock;

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

	if (padFlag) {
		cv::copyMakeBorder(
		    block, Inblock, 2, 2, 2, 2, cv::BORDER_CONSTANT, 0);
	} else {
		Inblock = block;
	}
	try {
		// rotate image to get the ridges vertical
		//   Matlab:  blockRotated = imrotate(block,
		//   rad2deg(orientation), 'nearest', 'crop');
		rotatedBlock.create(block.rows, block.cols, block.type());
		double orientDegrees = orientation * Rad2Deg;
		cv::Point2f center(
		    ((float)Inblock.cols / 2.0f), ((float)Inblock.rows / 2.0f));
		rot_mat = getRotationMatrix2D(center, orientDegrees, 1);
		cv::warpAffine(Inblock, rotatedBlock, rot_mat,
		    rotatedBlock.size(), cv::INTER_NEAREST);
	} catch (const cv::Exception &e) {
		std::stringstream ssErr;
		ssErr << "Exception during block rotation: " << e.what();
		throw NFIQ2::Exception(
		    NFIQ2::ErrorCode::FeatureCalculationError, ssErr.str());
	}

	return;
}
//////////////////////////////////////////////////////////////////////////////
void
NFIQ2::QualityFeatures::getRidgeValleyStructure(const cv::Mat &blockCropped,
    std::vector<uint8_t> &ridval, std::vector<double> &dt)
{
	// average profile of blockCropped: Compute average of each column to
	// get a projection of the grey values down the ridges.
	//    Matlab:  v3 = mean(blockCropped);
	//    Note: If A is a matrix, mean(A) treats the columns of A as
	//    vectors, returning
	//          a row vector of mean values.
	cv::Mat v3 = cv::Mat::zeros(blockCropped.cols, 1, CV_64F);
	cv::Mat blockCol;
	cv::Scalar colMean;
	for (int i = 0; i < blockCropped.cols; i++) {
		// extract a column from blockCropped
		blockCol = blockCropped.col(i);
		colMean = cv::mean(blockCol, cv::noArray());
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
	cv::Mat dttemp(v3.rows, 2, CV_64F);
	for (int i = 0; i < v3.rows; i++) {
		dttemp.at<double>(i, 0) = 1;
		dttemp.at<double>(i, 1) = i + 1;
	}

	cv::Mat dt1;
	try {
		cv::solve(dttemp, v3, dt1, cv::DECOMP_QR);
	} catch (const cv::Exception &e) {
		std::stringstream ssErr;
		ssErr << "Exception during ridge/valley processing: "
			 "cv::solve(cv::DECOMP_QR) "
		      << e.what();
		throw NFIQ2::Exception(
		    NFIQ2::ErrorCode::FeatureCalculationError, ssErr.str());
	}

	// Round to 10 decimal points to preserve score consistency across
	// platforms (10^10)

	dt1.forEach<double>([&](double &val, const int *) {
		val = round(val * 10000000000) / 10000000000;
	});

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
NFIQ2::QualityFeatures::GaborFilterCx(const int ksize, const double theta,
    const double freq, const int sigma, cv::Mat &FilterOut)
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
NFIQ2::QualityFeatures::Conv2D(const cv::Mat &imDFT, const cv::Mat &filter,
    cv::Mat &ConvOut, const cv::Size &imageSize, const cv::Size &dftSize)
{
	int OutType = filter.type();
	ConvOut.create(imageSize, OutType);

	// Input is the DFT of an image, and a complex-valued filter.  We
	// compute the DFT of the filter, multiply the two DFTs, then compute
	// the inverse transform.

	cv::Mat kernTmp(dftSize, filter.type(), cv::Scalar::all(0));
	// Copy the filter to the upper left corner of the tmp array
	cv::Mat kernROI(
	    kernTmp, cv::Range(0, filter.rows), cv::Range(0, filter.cols));
	filter.copyTo(kernROI);
	cv::dft(kernTmp, kernTmp, cv::DFT_COMPLEX_OUTPUT);

	// Multiply the two DFTs
	bool conjugateFlag = false;
	cv::Mat MulOut;
	mulSpectrums(imDFT, kernTmp, MulOut, 0, conjugateFlag);

	cv::Mat invDFT;
	// Get the inverse cv::DFT
	cv::dft(MulOut, invDFT, cv::DFT_INVERSE + cv::DFT_SCALE, 0);
	invDFT(cv::Rect(0, 0, ConvOut.cols, ConvOut.rows)).copyTo(ConvOut);

	return;
}

////////////////////////////////////////
// compute coherence for COH/CS
double
NFIQ2::QualityFeatures::calccoh(double gxx, double gyy, double gxy)
{
	return sqrt((gxx - gyy) * (gxx - gyy) + 4 * gxy * gxy) / (gxx + gyy);
}

double
NFIQ2::QualityFeatures::calcof(double gsxavg, double gsyavg)
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
NFIQ2::QualityFeatures::computeNumericalGradientX(const cv::Mat &mat)
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
NFIQ2::QualityFeatures::computeNumericalGradients(
    const cv::Mat &mat, cv::Mat &grad_x, cv::Mat &grad_y)
{
	// get x-gradient
	grad_x = computeNumericalGradientX(mat);
	// to get the y-gradient: take the x-gradient of the transpose matrix
	// and transpose it again
	grad_y = computeNumericalGradientX(mat.t()).t();
}

void
NFIQ2::QualityFeatures::addHistogramFeatures(
    std::unordered_map<std::string, double> &featureDataList,
    std::string featurePrefix, std::vector<double> &binBoundaries,
    std::vector<double> &dataVector, int binCount)
{
	binBoundaries.push_back(std::numeric_limits<double>::infinity());

	const auto myBinCount = binBoundaries.size();

	if (myBinCount != binCount) {
		std::stringstream s;
		s << "Wrong histogram bin count for " << featurePrefix
		  << ". Should be " << binCount << " but is " << myBinCount;
		throw NFIQ2::Exception(
		    NFIQ2::ErrorCode::FeatureCalculationError, s.str());
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
		std::pair<std::string, double> fd;

		std::stringstream s;
		s << featurePrefix << i;

		fd = std::make_pair(s.str(), bins[i]);

		featureDataList[fd.first] = fd.second;
	}

	cv::Mat dataMat(dataVector);
	cv::Scalar mean, stdDev;
	cv::meanStdDev(dataMat, mean, stdDev);

	std::pair<std::string, double> meanFD, stdDevFD;
	std::stringstream meanSs, stdDevSs;

	meanSs << featurePrefix << FeaturFunctionsMeanSuffix;
	stdDevSs << featurePrefix << FeaturFunctionsStdDevSuffix;

	meanFD = std::make_pair(meanSs.str(), mean.val[0]);

	stdDevFD = std::make_pair(stdDevSs.str(), stdDev.val[0]);

	featureDataList[meanFD.first] = meanFD.second;
	featureDataList[stdDevFD.first] = stdDevFD.second;

	if (bins) {
		delete[] bins;
	}
}

void
NFIQ2::QualityFeatures::addSamplingFeatureNames(
    std::vector<std::string> &featureNames, const char *prefix)
{
	for (int i = 0; i < maxSampleCount; i++) {
		std::stringstream s;
		s << prefix << i;
		featureNames.push_back(s.str());
	}
}

void
NFIQ2::QualityFeatures::addHistogramFeatureNames(
    std::vector<std::string> &featureNames, const std::string &prefix,
    int binCount)
{
	for (int i = 0; i < binCount; i++) {
		std::stringstream s;
		s << prefix << i;
		featureNames.push_back(s.str());
	}
	std::stringstream meanSs, stdDevSs;
	meanSs << prefix << FeaturFunctionsMeanSuffix;
	stdDevSs << prefix << FeaturFunctionsStdDevSuffix;
	featureNames.push_back(meanSs.str());
	featureNames.push_back(stdDevSs.str());
}
