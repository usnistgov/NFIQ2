#include <nfiq2_exception.hpp>

NFIQ2::Exception::Exception(const NFIQ2::ErrorCode &errorCode)
    : errorCode(errorCode)
    , errorMessage(errorCodeMessage.at(errorCode))
{
}

NFIQ2::Exception::Exception(
    const NFIQ2::ErrorCode &errorCode, const std::string &errorMessage)
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
