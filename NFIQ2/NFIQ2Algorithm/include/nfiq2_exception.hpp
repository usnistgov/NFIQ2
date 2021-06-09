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
	 * Constructor that supplies a default error description.
	 *
	 * @param errorCode
	 * Code that broadly describes the type of error.
	 */
	Exception(const NFIQ2::ErrorCode errorCode);

	/**
	 * @brief
	 * Constructor that relies on a developer-provided error description.
	 *
	 * @param errorCode
	 * Code that broadly describes the type of error.
	 * @param errorMessage
	 * Description of what happened.
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
	 * Obtain the code that broadly describes the type of error.
	 *
	 * @return
	 * Code that broadly describes the type of error.
	 */
	NFIQ2::ErrorCode getErrorCode() const;

	/**
	 * @brief
	 * Obtain a description of what happened.
	 *
	 * @return
	 * Description of what happened.
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
	/** Code that broadly describes the type of error. */
	const NFIQ2::ErrorCode errorCode { ErrorCode::UnknownError };
	/** Description of what happened. */
	const std::string errorMessage {};
};
} // namespace NFIQ

#endif /* NFIQ2_EXCEPTION_HPP_ */
