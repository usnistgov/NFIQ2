#include <nfiq2_data.hpp>

#include <iomanip>
#include <sstream>

NFIQ2::Data::Data() = default;

NFIQ2::Data::Data(const uint8_t *pData, uint32_t dataSize)
    : std::basic_string<uint8_t>(pData, dataSize)
{
}

NFIQ2::Data::Data(const Data &otherData)
    : std::basic_string<uint8_t>(otherData)
{
}

NFIQ2::Data::Data(const std::basic_string<uint8_t> &otherData)
    : std::basic_string<uint8_t>(otherData)
{
}

NFIQ2::Data::~Data() = default;

void
NFIQ2::Data::writeToFile(const std::string &filename) const
{
	bool success = false;
	if (!filename.empty()) {
		std::ofstream f;
		f.open(filename.c_str(), std::ios::out | std::ios::binary);
		if (f.is_open()) {
			f.write((char *)this->data(),
			    static_cast<std::streamsize>(this->size()));
			success = !f.bad(); // badbit is set if write was
					    // incomplete or failed
			f.close();
		} else {
			throw NFIQ2::Exception(
			    NFIQ2::ErrorCode::CannotWriteToFile);
		}
	}
	if (!success) {
		throw NFIQ2::Exception(NFIQ2::ErrorCode::CannotWriteToFile);
	}
}

void
NFIQ2::Data::readFromFile(const std::string &filename)
{
	bool success = false;
	if (!filename.empty()) {
		unsigned long len;
		unsigned char *pBuffer;
		std::ifstream f;
		f.open(filename.c_str(), std::ios::binary | std::ios::in);
		if (f.is_open()) {
			// detecting size
			f.seekg(0, std::ios::end);
			len = (unsigned long)f.tellg();
			f.seekg(0, std::ios::beg);
			if (len > 0) {
				pBuffer = new unsigned char[len];
				f.read((char *)pBuffer, len);
				this->assign(pBuffer, len);
				delete[] pBuffer;
				success = !f.fail(); // failbit is set if read
						     // was incomplete or failed
				f.close();
			} else {
				// must be zero since type is unsigned
				this->clear();
				// empty file is no error
				success = true;
			}
		} else {
			throw NFIQ2::Exception(
			    NFIQ2::ErrorCode::CannotReadFromFile);
		}
	}
	if (!success) {
		throw NFIQ2::Exception(NFIQ2::ErrorCode::CannotReadFromFile);
	}
}

std::string
NFIQ2::Data::toHexString() const
{
	if (this->size() <= 0) {
		throw NFIQ2::Exception(NFIQ2::ErrorCode::NoDataAvailable);
	}

	std::stringstream ss;
	ss << std::setfill('0') << std::right;

	for (unsigned long nPos = 0; nPos < this->size(); nPos++) {
		if (nPos > 0) {
			ss << " "; // separator
		}

		ss << std::setw(2) << std::uppercase << std::hex
		   << (unsigned int)this->at(nPos);
	}
	return ss.str();
}

void
NFIQ2::Data::fromBase64String(const std::string &base64String)
{
	const char *ptr = base64String.data();
	size_t len = base64String.size();

	this->clear();

	bool ok = true, end = false;
	int bits = 0, data = 0;
	while (len > 0) {
		// decode next char
		int c = -1;
		if (*ptr >= 'A' && *ptr <= 'Z') {
			c = *ptr - 'A';
		} else if (*ptr >= 'a' && *ptr <= 'z') {
			c = *ptr - 'a' + 26;
		} else if (*ptr >= '0' && *ptr <= '9') {
			c = *ptr - '0' + 52;
		} else if (*ptr == '+') {
			c = 62;
		} else if (*ptr == '/') {
			c = 63;
		} else if (*ptr == '=') {
			end = true; // equal sign is allowed at end
		} else if (*ptr == ' ' || *ptr == '\t' || *ptr == '\r' ||
		    *ptr == '\n') {
			// whitespace is allowed and ignored
		} else {
			ok = false;
		}

		ptr++;
		len--;
		if (c >= 0 && end) // data after end is not allowed
		{
			ok = false;
		}

		// collect bits
		if (c >= 0) {
			data = data << 6 | c;
			bits += 6;
			// extract full characters
			if (bits >= 8) {
				bits -= 8;
				*this += (unsigned char)(data >> bits);
			}
		}
	}

	if (!ok) {
		throw NFIQ2::Exception(NFIQ2::ErrorCode::CannotDecodeBase64);
	}
}

std::string
NFIQ2::Data::toBase64String() const
{
	const char base64Lookup[65] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
		'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U',
		'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
		'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u',
		'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', '+', '/', '=' };

	unsigned int size = static_cast<unsigned int>(this->size());
	const unsigned char *input = this->data();

	// calculate number of blocks (each block has 4 characters)
	unsigned int numberOfBlocks = size % 3 == 0 ? size / 3 : size / 3 + 1;
	// allocate output buffer
	char *output = new char[numberOfBlocks * 4 + 1];

	unsigned int inputOffset = 0;
	unsigned int outputOffset = 0;
	for (unsigned int rounds = 0; rounds < numberOfBlocks; rounds++) {
		unsigned int nrBytes;
		unsigned int i;
		if (rounds < numberOfBlocks - 1) {
			nrBytes = 3;
		} else {
			nrBytes = size % 3 == 0 ? 3 : size % 3;
		}

		// 3 input bytes
		unsigned char ic[3];
		for (i = 0; i < 3; i++) {
			if (i < nrBytes) {
				ic[i] = input[inputOffset++];
			} else {
				ic[i] = 0;
			}
		}
		// transform bytes 1...3 into 4 chars
		int sum = 256 * (256 * ic[0] + ic[1]) + ic[2];
		unsigned char oc[4];
		oc[3] = sum % 64;
		sum = sum / 64;
		oc[2] = sum % 64;
		sum = sum / 64;
		oc[1] = sum % 64;
		sum = sum / 64;
		oc[0] = sum % 64;
		// insert padding, if neccessary
		if (nrBytes == 2) {
			oc[3] = 64; //'='
		} else if (nrBytes == 1) {
			oc[3] = 64; //'='
			oc[2] = 64; //'='
		}
		// write 4 bytes into output buffer
		for (i = 0; i < 4; i++) {
			output[outputOffset++] = base64Lookup[oc[i]];
		}
	}
	// assign output buffer to std::string and delete it afterwards
	output[numberOfBlocks * 4] = '\0';
	std::string returnStr = output;
	delete[] output;

	return returnStr;
}
