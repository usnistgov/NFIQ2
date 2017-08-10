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

#ifndef _DEFS_H
#define _DEFS_H

/*********************************************************************/
/*          General Purpose Defines                                  */
/*********************************************************************/
#ifndef True
#define True		1
#define False		0
#endif
#ifndef TRUE
#define TRUE		True
#define FALSE		False
#endif
#define Yes		True
#define No		False
#define Empty		NULL
#ifndef None
#define None		-1
#endif
#ifndef FOUND
#define FOUND            1
#endif
#define NOT_FOUND_NEG   -1
#define EOL		EOF
#ifndef DEG2RAD
#define DEG2RAD	(double)(57.29578)
#endif
#define max(a, b)   ((a) > (b) ? (a) : (b))
#define min(a, b)   ((a) < (b) ? (a) : (b))
#define sround(x) ((int) (((x)<0) ? (x)-0.5 : (x)+0.5))
#define sround_uint(x) ((unsigned int) (((x)<0) ? (x)-0.5 : (x)+0.5))
#define align_to_16(_v_)   ((((_v_)+15)>>4)<<4)
#define align_to_32(_v_) ((((_v_)+31)>>5)<<5)
#ifndef CHUNKS
#define CHUNKS          100
#endif

#endif /* !_DEFS_H */
