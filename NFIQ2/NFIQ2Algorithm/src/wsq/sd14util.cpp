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
      LIBRARY: WSQ - Grayscale Image Compression

      FILE:    SD14UTIL.C
      AUTHOR:  Craig Watson
      DATE:    12/15/2000
      UPDATED:  02/24/2005 by MDG

      Contains routines responsible for decoding and converting an
      old image format used to WSQ-compress fingerprints in NIST
      Special Database 14.  This old format should be considered
      obsolete.

      ROUTINES:
#cat: wsq14_decode_file - Decompresses a WSQ-compressed datastream encoded
#cat:           according to an old image format used in NIST Special
#cat:           Database 14.  This routine should be used to decompress
#cat:           legacy data only.  This old format should be considered
#cat:           obsolete.
#cat: wsq14_2_wsq - Converts a WSQ-compressed datastream encoded according
#cat:           to an old image format used in NIST Special Database 14
#cat:           to the WSQ format compliant with the FBI's WSQ Gray-Scale
#cat:           Fingerprint Image Compression Specification.

***********************************************************************/

/***********************************************************/
/* NOTE: The routines in this file are provided to support */
/*       the decoding and converting of files exclusively  */
/*       distributed with NIST Special Database 14 (SD14). */
/*       The format of SD14 files is not compliant with    */
/*       the FBI's WSQ Gray-Scale Fingerprint Image        */
/*       Compression Specification.                        */
/*                                                         */
/*       ALL future WSQ developments should NOT use these  */
/*       routines, but rather use the FBI certifiable      */
/*       routines provided in the remainder of this        */
/*       library distribution.                             */
/***********************************************************/

#include <stdio.h>
#include <wsq.h>
#include <dataio.h>

/* Old format global trees. */
static Q_TREE q_tree_wsq14[Q_TREELEN];
/*
static W_TREE w_tree_wsq14[W_TREELEN];
*/

/* Prototypes for functions in this file. */
int wsq14_decode_file(unsigned char **, int *, int *, int *, int *, FILE *);
int wsq14_2_wsq(unsigned char **, int *, FILE *);
static int read_table_wsq14(unsigned short, DTT_TABLE *, DQT_TABLE *,
                      DHT_TABLE *, FILE *);
static int read_huff_table_wsq14(DHT_TABLE *, FILE *);
static void build_wsq_trees_wsq14(W_TREE [], const int, Q_TREE [], const int,
                      const int, const int);
static void build_shuffle_trees_wsq14(W_TREE [], const int, Q_TREE [],
                      const int, Q_TREE [], const int, const int, const int);
static int huffman_decode_data_file_wsq14(short *, DTT_TABLE *, DQT_TABLE *,
                      DHT_TABLE *, FILE *);
static int unshuffle_wsq14(short **, const DQT_TABLE *, Q_TREE [], const int,
                      short *, const int, const int);
static int shuffle_wsq14(short **, int *, const DQT_TABLE, Q_TREE [],
                      const int, short *, const int, const int);
static int shuffle_dqt_wsq14(DQT_TABLE *);
static void build_w_tree_wsq14(W_TREE [], const int, const int);
static void build_q_tree_wsq14(W_TREE [], Q_TREE []);
static void w_tree4_wsq14(W_TREE [], int, int, int, int, int, int, int);
static void q_tree16_wsq14(Q_TREE [], int, int, int, int, int);
static void q_tree4_wsq14(Q_TREE [], int, int, int, int, int);


/*************************************************************************/
/* WSQ14 Decoder routine.  Takes an open WSQ14 compressed file (old      */
/* format used with SD14) and reads in the encoded data, reformats the   */
/* Wavelet tree, and then calls the certifiable WSQ decoder to           */
/* reconstruct the pixmap.  This routine should ONLY be used to read     */
/* files distributed with the SD14 database.                             */
/*************************************************************************/
int wsq14_decode_file(unsigned char **odata, int *owidth, int *oheight,
                      int *odepth, int *lossyflag, FILE *infp)
{
   int ret;
   unsigned short marker;
   int num_pix;
   int width, height;
   unsigned char *cdata;
   float *fdata;
   short *qdata;

   /* Added by MDG on 02-24-05 */
   init_wsq_decoder_resources();

   /* Read the SOI_WSQ marker. */
   if((ret = read_marker_wsq(&marker, SOI_WSQ, infp))){
      free_wsq_decoder_resources();
      return(ret);
   }

   /* Read in supporting tables up to the SOF_WSQ marker. */
   if((ret = read_marker_wsq(&marker, TBLS_N_SOF, infp))){
      free_wsq_decoder_resources();
      return(ret);
   }
   while(marker != SOF_WSQ) {
      if((ret = read_table_wsq14(marker, &dtt_table, &dqt_table, dht_table, infp))){
         free_wsq_decoder_resources();
         return(ret);
      }
      if((ret = read_marker_wsq(&marker, TBLS_N_SOF, infp))){
         free_wsq_decoder_resources();
         return(ret);
      }
   }

   /* Read in the Frame Header. */
   if((ret = read_frame_header_wsq(&frm_header_wsq, infp))){
      free_wsq_decoder_resources();
      return(ret);
   }
   width = frm_header_wsq.width;
   height = frm_header_wsq.height;
   num_pix = width * height;

   if(debug > 0)
      fprintf(stderr, "SOI_WSQ, tables, and frame header read\n\n");

   /* Build WSQ decomposition trees. */
   build_wsq_trees_wsq14(w_tree, W_TREELEN, q_tree, Q_TREELEN, width, height);

   if(debug > 0)
      fprintf(stderr, "Tables for wavelet decomposition finished\n\n");

   /* Allocate working memory. */
   qdata = (short *) malloc(num_pix * sizeof(short));
   if(qdata == (short *)NULL) {
      fprintf(stderr,"ERROR: wsq_decode_1 : malloc : qdata1\n");
      free_wsq_decoder_resources();
      return(-20);
   }

   /* Decode the Huffman encoded data blocks. */
   if((ret = huffman_decode_data_file_wsq14(qdata, &dtt_table, &dqt_table,
                                           dht_table, infp))){
      free(qdata);
      free_wsq_decoder_resources();
      return(ret);
   }

   if(debug > 0)
      fprintf(stderr,
         "Quantized WSQ subband data blocks read and Huffman decoded\n\n");

   /* Decode the quantize wavelet subband data. */
   if((ret = unquantize(&fdata, &dqt_table, q_tree, Q_TREELEN,
                         qdata, width, height))){
      free(qdata);
      free_wsq_decoder_resources();
      return(ret);
   }

   if(debug > 0)
      fprintf(stderr, "WSQ subband data blocks unquantized\n\n");

   /* Done with quantized wavelet subband data. */
   free(qdata);

   if((ret = wsq_reconstruct(fdata, width, height, w_tree, W_TREELEN,
                              &dtt_table))){
      free(fdata);
      free_wsq_decoder_resources();
      return(ret);
   }

   if(debug > 0)
      fprintf(stderr, "WSQ reconstruction of image finished\n\n");

   cdata = (unsigned char *)malloc(num_pix * sizeof(unsigned char));
   if(cdata == (unsigned char *)NULL) {
      free(fdata);
      free_wsq_decoder_resources();
      fprintf(stderr,"ERROR: wsq_decode_1 : malloc : cdata\n");
      return(-21);
   }

   /* Convert floating point pixels to unsigned char pixels. */
   conv_img_2_uchar(cdata, fdata, width, height,
                      frm_header_wsq.m_shift, frm_header_wsq.r_scale);

   /* Done with floating point pixels. */
   free(fdata);

   /* Added by MDG on 02-24-05 */
   free_wsq_decoder_resources();

  if(debug > 0)
      fprintf(stderr, "Doubleing point pixels converted to unsigned char\n\n");


   /* Assign reconstructed pixmap and attributes to output pointers. */
   *odata = cdata;
   *owidth = width;
   *oheight = height;
   *odepth = 8;
   *lossyflag = 1;

   /* Return normally. */
   return(0);
}


/***************************************************************************/
/* WSQ14 Converter routine.  Takes an open WSQ14 compressed file (old      */
/* format used with SD14) and converts the file format to be compatible    */
/* with an FBI certifiable WSQ decoder.                                    */
/* NOTE:  Due to image bits already lossed, the resulting data is not      */
/* certifiable, but can be successfully decoded using a certifiable        */
/* decoder.                                                                */
/***************************************************************************/
int wsq14_2_wsq(unsigned char **odata, int *olen, FILE *infp)
{
   int ret, i;
   unsigned short marker;                 /* WSQ marker */
   int num_pix;                   /* image size and counter */
   int width, height;             /* image parameters */
   short *fdata;                  /* image pointers */
   short *qdata;                  /* image pointers */
   unsigned char *wsq_data, *huff_buf;
   int wsq_alloc, wsq_len;
   int block_sizes[2];
   unsigned char *huffbits, *huffvalues; /* huffman code parameters     */
   HUFFCODE *hufftable;          /* huffcode table              */
   int hsize, hsize1, hsize2, hsize3; /* Huffman coded blocks sizes */
   int qsize, qsize1, qsize2, qsize3; /* quantized block sizes */


/**********************************/
/* 1. READ OLD SD14 FORMAT IN ... */
/**********************************/

   /* Read the SOI_WSQ marker. */
   if((ret = read_marker_wsq(&marker, SOI_WSQ, infp))){
      return(ret);
   }

   /* Read in supporting tables up to the SOF_WSQ marker. */
   if((ret = read_marker_wsq(&marker, TBLS_N_SOF, infp))){
      return(ret);
   }
   while(marker != SOF_WSQ) {
      if((ret = read_table_wsq14(marker, &dtt_table, &dqt_table, dht_table, infp))){
         return(ret);
      }
      if((ret = read_marker_wsq(&marker, TBLS_N_SOF, infp))){
         return(ret);
      }
   }

   /* Read in the Frame Header. */
   if((ret = read_frame_header_wsq(&frm_header_wsq, infp))){
      return(ret);
   }
   width = frm_header_wsq.width;
   height = frm_header_wsq.height;
   num_pix = width * height;

   if(debug > 0)
      fprintf(stderr, "SOI_WSQ, tables, and frame header read\n\n");

   /* Build WSQ decomposition trees. */
   build_shuffle_trees_wsq14(w_tree, W_TREELEN, q_tree, Q_TREELEN,
                            q_tree_wsq14, Q_TREELEN, width, height);

   if(debug > 0)
      fprintf(stderr, "Tables for wavelet decomposition finished\n\n");

   /* Allocate working memory. */
   qdata = (short *) malloc(num_pix * sizeof(short));
   if(qdata == (short *)NULL) {
      fprintf(stderr,"ERROR: wsq_decode_1 : malloc : qdata1\n");
      return(-20);
   }

   /* Decode the Huffman encoded data blocks. */
   if((ret = huffman_decode_data_file_wsq14(qdata, &dtt_table, &dqt_table, dht_table,
                                  infp))){
      free(qdata);
      return(ret);
   }

   if(debug > 0)
      fprintf(stderr,
         "Quantized WSQ subband data blocks read and Huffman decoded\n\n");

/*************************************/
/* 2. CONVERT OLD FORMATTED DATA ... */
/*************************************/

   if((ret = unshuffle_wsq14(&fdata, &dqt_table, q_tree, Q_TREELEN,
                           qdata, width, height))){
      free(qdata);
      return(ret);
   }
   free(qdata);

   if((ret = shuffle_dqt_wsq14(&dqt_table)))
      return(ret);

   if((ret = shuffle_wsq14(&qdata, &qsize, dqt_table, q_tree_wsq14, Q_TREELEN,
                         fdata, width, height))){
      free(fdata);
      return(ret);
   }
   free(fdata);

/***********************************/
/* 3. WRITE NEW FORMATTED DATA ... */
/***********************************/

   for(i = 0; i < MAX_SUBBANDS; i++) {
      quant_vals.qbss[i] = dqt_table.q_bin[i];
      quant_vals.qzbs[i] = dqt_table.z_bin[i];
   }

   /* Compute quantized WSQ subband block sizes */
   quant_block_sizes(&qsize1, &qsize2, &qsize3, &quant_vals,
                           w_tree, W_TREELEN, q_tree, Q_TREELEN);

   if(qsize != qsize1+qsize2+qsize3){
      fprintf(stderr,
              "ERROR : wsq_encode_1 : problem w/quantization block sizes\n");
      return(-11);
   }

   /* Allocate a WSQ-encoded output buffer.  Allocate this buffer */
   /* to be the size of the original pixmap.  If the encoded data */
   /* exceeds this buffer size, then throw an error because we do */
   /* not want our compressed data to be larger than the original */
   /* image data.                                                 */
   wsq_data = (unsigned char *)malloc(num_pix);
   if(wsq_data == (unsigned char *)NULL){
      free(qdata);
      fprintf(stderr, "ERROR : wsq_encode_1 : malloc : wsq_data\n");
      return(-12);
   }
   wsq_alloc = num_pix;
   wsq_len = 0;

   /* Add a Start Of Image (SOI_WSQ) marker to the WSQ buffer. */
   if((ret = putc_ushort(SOI_WSQ, wsq_data, wsq_alloc, &wsq_len))){
      free(qdata);
      free(wsq_data);
      return(ret);
   }

   /* Store the Wavelet filter taps to the WSQ buffer. */
   if((ret = putc_transform_table(lofilt, MAX_LOFILT,
                                 hifilt, MAX_HIFILT,
                                 wsq_data, wsq_alloc, &wsq_len))){
      free(qdata);
      free(wsq_data);
      return(ret);
   }

   /* Store the quantization parameters to the WSQ buffer. */
   if((ret = putc_quantization_table(&quant_vals,
                                    wsq_data, wsq_alloc, &wsq_len))){
      free(qdata);
      free(wsq_data);
      return(ret);
   }

   /* Store a frame header to the WSQ buffer. */
   if((ret = putc_frame_header_wsq(width, height, frm_header_wsq.m_shift, frm_header_wsq.r_scale,
                              wsq_data, wsq_alloc, &wsq_len))){
      free(qdata);
      free(wsq_data);
      return(ret);
   }

   if(debug > 0)
      fprintf(stderr, "SOI_WSQ, tables, and frame header written\n\n");

   /* Allocate a temporary buffer for holding compressed block data.    */
   /* This buffer is allocated to the size of the original input image, */
   /* and it is "assumed" that the compressed blocks will not exceed    */
   /* this buffer size.                                                 */
   huff_buf = (unsigned char *)malloc(num_pix);
   if(huff_buf == (unsigned char *)NULL) {
      free(qdata);
      free(wsq_data);
      fprintf(stderr, "ERROR : wsq_encode_1 : malloc : huff_buf\n");
      return(-13);
   }

   /******************/
   /* ENCODE Block 1 */
   /******************/
   /* Compute Huffman table for Block 1. */
   if((ret = gen_hufftable_wsq(&hufftable, &huffbits, &huffvalues,
                            qdata, &qsize1, 1))){
      free(qdata);
      free(wsq_data);
      free(huff_buf);
      return(ret);
   }

   /* Store Huffman table for Block 1 to WSQ buffer. */
   if((ret = putc_huffman_table(DHT_WSQ, 0, huffbits, huffvalues,
                               wsq_data, wsq_alloc, &wsq_len))){
      free(qdata);
      free(wsq_data);
      free(huff_buf);
      free(huffbits);
      free(huffvalues);
      free(hufftable);
      return(ret);
   }
   free(huffbits);
   free(huffvalues);

   if(debug > 0)
      fprintf(stderr, "Huffman code Table 1 generated and written\n\n");

   /* Compress Block 1 data. */
   if((ret = compress_block(huff_buf, &hsize1, qdata, qsize1,
                           MAX_HUFFCOEFF, MAX_HUFFZRUN, hufftable))){
      free(qdata);
      free(wsq_data);
      free(huff_buf);
      free(hufftable);
      return(ret);
   }
   /* Done with current Huffman table. */
   free(hufftable);

   /* Accumulate number of bytes compressed. */
   hsize = hsize1;

   /* Store Block 1's header to WSQ buffer. */
   if((ret = putc_block_header(0, wsq_data, wsq_alloc, &wsq_len))){
      free(qdata);
      free(wsq_data);
      free(huff_buf);
      return(ret);
   }

   /* Store Block 1's compressed data to WSQ buffer. */
   if((ret = putc_bytes(huff_buf, hsize1, wsq_data, wsq_alloc, &wsq_len))){
      free(qdata);
      free(wsq_data);
      free(huff_buf);
      return(ret);
   }

   if(debug > 0)
      fprintf(stderr, "Block 1 compressed and written\n\n");

   /******************/
   /* ENCODE Block 2 */
   /******************/
   /* Compute  Huffman table for Blocks 2 & 3. */
   block_sizes[0] = qsize2;
   block_sizes[1] = qsize3;
   if((ret = gen_hufftable_wsq(&hufftable, &huffbits, &huffvalues,
                          qdata+qsize1, block_sizes, 2))){
      free(qdata);
      free(wsq_data);
      free(huff_buf);
      return(ret);
   }

   /* Store Huffman table for Blocks 2 & 3 to WSQ buffer. */
   if((ret = putc_huffman_table(DHT_WSQ, 1, huffbits, huffvalues,
                               wsq_data, wsq_alloc, &wsq_len))){
      free(qdata);
      free(wsq_data);
      free(huff_buf);
      free(huffbits);
      free(huffvalues);
      free(hufftable);
      return(ret);
   }
   free(huffbits);
   free(huffvalues);

   if(debug > 0)
      fprintf(stderr, "Huffman code Table 2 generated and written\n\n");

   /* Compress Block 2 data. */
   if((ret = compress_block(huff_buf, &hsize2, qdata+qsize1, qsize2,
                           MAX_HUFFCOEFF, MAX_HUFFZRUN, hufftable))){
      free(qdata);
      free(wsq_data);
      free(huff_buf);
      free(hufftable);
      return(ret);
   }

   /* Accumulate number of bytes compressed. */
   hsize += hsize2;

   /* Store Block 2's header to WSQ buffer. */
   if((ret = putc_block_header(1, wsq_data, wsq_alloc, &wsq_len))){
      free(qdata);
      free(wsq_data);
      free(huff_buf);
      free(hufftable);
      return(ret);
   }

   /* Store Block 2's compressed data to WSQ buffer. */
   if((ret = putc_bytes(huff_buf, hsize2, wsq_data, wsq_alloc, &wsq_len))){
      free(qdata);
      free(wsq_data);
      free(huff_buf);
      free(hufftable);
      return(ret);
   }

   if(debug > 0)
      fprintf(stderr, "Block 2 compressed and written\n\n");

   /******************/
   /* ENCODE Block 3 */
   /******************/
   /* Compress Block 3 data. */
   if((ret = compress_block(huff_buf, &hsize3, qdata+qsize1+qsize2, qsize3,
                           MAX_HUFFCOEFF, MAX_HUFFZRUN, hufftable))){
      free(qdata);
      free(wsq_data);
      free(huff_buf);
      free(hufftable);
      return(ret);
   }
   /* Done with current Huffman table. */
   free(hufftable);

   /* Done with quantized image buffer. */
   free(qdata);

   /* Accumulate number of bytes compressed. */
   hsize += hsize3;

   /* Store Block 3's header to WSQ buffer. */
   if((ret = putc_block_header(1, wsq_data, wsq_alloc, &wsq_len))){
      free(wsq_data);
      free(huff_buf);
      return(ret);
   }

   /* Store Block 3's compressed data to WSQ buffer. */
   if((ret = putc_bytes(huff_buf, hsize3, wsq_data, wsq_alloc, &wsq_len))){
      free(wsq_data);
      free(huff_buf);
      return(ret);
   }

   if(debug > 0)
      fprintf(stderr, "Block 3 compressed and written\n\n");

   /* Done with huffman compressing blocks, so done with buffer. */
   free(huff_buf);

   /* Add a End Of Image (EOI_WSQ) marker to the WSQ buffer. */
   if((ret = putc_ushort(EOI_WSQ, wsq_data, wsq_alloc, &wsq_len))){
      free(wsq_data);
      return(ret);
   }

   if(debug > 0) {
      fprintf(stderr,
              "hsize1 = %d :: hsize2 = %d :: hsize3 = %d\n", hsize1, hsize2, hsize3);
      fprintf(stderr,"@ complen = %d :: ratio = %.1f\n", hsize,
                      (float)(num_pix)/(float)hsize);
   }

   *odata = wsq_data;
   *olen = wsq_len;

   /* Return normally. */
   return(0);
}


/************************************/
/* Routine to read specified table. */
/************************************/
static int read_table_wsq14(
   unsigned short marker,         /* WSQ marker */
   DTT_TABLE *dtt_table,  /* transform table structure */
   DQT_TABLE *dqt_table,  /* quantization table structure */
   DHT_TABLE *dht_table,  /* huffman table structure */
   FILE *infp)            /* input file */
{
   int ret;
   unsigned char *comment;

   switch(marker){
   case DTT_WSQ:
      if((ret = read_transform_table(dtt_table, infp)))
         return(ret);
      break;
   case DQT_WSQ:
      if((ret = read_quantization_table(dqt_table, infp)))
         return(ret);
      break;
   case DHT_WSQ:
      if((ret = read_huff_table_wsq14(dht_table, infp)))
         return(ret);
      break;
   case COM_WSQ:
      if((ret = read_comment(&comment, infp)))
         return(ret);
#ifdef PRINT_COMMENT
      fprintf(stderr, "COMMENT: %s\n", comment);
#endif
      free(comment);
      break;
   default:
      fprintf(stderr,"ERROR: read_table : Invalid table defined -> {%u}\n",
              marker);
      return(-75);
   }

   return(0);
}


/************************************************/
/* Routine to read in huffman table parameters. */
/************************************************/
static int read_huff_table_wsq14(
   DHT_TABLE *dht_table,  /* huffman table structure */
   FILE *infp)            /* input file */
{
   int ret;
   unsigned short hdr_size;       /* header size */
   unsigned short cnt, bytes_cnt; /* counters */
   unsigned short num_hufvals;    /* number of huffvalues */
   unsigned char table;           /* huffman table indicator */
   unsigned char char_dat;

   if(debug > 0)
      fprintf(stderr, "Reading huffman table.\n");

   bytes_cnt = 0;
   if((ret = read_ushort(&hdr_size, infp)))
      return(ret);
   bytes_cnt += 2;

   while(bytes_cnt != hdr_size) {
      if((ret = read_byte(&table, infp)))
         return(ret);

      if(debug > 2)
         fprintf(stderr, "table = %d\n", table);

      num_hufvals = 0;
      bytes_cnt += 33;
      for(cnt = 0; cnt < 16; cnt++) {
/* WSQ14 OLD SPEC USED 16bits NEW 8bits */
         if((ret = read_byte(&char_dat, infp)))
            return(ret);
         if((ret = read_byte(&char_dat, infp)))
            return(ret);
         (dht_table+table)->huffbits[cnt] = char_dat;

         if(debug > 2)
            fprintf(stderr,
                    "huffbits[%d] = %d\n",
                    cnt, (dht_table+table)->huffbits[cnt]);

         num_hufvals += (dht_table+table)->huffbits[cnt];
      }

      if(num_hufvals > MAX_HUFFCOUNTS_WSQ+1){
         fprintf(stderr, "ERROR : read_huff_table_wsq14 : ");
         fprintf(stderr, "num_hufvals (%d) is larger than", num_hufvals);
         fprintf(stderr, " MAX_HUFFCOUNTS_WSQ (%d)\n", MAX_HUFFCOUNTS_WSQ+1);
         return(-2);
      }
      bytes_cnt += 2 * num_hufvals;

      for(cnt = 0; cnt < num_hufvals; cnt++) {
/* WSQ14 OLD SPEC USED 16bits NEW 8bits */
         if((ret = read_byte(&char_dat, infp)))
            return(ret);
         if((ret = read_byte(&char_dat, infp)))
            return(ret);
         (dht_table+table)->huffvalues[cnt] = char_dat;

         if(debug > 2)
            fprintf(stderr,
                    "huffvalues[%d] = %d\n",
                    cnt, (dht_table+table)->huffvalues[cnt]);

      }

      (dht_table+table)->tabdef = 1;
   }

   if(debug > 0)
      fprintf(stderr,
              "Finished reading huffman table.\n\n");

      return(0);
}


/************************************************************************/
/* Build WSQ decomposition trees.                                       */
/************************************************************************/
static void build_wsq_trees_wsq14(W_TREE w_tree[], const int w_treelen,
                     Q_TREE q_tree[], const int q_treelen,
                     const int width, const int height)
{
   int i;
   W_TREE w_tree_wsq14[W_TREELEN];

   /* This builds the old format of trees. */
   /* Build a W-TREE structure for the image. */
   build_w_tree_wsq14(w_tree_wsq14, width, height);
   /* Build a Q-TREE structure for the image. */
   build_q_tree_wsq14(w_tree_wsq14, q_tree);

   /* This builds the new certifiable trees. */
   for(i = 0; i < w_treelen; i++) {
      w_tree[i].x = w_tree_wsq14[i].x;
      w_tree[i].y = w_tree_wsq14[i].y;
      w_tree[i].lenx = w_tree_wsq14[i].lenx;
      w_tree[i].leny = w_tree_wsq14[i].leny;
      w_tree[i].inv_rw = 0;
      w_tree[i].inv_cl = 0;
   }
/*
      for(i = 0; i < w_treelen; i++)
         fprintf(stderr,
         "t%d -> x = %d  y = %d : dx = %d  dy = %d : ir = %d  ic = %d\n",
         i, w_tree[i].x, w_tree[i].y, w_tree[i].lenx, w_tree[i].leny,
         w_tree[i].inv_rw, w_tree[i].inv_cl);
      fprintf(stderr, "\n\n");
      for(i = 0; i < NUM_SUBBANDS; i++)
         fprintf(stderr, "t%d -> x = %d  y = %d : lx = %d  ly = %d\n",
         i, q_tree[i].x, q_tree[i].y, q_tree[i].lenx, q_tree[i].leny);
      fprintf(stderr, "\n\n");
*/
}


/*********************************************************/
/* Build new format Wavelet tree, and new and old format */
/* Quantization trees.                                   */
/*********************************************************/
static void build_shuffle_trees_wsq14(W_TREE w_tree[], const int w_treelen,
                     Q_TREE q_tree[], const int q_treelen,
                     Q_TREE q_tree_wsq14[], const int q_treelen_wsq14,
                     const int width, const int height)
{
   int i;

   /* These build the certifiable versions of the trees. */
   /* Build a W-TREE structure for the image. */
   build_w_tree(w_tree, width, height);
   /* Build a Q-TREE structure for the image. */
   build_q_tree(w_tree, q_tree);

   /* These build the old format versions of the trees. */
   for(i = 0; i < 7; i++) {
      q_tree_wsq14[i].x = q_tree[i].x;
      q_tree_wsq14[i].y = q_tree[i].y;
      q_tree_wsq14[i].lenx = q_tree[i].lenx;
      q_tree_wsq14[i].leny = q_tree[i].leny;
   }

   q_tree_wsq14[7].x = q_tree[8].x;
   q_tree_wsq14[7].y = q_tree[8].y;
   q_tree_wsq14[7].lenx = q_tree[8].lenx;
   q_tree_wsq14[7].leny = q_tree[8].leny;
   q_tree_wsq14[8].x = q_tree[7].x;
   q_tree_wsq14[8].y = q_tree[7].y;
   q_tree_wsq14[8].lenx = q_tree[7].lenx;
   q_tree_wsq14[8].leny = q_tree[7].leny;
   q_tree_wsq14[9].x = q_tree[10].x;
   q_tree_wsq14[9].y = q_tree[10].y;
   q_tree_wsq14[9].lenx = q_tree[10].lenx;
   q_tree_wsq14[9].leny = q_tree[10].leny;
   q_tree_wsq14[10].x = q_tree[9].x;
   q_tree_wsq14[10].y = q_tree[9].y;
   q_tree_wsq14[10].lenx = q_tree[9].lenx;
   q_tree_wsq14[10].leny = q_tree[9].leny;

   q_tree_wsq14[11].x = q_tree[13].x;
   q_tree_wsq14[11].y = q_tree[13].y;
   q_tree_wsq14[11].lenx = q_tree[13].lenx;
   q_tree_wsq14[11].leny = q_tree[13].leny;
   q_tree_wsq14[12].x = q_tree[14].x;
   q_tree_wsq14[12].y = q_tree[14].y;
   q_tree_wsq14[12].lenx = q_tree[14].lenx;
   q_tree_wsq14[12].leny = q_tree[14].leny;
   q_tree_wsq14[13].x = q_tree[11].x;
   q_tree_wsq14[13].y = q_tree[11].y;
   q_tree_wsq14[13].lenx = q_tree[11].lenx;
   q_tree_wsq14[13].leny = q_tree[11].leny;
   q_tree_wsq14[14].x = q_tree[12].x;
   q_tree_wsq14[14].y = q_tree[12].y;
   q_tree_wsq14[14].lenx = q_tree[12].lenx;
   q_tree_wsq14[14].leny = q_tree[12].leny;

   q_tree_wsq14[15].x = q_tree[18].x;
   q_tree_wsq14[15].y = q_tree[18].y;
   q_tree_wsq14[15].lenx = q_tree[18].lenx;
   q_tree_wsq14[15].leny = q_tree[18].leny;
   q_tree_wsq14[16].x = q_tree[17].x;
   q_tree_wsq14[16].y = q_tree[17].y;
   q_tree_wsq14[16].lenx = q_tree[17].lenx;
   q_tree_wsq14[16].leny = q_tree[17].leny;
   q_tree_wsq14[17].x = q_tree[16].x;
   q_tree_wsq14[17].y = q_tree[16].y;
   q_tree_wsq14[17].lenx = q_tree[16].lenx;
   q_tree_wsq14[17].leny = q_tree[16].leny;
   q_tree_wsq14[18].x = q_tree[15].x;
   q_tree_wsq14[18].y = q_tree[15].y;
   q_tree_wsq14[18].lenx = q_tree[15].lenx;
   q_tree_wsq14[18].leny = q_tree[15].leny;

   q_tree_wsq14[19].x = q_tree[23].x;
   q_tree_wsq14[19].y = q_tree[23].y;
   q_tree_wsq14[19].lenx = q_tree[23].lenx;
   q_tree_wsq14[19].leny = q_tree[23].leny;
   q_tree_wsq14[20].x = q_tree[24].x;
   q_tree_wsq14[20].y = q_tree[24].y;
   q_tree_wsq14[20].lenx = q_tree[24].lenx;
   q_tree_wsq14[20].leny = q_tree[24].leny;
   q_tree_wsq14[21].x = q_tree[25].x;
   q_tree_wsq14[21].y = q_tree[25].y;
   q_tree_wsq14[21].lenx = q_tree[25].lenx;
   q_tree_wsq14[21].leny = q_tree[25].leny;
   q_tree_wsq14[22].x = q_tree[26].x;
   q_tree_wsq14[22].y = q_tree[26].y;
   q_tree_wsq14[22].lenx = q_tree[26].lenx;
   q_tree_wsq14[22].leny = q_tree[26].leny;

   q_tree_wsq14[23].x = q_tree[20].x;
   q_tree_wsq14[23].y = q_tree[20].y;
   q_tree_wsq14[23].lenx = q_tree[20].lenx;
   q_tree_wsq14[23].leny = q_tree[20].leny;
   q_tree_wsq14[24].x = q_tree[19].x;
   q_tree_wsq14[24].y = q_tree[19].y;
   q_tree_wsq14[24].lenx = q_tree[19].lenx;
   q_tree_wsq14[24].leny = q_tree[19].leny;
   q_tree_wsq14[25].x = q_tree[22].x;
   q_tree_wsq14[25].y = q_tree[22].y;
   q_tree_wsq14[25].lenx = q_tree[22].lenx;
   q_tree_wsq14[25].leny = q_tree[22].leny;
   q_tree_wsq14[26].x = q_tree[21].x;
   q_tree_wsq14[26].y = q_tree[21].y;
   q_tree_wsq14[26].lenx = q_tree[21].lenx;
   q_tree_wsq14[26].leny = q_tree[21].leny;

   q_tree_wsq14[27].x = q_tree[33].x;
   q_tree_wsq14[27].y = q_tree[33].y;
   q_tree_wsq14[27].lenx = q_tree[33].lenx;
   q_tree_wsq14[27].leny = q_tree[33].leny;
   q_tree_wsq14[28].x = q_tree[34].x;
   q_tree_wsq14[28].y = q_tree[34].y;
   q_tree_wsq14[28].lenx = q_tree[34].lenx;
   q_tree_wsq14[28].leny = q_tree[34].leny;
   q_tree_wsq14[29].x = q_tree[31].x;
   q_tree_wsq14[29].y = q_tree[31].y;
   q_tree_wsq14[29].lenx = q_tree[31].lenx;
   q_tree_wsq14[29].leny = q_tree[31].leny;
   q_tree_wsq14[30].x = q_tree[32].x;
   q_tree_wsq14[30].y = q_tree[32].y;
   q_tree_wsq14[30].lenx = q_tree[32].lenx;
   q_tree_wsq14[30].leny = q_tree[32].leny;

   q_tree_wsq14[31].x = q_tree[30].x;
   q_tree_wsq14[31].y = q_tree[30].y;
   q_tree_wsq14[31].lenx = q_tree[30].lenx;
   q_tree_wsq14[31].leny = q_tree[30].leny;
   q_tree_wsq14[32].x = q_tree[29].x;
   q_tree_wsq14[32].y = q_tree[29].y;
   q_tree_wsq14[32].lenx = q_tree[29].lenx;
   q_tree_wsq14[32].leny = q_tree[29].leny;
   q_tree_wsq14[33].x = q_tree[28].x;
   q_tree_wsq14[33].y = q_tree[28].y;
   q_tree_wsq14[33].lenx = q_tree[28].lenx;
   q_tree_wsq14[33].leny = q_tree[28].leny;
   q_tree_wsq14[34].x = q_tree[27].x;
   q_tree_wsq14[34].y = q_tree[27].y;
   q_tree_wsq14[34].lenx = q_tree[27].lenx;
   q_tree_wsq14[34].leny = q_tree[27].leny;

   q_tree_wsq14[35].x = q_tree[43].x;
   q_tree_wsq14[35].y = q_tree[43].y;
   q_tree_wsq14[35].lenx = q_tree[43].lenx;
   q_tree_wsq14[35].leny = q_tree[43].leny;
   q_tree_wsq14[36].x = q_tree[44].x;
   q_tree_wsq14[36].y = q_tree[44].y;
   q_tree_wsq14[36].lenx = q_tree[44].lenx;
   q_tree_wsq14[36].leny = q_tree[44].leny;
   q_tree_wsq14[37].x = q_tree[45].x;
   q_tree_wsq14[37].y = q_tree[45].y;
   q_tree_wsq14[37].lenx = q_tree[45].lenx;
   q_tree_wsq14[37].leny = q_tree[45].leny;
   q_tree_wsq14[38].x = q_tree[46].x;
   q_tree_wsq14[38].y = q_tree[46].y;
   q_tree_wsq14[38].lenx = q_tree[46].lenx;
   q_tree_wsq14[38].leny = q_tree[46].leny;

   q_tree_wsq14[39].x = q_tree[48].x;
   q_tree_wsq14[39].y = q_tree[48].y;
   q_tree_wsq14[39].lenx = q_tree[48].lenx;
   q_tree_wsq14[39].leny = q_tree[48].leny;
   q_tree_wsq14[40].x = q_tree[47].x;
   q_tree_wsq14[40].y = q_tree[47].y;
   q_tree_wsq14[40].lenx = q_tree[47].lenx;
   q_tree_wsq14[40].leny = q_tree[47].leny;
   q_tree_wsq14[41].x = q_tree[50].x;
   q_tree_wsq14[41].y = q_tree[50].y;
   q_tree_wsq14[41].lenx = q_tree[50].lenx;
   q_tree_wsq14[41].leny = q_tree[50].leny;
   q_tree_wsq14[42].x = q_tree[49].x;
   q_tree_wsq14[42].y = q_tree[49].y;
   q_tree_wsq14[42].lenx = q_tree[49].lenx;
   q_tree_wsq14[42].leny = q_tree[49].leny;

   q_tree_wsq14[43].x = q_tree[37].x;
   q_tree_wsq14[43].y = q_tree[37].y;
   q_tree_wsq14[43].lenx = q_tree[37].lenx;
   q_tree_wsq14[43].leny = q_tree[37].leny;
   q_tree_wsq14[44].x = q_tree[38].x;
   q_tree_wsq14[44].y = q_tree[38].y;
   q_tree_wsq14[44].lenx = q_tree[38].lenx;
   q_tree_wsq14[44].leny = q_tree[38].leny;
   q_tree_wsq14[45].x = q_tree[35].x;
   q_tree_wsq14[45].y = q_tree[35].y;
   q_tree_wsq14[45].lenx = q_tree[35].lenx;
   q_tree_wsq14[45].leny = q_tree[35].leny;
   q_tree_wsq14[46].x = q_tree[36].x;
   q_tree_wsq14[46].y = q_tree[36].y;
   q_tree_wsq14[46].lenx = q_tree[36].lenx;
   q_tree_wsq14[46].leny = q_tree[36].leny;

   q_tree_wsq14[47].x = q_tree[42].x;
   q_tree_wsq14[47].y = q_tree[42].y;
   q_tree_wsq14[47].lenx = q_tree[42].lenx;
   q_tree_wsq14[47].leny = q_tree[42].leny;
   q_tree_wsq14[48].x = q_tree[41].x;
   q_tree_wsq14[48].y = q_tree[41].y;
   q_tree_wsq14[48].lenx = q_tree[41].lenx;
   q_tree_wsq14[48].leny = q_tree[41].leny;
   q_tree_wsq14[49].x = q_tree[40].x;
   q_tree_wsq14[49].y = q_tree[40].y;
   q_tree_wsq14[49].lenx = q_tree[40].lenx;
   q_tree_wsq14[49].leny = q_tree[40].leny;
   q_tree_wsq14[50].x = q_tree[39].x;
   q_tree_wsq14[50].y = q_tree[39].y;
   q_tree_wsq14[50].lenx = q_tree[39].lenx;
   q_tree_wsq14[50].leny = q_tree[39].leny;

   q_tree_wsq14[51].x = q_tree[51].x;
   q_tree_wsq14[51].y = q_tree[51].y;
   q_tree_wsq14[51].lenx = q_tree[51].lenx;
   q_tree_wsq14[51].leny = q_tree[51].leny;

   q_tree_wsq14[52].x = q_tree[53].x;
   q_tree_wsq14[52].y = q_tree[53].y;
   q_tree_wsq14[52].lenx = q_tree[53].lenx;
   q_tree_wsq14[52].leny = q_tree[53].leny;
   q_tree_wsq14[53].x = q_tree[52].x;
   q_tree_wsq14[53].y = q_tree[52].y;
   q_tree_wsq14[53].lenx = q_tree[52].lenx;
   q_tree_wsq14[53].leny = q_tree[52].leny;
   q_tree_wsq14[54].x = q_tree[55].x;
   q_tree_wsq14[54].y = q_tree[55].y;
   q_tree_wsq14[54].lenx = q_tree[55].lenx;
   q_tree_wsq14[54].leny = q_tree[55].leny;
   q_tree_wsq14[55].x = q_tree[54].x;
   q_tree_wsq14[55].y = q_tree[54].y;
   q_tree_wsq14[55].lenx = q_tree[54].lenx;
   q_tree_wsq14[55].leny = q_tree[54].leny;

   q_tree_wsq14[56].x = q_tree[58].x;
   q_tree_wsq14[56].y = q_tree[58].y;
   q_tree_wsq14[56].lenx = q_tree[58].lenx;
   q_tree_wsq14[56].leny = q_tree[58].leny;
   q_tree_wsq14[57].x = q_tree[59].x;
   q_tree_wsq14[57].y = q_tree[59].y;
   q_tree_wsq14[57].lenx = q_tree[59].lenx;
   q_tree_wsq14[57].leny = q_tree[59].leny;
   q_tree_wsq14[58].x = q_tree[56].x;
   q_tree_wsq14[58].y = q_tree[56].y;
   q_tree_wsq14[58].lenx = q_tree[56].lenx;
   q_tree_wsq14[58].leny = q_tree[56].leny;
   q_tree_wsq14[59].x = q_tree[57].x;
   q_tree_wsq14[59].y = q_tree[57].y;
   q_tree_wsq14[59].lenx = q_tree[57].lenx;
   q_tree_wsq14[59].leny = q_tree[57].leny;

   q_tree_wsq14[60].x = q_tree[63].x;
   q_tree_wsq14[60].y = q_tree[63].y;
   q_tree_wsq14[60].lenx = q_tree[63].lenx;
   q_tree_wsq14[60].leny = q_tree[63].leny;
   q_tree_wsq14[61].x = q_tree[62].x;
   q_tree_wsq14[61].y = q_tree[62].y;
   q_tree_wsq14[61].lenx = q_tree[62].lenx;
   q_tree_wsq14[61].leny = q_tree[62].leny;
   q_tree_wsq14[62].x = q_tree[61].x;
   q_tree_wsq14[62].y = q_tree[61].y;
   q_tree_wsq14[62].lenx = q_tree[61].lenx;
   q_tree_wsq14[62].leny = q_tree[61].leny;
   q_tree_wsq14[63].x = q_tree[60].x;
   q_tree_wsq14[63].y = q_tree[60].y;
   q_tree_wsq14[63].lenx = q_tree[60].lenx;
   q_tree_wsq14[63].leny = q_tree[60].leny;

/*
      for(i = 0; i < q_treelen; i++) {
         fprintf(stderr, "t%d -> x = %d  y = %d : lx = %d  ly = %d\n",
         i, q_tree[i].x, q_tree[i].y, q_tree[i].lenx, q_tree[i].leny);
         fprintf(stderr, "t%d -> x = %d  y = %d : lx = %d  ly = %d\n",
         i, q_tree_wsq14[i].x, q_tree_wsq14[i].y, q_tree_wsq14[i].lenx, q_tree_wsq14[i].leny);
      }
      fprintf(stderr, "\n\n");
*/
}


/********************************************************************/
/* Routine to decode an entire "block" of encoded data from a file. */
/********************************************************************/
static int huffman_decode_data_file_wsq14(
   short *ip,             /* image pointer */
   DTT_TABLE *dtt_table,  /*transform table pointer */
   DQT_TABLE *dqt_table,  /* quantization table */
   DHT_TABLE *dht_table,  /* huffman table */
   FILE *infp)            /* input file */
{
   int ret;
   int blk = 0;           /* block number */
   unsigned short marker;         /* WSQ markers */
   int bit_count;         /* bit count for nextbits_wsq routine */
   int n;                 /* zero run count */
   int nodeptr;           /* pointers for decoding */
   int last_size;         /* last huffvalue */
   unsigned char hufftable_id;    /* huffman table number */
   HUFFCODE *hufftable;   /* huffman code structure */
   int maxcode[MAX_HUFFBITS+1];       /* used in decoding data */
   int mincode[MAX_HUFFBITS+1];       /* used in decoding data */
   int valptr[MAX_HUFFBITS+1];        /* used in decoding data */
   unsigned short tbits;


   if((ret = read_marker_wsq(&marker, TBLS_N_SOB, infp)))
      return(ret);

   bit_count = 0;

   while(marker != EOI_WSQ) {

      if(marker != 0) {
         blk++;
         while(marker != SOB_WSQ) {
            if((ret = read_table_wsq14(marker, dtt_table, dqt_table,
                                dht_table, infp)))
               return(ret);
            if((ret = read_marker_wsq(&marker, TBLS_N_SOB, infp)))
               return(ret);
         }
         if((ret = read_block_header(&hufftable_id, infp)))
            return(ret);

         if((dht_table+hufftable_id)->tabdef != 1) {
            fprintf(stderr, "ERROR : huffman_decode_data_file : ");
            fprintf(stderr, "huffman table {%d} undefined.\n", hufftable_id);
            return(-53);
         }

         /* the next two routines reconstruct the huffman tables */
         if((ret = build_huffsizes(&hufftable, &last_size,
                       (dht_table+hufftable_id)->huffbits, MAX_HUFFCOUNTS_WSQ)))
            return(ret);
         build_huffcodes(hufftable);
         ret = check_huffcodes_wsq(hufftable, last_size);

         /* this routine builds a set of three tables used in decoding */
         /* the compressed data*/
         gen_decode_table(hufftable, maxcode, mincode, valptr,
                          (dht_table+hufftable_id)->huffbits);
         free(hufftable);
         bit_count = 0;
         marker = 0;
      }

      /* get next huffman category code from compressed input data stream */
      if((ret = decode_data_file(&nodeptr, mincode, maxcode, valptr,
                            (dht_table+hufftable_id)->huffvalues,
                            infp, &bit_count, &marker)))
         return(ret);

      if(nodeptr == -1)
         continue;

      if(nodeptr <= 100)
         for(n = 0; n < nodeptr; n++) {
            *ip++ = 0; /* z run */
         }
      else if(nodeptr > 106)
         *ip++ = nodeptr - 180;
      else if(nodeptr == 101){
         if((ret = nextbits_wsq(&tbits, &marker, infp, &bit_count, 8)))
            return(ret);
         *ip++ = tbits;
      }
      else if(nodeptr == 102){
         if((ret = nextbits_wsq(&tbits, &marker, infp, &bit_count, 8)))
            return(ret);
         *ip++ = -tbits;
      }
      else if(nodeptr == 103){
         if((ret = nextbits_wsq(&tbits, &marker, infp, &bit_count, 16)))
            return(ret);
         *ip++ = tbits;
      }
      else if(nodeptr == 104){
         if((ret = nextbits_wsq(&tbits, &marker, infp, &bit_count, 16)))
            return(ret);
         *ip++ = -tbits;
      }
      else if(nodeptr == 105) {
         if((ret = nextbits_wsq(&tbits, &marker, infp, &bit_count, 8)))
            return(ret);
         n = tbits;
         while(n--)
            *ip++ = 0;
      }
      else if(nodeptr == 106) {
         if((ret = nextbits_wsq(&tbits, &marker, infp, &bit_count, 16)))
            return(ret);
         n = tbits;
         while(n--)
            *ip++ = 0;
      }
      else {
         fprintf(stderr, 
                "ERROR: huffman_decode_data_file : Invalid code %d (%x).\n",
                nodeptr, nodeptr);
         return(-54);
      }
   }

   return(0);
}


/****************************************************/
/* Routine to unshuffle WSQ14 (old format) Subbands */
/****************************************************/
static int unshuffle_wsq14(
   short **ofip,         /* floating point image pointer         */
   const DQT_TABLE *dqt_table, /* quantization table structure   */
   Q_TREE q_tree[],      /* quantization table structure         */
   const int q_treelen,  /* size of q_tree                       */
   short *sip,           /* quantized image pointer              */
   const int width,      /* image width                          */
   const int height)     /* image height                         */
{
   short *fip;    /* floating point image */
   int row, col;  /* cover counter and row/column counters */
   short *fptr;   /* image pointers */
   short *sptr;
   int cnt;       /* subband counter */

   if((fip = (short *) calloc(width*height, sizeof(short))) == NULL) {
      fprintf(stderr,"ERROR : unquantize : calloc : fip\n");
      return(-2);
   }
   if(dqt_table->dqt_def != 1) {
      fprintf(stderr,
      "ERROR: unshuffle_wsq14 : quantization table parameters not defined!\n");
      return(-3);
   }

   sptr = sip;
   for(cnt = 0; cnt < NUM_SUBBANDS; cnt++)
      if(dqt_table->q_bin[cnt] != 0.0) {
         fptr = fip + (q_tree[cnt].y * width) + q_tree[cnt].x;

         for(row = 0; row < q_tree[cnt].leny; row++, fptr += width - q_tree[cnt].lenx)
            for(col = 0; col < q_tree[cnt].lenx; col++)
               *fptr++ = *sptr++;
      }

   *ofip = fip;
   return(0);
}


/***************************************************************/
/* Routine shuffles quantized subbands from old to new format. */
/***************************************************************/
static int shuffle_wsq14(
   short **osip,           /* quantized output             */
   int *ocmp_siz,          /* size of quantized output     */
   const DQT_TABLE dqt_table, /* quantization table structure   */
   Q_TREE q_tree_wsq14[],   /* quantization "tree"          */
   const int q_treelen,    /* size of q_tree               */
   short *fip,             /* floating point image pointer */
   const int width,        /* image width                  */
   const int height)       /* image height                 */
{
   short *fptr;           /* temp image pointer */
   short *sip, *sptr;     /* pointers to quantized image */
   int row, col;          /* temp image characteristic parameters */
   int cnt;               /* subband counter */

   if(dqt_table.dqt_def != 1) {
      fprintf(stderr,
      "ERROR: shuffle_wsq14 : quantization table parameters not defined!\n");
      return(-92);
   }

   /* Set up output buffer. */
   if((sip = (short *) calloc(width*height, sizeof(short))) == NULL) {
      fprintf(stderr,"ERROR : quantize : calloc : sip\n");
      return(-90);
   }
   sptr = sip;

   for(cnt = 0; cnt < NUM_SUBBANDS; cnt++)

      if(dqt_table.q_bin[cnt] != 0.0) {
         fptr = fip + (q_tree_wsq14[cnt].y * width) + q_tree_wsq14[cnt].x;

         for(row = 0; row < q_tree_wsq14[cnt].leny; row++, fptr += width - q_tree_wsq14[cnt].lenx)
            for(col = 0; col < q_tree_wsq14[cnt].lenx; col++)
               *sptr++ = *fptr++;
      }

   *osip = sip;
   *ocmp_siz = sptr - sip;
   return(0);
}


/**************************************************************/
/* Routine shuffles quantization tree from old to new format. */
/**************************************************************/
static int shuffle_dqt_wsq14(DQT_TABLE *dqt_table)
{
   int i;
   float tq[MAX_SUBBANDS], tz[MAX_SUBBANDS];

   for(i = 0; i < 7; i++) {
      tq[i] = dqt_table->q_bin[i];
      tz[i] = dqt_table->z_bin[i];
   }

   tq[7] = dqt_table->q_bin[8];
   tq[8] = dqt_table->q_bin[7];
   tq[9] = dqt_table->q_bin[10];
   tq[10] = dqt_table->q_bin[9];
   tq[11] = dqt_table->q_bin[13];
   tq[12] = dqt_table->q_bin[14];
   tq[13] = dqt_table->q_bin[11];
   tq[14] = dqt_table->q_bin[12];
   tq[15] = dqt_table->q_bin[18];
   tq[16] = dqt_table->q_bin[17];
   tq[17] = dqt_table->q_bin[16];
   tq[18] = dqt_table->q_bin[15];
   tq[19] = dqt_table->q_bin[23];
   tq[20] = dqt_table->q_bin[24];
   tq[21] = dqt_table->q_bin[25];
   tq[22] = dqt_table->q_bin[26];
   tq[23] = dqt_table->q_bin[20];
   tq[24] = dqt_table->q_bin[19];
   tq[25] = dqt_table->q_bin[22];
   tq[26] = dqt_table->q_bin[21];
   tq[27] = dqt_table->q_bin[33];
   tq[28] = dqt_table->q_bin[34];
   tq[29] = dqt_table->q_bin[31];
   tq[30] = dqt_table->q_bin[32];
   tq[31] = dqt_table->q_bin[30];
   tq[32] = dqt_table->q_bin[29];
   tq[33] = dqt_table->q_bin[28];
   tq[34] = dqt_table->q_bin[27];
   tq[35] = dqt_table->q_bin[43];
   tq[36] = dqt_table->q_bin[44];
   tq[37] = dqt_table->q_bin[45];
   tq[38] = dqt_table->q_bin[46];
   tq[39] = dqt_table->q_bin[48];
   tq[40] = dqt_table->q_bin[47];
   tq[41] = dqt_table->q_bin[50];
   tq[42] = dqt_table->q_bin[49];
   tq[43] = dqt_table->q_bin[37];
   tq[44] = dqt_table->q_bin[38];
   tq[45] = dqt_table->q_bin[35];
   tq[46] = dqt_table->q_bin[36];
   tq[47] = dqt_table->q_bin[42];
   tq[48] = dqt_table->q_bin[41];
   tq[49] = dqt_table->q_bin[40];
   tq[50] = dqt_table->q_bin[39];
   tq[51] = dqt_table->q_bin[51];
   tq[52] = dqt_table->q_bin[53];
   tq[53] = dqt_table->q_bin[52];
   tq[54] = dqt_table->q_bin[55];
   tq[55] = dqt_table->q_bin[54];
   tq[56] = dqt_table->q_bin[58];
   tq[57] = dqt_table->q_bin[59];
   tq[58] = dqt_table->q_bin[56];
   tq[59] = dqt_table->q_bin[57];
   tq[60] = dqt_table->q_bin[63];
   tq[61] = dqt_table->q_bin[62];
   tq[62] = dqt_table->q_bin[61];
   tq[63] = dqt_table->q_bin[60];

   tz[7] = dqt_table->z_bin[8];
   tz[8] = dqt_table->z_bin[7];
   tz[9] = dqt_table->z_bin[10];
   tz[10] = dqt_table->z_bin[9];
   tz[11] = dqt_table->z_bin[13];
   tz[12] = dqt_table->z_bin[14];
   tz[13] = dqt_table->z_bin[11];
   tz[14] = dqt_table->z_bin[12];
   tz[15] = dqt_table->z_bin[18];
   tz[16] = dqt_table->z_bin[17];
   tz[17] = dqt_table->z_bin[16];
   tz[18] = dqt_table->z_bin[15];
   tz[19] = dqt_table->z_bin[23];
   tz[20] = dqt_table->z_bin[24];
   tz[21] = dqt_table->z_bin[25];
   tz[22] = dqt_table->z_bin[26];
   tz[23] = dqt_table->z_bin[20];
   tz[24] = dqt_table->z_bin[19];
   tz[25] = dqt_table->z_bin[22];
   tz[26] = dqt_table->z_bin[21];
   tz[27] = dqt_table->z_bin[33];
   tz[28] = dqt_table->z_bin[34];
   tz[29] = dqt_table->z_bin[31];
   tz[30] = dqt_table->z_bin[32];
   tz[31] = dqt_table->z_bin[30];
   tz[32] = dqt_table->z_bin[29];
   tz[33] = dqt_table->z_bin[28];
   tz[34] = dqt_table->z_bin[27];
   tz[35] = dqt_table->z_bin[43];
   tz[36] = dqt_table->z_bin[44];
   tz[37] = dqt_table->z_bin[45];
   tz[38] = dqt_table->z_bin[46];
   tz[39] = dqt_table->z_bin[48];
   tz[40] = dqt_table->z_bin[47];
   tz[41] = dqt_table->z_bin[50];
   tz[42] = dqt_table->z_bin[49];
   tz[43] = dqt_table->z_bin[37];
   tz[44] = dqt_table->z_bin[38];
   tz[45] = dqt_table->z_bin[35];
   tz[46] = dqt_table->z_bin[36];
   tz[47] = dqt_table->z_bin[42];
   tz[48] = dqt_table->z_bin[41];
   tz[49] = dqt_table->z_bin[40];
   tz[50] = dqt_table->z_bin[39];
   tz[51] = dqt_table->z_bin[51];
   tz[52] = dqt_table->z_bin[53];
   tz[53] = dqt_table->z_bin[52];
   tz[54] = dqt_table->z_bin[55];
   tz[55] = dqt_table->z_bin[54];
   tz[56] = dqt_table->z_bin[58];
   tz[57] = dqt_table->z_bin[59];
   tz[58] = dqt_table->z_bin[56];
   tz[59] = dqt_table->z_bin[57];
   tz[60] = dqt_table->z_bin[63];
   tz[61] = dqt_table->z_bin[62];
   tz[62] = dqt_table->z_bin[61];
   tz[63] = dqt_table->z_bin[60];

   for(i = 0; i < MAX_SUBBANDS; i++) {
      dqt_table->q_bin[i] = tq[i];
      dqt_table->z_bin[i] = tz[i];
   }
   return(0);
}


/************************************************************/
/* Routine to obtain old format subband "x-y locations" for */
/* creating wavelets.                                       */
/************************************************************/
static void build_w_tree_wsq14(W_TREE w_tree[],
                        const int width, const int height)
{

   /* This table gives the location where the splitting of particular
      subbands will occur (upper left hand corner of named subband)
      relative to the specs subband numbering system (i.e. 0-63).  It
      also gives the subbands created by the given split.

      w_tree[?]     uppper left of this subband   subbands created
      ---------     ---------------------------   ----------------
          0                      0
          1                      0                51
          2                     52                52, 53, 54, 55
          3                     56                56, 57, 58, 59
          4                     19
          5                     35
          6                     19                19, 20, 21, 22
          7                     23                23, 24, 25, 26
          8                     27                27, 28, 29, 30
          9                     31                31, 32, 33, 34
         10                     35                35, 36, 37, 38
         11                     39                39, 40, 41, 42
         12                     43                43, 44, 45, 46
         13                     47                47, 48, 49, 50
         14                      0
         15                      0                4, 5, 6
         16                      7                7, 8, 9, 10
         17                     11                11, 12, 13, 14
         18                     15                15, 16, 17, 18
         19                      0                0, 1, 2, 3       */


   int lenx, lenx2, leny, leny2;	/* starting lengths of sections of
                                           the image being split into
                                           subbands */

   w_tree4_wsq14(w_tree, 0, 1, width, height, 0, 0, 1);

   if((w_tree[1].lenx % 2) == 0) {
      lenx = w_tree[1].lenx / 2;
      lenx2 = lenx;
   }
   else {
      lenx = (w_tree[1].lenx + 1) / 2;
      lenx2 = lenx - 1;
   }

   if((w_tree[1].leny % 2) == 0) {
      leny = w_tree[1].leny / 2;
      leny2 = leny;
   }
   else {
      leny = (w_tree[1].leny + 1) / 2;
      leny2 = leny - 1;
   }

   w_tree4_wsq14(w_tree, 4, 6, lenx2, leny, lenx, 0, 0);
   w_tree4_wsq14(w_tree, 5, 10, lenx, leny2, 0, leny, 0);
   w_tree4_wsq14(w_tree, 14, 15, lenx, leny, 0, 0, 0);

   w_tree[19].x = 0;
   w_tree[19].y = 0;
   if((w_tree[15].lenx % 2) == 0)
      w_tree[19].lenx = w_tree[15].lenx / 2;
   else
      w_tree[19].lenx = (w_tree[15].lenx + 1) / 2;

   if((w_tree[15].leny % 2) == 0)
      w_tree[19].leny = w_tree[15].leny / 2;
   else
      w_tree[19].leny = (w_tree[15].leny + 1) / 2;

   return;
}


/*********************************************************************/
/* Gives location and size of subband splits for build_w_tree_wsq14. */
/*********************************************************************/
static void w_tree4_wsq14(
   W_TREE w_tree[],	/* wavelet tree structure */
   int start1,		/* w_tree locations to start calculating */
   int start2,          /*    subband split locations and sizes  */
   int lenx,            /* (temp) subband split location and sizes */
   int leny,
   int x,
   int y,	
   int stop1)           /* 0 normal operation, 1 used to avoid
                           marking size and location of subbands
                           60-63 */
{
   int evenx, eveny;	/* Check length of subband for even or odd */
   int p1, p2;		/* w_tree locations for storing subband sizes and
                           locations */
   
   p1 = start1;
   p2 = start2;

   evenx = lenx % 2;
   eveny = leny % 2;

   w_tree[p1].x = x;
   w_tree[p1].y = y;
   w_tree[p1].lenx = lenx;
   w_tree[p1].leny = leny;
   
   w_tree[p2].x = x;
   w_tree[p2+2].x = x;
   w_tree[p2].y = y;
   w_tree[p2+1].y = y;

   if(evenx == 0) {
      w_tree[p2].lenx = lenx / 2;
      w_tree[p2+1].lenx = w_tree[p2].lenx;
      if(stop1 == 0)
         w_tree[p2+3].lenx = w_tree[p2].lenx;
   }
   else {
      w_tree[p2].lenx = (lenx +1) / 2;
      w_tree[p2+1].lenx = w_tree[p2].lenx - 1;
      if(stop1 == 0)
         w_tree[p2+3].lenx = w_tree[p2+1].lenx;
   }
   w_tree[p2+1].x = w_tree[p2].lenx + x;
   if(stop1 == 0)
      w_tree[p2+3].x = w_tree[p2+1].x;
   w_tree[p2+2].lenx = w_tree[p2].lenx;


   if(eveny == 0) {
      w_tree[p2].leny = leny / 2;
      w_tree[p2+2].leny = w_tree[p2].leny;
      if(stop1 == 0)
         w_tree[p2+3].leny = w_tree[p2].leny;
   }
   else {
      w_tree[p2].leny = (leny + 1) / 2;
      w_tree[p2+2].leny = w_tree[p2].leny - 1;
      if(stop1 == 0)
         w_tree[p2+3].leny = w_tree[p2+2].leny;
   }
   w_tree[p2+2].y = w_tree[p2].leny + y;
   if(stop1 == 0)
      w_tree[p2+3].y = w_tree[p2+2].y;
   w_tree[p2+1].leny = w_tree[p2].leny;
}


/*************************************************************/
/* Routine obtains the old format locations and sizes of the */
/* subbands 0-63.                                            */
/*************************************************************/
static void build_q_tree_wsq14(
   W_TREE w_tree[], /* wavelet tree structure */
   Q_TREE q_tree[])  /* quantization tree structure */
{

   q_tree16_wsq14(q_tree,3,w_tree[14].lenx,w_tree[14].leny,w_tree[14].x,w_tree[14].y);
   q_tree4_wsq14(q_tree,0,w_tree[19].lenx,w_tree[19].leny,w_tree[19].x,w_tree[19].y);
   q_tree16_wsq14(q_tree,19,w_tree[4].lenx,w_tree[4].leny,w_tree[4].x,w_tree[4].y);
   q_tree16_wsq14(q_tree,35,w_tree[5].lenx,w_tree[5].leny,w_tree[5].x,w_tree[5].y);
   q_tree4_wsq14(q_tree,52,w_tree[2].lenx,w_tree[2].leny,w_tree[2].x,w_tree[2].y);
   q_tree4_wsq14(q_tree,56,w_tree[3].lenx,w_tree[3].leny,w_tree[3].x,w_tree[3].y);
   q_tree4_wsq14(q_tree,60,w_tree[2].lenx,w_tree[3].leny,w_tree[2].x,w_tree[3].y);

   q_tree[51].x = w_tree[4].x;
   q_tree[51].y = w_tree[5].y;
   q_tree[51].lenx = w_tree[4].lenx;
   q_tree[51].leny = w_tree[5].leny;

   return;
}


/****************************************************************************/
/* Routine gives old format subband locations and sizes for lower frequency */
/* subbands in groups of 16 (i.e. 19-34 and 35-50).                         */
/****************************************************************************/
static void q_tree16_wsq14(
   Q_TREE q_tree[],  /* quantization tree structure */
   int start,           /* q_tree location of first subband
                           in the subband group being calculated */
   int lenx,            /* (temp) subband location and sizes */
   int leny,
   int x,
   int y)
{
   int tempx, temp2x;	/* temporary x values */
   int tempy, temp2y;	/* temporary y values */
   int evenx, eveny;	/* Check length of subband for even or odd */
   int p;		/* indicates subband information being stored */

   p = start;
   evenx = lenx % 2;
   eveny = leny % 2;

   if(evenx == 0) {
      tempx = lenx / 2;
      temp2x = tempx;
   }
   else {
      tempx = (lenx + 1) / 2;
      temp2x = tempx - 1;
   }
   
   if(eveny == 0) {
      tempy = leny / 2;
      temp2y = tempy;
   }
   else {
      tempy = (leny + 1) / 2;
      temp2y = tempy - 1;
   }

   evenx = tempx % 2;
   eveny = tempy % 2;

   q_tree[p].x = x;
   q_tree[p+2].x = x;
   q_tree[p].y = y;
   q_tree[p+1].y = y;
   if(evenx == 0) {
      q_tree[p].lenx = tempx / 2;
      q_tree[p+1].lenx = q_tree[p].lenx;
      q_tree[p+2].lenx = q_tree[p].lenx;
      q_tree[p+3].lenx = q_tree[p].lenx;
   }
   else {
      q_tree[p].lenx = (tempx + 1) / 2;
      q_tree[p+1].lenx = q_tree[p].lenx - 1;
      q_tree[p+2].lenx = q_tree[p].lenx;
      q_tree[p+3].lenx = q_tree[p+1].lenx;
   }
   q_tree[p+1].x = x + q_tree[p].lenx;
   q_tree[p+3].x = q_tree[p+1].x;
   if(eveny == 0) {
      q_tree[p].leny = tempy / 2;
      q_tree[p+1].leny = q_tree[p].leny;
      q_tree[p+2].leny = q_tree[p].leny;
      q_tree[p+3].leny = q_tree[p].leny;
   }
   else {
      q_tree[p].leny = (tempy + 1) / 2;
      q_tree[p+1].leny = q_tree[p].leny;
      q_tree[p+2].leny = q_tree[p].leny - 1;
      q_tree[p+3].leny = q_tree[p+2].leny;
   }
   q_tree[p+2].y = y + q_tree[p].leny;
   q_tree[p+3].y = q_tree[p+2].y;


   evenx = temp2x % 2;

   q_tree[p+4].x = x + tempx;
   q_tree[p+6].x = q_tree[p+4].x;
   q_tree[p+4].y = y;
   q_tree[p+5].y = y;
   q_tree[p+6].y = q_tree[p+2].y;
   q_tree[p+7].y = q_tree[p+2].y;
   if(evenx == 0) {
      q_tree[p+4].lenx = temp2x / 2;
      q_tree[p+5].lenx = q_tree[p+4].lenx;
      q_tree[p+6].lenx = q_tree[p+4].lenx;
      q_tree[p+7].lenx = q_tree[p+4].lenx;
   }
   else {
      q_tree[p+4].lenx = (temp2x + 1) / 2;
      q_tree[p+5].lenx = q_tree[p+4].lenx - 1;
      q_tree[p+6].lenx = q_tree[p+4].lenx;
      q_tree[p+7].lenx = q_tree[p+5].lenx;
   }
   q_tree[p+5].x = q_tree[p+4].x + q_tree[p+4].lenx;
   q_tree[p+7].x = q_tree[p+5].x;
   q_tree[p+4].leny = q_tree[p].leny;
   q_tree[p+5].leny = q_tree[p].leny;
   q_tree[p+6].leny = q_tree[p+2].leny;
   q_tree[p+7].leny = q_tree[p+2].leny;


   eveny = temp2y % 2;

   q_tree[p+8].x = x;
   q_tree[p+9].x = q_tree[p+1].x;
   q_tree[p+10].x = x;
   q_tree[p+11].x = q_tree[p+1].x;
   q_tree[p+8].y = y + tempy;
   q_tree[p+9].y = q_tree[p+8].y;
   q_tree[p+8].lenx = q_tree[p].lenx;
   q_tree[p+9].lenx = q_tree[p+1].lenx;
   q_tree[p+10].lenx = q_tree[p].lenx;
   q_tree[p+11].lenx = q_tree[p+1].lenx;
   if(eveny == 0) {
      q_tree[p+8].leny = temp2y / 2;
      q_tree[p+9].leny = q_tree[p+8].leny;
      q_tree[p+10].leny = q_tree[p+8].leny;
      q_tree[p+11].leny = q_tree[p+8].leny;
   }
   else {
      q_tree[p+8].leny = (temp2y + 1) / 2;
      q_tree[p+9].leny = q_tree[p+8].leny;
      q_tree[p+10].leny = q_tree[p+8].leny - 1;
      q_tree[p+11].leny = q_tree[p+10].leny;
   }
   q_tree[p+10].y = q_tree[p+8].y + q_tree[p+8].leny;
   q_tree[p+11].y = q_tree[p+10].y;


   q_tree[p+12].x = q_tree[p+4].x;
   q_tree[p+13].x = q_tree[p+5].x;
   q_tree[p+14].x = q_tree[p+4].x;
   q_tree[p+15].x = q_tree[p+5].x;
   q_tree[p+12].y = q_tree[p+8].y;
   q_tree[p+13].y = q_tree[p+8].y;
   q_tree[p+14].y = q_tree[p+10].y;
   q_tree[p+15].y = q_tree[p+10].y;
   q_tree[p+12].lenx = q_tree[p+4].lenx;
   q_tree[p+13].lenx = q_tree[p+5].lenx;
   q_tree[p+14].lenx = q_tree[p+4].lenx;
   q_tree[p+15].lenx = q_tree[p+5].lenx;
   q_tree[p+12].leny = q_tree[p+8].leny;
   q_tree[p+13].leny = q_tree[p+8].leny;
   q_tree[p+14].leny = q_tree[p+10].leny;
   q_tree[p+15].leny = q_tree[p+10].leny;
}


/*********************************************************************/
/* Routine gives old format subband locations and sizes for subbands */
/* in groups of 4 (i.e. 0-3 and 52-55).                              */
/*********************************************************************/
static void q_tree4_wsq14(
   Q_TREE q_tree[],  /* quantization tree structure */
   int start,		/* q_tree location of first subband
                           in the subband group being calculated */
   int lenx,            /* (temp) subband location and sizes */
   int leny,
   int x,
   int y)
{
   int evenx, eveny;	/* Check length of subband for even or odd */
   int p;		/* indicates subband information being stored */


   p = start;
   evenx = lenx % 2;
   eveny = leny % 2;


   q_tree[p].x = x;
   q_tree[p+2].x = x;
   q_tree[p].y = y;
   q_tree[p+1].y = y;
   if(evenx == 0) {
      q_tree[p].lenx = lenx / 2;
      q_tree[p+1].lenx = q_tree[p].lenx;
      q_tree[p+2].lenx = q_tree[p].lenx;
      q_tree[p+3].lenx = q_tree[p].lenx;
   }
   else {
      q_tree[p].lenx = (lenx + 1) / 2;
      q_tree[p+1].lenx = q_tree[p].lenx - 1;
      q_tree[p+2].lenx = q_tree[p].lenx;
      q_tree[p+3].lenx = q_tree[p+1].lenx;
   }
   q_tree[p+1].x = x + q_tree[p].lenx;
   q_tree[p+3].x = q_tree[p+1].x;
   if(eveny == 0) {
      q_tree[p].leny = leny / 2;
      q_tree[p+1].leny = q_tree[p].leny;
      q_tree[p+2].leny = q_tree[p].leny;
      q_tree[p+3].leny = q_tree[p].leny;
   }
   else {
      q_tree[p].leny = (leny + 1) / 2;
      q_tree[p+1].leny = q_tree[p].leny;
      q_tree[p+2].leny = q_tree[p].leny - 1;
      q_tree[p+3].leny = q_tree[p+2].leny;
   }
   q_tree[p+2].y = y + q_tree[p].leny;
   q_tree[p+3].y = q_tree[p+2].y;
}
