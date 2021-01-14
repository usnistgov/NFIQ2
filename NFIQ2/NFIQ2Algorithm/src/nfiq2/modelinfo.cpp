#include <nfiq2/modelinfo.hpp>
#include <nfiq2/nfiqexception.hpp>

#include <fstream>
#include <iostream>
#include <string>

const std::string NFIQ::ModelInfo::ModelInfoKeyName = "Name";
const std::string NFIQ::ModelInfo::ModelInfoKeyTrainer = "Trainer";
const std::string NFIQ::ModelInfo::ModelInfoKeyDescription = "Description";
const std::string NFIQ::ModelInfo::ModelInfoKeyVersion = "Version";
const std::string NFIQ::ModelInfo::ModelInfoKeyPath = "Path";
const std::string NFIQ::ModelInfo::ModelInfoKeyHash = "Hash";

NFIQ::ModelInfo::ModelInfo()
{
}

NFIQ::ModelInfo::ModelInfo(
    std::unordered_map<std::string, std::string> modelInfoMap)
    : modelName { modelInfoMap[ModelInfoKeyName] }
    , modelTrainer { modelInfoMap[ModelInfoKeyTrainer] }
    , modelDescription { modelInfoMap[ModelInfoKeyDescription] }
    , modelVersion { modelInfoMap[ModelInfoKeyVersion] }
    , modelPath { modelInfoMap[ModelInfoKeyPath] }
    , modelHash { modelInfoMap[ModelInfoKeyHash] }
{
}

std::string
NFIQ::ModelInfo::getModelName() const
{
	return this->modelName;
}

std::string
NFIQ::ModelInfo::getModelTrainer() const
{
	return this->modelTrainer;
}

std::string
NFIQ::ModelInfo::getModelDescription() const
{
	return this->modelDescription;
}

std::string
NFIQ::ModelInfo::getModelVersion() const
{
	return this->modelVersion;
}

std::string
NFIQ::ModelInfo::getModelPath() const
{
	return this->modelPath;
}

std::string
NFIQ::ModelInfo::getModelHash() const
{
	return this->modelHash;
}

std::unordered_map<std::string, std::string>
NFIQ::parseModelInfoFile(const std::string &modelInfoFilePath)
{
	std::unordered_map<std::string, std::string> modelInfoMap {};

	std::ifstream fp(modelInfoFilePath);
	std::string line;

	if (!fp) {
		throw NFIQ::NFIQException(e_Error_CannotReadFromFile,
		    "Failed to Read File: " + modelInfoFilePath);
	}

	while (std::getline(fp, line)) {
		const int eqPos = line.find('=');

		if (eqPos == std::string::npos) {
			continue;
		} else {
			const unsigned int llen = line.length();

			if (eqPos != 0 && eqPos < llen - 2) {
				const std::string start = line.substr(
				    0, eqPos - 1);
				const std::string end = line.substr(
				    eqPos + 2, llen - 1);
				modelInfoMap[start] = end;
			}
		}
	}
	fp.close();

	if (modelInfoMap.find(ModelInfo::ModelInfoKeyPath) ==
	    modelInfoMap.end()) {
		throw NFIQ::NFIQException(e_Error_NoDataAvailable,
		    "The required model information: " +
			ModelInfo::ModelInfoKeyPath + " was not found.");
	}
	if (modelInfoMap.find(ModelInfo::ModelInfoKeyHash) ==
	    modelInfoMap.end()) {
		throw NFIQ::NFIQException(e_Error_NoDataAvailable,
		    "The required model information: " +
			ModelInfo::ModelInfoKeyPath + " was not found.");
	}

	return modelInfoMap;
}

void
NFIQ::checkModelPath(std::unordered_map<std::string, std::string> &modelInfoMap,
    const std::string &modelInfoFilePath)
{
	const std::string originalPath =
	    modelInfoMap[ModelInfo::ModelInfoKeyPath];

	if ((originalPath.front() != '/') &&
	    ((originalPath.length() > 2) &&
		(originalPath.substr(0, 2) != "\\\\")) &&
	    ((originalPath.length() > 3) &&
		(originalPath.substr(1, 2) != ":\\")) &&
	    ((originalPath.length() > 3) &&
		(originalPath.substr(1, 2) != ":/"))) {

		std::size_t found = modelInfoFilePath.find_last_of("/\\");

		modelInfoMap[ModelInfo::ModelInfoKeyPath] =
		    modelInfoFilePath.substr(0, found) + '/' + originalPath;
	}
}
