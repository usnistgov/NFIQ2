#ifndef NFIQ2ALGORITHMIMPL_H_
#define NFIQ2ALGORITHMIMPL_H_

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <list>

#include "include/NFIQ2Algorithm.h"
#include "include/NFIQException.h"
#include "include/FingerprintImageData.h"
#include "include/InterfaceDefinitions.h"

#include "RandomForestML.h"
#include "NFIQ2AlgorithmImpl.h"

namespace NFIQ
{
  /**
  ******************************************************************************
  * @class Impl
  * @brief This class serves as a wrapper to return quality scores for a
  * fingerprint image
  ******************************************************************************/
  class NFIQ2Algorithm::Impl
  {
    public:
      /******************************************************************************/
      // --- Constructor / Destructor --- //
      /******************************************************************************/

      /**
      * @brief Default constructor of Impl
      */
# ifdef EMBED_RANDOMFOREST_PARAMETERS
      Impl();
# endif
      Impl( const std::string& fileName, const std::string& fileHash );

      /**
      * @brief Destructor
      */
      virtual ~Impl();

      /******************************************************************************/
      // --- Public functions --- //
      /******************************************************************************/

      /**
      * @fn computeQualityScore
      * @brief Computes the quality score from the input fingerprint image data
      * @param rawImage fingerprint image in raw format
      * @param bComputeActionableQuality if to compute actionable quality flags or not
      * @param actionableQuality compute actionable quality values
      * @param bOutputFeatures if to output feature values
      * @param qualityfeatureData list of computed feature data values
      * @param bOutputSpeed if to output speed of computed features
      * @param qualityFeatureSpeed list of feature computation speed
      * @return achieved quality score
      */
      unsigned int computeQualityScore(
        NFIQ::FingerprintImageData rawImage,
        bool bComputeActionableQuality, std::list<NFIQ::ActionableQualityFeedback>& actionableQuality,
        bool bOutputFeatures, std::list<NFIQ::QualityFeatureData>& qualityFeatureData,
        bool bOutputSpeed, std::list<NFIQ::QualityFeatureSpeed>& qualityFeatureSpeed );

      /**
       * @brief
       * Obtain all actionable quality feedback identifiers.
       *
       * @return
       * Vector of strings containing all actionable quality feedback identifiers.
       */
      static std::vector<std::string> getAllActionableIdentifiers();
      
      /**
       * @brief
       * Obtain all quality feature IDs from quality modules.
       *
       * @return
       * Vector of strings containing all quality feature IDs.
       */
      static std::vector<std::string> getAllQualityFeatureIDs();

      /**
       * @brief
       * Obtain all speed feature groups from quality modules.
       *
       * @return
       * Vector of strings containing all speed feature groups.
       */
      static std::vector<std::string> getAllSpeedFeatureGroups();

      /**
       * @brief
       * Obtain MD5 checksum of Random Forest parameter file loaded.
       *
       * @return
       * MD5 checksum of the Random Forest parameter file loaded.
       */
      std::string
      getParameterHash()
      const;

    private:
      std::list<NFIQ::QualityFeatureData> computeQualityFeatures(
        const NFIQ::FingerprintImageData& rawImage,
        bool bComputeActionableQuality, std::list<NFIQ::ActionableQualityFeedback>& actionableQuality,
        bool bOutputSpeed, std::list<NFIQ::QualityFeatureSpeed>& speedValues );
      double getQualityPrediction( std::list<NFIQ::QualityFeatureData>& featureVector );

      RandomForestML m_RandomForestML;
      std::string m_parameterHash{};
  };
}

#endif

/******************************************************************************/
