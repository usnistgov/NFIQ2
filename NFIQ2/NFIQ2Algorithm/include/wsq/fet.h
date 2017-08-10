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


#ifndef _FET_H
#define _FET_H

#include <stdio.h>
#include <stdlib.h> /* Added by MDG on 03-10-05 */

#ifndef True
#define True		1
#define False		0
#endif
#define FET_EXT		"fet"
#define MAXFETS		100
#define MAXFETLENGTH	512

typedef struct fetstruct{
   int alloc;
   int num;
   char **names;
   char **values;
} FET;

/* allocfet.c */
extern FET  *allocfet(int);
extern int  allocfet_ret(FET **, int);
extern FET  *reallocfet(FET *, int);
extern int  reallocfet_ret(FET **, int);
/* delfet.c */
extern void deletefet(char *, FET *);
extern int  deletefet_ret(char *, FET *);
/* extfet.c */
extern char *extractfet(char *, FET *);
extern int  extractfet_ret(char **, char *, FET *);
/* freefet.c */
extern void freefet(FET *);
/* lkupfet.c */
extern int  lookupfet(char **, char *, FET *);
/* printfet.c */
extern void printfet(FILE *, FET *);
/* readfet.c */
extern FET  *readfetfile(char *);
extern int  readfetfile_ret(FET **, char *);
/* strfet.c */
extern int fet2string(char **, FET *);
extern int string2fet(FET **, char *);
/* updatfet.c */
extern void updatefet(char *, char *, FET *);
extern int  updatefet_ret(char *, char *, FET *);
/* writefet.c */
extern void writefetfile(char *, FET *);
extern int  writefetfile_ret(char *, FET *);

#endif  /* !_FET_H */
