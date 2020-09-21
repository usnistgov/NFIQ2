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
  const std::list<NFIQ::QualityFeatureData>& featureVector,
  const std::list<NFIQ::QualityFeatureSpeed>& featureTimings ) const
{

  *( this->out ) << name << "," << std::to_string( fingerCode ) << ","
                 << score << "," << errmsg << "," << quantized << ","
                 << resampled << ",";

  if( this->verbose )
  {
    for( const auto& i : featureVector )
    {
      *( this->out ) << std::setprecision( 5 ) << i.featureDataDouble << ",";
    }
  }

  if( this->speed )
  {
    for( const auto& i : featureTimings )
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
    std::vector<std::string> vHeaders =
      NFIQ::NFIQ2Algorithm::getAllQualityFeatureIDs();
    std::string qualityHeaders{};

    for( auto& i : vHeaders )
    {
      qualityHeaders = qualityHeaders.append( i ).append( "," );
    }

    if (this->speed == false) {
      qualityHeaders.pop_back();
    }

    *( this->out ) << qualityHeaders;
  }

  if( this->speed )
  {
    std::vector<std::string> qHeaders =
      NFIQ::NFIQ2Algorithm::getAllSpeedFeatureGroups();
    std::string speedHeaders{};

    for( auto& i : qHeaders )
    {
      std::replace( i.begin(), i.end(), ' ', '_' );
      speedHeaders = speedHeaders.append( i ).append( "," );
    }

    speedHeaders.pop_back();

    *( this->out ) << speedHeaders;
  }
  *( this->out ) << "\n";
}

NFIQ2UI::Log::~Log()
{
  this->out = nullptr;
}
