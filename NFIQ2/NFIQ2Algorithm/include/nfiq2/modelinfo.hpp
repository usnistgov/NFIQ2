#ifndef NFIQ2_modelinfo_H_
#define NFIQ2_modelinfo_H_

#include <string>
#include <unordered_map>

namespace NFIQ {
/** Model Class containing Model Information */
class ModelInfo {
    public:
	ModelInfo(const std::string modelInfoFilePath);
	~ModelInfo();

	std::string getModelName();
	std::string getModelTrainer();
	std::string getModelDescription();
	std::string getModelVersion();
	std::string getModelPath();
	std::string getModelHash();

	static const std::string ModelInfoKeyName;
	static const std::string ModelInfoKeyTrainer;
	static const std::string ModelInfoKeyDescription;
	static const std::string ModelInfoKeyVersion;
	static const std::string ModelInfoKeyPath;
	static const std::string ModelInfoKeyHash;

    private:
	std::string modelName;
	std::string modelTrainer;
	std::string modelDescription;
	std::string modelVersion;
	std::string modelPath;
	std::string modelHash;

	ModelInfo(const ModelInfo &) = delete;
	ModelInfo &operator=(const ModelInfo &) = delete;
};

std::unordered_map<std::string, std::string> parseModelInfoFile(
    const std::string modelInfoFilePath);

} // namespace NFIQ

#endif
