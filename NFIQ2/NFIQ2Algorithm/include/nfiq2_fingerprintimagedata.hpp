/*
 * This file is part of NIST Fingerprint Image Quality (NFIQ) 2. For more
 * information on this project, refer to:
 *   - https://nist.gov/services-resources/software/nfiq2
 *   - https://github.com/usnistgov/NFIQ2
 *
 * This work is in the public domain. For complete licensing details, refer to:
 *   - https://github.com/usnistgov/NFIQ2/blob/master/LICENSE.md
 */

#ifndef NFIQ2_FINGERPRINTIMAGEDATA_HPP_
#define NFIQ2_FINGERPRINTIMAGEDATA_HPP_

#include <nfiq2_data.hpp>

namespace NFIQ2 {

/**
 * Binary data representing a decompressed fingerprint image, canonically
 * encoded as per ISO/IEC 19794-4:2005.
 */
class FingerprintImageData : public Data {
    public:
	/** 500 PPI resolution. */
	static const uint16_t Resolution500PPI { 500 };

	/** Default constructor. */
	FingerprintImageData();

	/**
	 * @brief
	 * Constructor that does not store image data.
	 *
	 * @param width
	 * Width of the image in pixels.
	 * @param height
	 * Height of the image in pixels.
	 * @param fingerCode
	 * Finger position of the fingerprint in the image.
	 * @param ppi
	 * Resolution of the image in pixels per inch.
	 */
	FingerprintImageData(
	    uint32_t width, uint32_t height, uint8_t fingerCode, uint16_t ppi);

	/**
	 * @brief
	 * Constructor storing image data.
	 *
	 * @param pData
	 * Pointer to decompressed 8 bit-per-pixel grayscale image data,
	 * canonically encoded as per ISO/IEC 19794-4:2005.
	 * @param dataSize
	 * Size of the buffer pointed to by `pData`.
	 * @param width
	 * Width of the image in pixels.
	 * @param height
	 * Height of the image in pixels.
	 * @param fingerCode
	 * Finger position of the fingerprint in the image.
	 * @param ppi
	 * Resolution of the image in pixels per inch.
	 */
	FingerprintImageData(const uint8_t *pData, uint32_t dataSize,
	    uint32_t width, uint32_t height, uint8_t fingerCode, uint16_t ppi);

	/** Copy constructor. */
	FingerprintImageData(const FingerprintImageData &otherData);

	/** Destructor. */
	virtual ~FingerprintImageData();

	/** Width of the fingerprint image in pixels. */
	uint32_t width { 0 };
	/** Height of the fingerprint image in pixels. */
	uint32_t height { 0 };
	/** ISO finger code of the fingerprint in the image. */
	uint8_t fingerCode { 0 };
	/** Pixels per inch of the fingerprint image. */
	uint16_t ppi { Resolution500PPI };

	/**
	 * @brief
	 * Obtain a copy of the image with near-white lines surrounding the
	 * fingerprint removed.
	 *
	 * @return
	 * Cropped fingerprint image.
	 *
	 * @throws NFIQ2::Exception
	 * Error performing the crop, or the image is too small to be processed
	 * after cropping.
	 */
	NFIQ2::FingerprintImageData copyRemovingNearWhiteFrame() const;
};
} // namespace NFIQ

#endif /* NFIQ2_FINGERPRINTIMAGEDATA_HPP_ */
