/******************************************************************************
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 ******************************************************************************/

#ifndef NFIQ2_UI_EXCEPTION_H_
#define NFIQ2_UI_EXCEPTION_H_

#include <string>

namespace NFIQ2UI {

/**
 *  @brief
 *  The parent class of all NFIQ2UI exceptions.
 *
 *  @details
 *  The classes derived from this class will have a default
 *  information string set indicating the type of exception.
 *  Any additional information string is appended to that
 *  string.
 */
class Exception : std::exception {
    public:
	/**
	 *  Construct an Exception object without
	 *  an information string.
	 */
	Exception();

	/**
	 *  Construct an Exception object with
	 *  an information string.
	 *
	 *  @param[in] info
	 *  The information string associated
	 *  with the exception.
	 */
	/*
	 *  Pass info by value so we can use move
	 *  semantics when setting object state.
	 */
	Exception(std::string info);

	virtual ~Exception() = default;

	/**
	 *  Obtain the information string associated
	 *  with the exception.
	 *
	 *  @return
	 *  The information string as a char array.
	 */
	const char *what() const noexcept override;

    private:
	std::string _info;
};

/**
 *  @brief
 *  File error when opening, reading, writing, etc.
 */
class FileOpenError : public Exception {
    public:
	/**
	 *  Construct a FileOpenError object with
	 *  the default information string.
	 */
	FileOpenError();

	/**
	 *  Construct a FileOpenError object with
	 *  an information string appended to the
	 *  default information string.
	 */
	FileOpenError(const std::string &info);
};

/**
 *  @brief
 *  Undefined Flag was given to the command line.
 */
class UndefinedFlagError : public Exception {
    public:
	/**
	 *  Construct an UndefinedFlagError object with
	 *  the default information string.
	 */
	UndefinedFlagError();

	/**
	 *  Construct an UndefinedFlagError object with
	 *  an information string appended to the
	 *  default information string.
	 */
	UndefinedFlagError(const std::string &info);
};

/**
 *  @brief
 *  An invalid argument was given to the command line
 */
class InvalidArgumentError : public Exception {
    public:
	/**
	 *  Construct an UndefinedFlagError object with
	 *  the default information string.
	 */
	InvalidArgumentError();

	/**
	 *  Construct an UndefinedFlagError object with
	 *  an information string appended to the
	 *  default information string.
	 */
	InvalidArgumentError(const std::string &info);
};

/**
 *  @brief
 *  The model info file was unable to be parsed.
 */
class PropertyParseError : public Exception {
    public:
	/**
	 *  Construct an PropertyParseError object with
	 *  the default information string.
	 */
	PropertyParseError();

	/**
	 *  Construct an PropertyParseError object with
	 *  an information string appended to the
	 *  default information string.
	 */
	PropertyParseError(const std::string &info);
};

/**
 *  @brief
 *  A requested file was unable to be located on disk.
 */
class FileNotFoundError : public Exception {
    public:
	/**
	 *  Construct an FileNotFoundError object with
	 *  the default information string.
	 */
	FileNotFoundError();

	/**
	 *  Construct an FileNotFoundError object with
	 *  an information string appended to the
	 *  default information string.
	 */
	FileNotFoundError(const std::string &info);
};

/**
 *  @brief
 *  The machine learning model object could not be constructed correctly
 */
class ModelConstructionError : public Exception {
    public:
	/**
	 *  Construct an ModelConstructionError object with
	 *  the default information string.
	 */
	ModelConstructionError();

	/**
	 *  Construct an ModelConstructionError object with
	 *  an information string appended to the
	 *  default information string.
	 */
	ModelConstructionError(const std::string &info);
};

/**
 *  @brief
 *  The resampling operation failed
 */
class ResampleError : public Exception {
    public:
	/**
	 *  Construct an ResampleError object with
	 *  the default information string.
	 */
	ResampleError();

	/**
	 *  Construct an ResampleError object with
	 *  an information string appended to the
	 *  default information string if the errorHandled
	 *  boolean is flipped.
	 */
	ResampleError(const std::string &info, const bool errorHandled);

	/**
	 *  Obtain whether this exception was already handled or if it must
	 *  be handled by the function catching the exception
	 *
	 *  @return
	 *  A boolean indicating whether the error was handled
	 */
	bool errorWasHandled() const noexcept;

    private:
	bool _errorHandled { false };
};

} // namespace NFIQ2UI

#endif /* NFIQ2_UI_EXCEPTION_H_ */
