#include "include/FingerprintImageData.h"
#include "wsq.h"

int debug = 0;

#include <string.h>
#include <stdlib.h>

using namespace NFIQ;

FingerprintImageData::FingerprintImageData()
: Data(),
m_ImageWidth(0),
m_ImageHeight(0),
m_FingerCode(0),
m_ImageDPI(NFIQ::e_ImageResolution_500dpi)
{

}

FingerprintImageData::FingerprintImageData(
	uint32_t imageWidth,
	uint32_t imageHeight,
	uint8_t fingerCode,
	uint16_t imageDPI)
	: Data(),
	m_ImageWidth(imageWidth),
	m_ImageHeight(imageHeight),
	m_FingerCode(fingerCode),
	m_ImageDPI(imageDPI)
{

}

FingerprintImageData::FingerprintImageData(
	const uint8_t* pData, 
	uint32_t dataSize,
	uint32_t imageWidth,
	uint32_t imageHeight,
	uint8_t fingerCode,
	uint16_t imageDPI)
	: Data(pData, dataSize),
	m_ImageWidth(imageWidth),
	m_ImageHeight(imageHeight),
	m_FingerCode(fingerCode),
	m_ImageDPI(imageDPI)
{

}

FingerprintImageData::FingerprintImageData(const FingerprintImageData& otherData)
: Data(otherData)
{
	m_ImageWidth = otherData.m_ImageWidth;
	m_ImageHeight = otherData.m_ImageHeight;
	m_FingerCode = otherData.m_FingerCode;
	m_ImageDPI = otherData.m_ImageDPI;
}

FingerprintImageData::~FingerprintImageData()
{
}

NFIQ::FingerprintImageData FingerprintImageData::toBMP(bool topDown)
{
	// information is retrieved from member variables
	// as information needs to available for raw data

	NFIQ::FingerprintImageData bmpData;
	bmpData.m_FingerCode = this->m_FingerCode;
	bmpData.m_ImageDPI = this->m_ImageDPI;
	bmpData.m_ImageHeight = this->m_ImageHeight;
	bmpData.m_ImageWidth = this->m_ImageWidth;

	// number of padding bytes between the lines
	unsigned int padding = 3 - ((this->m_ImageWidth + 3) & 3);

	// file header
	BITMAPFILEHEADER fileHeader;
	fileHeader.bfType = htols((WORD)'B' | (WORD)'M' << 8);
	unsigned int headerSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) 
		+ 0x100 * sizeof(RGBQUAD); // headers and color table with 256 greyscales
	fileHeader.bfOffBits = htoll(headerSize);
	fileHeader.bfSize = htoll(fileHeader.bfOffBits + this->m_ImageHeight * (this->m_ImageWidth + padding));
	fileHeader.bfReserved1 = htols(0);
	fileHeader.bfReserved2 = htols(0);

	// info header
	BITMAPINFOHEADER infoHeader;
	infoHeader.biSize = htoll(sizeof(infoHeader));
	infoHeader.biWidth = htoll(this->m_ImageWidth);
	infoHeader.biHeight = htoll( topDown ? -(long)this->m_ImageHeight : (long)this->m_ImageHeight );
	infoHeader.biPlanes = htols(1);
	infoHeader.biBitCount = htols(8);
	infoHeader.biCompression = htoll(c_BI_RGB); // no compression
	unsigned int imgSize = this->m_ImageHeight * (this->m_ImageWidth + padding);
	infoHeader.biSizeImage = htoll(imgSize);
	infoHeader.biXPelsPerMeter = htoll(this->m_ImageDPI <= 0 ? 0 : (long)((double)this->m_ImageDPI / 0.0254 + 1.0) );
	infoHeader.biYPelsPerMeter = infoHeader.biXPelsPerMeter;
	infoHeader.biClrUsed = htoll(0x100);
	infoHeader.biClrImportant = htoll(0x100);

	uint32_t len = headerSize + imgSize; // allocate data buffer
	unsigned char* data = new unsigned char[len];
	unsigned char* buf = data;

	// write file header
	memcpy(buf, &fileHeader, sizeof(BITMAPFILEHEADER));
	buf += sizeof(BITMAPFILEHEADER);

	// write info header
	memcpy(buf, &infoHeader, sizeof(BITMAPINFOHEADER));
	buf += sizeof(BITMAPINFOHEADER);

	// write color table
	for (unsigned int i = 0; i < 0x100; i++) 
	{
		RGBQUAD color = {(BYTE)i, (BYTE)i, (BYTE)i, 0};
		memcpy(buf, &color, sizeof(RGBQUAD));
		buf += sizeof(RGBQUAD);
	}

	// write pixels
	for (unsigned int y = 0; y < this->m_ImageHeight; y++) 
	{
		int yy = topDown ? y : (this->m_ImageHeight - 1 - y);
		memcpy(buf, (uint8_t*)this->data() + yy * this->m_ImageWidth, this->m_ImageWidth);
		buf += this->m_ImageWidth;
		memset(buf, 0x00, padding);
		buf += padding;
	}

	// now append data
	bmpData.assign(data, len);
	delete[] data;

	return bmpData;
}

void FingerprintImageData::fromBMP(NFIQ::FingerprintImageData & bmpData)
{
	// ignore member variables (i.e. height and width) of bmpData 
	// because they might not yet have been set properly
	// import them from bitmap data

	// except for finger code
	this->m_FingerCode = bmpData.m_FingerCode;

	const unsigned char* buf = (uint8_t*)bmpData.data();
	unsigned int len = bmpData.length();

	// get bitmap file header
	BITMAPFILEHEADER fileHeader;
	if ( !read_buf( &fileHeader, sizeof( fileHeader ), buf, len ) )
		throw NFIQ::NFIQException(NFIQ::e_Error_WrongFileType, "Wrong file type: cannot read bitmap file header");

	if (ltohs(fileHeader.bfType) != ((WORD)'B' | (WORD)'M' << 8))
		throw NFIQ::NFIQException(NFIQ::e_Error_WrongFileType, "Wrong file type: no bitmap file found");

	// get bitmap info header
	BITMAPINFOHEADER infoHeader;
	if ( !read_buf( &infoHeader, sizeof( infoHeader ), buf, len ) )
		throw NFIQ::NFIQException(NFIQ::e_Error_WrongFileType, "Wrong file type: cannot read bitmap info header");

	if (ltohl(infoHeader.biSize) != sizeof( infoHeader ) ||
		ltohs(infoHeader.biPlanes) != 1 ||
		ltohl(infoHeader.biCompression) != c_BI_RGB ) // no compression
		throw NFIQ::NFIQException(NFIQ::e_Error_WrongFileType, "Wrong file type: wrong info header detected (e.g. compression, planes, size)");

	// get bit count
	unsigned int bitCount = ltohs(infoHeader.biBitCount);
	unsigned long clrUsed = ltohl( infoHeader.biClrUsed );
	if ( bitCount == 8 )
	{
		if ( clrUsed == 0 )
			clrUsed = 256;
		if ( clrUsed > 256 )
			throw NFIQ::NFIQException(NFIQ::e_Error_WrongFileType, "Wrong file type: clrUsed is bigger than 256");
	}
	else
	{
		//other bitmap formats than 8 bpp are not supported
		throw NFIQ::NFIQException(NFIQ::e_Error_WrongFileType, "Wrong file type: only 8bit (greyscale) bitmaps are supported");
	}

	// color table
	RGBQUAD * colorTab = new RGBQUAD[clrUsed];
	if ( !read_buf( colorTab, sizeof( RGBQUAD ) * clrUsed, buf, len ) )
		throw NFIQ::NFIQException(NFIQ::e_Error_WrongFileType, "Wrong file type: cannot read color table");


	// get height and width
	int height = ltohl(infoHeader.biHeight);
	bool bottom_up = height >= 0; //bottom-up bitmap?
	if (height < 0)
		this->m_ImageHeight = -height;
	else
		this->m_ImageHeight = height;
	this->m_ImageWidth = ltohl(infoHeader.biWidth);

	// move to start of pixels
	buf = (uint8_t*)bmpData.data() + ltohl(fileHeader.bfOffBits);
	len = bmpData.length() - ltohl(fileHeader.bfOffBits);
	this->resize(this->m_ImageWidth * this->m_ImageHeight);

	// number of padding bytes between lines
	unsigned int pad_cnt = 3 - ((this->m_ImageWidth * (bitCount >> 3) + 3) & 3);

	// now add raw data
	unsigned int y = 0;
	for (y = 0; y < this->m_ImageHeight; y++)
	{
		int pos = (bottom_up ? this->m_ImageHeight - 1 - y : y) * this->m_ImageWidth;
		unsigned int x = 0;
		for (x = 0; x < this->m_ImageWidth; x++, pos++)
		{
			// get pixel from bitmap
			unsigned char pixel[3];
			if( bitCount == 8 )
			{ // 8 bpp
				unsigned char idx;
				if ( !read_buf( &idx, 1, buf, len ) ) // color index from file
					break;
				if ( idx > clrUsed )
					break;
				memcpy( pixel, colorTab + idx, 3 ); // pixel data from color table
			}

			// store pixel data: RGB ---> greyscale
			this->at(pos) = (unsigned char)(((unsigned short)pixel[0] + (unsigned short)pixel[1] + (unsigned short)pixel[2]) / 3);
		}
		if (x < this->m_ImageWidth ) //error in for-loop (x)
			break;
		if ( !read_buf( NULL, pad_cnt, buf, len ) ) // skip padding between lines
			break;
	}
	if( y < this->m_ImageHeight )
	{ // error in for-loop (y)
		delete [] colorTab;
		throw NFIQ::NFIQException(NFIQ::e_Error_WrongFileType, "Wrong file type: cannot read pixel data");
	}

	// get dpi
	this->m_ImageDPI = (int)(((double)ltohl( infoHeader.biXPelsPerMeter ) + 
		(double)ltohl( infoHeader.biXPelsPerMeter )) * 0.0254 / 2.0 + 0.5);
	if (this->m_ImageDPI <= 0)
		this->m_ImageDPI = NFIQ::e_ImageResolution_500dpi; //default

	delete [] colorTab;

}

NFIQ::FingerprintImageData FingerprintImageData::toWSQ(float compressionRate)
{
	// information is retrieved from member variables
	// as information needs to available for raw data

	NFIQ::FingerprintImageData wsqData;
	wsqData.m_FingerCode = this->m_FingerCode;
	wsqData.m_ImageDPI = this->m_ImageDPI;
	if (wsqData.m_ImageDPI <= 0) 
		wsqData.m_ImageDPI = NFIQ::e_ImageResolution_500dpi;
	wsqData.m_ImageHeight = this->m_ImageHeight;
	wsqData.m_ImageWidth = this->m_ImageWidth;
	// check for image dimensions
	if (this->m_ImageHeight < 256 || this->m_ImageHeight < 256)
		throw NFIQ::NFIQException(NFIQ::e_Error_ImageConversionError, "Image conversion error: Image dimensions must be at least 256 pixels");

	// check for compression rate
	if (compressionRate < 0 || compressionRate > 8)
		throw NFIQ::NFIQException(NFIQ::e_Error_ImageConversionError, "Image conversion error: Invalid compression rate");

	// convert to WSQ
	int wsq_len = 0;
	unsigned char* wsq_data = NULL;
	if (wsq_encode_mem( &wsq_data, &wsq_len, compressionRate, (unsigned char*)this->data(), 
		this->m_ImageWidth, this->m_ImageHeight, 8, wsqData.m_ImageDPI, (char*)"NIST-WSQ-NFIQ2") != 0 )
	{
		throw NFIQ::NFIQException(NFIQ::e_Error_ImageConversionError, "Image conversion error: Cannot encode to WSQ");
	}

	// assign WSQ data
	wsqData.assign(wsq_data, wsq_len);
	free(wsq_data);

	return wsqData;
}

void FingerprintImageData::fromWSQ(NFIQ::FingerprintImageData & wsqData)
{
	// ignore member variables (i.e. height and width) of wsqData 
	// because they might not yet have been set properly
	// import them from WSQ data

	// except for finger code
	this->m_FingerCode = wsqData.m_FingerCode;

	// decode WSQ
	int raw_w = 0;
	int raw_h = 0;
	int raw_d = 0;
	int raw_ppi = 0;
	unsigned char* raw_data = NULL;
	int lossyflag = 0;
	if (wsq_decode_mem(&raw_data, &raw_w, &raw_h, &raw_d, &raw_ppi, &lossyflag,
		(unsigned char*)wsqData.data(), wsqData.length()) != 0)
	{
		throw NFIQ::NFIQException(NFIQ::e_Error_ImageConversionError, "Image conversion error: Cannot decode from WSQ");
	}

	// check results
	if (raw_d != 8)
	{
		free(raw_data);
		throw NFIQ::NFIQException(NFIQ::e_Error_ImageConversionError, "Image conversion error: Cannot decode WSQ with other bitdepth than 8");
	}

	if (raw_w <= 0 || raw_d <= 0)
	{
		free(raw_data);
		throw NFIQ::NFIQException(NFIQ::e_Error_ImageConversionError, "Image conversion error: Wrong image dimensions detected");
	}

	// get results
	this->m_ImageDPI = raw_ppi;
	if (this->m_ImageDPI <= 0)
		this->m_ImageDPI = NFIQ::e_ImageResolution_500dpi;
	this->m_ImageHeight = raw_h;
	this->m_ImageWidth = raw_w;
	this->assign(raw_data, raw_h * raw_w);

	free(raw_data);
}

NFIQ::FingerprintImageData FingerprintImageData::removeWhiteFrameAroundFingerprint()
{
	// make local copy of internal fingerprint image
	NFIQ::FingerprintImageData localFingerprintImage(this->m_ImageWidth, this->m_ImageHeight, 
		this->m_FingerCode, this->m_ImageDPI);
	// copy data now
	localFingerprintImage.resize(this->size());
	memcpy((void*)localFingerprintImage.data(), this->data(), this->size());

	cv::Mat img;
	try
	{
		// get matrix from fingerprint image
		img = cv::Mat(localFingerprintImage.m_ImageHeight, localFingerprintImage.m_ImageWidth, CV_8UC1, (void*)localFingerprintImage.data());
	}
	catch (cv::Exception & e)
	{
		std::stringstream ssErr;
		ssErr << "Cannot get matrix from fingerprint image: " << e.what();
		throw NFIQ::NFIQException(NFIQ::e_Error_FeatureCalculationError, ssErr.str());
	}

	// start from top of image and find top row index that is already part of the fingerprint image
	int topRowIndex = 0;
	for (int i = 0; i < img.rows; i++)
	{
		double mu = computeMuFromRow(i, img);
		if (mu <= MU_THRESHOLD)
		{
			// Mu is not > threshold anymore -> top row index found
			if (i == 0)
				topRowIndex = i;
			else
				topRowIndex = (i - 1);
			break;
		}
	}

	// start from bottom of image and find bottom row index that is already part of the fingerprint image
	int bottomRowIndex = (img.rows - 1);
	for (int i = (img.rows - 1); i >= 0; i--)
	{
		double mu = computeMuFromRow(i, img);
		if (mu <= MU_THRESHOLD)
		{
			// Mu is not > threshold anymore -> bottom row index found
			if (i == (img.rows - 1))
				bottomRowIndex = i;
			else
				bottomRowIndex = (i + 1);
			break;
		}
	}

	// start from left of image and find left index that is already part of the fingerprint image
	int leftIndex = 0;
	for (int j = 0; j < img.cols; j++)
	{
		double mu = computeMuFromColumn(j, img);
		if (mu <= MU_THRESHOLD)
		{
			// Mu is not > threshold anymore -> left index found
			if (j == 0)
				leftIndex = j;
			else
				leftIndex = (j - 1);
			break;
		}
	}

	// start from right of image and find right index that is already part of the fingerprint image
	int rightIndex = (img.cols - 1);
	for (int j = (img.cols - 1); j >= 0; j--)
	{
		double mu = computeMuFromColumn(j, img);
		if (mu <= MU_THRESHOLD)
		{
			// Mu is not > threshold anymore -> right index found
			if (j == (img.cols - 1))
				rightIndex = j;
			else
				rightIndex = (j + 1);
			break;
		}
	}

	// now crop image according to detected border indices
	int width = rightIndex - leftIndex + 1;
	if (width < 0)
		width = img.cols;
	int height = bottomRowIndex - topRowIndex + 1;
	if (height < 0)
		height = img.rows;
	cv::Rect roi(leftIndex, topRowIndex, width, height);
	cv::Mat roiImg = img(roi);

	NFIQ::FingerprintImageData croppedImage;
	croppedImage.m_ImageHeight = roiImg.rows;
	croppedImage.m_ImageWidth = roiImg.cols;
	croppedImage.m_FingerCode = this->m_FingerCode;
	croppedImage.m_ImageDPI = this->m_ImageDPI;
	// copy data now
	unsigned int size = roiImg.rows * roiImg.cols;
	croppedImage.resize(size);
	unsigned int counter = 0;
	for (int i = 0; i < roiImg.rows; i++)
	{
		for (int j = 0; j < roiImg.cols; j++)
		{
			croppedImage.at(counter) = roiImg.at<uchar>(i, j);
			counter++;
		}
	}

	return croppedImage;
}

double FingerprintImageData::computeMuFromRow(unsigned int rowIndex, cv::Mat & img)
{
	double mu = 0.0;
	for (int j = 0; j < img.cols; j++)
	{
		// get gray value of image (0 = black, 255 = white)
		mu += (double)img.at<uchar>(rowIndex, j);	
	}

	mu /= img.cols;
	return mu;
}

double FingerprintImageData::computeMuFromColumn(unsigned int columnIndex, cv::Mat & img)
{
	double mu = 0.0;
	for (int i = 0; i < img.rows; i++)
	{
		// get gray value of image (0 = black, 255 = white)
		mu += (double)img.at<uchar>(i, columnIndex);	
	}

	mu /= img.rows;
	return mu;
}

bool FingerprintImageData::read_buf( void * dest, unsigned int dest_size, const unsigned char * & buf, unsigned int & buf_len )
{
	bool success = true;

	if( dest_size > buf_len ) {
		dest_size = buf_len;
		success =  false;
	}

	if( dest != NULL )
		memcpy( dest, buf, dest_size );
	buf += dest_size;
	buf_len -= dest_size;

	return success;
}

bool FingerprintImageData::write_buf( void * src, unsigned int src_size, unsigned char * & buf, unsigned int & buf_len )
{
	bool success = true;

	if( src_size > buf_len ) {
		src_size = buf_len;
		success =  false;
	}

	if( src != NULL )
		memcpy( buf, src, src_size );
	else
		memset( buf, 0, src_size );
	buf += src_size;
	buf_len -= src_size;

	return success;
}
