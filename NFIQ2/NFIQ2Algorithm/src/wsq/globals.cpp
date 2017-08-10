/*******************************************************************************

License: 
This software and/or related materials was developed at the National Institute
of Standards and Technology (NIST) by employees of the Federal Government
in the course of their official duties. Pursuant to title 17 Section 105
of the United States Code, this software is not subject to copyright
protection and is in the public domain. 

This software and/or related materials have been determined to be not subject
to the EAR (see Part 734.3 of the EAR for exact details) because it is
a publicly available technology and software, and is freely distributed
to any interested party with no licensing requirements.  Therefore, it is 
permissible to distribute this software as a free download from the internet.

Disclaimer: 
This software and/or related materials was developed to promote biometric
standards and biometric technology testing for the Federal Government
in accordance with the USA PATRIOT Act and the Enhanced Border Security
and Visa Entry Reform Act. Specific hardware and software products identified
in this software were used in order to perform the software development.
In no case does such identification imply recommendation or endorsement
by the National Institute of Standards and Technology, nor does it imply that
the products and equipment identified are necessarily the best available
for the purpose.

This software and/or related materials are provided "AS-IS" without warranty
of any kind including NO WARRANTY OF PERFORMANCE, MERCHANTABILITY,
NO WARRANTY OF NON-INFRINGEMENT OF ANY 3RD PARTY INTELLECTUAL PROPERTY
or FITNESS FOR A PARTICULAR PURPOSE or for any purpose whatsoever, for the
licensed product, however used. In no event shall NIST be liable for any
damages and/or costs, including but not limited to incidental or consequential
damages of any kind, including economic damage or injury to property and lost
profits, regardless of whether NIST shall be advised, have reason to know,
or in fact shall know of the possibility.

By using this software, you agree to bear all risk relating to quality,
use and performance of the software and/or related materials.  You agree
to hold the Government harmless from any claim arising from your use
of the software.

*******************************************************************************/


/***********************************************************************
      LIBRARY: WSQ - Grayscale Image Compression

      FILE:    GLOBALS.C
      AUTHORS: Craig Watson
               Michael Garris
      DATE:    11/24/1999
      UPDATED: 10/24/07 (Kenneth Ko)

      Contains global variable declarations and assignments
      that support WSQ image compression.

***********************************************************************/

#include <wsq.h>

/*
int debug;
*/
#ifdef TARGET_OS
   QUANT_VALS quant_vals;

   W_TREE w_tree[W_TREELEN];

   Q_TREE q_tree[Q_TREELEN];

   DTT_TABLE dtt_table;

   DQT_TABLE dqt_table;

   DHT_TABLE dht_table[MAX_DHT_TABLES];

   FRM_HEADER_WSQ frm_header_wsq;
#else
   QUANT_VALS quant_vals = {};

   W_TREE w_tree[W_TREELEN] = {};

   Q_TREE q_tree[Q_TREELEN] = {};

   DTT_TABLE dtt_table = {};

   DQT_TABLE dqt_table = {};

   DHT_TABLE dht_table[MAX_DHT_TABLES] = {};

   FRM_HEADER_WSQ frm_header_wsq = {};
#endif

#ifdef FILTBANK_EVEN_8X8_1
float hifilt[MAX_HIFILT] =  {
                              0.03226944131446922,
                             -0.05261415011924844,
                             -0.18870142780632693,
                              0.60328894481393847,
                             -0.60328894481393847,
                              0.18870142780632693,
                              0.05261415011924844,
                             -0.03226944131446922 };

float lofilt[MAX_LOFILT] =  {
                              0.07565691101399093,
                             -0.12335584105275092,
                             -0.09789296778409587,
                              0.85269867900940344,
                              0.85269867900940344,
                             -0.09789296778409587,
                             -0.12335584105275092,
                              0.07565691101399093 };
#else
float hifilt[MAX_HIFILT] =  { 0.06453888262893845,
                              -0.04068941760955844,
                              -0.41809227322221221,
                               0.78848561640566439,
                              -0.41809227322221221,
                              -0.04068941760955844,
                               0.06453888262893845 };

float lofilt[MAX_LOFILT] =  { 0.03782845550699546,
                              -0.02384946501938000,
                              -0.11062440441842342,
                               0.37740285561265380,
                               0.85269867900940344,
                               0.37740285561265380,
                              -0.11062440441842342,
                              -0.02384946501938000,
                               0.03782845550699546 };
#endif
