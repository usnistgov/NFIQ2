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

#ifndef __SERIALIZEFPDATA_H
#define __SERIALIZEFPDATA_H

#include "deserializeFpData.h"

namespace FingerJetFxOSE {
  namespace FpRecEngineImpl {
    namespace Embedded {
      struct Writer : public HResult {
        unsigned char * data;
        size_t size;
        size_t cur;
        bool dataInBigEndian;
        bool reversed; // data written in the stream will be reversed

        Writer(unsigned char * data_,  size_t size_, bool dataInBigEndian_ = false)
          : data(data_), size(size_), cur(0), dataInBigEndian(dataInBigEndian_), reversed(false) {
        };
        virtual FRFXLL_RESULT putch(int32 c) { 
          if (!data) {
          } else if (eof()) {
            data = NULL;
          } else {
            data[cur] = uint8(c & 0xff); 
          }
          cur++;
          return GetResult();
        }
        virtual bool eof() const { return data ? cur >= size : false; }
        bool bad() const { return rc < FRFXLL_OK; }
        bool writeInBigEndian() const {
          return dataInBigEndian != reversed;
        }
        Writer & SetError(FRFXLL_RESULT rc_) {
          if (!bad()) rc = rc_;
          return *this;
        }
        Writer & operator << (uint8 x) {
          putch(x);
          return *this;
        }
        Writer & operator << (int8 x) {
          putch(x);
          return *this;
        }
        Writer & operator << (uint16 x) {
          if (writeInBigEndian()) {
            putch(x >> 8);
            putch(x);
          } else {
            putch(x);
            putch(x >> 8);
          }
          return *this;
        }
        Writer & operator << (int16 x) {
          return *this << (uint16)x;
        }
        Writer & operator << (uint32 x) {
          if (writeInBigEndian()) {
            *this << (uint16)(x >> 16) << (uint16)(x);
          } else {
            *this << (uint16)(x) << (uint16)(x >> 16);
          }
          return *this;
        }
        Writer & operator << (int32 x) {
          return *this << (uint32)x;
        }
      };
      struct Reversed {
        Writer & w;
        bool oldReversed;

        Reversed(Writer & w_, bool reversed = true) 
          : w(w_)
          , oldReversed(w_.reversed) 
        {
          w.reversed = reversed;
        }
        ~Reversed() { w.reversed = oldReversed; }
      };

      inline void Reverse(unsigned char data[], size_t size) {
        for (size_t i = 0; i < --size; i++) {
          unsigned char t = data[i];
          data[i] = data[size];
          data[size] = t;
        }
      }

      struct StdFmdSerializer {
        static const uint16 ImageSizeX = 250;
        static const uint16 ImageSizeY = 400;
        static const uint16 Resolution = StdFmdDeserializer::Resolution;
        static const uint16 DefaultResolution = 197; // 500DPI / 2.54

        unsigned char * const dataOut;
        const size_t dataOutSize;
        size_t recordHeaderLength;
        size_t viewDataOffset;
        size_t viewDataLength;
        size_t viewEDBOffset;
        size_t viewEDBLength;

        uint32 CBEFF;
        uint16 fingerPosition;
        uint16 viewNumber;
        uint16 resolutionX;
        uint16 resolutionY;
        uint16 imageSizeX;
        uint16 imageSizeY;
        uint8  rotation;
        uint8  fingerQuality;
        uint8  impressionType;

        StdFmdSerializer(unsigned char * dataOut_,
                         const size_t & dataOutSize_,
                         bool & isExtended_)
          : dataOut(dataOut_)
          , dataOutSize(dataOutSize_)
        {
          viewDataOffset =  0;
          viewDataLength =  0;
          viewEDBOffset =   0;
          viewEDBLength =   0;
          CBEFF =           0x00330000;
          fingerPosition =  0;
          viewNumber =      0;
          resolutionX =     DefaultResolution;
          resolutionY =     DefaultResolution;
          imageSizeX  =     FRFXLL_IMAGE_SIZE_NOT_SPECIFIED;
          imageSizeY  =     FRFXLL_IMAGE_SIZE_NOT_SPECIFIED;
          rotation    =     0;
          fingerQuality  =  86;
          impressionType =  0; // live scan
        }
        static uint8 QualityFromConfidence(uint8 confidence) {
          return min<uint8>((confidence + 1) / 2, 100);
        }
        virtual void WriteTotalLength(Writer & wr, size_t len) {}; // in ISO 4 bytes in ANSI can be 2 bytes (0x001A - 0xFFFF) or 6 bytes (0x000000010000 - 0x0000FFFFFFFF)
        virtual void WriteDeviceInfo(Writer & wr, const MatchData & md) {};
        virtual void WriteImageSize(Writer & wr, uint16 sizeX, uint16 sizeY) {};
        virtual void WriteTheta(Writer & wr, uint8 theta) {};
        void WriteMinutiae(Writer & wr, const MatchData & md) {
          uint16 sizeX = 0;
          uint16 sizeY = 0;

          size_t i;
          for ( i = 0; i < md.numMinutia; i++ ) {
            Point pos = md.minutia[i].position + md.offset;
            if ( resolutionX != Resolution ) {
              pos.x = muldiv(pos.x, resolutionX, Resolution);
            }
            if ( pos.x > sizeX ) sizeX = pos.x;
            if ( resolutionY != Resolution ) {
              pos.y = muldiv(pos.y, resolutionY, Resolution);
            }
            if ( pos.y > sizeY ) sizeY = pos.y;
            wr << uint16((pos.x & 0x3FFF) | (md.minutia[i].type << 14));    // X
            wr << uint16(pos.y & 0x3FFF);                                   // Y
            WriteTheta(wr, md.minutia[i].theta);                            // Theta
            wr << QualityFromConfidence(md.minutia[i].conf);                // Quality
          }
          size_t saved = wr.cur;
          WriteImageSize(wr, imageSizeX != FRFXLL_IMAGE_SIZE_NOT_SPECIFIED ? imageSizeX : (sizeX + 8), imageSizeY != FRFXLL_IMAGE_SIZE_NOT_SPECIFIED ? imageSizeY : (sizeY + 8));
          wr.cur = saved;
        }
        void WriteRecordHeaders(Writer & wr, const MatchData & md) {
          wr.cur = 0;
          wr << uint32(0x464D5200);                                         // Format Identifier
          wr << uint32(0x20323000);                                         // Version of this standard
          WriteTotalLength(wr, 0);                                          // Length of total record in bytes
          WriteDeviceInfo(wr, md);                                          // Capture device/system information
          wr << uint16(imageSizeX);                                         // Image Size in X
          wr << uint16(imageSizeY);                                         // Image Size in Y
          wr << uint16(resolutionX);                                        // X (horizontal) Resolution
          wr << uint16(resolutionY);                                        // Y (vertical) Resolution
          wr << uint8(0);                                                   // Number of Finger Views
          wr << uint8(0);                                                   // Reserved byte
        }
        void WriteViewData(Writer & wr, const MatchData & md) {
          wr.cur = viewDataOffset;
          wr << uint8(fingerPosition);                                      // Finger Position
          wr << uint8((viewNumber<<4) + (impressionType & 0x0F));           // View Number
                                                                            // Impression Type
          wr << fingerQuality;                                              // Finger Quality
          wr << uint8(md.numMinutia);                                       // Number of Minutiae
          WriteMinutiae(wr, md);
          wr << uint16(0);                                                  // Extended Data Block Length
          wr.cur = viewDataOffset - 2;
          wr << uint8(1);                                                   // Number of Finger Views
        }
        void WriteViewEDBHeaders(Writer & wr) {
          wr.cur = viewEDBOffset - 2;
          wr << uint16(viewEDBLength);                                      // Extended Data Block Length
          wr << uint8(0x01) << uint8(0x01);                                 // Extended Data Area Type Code
          wr << uint16(0);                                                  // Extended Data Area Length
        }
        void ComputeLengthAndOffset(size_t & size, const MatchData & md) {
          viewDataOffset = recordHeaderLength;
          viewDataLength = 6 + 6 * md.numMinutia; // 4 = view header length; 2 = EDB Length length
          viewEDBOffset = viewDataOffset + viewDataLength;
          viewEDBLength = 0;
          size = recordHeaderLength + viewDataLength + viewEDBLength;
        }
        FRFXLL_RESULT UnpackParameters(const void * parameters) {
          if ( NULL == parameters ) return FRFXLL_OK;
          const FRFXLL_OUTPUT_PARAM_ISO_ANSI * pParams = reinterpret_cast<const FRFXLL_OUTPUT_PARAM_ISO_ANSI *>(parameters);
          if ( NULL == pParams ) return FRFXLL_ERR_INVALID_PARAM;
          uint8 version = 0;
          switch ( pParams->length ) {
            case sizeof(FRFXLL_OUTPUT_PARAM_ISO_ANSI):
              version = 1;
              break;
            default:
              return FRFXLL_ERR_INVALID_PARAM;
          }
          if (version >= 1 && pParams->CBEFF != FRFXLL_CBEFF_NOT_SPECIFIED) CBEFF = pParams->CBEFF;
          if (version >= 1 && pParams->fingerPosition != FRFXLL_FINGER_POSITION_NOT_SPECIFIED) fingerPosition = pParams->fingerPosition;
          if ( !ValidFingerPosition(fingerPosition) ) return FRFXLL_ERR_INVALID_PARAM;
          if (version >= 1 && pParams->viewNumber != FRFXLL_VIEW_NUMBER_NOT_SPECIFIED)   viewNumber = pParams->viewNumber;
          if (version >= 1 && pParams->resolutionX != FRFXLL_RESOLUTION_NOT_SPECIFIED) resolutionX = pParams->resolutionX;
          if ( !ValidResolution(resolutionX) ) return FRFXLL_ERR_INVALID_PARAM;
          if (version >= 1 && pParams->resolutionY != FRFXLL_RESOLUTION_NOT_SPECIFIED) resolutionY = pParams->resolutionY;
          if ( !ValidResolution(resolutionY) ) return FRFXLL_ERR_INVALID_PARAM;
          if (version >= 1 && pParams->imageSizeX != FRFXLL_IMAGE_SIZE_NOT_SPECIFIED) {
            imageSizeX = pParams->imageSizeX;
            if ( !ValidImageSize(imageSizeX) ) return FRFXLL_ERR_INVALID_PARAM;
          }
          if (version >= 1 && pParams->imageSizeY != FRFXLL_IMAGE_SIZE_NOT_SPECIFIED) {
            imageSizeY = pParams->imageSizeY;
            if ( !ValidImageSize(imageSizeY) ) return FRFXLL_ERR_INVALID_PARAM;
          }
          if (version >= 1) rotation = pParams->rotation;
          if (version >= 1) {
            if (pParams->fingerQuality > 100) return FRFXLL_ERR_INVALID_PARAM;
            fingerQuality = pParams->fingerQuality;
          }
          if (version >= 1) {
            if (pParams->impressionType > 8)  return FRFXLL_ERR_INVALID_PARAM;
            impressionType = pParams->impressionType;
          }
          return FRFXLL_OK;
        }
        FRFXLL_RESULT SerializeFeatureSet(
          size_t & size,
          const MatchData & md0,
          const void * parameters,
          bool fixedSize,
          FtrCode ftrCode
        ) {
          FRFXLL_RESULT rc = UnpackParameters(parameters);
          if ( rc < FRFXLL_OK ) return rc;
          MatchData md(md0);
          if (rotation) {
            md.RotateAndShift(rotation);
          }

          ComputeLengthAndOffset(size, md);
          if ( NULL == dataOut ) return FRFXLL_OK;
          if ( dataOutSize < recordHeaderLength ) return FRFXLL_ERR_MORE_DATA;

          size_t   totalLength = 0;
          Writer   wr(dataOut, dataOutSize, true);
          rc = FRFXLL_ERR_MORE_DATA;
          WriteRecordHeaders(wr, md);
          totalLength = recordHeaderLength;
          if ( dataOutSize >= viewDataOffset + viewDataLength ) {
            WriteViewData(wr, md);
            totalLength += viewDataLength;
          }
          WriteTotalLength(wr, totalLength); // in ISO 4 bytes; in ANSI can be 2 bytes (0x001A - 0xFFFF) or 6 bytes (0x000000010000 - 0x0000FFFFFFFF)
          return FRFXLL_OK;
        }
      };

      struct IsoFmdSerializer : public StdFmdSerializer {
        IsoFmdSerializer(unsigned char * dataOut_,
                         const size_t & dataOutSize_,
                         bool & isExtended_)
                         : StdFmdSerializer(dataOut_, dataOutSize_, isExtended_) {
          recordHeaderLength = 24;
        }
        virtual void WriteTotalLength(Writer & wr, size_t len) { // in ISO 4 bytes
          wr.cur = 8;
          wr << uint32(len);
        }
        virtual void WriteDeviceInfo(Writer & wr, const MatchData & md) {
          wr << uint8(0) << uint8(0);
        }
        virtual void WriteImageSize(Writer & wr, uint16 sizeX, uint16 sizeY) {
          wr.cur = 14;
          wr << sizeX;
          wr << sizeY;
        }
        virtual void WriteTheta(Writer & wr, uint8 theta) {
          wr << uint8(-theta + 64);
        }
      };

      struct AnsiFmdSerializer : public StdFmdSerializer {
        AnsiFmdSerializer(unsigned char * dataOut_,
                          const size_t & dataOutSize_,
                          bool & isExtended_)
                          : StdFmdSerializer(dataOut_, dataOutSize_, isExtended_) {
          recordHeaderLength = 26;
        }
        virtual void WriteTotalLength(Writer & wr, size_t len) { // in ANSI can be 2 bytes (0x001A - 0xFFFF) or 6 bytes (0x000000010000 - 0x0000FFFFFFFF)
          wr.cur = 8;
          wr << uint16(len); // 2 bytes should be enough
        }
        virtual void WriteDeviceInfo(Writer & wr, const MatchData & md) {
          wr << CBEFF;                                      // CBEFF product identifier
          wr << uint8(0) << uint8(0);
        }
        virtual void WriteImageSize(Writer & wr, uint16 sizeX, uint16 sizeY) {
          wr.cur = 16;
          wr << sizeX;
          wr << sizeY;
        }
        virtual void WriteTheta(Writer & wr, uint8 theta) {
          theta = -theta + 64;
          theta = muldiv(theta, 180, 256);
          wr << uint8(theta);
        }
      };

      inline FRFXLL_RESULT WriteIsoFeatures(
        unsigned char dataOut[], 
        size_t & size, 
        const MatchData & md,
        const void * parameters,
        bool isExtended,
        bool fixedSize = false,
        bool isTemplate = false,
        FtrCode ftrCode = FtrCode()
      ) {
        IsoFmdSerializer isoFS(dataOut, size, isExtended);
        return isoFS.SerializeFeatureSet(size, md, parameters, fixedSize, ftrCode);
      }

      inline FRFXLL_RESULT WriteAnsiFeatures(
        unsigned char dataOut[], 
        size_t & size, 
        const MatchData & md, 
        const void * parameters,
        bool isExtended,
        bool fixedSize = false,
        bool isTemplate = false,
        FtrCode ftrCode = FtrCode()
      ) {
        AnsiFmdSerializer ansiFS(dataOut, size, isExtended);
        return ansiFS.SerializeFeatureSet(size, md, parameters, fixedSize, ftrCode);
      }

    }
  }
}
#endif
