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


#ifndef _IHEAD_H
#define _IHEAD_H

/************************************************************/
/*         File Name: IHead.h                               */
/*         Package:   NIST Internal Image Header            */
/*         Author:    Michael D. Garris                     */
/*         Date:      2/08/90                               */
/*         Updated:   3/14/05 by MDG                        */
/************************************************************/

#include <stdio.h> /* Added by MDG in 03-14-05 */
#include <stdlib.h> /* Added by MDG in 03-14-05 */

/* Defines used by the ihead structure */
#define IHDR_SIZE	288	/* len of hdr record (always even bytes) */
#define SHORT_CHARS 	8	/* # of ASCII chars to represent a short */
#define BUFSIZE		80	/* default buffer size */
#define DATELEN		26	/* character length of date string */

typedef struct ihead{
   char id[BUFSIZE];			/* identification/comment field */
   char created[DATELEN];		/* date created */
   char width[SHORT_CHARS];		/* pixel width of image */
   char height[SHORT_CHARS];		/* pixel height of image */
   char depth[SHORT_CHARS];		/* bits per pixel */
   char density[SHORT_CHARS];		/* pixels per inch */
   char compress[SHORT_CHARS];		/* compression code */
   char complen[SHORT_CHARS];		/* compressed data length */
   char align[SHORT_CHARS];		/* scanline multiple: 8|16|32 */
   char unitsize[SHORT_CHARS];		/* bit size of image memory units */
   char sigbit;				/* 0->sigbit first | 1->sigbit last */
   char byte_order;			/* 0->highlow | 1->lowhigh*/
   char pix_offset[SHORT_CHARS];	/* pixel column offset */
   char whitepix[SHORT_CHARS];		/* intensity of white pixel */
   char issigned;			/* 0->unsigned data | 1->signed data */
   char rm_cm;				/* 0->row maj | 1->column maj */
   char tb_bt;				/* 0->top2bottom | 1->bottom2top */
   char lr_rl;				/* 0->left2right | 1->right2left */
   char parent[BUFSIZE];		/* parent image file */
   char par_x[SHORT_CHARS];		/* from x pixel in parent */
   char par_y[SHORT_CHARS];		/* from y pixel in parent */
}IHEAD;

/* General Defines */
#define UNCOMP		0
#define CCITT_G3	1
#define CCITT_G4	2
#define RL		5
#define JPEG_SD		6
#define WSQ_SD14	7
#define MSBF		'0'
#define LSBF		'1'
#define HILOW		'0'
#define LOWHI		'1'
#define UNSIGNED	'0'
#define SIGNED		'1'
#define ROW_MAJ		'0'
#define COL_MAJ		'1'
#define TOP2BOT		'0'
#define BOT2TOP		'1'
#define LEFT2RIGHT	'0'
#define RIGHT2LEFT	'1'

#define BYTE_SIZE	8.0

/* getcomp.c */
extern int getcomptype(char *);
/* getnset.c */
extern char *get_id(IHEAD *);
extern int set_id(IHEAD *, char *);
extern char *get_created(IHEAD *);
extern int set_created(IHEAD *);
extern int get_width(IHEAD *);
extern int set_width(IHEAD *, int);
extern int get_height(IHEAD *);
extern int set_height(IHEAD *, int);
extern int get_depth(IHEAD *);
extern int set_depth(IHEAD *, int);
extern int get_density(IHEAD *);
extern int set_density(IHEAD *, int);
extern int get_compression(IHEAD *);
extern int set_compression(IHEAD *, int);
extern int get_complen(IHEAD *);
extern int set_complen(IHEAD *, int);
extern int get_align(IHEAD *);
extern int set_align(IHEAD *, int);
extern int get_unitsize(IHEAD *);
extern int set_unitsize(IHEAD *, int);
extern int get_sigbit(IHEAD *);
extern int set_sigbit(IHEAD *, int);
extern int get_byte_order(IHEAD *);
extern int set_byte_order(IHEAD *, int);
extern int get_pix_offset(IHEAD *);
extern int set_pix_offset(IHEAD *, int);
extern int get_whitepix(IHEAD *);
extern int set_whitepix(IHEAD *, int);
extern int get_issigned(IHEAD *);
extern int set_issigned(IHEAD *, int);
extern int get_rm_cm(IHEAD *);
extern int set_rm_cm(IHEAD *, int);
extern int get_tb_bt(IHEAD *);
extern int set_tb_bt(IHEAD *, int);
extern int get_lr_rl(IHEAD *);
extern int set_lr_rl(IHEAD *, int);
extern char *get_parent(IHEAD *);
extern int set_parent(IHEAD *, char *);
extern int get_par_x(IHEAD *);
extern int set_par_x(IHEAD *, int);
extern int get_par_y(IHEAD *);
extern int set_par_y(IHEAD *, int);
/* nullihdr.c */
extern void nullihdr(IHEAD *);
/* parsihdr.c */
extern void parseihdrid(char *, char *, char *);
/* prntihdr.c */
extern void printihdr(IHEAD *, FILE *);
/* readihdr.c */
extern IHEAD *readihdr(register FILE *);
/* valdcomp.c */
extern int valid_compression(int);
/* writihdr.c */
extern void writeihdr(FILE *, IHEAD *);

#endif  /* !_IHEAD_H */
