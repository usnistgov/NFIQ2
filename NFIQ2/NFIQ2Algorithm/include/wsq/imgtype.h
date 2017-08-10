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


#ifndef _IMGTYPE_H
#define _IMGTYPE_H

/* UPDATED: 03/15/2005 by MDG */
/* UPDATED: 01/24/2008 by Kenneth Ko */
/* UPDATED: 01/31/2008 by Kenneth Ko */
/* UPDATED: 09/03/2008 by Kenneth Ko */
/* UPDATED: 01/06/2009 by Kenneth Ko - add support for HPUX compile */

#include <wsq.h>
#include <jpegb.h>
#include <jpegl.h>
#include <ihead.h>
#include <an2k.h>
#include <dataio.h>
#include <computil.h>
#ifdef __NBIS_JASPER__
	#include <jasper/jasper.h>
#endif
#ifdef __NBIS_PNG__
	#include <png.h>
#endif

#define UNKNOWN_IMG -1
#define RAW_IMG     0
#define WSQ_IMG     1
#define JPEGL_IMG   2
#define JPEGB_IMG   3
#define IHEAD_IMG   4
#define ANSI_NIST_IMG 5
#define JP2_IMG   6
#define PNG_IMG   7

/* imgtype.c */
extern int image_type(int *, unsigned char *, const int);
extern int jpeg_type(int *, unsigned char *, const int);
#ifdef __NBIS_JASPER__ 
	extern int is_jp2(unsigned char *, const int);
#endif

#endif /* !_IMGTYPE_H */
