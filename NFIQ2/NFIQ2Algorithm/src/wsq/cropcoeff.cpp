/************************************************************************
                               NOTICE
 
This MITRE-modified NIST code was produced for the U. S. Government
under Contract No. W15P7T-07-C-F700. Pursuant to Title 17 Section 105 
of the United States Code, this software is not subject to copyright 
protection and is in the public domain. NIST and MITRE assume no 
responsibility whatsoever for use by other parties of its source code 
or open source server, and makes no guarantees, expressed or implied,
about its quality, reliability, or any other characteristic.

This software has been determined to be outside the scope of the EAR
(see Part 734.3 of the EAR for exact details) as it has been created solely
by employees of the U.S. Government; it is freely distributed with no
licensing requirements; and it is considered public domain.  Therefore,
it is permissible to distribute this software as a free download from the
internet.

The algorithm and its benefits are briefly described in the
MITRE Technical Report "Fingerprint Recompression after Segmentation"
(MTR080005), available at 
http://www.mitre.org/work/tech_papers/tech_papers_08/08_0060/index.html
 
************************************************************************/

/***********************************************************************
      LIBRARY: WSQ - Grayscale Image Compression

      FILE:    CROPCOEFF.C
      AUTHORS: Margaret Lepley (MITRE)
      Heavy use of previous WSQ code from:
               Craig Watson
               Michael Garris
      DATE:    1/16/08
      UPDATE:  2/7/08 Error handling for invalid boxes added.

      Contains routines responsible for cropping a WSQ compressed
      datastream to a smaller WSQ codestream via CropCoeff method.

      ROUTINES:
#cat: quant_block_size2 - Near duplicate of function already in NBIS 
#cat:                library, but passing a precomputed DQT_TABLE 
#cat:                pointer rather than QUANT_VALS.
#cat:                I.e. no rate control is performed in this version.
#cat: wsq_crop_qdata - Crop quantized coeff structures.
#cat: wsq_cropcoeff_mem - Crops input buffer of WSQ compressed bytes
#cat:                into output WSQ buffer, by eliminating unneeded 
#cat:                wavelet coefficients. First call (NULL output,
#cat:                and NULL qdata) will decode original data. 
#cat:                Subsequent calls reuse data, and do not repeat 
#cat:                the decode.
#cat: wsq_huffcode_mem - WSQ Huffman codes quantized coefficient array,
#cat:                returning a changed memory buffer. Can be called
#cat:                repeatedly with the same codestream header buffer.  
#cat:                and won't reread the data.
#cat: wsq_dehuff_mem - Decodes WSQ to a quantized coefficient array,
#cat:                and stops. Internal decode info (dtt_table,dqt_table)
#cat:                is retained. Call once, followed by multiple calls
#cat:                to wsq_crop_qdata and wsq_huffcode_mem. 
#cat: read_wsq_frame_header - Parses WSQ memory until the frame header is
#cat:                found. The image dimensions and scale and shift
#cat:                fields are read and returned.


***********************************************************************/

#include <stdio.h>
#include <string.h>

#include <wsq.h>
#include <dataio.h>

Q_TREE q_tree2[Q_TREELEN];
Q_TREE q_tree3[Q_TREELEN];

/************************************************************************/
/* Compute quantized WSQ subband block sizes, using DQT_TABLE input     */
/* Near duplicate of quant_block_sizes (util.c), but passing a          */
/* precomputed DQT_TABLE pointer rather than QUANT_VALS.                */
/* I.e. no rate control is performed in this version.                   */
/************************************************************************/
void quant_block_sizes2(int *oqsize1, int *oqsize2, int *oqsize3,
   const DQT_TABLE *dqt_table, /* quantization table structure   */
                 W_TREE *w_tree, const int w_treelen,
                 Q_TREE *q_tree, const int q_treelen)
{
   int qsize1, qsize2, qsize3;
   int node;

   /* Compute temporary sizes of 3 WSQ subband blocks. */
   qsize1 = w_tree[14].lenx * w_tree[14].leny;
   qsize2 = (w_tree[5].leny * w_tree[1].lenx) +
            (w_tree[4].lenx * w_tree[4].leny);
   qsize3 = (w_tree[2].lenx * w_tree[2].leny) +
            (w_tree[3].lenx * w_tree[3].leny);

   /* Adjust size of quantized WSQ subband blocks. */
   for (node = 0; node < STRT_SUBBAND_2; node++)
      if(dqt_table->q_bin[node] == 0.0)
         qsize1 -= (q_tree[node].lenx * q_tree[node].leny);

   for (node = STRT_SUBBAND_2; node < STRT_SUBBAND_3; node++)
      if(dqt_table->q_bin[node] == 0.0)
          qsize2 -= (q_tree[node].lenx * q_tree[node].leny);

   for (node = STRT_SUBBAND_3; node < STRT_SUBBAND_DEL; node++)
      if(dqt_table->q_bin[node] == 0.0)
         qsize3 -= (q_tree[node].lenx * q_tree[node].leny);

   *oqsize1 = qsize1;
   *oqsize2 = qsize2;
   *oqsize3 = qsize3;
}

/*****************************************************************/
/* Routine to crop quantized coefficients.                       */
/* This figures out what coefficients to keep and copies them    */
/* into pre-allocated memory (scp).                              */
/* scp must point to at least width*height*sizeof(short) memory  */
/* WARNING: once this is called, w_tree is no longer available   */
/* for full image decodes. Instead it has cropped image dims     */
/* Version 2.0 -- jumps to required areas and uses memcpy,instead*/
/* of scanning the entire array and copying one element at a time*/
/*****************************************************************/
int wsq_crop_qdata(
   const DQT_TABLE *dqt_table, /* quantization table structure   */
   Q_TREE q_tree[], 
   Q_TREE q_tree2[],
   Q_TREE q_tree3[],
   short *sip,           /* Original quantized data pointer      */
   int ulx,              /* UL corner col */
   int uly,              /* UL corner row */
   int width,            /* Crop region width */
   int height,           /* Crop region height */
   short *scp)           /* Cropped quantized data pointer       */
{
   int row, col;  /* cover counter and row/column counters */
   short *cptr;   /* image pointers */
   short *sptr;
   short *bptr;
   int cnt;       /* subband counter */
   int numbytes;  /* memcpy amount */
   
   if (ulx%32 || uly%32) 
     fprintf(stderr, "SERIOUS WARNING : wsq_crop_qdata will produce awful results. \n\tUL (%d,%d) is not a multiple of 32\n", ulx,uly);

   /* Figure out which subband coefficients to keep.     */
   /* q_tree3 dims are the UL corner of each new subband */
   /* q_tree2 dims are the dims of the new subbands      */
   /* Build with the cropped width/height MUST be the last tree build prior
      to using w_tree and q_tree2 for encoding the cropped data. Once this
      occurs, w_tree can no longer be used to access the original uncropped
      data.  Note that q_tree is not touched, so it can still be used to 
      access the uncropped coefficient data. 
   */
   build_wsq_trees(w_tree, W_TREELEN, q_tree3, Q_TREELEN, ulx, uly);
   build_wsq_trees(w_tree, W_TREELEN, q_tree2, Q_TREELEN, width, height);

   if(dqt_table->dqt_def != 1) {
      fprintf(stderr,
      "ERROR: unquantize : quantization table parameters not defined!\n");
      return(-92);
   }

   bptr = sip;
   cptr = scp;
   for(cnt = 0; cnt < NUM_SUBBANDS; cnt++) {
      if(dqt_table->q_bin[cnt] == 0.0)
         continue;
      /* Length of each new subband row in bytes */
      numbytes = q_tree2[cnt].lenx*sizeof(short);
      /* Start subband offset to first item to be copied */
      sptr = bptr + q_tree3[cnt].leny*q_tree[cnt].lenx + q_tree3[cnt].lenx;
      /* Copy from each row as one chunk */
      for(row = 0; row < q_tree2[cnt].leny; row++){
	  memcpy(cptr,sptr,numbytes);
	  cptr += q_tree2[cnt].lenx;
	  sptr += q_tree[cnt].lenx;
      }
      /* Move subband pointer to next subband */
      bptr += q_tree[cnt].lenx * q_tree[cnt].leny;
   }
   return(0);
}
 
/*************************************************************
This function can be called many times on the same codestream, 
and only dehuffs the original data once.  On the first call make 
sure that *odata and *pqdata are NULL, and it will read the data
and create the structures. Subsequent calls where these pointers
aren't NULL will use the extant pqdata and idata to fill odata.
If the corner requests are not a locally valid box (positive rectangle
which intersects actual image area), then *ow / *oh will be returned 
as -1 and odata won't contain wsq compressed data.
**************************************************************/
int wsq_cropcoeff_mem(
   unsigned char **odata, /* Cropped WSQ mem */
   int *olen,             /* Cropped WSQ coded length */
   int *ow, int *oh,      /* Actual crop width/height */
   int ulx, int uly,      /* UL corner request */
   int lrx, int lry,      /* LR corner request */
                          /* Note: LR corner pixel not included in crop */
   int *iw, int *ih,      /* Input WSQ dimensions */
   unsigned char *idata,  /* Input WSQ data */
   const int ilen,        /* Input WSQ length */
   short **pqdata,        /* Pointer to input image qdata array */
   int *hgt_pos,          /* Position of Frame Header Height in idata */
   int *huff_pos          /* Position where Huff data begins in idata */
)
{
   int ret;
   int width, height;             /* image parameters */
   unsigned char *wsq_data;      /* compressed data buffer      */
   short *qdata;                  /* image pointers */
   short *qdata2;                  /* image pointers */
   int first;
   double scale, shift;

   qdata = *pqdata;
   wsq_data = *odata;

   if (qdata != NULL && wsq_data != NULL) first = 0;
   else first = 1;

   if (first) {
     if ((ret = wsq_dehuff_mem(&qdata, &width, &height, &scale, &shift, hgt_pos, 
		     huff_pos, idata, ilen))) {
       return(ret);
     }

     free_wsq_decoder_resources();
     *pqdata = qdata;
     *iw = width;
     *ih = height;

     /* Allocate a WSQ-encoded output buffer.  Allocate this buffer */
     /* to be the size of the original codestream. If the encoded data */
     /* exceeds this buffer size, then throw an error because we do */
     /* not want our compressed data to be larger than the original */
     /* image data.                                                 */
     wsq_data = (unsigned char *)malloc(ilen);
     if(wsq_data == (unsigned char *)NULL){
       fprintf(stderr, "ERROR : wsq_cropcoeff_1 : malloc : wsq_data\n");
       return(-12);
     }
   }
   else {
     width = *iw;
     height = *ih;
   }

   /* Check that box corners define a valid box */
   if (ulx >= lrx || uly >=lry) {
     fprintf(stderr, "WARNING : wsq_cropcoeff_mem : invalid box UL(%d,%d), LR(%d,%d)\n", 
	     ulx, uly, lrx, lry);
     *ow = -1; 
     *oh = -1;
     return(0);
   }
   /* Check that box intersects image area */
   if (ulx >= width || uly >= height || lrx < 1 || lry < 1) {
     fprintf(stderr, "WARNING : wsq_cropcoeff_mem : box outside image\n");
     fprintf(stderr, "        UL(%d,%d), LR(%d,%d)  Image width %d height %d\n", 
	     ulx, uly, lrx, lry, width, height);
     *ow = -1; 
     *oh = -1;
     return(0);
   }
   /* The above are not fatal errors to further processing on the image,
      but do mean no image is generated for this box, so processing might as 
      well stop immediately. */

   /* Make sure UL corner is on the image and a multiple of 32  */
   if (ulx < 0) ulx = 0;
   if (uly < 0) uly = 0;
   ulx /= 32; ulx *= 32;
   uly /= 32; uly *= 32; 
   /* Make sure LR corner is on the image */
   if (lrx > width) lrx = width;
   if (lry > height) lry = height;

   *oh = lry - uly;
   *ow = lrx - ulx;

   /* Allocate working memory. */
   qdata2 = (short *) malloc((*ow)*(*oh) * sizeof(short));
   if(qdata2 == (short *)NULL) {
      fprintf(stderr,"ERROR: wsq_cropcoeff_mem : malloc : qdata2\n");
      return(-20);
   }

   /* Crop the wavelet coefficients back */
   if((ret = wsq_crop_qdata(&dqt_table, q_tree, q_tree2, q_tree3, qdata, ulx, uly, *ow, *oh, qdata2))) {
       free(qdata2);
       return(ret);
   }

   if(debug > 0)
     fprintf(stderr, "Cropped coefficients: UL (%d,%d)  %d x %d\n", ulx,uly,  *ow, *oh);

   if((ret = wsq_huffcode_mem(wsq_data, olen, 
		    qdata2, *ow, *oh, 
		    idata, ilen, *hgt_pos, *huff_pos))){
       free(qdata2);
       return(ret);
   }

   /* Done with cropped quantized image buffer. */
   free(qdata2);

   *odata = wsq_data;

   /* Return normally. */
   return(0);
}
/*************************************************************
   This function Huffman encodes a quantized coefficient 
   subband array (qdata2) for a widthxheight image. It can 
   only be called after wsq_crop_qdata, which sets up q_tree2.

   Other inputs are
      idata      : Buffer with exact header info up to
                   position huff_pos (except height/width)
      hgt_pos    : Location of frame header Height field.
      huff_pos   : Location where huffman coding tables
                   begin in the codestream.
      wsq_data   : Buffer for output data (exists)
      wsq_alloc  : Length of wsq_data

   On return wsq_data contains the compressed codestream
   and olen is the encoded length.

The bulk of this code is a near copy of parts of wsq_encode_mem.
***************************************************************/
int wsq_huffcode_mem(
     unsigned char *wsq_data, /* Output WSQ memory */
     int *olen,               /* Output WSQ length */
     short *qdata2,           /* Cropped coefficients to encode */
     int width, int height,   /* Dimensions of cropped area */
     unsigned char *idata,    /* Original WSQ codestream */
     const int wsq_alloc,     /* Available length of wsq_data */
     const int hgt_pos,       /* Position of Frame Header Height in idata */
     const int huff_pos       /* Position where Huff data begins in idata */
)
{
   int ret, num_pix;
   int qsize1, qsize2, qsize3;   /* Quantized block sizes */
   unsigned char *huffbits, *huffvalues; /* huffman code parameters     */
   HUFFCODE *hufftable;          /* huffcode table              */
   unsigned char *huff_buf;      /* huffman encoded buffer      */
   int hsize, hsize1, hsize2, hsize3; /* Huffman coded blocks sizes */
   int wsq_len;                 /* number of bytes used in buffer   */
   int block_sizes[2];

   /* Copy the original buffer into the new, up to where huffman tables begin. */
   /* Incorrect sections will be overwritten as we go along.                    */
   memcpy(wsq_data,idata,huff_pos);

   /* Modify width/height fields in wsq_data buffer */
   wsq_len = hgt_pos;
   putc_ushort(height,wsq_data,huff_pos,&wsq_len);
   putc_ushort(width,wsq_data,huff_pos,&wsq_len);

   if(debug > 0)
      fprintf(stderr, "SOI, tables, and frame header written\n\n");

   /* Compute quantized WSQ subband block sizes */
   quant_block_sizes2(&qsize1, &qsize2, &qsize3, &dqt_table,
                           w_tree, W_TREELEN, q_tree2, Q_TREELEN);

   wsq_len = huff_pos;

   /* Cropped image dimensions */
   num_pix = height * width;

   /* Allocate a temporary buffer for holding compressed block data.    */
   /* This buffer is allocated to the size of the original input image, */
   /* and it is "assumed" that the compressed blocks will not exceed    */
   /* this buffer size.                                                 */
   huff_buf = (unsigned char *)malloc(num_pix);
   if(huff_buf == (unsigned char *)NULL) {
      free(qdata2);
      fprintf(stderr, "ERROR : wsq_huffcode_1 : malloc : huff_buf\n");
      return(-13);
   }

   /******************/
   /* ENCODE Block 1 */
   /******************/
   /* Compute Huffman table for Block 1. */
   if((ret = gen_hufftable_wsq(&hufftable, &huffbits, &huffvalues,
                              qdata2, &qsize1, 1))){
      free(qdata2);
      free(huff_buf);
      return(ret);
   }

   /* Store Huffman table for Block 1 to WSQ buffer. */
   if((ret = putc_huffman_table(DHT_WSQ, 0, huffbits, huffvalues,
                               wsq_data, wsq_alloc, &wsq_len))){
      free(qdata2);
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
   if((ret = compress_block(huff_buf, &hsize1, qdata2, qsize1,
                           MAX_HUFFCOEFF, MAX_HUFFZRUN, hufftable))){
      free(qdata2);
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
      free(qdata2);
      free(huff_buf);
      return(ret);
   }

   /* Store Block 1's compressed data to WSQ buffer. */
   if((ret = putc_bytes(huff_buf, hsize1, wsq_data, wsq_alloc, &wsq_len))){
      free(qdata2);
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
                          qdata2+qsize1, block_sizes, 2))){
      free(qdata2);
      free(huff_buf);
      return(ret);
   }

   /* Store Huffman table for Blocks 2 & 3 to WSQ buffer. */
   if((ret = putc_huffman_table(DHT_WSQ, 1, huffbits, huffvalues,
                               wsq_data, wsq_alloc, &wsq_len))){
      free(qdata2);
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
   if((ret = compress_block(huff_buf, &hsize2, qdata2+qsize1, qsize2,
                           MAX_HUFFCOEFF, MAX_HUFFZRUN, hufftable))){
      free(qdata2);
      free(huff_buf);
      free(hufftable);
      return(ret);
   }

   /* Accumulate number of bytes compressed. */
   hsize += hsize2;

   /* Store Block 2's header to WSQ buffer. */
   if((ret = putc_block_header(1, wsq_data, wsq_alloc, &wsq_len))){
      free(qdata2);
      free(huff_buf);
      free(hufftable);
      return(ret);
   }

   /* Store Block 2's compressed data to WSQ buffer. */
   if((ret = putc_bytes(huff_buf, hsize2, wsq_data, wsq_alloc, &wsq_len))){
      free(qdata2);
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
   if((ret = compress_block(huff_buf, &hsize3, qdata2+qsize1+qsize2, qsize3,
                           MAX_HUFFCOEFF, MAX_HUFFZRUN, hufftable))){
      free(qdata2);
      free(huff_buf);
      free(hufftable);
      return(ret);
   }
   /* Done with current Huffman table. */
   free(hufftable);

   /* Accumulate number of bytes compressed. */
   hsize += hsize3;

   /* Store Block 3's header to WSQ buffer. */
   if((ret = putc_block_header(1, wsq_data, wsq_alloc, &wsq_len))){
      free(huff_buf);
      return(ret);
   }

   /* Store Block 3's compressed data to WSQ buffer. */
   if((ret = putc_bytes(huff_buf, hsize3, wsq_data, wsq_alloc, &wsq_len))){
      free(huff_buf);
      return(ret);
   }

   if(debug > 0)
      fprintf(stderr, "Block 3 compressed and written\n\n");

   /* Done with huffman compressing blocks, so done with buffer. */
   free(huff_buf);

   /* Add a End Of Image (EOI) marker to the WSQ buffer. */
   if((ret = putc_ushort(EOI_WSQ, wsq_data, wsq_alloc, &wsq_len))){
      return(ret);
   }

   if(debug > 1) {
      fprintf(stderr,
              "hsize1 = %d :: hsize2 = %d :: hsize3 = %d\n", hsize1, hsize2, hsize3);
   }
   if(debug > 0)
      fprintf(stdout,"  \t\tCropped complen = %d :: ratio = %.1f\n",
              hsize, (float)(num_pix)/(float)hsize);

   *olen = wsq_len;

   /* Return normally. */
   return(0);
}

/*************************************************************
   This function Huffman decodes WSQ memory into a quantized
   coefficient subband array (pqdata). Several other values
   are returned as well:
      iw, ih     :  Image (and pqdata) dimensions
      scale,shift:  Parameters from frame header
      hgt_pos    :  Location of frame header Height field.
      huff_pos   :  Location where huffman coding tables
                    begin in the codestream.
   This does not free_wsq_decoder_resources when successful.
   The calling function should do this once dtt_table is no
   longer required.

The bulk of this code is a near copy of parts of wsq_decode_mem.
***************************************************************/
int wsq_dehuff_mem(
   short **pqdata,    /* Returned pointer to quantized coeff data */
   int *iw, int *ih,  /* Dimensions of qdata / image */
   double *scale,     /* r_scale from Frame header */
   double *shift,     /* m_shift from Frame header */
   int *hgt_pos,      /* Position of Frame Header Height in idata */
   int *huff_pos,     /* Position where Huff data begins in idata */
   unsigned char *idata, /* Input WSQ mem */
   const int ilen        /* Length of idata */
)
{
   int ret, i, num_pix;
   unsigned short marker;         /* WSQ marker */
   int width, height;             /* image parameters */
   unsigned char *cbufptr;        /* points to current byte in buffer */
   unsigned char *ebufptr;        /* points to end of buffer */
   short *qdata;                  /* image pointers */
   int ihsize;
   int found_dqt, found_dtt;

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

   found_dqt = 0;
   found_dtt = 0;
   
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
      if (marker == DQT_WSQ) found_dqt = 1;
      else if (marker == DTT_WSQ) found_dtt = 1;

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

   /* Figure out where the frame header image dimensions are located 
      since later functions may want to change the contents */
   *hgt_pos = cbufptr-idata - 13;

   width = frm_header_wsq.width;
   height = frm_header_wsq.height;
   *scale = frm_header_wsq.r_scale;
   *shift = frm_header_wsq.m_shift;
   *iw = width;
   *ih = height;

   if(debug > 0)
      fprintf(stderr, "SOI, tables, and frame header read\n\n");

   /* Build WSQ decomposition trees. */
   build_wsq_trees(w_tree, W_TREELEN, q_tree, Q_TREELEN, width, height);

   if(debug > 0)
      fprintf(stderr, "Tables for wavelet decomposition finished\n\n");

   /* The Q-tables and T-tables are not always located prior to the 
      frame header.  We need to find out where they finally appear and
      mark an end position.  Later code may be copying this section of 
      the codestream verbatim, to make sure to retain Q/T-tables. 
      (Another way to do this would be to read info during normal decode, 
      and allow them to be written out during encode functions. But 
      library structures and functions don't currently support that option.)
   */
   if (found_dqt && found_dtt) {
     /* Remember where the huffman encoded data starts. This is where */
     /* the next output writing will occur. */
     *huff_pos = cbufptr - idata;
     /* Remember the original huffman stream length */
     ihsize = ilen - *huff_pos;  
   }
   else { /* Continue looking for transform or q-tables */
     if((ret = getc_marker_wsq(&marker, TBLS_N_SOB, &cbufptr, ebufptr))){
       free_wsq_decoder_resources();
       return(ret);
     }
     while(marker != SOB_WSQ && marker != DHT_WSQ) {
       if((ret = getc_table_wsq(marker, &dtt_table, &dqt_table,
				dht_table, &cbufptr, ebufptr))){
	 free_wsq_decoder_resources();
	 return(ret);
       }       
       if (marker == DQT_WSQ) found_dqt = 1;
       else if (marker == DTT_WSQ) found_dtt = 1;
       if (found_dqt && found_dtt) break;
       
       if((ret = getc_marker_wsq(&marker, TBLS_N_SOB, &cbufptr, ebufptr))){
	 free_wsq_decoder_resources();
	 return(ret);
       }
     }
     if (found_dqt && found_dtt) {
       /* Remember where the huffman encoded data starts. This is where */
       /* the next output writing will occur. */
       *huff_pos = cbufptr - idata;
       /* Remember the original huffman stream length */
       ihsize = ilen - *huff_pos;  
     }
     else {
       fprintf(stderr,"ERROR: Didn't find DTT and DQT before DHT\n");
     }
   }

   num_pix = width * height;

   /* Allocate working memory. */
   qdata = (short *) malloc(num_pix * sizeof(short));
   if(qdata == (short *)NULL) {
      free_wsq_decoder_resources();
      fprintf(stderr,"ERROR: wsq_dehuff_mem : malloc : qdata1\n");
      return(-20);
   }

   /* Decode the Huffman encoded data blocks. */
   if((ret = huffman_decode_data_mem(qdata, &dtt_table, &dqt_table, dht_table,
				     &cbufptr, ebufptr))){
      free(qdata);
      free_wsq_decoder_resources();
      return(ret);
   }
   /* Compute original huffman coded length */
   ihsize -= ebufptr-cbufptr;

   if(debug > 0)
      fprintf(stderr,
	     "Quantized WSQ subband data blocks read and Huffman decoded\n\n");

   *pqdata = qdata;

   if (debug > 0) 
     fprintf(stdout,"Original complen = %d :: ratio = %.3f \n",
	     ihsize, (float)(num_pix)/(float)ihsize);

   /* Return normally. */
   return(0);
}

/*************************************************************
   This function parses WSQ memory until the frame header is
   found. The image dimensions and scale and shift values in
   the header are returned.
   Inputs:
      idata      :  WSQ data
      ilen       :  WSQ length
   Outputs:
      iw, ih     :  Image (and pqdata) dimensions
      scale,shift:  Parameters from frame header

***************************************************************/
int read_wsq_frame_header(
   unsigned char *idata, /* Input WSQ mem */
   const int ilen,       /* Length of idata */
   int *iw, int *ih,     /* Dimensions of image */
   double *scale,        /* r_scale from Frame header */
   double *shift        /* m_shift from Frame header */
)
{
   int ret;
   unsigned short marker;         /* WSQ marker */
   unsigned char *cbufptr;        /* points to current byte in buffer */
   unsigned char *ebufptr;        /* points to end of buffer */
   FRM_HEADER_WSQ frm_header;
   unsigned short  hdr_size;

   /* Set memory buffer pointers. */
   cbufptr = idata;
   ebufptr = idata + ilen;

   /* Read the SOI marker. */
   if((ret = getc_marker_wsq(&marker, SOI_WSQ, &cbufptr, ebufptr))){
     return(ret);
   }

   if((ret = getc_marker_wsq(&marker, TBLS_N_SOF, &cbufptr, ebufptr))){
     return(ret);
   }
   while(marker != SOF_WSQ) {
     if((ret = getc_ushort(&hdr_size,&cbufptr,ebufptr)))
       return(ret);
     cbufptr += hdr_size-2;
     if((ret = getc_marker_wsq(&marker, TBLS_N_SOF, &cbufptr, ebufptr)))
       return(ret);
   }
   if((ret = getc_frame_header_wsq(&frm_header, &cbufptr, ebufptr))){
     return(ret);
   }

   *iw = frm_header.width;
   *ih = frm_header.height;
   *scale = frm_header.r_scale;
   *shift = frm_header.m_shift;

   return(0);
}
