#include <nfiq2_exception.hpp>
#include <nfiq2_modelinfo.hpp>

#include <algorithm>
#include <cctype>
#include <fstream>
#include <string>

static std::string trimWhitespace(const std::string &s);

const char NFIQ2::ModelInfo::ModelInfoKeyName[] { "Name" };
const char NFIQ2::ModelInfo::ModelInfoKeyTrainer[] { "Trainer" };
const char NFIQ2::ModelInfo::ModelInfoKeyDescription[] { "Description" };
const char NFIQ2::ModelInfo::ModelInfoKeyVersion[] { "Version" };
const char NFIQ2::ModelInfo::ModelInfoKeyPath[] { "Path" };
const char NFIQ2::ModelInfo::ModelInfoKeyHash[] { "Hash" };

NFIQ2::ModelInfo::ModelInfo() = default;

NFIQ2::ModelInfo::ModelInfo(const std::string &modelInfoFilePath)
{
	std::ifstream fp(modelInfoFilePath);
	std::string line;

	if (!fp) {
		throw NFIQ2::Exception(NFIQ2::ErrorCode::CannotReadFromFile,
		    "Failed to Read File: " + modelInfoFilePath);
	}

	while (std::getline(fp, line)) {
		const size_t eqPos = line.find('=');

		if (eqPos == std::string::npos) {
			continue;
		} else {
			const size_t llen = line.length();

			if (eqPos != 0 && eqPos < llen - 1) {
				const std::string start = trimWhitespace(
				    line.substr(0, eqPos));
				if (start.empty())
					continue;

				const std::string end = trimWhitespace(
				    line.substr(eqPos + 1, llen - 1));
				if (end.empty())
					continue;

				if (start == ModelInfo::ModelInfoKeyName) {
					this->modelName = end;
				} else if (start ==
				    ModelInfo::ModelInfoKeyTrainer) {
					this->modelTrainer = end;
				} else if (start ==
				    ModelInfo::ModelInfoKeyDescription) {
					this->modelDescription = end;
				} else if (start ==
				    ModelInfo::ModelInfoKeyVersion) {
					this->modelVersion = end;
				} else if (start ==
				    ModelInfo::ModelInfoKeyPath) {

					// Checks if the path to the model is
					// relative to the model info file and
					// not the cwd.
					if (((end.length() > 1) &&
						(end.front() == '/')) ||
					    ((end.length() > 2) &&
						(end.substr(0, 2) == "\\\\")) ||
					    ((end.length() > 3) &&
						(end.substr(1, 2) == ":\\")) ||
					    ((end.length() > 3) &&
						(end.substr(1, 2) == ":/"))) {
						this->modelPath = end;
					} else {
						const auto found =
						    modelInfoFilePath
							.find_last_of("/\\");

						if (found ==
						    std::string::npos) {
							this->modelPath = "./" +
							    end;
						} else {
							this->modelPath =
							    modelInfoFilePath
								.substr(
								    0, found) +
							    '/' + end;
						}
					}
				} else if (start ==
				    ModelInfo::ModelInfoKeyHash) {
					this->modelHash = end;
				}
			}
		}
	}
	fp.close();

	if (this->modelPath.empty()) {
		throw NFIQ2::Exception(NFIQ2::ErrorCode::NoDataAvailable,
		    "The required model information: " +
			std::string { ModelInfo::ModelInfoKeyPath } +
			" was not found.");
	}
	if (this->modelHash.empty()) {
		throw NFIQ2::Exception(NFIQ2::ErrorCode::NoDataAvailable,
		    "The required model information: " +
			std::string { ModelInfo::ModelInfoKeyHash } +
			" was not found.");
	}
}

std::string
NFIQ2::ModelInfo::getModelName() const
{
	return this->modelName;
}

std::string
NFIQ2::ModelInfo::getModelTrainer() const
{
	return this->modelTrainer;
}

std::string
NFIQ2::ModelInfo::getModelDescription() const
{
	return this->modelDescription;
}

std::string
NFIQ2::ModelInfo::getModelVersion() const
{
	return this->modelVersion;
}

std::string
NFIQ2::ModelInfo::getModelPath() const
{
	return this->modelPath;
}

std::string
NFIQ2::ModelInfo::getModelHash() const
{
	return this->modelHash;
}

std::string
trimWhitespace(const std::string &s)
{
	std::string output { s };

	/* Erase from beginning until the first non-whitespace */
	output.erase(output.begin(),
	    std::find_if(output.begin(), output.end(),
		[&](const char &c) -> bool { return (!std::isspace(c)); }));

	/* Erase from the last non-whitespace to the end */
	output.erase(
	    std::find_if(output.rbegin(), output.rend(),
		[&](const char &c) -> bool { return (!std::isspace(c)); })
		.base(),
	    output.end());

	return output;
}
