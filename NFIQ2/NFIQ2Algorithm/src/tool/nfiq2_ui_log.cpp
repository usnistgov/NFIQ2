/******************************************************************************
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 ******************************************************************************/

#include <iomanip>
#include <iostream>
#include <string>

#include <NFIQ2Algorithm.h>

#include "nfiq2_ui_exception.h"
#include "nfiq2_ui_log.h"
#include "nfiq2_ui_types.h"

// Responsible for all print outputs
NFIQ2UI::Log::Log( const Flags& flags, const std::string& path )
{
  this->verbose = flags.verbose;
  this->debug = flags.debug;
  this->speed = flags.speed;

  if( path.empty() )
  {
    out = &std::cout;
  }
  else
  {
    this->logFile.open( path );
    if( !this->logFile )
    {
      throw NFIQ2UI::FileOpenError( "Logger could not open file: " + path );
    }
    // Set output to designated file
    this->out = &( this->logFile );
  }
}

// Prints the qualityScore of the Image
void NFIQ2UI::Log::printScore(
  const std::string& name, uint8_t fingerCode, unsigned int score,
  const std::string& errmsg, const bool quantized, const bool resampled,
  std::list<NFIQ::QualityFeatureData> featureVector,
  std::list<NFIQ::QualityFeatureSpeed> featureTimings ) const
{

  *( this->out ) << name << "," << std::to_string( fingerCode ) << "," << score
                 << "," << errmsg << "," << quantized << "," << resampled << ",";

  if( this->verbose )
  {
    for( const auto i : featureVector )
    {
      *( this->out ) << std::setprecision( 3 ) << i.featureDataDouble << ",";
    }
  }

  if( this->speed )
  {
    for( const auto i : featureTimings )
    {
      *( this->out ) << std::setprecision( 3 ) << i.featureSpeed << ",";
    }
  }
  *( this->out ) << "\n";
}

// Prints the quality score of a single image
void NFIQ2UI::Log::printSingle( unsigned int qualityScore ) const
{
  *( this->out ) << qualityScore << "\n";
}

// Prints output from Multi-threaded operations
void NFIQ2UI::Log::printThreaded( const std::string& message ) const
{
  *( this->out ) << message;
}

// Prints debug messages to stdout
void NFIQ2UI::Log::debugMsg( const std::string& message ) const
{
  if( this->debug )
  {
    *( this->out ) << "DEBUG 	| " << message << "\n";
  }
}

// Prints the CSV Header
void NFIQ2UI::Log::printCSVHeader() const
{
  *( this->out ) << "Filename"
                 << ","
                 << "FingerCode"
                 << ","
                 << "QualityScore"
                 << ","
                 << "OptionalError"
                 << ","
                 << "Quantized"
                 << ","
                 << "Re-sampled"
                 << ",";

  if( this->verbose )
  {
    *( this->out )
        << "FDA_Bin10_0,FDA_Bin10_1,FDA_Bin10_2,FDA_Bin10_3,FDA_Bin10_4,FDA_"
        "Bin10_5,FDA_Bin10_6,FDA_Bin10_7,FDA_Bin10_8,FDA_Bin10_9,FDA_Bin10_"
        "Mean,FDA_Bin10_StdDev,FingerJetFX_MinCount_COMMinRect200x200,"
        "FingerJetFX_MinutiaeCount,FJFXPos_Mu_MinutiaeQuality_2,FJFXPos_OCL_"
        "MinutiaeQuality_80,ImgProcROIArea_Mean,LCS_Bin10_0,LCS_Bin10_1,LCS_"
        "Bin10_2,LCS_Bin10_3,LCS_Bin10_4,LCS_Bin10_5,LCS_Bin10_6,LCS_Bin10_"
        "7,LCS_Bin10_8,LCS_Bin10_9,LCS_Bin10_Mean,LCS_Bin10_StdDev,MMB,Mu,"
        "OCL_Bin10_0,OCL_Bin10_1,OCL_Bin10_2,OCL_Bin10_3,OCL_Bin10_4,OCL_"
        "Bin10_5,OCL_Bin10_6,OCL_Bin10_7,OCL_Bin10_8,OCL_Bin10_9,OCL_Bin10_"
        "Mean,OCL_Bin10_StdDev,OF_Bin10_0,OF_Bin10_1,OF_Bin10_2,OF_Bin10_3,"
        "OF_Bin10_4,OF_Bin10_5,OF_Bin10_6,OF_Bin10_7,OF_Bin10_8,OF_Bin10_9,"
        "OF_Bin10_Mean,OF_Bin10_StdDev,OrientationMap_ROIFilter_"
        "CoherenceRel,OrientationMap_ROIFilter_CoherenceSum,RVUP_Bin10_0,"
        "RVUP_Bin10_1,RVUP_Bin10_2,RVUP_Bin10_3,RVUP_Bin10_4,RVUP_Bin10_5,"
        "RVUP_Bin10_6,RVUP_Bin10_7,RVUP_Bin10_8,RVUP_Bin10_9,RVUP_Bin10_"
        "Mean,RVUP_Bin10_StdDev,";
  }

  if( this->speed )
  {
    *( this->out )
        << "Contrast,Frequency domain,Minutiae,Minutiae quality,Region of "
        "interest,Local clarity,Orientation certainty,Orientation "
        "flow,Quality map,Ridge valley uniformity,";
  }
  *( this->out ) << "\n";
}

NFIQ2UI::Log::~Log()
{
  this->out = nullptr;
}

// -- Code will try to be used in the future
// when changes to NFIQ2 allow for headers to get
// collected before an image is processed --

/*
void NFIQ2UI::Log::printCSVHeader(
  std::list<NFIQ::QualityFeatureData> featureVector,
  std::list<NFIQ::QualityFeatureSpeed> featureTimings) {

 *(this->out) << "Filename" << ","
               << "QualityScore" << ","
               << "OptionalError" << ","
               << "Quantized" << ","
               << "Re-sampled" << ",";

  if (this->verboseFlag) {
    for (const auto i : featureVector){
      *(this->out) << i.featureID << ",";
    }
  }

  if (this->speedFlag) {
    for (const auto i : featureTimings){
      *(this->out) << i.featureIDGroup << ",";
    }
  }
  *(this->out)  << "\n";
}
*/
