#ifndef NFIQ2_MODELINFO_H_
#define NFIQ2_MODELINFO_H_

#include <string>
#include <unordered_map>

namespace NFIQ {
/** Model Class containing Model Information */
class ModelInfo {
    public:
	ModelInfo();
	ModelInfo(std::unordered_map<std::string, std::string> modelInfoMap);

	static const std::string ModelInfoKeyName;
	static const std::string ModelInfoKeyTrainer;
	static const std::string ModelInfoKeyDescription;
	static const std::string ModelInfoKeyVersion;
	static const std::string ModelInfoKeyPath;
	static const std::string ModelInfoKeyHash;

	const std::string modelName;
	const std::string modelTrainer;
	const std::string modelDescription;
	const std::string modelVersion;
	const std::string modelPath;
	const std::string modelHash;
};

/**
 * @brief
 * Generates a Key/Value Pair structure containing information from the model
 * info text file
 *
 * @param modelInfoFilePath
 * Path to model info text file
 *
 * @return
 * Map of values parsed from model info text file
 */
std::unordered_map<std::string, std::string> parseModelInfoFile(
    const std::string &modelInfoFilePath);

/**
 * @brief
 * Checks to see if the path to the model is relative to the model info file
 * and not the cwd. Updates the path in the map accordingly.
 *
 * @param modelInfoMap
 * A reference to the map containing information parsed from the info file.
 *
 * @param modelInfoFilePath
 * Path to model info text file
 */
void checkModelPath(std::unordered_map<std::string, std::string> &modelInfoMap,
    const std::string &modelInfoFilePath);

} // namespace NFIQ

#endif
