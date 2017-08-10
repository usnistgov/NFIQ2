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

      FILE:    HUFTABLE.C
      AUTHORS: Craig Watson
               Michael Garris
      DATE:    12/01/2000
      UPDATED: 03/16/2005 by MDG

      Contains routines responsible for processing huffman tables
      used for JPEGL (lossless) image compression.

      ROUTINES:
#cat: gen_huff_tables - Given frequency of difference categories, generates
#cat:                   huffman tables for use with JPEGL compression.
#cat: read_huffman_table_jpegl - Reads the next huffman table from an
#cat:                   open JPEGL compressed file.
#cat: getc_huffman_table_jpegl - Reads the next huffman table from a
#cat:                   JPEGL compressed memory buffer.
#cat: free_HUFF_TABLES - deallocates a list of huffman tables.
#cat:
#cat: free_HUFF_TABLE - deallocates a huffman table structure.
#cat:

***********************************************************************/

#include <stdio.h>
#include <jpegl.h>
#include <dataio.h>

/*****************************************************/
/* for encoder                                       */
int gen_huff_tables(HUF_TABLE **huf_table, const int N)
{
   int i, ret, adjust; 
   HUFFCODE *thuffcode_table;

   for(i = 0; i < N; i++) {

      huf_table[i]->table_id = MIN_HUFFTABLE_ID + i;

      if((ret = find_huff_sizes(&(huf_table[i]->codesize),
                               huf_table[i]->freq, MAX_HUFFCOUNTS_JPEGL))){
         return(ret);
      }

      if((ret = find_num_huff_sizes(&(huf_table[i]->bits), &adjust,
                             huf_table[i]->codesize, MAX_HUFFCOUNTS_JPEGL))){
         return(ret);
      }

      if(adjust){
         if((ret = sort_huffbits(huf_table[i]->bits))){
            return(ret);
         }
      }

      if((ret = sort_code_sizes(&(huf_table[i]->values),
                               huf_table[i]->codesize, MAX_HUFFCOUNTS_JPEGL))){
         return(ret);
      }

      if((ret = build_huffsizes(&thuffcode_table, &(huf_table[i]->last_size),
                               huf_table[i]->bits, MAX_HUFFCOUNTS_JPEGL))){
         return(ret);
      }

      build_huffcodes(thuffcode_table);

      if((ret = build_huffcode_table(&(huf_table[i]->huffcode_table),
                       thuffcode_table, huf_table[i]->last_size,
                       huf_table[i]->values, MAX_HUFFCOUNTS_JPEGL))){
         free(thuffcode_table);
         return(ret);
      }

      free(thuffcode_table);
   }

   return(0);
}


/***************************************************/
/* for decoder                                     */
int read_huffman_table_jpegl(HUF_TABLE **huf_table, FILE *infp)
{
   int ret, i, bytes_left;
   unsigned char table_id;
   HUF_TABLE *thuf_table;

   thuf_table = (HUF_TABLE *)calloc(1, sizeof(HUF_TABLE));
   if(thuf_table == (HUF_TABLE *)NULL){
      fprintf(stderr, "ERROR : read_huffman_table_jpegl : ");
      fprintf(stderr, "calloc : thuf_table\n");
      return(-2);
   }

   if((ret = read_huffman_table(&table_id, &(thuf_table->bits),
                 &(thuf_table->values), MAX_HUFFCOUNTS_JPEGL,
                 infp, READ_TABLE_LEN, &bytes_left))){
      free_HUFF_TABLE(thuf_table);
      return(ret);
   }

   /* There should only be one table in each DHT record in a */
   /* JPEGL file.  If extra bytes remain, then ERROR. */
   if(bytes_left){
      fprintf(stderr, "ERROR : read_huffman_table_jpegl : ");
      fprintf(stderr, "extra bytes after huffman table ID = %d\n", table_id);
      free_HUFF_TABLE(thuf_table);
      return(-3);
   }

   if((table_id < MIN_HUFFTABLE_ID) ||
      (table_id >= (MIN_HUFFTABLE_ID + MAX_CMPNTS))){

      if(table_id <= 3){
         fprintf(stderr, "WARNING : read_huffman_table_jpegl : ");
         fprintf(stderr, "huffman table index %d not in range %d - %d\n",
                          table_id, MIN_HUFFTABLE_ID,
                          MAX_CMPNTS+MIN_HUFFTABLE_ID-1);
         fprintf(stderr, "Attempting to decode with given table index.");
         fprintf(stderr, " Assuming index values 0-3 are being used.\n");
         table_id += MIN_HUFFTABLE_ID;
      }
      else{
         fprintf(stderr, "ERROR : read_huffman_table_jpegl : ");
         fprintf(stderr, "huffman table index %d not in range %d - %d\n",
                          table_id, MIN_HUFFTABLE_ID,
                          MAX_CMPNTS+MIN_HUFFTABLE_ID-1);
         free_HUFF_TABLE(thuf_table);
         return(-4);
      }
   }

   thuf_table->table_id = table_id;
   thuf_table->def = 1;

   /* Add new huffman table to list. */

   i = thuf_table->table_id - MIN_HUFFTABLE_ID;
   if((huf_table[i] != (HUF_TABLE *)NULL) &&
      (huf_table[i]->def == 1)){

      fprintf(stderr,
      "ERROR : jpegl_decode_mem : huffman table %d illegally redefined\n",
              thuf_table->table_id);
      free_HUFF_TABLE(thuf_table);
      return(-5);
   }

   huf_table[i] = thuf_table;

   /* Build rest of table. */

   thuf_table->maxcode = (int *)calloc(MAX_HUFFCOUNTS_JPEGL+1,
                                          sizeof(int));
   if(thuf_table->maxcode == (int *)NULL){
      fprintf(stderr, "ERROR : read_huffman_table_jpegl : ");
      fprintf(stderr, "calloc : maxcode\n");
      free_HUFF_TABLE(thuf_table);
      return(-6);
   }

   thuf_table->mincode = (int *)calloc(MAX_HUFFCOUNTS_JPEGL+1,
                                          sizeof(int));
   if(thuf_table->mincode == (int *)NULL){
      fprintf(stderr, "ERROR : read_huffman_table_jpegl : ");
      fprintf(stderr, "calloc : mincode\n");
      free_HUFF_TABLE(thuf_table);
      return(-7);
   }

   thuf_table->valptr = (int *)calloc(MAX_HUFFCOUNTS_JPEGL+1, sizeof(int));
   if(thuf_table->valptr == (int *)NULL){
      fprintf(stderr, "ERROR : read_huffman_table_jpegl : ");
      fprintf(stderr, "calloc : valptr\n");
      free_HUFF_TABLE(thuf_table);
      return(-8);
   }

   /*the next two routines reconstruct the huffman tables that were used
     in the Jpeg lossless compression*/
   if((ret = build_huffsizes(&(thuf_table->huffcode_table),
                            &(thuf_table->last_size), thuf_table->bits,
                            MAX_HUFFCOUNTS_JPEGL))){
      free_HUFF_TABLE(thuf_table);
      return(ret);
   }

   build_huffcodes(thuf_table->huffcode_table);

   /*this routine builds a set of three tables used in decoding the compressed
     data*/
   gen_decode_table(thuf_table->huffcode_table,
                    thuf_table->maxcode, thuf_table->mincode,
                    thuf_table->valptr, thuf_table->bits);

   free(thuf_table->huffcode_table);
   thuf_table->huffcode_table = (HUFFCODE *)NULL;

   return(0);
}

/***************************************************/
/* for decoder                                     */
int getc_huffman_table_jpegl(HUF_TABLE **huf_table,
                             unsigned char **cbufptr, unsigned char *ebufptr)
{
   int ret, i, bytes_left;
   unsigned char table_id;
   HUF_TABLE *thuf_table;

   thuf_table = (HUF_TABLE *)calloc(1, sizeof(HUF_TABLE));
   if(thuf_table == (HUF_TABLE *)NULL){
      fprintf(stderr, "ERROR : getc_huffman_table_jpegl : ");
      fprintf(stderr, "calloc : thuf_table\n");
      return(-2);
   }

   if((ret = getc_huffman_table(&table_id, &(thuf_table->bits),
                 &(thuf_table->values), MAX_HUFFCOUNTS_JPEGL,
                 cbufptr, ebufptr, READ_TABLE_LEN, &bytes_left))){
      free_HUFF_TABLE(thuf_table);
      return(ret);
   }

   /* There should only be one table in each DHT record in a */
   /* JPEGL file.  If extra bytes remain, then ERROR. */
   if(bytes_left){
      fprintf(stderr, "ERROR : getc_huffman_table_jpegl : ");
      fprintf(stderr, "extra bytes after huffman table ID = %d\n", table_id);
      free_HUFF_TABLE(thuf_table);
      return(-3);
   }

   if((table_id < MIN_HUFFTABLE_ID) ||
      (table_id >= (MIN_HUFFTABLE_ID + MAX_CMPNTS))){

      if(table_id <= 3){
         fprintf(stderr, "WARNING : getc_huffman_table_jpegl : ");
         fprintf(stderr, "huffman table index %d not in range %d - %d\n",
                          table_id, MIN_HUFFTABLE_ID,
                          MAX_CMPNTS+MIN_HUFFTABLE_ID-1);
         fprintf(stderr, "Attempting to decode with given table index.");
         fprintf(stderr, " Assuming index values 0-3 are being used.\n");
         table_id += MIN_HUFFTABLE_ID;
      }
      else{
         fprintf(stderr, "ERROR : getc_huffman_table_jpegl : ");
         fprintf(stderr, "huffman table index %d not in range %d - %d\n",
                          table_id, MIN_HUFFTABLE_ID,
                          MAX_CMPNTS+MIN_HUFFTABLE_ID-1);
         free_HUFF_TABLE(thuf_table);
         return(-4);
      }
   }

   thuf_table->table_id = table_id;
   thuf_table->def = 1;

   /* Add new huffman table to list. */

   i = thuf_table->table_id - MIN_HUFFTABLE_ID;
   if((huf_table[i] != (HUF_TABLE *)NULL) &&
      (huf_table[i]->def == 1)){

      fprintf(stderr,
      "ERROR : jpegl_decode_mem : huffman table %d illegally redefined\n",
              thuf_table->table_id);
      free_HUFF_TABLE(thuf_table);
      return(-5);
   }

   huf_table[i] = thuf_table;

   /* Build rest of table. */

   thuf_table->maxcode = (int *)calloc(MAX_HUFFCOUNTS_JPEGL+1,
                                          sizeof(int));
   if(thuf_table->maxcode == (int *)NULL){
      fprintf(stderr, "ERROR : getc_huffman_table_jpegl : ");
      fprintf(stderr, "calloc : maxcode\n");
      free_HUFF_TABLE(thuf_table);
      return(-6);
   }

   thuf_table->mincode = (int *)calloc(MAX_HUFFCOUNTS_JPEGL+1,
                                          sizeof(int));
   if(thuf_table->mincode == (int *)NULL){
      fprintf(stderr, "ERROR : getc_huffman_table_jpegl : ");
      fprintf(stderr, "calloc : mincode\n");
      free_HUFF_TABLE(thuf_table);
      return(-7);
   }

   thuf_table->valptr = (int *)calloc(MAX_HUFFCOUNTS_JPEGL+1, sizeof(int));
   if(thuf_table->valptr == (int *)NULL){
      fprintf(stderr, "ERROR : getc_huffman_table_jpegl : ");
      fprintf(stderr, "calloc : valptr\n");
      free_HUFF_TABLE(thuf_table);
      return(-8);
   }

   /*the next two routines reconstruct the huffman tables that were used
     in the Jpeg lossless compression*/
   if((ret = build_huffsizes(&(thuf_table->huffcode_table),
                            &(thuf_table->last_size), thuf_table->bits,
                            MAX_HUFFCOUNTS_JPEGL))){
      free_HUFF_TABLE(thuf_table);
      return(ret);
   }

   build_huffcodes(thuf_table->huffcode_table);

   /*this routine builds a set of three tables used in decoding the compressed
     data*/
   gen_decode_table(thuf_table->huffcode_table,
                    thuf_table->maxcode, thuf_table->mincode,
                    thuf_table->valptr, thuf_table->bits);

   free(thuf_table->huffcode_table);
   thuf_table->huffcode_table = (HUFFCODE *)NULL;

   return(0);
}

/******************************************************/
/* Deallocate list of JPEGL huffman table structures. */
/******************************************************/
void free_HUFF_TABLES(HUF_TABLE **huf_table, const int N)
{
   int i;

   for(i = 0; i < N; i++){
      if(huf_table[i] != (HUF_TABLE *)NULL){
         free_HUFF_TABLE(huf_table[i]);
      }
   }
}

/************** ********************************/
/* Deallocate a JPEGL huffman table structure. */
/***********************************************/
void free_HUFF_TABLE(HUF_TABLE *huf_table)
{
   if(huf_table->freq != (int *)NULL)
      free(huf_table->freq);

   if(huf_table->codesize != (int *)NULL)
      free(huf_table->codesize);

   if(huf_table->bits != (unsigned char *)NULL)
      free(huf_table->bits);

   if(huf_table->values != (unsigned char *)NULL)
      free(huf_table->values);

   if(huf_table->huffcode_table != (HUFFCODE *)NULL)
      free(huf_table->huffcode_table);

   if(huf_table->maxcode != (int *)NULL)
      free(huf_table->maxcode);

   if(huf_table->mincode != (int *)NULL)
      free(huf_table->mincode);

   if(huf_table->valptr != (int *)NULL)
      free(huf_table->valptr);

   free(huf_table);
}
