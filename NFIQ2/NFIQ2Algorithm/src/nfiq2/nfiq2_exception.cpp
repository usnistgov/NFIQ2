#include <nfiq2_exception.hpp>

NFIQ2::NFIQException::NFIQException(const NFIQ2::ErrorCode &errorCode)
    : returnCode(errorCode)
    , errorMessage(errorCodeMessage.at(errorCode))
{

	if (this->errorMessage.compare("") == 0) {
		this->errorMessage = "Undefined return code";
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
	return this->errorMessage.c_str();
}

NFIQ2::ErrorCode
NFIQ2::NFIQException::getErrorCode() const
{
	return this->returnCode;
}

std::string
NFIQ2::NFIQException::getErrorMessage() const
{
	return this->errorMessage;
}
