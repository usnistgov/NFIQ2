#include <nfiq2_exception.hpp>

#include <map>
#include <stdexcept>

NFIQ2::Exception::Exception(const NFIQ2::ErrorCode errorCode)
    : errorCode { errorCode }
    , errorMessage { defaultErrorMessage(errorCode) }
{
}

NFIQ2::Exception::Exception(
    const NFIQ2::ErrorCode errorCode, const std::string &errorMessage)
    : errorCode(errorCode)
    , errorMessage(errorMessage)
{
}

NFIQ2::Exception::~Exception() noexcept = default;

const char *
NFIQ2::Exception::what() const noexcept
{
	return this->errorMessage.c_str();
}

NFIQ2::ErrorCode
NFIQ2::Exception::getErrorCode() const
{
	return this->errorCode;
}

std::string
NFIQ2::Exception::getErrorMessage() const
{
	return this->errorMessage;
}

std::string
NFIQ2::Exception::defaultErrorMessage(const NFIQ2::ErrorCode errorCode)
{
	/* Map of ErrorCode and their respective explanations. */
	static const std::map<NFIQ2::ErrorCode, std::string> errorCodeMessage {
		{ NFIQ2::ErrorCode::UnknownError, "Unknown error" },
		{ NFIQ2::ErrorCode::NotEnoughMemory, "Not enough memory" },
		{ NFIQ2::ErrorCode::BadArguments, "Bad arguments" },
		{ NFIQ2::ErrorCode::FeatureCalculationError,
		    "Feature calculation error" },
		{ NFIQ2::ErrorCode::CannotWriteToFile, "Cannot write to file" },
		{ NFIQ2::ErrorCode::CannotReadFromFile,
		    "Cannot read from file" },
		{ NFIQ2::ErrorCode::NoDataAvailable, "No data available" },
		{ NFIQ2::ErrorCode::CannotDecodeBase64,
		    "Cannot decode base64 string" },
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

	const auto message = errorCodeMessage.find(errorCode);
	if (message == errorCodeMessage.end()) {
		try {
			return (errorCodeMessage.at(ErrorCode::UnknownError));
		} catch (const std::out_of_range &) {
			return {};
		}
	} else
		return (message->second);
}
