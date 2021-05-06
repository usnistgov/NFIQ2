#ifndef NFIQ2_VERSION_HPP_
#define NFIQ2_VERSION_HPP_

#include <string>

namespace NFIQ2 { namespace Version {

/** Major version number. */
extern const unsigned int Major;
/** Minor version number. */
extern const unsigned int Minor;
/** Patch version number. */
extern const unsigned int Patch;

/** Full version information, including build date and git SHA. */
extern const std::string Full;
/** Version information formatted in a way suitable for printing. */
extern const std::string Pretty;
/** Build version status (e.g., pre-release, beta, etc.). */
extern const std::string Status;
/** Current build date (%Y%m%d%H%M). */
extern const std::string BuildDate;
/** Current build commit (git short SHA). */
extern const std::string Commit;

/*
 * Third-Party Components.
 */
/** OpenCV version. */
extern const std::string OpenCV;
/** FingerJet version. */
std::string FingerJet();

} // namespace Version
} // namespace NFIQ

#endif /* NFIQ2_VERSION_H_ */
