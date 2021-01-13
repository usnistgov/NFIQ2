#ifndef NFIQ2_MODELINFO_H_
#define NFIQ2_MODELINFO_H_

#include <string>
#include <unordered_map>

namespace NFIQ {
/** Model Class containing Model Information */
class ModelInfo {
    public:
	ModelInfo(const std::string& modelInfoFilePath);
	~ModelInfo();

	/**
	 * @brief
	 * Computes the quality score from the input fingerprint image
	 * data.
	 *
	 * @return
	 * Achieved quality score
	 */
	std::string getModelName() const;

	/**
	 * @brief
	 * Computes the quality score from the input fingerprint image
	 * data.
	 *
	 * @return
	 * Achieved quality score
	 */
	std::string getModelTrainer() const;

	/**
	 * @brief
	 * Computes the quality score from the input fingerprint image
	 * data.
	 *
	 * @return
	 * Achieved quality score
	 */
	std::string getModelDescription() const;

	/**
	 * @brief
	 * Computes the quality score from the input fingerprint image
	 * data.
	 *
	 * @return
	 * Achieved quality score
	 */
	std::string getModelVersion() const;

	/**
	 * @brief
	 * Computes the quality score from the input fingerprint image
	 * data.
	 *
	 * @return
	 * Achieved quality score
	 */
	std::string getModelPath() const;

	/**
	 * @brief
	 * Computes the quality score from the input fingerprint image
	 * data.
	 *
	 * @return
	 * Achieved quality score
	 */
	std::string getModelHash() const;

	/**
	 * @brief
	 * Computes the quality score from the input fingerprint image
	 * data.
	 *
	 * @return
	 * Achieved quality score
	 */
	void setModelPath(const std::string& newPath);

	/** Static strings for Model Info File Keys */
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

/**
 * @brief
 * Computes the quality score from the input fingerprint image
 * data.
 *
 * @return
 * Achieved quality score
 */
std::unordered_map<std::string, std::string> parseModelInfoFile(
    const std::string& modelInfoFilePath);

} // namespace NFIQ

#endif
