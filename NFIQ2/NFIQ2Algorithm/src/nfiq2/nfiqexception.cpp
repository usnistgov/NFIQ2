#include <nfiq2_nfiqexception.hpp>

/**
 * Predefined error messages for the corresponding error codes
 */
const std::string c_ErrorMessages[] = {
	/*  0 */ "",
	/*  1 */ "Unknown error",
	/*  2 */ "Elective refusal to produce a proprietary template",
	/*  3 */ "",
	/*  4 */ "Elective refusal to process the input image",
	/*  5 */ "",
	/*  6 */ "Involuntary failure to extract",
	/*  7 */ "",
	/*  8 */ "Not enough memory",
	/*  9 */ "",
	/* 10 */ "Output type not supported",
	/* 11 */ "",
	/* 12 */ "Null pointer",
	/* 13 */ "",
	/* 14 */ "Bad arguments",
	/* 15 */ "",
	/* 16 */ "",
	/* 17 */ "",
	/* 18 */ "",
	/* 19 */ "",
	/* 20 */ "Utility calculation error",
	/* 21 */ "General fingerprint comparison error",
	/* 22 */ "Feature calculation error",
	/* 23 */ "Fingerprint image not available",
	/* 24 */ "Function not implemented",
	/* 25 */ "Cannot write to file",
	/* 26 */ "Cannot read from file",
	/* 27 */ "No data available",
	/* 28 */ "Module not found",
	/* 29 */ "No Input/Output Module has been selected",
	/* 30 */ "No Utility Estimation Module has been selected",
	/* 31 */ "No Machine Learning Module has been selected",
	/* 32 */ "Cannot parse XML",
	/* 33 */ "Cannot decode base64 string",
	/* 34 */ "Cannot compose XML",
	/* 35 */ "No database connection possible",
	/* 36 */ "Wrong file type",
	/* 37 */ "Wrong file content detected",
	/* 38 */ "Image conversion error",
	/* 39 */
	"Too few genuine scores remaining after iteration in utility "
	"computation",
	/* 40 */ "A fusion of utility values is not possible",
	/* 41 */ "An invalid configuration entry was found",
	/* 42 */ "An machine learning error occured",
	// dedicated FJFX feature computation errors
	/* 43 */ "Cannot create context for FJFX feature extractor",
	/* 44 */ "Cannot create feature set from fingerprint data",
	/* 45 */ "No feature set could be created",
	/* 46 */ "Cannot export/convert to ISO template format",
	/* 47 */ "The created ISO template is too small",
	/* 48 */ "Cannot allocate data for Finger Minutiae Record (FMR)",
	/* 49 */ "Cannot init biometric data block (BDB)",
	/* 50 */ "No minutiae records found",
	/* 51 */ "Cannot get minutiae records",
	/* 52 */ "No minutiae have been found (min count = 0)",
	/* 53 */ "Cannot get minutiae data",
	/* 54 */ "Invalid NFIQ2 Score",
	/* 55 */ "Invalid Image Size"
};

NFIQ2::NFIQException::NFIQException(uint32_t returnCode)
    : m_ReturnCode(returnCode)
{
	m_ErrorMessage = c_ErrorMessages[returnCode];
	if (m_ErrorMessage.compare("") == 0) {
		m_ErrorMessage = "Undefined return code";
	}
}

NFIQ2::NFIQException::NFIQException(
    uint32_t returnCode, std::string errorMessage)
    : m_ReturnCode(returnCode)
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
