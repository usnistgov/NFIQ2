#include <nfiq2_exception.hpp>

NFIQ2::NFIQException::NFIQException(const NFIQ2::ErrorCode &errorCode)
    : returnCode(errorCode)
    , errorMessage(errorCodeMessage.at(errorCode))
{

	if (errorMessage.compare("") == 0) {
		errorMessage = "Undefined return code";
	}
}

NFIQ2::NFIQException::NFIQException(
    const NFIQ2::ErrorCode &errorCode, const std::string &errorMessage)
    : returnCode(errorCode)
    , errorMessage(errorMessage)
{
}

NFIQ2::NFIQException::~NFIQException() noexcept
{
}

const char *
NFIQ2::NFIQException::what() const noexcept
{
	return errorMessage.c_str();
}
