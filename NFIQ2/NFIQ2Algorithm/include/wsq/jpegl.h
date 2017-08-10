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


/****************************************************************/
/*                                                              */
/*              This header was created for use with            */
/*              lossless jpeg compression and decompression     */
/*              designed for 8 bit input precision              */
/*                                                              */
/*              developed by:  Craig Watson                     */
/*              date:  6 Nov 91                                 */
/*              updated:  22 DEC 97                             */
/*              updated:  03/11/2005 by MDG                     */
/*                                                              */
/****************************************************************/
#ifndef _JPEGL_H
#define _JPEGL_H

#ifndef _NISTCOM_H
#include <nistcom.h>
#endif

/* JPEGL Marker Definitions */
#define SOF3 0xffc3
#define DHT  0xffc4
#define RST0 0xffd0
#define RST1 0xffd1
#define RST2 0xffd2
#define RST3 0xffd3
#define RST4 0xffd4
#define RST5 0xffd5
#define RST6 0xffd6
#define RST7 0xffd7
#define SOI  0xffd8
#define EOI  0xffd9
#define SOS  0xffda
#define DNL  0xffdc
#define DRI  0xffdd
#define COM  0xfffe
#define APP0 0xffe0
/* Case for getting ANY marker. */
#define ANY  0xffff
/* Cases for getting a table from a set of possible ones. */
#define TBLS_N_SOF     2
#define TBLS_N_SOS     (TBLS_N_SOF + 1)

/* Predictor Definitions */
/*    c b    */
/*    a x    */
#define   PRED1       1 /* Px = Ra */
#define   PRED2       2 /* Px = Rb */
#define   PRED3       3 /* Px = Rc */
#define   PRED4       4 /* Px = Ra+Rb-Rc */
#define   PRED5       5 /* Px = Ra+((Rb-Rc)/2) */
#define   PRED6       6 /* Px = Rb+((Ra-Rc)/2) */
#define   PRED7       7 /* Px = (Ra+Rb)/2 */
#define   BITSET      0x01
#define   LSBITMASK   0x0001
#define   CATMASK     0x8000

#define   NO_INTRLV      0
#define   MAX_CMPNTS 4
#define   FREE_IMAGE     1
#define   NO_FREE_IMAGE  0
#define   BUF_SIZE       50000   /* Compressed image buffer size */

#define   MIN_HUFFTABLE_ID 16  /* Set according to JPEGL spec */
#define   MAX_HUFFBITS     16  /* DO NOT CHANGE THIS CONSTANT!! */
#define   MAX_HUFFCOUNTS_JPEGL 16  /* Length of code table: change as */
                                   /* needed but DO NOT EXCEED 256    */
#define   MAX_CATEGORY  10    /* Largest difference category for uchar data */
#define   LARGESTDIFF   511   /* Largest difference value */

#define   READ_TABLE_LEN    1
#define   NO_READ_TABLE_LEN 0

#define   FIRSTBIT     7
#ifndef   BITSPERBYTE
#define   BITSPERBYTE  8
#endif

/* JFIF SCAN UNIT DESIGNATORS */
#define UNKNOWN_UNITS    0
#define PPI_UNITS       1  /* pixels per inch */
#define PPCM_UNITS      2  /* pixels per centimeter */


typedef struct hcode {
   short size;
   unsigned int code;
} HUFFCODE;

#define JFIF_IDENT      "JFIF"
#define JFIF_IDENT_LEN  5
#define JFIF_VERSION    0x0102
#define JFIF_HEADER_LEN 16

typedef struct jheader {
   unsigned short ver;
   char ident[JFIF_IDENT_LEN];
   unsigned char units;
   unsigned short dx, dy;
   unsigned char tx, ty;
} JFIF_HEADER;

typedef struct image_dat {
   int max_width, max_height, pix_depth, ppi;
   int intrlv; /* 0 = no, 1 = yes */
   int n_cmpnts;
   int cmpnt_depth;
   int hor_sampfctr[MAX_CMPNTS];
   int vrt_sampfctr[MAX_CMPNTS];
   int samp_width[MAX_CMPNTS];
   int samp_height[MAX_CMPNTS];
   unsigned char point_trans[MAX_CMPNTS];
   unsigned char predict[MAX_CMPNTS];
   unsigned char *image[MAX_CMPNTS];
   short *diff[MAX_CMPNTS]; /* was short ** */
} IMG_DAT;

typedef struct htable {
   unsigned char def;
   unsigned char table_id;
   unsigned char *bits;
   unsigned char *values;
   int last_size;
   int *codesize;
   int *freq;
   int *maxcode;
   int *mincode;
   int *valptr;
   HUFFCODE *huffcode_table;
} HUF_TABLE;

typedef struct fheader {
   unsigned char prec;
   unsigned short x;
   unsigned short y;
   unsigned char Nf;
   unsigned char C[MAX_CMPNTS];
   unsigned char HV[MAX_CMPNTS];
   unsigned char Tq[MAX_CMPNTS];
} FRM_HEADER_JPEGL;

typedef struct sheader {
   unsigned char Ns;
   unsigned char Cs[MAX_CMPNTS];
   unsigned char Tda[MAX_CMPNTS];
   unsigned char Ss;
   unsigned char Se;
   unsigned char Ahl;
} SCN_HEADER;

/* GLOBAL VARIABLES */
extern int debug;

/* encoder.c */
extern int jpegl_encode_mem(unsigned char **, int *, IMG_DAT *, char *);
extern int gen_diff_freqs(IMG_DAT *, HUF_TABLE **);
extern int compress_image_intrlv(IMG_DAT *, HUF_TABLE **,
                    unsigned char *, const int, int *);
extern int compress_image_non_intrlv(IMG_DAT *, HUF_TABLE **,
                    unsigned char *, const int, int *);
extern int code_diff(HUFFCODE *, HUFFCODE *, int *, unsigned int *, short *);

/* decoder.c */
extern int jpegl_decode_mem(IMG_DAT **, int *, unsigned char *, const int);
extern void build_huff_decode_table(int [MAX_CATEGORY][LARGESTDIFF+1]);
extern int decode_data(int *, int *, int *, int *, unsigned char *,
                    unsigned char **, unsigned char *, int *);
extern int nextbits_jpegl(unsigned short *, FILE *, int *, const int);
extern int getc_nextbits_jpegl(unsigned short *, unsigned char **,
                    unsigned char *, int *, const int);

/* huff.c */
extern int read_huffman_table(unsigned char *, unsigned char **,
                   unsigned char **, const int, FILE *, const int, int *);
extern int getc_huffman_table(unsigned char *, unsigned char **,
                   unsigned char **, const int, unsigned char **,
                   unsigned char *, const int, int *);
extern int write_huffman_table(const unsigned short, const unsigned char,
                   unsigned char *, unsigned char *, FILE  *);
extern int putc_huffman_table(const unsigned short, const unsigned char,
                   unsigned char *, unsigned char *, unsigned char *,
                   const int, int *);
extern int find_huff_sizes(int **, int *, const int);
extern void find_least_freq(int *, int *, int *, const int);
extern int find_num_huff_sizes(unsigned char **, int *, int *, const int);
extern int sort_huffbits(unsigned char *);
extern int sort_code_sizes(unsigned char **, int *, const int);
extern int build_huffcode_table(HUFFCODE **, HUFFCODE *, const int,
                   unsigned char *, const int);
extern int build_huffsizes(HUFFCODE **, int *, unsigned char *, const int);
extern void build_huffcodes(HUFFCODE *);
extern void gen_decode_table(HUFFCODE *, int *, int *, int *, unsigned char *);

/* huftable.c */
extern int gen_huff_tables(HUF_TABLE **, const int);
extern int read_huffman_table_jpegl(HUF_TABLE **, FILE *);
extern int getc_huffman_table_jpegl(HUF_TABLE **, unsigned char **,
                   unsigned char *);
extern void free_HUFF_TABLES(HUF_TABLE **, const int);
extern void free_HUFF_TABLE(HUF_TABLE *);

/* imgdat.c */
int get_IMG_DAT_image(unsigned char **, int *, int *, int *, int *, int *,
                   IMG_DAT *);
extern int setup_IMG_DAT_nonintrlv_encode(IMG_DAT **, unsigned char *,
                   const int, const int, const int, const int, int *, int *,
                   const int, const unsigned char, const unsigned char);
extern int setup_IMG_DAT_decode(IMG_DAT **, const int, FRM_HEADER_JPEGL *);
extern int update_IMG_DAT_decode(IMG_DAT *, SCN_HEADER *, HUF_TABLE **);
extern void free_IMG_DAT(IMG_DAT *, const int);

/* ppi.c */
extern int get_ppi_jpegl(int *, JFIF_HEADER *);

/* tableio.c */
extern int read_marker_jpegl(unsigned short *, const int, FILE *);
extern int getc_marker_jpegl(unsigned short *, const int, unsigned char **,
                   unsigned char *);
extern int setup_jfif_header(JFIF_HEADER **, const unsigned char,
                   const int, const int);
extern int read_jfif_header(JFIF_HEADER **, FILE *);
extern int getc_jfif_header(JFIF_HEADER **, unsigned char **, unsigned char *);
extern int write_jfif_header(JFIF_HEADER *, FILE *);
extern int putc_jfif_header(JFIF_HEADER *, unsigned char *, const int, int *);
extern int read_table_jpegl(const unsigned short, HUF_TABLE **, FILE *);
extern int getc_table_jpegl(const unsigned short, HUF_TABLE **,
                   unsigned char **, unsigned char *);
extern int setup_frame_header_jpegl(FRM_HEADER_JPEGL **, IMG_DAT *);
extern int read_frame_header_jpegl(FRM_HEADER_JPEGL **, FILE *);
extern int getc_frame_header_jpegl(FRM_HEADER_JPEGL **, unsigned char **,
                   unsigned char *);
extern int write_frame_header_jpegl(FRM_HEADER_JPEGL *, FILE *);
extern int putc_frame_header_jpegl(FRM_HEADER_JPEGL *, unsigned char *,
                   const int, int *);
extern int setup_scan_header(SCN_HEADER **, IMG_DAT *, const int);
extern int read_scan_header(SCN_HEADER **, FILE *);
extern int getc_scan_header(SCN_HEADER **, unsigned char **, unsigned char *);
extern int write_scan_header(SCN_HEADER *, FILE *);
extern int putc_scan_header(SCN_HEADER *, unsigned char *, const int, int *);
extern int read_comment(unsigned char **, FILE *);
extern int getc_comment(unsigned char **, unsigned char **, unsigned char *);
extern int write_comment(const unsigned short, unsigned char *, const int,
                   FILE *);
extern int putc_comment(const unsigned short, unsigned char *, const int,
                   unsigned char *, const int, int *);
extern int add_comment_jpegl(unsigned char **, int *, unsigned char *,
                   const int, unsigned char *);
extern int getc_nistcom_jpegl(NISTCOM **, unsigned char *, const int);
extern int putc_nistcom_jpegl(char *, const int, const int, const int,
                   const int, const int, const int, int *, int *,
                   const int, unsigned char *, const int, int *);


/* util.c */
extern int predict(short *, unsigned char *, const int, const int, const int,
                   const int, const int);
extern short categorize(const short);

#endif /* !_JPEGL_H */
