#include <nfiq2_exception.hpp>

NFIQ2::NFIQException::NFIQException(const NFIQ2::ErrorCode &errorCode)
    : errorCode(errorCode)
    , errorMessage(errorCodeMessage.at(errorCode))
{
}

NFIQ2::NFIQException::NFIQException(
    const NFIQ2::ErrorCode &errorCode, const std::string &errorMessage)
    : errorCode(errorCode)
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
	return this->errorCode;
}

std::string
NFIQ2::NFIQException::getErrorMessage() const
{
	return this->errorMessage;
}
