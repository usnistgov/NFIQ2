#ifndef NFIQEXCEPTION_H
#define NFIQEXCEPTION_H

#include <exception>
#include <string>

namespace NFIQ {
/**
 * This type represents error codes defined within this framework.
 */
typedef enum error_code_e {
    e_Error_UnknownError = 1,
    e_Error_RefusalToProduceTemplate = 2,
    e_Error_RefusalToProcessInputImage = 4,
    e_Error_FailureToExtract = 6,
    e_Error_NotEnoughMemory = 8,
    e_Error_OutputTypeNotSupported = 10,
    e_Error_NullPointer = 12,
    e_Error_BadArguments = 14,
    e_Error_UtilityCalculationError = 20,
    e_Error_ComparisonError = 21,
    e_Error_FeatureCalculationError = 22,
    e_Error_ImageNotAvailable = 23,
    e_Error_FunctionNotImplemented = 24,
    e_Error_CannotWriteToFile = 25,
    e_Error_CannotReadFromFile = 26,
    e_Error_NoDataAvailable = 27,
    e_Error_ModuleNotFound = 28,
    e_Error_InputOutputModuleNotSelected = 29,
    e_Error_UtilityEstimationModuleNotSelected = 30,
    e_Error_MachineLearningModuleNotSelected = 31,
    e_Error_CannotParseXML = 32,
    e_Error_CannotDecodeBase64 = 33,
    e_Error_CannotComposeXML = 34,
    e_Error_NoDatabaseConnection = 35,
    e_Error_WrongFileType = 36,
    e_Error_WrongFileContent = 37,
    e_Error_ImageConversionError = 38,
    e_Error_TooFewGenuineScores = 39,
    e_Error_CannotFuseUtility = 40,
    e_Error_InvalidConfiguration = 41,
    e_Error_MachineLearningError = 42,
    // dedicated FJFX feature computation errors
    e_Error_FeatureCalculationError_FJFX_CannotCreateContext = 43,
    e_Error_FeatureCalculationError_FJFX_CannotCreateFeatureSet = 44,
    e_Error_FeatureCalculationError_FJFX_NoFeatureSetCreated = 45,
    e_Error_FeatureCalculationError_FJFX_CannotExportISO = 46,
    e_Error_FeatureCalculationError_FJFX_ISOTemplateTooSmall = 47,
    e_Error_FeatureCalculationError_FJFX_CannotAllocateFMR = 48,
    e_Error_FeatureCalculationError_FJFX_CannotInitBDB = 49,
    e_Error_FeatureCalculationError_FJFX_NoMinutiaeRecords = 50,
    e_Error_FeatureCalculationError_FJFX_CannotGetMinutiaeRecords = 51,
    e_Error_FeatureCalculationError_FJFX_NoMinutiaeFound = 52,
    e_Error_FeatureCalculationError_FJFX_CannotGetMinutiaeData = 53
} ErrorCode;

/**
******************************************************************************
* @class NFIQException
* @brief Represents the exception class used within this framework.
******************************************************************************/
class NFIQException : public std::exception {
   public:
    /******************************************************************************/
    // --- Constructor / Destructor --- //
    /******************************************************************************/

    /**
     * @brief Constructor which uses supplied error code and default message
     */
    explicit NFIQException(uint32_t returnCode);

    /**
     * @brief Constructor which uses supplied error code and user-defined
     * message
     */
    NFIQException(uint32_t returnCode, std::string errorMessage);

    /**
     * @brief Destructo
     */
    virtual ~NFIQException() noexcept;

    /******************************************************************************/
    // --- General Framework Functions --- //
    /******************************************************************************/

    /**
     * @fn what
     * @brief Inherited function from std::exception
     * @return The exception message
     */
    virtual const char*
    what() const noexcept;

    /**
     * @fn getReturnCode
     * @brief Returns the return code of the exception
     * @return The return code
     */
    uint32_t
    getReturnCode() const {
        return m_ReturnCode;
    }

    /**
     * @fn getErrorMessage
     * @brief Returns the error message of the exception
     * @return The error message
     */
    std::string
    getErrorMessage() const {
        return m_ErrorMessage;
    }

   private:
    uint32_t m_ReturnCode;       ///< The return code of the exception
    std::string m_ErrorMessage;  ///< The error message of the exception
};
}  // namespace NFIQ

#endif

/******************************************************************************/
