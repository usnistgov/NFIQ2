/******************************************************************************
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 ******************************************************************************/

#include <tool/nfiq2_ui_exception.h>

#include <string>

NFIQ2UI::Exception::Exception()
    : exception()
{
}

NFIQ2UI::Exception::Exception(std::string info)
    : _info { std::move(info) }
{
}

const char *
NFIQ2UI::Exception::what() const noexcept
{
	return (this->_info.c_str());
}

NFIQ2UI::FileOpenError::FileOpenError(const std::string &info)
    : Exception("FileOpenError: " + info)
{
}

NFIQ2UI::UndefinedFlagError::UndefinedFlagError(const std::string &info)
    : Exception("UndefinedFlagError: " + info)
{
}

NFIQ2UI::InvalidArgumentError::InvalidArgumentError(const std::string &info)
    : Exception("InvalidArgumentError: " + info)
{
}

NFIQ2UI::PropertyParseError::PropertyParseError(const std::string &info)
    : Exception("PropertyParseError: " + info)
{
}

NFIQ2UI::FileNotFoundError::FileNotFoundError(const std::string &info)
    : Exception("FileNotFoundError: " + info)
{
}

NFIQ2UI::ModelConstructionError::ModelConstructionError(const std::string &info)
    : Exception("ModelConstructionError: " + info)
{
}

NFIQ2UI::ResampleError::ResampleError(
    const std::string &info, const bool errorHandled = true)
    : Exception("ResampleError: " + info)
{
	this->_errorHandled = errorHandled;
}

bool
NFIQ2UI::ResampleError::errorWasHandled() const noexcept
{
	return this->_errorHandled;
}
