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

/** Full version information. */
extern const std::string Full;
/** Pretty version information. */
extern const std::string Pretty;
/** Build version status. */
extern const std::string Status;
/** Current build date. */
extern const std::string BuildDate;
/** Current build commit. */
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
