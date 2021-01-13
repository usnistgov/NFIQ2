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

NFIQ::ModelInfo::ModelInfo(const std::string& modelInfoFilePath)
{

	std::unordered_map<std::string, std::string> modelInfoMap {};
	try {
		modelInfoMap = parseModelInfoFile(modelInfoFilePath);
	} catch (...) {
		throw NFIQ::NFIQException(1, "Model Class Failed Construction");
	}

	this->modelName = modelInfoMap[ModelInfoKeyName];
	this->modelTrainer = modelInfoMap[ModelInfoKeyTrainer];
	this->modelDescription = modelInfoMap[ModelInfoKeyDescription];
	this->modelVersion = modelInfoMap[ModelInfoKeyVersion];
	this->modelPath = modelInfoMap[ModelInfoKeyPath];
	this->modelHash = modelInfoMap[ModelInfoKeyHash];
}

NFIQ::ModelInfo::~ModelInfo()
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

void
NFIQ::ModelInfo::setModelPath(const std::string& newPath) {
	this->modelPath = newPath;
}

std::unordered_map<std::string, std::string>
NFIQ::parseModelInfoFile(const std::string& modelInfoFilePath)
{
	std::unordered_map<std::string, std::string> modelInfoMap {};

	std::ifstream fp(modelInfoFilePath);
	std::string line;

	if (fp.is_open()) {

		while (std::getline(fp, line)) {

			const std::string start = line.substr(
			    0, line.find('=') - 1);
			const std::string end = line.substr(
			    line.find('=') + 2, line.size() - 1);

			modelInfoMap[start] = end;
		}

		fp.close();

	} else {
		throw NFIQ::NFIQException(
		    26, "Failed to Read File: " + modelInfoFilePath);
	}

	if (modelInfoMap.find(ModelInfo::ModelInfoKeyName) ==
		modelInfoMap.end() ||
	    modelInfoMap.find(ModelInfo::ModelInfoKeyTrainer) ==
		modelInfoMap.end() ||
	    modelInfoMap.find(ModelInfo::ModelInfoKeyDescription) ==
		modelInfoMap.end() ||
	    modelInfoMap.find(ModelInfo::ModelInfoKeyVersion) ==
		modelInfoMap.end() ||
	    modelInfoMap.find(ModelInfo::ModelInfoKeyPath) ==
		modelInfoMap.end() ||
	    modelInfoMap.find(ModelInfo::ModelInfoKeyHash) ==
		modelInfoMap.end()) {

		throw NFIQ::NFIQException(
		    1, "File missing required model info");
	}

	return modelInfoMap;
}
