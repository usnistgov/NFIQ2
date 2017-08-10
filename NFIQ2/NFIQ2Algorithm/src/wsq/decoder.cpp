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

      FILE:    DECODER.C
      AUTHORS: Craig Watson
               Michael Garris
      DATE:    12/02/1999
      UPDATED: 02/24/2005 by MDG

      Contains routines responsible for decoding a WSQ compressed
      datastream.

      ROUTINES:
#cat: wsq_decode_mem - Decodes a datastream of WSQ compressed bytes
#cat:                  from a memory buffer, returning a lossy
#cat:                  reconstructed pixmap.
#cat: wsq_decode_file - Decodes a datastream of WSQ compressed bytes
#cat:                  from an open file, returning a lossy
#cat:                  reconstructed pixmap.
#cat: huffman_decode_data_mem - Decodes a block of huffman encoded
#cat:                  data from a memory buffer.
#cat: huffman_decode_data_file - Decodes a block of huffman encoded
#cat:                  data from an open file.
#cat: decode_data_mem - Decodes huffman encoded data from a memory buffer.
#cat:
#cat: decode_data_file - Decodes huffman encoded data from an open file.
#cat:
#cat: nextbits_wsq - Gets next sequence of bits for data decoding from
#cat:                    an open file.
#cat: getc_nextbits_wsq - Gets next sequence of bits for data decoding
#cat:                    from a memory buffer.

***********************************************************************/

#include <stdio.h>
#include <wsq.h>
#include <dataio.h>

/************************************************************************/
/*              This is an implementation based on the Crinimal         */
/*              Justice Information Services (CJIS) document            */
/*              "WSQ Gray-scale Fingerprint Compression                 */
/*              Specification", Dec. 1997.                              */
/***************************************************************************/
/* WSQ Decoder routine.  Takes an WSQ compressed memory buffer and decodes */
/* it, returning the reconstructed pixmap.                                 */
/***************************************************************************/
int wsq_decode_mem(unsigned char **odata, int *ow, int *oh, int *od, int *oppi,
                   int *lossyflag, unsigned char *idata, const int ilen)
{
   int ret, i;
   unsigned short marker;         /* WSQ marker */
   int num_pix;                   /* image size and counter */
   int width, height, ppi;        /* image parameters */
   unsigned char *cdata;          /* image pointer */
   float *fdata;                  /* image pointers */
   short *qdata;                  /* image pointers */
   unsigned char *cbufptr;        /* points to current byte in buffer */
   unsigned char *ebufptr;        /* points to end of buffer */

   /* Added by MDG on 02-24-05 */
   init_wsq_decoder_resources();

   /* Set memory buffer pointers. */
   cbufptr = idata;
   ebufptr = idata + ilen;

   /* Init DHT Tables to 0. */
   for(i = 0; i < MAX_DHT_TABLES; i++)
      (dht_table + i)->tabdef = 0;

   /* Read the SOI marker. */
   if((ret = getc_marker_wsq(&marker, SOI_WSQ, &cbufptr, ebufptr))){
      free_wsq_decoder_resources();
      return(ret);
   }

   /* Read in supporting tables up to the SOF marker. */
   if((ret = getc_marker_wsq(&marker, TBLS_N_SOF, &cbufptr, ebufptr))){
      free_wsq_decoder_resources();
      return(ret);
   }
   while(marker != SOF_WSQ) {
      if((ret = getc_table_wsq(marker, &dtt_table, &dqt_table, dht_table,
                          &cbufptr, ebufptr))){
         free_wsq_decoder_resources();
         return(ret);
      }
      if((ret = getc_marker_wsq(&marker, TBLS_N_SOF, &cbufptr, ebufptr))){
         free_wsq_decoder_resources();
         return(ret);
      }
   }

   /* Read in the Frame Header. */
   if((ret = getc_frame_header_wsq(&frm_header_wsq, &cbufptr, ebufptr))){
      free_wsq_decoder_resources();
      return(ret);
   }
   width = frm_header_wsq.width;
   height = frm_header_wsq.height;
   num_pix = width * height;

   if((ret = getc_ppi_wsq(&ppi, idata, ilen))){
      free_wsq_decoder_resources();
      return(ret);
   }

   if(debug > 0)
      fprintf(stderr, "SOI, tables, and frame header read\n\n");

   /* Build WSQ decomposition trees. */
   build_wsq_trees(w_tree, W_TREELEN, q_tree, Q_TREELEN, width, height);

   if(debug > 0)
      fprintf(stderr, "Tables for wavelet decomposition finished\n\n");

   /* Allocate working memory. */
   qdata = (short *) malloc(num_pix * sizeof(short));
   if(qdata == (short *)NULL) {
      fprintf(stderr,"ERROR: wsq_decode_mem : malloc : qdata1\n");
      free_wsq_decoder_resources();
      return(-20);
   }
   /* Decode the Huffman encoded data blocks. */
   if((ret = huffman_decode_data_mem(qdata, &dtt_table, &dqt_table, dht_table,
                                    &cbufptr, ebufptr))){
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
      fprintf(stderr,"ERROR: wsq_decode_mem : malloc : cdata\n");
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
   *ow = width;
   *oh = height;
   *od = 8;
   *oppi = ppi;
   *lossyflag = 1;

   /* Return normally. */
   return(0);
}

/**************************************************************************/
/* WSQ File Decoder routine.  Takes an open WSQ compressed file and reads */
/* in the WSQ encoded data, returning a decoded reconstructed pixmap.     */
/**************************************************************************/
int wsq_decode_file(unsigned char **odata, int *ow, int *oh, int *od, int *oppi,
                    int *lossyflag, FILE *infp)
{
   int ret;
   unsigned short marker;         /* WSQ marker */
   int num_pix;                   /* image size and counter */
   int width, height, ppi;        /* image parameters */
   unsigned char *cdata;          /* image pointer */
   float *fdata;                  /* image pointers */
   short *qdata;                  /* image pointers */

   /* Added by MDG on 02-24-05 */
   init_wsq_decoder_resources();

   /* Read the SOI marker. */
   if((ret = read_marker_wsq(&marker, SOI_WSQ, infp))){
      free_wsq_decoder_resources();
      return(ret);
   }

   /* Read in supporting tables up to the SOF marker. */
   if((ret = read_marker_wsq(&marker, TBLS_N_SOF, infp))){
      free_wsq_decoder_resources();
      return(ret);
   }
   while(marker != SOF_WSQ) {
      if((ret = read_table_wsq(marker, &dtt_table, &dqt_table, dht_table, infp))){
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

   if((ret = read_ppi_wsq(&ppi, infp))){
      free_wsq_decoder_resources();
      return(ret);
   }

   if(debug > 0)
      fprintf(stderr, "SOI, tables, and frame header read\n\n");

   /* Build WSQ decomposition trees. */
   build_wsq_trees(w_tree, W_TREELEN, q_tree, Q_TREELEN, width, height);

   if(debug > 0)
      fprintf(stderr, "Tables for wavelet decomposition finished\n\n");

   /* Allocate working memory. */
   qdata = (short *) malloc(num_pix * sizeof(short));
   if(qdata == (short *)NULL) {
      free_wsq_decoder_resources();
      fprintf(stderr,"ERROR: wsq_decode_file : malloc : qdata1\n");
      return(-20);
   }

   /* Decode the Huffman encoded data blocks. */
   if((ret = huffman_decode_data_file(qdata, &dtt_table, &dqt_table, dht_table,
                                     infp))){
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
      fprintf(stderr,"ERROR: wsq_decode_file : malloc : cdata\n");
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
   *ow = width;
   *oh = height;
   *od = 8;
   *oppi = ppi;
   *lossyflag = 1;

   /* Return normally. */
   return(0);
}

/***************************************************************************/
/* Routine to decode an entire "block" of encoded data from memory buffer. */
/***************************************************************************/
int huffman_decode_data_mem(
   short *ip,               /* image pointer */
   DTT_TABLE *dtt_table,    /*transform table pointer */
   DQT_TABLE *dqt_table,    /* quantization table */
   DHT_TABLE *dht_table,    /* huffman table */
   unsigned char **cbufptr, /* points to current byte in input buffer */
   unsigned char *ebufptr)  /* points to end of input buffer */
{
   int ret;
   int blk = 0;           /* block number */
   unsigned short marker; /* WSQ markers */
   int bit_count;         /* bit count for getc_nextbits_wsq routine */
   int n;                 /* zero run count */
   int nodeptr;           /* pointers for decoding */
   int last_size;         /* last huffvalue */
   unsigned char hufftable_id;    /* huffman table number */
   HUFFCODE *hufftable;   /* huffman code structure */
   int maxcode[MAX_HUFFBITS+1]; /* used in decoding data */
   int mincode[MAX_HUFFBITS+1]; /* used in decoding data */
   int valptr[MAX_HUFFBITS+1];  /* used in decoding data */
   unsigned short tbits;
   int ipc, ipc_mx, ipc_q;   /* image byte count adjustment parameters */


   if((ret = getc_marker_wsq(&marker, TBLS_N_SOB, cbufptr, ebufptr)))
      return(ret);

   bit_count = 0;
   ipc = 0;
   ipc_q = 0;
   ipc_mx = frm_header_wsq.width * frm_header_wsq.height;

   while(marker != EOI_WSQ) {

      if(marker != 0) {
         blk++;
         while(marker != SOB_WSQ) {
            if((ret = getc_table_wsq(marker, dtt_table, dqt_table,
                                dht_table, cbufptr, ebufptr)))
               return(ret);
            if((ret = getc_marker_wsq(&marker, TBLS_N_SOB, cbufptr, ebufptr)))
               return(ret);
         }
         if(dqt_table->dqt_def && !ipc_q) {
            for(n = 0; n < 64; n++)
               if(dqt_table->q_bin[n] == 0.0)
                  ipc_mx -= q_tree[n].lenx*q_tree[n].leny;

            ipc_q = 1;
         }
         if((ret = getc_block_header(&hufftable_id, cbufptr, ebufptr)))
            return(ret);

         if((dht_table+hufftable_id)->tabdef != 1) {
            fprintf(stderr, "ERROR : huffman_decode_data_mem : ");
            fprintf(stderr, "huffman table {%d} undefined.\n", hufftable_id);
            return(-51);
         }

         /* the next two routines reconstruct the huffman tables */
         if((ret = build_huffsizes(&hufftable, &last_size,
                                  (dht_table+hufftable_id)->huffbits,
                                  MAX_HUFFCOUNTS_WSQ)))
            return(ret);

         build_huffcodes(hufftable);
         if((ret = check_huffcodes_wsq(hufftable, last_size)))
            fprintf(stderr, "         hufftable_id = %d\n", hufftable_id);

         /* this routine builds a set of three tables used in decoding */
         /* the compressed data*/
         gen_decode_table(hufftable, maxcode, mincode, valptr,
                          (dht_table+hufftable_id)->huffbits);
         free(hufftable);
         bit_count = 0;
         marker = 0;
      }

      /* get next huffman category code from compressed input data stream */
      if((ret = decode_data_mem(&nodeptr, mincode, maxcode, valptr,
                            (dht_table+hufftable_id)->huffvalues,
                            cbufptr, ebufptr, &bit_count, &marker)))
         return(ret);

      if(nodeptr == -1) {
         while(marker == COM_WSQ && blk == 3) {
            if((ret = getc_table_wsq(marker, dtt_table, dqt_table,
                                dht_table, cbufptr, ebufptr)))
               return(ret);
            if((ret = getc_marker_wsq(&marker, ANY_WSQ, cbufptr, ebufptr)))
               return(ret);
         }
         continue;
      }

      if(ipc > ipc_mx) {
         fprintf(stderr, "ERROR : huffman_decode_data_mem [1]: ");
         fprintf(stderr, "Decoded data extends past image buffer. ");
         fprintf(stderr, "Encoded data appears corrupt or non-standard.\n");
         fflush(stderr);
         return(-51);
      }

      if(nodeptr > 0 && nodeptr <= 100) {
         ipc += nodeptr;
         if(ipc > ipc_mx) {
            fprintf(stderr, "ERROR : huffman_decode_data_mem [2]: ");
            fprintf(stderr, "Decoded data extends past image buffer. ");
            fprintf(stderr, "Encoded data appears corrupt or non-standard.\n");
            fflush(stderr);
            return(-51);
         }
         for(n = 0; n < nodeptr; n++)
            *ip++ = 0; /* z run */
      }
      else if(nodeptr > 106 && nodeptr < 0xff) {
         *ip++ = nodeptr - 180;
         ipc++;
      }
      else if(nodeptr == 101){
         if((ret = getc_nextbits_wsq(&tbits, &marker, cbufptr, ebufptr,
                                &bit_count, 8)))
            return(ret);
         *ip++ = tbits;
         ipc++;
      }
      else if(nodeptr == 102){
         if((ret = getc_nextbits_wsq(&tbits, &marker, cbufptr, ebufptr,
                                &bit_count, 8)))
            return(ret);
         *ip++ = -tbits;
         ipc++;
      }
      else if(nodeptr == 103){
         if((ret = getc_nextbits_wsq(&tbits, &marker, cbufptr, ebufptr,
                                &bit_count, 16)))
            return(ret);
         *ip++ = tbits;
         ipc++;
      }
      else if(nodeptr == 104){
         if((ret = getc_nextbits_wsq(&tbits, &marker, cbufptr, ebufptr,
                                &bit_count, 16)))
            return(ret);
         *ip++ = -tbits;
         ipc++;
      }
      else if(nodeptr == 105) {
         if((ret = getc_nextbits_wsq(&tbits, &marker, cbufptr, ebufptr,
                                &bit_count, 8)))
            return(ret);
         ipc += tbits;
         if(ipc > ipc_mx) {
            fprintf(stderr, "ERROR : huffman_decode_data_mem [3]: ");
            fprintf(stderr, "Decoded data extends past image buffer. ");
            fprintf(stderr, "Encoded data appears corrupt or non-standard.\n");
            fflush(stderr);
            return(-51);
         }
         n = tbits;
         while(n--)
            *ip++ = 0;
      }
      else if(nodeptr == 106) {
         if((ret = getc_nextbits_wsq(&tbits, &marker, cbufptr, ebufptr,
                                &bit_count, 16)))
            return(ret);
         ipc += tbits;
         if(ipc > ipc_mx) {
            fprintf(stderr, "ERROR : huffman_decode_data_mem [4]: ");
            fprintf(stderr, "Decoded data extends past image buffer. ");
            fprintf(stderr, "Encoded data appears corrupt or non-standard.\n");
            fflush(stderr);
            return(-51);
         }
         n = tbits;
         while(n--)
            *ip++ = 0;
      }
      else {
         fprintf(stderr, 
                "ERROR: huffman_decode_data_mem : Invalid code %d (%x).\n",
                nodeptr, nodeptr);
         return(-52);
      }
   }

   return(0);
}

/********************************************************************/
/* Routine to decode an entire "block" of encoded data from a file. */
/********************************************************************/
int huffman_decode_data_file(
   short *ip,             /* image pointer */
   DTT_TABLE *dtt_table,  /*transform table pointer */
   DQT_TABLE *dqt_table,  /* quantization table */
   DHT_TABLE *dht_table,  /* huffman table */
   FILE *infp)            /* input file */
{
   int ret;
   int blk = 0;           /* block number */
   unsigned short marker; /* WSQ markers */
   int bit_count;         /* bit count for nextbits_wsq routine */
   int n;                 /* zero run count */
   int nodeptr;           /* pointers for decoding */
   int last_size;         /* last huffvalue */
   unsigned char hufftable_id;    /* huffman table number */
   HUFFCODE *hufftable;   /* huffman code structure */
   int maxcode[MAX_HUFFBITS+1]; /* used in decoding data */
   int mincode[MAX_HUFFBITS+1]; /* used in decoding data */
   int valptr[MAX_HUFFBITS+1];  /* used in decoding data */
   unsigned short tbits;


   if((ret = read_marker_wsq(&marker, TBLS_N_SOB, infp)))
      return(ret);

   bit_count = 0;

   while(marker != EOI_WSQ) {

      if(marker != 0) {
         blk++;
         while(marker != SOB_WSQ) {
            if((ret = read_table_wsq(marker, dtt_table, dqt_table,
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
         if((ret = check_huffcodes_wsq(hufftable, last_size)))
            fprintf(stderr, "         hufftable_id = %d\n", hufftable_id);

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

      if(nodeptr == -1) {
         while(marker == COM_WSQ && blk == 3) {
            if((ret = read_table_wsq(marker, dtt_table, dqt_table,
                                dht_table, infp)))
               return(ret);
            if((ret = read_marker_wsq(&marker, ANY_WSQ, infp)))
               return(ret);
         }
         continue;
      }

      if(nodeptr > 0 && nodeptr <= 100)
         for(n = 0; n < nodeptr; n++) {
            *ip++ = 0; /* z run */
         }
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
      else if(nodeptr < 0xff)
         *ip++ = nodeptr - 180;
      else {
         fprintf(stderr, 
                "ERROR: huffman_decode_data_file : Invalid code %d (%x).\n",
                nodeptr, nodeptr);
         return(-54);
      }
   }

   return(0);
}

/**********************************************************/
/* Routine to decode the encoded data from memory buffer. */
/**********************************************************/
int decode_data_mem(
   int *onodeptr,       /* returned huffman code category        */
   int *mincode,        /* points to minimum code value for      */
                        /*    a given code length                */
   int *maxcode,        /* points to maximum code value for      */
                        /*    a given code length                */
   int *valptr,         /* points to first code in the huffman   */
                        /*    code table for a given code length */
   unsigned char *huffvalues,   /* defines order of huffman code          */
                                /*    lengths in relation to code sizes   */
   unsigned char **cbufptr,     /* points to current byte in input buffer */
   unsigned char *ebufptr,      /* points to end of input buffer          */
   int *bit_count,      /* marks the bit to receive from the input byte */
   unsigned short *marker)
{
   int ret;
   int inx, inx2;       /*increment variables*/
   unsigned short code, tbits;  /* becomes a huffman code word
                                   (one bit at a time)*/

   if((ret = getc_nextbits_wsq(&code, marker, cbufptr, ebufptr, bit_count, 1)))
      return(ret);

   if(*marker != 0){
      *onodeptr = -1;
      return(0);
   }

   for(inx = 1; (int)code > maxcode[inx]; inx++) {
      if((ret = getc_nextbits_wsq(&tbits, marker, cbufptr, ebufptr, bit_count, 1)))
         return(ret);

      code = (code << 1) + tbits;
      if(*marker != 0){
         *onodeptr = -1;
         return(0);
      }
   }
   inx2 = valptr[inx];
   inx2 = inx2 + code - mincode[inx];

   *onodeptr = huffvalues[inx2];
   return(0);
}

/*************************************************/
/* Routine to decode the encoded data from file. */
/*************************************************/
int decode_data_file(
   int *onodeptr,       /* returned huffman code category        */
   int *mincode,        /* points to minimum code value for      */
                        /*    a given code length                */
   int *maxcode,        /* points to maximum code value for      */
                        /*    a given code length                */
   int *valptr,         /* points to first code in the huffman   */
                        /*    code table for a given code length */
   unsigned char *huffvalues,   /* defines order of huffman code         */
                        /*    lengths in relation to code sizes  */
   FILE *infp,          /* compressed input data file            */
   int *bit_count,      /* marks the bit to receive from the input byte */
   unsigned short *marker)
{
   int ret;
   int inx, inx2;               /*increment variables*/
   unsigned short code, tbits;  /*becomes a huffman code word
                                  (one bit at a time)*/

   if((ret = nextbits_wsq(&code, marker, infp, bit_count, 1)))
      return(ret);

   if(*marker != 0){
      *onodeptr = -1;
      return(0);
   }

   for(inx = 1; (int)code > maxcode[inx]; inx++) {
      if((ret = nextbits_wsq(&tbits, marker, infp, bit_count, 1)))
         return(ret);

      code = (code << 1) + tbits;
      if(*marker != 0){
         *onodeptr = -1;
         return(0);
      }
   }
   inx2 = valptr[inx];
   inx2 = inx2 + code - mincode[inx];

   *onodeptr = huffvalues[inx2];
   return(0);
}

/*********************************************/
/* Routine to get nextbit(s) of data stream. */
/*********************************************/
int nextbits_wsq(
   unsigned short *obits,  /* returned bits */
   unsigned short *marker, /* returned marker */
   FILE *file,          /* compressed input data file */
   int *bit_count,      /* marks the bit to receive from the input byte */
   const int bits_req)  /* number of bits requested */
{
   int ret;
   static unsigned char code;   /*next byte of data*/
   static unsigned char code2;  /*stuffed byte of data*/
   unsigned short bits, tbits;  /*bits of current data byte requested*/
   int bits_needed;     /*additional bits required to finish request*/

                              /*used to "mask out" n number of
                                bits from data stream*/
   static unsigned char bit_mask[9] = {0x00,0x01,0x03,0x07,0x0f,
                                       0x1f,0x3f,0x7f,0xff};
   if(*bit_count == 0) {
      code = (unsigned char)getc(file);
      *bit_count = 8;
      if(code == 0xFF) {
         code2 = (unsigned char)getc(file);
         if(code2 != 0x00 && bits_req == 1) {
            *marker = (code << 8) | code2;
            *obits = 1;
            return(0);
         }
         if(code2 != 0x00) {
            fprintf(stderr, "ERROR: nextbits_wsq : No stuffed zeros\n");
            return(-38);
         }
      }
   }
   if(bits_req <= *bit_count) {
      bits = (code >>(*bit_count - bits_req)) & (bit_mask[bits_req]);
      *bit_count -= bits_req;
      code &= bit_mask[*bit_count];
   }
   else {
      bits_needed = bits_req - *bit_count;
      bits = code << bits_needed;
      *bit_count = 0;
      if((ret = nextbits_wsq(&tbits, (unsigned short *)NULL, file,
                          bit_count, bits_needed)))
         return(ret);
      bits |= tbits;
   }

   *obits = bits;
   return(0);
}

/****************************************************************/
/* Routine to get nextbit(s) of data stream from memory buffer. */
/****************************************************************/
int getc_nextbits_wsq(
   unsigned short *obits,       /* returned bits */
   unsigned short *marker,      /* returned marker */
   unsigned char **cbufptr,     /* points to current byte in input buffer */
   unsigned char *ebufptr,      /* points to end of input buffer */
   int *bit_count,      /* marks the bit to receive from the input byte */
   const int bits_req)  /* number of bits requested */
{
   int ret;
   static unsigned char code;   /*next byte of data*/
   static unsigned char code2;  /*stuffed byte of data*/
   unsigned short bits, tbits;  /*bits of current data byte requested*/
   int bits_needed;     /*additional bits required to finish request*/

                              /*used to "mask out" n number of
                                bits from data stream*/
   static unsigned char bit_mask[9] = {0x00,0x01,0x03,0x07,0x0f,
                                       0x1f,0x3f,0x7f,0xff};
   if(*bit_count == 0) {
      if((ret = getc_byte(&code, cbufptr, ebufptr))){
         return(ret);
      }
      *bit_count = 8;
      if(code == 0xFF) {
         if((ret = getc_byte(&code2, cbufptr, ebufptr))){
            return(ret);
         }
         if(code2 != 0x00 && bits_req == 1) {
            *marker = (code << 8) | code2;
            *obits = 1;
            return(0);
         }
         if(code2 != 0x00) {
            fprintf(stderr, "ERROR: getc_nextbits_wsq : No stuffed zeros\n");
            return(-41);
         }
      }
   }
   if(bits_req <= *bit_count) {
      bits = (code >>(*bit_count - bits_req)) & (bit_mask[bits_req]);
      *bit_count -= bits_req;
      code &= bit_mask[*bit_count];
   }
   else {
      bits_needed = bits_req - *bit_count;
      bits = code << bits_needed;
      *bit_count = 0;
      if((ret = getc_nextbits_wsq(&tbits, (unsigned short *)NULL, cbufptr,
                             ebufptr, bit_count, bits_needed)))
         return(ret);
      bits |= tbits;
   }

   *obits = bits;
   return(0);
}
