/*
    FingerJetFX OSE -- Fingerprint Feature Extractor, Open Source Edition

    Copyright (c) 2011 by DigitalPersona, Inc. All rights reserved.

    DigitalPersona, FingerJet, and FingerJetFX are registered trademarks 
    or trademarks of DigitalPersona, Inc. in the United States and other
    countries.

    FingerJetFX OSE is open source software that you may modify and/or
    redistribute under the terms of the GNU Lesser General Public License
    as published by the Free Software Foundation, either version 3 of the 
    License, or (at your option) any later version, provided that the 
    conditions specified in the COPYRIGHT.txt file provided with this 
    software are met.
 
    For more information, please visit digitalpersona.com/fingerjetfx.
*/ 
/*
      LIBRARY: FRFXLL - Fingerprint Feature Extractor - Low Level API

      ALGORITHM:      Alexander Ivanisov
                      Yi Chen
                      Salil Prabhakar
      IMPLEMENTATION: Alexander Ivanisov
                      Jacob Kaminsky
                      Lixin Wei
      DATE:           11/08/2011
*/

#ifndef __DESERIALIZEFPDATA_H
#define __DESERIALIZEFPDATA_H

#include <algorithm> // sort
#include "member_comp.h"
#include "dpfr.h"
#include "dpError.h"
#include "matchData.h"
#include "intmath.h"

typedef struct tag_FRFXLL_INPUT_PARAM_ISO_ANSI
{
  size_t length;      // size of this structure, for extensibility
  unsigned int    CBEFF;
  unsigned short  fingerPosition;
  unsigned short  viewNumber;
  unsigned char   rotation;   // <rotation in degrees clockwise> * 128 / 180, keep in mind rounding 
} FRFXLL_INPUT_PARAM_ISO_ANSI;

namespace FingerJetFxOSE {
  namespace FpRecEngineImpl {
    namespace Embedded {
      const FRFXLL_RESULT ErrorBadFingerprintData = FRFXLL_ERR_INVALID_DATA;
      const FRFXLL_RESULT ErrorBadAlignmentData = FRFXLL_ERR_INVALID_DATA;
      const FRFXLL_RESULT ErrorAlignmentDataDisqualified = FRFXLL_ERR_INVALID_DATA;

      inline bool IsSystemIsBigEndian() {
        int one = 1;
        return !(*((char *)(&one)));
      };

      struct Reader {
        const unsigned char * start;
        const unsigned char * end;
        const unsigned char * cur;
        bool  dataInBigEndian;
        FRFXLL_RESULT rc;

        Reader(const unsigned char data[], size_t size, bool dataInBigEndian_ = false) 
          : start(data)
          , end(data + size)
          , cur(data)
          , dataInBigEndian(dataInBigEndian_)
          , rc(FRFXLL_OK) 
        {
        };
        virtual int getch() { 
          if (eof()) {
            SetError(FRFXLL_ERR_MORE_DATA);
            return -1;
          } else {
            return *(cur++); 
          }
        }
        virtual bool eof() const { return cur >= end; }
        bool bad() const { return rc < FRFXLL_OK; }
        void skip(size_t offs) {
          for (;offs && !eof(); --offs) {
            getch();
          }
        }
        Reader & SetError(FRFXLL_RESULT rc_) {
          if (!bad()) {
            rc = rc_;
          }
          return *this;
        }
        Reader & operator >> (uint8 & x) { x = (uint8)getch(); return *this; }
        Reader & operator >> (int8 & x) { 
          uint8 t; 
          *this >> t; 
          x = (int8)(t); 
          return *this; 
        }
        Reader & operator >> (uint16 & x) { 
          uint8 msb, lsb;
          if (dataInBigEndian) {
            *this >> msb >> lsb;
          } else {
            *this >> lsb >> msb;
          }
          x = lsb | (uint16(msb) << 8);
          return *this; 
        }
        Reader & operator >> (int16 & x) { 
          uint16 v;
          *this >> v;
          x = (int16)(v);
          return *this;
        }
        Reader & operator >> (uint32 & x) { 
          uint16 msb, lsb;
          if (dataInBigEndian) {
            *this >> msb >> lsb;
          } else {
            *this >> lsb >> msb;
          }
          x = lsb | (uint32(msb) << 16);
          return *this; 
        }
        Reader & operator >> (int32 & x) { 
          uint32 v;
          *this >> v;
          x = (int32)(v);
          return *this;
        }

        Reader & operator >> (uint64 & x) { 
          uint32 msb, lsb;
          if (dataInBigEndian) {
            *this >> msb >> lsb;
          } else {
            *this >> lsb >> msb;
          }
          x = lsb | (uint64(msb) << 32);
          return *this; 
        }

      };

      struct FtrCode {
        static const uint16 DPFP_CODE_BITS_USED_MASK    = 0x8000;
        static const uint16 DPFP_ENCRYPTED_MASK         = 0x4000;
        static const uint16 DPFP_LITTLE_ENDIAN_MASK     = 0x2000;
        // DPFP_MORE_FEATURES_FOLLOW indicates that after the current template,
        // there are more.  The address of the next template should be calculated by
        // adding the address of compressedData and compressedDataLen + 1 (for the end marker).
        static const uint16 DPFP_MORE_FEATURES_FOLLOW   = 0x1000;
        static const uint16 DPFP_EXTENDED_FEATURES      = 0x0800;
        static const uint16 DPFP_ALLOW_LEARNING         = 0x0400;
        static const uint16 DPFP_XTEA_MASK              = 0x0200;
        static const uint16 DPFP_PLAIN = 47;      //These remain for backward compatibility.  See FP_ENCRYPTED_MASK and
        static const uint16 DPFP_ENCRYPTED = 74;  //FP_IS_ENCRYPTED below for current usage.
        uint16 value;

        bool BytesSwapped() const { // should work for any byte order
          if (value == DPFP_PLAIN || value == DPFP_ENCRYPTED) return false;
          if (value == (DPFP_PLAIN << 8) || value == (DPFP_ENCRYPTED << 8)) return true; 
          if (value & DPFP_CODE_BITS_USED_MASK) return false;
          if (value & (DPFP_CODE_BITS_USED_MASK >> 8)) return true;
          return false;
        }
        friend Reader & operator >> (Reader & r, FtrCode & ftr) {
          r >> ftr.value;
          if (ftr.BytesSwapped()) {
            ftr.value = ((ftr.value & 0xff) << 8) | ((ftr.value & 0xff00) >> 8);
            r.dataInBigEndian = !r.dataInBigEndian;
          }
          if (ftr.value & DPFP_CODE_BITS_USED_MASK) {
            if (r.dataInBigEndian == ((ftr.value & DPFP_LITTLE_ENDIAN_MASK) != 0)) {
              r.rc = CheckResult(ErrorBadFingerprintData);
            }
          }
          if (!ftr.IsValid()) {
            r.rc = CheckResult(ErrorBadFingerprintData);
          }
          return r;
        }
        bool IsValid() const {
          if (value == DPFP_PLAIN || value == DPFP_ENCRYPTED) return true;
          return (value & DPFP_CODE_BITS_USED_MASK) != 0;
        }
        bool IsEncrypted() const { return (value & DPFP_CODE_BITS_USED_MASK) ? (value & DPFP_ENCRYPTED_MASK) != 0 : value != DPFP_PLAIN; }
        bool IsTeaEncrypted() const { return IsEncrypted() && !(value & DPFP_XTEA_MASK); }
        bool MoreViewsFollow() const { return (value & DPFP_MORE_FEATURES_FOLLOW) != 0; }
        bool HasEFBs() const { return (value & DPFP_EXTENDED_FEATURES) != 0; }
        FtrCode(uint16 code) : value(code) {}
        FtrCode() 
          : value(DPFP_CODE_BITS_USED_MASK | DPFP_ENCRYPTED_MASK | DPFP_LITTLE_ENDIAN_MASK | DPFP_EXTENDED_FEATURES) 
        {}
      };

      inline bool ValidTotalLength(Reader & r, size_t size) { return r.end >= r.start && size <= (size_t)(r.end - r.start); }
      inline bool ValidLength(Reader & r, size_t size) { return r.end >= r.cur && size <= (size_t)(r.end - r.cur); }
      inline bool ValidFingerPosition(uint16 fingerPosition) { return fingerPosition <= 10; };
      inline bool ValidResolution(uint16 resolution) { return resolution >= 99 && resolution <= 1000; };
      inline bool ValidImageSize(uint16 imageSize) { return imageSize >= 16 && imageSize < (1<<14); };

      struct StdFmdDeserializer {
        static const uint16 Resolution = 167;
        static const uint32 CBEFF_DEFAULT = 0x00330000;
        const unsigned char * const dataIn;
        const size_t & dataInSize;
        size_t totalLength; // in ISO 4 bytes; in ANSI can be 2 bytes (0x001A - 0xFFFF) or 6 bytes (0x000000010000 - 0x0000FFFFFFFF)
        uint8  devType1, devType2;
        uint16 imageSizeX;
        uint16 imageSizeY;
        uint16 resolutionX;
        uint16 resolutionY;
        uint8  numOfFingerViews;
        uint8  numOfMinutiae;

        uint32 CBEFF;
        uint16 specifiedFinger;
        uint16 specifiedView;
        uint8  rotation;

        StdFmdDeserializer(const unsigned char * dataIn_,
                           const size_t & dataInSize_)
          : dataIn(dataIn_)
          , dataInSize(dataInSize_)
          , CBEFF(0)
          , specifiedFinger(FRFXLL_FINGER_POSITION_NOT_SPECIFIED)
          , specifiedView(FRFXLL_VIEW_NUMBER_NOT_SPECIFIED)
          , rotation(0)
        {
        }
        virtual void ReadTotalLength(Reader & r) {} // in ISO 4 bytes; in ANSI can be 2 bytes (0x001A - 0xFFFF) or 6 bytes (0x000000010000 - 0x0000FFFFFFFF)
        virtual void ReadDeviceInfo(Reader & r) {}
        virtual void ReadTheta(Reader & r, Minutia & mp) {}
        FRFXLL_RESULT ReadRecordHeaders(Reader & r) {
          FRFXLL_RESULT rc = FRFXLL_OK;
          uint32 formatID;
          uint8  majorVersion;
          uint8  minorVersion;
          r >> formatID;
          if ( 0x464D5200 != formatID ) return CheckResult(ErrorBadFingerprintData);
          r.cur += 1;                                       // skip the leading space.
          r >> majorVersion;
          if ( majorVersion - '0' < 2 ) return CheckResult(ErrorBadFingerprintData);
          r >> minorVersion;
          r.cur += 1;                                       // skip the ending zero.
          ReadTotalLength(r); // in ISO 4 bytes; in ANSI can be 2 bytes (0x001A - 0xFFFF) or 6 bytes (0x000000010000 - 0x0000FFFFFFFF)
          if ( !ValidTotalLength(r, totalLength) ) return CheckResult(ErrorBadFingerprintData);
          ReadDeviceInfo(r);
          r >> imageSizeX;
          if ( !ValidImageSize(imageSizeX) ) return CheckResult(ErrorBadFingerprintData);
          r >> imageSizeY;
          if ( !ValidImageSize(imageSizeY) ) return CheckResult(ErrorBadFingerprintData);
          r >> resolutionX;
          if ( !ValidResolution(resolutionX) ) return CheckResult(ErrorBadFingerprintData);
          r >> resolutionY;
          if ( !ValidResolution(resolutionY) ) return CheckResult(ErrorBadFingerprintData);
          r >> numOfFingerViews;
          r.cur += 1;                                       // skip the reserved byte.
          return rc;
        }
        static uint8 ConfidenceFromQuality(uint8 quality) {
          return quality * 2;
        }
        static uint8 GlobalQualityFromFingerQuality(uint8 quality) {
          return quality;
        }
        FRFXLL_RESULT ReadMinutiae(Reader & r, MatchData & md) {
          uint16 x;
          uint16 y;
          const unsigned char * saved = r.cur;
          md.numMinutia = min((uint8)MatchData::Capacity, numOfMinutiae);
          for ( uint16 i = 0; i < md.numMinutia; i++ ) {
            r >> x;
            md.minutia[i].type = uint8(x >> 14);
            x &= 0x3FFF;
            md.minutia[i].position.x = muldiv(x, Resolution, resolutionX);
            r >> y;
            y &= 0x3FFF;
            md.minutia[i].position.y = muldiv(y, Resolution, resolutionY);
            ReadTheta(r, md.minutia[i]);
            uint8 b;
            r >> b;
            AssertR( b <= 100, ErrorBadFingerprintData );
            md.minutia[i].conf = ConfidenceFromQuality(b);
          }
          r.cur = saved + 6 * numOfMinutiae;
          return FRFXLL_OK;
        }
        void ConstructFootprint(MatchData & md) {
          md.offset.x = 0; md.offset.y = 0;
          if ( md.numMinutia > 0 ) {
            md.offset = md.minutia[0].position;
            for ( uint16 i = 1; i < md.numMinutia; i++ ) {
              Point & pos = md.minutia[i].position;
              if ( pos.x < md.offset.x ) md.offset.x = pos.x;
              if ( pos.y < md.offset.y ) md.offset.y = pos.y;
            }
          }
          uint16 xmax = 0x0000;
          uint16 ymax = 0x0000;
          uint16 j = 0;
          for (uint16 i = 0; i < md.numMinutia; i++) {
            Point & pos = md.minutia[i].position;
            pos.x -= md.offset.x;
            pos.y -= md.offset.y;
            if ( pos.x > 256 || pos.y > 400 ) { // rare case of out-of-bound minutia.
              continue;
            }
            if ( pos.x > xmax ) xmax = pos.x;
            if ( pos.y > ymax ) ymax = pos.y;
            if ( i != j ) {                     // shift the minutia as necessary.
              md.minutia[j] = md.minutia[i];
            }
            j++;
          }
          md.numMinutia = j;

          md.footprint.area = uint32(xmax) * ymax;
          xmax = (xmax+4)/8; ymax = (ymax+4)/8;
          for ( int32 y = 0; y <= ymax; y++ ) {
            for ( int32 x = 0; x <=xmax; x++ ) {
              md.footprint.Pixel(x, y) = true;
            }
          }
        }
        bool CheckView(uint8 finger, uint8 view) {
          return ( specifiedFinger == FRFXLL_FINGER_POSITION_NOT_SPECIFIED && specifiedView == FRFXLL_VIEW_NUMBER_NOT_SPECIFIED ) ||
                 ( specifiedFinger == finger && specifiedView == FRFXLL_VIEW_NUMBER_NOT_SPECIFIED ) ||
                 ( specifiedFinger == finger && specifiedView == view );
        }
        FRFXLL_RESULT ReadViewData(Reader & r, MatchData & md) {
          FRFXLL_RESULT rc = FRFXLL_ERR_NO_FP;
          uint8 finger;
          uint8 b;
          uint8 view;
          uint8 impressionType;
          uint8 fingerQuality;
          uint16 viewEDBLength;

          r >> finger;
          r >> b;
          view = b >> 4;
          impressionType = b & 0x0F;
          r >> fingerQuality;
          r >> numOfMinutiae;

          bool viewChecked = CheckView(finger, view);
          if ( viewChecked ) {
            md.Init();
            CheckR( ReadMinutiae(r, md) );
            if (rotation) {
              md.RotateAndShift(rotation);
            }
            ConstructFootprint(md);
            rc = FRFXLL_OK;
          } else {
            r.cur += 6 * numOfMinutiae;
          }

          r >> viewEDBLength;
          if ( !ValidLength(r, viewEDBLength) ) return CheckResult(ErrorBadFingerprintData);
          r.cur += viewEDBLength;
          md.Precompute();
          return rc;
        }
        FRFXLL_RESULT ReadViews(Reader & r, MatchData & md) {
          FRFXLL_RESULT rc = FRFXLL_OK;
          for (uint8 i = 0; i < numOfFingerViews; i++) {
            rc = ReadViewData(r, md);
            if ( FRFXLL_ERR_NO_FP != rc ) break;
          }
          return rc;
        }
        FRFXLL_RESULT UnpackParameters(const void * parameters) {
          if ( NULL == parameters ) return FRFXLL_OK;
          const FRFXLL_INPUT_PARAM_ISO_ANSI * pParams = reinterpret_cast<const FRFXLL_INPUT_PARAM_ISO_ANSI *>(parameters);
          if ( NULL == pParams ) return FRFXLL_ERR_INVALID_PARAM;
          uint8 version = 0;
          switch ( pParams->length ) {
            case sizeof(FRFXLL_INPUT_PARAM_ISO_ANSI):
              version = 1;
              break;
            default:
              return FRFXLL_ERR_INVALID_PARAM;
          }
          if (version >= 1 && pParams->CBEFF != FRFXLL_CBEFF_NOT_SPECIFIED) CBEFF = pParams->CBEFF;
          if (version >= 1 && pParams->fingerPosition != FRFXLL_FINGER_POSITION_NOT_SPECIFIED) specifiedFinger = pParams->fingerPosition;
          if (version >= 1 && pParams->viewNumber != FRFXLL_VIEW_NUMBER_NOT_SPECIFIED) specifiedView = pParams->viewNumber;
          if (version >= 1) rotation = pParams->rotation;
          return FRFXLL_OK;
        }
        FRFXLL_RESULT DeserializeFeatureSet(MatchData & md, const void * parameters) {
          FRFXLL_RESULT rc = UnpackParameters(parameters);
          if ( rc < FRFXLL_OK ) return rc;

          Reader r(dataIn, dataInSize, true);
          rc = ReadRecordHeaders(r);
          if ( FRFXLL_OK == rc && numOfFingerViews > 0 ) {
            rc = ReadViews(r, md);
          }
          return rc;
        }
      };

      struct IsoFmdDeserializer : public StdFmdDeserializer {
        IsoFmdDeserializer(const unsigned char * dataIn_,
                           const size_t & dataInSize_)
        : StdFmdDeserializer(dataIn_, dataInSize_) {
        }
        virtual void ReadTotalLength(Reader & r) { // in ISO 4 bytes
          uint32 length;
          r >> length;
          totalLength = length;
        }
        virtual void ReadDeviceInfo(Reader & r) {
          r >> devType1 >> devType2;                                // Device type ID.
        }
        virtual void ReadTheta(Reader & r, Minutia & mp) {
          r >> mp.theta;
        }
      };

      struct AnsiFmdDeserializer : public StdFmdDeserializer {
        AnsiFmdDeserializer(const unsigned char * dataIn_,
                            const size_t & dataInSize_)
        : StdFmdDeserializer(dataIn_, dataInSize_) {}
        
        virtual void ReadTotalLength(Reader & r) { // in ANSI can be 2 bytes (0x001A - 0xFFFF) or 6 bytes (0x000000010000 - 0x0000FFFFFFFF)
          uint32 length;
          r >> (uint16&)length;
          if ((uint16&)length != 0) {
            length = (uint16&)length;
          } else {
            r >> length;
          }
          totalLength = length;
        }
        virtual void ReadDeviceInfo(Reader & r) {
          r >> CBEFF;                                       // CBEFF product ID.
          r >> devType1 >> devType2;                        // Equipment ID.
        }
        virtual void ReadTheta(Reader & r, Minutia & mp) {
          uint8 theta;
          r >> theta;
          theta = muldiv(theta, 256, 180);
          theta = -theta + 64;
        }
      };

      inline FRFXLL_RESULT ReadIsoFeatures(
        const unsigned char data[],
        size_t size, 
        MatchData & md, 
        const void * parameters
      ) {
        IsoFmdDeserializer isoFDS(data, size);
        return isoFDS.DeserializeFeatureSet(md, parameters);
      }

      inline FRFXLL_RESULT ReadAnsiFeatures(
        const unsigned char data[], 
        size_t size, 
        MatchData & md, 
        const void * parameters
      ) {
        AnsiFmdDeserializer ansiFDS(data, size);
        return ansiFDS.DeserializeFeatureSet(md, parameters);
      }

    }
  }
}
#endif // __DESERIALIZEFPDATA_H

