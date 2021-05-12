#ifndef NFIQ2_EXCEPTION_HPP_
#define NFIQ2_EXCEPTION_HPP_

#include <exception>
#include <map>
#include <string>

namespace NFIQ2 {

/** Error codes used within the NFIQ2 namespace. */
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
	FJFX_CannotCreateContext,
	FJFX_CannotCreateFeatureSet,
	FJFX_NoFeatureSetCreated,
	InvalidNFIQ2Score,
	InvalidImageSize
};

/** Map of ErrorCode and their respective explanations. */
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
	    "An machine learning error occurred" },
	{ NFIQ2::ErrorCode::FJFX_CannotCreateContext,
	    "Cannot create context for FJFX feature extractor" },
	{ NFIQ2::ErrorCode::FJFX_CannotCreateFeatureSet,
	    "Cannot create feature set from fingerprint data" },
	{ NFIQ2::ErrorCode::FJFX_NoFeatureSetCreated,
	    "No feature set could be created" },
	{ NFIQ2::ErrorCode::InvalidNFIQ2Score, "Invalid NFIQ2 Score" },
	{ NFIQ2::ErrorCode::InvalidImageSize, "Invalid Image Size" }
};

/** Exceptions thrown from NFIQ2 functions. */
class Exception : public std::exception {
    public:
	/**
	 * @brief
	 * Constructor which uses supplied error code and default message.
	 *
	 * @param errorCode
	 * Error code for thrown exception.
	 */
	Exception(const NFIQ2::ErrorCode &errorCode);

	/**
	 * @brief
	 * Constructor which uses supplied error code and user-defined message.
	 *
	 * @param errorCode
	 * Error code for thrown exception.
	 *
	 * @param errorMessage
	 * User defined error message for thrown exception.
	 */
	Exception(
	    const NFIQ2::ErrorCode &errorCode, const std::string &errorMessage);

	/** Destructor. */
	virtual ~Exception() noexcept;

	/**
	 * @brief
	 * Extracts thrown exception error message string.
	 *
	 * @return
	 * The exception message.
	 */
	virtual const char *what() const noexcept;

	/**
	 * @brief
	 * Obtain the return code of the exception.
	 *
	 * @return
	 * The return code.
	 */
	NFIQ2::ErrorCode getErrorCode() const;

	/**
	 * @brief
	 * Obtain the error message of the exception.
	 *
	 * @return
	 * The error message.
	 */
	std::string getErrorMessage() const;

    private:
	/** Exception error type. */
	NFIQ2::ErrorCode errorCode;
	/** Error message string. */
	std::string errorMessage;
};
} // namespace NFIQ

#endif /* NFIQ2_EXCEPTION_HPP_ */
