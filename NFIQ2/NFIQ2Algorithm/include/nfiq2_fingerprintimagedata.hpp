#ifndef NFIQ2_FINGERPRINTIMAGEDATA_HPP_
#define NFIQ2_FINGERPRINTIMAGEDATA_HPP_

#include <nfiq2_data.hpp>

/**
 * Pixel intensity threshold used for determining whitespace
 * around fingerprint.
 */
#define MU_THRESHOLD 250

namespace NFIQ2 {

/** This typedef represents the resolution of an image (in dpi). */
typedef enum image_resolution_e {
	/** 500 dpi resolution. */
	e_ImageResolution_500dpi = 500,
	/** 1000 dpi resolution. */
	e_ImageResolution_1000dpi = 1000
} ImageResolution;

/** This class manages fingerprint image data (derived from class Data). */
class FingerprintImageData : public Data {
    public:
	/**
	 * @brief
	 * Default FingerprintImageData constructor.
	 *
	 * @note
	 * Canonically encoded as per ISO/IEC 19794-4:2005.
	 */
	FingerprintImageData();

	/**
	 * @brief
	 * Standard FingerprintImageData constructor.
	 *
	 * @note
	 * Canonically encoded as per ISO/IEC 19794-4:2005.
	 */
	FingerprintImageData(uint32_t imageWidth, uint32_t imageHeight,
	    uint8_t fingerCode, uint16_t imageDPI);

	/**
	 * @brief
	 * Constructor with available pointer to data.
	 *
	 * @note
	 * Canonically encoded as per ISO/IEC 19794-4:2005.
	 */
	FingerprintImageData(const uint8_t *pData, uint32_t dataSize,
	    uint32_t imageWidth, uint32_t imageHeight, uint8_t fingerCode,
	    uint16_t imageDPI);

	/** Copy constructor. */
	FingerprintImageData(const FingerprintImageData &otherData);

	/** Destructor. */
	virtual ~FingerprintImageData();

	/** Width of the fingerprint image (in pixels) */
	uint32_t m_ImageWidth;
	/** Height of the fingerprint image (in pixels) */
	uint32_t m_ImageHeight;
	/** ISO finger code of the fingerprint in the image */
	uint8_t m_FingerCode;
	/** Dots per inch of the fingerprint image (default: 500 dpi) */
	int16_t m_ImageDPI;

	/**
	 * @brief
	 * Uses a simple algorithm to remove white lines around the
	 * fingerprint image (internally represented in raw data format).
	 *
	 * @return
	 * The cropped/segmented fingerprint image in raw format.
	 *
	 * @throws NFIQException
	 */
	NFIQ2::FingerprintImageData removeWhiteFrameAroundFingerprint() const;
};
} // namespace NFIQ

#endif /* NFIQ2_FINGERPRINTIMAGEDATA_HPP_ */
