#ifndef NFIQ2_EXCEPTION_HPP_
#define NFIQ2_EXCEPTION_HPP_

#include <exception>
#include <map>
#include <string>

namespace NFIQ2 {
/**
 * This class represents error codes defined within this framework.
 */
enum class ErrorCode {
	UnknownError,
	NotEnoughMemory,
	BadArguments,
	FeatureCalculationError,
	CannotWriteToFile,
	CannotReadFromFile,
	NoDataAvailable,
	CannotDecodeBase64,
	InvalidConfiguration,
	MachineLearningError,
	// dedicated FJFX feature computation errors
	FJFX_CannotCreateContext,
	FJFX_CannotCreateFeatureSet,
	FJFX_NoFeatureSetCreated,
	// Scores must be between 0 and 100
	InvalidNFIQ2Score,
	InvalidImageSize
};

static const std::map<NFIQ2::ErrorCode, std::string> errorCodeMessage {
	{ NFIQ2::ErrorCode::UnknownError, "Unknown error" },
	{ NFIQ2::ErrorCode::NotEnoughMemory, "Not enough memory" },
	{ NFIQ2::ErrorCode::BadArguments, "Bad arguments" },
	{ NFIQ2::ErrorCode::FeatureCalculationError,
	    "Feature calculation error" },
	{ NFIQ2::ErrorCode::CannotWriteToFile, "Cannot write to file" },
	{ NFIQ2::ErrorCode::CannotReadFromFile, "Cannot read from file" },
	{ NFIQ2::ErrorCode::NoDataAvailable, "No data available" },
	{ NFIQ2::ErrorCode::CannotDecodeBase64, "Cannot decode base64 string" },
	{ NFIQ2::ErrorCode::InvalidConfiguration,
	    "An invalid configuration entry was found" },
	{ NFIQ2::ErrorCode::MachineLearningError,
	    "An machine learning error occured" },
	{ NFIQ2::ErrorCode::FJFX_CannotCreateContext,
	    "Cannot create context for FJFX feature extractor" },
	{ NFIQ2::ErrorCode::FJFX_CannotCreateFeatureSet,
	    "Cannot create feature set from fingerprint data" },
	{ NFIQ2::ErrorCode::FJFX_NoFeatureSetCreated,
	    "No feature set could be created" },
	{ NFIQ2::ErrorCode::InvalidNFIQ2Score, "Invalid NFIQ2 Score" },
	{ NFIQ2::ErrorCode::InvalidImageSize, "Invalid Image Size" }
};

/**
******************************************************************************
* @class NFIQException
* @brief Represents the exception class used within this framework.
******************************************************************************/
class NFIQException : public std::exception {
    public:
	/******************************************************************************/
	// --- Constructor / Destructor --- //
	/******************************************************************************/

	/**
	 * @brief Constructor which uses supplied error code and default message
	 */
	NFIQException(const NFIQ2::ErrorCode &errorCode);

	/**
	 * @brief Constructor which uses supplied error code and user-defined
	 * message
	 */
	NFIQException(
	    const NFIQ2::ErrorCode &errorCode, const std::string &errorMessage);

	/**
	 * @brief Destructo
	 */
	virtual ~NFIQException() noexcept;

	/******************************************************************************/
	// --- General Framework Functions --- //
	/******************************************************************************/

	/**
	 * @fn what
	 * @brief Inherited function from std::exception
	 * @return The exception message
	 */
	virtual const char *what() const noexcept;

	/**
	 * @fn getErrorCode
	 * @brief Returns the return code of the exception
	 * @return The return code
	 */
	NFIQ2::ErrorCode getErrorCode() const;

	/**
	 * @fn getErrorMessage
	 * @brief Returns the error message of the exception
	 * @return The error message
	 */
	std::string getErrorMessage() const;

    private:
	NFIQ2::ErrorCode errorCode; ///< The return code of the exception
	std::string errorMessage;   ///< The error message of the exception
};
} // namespace NFIQ

#endif

/******************************************************************************/
