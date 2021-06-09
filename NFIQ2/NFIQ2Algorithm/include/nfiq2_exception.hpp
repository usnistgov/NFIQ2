/*
 * This file is part of NIST Fingerprint Image Quality (NFIQ) 2. For more
 * information on this project, refer to:
 *   - https://nist.gov/services-resources/software/nfiq2
 *   - https://github.com/usnistgov/NFIQ2
 *
 * This work is in the public domain. For complete licensing details, refer to:
 *   - https://github.com/usnistgov/NFIQ2/blob/master/LICENSE.md
 */

#ifndef NFIQ2_EXCEPTION_HPP_
#define NFIQ2_EXCEPTION_HPP_

#include <exception>
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
	Exception(const NFIQ2::ErrorCode errorCode);

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
	    const NFIQ2::ErrorCode errorCode, const std::string &errorMessage);

	/** Destructor. */
	virtual ~Exception() noexcept;

	/**
	 * @brief
	 * Extracts thrown exception error message string.
	 *
	 * @return
	 * The exception message.
	 */
	virtual const char *what() const noexcept override;

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

	/**
	 * @brief
	 * Obtain a default error message for a given ErrorCode.
	 *
	 * @param errorCode
	 * Error code from ErrorCode
	 *
	 * @return
	 * String containing default description of `errorCode`.
	 */
	static std::string defaultErrorMessage(
	    const NFIQ2::ErrorCode errorCode);

    private:
	/** Exception error type. */
	const NFIQ2::ErrorCode errorCode { ErrorCode::UnknownError };
	/** Error message string. */
	const std::string errorMessage {};
};
} // namespace NFIQ

#endif /* NFIQ2_EXCEPTION_HPP_ */
