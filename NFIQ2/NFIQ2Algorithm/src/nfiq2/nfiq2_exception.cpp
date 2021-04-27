#include <nfiq2_exception.hpp>

NFIQ2::NFIQException::NFIQException(const NFIQ2::ErrorCode &errorCode)
    : m_ReturnCode(static_cast<int>(errorCode))
    , m_ErrorMessage(errorCodeMessage.at(errorCode))
{

	if (m_ErrorMessage.compare("") == 0) {
		m_ErrorMessage = "Undefined return code";
	}
}

NFIQ2::NFIQException::NFIQException(
    const NFIQ2::ErrorCode &errorCode, const std::string &errorMessage)
    : m_ReturnCode(static_cast<int>(errorCode))
    , m_ErrorMessage(errorMessage)
{
}

NFIQ2::NFIQException::~NFIQException() noexcept
{
}

const char *
NFIQ2::NFIQException::what() const noexcept
{
	return m_ErrorMessage.c_str();
}
