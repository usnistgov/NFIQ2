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

      FILE:    HUFF.C
      AUTHOR:  Craig Watson
      DATE:    11/11/1991
      UPDATED: 03/16/2005 by MDG

      Contains low-level routines responsible for computing huffman
      tables used in JPEGL (lossless) image compression.

      ROUTINES:
#cat: read_huffman_table - Reads a huffman table from an open file.
#cat:
#cat: getc_huffman_table - Reads a huffman table from a memory buffer.
#cat:
#cat: write_huffman_table - Writes a huffman table to an open file.
#cat:
#cat: putc_huffman_table - Writes a huffman table to a memory buffer.
#cat:
#cat: find_huff_sizes - Optimizes code sizes by the frequency of
#cat:                   pixel difference values.
#cat: find_least_freq - Finds the larges pixel difference with the
#cat:                   least frequency.
#cat: find_num_huff_sizes - Determines the number of codes for each size.
#cat:
#cat: sort_huffbits - Ensures that no huffman code size is greater than 16.
#cat:
#cat: sort_code_sizes - Sorts a list of huffman code sizes.
#cat:
#cat: build_huffcode_table - Sorts huffman codes and sizes.
#cat:
#cat: build_huffsizes - Defines the code sizes for each difference category.
#cat:
#cat: build_huffcodes - Defines the huffman codes need for each difference
#cat:                   category.
#cat: gen_decode_table - Generates the huffman decode table.
#cat:

***********************************************************************/

#include <stdio.h>
#include <jpegl.h>
#include <dataio.h>

/********************************************/
/* Reads huffman table from compressed file */
/********************************************/
int read_huffman_table(unsigned char *otable_id, unsigned char **ohuffbits,
                       unsigned char **ohuffvalues, const int max_huffcounts,
                       FILE *infp, const int read_table_len, int *bytes_left)
{
   int ret, i;
   unsigned short table_len;
   unsigned char table_id;
   unsigned char *huffbits, *huffvalues;
   unsigned short num_hufvals;

   if(debug > 0)
      fprintf(stdout, "Start reading huffman table.\n");

   /* table_len */
   if(read_table_len){
      if((ret = read_ushort(&table_len, infp)))
         return(ret);
      *bytes_left = table_len - 2;
   }

   /* If no bytes left ... */ 
   if((*bytes_left) <= 0){
      fprintf(stderr, "ERROR : read_huffman_table : ");
      fprintf(stderr, "no huffman table bytes remaining\n");
      return(-2);
   }

   /* Table ID */
   if((ret = read_byte(&table_id, infp)))
      return(ret);
   (*bytes_left)--;

   huffbits = (unsigned char *)calloc(MAX_HUFFBITS, sizeof(unsigned char));
   if(huffbits == (unsigned char *)NULL){
      fprintf(stderr,
              "ERROR : read_huffman_table : calloc : huffbits\n");
      return(-3);
   }

   num_hufvals = 0;
   /* L1 ... L16 */
   for(i = 0; i < MAX_HUFFBITS; i++){
      if((ret = read_byte(&(huffbits[i]), infp))){
         free(huffbits);
         return(ret);
      }
      num_hufvals += huffbits[i];
   }
   (*bytes_left) -= MAX_HUFFBITS;

   if(num_hufvals > max_huffcounts+1){
      fprintf(stderr, "ERROR : read_huffman_table : ");
      fprintf(stderr, "num_hufvals (%d) is larger", num_hufvals);
      fprintf(stderr, "than MAX_HUFFCOUNTS (%d)\n", max_huffcounts+1);
      free(huffbits);
      return(-4);
   }

   /* Could allocate only the amount needed ... then we wouldn't */
   /* need to pass MAX_HUFFCOUNTS. */
   huffvalues = (unsigned char *)calloc(max_huffcounts+1,
                                        sizeof(unsigned char));
   if(huffvalues == (unsigned char *)NULL){
      fprintf(stderr,
              "ERROR : read_huffman_table : calloc : huffvalues\n");
      free(huffbits);
      return(-5);
   }

   /* V1,1 ... V16,16 */
   for(i = 0; i < num_hufvals; i++){
      if((ret = read_byte(&(huffvalues[i]), infp))){
         free(huffbits);
         free(huffvalues);
         return(ret);
      }
   }
   (*bytes_left) -= num_hufvals;

   if(debug > 1){
      fprintf(stdout, "Table Len = %d\n", table_len);
      fprintf(stdout, "Table ID = %d\n", table_id);
      for(i = 0; i < MAX_HUFFBITS; i++)
         fprintf(stdout, "bits[%d] = %d\n", i, huffbits[i]);
      for(i = 0; i < num_hufvals; i++)
         fprintf(stdout, "values[%d] = %d\n", i, huffvalues[i]);
   }
   
   if(debug > 0)
      fprintf(stdout, "Finished reading huffman table.\n");

   *otable_id = table_id;
   *ohuffbits = huffbits;
   *ohuffvalues = huffvalues;

   return(0);
}

/*****************************************************/
/* Reads huffman table from compressed memory buffer */
/*****************************************************/
int getc_huffman_table(unsigned char *otable_id, unsigned char **ohuffbits,
                       unsigned char **ohuffvalues, const int max_huffcounts,
                       unsigned char **cbufptr, unsigned char *ebufptr,
                       const int read_table_len, int *bytes_left)
{
   int ret, i;
   unsigned short table_len;
   unsigned char table_id;
   unsigned char *huffbits, *huffvalues;
   unsigned short num_hufvals;

   if(debug > 0)
      fprintf(stdout, "Start reading huffman table.\n");

   /* table_len */
   if(read_table_len){
      if((ret = getc_ushort(&table_len, cbufptr, ebufptr)))
         return(ret);
      *bytes_left = table_len - 2;
   }

   /* If no bytes left ... */ 
   if((*bytes_left) <= 0){
      fprintf(stderr, "ERROR : getc_huffman_table : ");
      fprintf(stderr, "no huffman table bytes remaining\n");
      return(-2);
   }

   /* Table ID */
   if((ret = getc_byte(&table_id, cbufptr, ebufptr)))
      return(ret);
   (*bytes_left)--;

   huffbits = (unsigned char *)calloc(MAX_HUFFBITS, sizeof(unsigned char));
   if(huffbits == (unsigned char *)NULL){
      fprintf(stderr,
              "ERROR : getc_huffman_table : calloc : huffbits\n");
      return(-3);
   }

   num_hufvals = 0;
   /* L1 ... L16 */
   for(i = 0; i < MAX_HUFFBITS; i++){
      if((ret = getc_byte(&(huffbits[i]), cbufptr, ebufptr))){
         free(huffbits);
         return(ret);
      }
      num_hufvals += huffbits[i];
   }
   (*bytes_left) -= MAX_HUFFBITS;

   if(num_hufvals > max_huffcounts+1){
      fprintf(stderr, "ERROR : getc_huffman_table : ");
      fprintf(stderr, "num_hufvals (%d) is larger", num_hufvals);
      fprintf(stderr, "than MAX_HUFFCOUNTS (%d)\n", max_huffcounts+1);
      free(huffbits);
      return(-4);
   }

   /* Could allocate only the amount needed ... then we wouldn't */
   /* need to pass MAX_HUFFCOUNTS. */
   huffvalues = (unsigned char *)calloc(max_huffcounts+1,
                                        sizeof(unsigned char));
   if(huffvalues == (unsigned char *)NULL){
      fprintf(stderr,
              "ERROR : getc_huffman_table : calloc : huffvalues\n");
      free(huffbits);
      return(-5);
   }

   /* V1,1 ... V16,16 */
   for(i = 0; i < num_hufvals; i++){
      if((ret = getc_byte(&(huffvalues[i]), cbufptr, ebufptr))){
         free(huffbits);
         free(huffvalues);
         return(ret);
      }
   }
   (*bytes_left) -= num_hufvals;

   if(debug > 1){
      fprintf(stdout, "Table Len = %d\n", table_len);
      fprintf(stdout, "Table ID = %d\n", table_id);
      for(i = 0; i < MAX_HUFFBITS; i++)
         fprintf(stdout, "bits[%d] = %d\n", i, huffbits[i]);
      for(i = 0; i < num_hufvals; i++)
         fprintf(stdout, "values[%d] = %d\n", i, huffvalues[i]);
   }
   
   if(debug > 0)
      fprintf(stdout, "Finished reading huffman table.\n");

   *otable_id = table_id;
   *ohuffbits = huffbits;
   *ohuffvalues = huffvalues;

   return(0);
}

/***********************************************/
/* Writes huffman table to the compressed file */
/***********************************************/
int write_huffman_table(
   const unsigned short marker,  /* Markers are different for JPEGL and WSQ */
   const unsigned char table_id, /* huffman table indicator  */
   unsigned char *huffbits,      /* huffman table parameters */
   unsigned char *huffvalues,
   FILE  *outfp)         /* output file              */
{
   int i, ret;
   unsigned short table_len, values_offset;

   if(debug > 0)
      fprintf(stdout, "Start writing huffman table.\n");

   /* DHT */
   if((ret = write_ushort(marker, outfp)))
      return(ret);

   /* "value(2) + table id(1) + bits(16)" */
   table_len = values_offset = 3 + MAX_HUFFBITS;
   for(i = 0; i < MAX_HUFFBITS; i++)
      table_len += huffbits[i];   /* values size */

   if(debug > 1){
      fprintf(stdout, "Table Len = %d\n", table_len);
      fprintf(stdout, "Table ID = %d\n", table_id);
      for(i = 0; i < MAX_HUFFBITS; i++)
         fprintf(stdout, "bits[%d] = %d\n", i, huffbits[i]);
      for(i = 0; i < table_len-values_offset; i++)
         fprintf(stdout, "values[%d] = %d\n", i, huffvalues[i]);
   }
   
   /* Table Len */
   if((ret = write_ushort(table_len, outfp)))
      return(ret);

   /* Table ID */
   if((ret = write_byte(table_id, outfp)))
      return(ret);

   /* Huffbits (MAX_HUFFBITS) */
   for(i = 0; i < MAX_HUFFBITS; i++){
      if((ret = write_byte(huffbits[i], outfp)))
         return(ret);
   }

   /* Huffvalues (MAX_HUFFCOUNTS) */
   for(i = 0; i < table_len-values_offset; i++){
      if((ret = write_byte(huffvalues[i], outfp)))
         return(ret);
   }

   if(debug > 0)
      fprintf(stdout, "Finished writing huffman table.\n\n");

   return(0);
}

/********************************************************/
/* Writes huffman table to the compressed memory buffer */
/********************************************************/
int putc_huffman_table(
   const unsigned short marker,  /* Markers are different for JPEGL and WSQ */
   const unsigned char table_id,   /* huffman table indicator  */
   unsigned char *huffbits,      /* huffman table parameters */
   unsigned char *huffvalues,
   unsigned char *outbuf,        /* output byte buffer       */
   const int outalloc,   /* allocated size of buffer */
   int   *outlen)        /* filled length of buffer  */
{
   int i, ret;
   unsigned short table_len, values_offset;

   if(debug > 0)
      fprintf(stdout, "Start writing huffman table.\n");

   /* DHT */
   if((ret = putc_ushort(marker, outbuf, outalloc, outlen)))
      return(ret);

   /* "value(2) + table id(1) + bits(16)" */
   table_len = values_offset = 3 + MAX_HUFFBITS;
   for(i = 0; i < MAX_HUFFBITS; i++)
      table_len += huffbits[i];   /* values size */

   if(debug > 1){
      fprintf(stdout, "Table Len = %d\n", table_len);
      fprintf(stdout, "Table ID = %d\n", table_id);
      for(i = 0; i < MAX_HUFFBITS; i++)
         fprintf(stdout, "bits[%d] = %d\n", i, huffbits[i]);
      for(i = 0; i < table_len-values_offset; i++)
         fprintf(stdout, "values[%d] = %d\n", i, huffvalues[i]);
   }

   /* Table Len */
   if((ret = putc_ushort(table_len, outbuf, outalloc, outlen)))
      return(ret);

   /* Table ID */
   if((ret = putc_byte(table_id, outbuf, outalloc, outlen)))
      return(ret);

   /* Huffbits (MAX_HUFFBITS) */
   for(i = 0; i < MAX_HUFFBITS; i++){
      if((ret = putc_byte(huffbits[i], outbuf, outalloc, outlen)))
         return(ret);
   }

   /* Huffvalues (MAX_HUFFCOUNTS) */
   for(i = 0; i < table_len-values_offset; i++){
      if((ret = putc_byte(huffvalues[i], outbuf, outalloc, outlen)))
         return(ret);
   }

   if(debug > 0)
      fprintf(stdout, "Finished writing huffman table.\n\n");

   return(0);
}

/******************************************************************/
/*routine to optimize code sizes by frequency of difference values*/
/******************************************************************/
int find_huff_sizes(int **ocodesize, int *freq, const int max_huffcounts)
{
   int *codesize;       /*codesizes for each category*/
   int *others;         /*pointer used to generate codesizes*/
   int value1;          /*smallest and next smallest frequency*/
   int value2;          /*of difference occurrence in the largest
			  difference category*/
   int i;               /*increment variable*/


   codesize = (int *)calloc(max_huffcounts+1, sizeof(int));
   if(codesize == (int *)NULL){
      fprintf(stderr, "ERROR : find_huff_sizes : calloc : codesize\n");
      return(-2);
   }
   others = (int *)malloc((max_huffcounts+1) * sizeof(int));
   if(others == (int *)NULL){
      fprintf(stderr, "ERROR : find_huff_sizes : malloc : others\n");
      return(-3);
   }

   for (i = 0; i <= max_huffcounts; i++) 
      others[i] = -1;

   while(1) {

      find_least_freq(&value1, &value2, freq, max_huffcounts);

      if(value2 == -1) {
	 free(others);
	 if(debug > 2){
	    for (i = 0; i <= max_huffcounts; i++) 
	       fprintf(stdout, "codesize[%d] = %d\n", i, codesize[i]);
	 }
         break;
      }

      freq[value1] += freq[value2];
      freq[value2] = 0;

      codesize[value1]++;
      while(others[value1] != -1) {
	 value1 = others[value1];
	 codesize[value1]++;
      }
      others[value1] = value2;
      codesize[value2]++;

      while(others[value2] != -1) {
	 value2 = others[value2];
	 codesize[value2]++;
      }
   }

   *ocodesize = codesize;
   return(0);
}

/***********************************************************************/
/*routine to find the largest difference with the least frequency value*/
/***********************************************************************/
void find_least_freq(int *value1, int *value2, int *freq,
                     const int max_huffcounts)
{
   int i;               /*increment variable*/
   int code_temp;       /*store code*/
   int value_temp;      /*store size*/
   int code2 = 0;       /*next smallest frequency in largest diff category*/
   int code1 = 0;       /*smallest frequency in largest difference category*/
   int set = 1;         /*flag first two non-zero frequency values*/

   *value1 = -1;
   *value2 = -1;

   for(i = 0; i <= max_huffcounts; i++) {
      if(freq[i] == 0)
	 continue;
      if(set == 1) {
	 code1 = freq[i];
	 *value1 = i;
	 set++;
	 continue;
      }
      if(set == 2) {
	 code2 = freq[i];
	 *value2 = i;
	 set++;
      }
      code_temp = freq[i];
      value_temp = i;
      if(code1 < code_temp && code2 < code_temp)
	 continue;
      if((code_temp < code1) || (code_temp == code1 && value_temp > *value1)) {
	 code2 = code1;
	 *value2 = *value1;
	 code1 = code_temp;
	 *value1 = value_temp;
	 continue;
      }
      if((code_temp < code2) || (code_temp == code2 && value_temp > *value2)) {
	 code2 = code_temp;
	 *value2 = value_temp;
      }
   }
}

/**********************************************/
/*routine to find number of codes of each size*/
/**********************************************/
int find_num_huff_sizes(unsigned char **obits, int *adjust, int *codesize,
                        const int max_huffcounts)
{
   unsigned char *bits;    /*defines number of codes for each size*/
   int i;          /*increment variable*/

   *adjust = 0;

   /* Allocate 2X desired number of bits due to possible codesize. */
   bits = (unsigned char *)calloc((MAX_HUFFBITS<<1), sizeof(unsigned char));
   if(bits == (unsigned char *)NULL){
      fprintf(stderr, "ERROR : find_num_huff_sizes : calloc : bits\n");
      return(-2);
   }

   for(i = 0; i < max_huffcounts; i++) {
      if(codesize[i] != 0)
	 bits[(codesize[i] - 1)]++;
         if(codesize[i] > MAX_HUFFBITS)
            *adjust = 1;
   }

   if(debug > 2){
      for(i = 0; i < MAX_HUFFBITS<<1; i++)
	 fprintf(stdout, "bits[%d] = %d\n", i, bits[i]);
      fprintf(stdout, "ADJUST = %d\n", *adjust);
   }

   *obits = bits;
   return(0);
}

/****************************************************************/
/*routine to insure that no huffman code size is greater than 16*/
/****************************************************************/
int sort_huffbits(unsigned char *bits)
{
   int i, j;
   int l1, l2, l3;
   short *tbits;

   l3 = MAX_HUFFBITS<<1;       /* 32 */
   l1 = l3 - 1;                /* 31 */
   l2 = MAX_HUFFBITS - 1;      /* 15 */

   tbits = (short *)malloc(l3*sizeof(short));
   if(tbits == (short *)NULL){
      fprintf(stderr, "ERROR : sort_huffbits : malloc : tbits\n");
      return(-2);
   }


   for(i = 0; i < MAX_HUFFBITS<<1; i++)
      tbits[i] = bits[i];

   for(i = l1; i > l2; i--) {
      while(tbits[i] > 0) {
         j = i - 2;
         while(tbits[j] == 0)
            j--;
         tbits[i] -= 2;
         tbits[i - 1] += 1;
         tbits[j + 1] += 2;
         tbits[j] -= 1;
      }
      tbits[i] = 0;
   }

   while(tbits[i] == 0)
      i--;

   tbits[i] -= 1;

   for(i = 0; i < MAX_HUFFBITS<<1; i++)
      bits[i] = tbits[i];
   free(tbits);

   for(i = MAX_HUFFBITS; i < l3; i++){
      if(bits[i] > 0){
         fprintf(stderr,
            "ERROR : sort_huffbits : Code length of %d is greater than 16.\n",
            i);
         return(-3);
      }
   }

   if(debug > 1){
      fprintf(stdout, "Huffbits after sorting.\n");
      for(i = 0; i < MAX_HUFFBITS<<1; i++)
         fprintf(stdout,"sort_bits[%d] = %d\n", i, bits[i]);
   }

   return(0);
}

/****************************************/
/*routine to sort the huffman code sizes*/
/****************************************/
int sort_code_sizes(unsigned char **ovalues, int *codesize,
         const int max_huffcounts)
{
   unsigned char *values;      /*defines order of huffman codelengths in
                         relation to the code sizes*/
   int i, i2 = 0, i3;  /*increment variables*/


   values = (unsigned char *)calloc(max_huffcounts+1, sizeof(unsigned char));
   if(values == (unsigned char *)NULL){
      fprintf(stderr, "ERROR : sort_code_sizes : calloc : value\n");
      return(-2);
   }

   for(i = 1; i <= (MAX_HUFFBITS<<1); i++) {
      for(i3 = 0; i3 < max_huffcounts; i3++) {
	 if(codesize[i3] == i) {
	    values[i2] = i3;
	    i2++;
	 }
      }
   }

   if(debug > 2){
      for(i = 0; i <= max_huffcounts; i++)
	 fprintf(stdout, "values[%d] = %d\n", i, values[i]);
   }

   *ovalues = values;
   return(0);
}

/*****************************************/
/*routine to sort huffman codes and sizes*/
/*****************************************/
int build_huffcode_table(HUFFCODE **ohuffcode_table,
          HUFFCODE *in_huffcode_table, const int last_size,
          unsigned char *values, const int max_huffcounts)
{
   int size;      /*huffman code size variable*/
   HUFFCODE *new_huffcode_table; /*pointer to a huffman code structure*/

   new_huffcode_table = (HUFFCODE *)calloc(max_huffcounts+1, sizeof(HUFFCODE));
   if(new_huffcode_table == (HUFFCODE *)NULL){
      fprintf(stderr,
      "ERROR : build_huffcode_table : calloc : new_huffcode_table\n");
      return(-2);
   }

   for(size = 0; size < last_size; size++) {
      (new_huffcode_table+values[size])->code = (in_huffcode_table+size)->code;
      (new_huffcode_table+values[size])->size = (in_huffcode_table+size)->size;
   }

   if(debug > 3){
      for(size = 0; size <= max_huffcounts; size++) {
         fprintf(stdout, "huff_size[%d] = %d\n", size,
                 new_huffcode_table[size].size);
         fprintf(stdout, "huff_code[%d] = %d\n", size,
                 new_huffcode_table[size].code);
      }
   }

   *ohuffcode_table = new_huffcode_table;
   return(0);
}

/**************************************************************************/
/*This routine defines the huffman code sizes for each difference category*/
/**************************************************************************/
int build_huffsizes(HUFFCODE **ohuffcode_table, int *temp_size,
                    unsigned char *huffbits, const int max_huffcounts)
{
   HUFFCODE *huffcode_table;    /*table of huffman codes and sizes*/
   int code_size;               /*code sizes*/
   int number_of_codes = 1;     /*the number codes for a given code size*/

   huffcode_table = (HUFFCODE *)calloc(max_huffcounts+1, sizeof(HUFFCODE));
   if(huffcode_table == (HUFFCODE *)NULL){
      fprintf(stderr, "ERROR : build_huffsizes : calloc : huffcode_table\n");
      return(-2);
   }

   *temp_size = 0;

   for(code_size = 1; code_size <= MAX_HUFFBITS; code_size++) {
      while(number_of_codes <= huffbits[code_size - 1]) {
	 (huffcode_table + *temp_size)->size = code_size;
	 (*temp_size)++;
	 number_of_codes++;
      }
      number_of_codes = 1;
   }
   (huffcode_table+(*temp_size))->size = 0;

   if(debug > 2){
      int ii;
      fprintf(stderr, "In build_huffsizes:\n");
      for(ii = 0; ii < max_huffcounts+1; ii++)
         fprintf(stderr, "hf_sz[%d] = %d\n", ii, huffcode_table[ii].size);
      fflush(stderr);
   }

   *ohuffcode_table = huffcode_table;
   return(0);
}

/****************************************************************************/
/*This routine defines the huffman codes needed for each difference category*/
/****************************************************************************/
void build_huffcodes(HUFFCODE *huffcode_table)
{
   int pointer = 0;                     /*pointer to code word information*/
   unsigned short temp_code = 0;        /*used to construct code word*/
   short  temp_size;                    /*used to construct code size*/

   temp_size = huffcode_table->size;
   if((huffcode_table+pointer)->size == 0)
      return;

   do {
      do {
	 (huffcode_table+pointer)->code = temp_code;
	 temp_code++;
	 pointer++;
      } while((huffcode_table+pointer)->size == temp_size);

      if((huffcode_table+pointer)->size == 0)
	 return;

      do {
	 temp_code <<= 1;
	 temp_size++;
      } while((huffcode_table+pointer)->size != temp_size);
   } while((huffcode_table+pointer)->size == temp_size);
}

/*********************************************/
/*routine to generate tables needed to decode*/
/*********************************************/
void gen_decode_table(HUFFCODE *huffcode_table,
            int *maxcode, int *mincode, int *valptr, unsigned char *huffbits)
{
   int i, i2 = 0;                   /*increment variables*/

   for(i = 0; i <= MAX_HUFFBITS; i++) {
      maxcode[i] = 0;
      mincode[i] = 0;
      valptr[i] = 0;
   }

   for(i = 1; i <= MAX_HUFFBITS; i++) {
      if(huffbits[i-1] == 0) {
	 maxcode[i] = -1;
	 continue;
      }
      valptr[i] = i2;
      mincode[i] = (huffcode_table + i2)->code;
      i2 = i2 + huffbits[i - 1] - 1;
      maxcode[i] = (huffcode_table + i2)->code;
      i2++;
   }
}

