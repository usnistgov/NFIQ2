#ifndef NFIQ2_QUALITYMODULES_MODULE_H_
#define NFIQ2_QUALITYMODULES_MODULE_H_

#include <nfiq2_constants.hpp>
#include <nfiq2_fingerprintimagedata.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace NFIQ2 { namespace QualityMeasures {
class Algorithm {
    public:
	Algorithm();

	virtual ~Algorithm();

	/** @return Identifier for this particular feature */
	virtual std::string getName() const = 0;

	/** @return computed quality feature speed */
	virtual double getSpeed() const;

	/** @return computed quality features */
	virtual std::unordered_map<std::string, double> getFeatures() const;

    protected:
	void setSpeed(const double featureSpeed);

	void setFeatures(
	    const std::unordered_map<std::string, double> &featureResult);

    private:
	double speed {};

	std::unordered_map<std::string, double> features {};
};

}}

#endif /* NFIQ2_QUALITYMODULES_MODULE_H_ */

/******************************************************************************/
