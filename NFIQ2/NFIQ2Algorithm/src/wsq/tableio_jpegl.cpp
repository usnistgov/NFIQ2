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
      LIBRARY: JPEGL - Lossless JPEG Image Compression

      FILE:    TABLEIO.C
      AUTHORS: Craig Watson
               Michael Garris
      DATE:    12/01/1997
      UPDATED: 03/16/2005 by MDG

      Contains routines responsible for reading and writing the
      various tables and blocks used by the JPEGL (lossless)
      encoder/decoder.

      ROUTINES:
#cat: read_marker_jpegl - Read a specified JPEGL marker from 
#cat:                     an open file.
#cat: getc_marker_jpegl - Read a specified JPEGL marker from
#cat:                     a memory buffer.
#cat: setup_jfif_header - Initializes a JFIF Header for JPEGL compression
#cat:                     given pixel scan resolution attributes.
#cat: read_jfif_header - Reads a JFIF Header from an open JPEGL
#cat:                    compressed file.
#cat: getc_jfif_header - Reads a JFIF Header from a JPEGL compressed
#cat:                    memory buffer.
#cat: write_jfif_header - Writes a JFIF Header to an open file.
#cat:
#cat: putc_jfif_header - Writes a JFIF Header to a memory buffer.
#cat:
#cat: read_table_jpegl - Reads huffman tables or comment blocks from
#cat:                    an open file.
#cat: getc_table_jpegl - Reads huffman tables or comment blocks from
#cat:                    a memory buffer.
#cat: setup_frame_header_jpegl - Initializes a JPEGL Frame Header given
#cat:                    attributes of the pixmap to be compressed.
#cat: read_frame_header_jpegl - Reads a Frame Header from an open
#cat:                    JPEGL compressed file.
#cat: getc_frame_header_jpegl - Reads a Frame Header from a JPEGL
#cat:                    compressed memory buffer.
#cat: write_frame_header_jpegl - Writes a JPEGL Frame Header to
#cat:                    an open file.
#cat: putc_frame_header_jpegl - Writes a JPEGL Frame Header to
#cat:                    a memory buffer.
#cat: setup_scan_header - Initializes a JPEGL SCN Header given attributes
#cat:                    of the pixmap to be compressed.
#cat: read_scan_header - Reads a SCN Header fram an open
#cat:                    JPEGL compressed file.
#cat: getc_scan_header - Reads a SCN Header fram a JPEGL
#cat:                    compressed memory buffer.
#cat: write_scan_header - Writes a JPEGL SCN Header to an open file.
#cat:
#cat: putc_scan_header - Writes a JPEGL SCN Header to a memory buffer.
#cat:
#cat: read_comment - Reads the contents of a JPEGL comment block from
#cat:                    an open file, returning the comment text as a
#cat:                    null-terminated string.
#cat: getc_comment - Reads the contents of a JPEGL comment block from
#cat:                    a memory buffer, returning the comment text as a
#cat:                    null-terminated string.
#cat: write_comment - Writes a text string out as a JPEGL comment block
#cat:                    to an open file.
#cat: putc_comment - Writes a text string out as a JPEGL comment block
#cat:                    to a memory buffer.
#cat: add_comment_jpegl - Inserts a comment block into a preexisting JPEGL
#cat:                    datastream.
#cat: getc_nistcom_jpegl - Find and return the first NISTCOM comment block
#cat:                    from a JPEGL encoded datastream.
#cat: putc_nistcom_jpegl - Generate a JPEGL NISTCOM comment from the
#cat:                    attributes passed, and writes the NISTCOM and
#cat:                    possibly a general comment to a memory buffer.

***********************************************************************/

#include <stdio.h>
#include <string.h>
#include <jpegl.h>
#include <computil.h>
#include <dataio.h>

/************************************/
/* Get markers from compressed file */
/************************************/
int read_marker_jpegl(unsigned short *omarker, const int type, FILE *infp)
{
   int ret;
   unsigned short marker;

   if((ret = read_ushort(&marker, infp)))
      return(ret);

   if(debug > 0)
      fprintf(stdout, "Read Marker = %d, type %d\n", marker, type);

   switch(type){
   case SOI:
      if(marker != SOI) {
         fprintf(stderr,
         "ERROR : read_marker_jpegl : No SOI marker. {%d}\n", marker);
         return(-2);
      }
      break;
   case APP0:
      if(marker != APP0) {
         fprintf(stderr,
         "ERROR : read_marker_jpegl : No APP0 (JFIF) marker. {%d}\n", marker);
         return(-3);
      }
      break;
   case TBLS_N_SOF:
      if(marker != DHT && marker != COM && marker != SOF3 ) {
         fprintf(stderr, "ERROR : read_marker_jpegl : ");
         fprintf(stderr, "No DHT, COM, or SOF3 markers.\n");
         return(-4);
      }
      break;
   case TBLS_N_SOS:
      if(marker != DHT && marker != COM && marker != SOS ) {
         fprintf(stderr, "ERROR : read_marker_jpegl : ");
         fprintf(stderr, "No DHT, COM, or SOS markers.\n");
         return(-5);
      }
      break;
   case ANY:
      if((marker & 0xff00) != 0xff00){
	fprintf(stderr,"ERROR : read_marker_jpegl : no marker found {%04X}\n",
                marker);
         return(-6);
      }
      break;
   default:
      fprintf(stderr,
      "ERROR : read_marker_jpegl : Invalid marker -> {%4X}\n", marker);
      return(-6);
   }

   *omarker =  marker;
   return(0);
}

/*********************************************/
/* Get markers from compressed memory buffer */
/*********************************************/
int getc_marker_jpegl(unsigned short *omarker, const int type,
                unsigned char **cbufptr, unsigned char *ebufptr)
{
   int ret;
   unsigned short marker;

   if((ret = getc_ushort(&marker, cbufptr, ebufptr)))
      return(ret);

   if(debug > 0)
      fprintf(stdout, "Read Marker = %d, type %d\n", marker, type);

   switch(type){
   case SOI:
      if(marker != SOI) {
         fprintf(stderr,
         "ERROR : getc_marker_jpegl : No SOI marker. {%d}\n", marker);
         return(-2);
      }
      break;
   case APP0:
      if(marker != APP0) {
         fprintf(stderr,
         "ERROR : getc_marker_jpegl : No APP0 (JFIF) marker. {%d}\n", marker);
         return(-3);
      }
      break;
   case TBLS_N_SOF:
      if(marker != DHT && marker != COM && marker != SOF3 ) {
         fprintf(stderr, "ERROR : getc_marker_jpegl : ");
         fprintf(stderr, "No DHT, COM, or SOF3 markers.\n");
         return(-4);
      }
      break;
   case TBLS_N_SOS:
      if(marker != DHT && marker != COM && marker != SOS ) {
         fprintf(stderr, "ERROR : getc_marker_jpegl : ");
         fprintf(stderr, "No DHT, COM, or SOS markers.\n");
         return(-5);
      }
      break;
   case ANY:
      if((marker & 0xff00) != 0xff00){
	fprintf(stderr,"ERROR : getc_marker_jpegl : no marker found {%04X}\n",
                marker);
         return(-6);
      }
      break;
   default:
      fprintf(stderr,
      "ERROR : getc_marker_jpegl : Invalid marker -> {%4X}\n", marker);
      return(-6);
   }

   *omarker =  marker;
   return(0);
}

/***********************/
/* Setup a JFIF header */
/***********************/
int setup_jfif_header(JFIF_HEADER **ojfif_header,
                      const unsigned char units, const int dx, const int dy)
{
   JFIF_HEADER *jfif_header;

   jfif_header = (JFIF_HEADER *)calloc(1, sizeof(JFIF_HEADER));
   if(jfif_header == (JFIF_HEADER *)NULL){
      fprintf(stderr, "ERROR : setup_jfif_header : calloc : jfif_header\n");
      return(-2);
   }
   sprintf(jfif_header->ident, "%s", JFIF_IDENT);
   jfif_header->ver = JFIF_VERSION;

   /* Trap for special case where dx and dy == -1 ==> unknown. */
   if((dx == -1) || (dy == -1)){
      jfif_header->units = UNKNOWN_UNITS;
      jfif_header->dx = 0;
      jfif_header->dy = 0;
   }
   else{
      jfif_header->units = units;
      jfif_header->dx = dx;
      jfif_header->dy = dy;
   }
   jfif_header->tx = 0;
   jfif_header->ty = 0;
   
   *ojfif_header = jfif_header;
   return(0);
}

/*****************************************/
/* Reads JFIF table from compressed file */
/*****************************************/
int read_jfif_header(JFIF_HEADER **ojfif_header, FILE *infp)
{
   int ret, i;
   JFIF_HEADER *jfif_header;
   unsigned short table_len;

   if(debug > 0)
      fprintf(stdout, "Start reading JFIF header.\n");

   jfif_header = (JFIF_HEADER *)malloc(sizeof(JFIF_HEADER));
   if(jfif_header == (JFIF_HEADER *)NULL){
      fprintf(stderr, "ERROR : read_jfif_header : malloc : jfif_header\n");
      return(-2);
   }

   /* table_len */
   if((ret = read_ushort(&table_len, infp))){
      free(jfif_header);
      return(-3);
   }

   /* Ident */
   for(i = 0; i < JFIF_IDENT_LEN; i++){
      if((ret = read_byte((unsigned char *)&(jfif_header->ident[i]), infp))){
         free(jfif_header);
         return(-4);
      }
   }

   if(strcmp(jfif_header->ident, JFIF_IDENT) != 0){
      fprintf(stderr, "ERROR : read_jfif_header : Not a JFIF Header\n");
      free(jfif_header);
      return(-5);
   }

   if((ret = read_ushort(&(jfif_header->ver), infp))){
      free(jfif_header);
      return(-6);
   }
   if((ret = read_byte(&(jfif_header->units), infp))){
      free(jfif_header);
      return(-7);
   }
   if((ret = read_ushort(&(jfif_header->dx), infp))){
      free(jfif_header);
      return(-8);
   }
   if((ret = read_ushort(&(jfif_header->dy), infp))){
      free(jfif_header);
      return(-9);
   }
   if((ret = read_byte(&(jfif_header->tx), infp))){
      free(jfif_header);
      return(-10);
   }
   if((ret = read_byte(&(jfif_header->ty), infp))){
      free(jfif_header);
      return(-11);
   }

   if(jfif_header->ty != 0 || jfif_header->tx != 0){
      fprintf(stderr, "ERROR : read_jfif_header : Can't handle thumbnails\n");
      free(jfif_header);
      return(-12);
   }

   if(debug > 1){
      fprintf(stdout, "Table Len = %d\n", table_len);
      fprintf(stdout, "Ident = %s\n", jfif_header->ident);
      fprintf(stdout, "version = %d.", (jfif_header->ver & 0xff00) >> 8);
      fprintf(stdout, "%02d\n", (jfif_header->ver & 0x00ff));
      fprintf(stdout, "units = %d\n", jfif_header->units);
      fprintf(stdout, "dx = %d\n", jfif_header->dx);
      fprintf(stdout, "dy = %d\n", jfif_header->dy);
      fprintf(stdout, "tx = %d\n", jfif_header->tx);
      fprintf(stdout, "ty = %d\n", jfif_header->ty);
   }

   if(debug > 0)
      fprintf(stdout, "Finished reading JFIF header.\n");

   *ojfif_header = jfif_header;
   return(0);
}

/**************************************************/
/* Reads JFIF table from compressed memory buffer */
/**************************************************/
int getc_jfif_header(JFIF_HEADER **ojfif_header,
                     unsigned char **cbufptr, unsigned char *ebufptr)
{
   int ret, i;
   JFIF_HEADER *jfif_header;
   unsigned short table_len;

   if(debug > 0)
      fprintf(stdout, "Start reading JFIF header.\n");

   jfif_header = (JFIF_HEADER *)malloc(sizeof(JFIF_HEADER));
   if(jfif_header == (JFIF_HEADER *)NULL){
      fprintf(stderr, "ERROR : getc_jfif_header : malloc : jfif_header\n");
      return(-2);
   }

   /* table_len */
   if((ret = getc_ushort(&table_len, cbufptr, ebufptr))){
      free(jfif_header);
      return(-3);
   }

   /* Ident */
   for(i = 0; i < JFIF_IDENT_LEN; i++){
      if((ret = getc_byte((unsigned char *)&(jfif_header->ident[i]),
                          cbufptr, ebufptr))){
         free(jfif_header);
         return(-4);
      }
   }

   if(strcmp(jfif_header->ident, JFIF_IDENT) != 0){
      fprintf(stderr, "ERROR : getc_jfif_header : Not a JFIF Header\n");
      free(jfif_header);
      return(-5);
   }

   if((ret = getc_ushort(&(jfif_header->ver), cbufptr, ebufptr))){
      free(jfif_header);
      return(-6);
   }
   if((ret = getc_byte(&(jfif_header->units), cbufptr, ebufptr))){
      free(jfif_header);
      return(-7);
   }
   if((ret = getc_ushort(&(jfif_header->dx), cbufptr, ebufptr))){
      free(jfif_header);
      return(-8);
   }
   if((ret = getc_ushort(&(jfif_header->dy), cbufptr, ebufptr))){
      free(jfif_header);
      return(-9);
   }
   if((ret = getc_byte(&(jfif_header->tx), cbufptr, ebufptr))){
      free(jfif_header);
      return(-10);
   }
   if((ret = getc_byte(&(jfif_header->ty), cbufptr, ebufptr))){
      free(jfif_header);
      return(-11);
   }

   if(jfif_header->ty != 0 || jfif_header->tx != 0){
      fprintf(stderr, "ERROR : getc_jfif_header : Can't handle thumbnails\n");
      free(jfif_header);
      return(-12);
   }

   if(debug > 1){
      fprintf(stdout, "Table Len = %d\n", table_len);
      fprintf(stdout, "Ident = %s\n", jfif_header->ident);
      fprintf(stdout, "version = %d.", (jfif_header->ver & 0xff00) >> 8);
      fprintf(stdout, "%02d\n", (jfif_header->ver & 0x00ff));
      fprintf(stdout, "units = %d\n", jfif_header->units);
      fprintf(stdout, "dx = %d\n", jfif_header->dx);
      fprintf(stdout, "dy = %d\n", jfif_header->dy);
      fprintf(stdout, "tx = %d\n", jfif_header->tx);
      fprintf(stdout, "ty = %d\n", jfif_header->ty);
   }

   if(debug > 0)
      fprintf(stdout, "Finished reading JFIF header.\n");

   *ojfif_header = jfif_header;
   return(0);
}

/******************************************/
/* Writes JFIF table into compressed file */
/******************************************/
int write_jfif_header(JFIF_HEADER *jfif_header, FILE *outfp)
{
   int table_len, i;
   int ret;

   if(debug > 0)
      fprintf(stdout, "Start writing JFIF header.\n");
   
   if(strcmp(jfif_header->ident, JFIF_IDENT) != 0){
      fprintf(stderr, "ERROR : write_jfif_header : Not a JFIF Header\n");
      return(-2);
   }

   if(jfif_header->ty != 0 || jfif_header->tx != 0){
      fprintf(stderr, "ERROR : write_jfif_header : Can't handle thumbnails\n");
      return(-3);
   }

   table_len = JFIF_HEADER_LEN;

   if(debug > 1){
      fprintf(stdout, "Table Len = %d\n", table_len);
      fprintf(stdout, "Ident = %s\n", jfif_header->ident);
      fprintf(stdout, "version = %d.", (jfif_header->ver & 0xff00) >> 8);
      fprintf(stdout, "%02d\n", (jfif_header->ver & 0x00ff));
      fprintf(stdout, "units = %d\n", jfif_header->units);
      fprintf(stdout, "dx = %d\n", jfif_header->dx);
      fprintf(stdout, "dy = %d\n", jfif_header->dy);
      fprintf(stdout, "tx = %d\n", jfif_header->tx);
      fprintf(stdout, "ty = %d\n", jfif_header->ty);
   }

   /* APP0 0xffe0 */
   if((ret = write_ushort(APP0, outfp)))
      return(ret);

   /* Table_len */
   if((ret = write_ushort(table_len, outfp)))
      return(ret);

   /* Ident */
   for(i = 0; i < JFIF_IDENT_LEN; i++)
      if((ret = write_byte(jfif_header->ident[i], outfp)))
         return(ret);

   if((ret = write_ushort(jfif_header->ver, outfp)))
      return(ret);
   if((ret = write_byte(jfif_header->units, outfp)))
      return(ret);
   if((ret = write_ushort(jfif_header->dx, outfp)))
      return(ret);
   if((ret = write_ushort(jfif_header->dy, outfp)))
      return(ret);
   if((ret = write_byte(jfif_header->tx, outfp)))
      return(ret);
   if((ret = write_byte(jfif_header->ty, outfp)))
      return(ret);

   if(debug > 0)
      fprintf(stdout, "Finished writing JFIF header.\n");

   return(0);
}

/***************************************************/
/* Writes JFIF table into compressed memory buffer */
/***************************************************/
int putc_jfif_header(JFIF_HEADER *jfif_header, unsigned char *outbuf,
                     const int outalloc, int *outlen)
{
   int table_len, i;
   int ret;

   if(debug > 0)
      fprintf(stdout, "Start writing JFIF header.\n");
   
   if(strcmp(jfif_header->ident, JFIF_IDENT) != 0){
      fprintf(stderr, "ERROR : putc_jfif_header : Not a JFIF Header\n");
      return(-2);
   }

   if(jfif_header->ty != 0 || jfif_header->tx != 0){
      fprintf(stderr, "ERROR : putc_jfif_header : Can't handle thumbnails\n");
      return(-3);
   }

   table_len = JFIF_HEADER_LEN;

   if(debug > 1){
      fprintf(stdout, "Table Len = %d\n", table_len);
      fprintf(stdout, "Ident = %s\n", jfif_header->ident);
      fprintf(stdout, "version = %d.", (jfif_header->ver & 0xff00) >> 8);
      fprintf(stdout, "%02d\n", (jfif_header->ver & 0x00ff));
      fprintf(stdout, "units = %d\n", jfif_header->units);
      fprintf(stdout, "dx = %d\n", jfif_header->dx);
      fprintf(stdout, "dy = %d\n", jfif_header->dy);
      fprintf(stdout, "tx = %d\n", jfif_header->tx);
      fprintf(stdout, "ty = %d\n", jfif_header->ty);
   }

   /* APP0 0xffe0 */
   if((ret = putc_ushort(APP0, outbuf, outalloc, outlen)))
      return(ret);

   /* Table_len */
   if((ret = putc_ushort(table_len, outbuf, outalloc, outlen)))
      return(ret);

   /* Ident */
   for(i = 0; i < JFIF_IDENT_LEN; i++)
      if((ret = putc_byte(jfif_header->ident[i], outbuf, outalloc, outlen)))
         return(ret);

   if((ret = putc_ushort(jfif_header->ver, outbuf, outalloc, outlen)))
      return(ret);
   if((ret = putc_byte(jfif_header->units, outbuf, outalloc, outlen)))
      return(ret);
   if((ret = putc_ushort(jfif_header->dx, outbuf, outalloc, outlen)))
      return(ret);
   if((ret = putc_ushort(jfif_header->dy, outbuf, outalloc, outlen)))
      return(ret);
   if((ret = putc_byte(jfif_header->tx, outbuf, outalloc, outlen)))
      return(ret);
   if((ret = putc_byte(jfif_header->ty, outbuf, outalloc, outlen)))
      return(ret);

   if(debug > 0)
      fprintf(stdout, "Finished writing JFIF header.\n");

   return(0);
}

/********************************************************/
/* Reads Huffman table or comment from compressed file. */
/********************************************************/
int read_table_jpegl(const unsigned short marker, HUF_TABLE **huf_table,
                     FILE *infp)
{
   int ret;
   unsigned char *comment;

   switch(marker){
   case DHT:
      if((ret = read_huffman_table_jpegl(huf_table, infp)))
         return(ret);
      break;
   case COM:
      if((ret = read_comment(&comment, infp)))
         return(ret);
#ifdef PRINT_COMMENT
      fprintf(stderr, "COMMENT:\n%s\n\n", comment);
#endif
      free(comment);
      break;
   default:
      fprintf(stderr,
              "ERROR: read_table_jpegl : Invalid table defined -> {%u}\n",
              marker);
      return(-2);
   }

   return(0);
}

/*****************************************************************/
/* Reads Huffman table or comment from compressed memory buffer. */
/*****************************************************************/
int getc_table_jpegl(const unsigned short marker, HUF_TABLE **huf_table,
                     unsigned char **cbufptr, unsigned char *ebufptr)
{
   int ret;
   unsigned char *comment;

   switch(marker){
   case DHT:
      if((ret = getc_huffman_table_jpegl(huf_table, cbufptr, ebufptr)))
         return(ret);
      break;
   case COM:
      if((ret = getc_comment(&comment, cbufptr, ebufptr)))
         return(ret);
#ifdef PRINT_COMMENT
      fprintf(stderr, "COMMENT:\n%s\n\n", comment);
#endif
      free(comment);
      break;
   default:
      fprintf(stderr,
              "ERROR: getc_table_jpegl : Invalid table defined -> {%u}\n",
              marker);
      return(-2);
   }

   return(0);
}

/********************************************/
/* Sets frame header values from image data */
/********************************************/
int setup_frame_header_jpegl(FRM_HEADER_JPEGL **ofrm_header, IMG_DAT *img_dat)
{
   int i;
   FRM_HEADER_JPEGL *frm_header;

   frm_header = (FRM_HEADER_JPEGL *)malloc(sizeof(FRM_HEADER_JPEGL));
   if(frm_header == (FRM_HEADER_JPEGL *)NULL){
      fprintf(stderr, "ERROR : setup_frame_header_jpegl : ");
      fprintf(stderr, "malloc : frm_header\n");
      return(-2);
   }

   frm_header->Nf = img_dat->n_cmpnts;
   frm_header->prec = img_dat->cmpnt_depth;  /* 8 ==> uchar components */
   frm_header->x = img_dat->max_width;
   frm_header->y = img_dat->max_height;

   for(i = 0; i < frm_header->Nf; i++) {
      frm_header->C[i] = i;
      frm_header->HV[i] = (img_dat->hor_sampfctr[i]<<4) |
                          img_dat->vrt_sampfctr[i];
      frm_header->Tq[i] = 0;
   }

   *ofrm_header = frm_header;
   return(0);
}


/*******************************************/
/* Reads frame header from compressed file */
/*******************************************/
int read_frame_header_jpegl(FRM_HEADER_JPEGL **ofrm_header, FILE *infp)
{
   int ret, i;
   unsigned short Lf;
   FRM_HEADER_JPEGL *frm_header;

   if(debug > 0)
      fprintf(stdout, "Start reading frame header.\n");

   frm_header = (FRM_HEADER_JPEGL *)malloc(sizeof(FRM_HEADER_JPEGL));
   if(frm_header == (FRM_HEADER_JPEGL *)NULL){
      fprintf(stderr, "ERROR : read_frame_header_jpegl : ");
      fprintf(stderr, "malloc : frm_header\n");
      return(-2);
   }

   /* Lf */
   if((ret = read_ushort(&Lf, infp))){
      free(frm_header);
      return(ret);
   }
   /* P */
   if((ret = read_byte(&(frm_header->prec), infp))){
      free(frm_header);
      return(ret);
   }
   /* Y */
   if((ret = read_ushort(&(frm_header->y), infp))){
      free(frm_header);
      return(ret);
   }
   /* X */
   if((ret = read_ushort(&(frm_header->x), infp))){
      free(frm_header);
      return(ret);
   }
   /* Nf */
   if((ret = read_byte(&(frm_header->Nf), infp))){
      free(frm_header);
      return(ret);
   }

   for(i = 0; i < frm_header->Nf; i++) {
      /* C */
      if((ret = read_byte(&(frm_header->C[i]), infp))){
         free(frm_header);
         return(ret);
      }
      /* HV */
      if((ret = read_byte(&(frm_header->HV[i]), infp))){
         free(frm_header);
         return(ret);
      }
      /* Tq */
      if((ret = read_byte(&(frm_header->Tq[i]), infp))){
         free(frm_header);
         return(ret);
      }
   }

   if(debug > 1){
      fprintf(stdout, "Lf = %d\n", Lf);
      fprintf(stdout, "P = %d\n", frm_header->prec);
      fprintf(stdout, "Y = %d\n", frm_header->y);
      fprintf(stdout, "X = %d\n", frm_header->x);
      fprintf(stdout, "Nf = %d\n", frm_header->Nf);
      for(i = 0; i < frm_header->Nf; i++) {
         fprintf(stdout, "C[%d] = %d\n", i, frm_header->C[i]);
         fprintf(stdout, "HV[%d] = %d\n", i, frm_header->HV[i]);
         fprintf(stdout, "Tq[%d] = %d\n", i, frm_header->Tq[i]);
      }
   }

   if(debug > 0)
      fprintf(stdout, "Finished frame frame header.\n\n");

   *ofrm_header = frm_header;
   return(0);
}

/****************************************************/
/* Reads frame header from compressed memory buffer */
/****************************************************/
int getc_frame_header_jpegl(FRM_HEADER_JPEGL **ofrm_header,
                      unsigned char **cbufptr, unsigned char *ebufptr)
{
   int ret, i;
   unsigned short Lf;
   FRM_HEADER_JPEGL *frm_header;

   if(debug > 0)
      fprintf(stdout, "Start reading frame header.\n");

   frm_header = (FRM_HEADER_JPEGL *)malloc(sizeof(FRM_HEADER_JPEGL));
   if(frm_header == (FRM_HEADER_JPEGL *)NULL){
      fprintf(stderr, "ERROR : getc_frame_header_jpegl : ");
      fprintf(stderr, "malloc : frm_header\n");
      return(-2);
   }

   /* Lf */
   if((ret = getc_ushort(&Lf, cbufptr, ebufptr))){
      free(frm_header);
      return(ret);
   }
   /* P */
   if((ret = getc_byte(&(frm_header->prec), cbufptr, ebufptr))){
      free(frm_header);
      return(ret);
   }
   /* Y */
   if((ret = getc_ushort(&(frm_header->y), cbufptr, ebufptr))){
      free(frm_header);
      return(ret);
   }
   /* X */
   if((ret = getc_ushort(&(frm_header->x), cbufptr, ebufptr))){
      free(frm_header);
      return(ret);
   }
   /* Nf */
   if((ret = getc_byte(&(frm_header->Nf), cbufptr, ebufptr))){
      free(frm_header);
      return(ret);
   }

   for(i = 0; i < frm_header->Nf; i++) {
      /* C */
      if((ret = getc_byte(&(frm_header->C[i]), cbufptr, ebufptr))){
         free(frm_header);
         return(ret);
      }
      /* HV */
      if((ret = getc_byte(&(frm_header->HV[i]), cbufptr, ebufptr))){
         free(frm_header);
         return(ret);
      }
      /* Tq */
      if((ret = getc_byte(&(frm_header->Tq[i]), cbufptr, ebufptr))){
         free(frm_header);
         return(ret);
      }
   }

   if(debug > 1){
      fprintf(stdout, "Lf = %d\n", Lf);
      fprintf(stdout, "P = %d\n", frm_header->prec);
      fprintf(stdout, "Y = %d\n", frm_header->y);
      fprintf(stdout, "X = %d\n", frm_header->x);
      fprintf(stdout, "Nf = %d\n", frm_header->Nf);
      for(i = 0; i < frm_header->Nf; i++) {
         fprintf(stdout, "C[%d] = %d\n", i, frm_header->C[i]);
         fprintf(stdout, "HV[%d] = %d\n", i, frm_header->HV[i]);
         fprintf(stdout, "Tq[%d] = %d\n", i, frm_header->Tq[i]);
      }
   }

   if(debug > 0)
      fprintf(stdout, "Finished frame frame header.\n\n");

   *ofrm_header = frm_header;
   return(0);
}


/**********************************************/
/* Writes frame header to the compressed file */
/**********************************************/
int write_frame_header_jpegl(FRM_HEADER_JPEGL *frm_header, FILE *outfp)
{
   int i, ret;

   if(debug > 0)
      fprintf(stdout, "Start writing frame header.\n");

   if(debug > 1){
      fprintf(stdout, "Lf = %d\n", 8+(3*frm_header->Nf));
      fprintf(stdout, "P = %d\n", frm_header->prec);
      fprintf(stdout, "Y = %d\n", frm_header->y);
      fprintf(stdout, "X = %d\n", frm_header->x);
      fprintf(stdout, "Nf = %d\n", frm_header->Nf);
      for(i = 0; i < frm_header->Nf; i++) {
         fprintf(stdout, "C[%d] = %d\n", i, frm_header->C[i]);
         fprintf(stdout, "HV[%d] = %d\n", i, frm_header->HV[i]);
         fprintf(stdout, "Tq[%d] = %d\n", i, frm_header->Tq[i]);
      }
   }

   /* SOF3 */
   if((ret = write_ushort(SOF3, outfp)))
      return(ret);
   /* Lf */
   if((ret = write_ushort((8+(3*frm_header->Nf)), outfp)))
      return(ret);
   /* P */
   if((ret = write_byte(frm_header->prec, outfp)))
      return(ret);
   /* Y */
   if((ret = write_ushort(frm_header->y, outfp)))
      return(ret);
   /* X */
   if((ret = write_ushort(frm_header->x, outfp)))
      return(ret);
   /* Nf */
   if((ret = write_byte(frm_header->Nf, outfp)))
      return(ret);

   for(i = 0; i < frm_header->Nf; i++) {
      /* C */
      if((ret = write_byte(frm_header->C[i], outfp)))
         return(ret);
      /* HV */
      if((ret = write_byte(frm_header->HV[i], outfp)))
         return(ret);
      /* Tq */
      if((ret = write_byte(frm_header->Tq[i], outfp)))
         return(ret);
   }

   if(debug > 0)
      fprintf(stdout, "Finished writing frame header.\n\n");

   return(0);
}

/*******************************************************/
/* Writes frame header to the compressed memory buffer */
/*******************************************************/
int putc_frame_header_jpegl(FRM_HEADER_JPEGL *frm_header, unsigned char *outbuf,
                       const int outalloc, int *outlen)
{
   int i, ret;

   if(debug > 0)
      fprintf(stdout, "Start writing frame header.\n");

   if(debug > 1){
      fprintf(stdout, "Lf = %d\n", 8+(3*frm_header->Nf));
      fprintf(stdout, "P = %d\n", frm_header->prec);
      fprintf(stdout, "Y = %d\n", frm_header->y);
      fprintf(stdout, "X = %d\n", frm_header->x);
      fprintf(stdout, "Nf = %d\n", frm_header->Nf);
      for(i = 0; i < frm_header->Nf; i++) {
         fprintf(stdout, "C[%d] = %d\n", i, frm_header->C[i]);
         fprintf(stdout, "HV[%d] = %d\n", i, frm_header->HV[i]);
         fprintf(stdout, "Tq[%d] = %d\n", i, frm_header->Tq[i]);
      }
   }

   /* SOF3 */
   if((ret = putc_ushort(SOF3, outbuf, outalloc, outlen)))
      return(ret);
   /* Lf */
   if((ret = putc_ushort((8+(3*frm_header->Nf)), outbuf, outalloc, outlen)))
      return(ret);
   /* P */
   if((ret = putc_byte(frm_header->prec, outbuf, outalloc, outlen)))
      return(ret);
   /* Y */
   if((ret = putc_ushort(frm_header->y, outbuf, outalloc, outlen)))
      return(ret);
   /* X */
   if((ret = putc_ushort(frm_header->x, outbuf, outalloc, outlen)))
      return(ret);
   /* Nf */
   if((ret = putc_byte(frm_header->Nf, outbuf, outalloc, outlen)))
      return(ret);

   for(i = 0; i < frm_header->Nf; i++) {
      /* C */
      if((ret = putc_byte(frm_header->C[i], outbuf, outalloc, outlen)))
         return(ret);
      /* HV */
      if((ret = putc_byte(frm_header->HV[i], outbuf, outalloc, outlen)))
         return(ret);
      /* Tq */
      if((ret = putc_byte(frm_header->Tq[i], outbuf, outalloc, outlen)))
         return(ret);
   }

   if(debug > 0)
      fprintf(stdout, "Finished writing frame header.\n\n");

   return(0);
}

/*********************************************/
/* Writes scan header to the compressed file */
/*********************************************/
int setup_scan_header(SCN_HEADER **oscn_header, IMG_DAT *img_dat,
                      const int cmpnt_i)
{
   int i;
   SCN_HEADER *scn_header;

   scn_header = (SCN_HEADER *)malloc(sizeof(SCN_HEADER));
   if(scn_header == (SCN_HEADER *)NULL){
      fprintf(stderr, "ERROR : setup_scan_header : malloc : scn_header\n");
      return(-2);
   }

   /* When nonintrlv, one scan header is setup and written out for */
   /* each component plane. */
   if(!(img_dat->intrlv)) {
      scn_header->Ns = 1;
      scn_header->Cs[0] = cmpnt_i;
      scn_header->Tda[0] = cmpnt_i<<4;
      scn_header->Ahl = img_dat->point_trans[cmpnt_i];
      scn_header->Ss = img_dat->predict[cmpnt_i];
      scn_header->Se = 0;
   }
   /* When intrlv, one scan header is setup and written out for */
   /* ALL components. */
   else {
      scn_header->Ns = img_dat->n_cmpnts;
      for(i = 0; i < img_dat->n_cmpnts; i++) {
         scn_header->Cs[i] = i;
         scn_header->Tda[i] = i<<4;
      }
      /* One point transform applies to all interleaved components, */
      /* so choose 1st one. */
      scn_header->Ahl = img_dat->point_trans[0];
      /* One predictor applies to all interleaved components, */
      /* so choose 1st one. */
      scn_header->Ss = img_dat->predict[0];
      scn_header->Se = 0;
   }

   *oscn_header = scn_header;
   return(0);
}

/******************************************/
/* Reads scan header from compressed file */
/******************************************/
int read_scan_header(SCN_HEADER **oscn_header, FILE *infp)
{
   int ret, i;
   unsigned short Ls;
   SCN_HEADER *scn_header;

   if(debug > 0)
      fprintf(stdout, "Start reading scan header\n");

   scn_header = (SCN_HEADER *)malloc(sizeof(SCN_HEADER));
   if(scn_header == (SCN_HEADER *)NULL){
      fprintf(stderr, "ERROR : read_scan_header : malloc : scn_header\n");
      return(-2);
   }

   /* Ls */
   if((ret = read_ushort(&Ls, infp))){
      free(scn_header);
      return(ret);
   }
   /* Ns */
   if((ret = read_byte(&(scn_header->Ns), infp))){
      free(scn_header);
      return(ret);
   }

   for(i = 0; i < scn_header->Ns; i++) {
      /* Cs */
      if((ret = read_byte(&(scn_header->Cs[i]), infp))){
         free(scn_header);
         return(ret);
      }
      /* Tda */
      if((ret = read_byte(&(scn_header->Tda[i]), infp))){
         free(scn_header);
         return(ret);
      }
      scn_header->Tda[i] = scn_header->Tda[i] >> 4;
   }

   /* Ss */
   if((ret = read_byte(&(scn_header->Ss), infp))){
      free(scn_header);
      return(ret);
   }
   /* Se */
   if((ret = read_byte(&(scn_header->Se), infp))){
      free(scn_header);
      return(ret);
   }
   /* Ahl */
   if((ret = read_byte(&(scn_header->Ahl), infp))){
      free(scn_header);
      return(ret);
   }

   if(debug > 1){
      fprintf(stdout, "Ls = %d\n", Ls);
      fprintf(stdout, "Ns = %d\n", scn_header->Ns);

      for(i = 0; i < scn_header->Ns; i++) {
         fprintf(stdout, "Cs[%d] = %d\n", i, scn_header->Cs[i]);
         fprintf(stdout, "Tda[%d] = %d\n", i, scn_header->Tda[i]);
      }

      fprintf(stdout, "Ss = %d\n", scn_header->Ss);
      fprintf(stdout, "Se = %d\n", scn_header->Se);
      fprintf(stdout, "Ahl = %d\n", scn_header->Ahl);
   }

   if(debug > 0)
      fprintf(stdout, "Finished reading scan header\n");


   *oscn_header = scn_header;
   return(0);
}

/***************************************************/
/* Reads scan header from compressed memory buffer */
/***************************************************/
int getc_scan_header(SCN_HEADER **oscn_header, unsigned char **cbufptr,
         unsigned char *ebufptr)
{
   int ret, i;
   unsigned short Ls;
   SCN_HEADER *scn_header;

   if(debug > 0)
      fprintf(stdout, "Start reading scan header\n");

   scn_header = (SCN_HEADER *)malloc(sizeof(SCN_HEADER));
   if(scn_header == (SCN_HEADER *)NULL){
      fprintf(stderr, "ERROR : getc_scan_header : malloc : scn_header\n");
      return(-2);
   }

   /* Ls */
   if((ret = getc_ushort(&Ls, cbufptr, ebufptr))){
      free(scn_header);
      return(ret);
   }
   /* Ns */
   if((ret = getc_byte(&(scn_header->Ns), cbufptr, ebufptr))){
      free(scn_header);
      return(ret);
   }

   for(i = 0; i < scn_header->Ns; i++) {
      /* Cs */
      if((ret = getc_byte(&(scn_header->Cs[i]), cbufptr, ebufptr))){
         free(scn_header);
         return(ret);
      }
      /* Tda */
      if((ret = getc_byte(&(scn_header->Tda[i]), cbufptr, ebufptr))){
         free(scn_header);
         return(ret);
      }
      scn_header->Tda[i] = scn_header->Tda[i] >> 4;
   }

   /* Ss */
   if((ret = getc_byte(&(scn_header->Ss), cbufptr, ebufptr))){
      free(scn_header);
      return(ret);
   }
   /* Se */
   if((ret = getc_byte(&(scn_header->Se), cbufptr, ebufptr))){
      free(scn_header);
      return(ret);
   }
   /* Ahl */
   if((ret = getc_byte(&(scn_header->Ahl), cbufptr, ebufptr))){
      free(scn_header);
      return(ret);
   }

   if(debug > 1){
      fprintf(stdout, "Ls = %d\n", Ls);
      fprintf(stdout, "Ns = %d\n", scn_header->Ns);

      for(i = 0; i < scn_header->Ns; i++) {
         fprintf(stdout, "Cs[%d] = %d\n", i, scn_header->Cs[i]);
         fprintf(stdout, "Tda[%d] = %d\n", i, scn_header->Tda[i]);
      }

      fprintf(stdout, "Ss = %d\n", scn_header->Ss);
      fprintf(stdout, "Se = %d\n", scn_header->Se);
      fprintf(stdout, "Ahl = %d\n", scn_header->Ahl);
   }

   if(debug > 0)
      fprintf(stdout, "Finished reading scan header\n");


   *oscn_header = scn_header;
   return(0);
}

/*********************************************/
/* Writes scan header to the compressed file */
/*********************************************/
int write_scan_header(SCN_HEADER *scn_header, FILE *outfp)
{
   int i, ret;

   if(debug > 0)
      fprintf(stdout, "Start writing scan header\n");

   if(debug > 1){
      fprintf(stdout, "Ls = %d\n", 6+(2*scn_header->Ns));
      fprintf(stdout, "Ns = %d\n", scn_header->Ns);

      for(i = 0; i < scn_header->Ns; i++) {
         fprintf(stdout, "Cs[%d] = %d\n", i, scn_header->Cs[i]);
         fprintf(stdout, "Tda[%d] = %d\n", i, scn_header->Tda[i]);
      }

      fprintf(stdout, "Ss = %d\n", scn_header->Ss);
      fprintf(stdout, "Se = %d\n", scn_header->Se);
      fprintf(stdout, "Ahl = %d\n", scn_header->Ahl);
   }

   /* SOS */
   if((ret = write_ushort(SOS, outfp)))
      return(ret);
   /* Ls */
   if((ret = write_ushort((6+(2*scn_header->Ns)), outfp)))
      return(ret);
   /* Ns */
   if((ret = write_byte(scn_header->Ns, outfp)))
      return(ret);

   for(i = 0; i < scn_header->Ns; i++) {
      /* Cs */
      if((ret = write_byte(scn_header->Cs[i], outfp)))
         return(ret);
      /* Tda */
      if((ret = write_byte(scn_header->Tda[i], outfp)))
         return(ret);
   }

   /* Ss */
   if((ret = write_byte(scn_header->Ss, outfp)))
      return(ret);
   /* Se */
   if((ret = write_byte(scn_header->Se, outfp)))
      return(ret);
   /* Ahl */
   if((ret = write_byte(scn_header->Ahl, outfp)))
      return(ret);

   if(debug > 0)
      fprintf(stdout, "Finished writing scan header\n");

   return(0);
}

/******************************************************/
/* Writes scan header to the compressed memory buffer */
/******************************************************/
int putc_scan_header(SCN_HEADER *scn_header, unsigned char *outbuf,
                     const int outalloc, int *outlen)
{
   int i, ret;

   if(debug > 0)
      fprintf(stdout, "Start writing scan header\n");

   if(debug > 1){
      fprintf(stdout, "Ls = %d\n", 6+(2*scn_header->Ns));
      fprintf(stdout, "Ns = %d\n", scn_header->Ns);

      for(i = 0; i < scn_header->Ns; i++) {
         fprintf(stdout, "Cs[%d] = %d\n", i, scn_header->Cs[i]);
         fprintf(stdout, "Tda[%d] = %d\n", i, scn_header->Tda[i]);
      }

      fprintf(stdout, "Ss = %d\n", scn_header->Ss);
      fprintf(stdout, "Se = %d\n", scn_header->Se);
      fprintf(stdout, "Ahl = %d\n", scn_header->Ahl);
   }

   /* SOS */
   if((ret = putc_ushort(SOS, outbuf, outalloc, outlen)))
      return(ret);
   /* Ls */
   if((ret = putc_ushort((6+(2*scn_header->Ns)), outbuf, outalloc, outlen)))
      return(ret);
   /* Ns */
   if((ret = putc_byte(scn_header->Ns, outbuf, outalloc, outlen)))
      return(ret);

   for(i = 0; i < scn_header->Ns; i++) {
      /* Cs */
      if((ret = putc_byte(scn_header->Cs[i], outbuf, outalloc, outlen)))
         return(ret);
      /* Tda */
      if((ret = putc_byte(scn_header->Tda[i], outbuf, outalloc, outlen)))
         return(ret);
   }

   /* Ss */
   if((ret = putc_byte(scn_header->Ss, outbuf, outalloc, outlen)))
      return(ret);
   /* Se */
   if((ret = putc_byte(scn_header->Se, outbuf, outalloc, outlen)))
      return(ret);
   /* Ahl */
   if((ret = putc_byte(scn_header->Ahl, outbuf, outalloc, outlen)))
      return(ret);

   if(debug > 0)
      fprintf(stdout, "Finished writing scan header\n");

   return(0);
}

/************************************************************************/
/* Routine to read comment block from an open file.                     */
/*      NOTE: forces return of NULL termnated comment string.           */
/************************************************************************/
int read_comment(
   unsigned char **ocomment,
   FILE *infp)            /* input file */
{
   int ret, cs;
   unsigned short hdr_size;              /* header size */
   unsigned char *comment;

   if(debug > 0)
      fprintf(stderr, "Reading Comment Field.\n");

   if((ret = read_ushort(&hdr_size, infp)))
      return(ret);

   /* cs = hdr_size - sizeof(length value) */
   cs = hdr_size - 2;

   /* Allocate including a possible NULL terminator. */
   comment = (unsigned char *)calloc(cs+1, sizeof(unsigned char));
   if(comment == (unsigned char *)NULL){
      fprintf(stderr, "ERROR : read_comment : malloc : comment\n");
      return(-2);
   }

   /* Read only the number of bytes as specified in the header length. */
   if((ret = fread(comment, sizeof(unsigned char), cs, infp)) != cs){
      fprintf(stderr,
              "ERROR : read_comment : fread : only %d of %d bytes read\n",
              ret, cs);
      free(comment);
      return(-3);
   }

   /* If comment did not explicitly contain a NULL terminator, it will */
   /* have one here by default due to the calloc of one extra byte at  */
   /* the end. */

   if(debug > 0)
      fprintf(stderr, "Comment =  %s", comment);

   *ocomment = comment;
   return(0);
}

/************************************************************************/
/* Routine to read comment block from a memory buffer.                  */
/*      NOTE: forces return of NULL termnated comment string.           */
/************************************************************************/
int getc_comment(
   unsigned char **ocomment,
   unsigned char **cbufptr,  /* current byte in input buffer */
   unsigned char *ebufptr)   /* end of input buffer */
{
   int ret, cs;
   unsigned short hdr_size;              /* header size */
   unsigned char *comment;

   if(debug > 0)
      fprintf(stderr, "Reading Comment Field.\n");

   if((ret = getc_ushort(&hdr_size, cbufptr, ebufptr)))
      return(ret);

   /* cs = hdr_size - sizeof(length value) */
   cs = hdr_size - 2;

   /* Allocate including a possible NULL terminator. */
   comment = (unsigned char *)calloc(cs+1, sizeof(unsigned char));
   if(comment == (unsigned char *)NULL){
      fprintf(stderr, "ERROR : getc_comment : malloc : comment\n");
      return(-2);
   }

   /* Read only the number of bytes as specified in the header length. */
   if((ret = getc_bytes(&comment, cs, cbufptr, ebufptr))){
      free(comment);
      return(ret);
   }

   /* If comment did not explicitly contain a NULL terminator, it will */
   /* have one here by default due to the calloc of one extra byte at  */
   /* the end. */

   if(debug > 0)
      fprintf(stderr, "Comment =  %s", comment);

   *ocomment = comment;
   return(0);
}

/***********************************/
/* Routine to write comment field. */
/***********************************/
int write_comment(
   const unsigned short marker,
   unsigned char *comment,            /* comment */
   const int cs,          /* comment size */
   FILE *outfp)            /* output file */
{
   int ret;
   unsigned short hdr_size;              /* header size */

   if(debug > 0)
      fprintf(stderr, "Writing Comment Field.\n");

   if((ret = write_ushort(marker, outfp)))
      return(ret);
   /* comment size */
   hdr_size = 2 + cs;
   if((ret = write_ushort(hdr_size, outfp)))
      return(ret);

   if((ret = fwrite(comment, cs, sizeof(unsigned char), outfp)) != cs){
     fprintf(stderr,
             "ERROR : write_comment : fwrite : only %d of %d bytes written\n",
             ret, cs);
     return(-2);
   }

   if(debug > 0)
      fprintf(stderr, "Finished Writing Comment Field.\n");

   return(0);
}


/****************************************/
/* Puts comment field in output buffer. */
/****************************************/
int putc_comment(
   const unsigned short marker,
   unsigned char *comment,        /* comment */
   const int cs,      /* comment size */
   unsigned char *odata,      /* output byte buffer       */
   const int oalloc,  /* allocated size of buffer */
   int   *olen)       /* filled length of buffer  */
{
   int ret, i;
   unsigned short hdr_size;              /* header size */

   if(debug > 0)
      fprintf(stderr, "Writing Comment Field to Buffer.\n");

   if((ret = putc_ushort(marker, odata, oalloc, olen)))
      return(ret);
   /* comment size */
   hdr_size = 2 + cs;
   if((ret = putc_ushort(hdr_size, odata, oalloc, olen)))
      return(ret);
   for(i = 0; i < cs; i++)
      if((ret = putc_byte(comment[i], odata, oalloc, olen)))
         return(ret);

   if(debug > 0)
      fprintf(stderr, "Finished Writing Comment Field to Buffer.\n");

   return(0);
}

/*******************************************/
int add_comment_jpegl(unsigned char **ocdata, int *oclen, unsigned char *idata,
                      const int ilen, unsigned char *comment)
{
   int ret, nlen, nalloc;
   unsigned short marker;
   unsigned char *ndata, *cbufptr, *ebufptr;
   unsigned char *ocomment;
   JFIF_HEADER *jfif_header;

   if((comment == (unsigned char *)NULL) ||
      (strlen((char *)comment) == 0)){
      fprintf(stderr,
             "ERROR : add_comment_jpegl : empty comment passed\n");
      return(-2);
   }

   /* New compressed byte stream length including:                */
   /*    orig byte strem + marker + header length + comment bytes */
   nalloc = ilen + (sizeof(unsigned short) << 1) + strlen((char *)comment);
   /* Initialize current filled length to 0. */
   nlen = 0;

   /* Allocate new compressed byte stream. */
   if((ndata = (unsigned char *)malloc(nalloc * sizeof(unsigned char))) ==
                                       (unsigned char *)NULL){
      fprintf(stderr, "ERROR : add_comment_jpegl : malloc : ndata\n");
      return(-3);
   }
   cbufptr = idata;
   ebufptr = idata + ilen;

   /* Parse idata and determine comment destination in byte stream. */
   
   /* Parse SOI */
   if((ret = getc_marker_jpegl(&marker, SOI, &cbufptr, ebufptr))){
      free(ndata);
      return(ret);
   }

   /* Copy SOI */
   if((ret = putc_ushort(marker, ndata, nalloc, &nlen))){
      free(ndata);
      return(ret);
   }

   /* Read next marker. */
   if((ret = getc_ushort(&marker, &cbufptr, ebufptr))){
      free(ndata);
      return(ret);
   }

   /* If JFIF header ... */
   if(marker == APP0){
      /* Read JFIF header. */
      if((ret = getc_jfif_header(&jfif_header, &cbufptr, ebufptr))){
         free(ndata);
         return(ret);
      }
      /* Copy JFIF header. */
      if((ret = putc_jfif_header(jfif_header, ndata, nalloc, &nlen))){
         free(ndata);
         free(jfif_header);
         return(ret);
      }
      free(jfif_header);
      /* Read next marker. */
      if((ret = getc_ushort(&marker, &cbufptr, ebufptr))){
         free(ndata);
         return(ret);
      }
   }

   /* If COM segment ... */
   if (marker == COM){
      /* Do ... while COM segments exist in input byte stream ... */
      do{
         /* Read COM segment. */
         if((ret = getc_comment(&ocomment, &cbufptr, ebufptr))){
            free(ndata);
            return(ret);
         }
         /* Copy COM segment. */
         if((ret = putc_comment(COM, ocomment, strlen((char *)ocomment),
                               ndata, nalloc, &nlen))){
            free(ndata);
            free(ocomment);
            return(ret);
         }
         free(ocomment);
         /* Read next marker. */
         if((ret = getc_ushort(&marker, &cbufptr, ebufptr))){
            free(ndata);
            return(ret);
         }
      }while(marker == COM);
   }

   /* Back up to start of last marker read. */
   cbufptr -= sizeof(unsigned short);

   /* Insert Comment Segment - includes NULL in comment string. */
   if((ret = putc_comment(COM, comment, strlen((char *)comment),
                         ndata, nalloc, &nlen))){
      free(ndata);
      return(ret);
   }

   /* Append remaining byte stream */
   if((ret = putc_bytes(cbufptr, ebufptr - cbufptr, ndata, nalloc, &nlen))){
      free(ndata);
      return(ret);
   }

   *ocdata = ndata;
   *oclen = nalloc;

   return(0);
}

/*****************************************************************/
/* Get and return first NISTCOM from encoded data stream.        */
/*****************************************************************/
int getc_nistcom_jpegl(NISTCOM **onistcom, unsigned char *idata,
                        const int ilen)
{
   int ret;
   unsigned short marker;
   unsigned char *cbufptr, *ebufptr;
   NISTCOM *nistcom;
   char *comment_text;
   unsigned char *ucomment_text;

   cbufptr = idata;
   ebufptr = idata + ilen;

   /* Get SOI */
   if((ret = getc_marker_jpegl(&marker, SOI, &cbufptr, ebufptr)))
      return(ret);

   /* Get next marker. */
   if((ret = getc_marker_jpegl(&marker, ANY, &cbufptr, ebufptr)))
      return(ret);

   /* While not at Start of Scan (SOS) -     */
   /*    the start of encoded image data ... */
   while(marker != SOS){
      if(marker == COM){
         if(strncmp((char *)cbufptr+2 /* skip Length */,
                    NCM_HEADER, strlen(NCM_HEADER)) == 0){
            if((ret = getc_comment(&ucomment_text, &cbufptr,
                                  ebufptr)))
               return(ret);
            comment_text = (char *)ucomment_text;
            if((ret = string2fet(&nistcom, comment_text)))
               return(ret);
            *onistcom = nistcom;
            return(0);
         }
      }
      /* Skip marker segment. */
      if((ret = getc_skip_marker_segment(marker, &cbufptr, ebufptr)))
         return(ret);
      /* Get next marker. */
      if((ret = getc_marker_jpegl(&marker, ANY, &cbufptr, ebufptr)))
         return(ret);
   }

   /* NISTCOM not found ... */
   *onistcom = (NISTCOM *)NULL;
   return(0);
}

/*******************************************/
int putc_nistcom_jpegl(char *comment_text, const int w, const int h,
                       const int d, const int ppi, const int lossyflag,
                       const int n_cmpnts,
                       int *hor_sampfctr, int *vrt_sampfctr,
                       const int predict,
                       unsigned char *odata, const int oalloc, int *olen)
{
   int ret, gencomflag;
   NISTCOM *nistcom;
   char *comstr;

   /* Add Comment(s) here. */
   nistcom = (NISTCOM *)NULL;
   gencomflag = 0;
   if(comment_text != (char *)NULL){
      /* if NISTCOM ... */
      if(strncmp(comment_text, NCM_HEADER, strlen(NCM_HEADER)) == 0){
         if((ret = string2fet(&nistcom, comment_text))){
            return(ret);
         }
      }
      /* If general comment ... */
      else{
         gencomflag = 1;
      }
   }
   /* Otherwise, no comment passed ... */

   /* Combine image attributes to NISTCOM. */
   if((ret = combine_jpegl_nistcom(&nistcom, w, h, d, ppi, lossyflag,
                                  n_cmpnts, hor_sampfctr, vrt_sampfctr,
                                  0 /* nonintrlv */, predict))){
      if(nistcom != (NISTCOM *)NULL)
         freefet(nistcom);
      return(ret);
   }

   /* Put NISTCOM ... */
   /* NISTCOM to string. */
   if((ret = fet2string(&comstr, nistcom))){
      freefet(nistcom);
      return(ret);
   }
   /* Put NISTCOM comment string. */
   if((ret = putc_comment(COM, (unsigned char *)comstr, strlen((char *)comstr),
                          odata, oalloc, olen))){
      freefet(nistcom);
      free(comstr);
      return(ret);
   }
   freefet(nistcom);
   free(comstr);

   /* If general comment exists ... */
   if(gencomflag){
      /* Put general comment to its own segment. */
      if((ret = putc_comment(COM, (unsigned char *)comment_text,
                             strlen((char *)comment_text),
                             odata, oalloc, olen)))
         return(ret);
   }

   return(0);
}
