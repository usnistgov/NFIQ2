#ifndef FINGERPRINTIMAGEDATA_H
#define FINGERPRINTIMAGEDATA_H

#include "include/Data.h"

#if defined WIN32 || defined WINDOWS
#include <windows.h>
#else
typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
#endif

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#define MU_THRESHOLD 250

#define c_BI_RGB 0

namespace NFIQ
{

#pragma pack(1)

	typedef struct BITMAPFILEHEADER_t
	{
		WORD bfType;
		DWORD bfSize;
		WORD bfReserved1;
		WORD bfReserved2;
		DWORD bfOffBits;
	} BITMAPFILEHEADER;

	typedef struct BITMAPINFOHEADER_t
	{
		DWORD biSize;
		DWORD biWidth;
		DWORD biHeight;
		WORD biPlanes;
		WORD biBitCount;
		DWORD biCompression;
		DWORD biSizeImage;
		DWORD biXPelsPerMeter; 
		DWORD biYPelsPerMeter;
		DWORD biClrUsed;
		DWORD biClrImportant;
	} BITMAPINFOHEADER;

	typedef struct RGBQUAD_t
	{
		BYTE rgbBlue;
		BYTE rgbGreen;
		BYTE rgbRed;
		BYTE rgbReserved;
	} RGBQUAD;

#pragma pack()

	/**
	* This type represents the resolution of an image (in dpi)
	*/
	typedef enum image_resolution_e
	{
		e_ImageResolution_500dpi = 500,		///< 500 dpi
		e_ImageResolution_1000dpi = 1000	///< 1000 dpi
	} ImageResolution;

	/**
	******************************************************************************
	* @class FingerprintImageData
	* @brief This class manages fingerprint image data (derived from class Data).
	******************************************************************************/
	class FingerprintImageData : public Data
	{
	public:

		/******************************************************************************/
		// --- Constructor / Destructor --- //
		/******************************************************************************/

		/**
		* @brief Default constructor of FingerprintImageData
		*/
		FingerprintImageData();

		/**
		* @brief Constructor of FingerprintImageData
		*/
		FingerprintImageData(
			uint32_t imageWidth,
			uint32_t imageHeight,
			uint8_t fingerCode,
			uint16_t imageDPI);

		/**
		* @brief Constructor of data with available pointer to data
		*/
		FingerprintImageData(
			const uint8_t* pData, 
			uint32_t dataSize,
			uint32_t imageWidth,
			uint32_t imageHeight,
			uint8_t fingerCode,
			uint16_t imageDPI);

		/**
		* @brief Copy constructor
		*/
		FingerprintImageData(const FingerprintImageData& otherData); 

		/**
		* @brief Destructor
		*/
		virtual ~FingerprintImageData();

		/******************************************************************************/
		// --- Public Members --- //
		/******************************************************************************/

		uint32_t	m_ImageWidth;	///< Width of the fingerprint image (in pixels)
		uint32_t	m_ImageHeight;	///< Height of the fingerprint image (in pixels)
		uint8_t		m_FingerCode;	///< ISO finger code of the fingerprint in the image
		int16_t		m_ImageDPI;	///< Dots per inch of the fingerprint image (default: 500 dpi)

		/******************************************************************************/
		// --- Image Format Conversion Functions --- //
		/******************************************************************************/

		/**
		* @fn toBMP
		* @brief Converts the fingerprint image into Bitmap (BMP) format.
		* @param topDown Image is saved as top down or bottom up.
		* @return The fingerprint image in BMP format.
		* @throws NFIQException
		*/
		NFIQ::FingerprintImageData toBMP(bool topDown = false);

		/**
		* @fn fromBMP
		* @brief Imports a fingerprint image from BMP data into the current object (internally represented in raw data format).
		* @param bmpData The fingerprint image in BMP format.
		* @throws NFIQException
		*/
		void fromBMP(NFIQ::FingerprintImageData & bmpData);

		/**
		* @fn toWSQ
		* @brief Converts the fingerprint image into WSQ format.
		* @param compressionRate The rate for WSQ compression.
		* @return The fingerprint image in WSQ format.
		* @throws NFIQException
		*/
		NFIQ::FingerprintImageData toWSQ(float compressionRate);

		/**
		* @fn fromWSQ
		* @brief Imports a fingerprint image from WSQ data into the current object (internally represented in raw data format).
		* @param wsqData The fingerprint image in WSQ format.
		* @throws NFIQException
		*/
		void fromWSQ(NFIQ::FingerprintImageData & wsqData);

		/**
		* @fn removeWhiteFrameAroundFingerprint
		* @brief Uses a simple algorithm to remove white lines around the fingerprint image (internally represented in raw data format).
		* @return The cropped/segmented fingerprint image in raw format.
		* @throws NFIQException
		*/
		NFIQ::FingerprintImageData removeWhiteFrameAroundFingerprint();


		/******************************************************************************/
		// --- Helper Functions --- //
		/******************************************************************************/

		// convert to little endian
		static inline WORD htols( const WORD in )
		{
			WORD out;
			((BYTE *)&out)[0] = (BYTE)in;
			((BYTE *)&out)[1] = (BYTE)(in >> 8);
			return out;
		}
		static inline DWORD htoll( const DWORD in )
		{
			DWORD out;
			((BYTE *)&out)[0] = (BYTE)in;
			((BYTE *)&out)[1] = (BYTE)(in >> 8);
			((BYTE *)&out)[2] = (BYTE)(in >> 16);
			((BYTE *)&out)[3] = (BYTE)(in >> 24);
			return out;
		}

		// convert to big endian
		static inline WORD htobs( const WORD in )
		{
			WORD out;
			((BYTE *)&out)[0] = (BYTE)(in >> 8);
			((BYTE *)&out)[1] = (BYTE)in;
			return out;
		}
		static inline DWORD htobl( const DWORD in )
		{
			DWORD out;
			((BYTE *)&out)[0] = (BYTE)(in >> 24);
			((BYTE *)&out)[1] = (BYTE)(in >> 16);
			((BYTE *)&out)[2] = (BYTE)(in >> 8);
			((BYTE *)&out)[3] = (BYTE)in;
			return out;
		}

		// convert from little endian
		static inline WORD ltohs( WORD in )
		{
			return (WORD)((const BYTE *)&in)[0] |
				(WORD)((const BYTE *)&in)[1] << 8;
		}
		static inline DWORD ltohl( const DWORD in )
		{
			return (DWORD)((const BYTE *)&in)[0] |
				(DWORD)((const BYTE *)&in)[1] << 8 |
				(DWORD)((const BYTE *)&in)[2] << 16 |
				(DWORD)((const BYTE *)&in)[3] << 24;
		}

		// convert from big endian
		static inline WORD btohs( WORD in )
		{
			return (WORD)((const BYTE *)&in)[0] << 8 |
				(WORD)((const BYTE *)&in)[1];
		}
		static inline DWORD btohl( const DWORD in )
		{
			return (DWORD)((const BYTE *)&in)[0] << 24 |
				(DWORD)((const BYTE *)&in)[1] << 16 |
				(DWORD)((const BYTE *)&in)[2] << 8 |
				(DWORD)((const BYTE *)&in)[3];
		}

	protected:
		/**
		* @brief               read data from memory buffer
		*
		* @param   dest        pointer to destination (i.e. where to put read data) (or NULL to skip data)
		* @param	dest_size   size of destination (i.e. number of bytes to read)
		* @param   buf         pointer to memory buffer (is advanced during read)
		* @param   buf_len     length of buffer (is reduced during read)
		* @return              if successful
		*/
		bool read_buf( void * dest, unsigned int dest_size, const unsigned char * & buf, unsigned int & buf_len );

		/**
		* @brief              write data to memory buffer
		*
		* @param   src        pointer to source (i.e. where to get data from) (or NULL to fill with 0)
		* @param	src_size   size of source (i.e. number of bytes to write)
		* @param   buf        pointer into memory buffer (is advanced during write)
		* @param   buf_len    length of free space in buffer (is reduced during write)
		* @return             if successful
		*/
		bool write_buf( void * src, unsigned int src_size, unsigned char * & buf, unsigned int & buf_len );

		double computeMuFromRow(unsigned int rowIndex, cv::Mat & img);
		double computeMuFromColumn(unsigned int columnIndex, cv::Mat & img);

	};
}

#endif

/******************************************************************************/
