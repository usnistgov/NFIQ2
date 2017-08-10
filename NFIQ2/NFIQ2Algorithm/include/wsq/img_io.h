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


#ifndef _IMG_IO_H
#define _IMG_IO_H

/* UPDATED: 03/15/2005 by MDG */
/* UPDATED: 12/22/2008 by Gregory Fiumara */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <wsq.h>
#include <ihead.h>
#include <ioutil.h>
#include <memalloc.h>
#include <imgutil.h>
#include <util.h>

/* grp4comp.c */
extern void grp4decomp(unsigned char *, int, int, int, unsigned char *, int *);
extern void grp4comp(unsigned char *, int, int, int, unsigned char *, int *);

/* img_io.c */
extern int read_raw_from_filesize(char *, unsigned char **, int *);
extern int write_raw_from_memsize(char *, unsigned char *, const int);
extern int read_ihead(char *, IHEAD **, unsigned char **, int *, int *, int *);
extern int read_raw(char *, unsigned char **, int *, int *, int *);
extern int read_raw_or_ihead_wsq(const int, char *, IHEAD **,
                             unsigned char **, int *, int *, int *);
int write_raw_or_ihead(const int, char *, unsigned char *,
                       const int, const int, const int, const int);

/* readihdr.c */
extern void ReadBinaryRaster(char *, IHEAD **, unsigned char **,
                            int *, int *, int *);
extern void ReadIheadRaster(char *, IHEAD **, unsigned char **,
                            int *, int *, int *);

/* writeihdr.c */
extern void writeihdrfile(char *, IHEAD *, unsigned char *);
extern void writeihdrsubimage(char *, unsigned char *, int, int, int,
                              char *, int, int);
extern void write_fields(char *, char **, int, IHEAD **, unsigned char **,
                         int, int);

#endif /* !_IMG_IO_H */
