/******************************************************************************
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 ******************************************************************************/

#include <be_framework.h>
#include <be_io_utility.h>
#include <nfiq2_version.hpp>
#include <tool/nfiq2_ui_log.h>
#include <tool/nfiq2_ui_types.h>
#include <tool/nfiq2_ui_utils.h>

#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace BE = BiometricEvaluation;

std::string
NFIQ2UI::getBiomevalVersion()
{
	return (std::to_string(BE::Framework::getMajorVersion()) + "." +
	    std::to_string(BE::Framework::getMinorVersion()));
}

// Returns FileType Enum
NFIQ2UI::FileType
NFIQ2UI::getFileType(const BE::Memory::uint8Array &dataArray)
{
	NFIQ2UI::FileType type =
	    NFIQ2UI::FileType::Standard; // Default is a normal image
	if (dataArray.size() < 6) {
		type = NFIQ2UI::FileType::Unknown;
	} else if (NFIQ2UI::isAN2K(dataArray)) {
		type = NFIQ2UI::FileType::AN2K; // AN2K
	} else if (NFIQ2UI::isANSI2004()) {
		type = NFIQ2UI::FileType::ANSI2004; // ANSI2004
	}

	return type;
}

// Checks if a path is a AN2K file
bool
NFIQ2UI::isAN2K(const std::string &path)
{
	std::ifstream an2kFile(path, std::ios::in | std::ios::binary);

	if (an2kFile) {
		an2kFile.seekg(0, std::ios::end);
		const auto filesize = an2kFile.tellg();

		if (filesize >= 6) {
			an2kFile.seekg(0, std::ios::beg);
			unsigned char magic[6] = { 0 };
			an2kFile.read((char *)magic, sizeof(magic));

			// make it here with magic
			BE::Memory::uint8Array dataArray {};
			dataArray.copy(magic, sizeof(magic));

			return NFIQ2UI::isAN2K(dataArray);
		}
	}
	return false;
}

// Checks if a data-blob is a AN2K record
bool
NFIQ2UI::isAN2K(const BE::Memory::uint8Array &dataArray)
{
	// Hex representation of the string: "1.001:"
	// Each ANSI/NIST record begins with this string
	std::array<uint8_t, 6> bytes {};
	const std::array<uint8_t, 6> an2kBytes { 0x31, 0x2e, 0x30, 0x30, 0x31,
		0x3a };

	for (unsigned long i = 0; i < 6; i++) {
		bytes[i] = dataArray[static_cast<long>(i)];
	}
	bool result = false;

	if (memcmp(&bytes, &an2kBytes, bytes.size()) == 0) {
		result = true;
	}
	return result;
}

// Checks if a path is a RecordStore
bool
NFIQ2UI::isRecordStore(const std::string &path)
{
	if (BE::IO::Utility::pathIsDirectory(path)) {
		std::string rsPath = "/.rscontrol.prop";
		if (BE::IO::Utility::fileExists(path + rsPath)) {
			return true;
		}
	}
	return false;
}

// Gets content from batch files
std::tuple<std::vector<std::string>, int>
NFIQ2UI::getFileContent(const std::string &filename)
{
	std::ifstream batchOpen;
	std::vector<std::string> content {};
	batchOpen.exceptions(std::ifstream::badbit);
	int count = 0;

	try {
		batchOpen.open(filename);
		std::string line {};

		while (getline(batchOpen, line)) {
			count++;
			content.push_back(line);
		}
	} catch (const std::ifstream::failure &e) {
		std::cerr << "Error: Could not get line from Batch file"
			  << e.what() << "\n";
	}
	batchOpen.close();
	std::tuple<std::vector<std::string>, int> ret(content, count);
	return ret;
}

// Removes ending slash in path names
std::string
NFIQ2UI::removeSlash(std::string path)
{
	if (!path.empty() && (path[path.size() - 1] == '/')) {
		path.pop_back();
	}
	return path;
}

// Yes/No prompt
bool
NFIQ2UI::yesOrNo(const std::string &prompt, bool default_answer,
    bool show_options, bool allow_default_answer)
{
	std::string input;
	for (;;) {
		std::cout << prompt;
		if (show_options) {
			if (allow_default_answer) {
				if (default_answer == true) {
					std::cout << " ([Y]/n)";
				} else {
					std::cout << " (y/[N])";
				}
			} else {
				std::cout << " (y/n)";
			}
		}
		std::cout << ": ";
		std::getline(std::cin, input);
		try {
			switch (input.at(0)) {
			case 'Y':
			case 'y':
				return (true);
			case 'n':
			case 'N':
				return (false);
			}
		} catch (const std::out_of_range &) {
			if (allow_default_answer) {
				return (default_answer);
			}
		}
	}
}

// Checks the threads given and prompts user if the requested amount is higher
// than CPU cores/threads
unsigned int
NFIQ2UI::checkThreads(const std::string &threadArg)
{
	unsigned int n = std::thread::hardware_concurrency();
	unsigned int input = 1;

	try {
		input = static_cast<unsigned int>(std::stoul(threadArg));
	} catch (const std::invalid_argument &e) {
		std::cerr << e.what() << "\n";
		std::cerr << "Number not given to threading flag. Single "
			     "threading."
			  << "\n";
		return 1;
	} catch (const std::out_of_range &e) {
		std::cerr << e.what() << "\n";
		std::cerr << "Number given to threading flag was out of range. "
			     "Single "
			     "threading."
			  << "\n";
		return 1;
	}

	if (input > n) {
		const std::string prompt =
		    "The requested number of threads exceeds the number of "
		    "cores.\nYou "
		    "currently have " +
		    std::to_string(n) +
		    " CPU cores.\nWould you like to change the number of "
		    "threads to " +
		    std::to_string(n) + "?\n";

		const bool response = NFIQ2UI::yesOrNo(
		    prompt, false, true, true);

		if (response) {
			return n;
		} else {
			return input;
		}
	} else if (input < 1) {
		std::cerr << "Non positive value given to threading flag. "
			     "Single threading."
			  << "\n";
		return 1;
	} else {
		return input;
	}
}

std::string
NFIQ2UI::formatDouble(const double &d, const uint8_t precision)
{
	switch (std::fpclassify(d)) {
	case FP_NORMAL:
		break;
	case FP_ZERO:
		return "0";
	default:
		return std::to_string(d);
	}

	const std::string s = std::to_string(d);

	if (!std::isfinite(d)) {
		return s;
	}

	if (static_cast<long>(d) == d) {
		return std::to_string(static_cast<long>(d));
	}

	const std::string::size_type decimalPosition = s.find('.');

	if (decimalPosition == std::string::npos ||
	    precision >= (s.length() - decimalPosition - 1)) {
		return s;
	}

	return s.substr(0, decimalPosition + precision + 1);
}

// Usage of NFIQ2 tool
void
NFIQ2UI::printUsage()
{
	std::cout << "\n";
	std::cout << "                               Welcome to NFIQ2"
		  << "\n\n";
	std::cout
	    << "      Please provide individual files or directories to the command line."
	    << "\n";
	std::cout
	    << "            If you would like to use one of the flags listed below,"
	    << "\n"
	    << "              please include them before any standard arguments."
	    << "\n"
	    << "\n";

	std::cout << "Currently implemented flags and their functions: "
		  << "\n";
	std::cout
	    << "-i [file/directory path]: Processing provided Images and Directories"
	    << "\n";
	std::cout << "-f [batch file path]: Processing provided batch files"
		  << "\n";
	std::cout << "-o [file path]: Saving all output to a specified file"
		  << "\n";
	std::cout << "-j [# of threads]: Enables Multi-Threading for Batch and "
		     "RecordStore processes"
		  << "\n";
	std::cout << "-m [model info file]: Path to alternate model info file "
		  << "\n";
	std::cout
	    << "-a: Displays actionable quality scores about each processed image\n";
	std::cout
	    << "-v: Displays individual quality score information about each processed image"
	    << "\n";
	std::cout
	    << "-q: Displays individual quality speed timings about each processed image"
	    << "\n";
	std::cout
	    << "-d: Displays additional debug information as each score gets computed"
	    << "\n";
	std::cout
	    << "-F: Forces computation to occur. Tool does not prompt user "
	       "with yes/no options"
	    << "\n";
	std::cout << "-r: Recursive file scanning if a directory is provided"
		  << "\n";
	std::cout << "\nVersion Info\n------------\n"
		  << "Biometric Evaluation: " << NFIQ2UI::getBiomevalVersion()
		  << "\n"
		  << "FingerJet: " << NFIQ2::Version::FingerJet()
		  << "\n"
		     "OpenCV: "
		  << NFIQ2::Version::OpenCV
		  << "\n"
		     "NFIQ 2: "
		  << NFIQ2::Version::Pretty
		  << " (Date: " << NFIQ2::Version::BuildDate
		  << ", Commit: " << NFIQ2::Version::Commit << ")\n";
}

// Print to stdout if undefined flag is used
void
NFIQ2UI::printUndefinedFlag()
{
	std::cout << "\n";
	std::cout << "User Provided Undefined Flag or Defined Flag without "
		     "Required "
		     "Argument"
		  << "\n";
	std::cout << "Please Only Use Defined Flags and make sure to provide "
		     "required arguments"
		  << "\n";
	std::cout << "\n";
}

// WIP ////////////////////////////////////////////////////////////////////////
bool
NFIQ2UI::isANSI2004() // const BE::Memory::uint8Array &dataArray)
{
	return false;
}
