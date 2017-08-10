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

#ifndef _BINOPS_H
#define _BINOPS_H

/************************************************************/
/*         File Name: Binops.h                              */
/*         Package:   NIST Binary Image Utilities           */
/*         Author:    Michael D. Garris                     */
/*         Updated:   03/14/2005                            */
/*         Updated:   06/08/2005 by MDG                     */
/************************************************************/

#define BINARY_COPY	0
#define BINARY_OR	1
#define BINARY_AND	2
#define BINARY_XOR	3
#define BINARY_INVERT	4
#define BINARY_ZERO	5
#define BINARY_ONE	6

#ifndef BITSPERBYTE
#define BITSPERBYTE CHAR_BIT
#endif

/* bincopy.c */
extern void binary_subimage_copy_8 (register unsigned char *, int, int,
                   register unsigned char *, int, int, int, int, int, int,
                   int, int);
extern void binary_subimage_copy_gt(register unsigned char *, int, int,
                   register unsigned char *, int, int, int, int, int, int,
                   int, int);
extern void binary_subimage_copy_lt(register unsigned char *, int, int,
                   register unsigned char *, int, int, int, int, int, int,
                   int, int);
extern void binary_subimage_copy_eq(register unsigned char *, int, int,
                   register unsigned char *, int, int, int, int, int, int,
                   int, int);
extern void binary_subimage_copy(register unsigned char *, int, int,
                   register unsigned char *, int, int, int, int, int, int,
                   int, int);
/* binfill.c */
extern void binary_fill_partial(int, unsigned char *, int i, unsigned char *,
                   int, int);
extern int gb(unsigned char *, int);
extern void sb(unsigned char *, int, int);
/* binpad.c */
extern int binary_image_pad(unsigned char **, unsigned int, unsigned int,
                   unsigned int, unsigned int, int);
extern int binary_image_mpad(unsigned char **, unsigned int *, unsigned int *,
                   unsigned int, unsigned int, int);

#endif  /* !_BINOPS_H */
