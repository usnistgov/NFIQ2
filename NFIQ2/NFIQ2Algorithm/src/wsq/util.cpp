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

      FILE:    UTIL.C
      AUTHORS: Craig Watson
               Michael Garris
      DATE:    11/24/1999
      UPDATED: 02/24/2005 by MDG

      Contains gernal routines responsible for supporting WSQ
      image compression.

      ROUTINES:
#cat: conv_img_2_flt_ret - Converts an image's unsigned character pixels
#cat:                    to floating point values in the range +/- 128.0.
#cat:                    Returns on error.
#cat: conv_img_2_flt - Converts an image's unsigned character pixels
#cat:                  to floating point values in the range +/- 128.0.
#cat: conv_img_2_uchar - Converts an image's floating point pixels
#cat:                  unsigned character pixels.
#cat: variance - Calculates the variances within image subbands.
#cat:
#cat: quantize - Quantizes the image's wavelet subbands.
#cat:
#cat: quant_block_sizes - Quantizes an image's subband block.
#cat:
#cat: unquantize - Unquantizes an image's wavelet subbands.
#cat:
#cat: wsq_decompose - Computes the wavelet decomposition of an input image.
#cat:
#cat: get_lets - Compute the wavelet subband decomposition for the image.
#cat:
#cat: wsq_reconstruct - Reconstructs a lossy floating point pixmap from
#cat:                  a WSQ compressed datastream.
#cat: join_lets - Reconstruct the image from the wavelet subbands.
#cat:
#cat: int_sign - Get the sign of the sythesis filter coefficients.
#cat:
#cat: image_size - Computes the size in bytes of a WSQ compressed image
#cat:                  file, including headers, tables, and parameters.
#cat: init_wsq_decoder_resources - Initializes memory resources used by the
#cat:                      WSQ decoder
#cat: free_wsq_decoder_resources - Deallocates memory resources used by the
#cat:                      WSQ decoder

***********************************************************************/

#include <math.h>
#include <stdio.h>
#include <string.h>

#include <wsq.h>
#include <defs.h>
#include <dataio.h>

/******************************************************************/
/*        The routines in this file do numerous things            */
/*        related to the WSQ algorithm such as:                   */
/*        converting the image data from unsigned char            */
/*        to float and integer to unsigned char,                  */
/*        splitting the image into the subbands as well           */
/*        the rejoining process, subband variance                 */
/*        calculations, and quantization.                         */
/******************************************************************/
/******************************************************************/
/* This routine converts the unsigned char data to float.  In the */
/* process it shifts and scales the data so the values range from */
/* +/- 128.0 This function returns on error.                      */
/******************************************************************/
int conv_img_2_flt_ret(
   float *fip,         /* output float image data  */
   float *m_shift,     /* shifting parameter       */
   float *r_scale,     /* scaling parameter        */
   unsigned char *data,        /* input unsigned char data */
   const int num_pix)  /* num pixels in image      */

{
   int cnt;                     /* pixel cnt */
   unsigned int sum, overflow;  /* sum of pixel values */
   float mean;                  /* mean pixel value */
   int low, high;               /* low/high pixel values */
   float low_diff, high_diff;   /* new low/high pixels values shifting */

   sum = 0;
   overflow = 0;
   low = 255;
   high = 0;
   for(cnt = 0; cnt < num_pix; cnt++) {
      if(data[cnt] > high)
         high = data[cnt];
      if(data[cnt] < low)
         low = data[cnt];
      sum += data[cnt];
      if(sum < overflow) {
         fprintf(stderr, "ERROR: conv_img_2_flt: overflow at %d\n", cnt);
         return(-91);
      }
      overflow = sum;
   }

   mean = (float) sum / (float)num_pix;
   *m_shift = mean;

   low_diff = *m_shift - low;
   high_diff = high - *m_shift;

   if(low_diff >= high_diff)
      *r_scale = low_diff;
   else
      *r_scale = high_diff;

   *r_scale /= (float)128.0;

   for(cnt = 0; cnt < num_pix; cnt++) {
      fip[cnt] = ((float)data[cnt] - *m_shift) / *r_scale;
   }
   return(0);
}

/******************************************************************/
/* This routine converts the unsigned char data to float.  In the */
/* process it shifts and scales the data so the values range from */
/* +/- 128.0                                                      */
/******************************************************************/
void conv_img_2_flt(
   float *fip,         /* output float image data  */
   float *m_shift,     /* shifting parameter       */
   float *r_scale,     /* scaling parameter        */
   unsigned char *data,        /* input unsigned char data */
   const int num_pix)  /* num pixels in image      */

{
   int cnt;                     /* pixel cnt */
   unsigned int sum, overflow;  /* sum of pixel values */
   float mean;                  /* mean pixel value */
   int low, high;               /* low/high pixel values */
   float low_diff, high_diff;   /* new low/high pixels values shifting */

   sum = 0;
   overflow = 0;
   low = 255;
   high = 0;
   for(cnt = 0; cnt < num_pix; cnt++) {
      if(data[cnt] > high)
         high = data[cnt];
      if(data[cnt] < low)
         low = data[cnt];
      sum += data[cnt];
      if(sum < overflow) {
         fprintf(stderr, "ERROR: conv_img_2_flt: overflow at pixel %d\n", cnt);
         exit(-1);
      }
      overflow = sum;
   }

   mean = (float) sum / (float)num_pix;
   *m_shift = mean;

   low_diff = *m_shift - low;
   high_diff = high - *m_shift;

   if(low_diff >= high_diff)
      *r_scale = low_diff;
   else
      *r_scale = high_diff;

   *r_scale /= (float)128.0;

   for(cnt = 0; cnt < num_pix; cnt++) {
      fip[cnt] = ((float)data[cnt] - *m_shift) / *r_scale;
   }
}

/*********************************************************/
/* Routine to convert image from float to unsigned char. */
/*********************************************************/
void conv_img_2_uchar(
   unsigned char *data,                   /* uchar image pointer    */
   float *img,                    /* image pointer          */
   const int width,               /* image width            */
   const int height,              /* image height           */
   const float m_shift,           /* shifting parameter     */
   const float r_scale)           /* scaling parameter      */
{
   int r, c;       /* row/column counters */
   float img_tmp;  /* temp image data store */

   for (r = 0; r < height; r++) {
      for (c = 0; c < width; c++) {
         img_tmp = (*img * r_scale) + m_shift;
         img_tmp += 0.5;
         if (img_tmp < 0.0)
            *data = 0; /* neg pix poss after quantization */
         else if (img_tmp > 255.0)
            *data = 255;
         else
            *data = (unsigned char)img_tmp;

         ++img;
         ++data;
      }
   }
}

/**********************************************************/
/* This routine calculates the variances of the subbands. */
/**********************************************************/
void variance(
   QUANT_VALS *quant_vals, /* quantization parameters */
   Q_TREE q_tree[],        /* quantization "tree"     */
   const int q_treelen,    /* length of q_tree        */
   float *fip,             /* image pointer           */
   const int width,        /* image width             */
   const int height)       /* image height            */
{
   float *fp;              /* temp image pointer */
   int cvr;                /* subband counter */
   int lenx = 0, leny = 0; /* dimensions of area to calculate variance */
   int skipx, skipy;       /* pixels to skip to get to area for
                              variance calculation */
   int row, col;           /* dimension counters */
   float ssq;             /* sum of squares */
   float sum2;            /* variance calculation parameter */
   float sum_pix;         /* sum of pixels */
   float vsum;            /* variance sum for subbands 0-3 */
   

   vsum = 0.0;
   for(cvr = 0; cvr < 4; cvr++) {
      fp = fip + (q_tree[cvr].y * width) + q_tree[cvr].x;
      ssq = 0.0;
      sum_pix = 0.0;

      skipx = q_tree[cvr].lenx / 8;
      skipy = (9 * q_tree[cvr].leny)/32;
   
      lenx = (3 * q_tree[cvr].lenx)/4;
      leny = (7 * q_tree[cvr].leny)/16;

      fp += (skipy * width) + skipx;
      for(row = 0; row < leny; row++, fp += (width - lenx)) {
         for(col = 0; col < lenx; col++) {
            sum_pix += *fp;
            ssq += *fp * *fp;
            fp++;
         }
      }
      sum2 = (sum_pix * sum_pix)/(lenx * leny);
      quant_vals->var[cvr] = (float)((ssq - sum2)/((lenx * leny)-1.0));
      vsum += quant_vals->var[cvr];
   }

   if(vsum < 20000.0) {
      for(cvr = 0; cvr < NUM_SUBBANDS; cvr++) {
         fp = fip + (q_tree[cvr].y * width) + q_tree[cvr].x;
         ssq = 0.0;
         sum_pix = 0.0;

         lenx = q_tree[cvr].lenx;
         leny = q_tree[cvr].leny;

         for(row = 0; row < leny; row++, fp += (width - lenx)) {
            for(col = 0; col < lenx; col++) {
               sum_pix += *fp;
               ssq += *fp * *fp;
               fp++;
            }
         }
         sum2 = (sum_pix * sum_pix)/(lenx * leny);
         quant_vals->var[cvr] = (float)((ssq - sum2)/((lenx * leny)-1.0));
      }
   }
   else {
      for(cvr = 4; cvr < NUM_SUBBANDS; cvr++) {
         fp = fip + (q_tree[cvr].y * width) + q_tree[cvr].x;
         ssq = 0.0;
         sum_pix = 0.0;

         skipx = q_tree[cvr].lenx / 8;
         skipy = (9 * q_tree[cvr].leny)/32;
   
         lenx = (3 * q_tree[cvr].lenx)/4;
         leny = (7 * q_tree[cvr].leny)/16;

         fp += (skipy * width) + skipx;
         for(row = 0; row < leny; row++, fp += (width - lenx)) {
            for(col = 0; col < lenx; col++) {
               sum_pix += *fp;
               ssq += *fp * *fp;
               fp++;
            }
         }
         sum2 = (sum_pix * sum_pix)/(lenx * leny);
         quant_vals->var[cvr] = (float)((ssq - sum2)/((lenx * leny)-1.0));
      }
   }
}

/************************************************/
/* This routine quantizes the wavelet subbands. */
/************************************************/
int quantize(
   short **osip,           /* quantized output             */
   int *ocmp_siz,          /* size of quantized output     */
   QUANT_VALS *quant_vals, /* quantization parameters      */
   Q_TREE q_tree[],        /* quantization "tree"          */
   const int q_treelen,    /* size of q_tree               */
   float *fip,             /* floating point image pointer */
   const int width,        /* image width                  */
   const int height)       /* image height                 */
{
   int i;                 /* temp counter */
   int j;                 /* interation index */
   float *fptr;           /* temp image pointer */
   short *sip, *sptr;     /* pointers to quantized image */
   int row, col;          /* temp image characteristic parameters */
   int cnt;               /* subband counter */
   float zbin;            /* zero bin size */
   float A[NUM_SUBBANDS]; /* subband "weights" for quantization */
   float m[NUM_SUBBANDS]; /* subband size to image size ratios */
                          /* (reciprocal of FBI spec for 'm')  */
   float m1, m2, m3;      /* reciprocal constants for 'm' */
   float sigma[NUM_SUBBANDS]; /* square root of subband variances */
   int K0[NUM_SUBBANDS];  /* initial list of subbands w/variance >= thresh */
   int K1[NUM_SUBBANDS];  /* working list of subbands */
   int *K, *nK;           /* pointers to sets of subbands */
   int NP[NUM_SUBBANDS];  /* current subbounds with nonpositive bit rates. */
   int K0len;             /* number of subbands in K0 */
   int Klen, nKlen;       /* number of subbands in other subband lists */
   int NPlen;             /* number of subbands flagged in NP */
   float S;               /* current frac of subbands w/positive bit rate */
   float q;               /* current proportionality constant */
   float P;               /* product of 'q/Q' ratios */

   /* Set up 'A' table. */   
   for(cnt = 0; cnt < STRT_SUBBAND_3; cnt++)
      A[cnt] = 1.0;
   A[cnt++ /*52*/] = 1.32;
   A[cnt++ /*53*/] = 1.08;
   A[cnt++ /*54*/] = 1.42;
   A[cnt++ /*55*/] = 1.08;
   A[cnt++ /*56*/] = 1.32;
   A[cnt++ /*57*/] = 1.42;
   A[cnt++ /*58*/] = 1.08;
   A[cnt++ /*59*/] = 1.08;

   for(cnt = 0; cnt < MAX_SUBBANDS; cnt++) {
      quant_vals->qbss[cnt] = 0.0;
      quant_vals->qzbs[cnt] = 0.0;
   }

   /* Set up 'Q1' (prime) table. */
   for(cnt = 0; cnt < NUM_SUBBANDS; cnt++) {
      if(quant_vals->var[cnt] < VARIANCE_THRESH)
         quant_vals->qbss[cnt] = 0.0;
      else
         /* NOTE: q has been taken out of the denominator in the next */
         /*       2 formulas from the original code. */
         if(cnt < STRT_SIZE_REGION_2 /*4*/)
            quant_vals->qbss[cnt] = 1.0;
         else
            quant_vals->qbss[cnt] = 10.0 / (A[cnt] *
                                    (float)log(quant_vals->var[cnt]));
   }


   /* Set up output buffer. */
   if((sip = (short *) calloc(width*height, sizeof(short))) == NULL) {
      fprintf(stderr,"ERROR : quantize : calloc : sip\n");
      return(-90);
   }
   sptr = sip;

   /* Set up 'm' table (these values are the reciprocal of 'm' in */
   /* the FBI spec).                                              */
   m1 = 1.0/1024.0;
   m2 = 1.0/256.0;
   m3 = 1.0/16.0;
   for(cnt = 0; cnt < STRT_SIZE_REGION_2; cnt++)
      m[cnt] = m1;
   for(cnt = STRT_SIZE_REGION_2; cnt < STRT_SIZE_REGION_3; cnt++)
      m[cnt] = m2;
   for(cnt = STRT_SIZE_REGION_3; cnt < NUM_SUBBANDS; cnt++)
      m[cnt] = m3;

   j = 0;
   /* Initialize 'K0' and 'K1' lists. */
   K0len = 0;
   for(cnt = 0; cnt < NUM_SUBBANDS; cnt++){
      if(quant_vals->var[cnt] >= VARIANCE_THRESH){
         K0[K0len] = cnt;
         K1[K0len++] = cnt;
         /* Compute square root of subband variance. */
         sigma[cnt] = sqrt(quant_vals->var[cnt]);
      }
   }
   K = K1;
   Klen = K0len;

   while(1){
      /* Compute new 'S' */
      S = 0.0;
      for(i = 0; i < Klen; i++){
         /* Remeber 'm' is the reciprocal of spec. */
         S += m[K[i]];
      }

      /* Compute product 'P' */
      P = 1.0;
      for(i = 0; i < Klen; i++){
         /* Remeber 'm' is the reciprocal of spec. */
         P *= pow((sigma[K[i]] / quant_vals->qbss[K[i]]), m[K[i]]);
      }

      /* Compute new 'q' */
      q = (pow(2.0f,(float)((quant_vals->r/S)-1.0))/2.5) / pow(P, (float)(1.0/S));

      /* Flag subbands with non-positive bitrate. */
      memset(NP, 0, NUM_SUBBANDS * sizeof(int));
      NPlen = 0;
      for(i = 0; i < Klen; i++){
         if((quant_vals->qbss[K[i]] / q) >= (5.0*sigma[K[i]])){
            NP[K[i]] = TRUE;
            NPlen++;
         }
      }

      /* If list of subbands with non-positive bitrate is empty ... */
      if(NPlen == 0){
         /* Then we are done, so break from while loop. */
         break;
      }

      /* Assign new subband set to previous set K minus subbands in set NP. */
      nK = K1;
      nKlen = 0;
      for(i = 0; i < Klen; i++){
         if(!NP[K[i]])
            nK[nKlen++] = K[i];
      }

      /* Assign new set as K. */
      K = nK;
      Klen = nKlen;

      /* Bump iteration counter. */
      j++;
   }

   /* Flag subbands that are in set 'K0' (the very first set). */
   nK = K1;
   memset(nK, 0, NUM_SUBBANDS * sizeof(int));
   for(i = 0; i < K0len; i++){
      nK[K0[i]] = TRUE;
   }
   /* Set 'Q' values. */
   for(cnt = 0; cnt < NUM_SUBBANDS; cnt++) {
      if(nK[cnt])
         quant_vals->qbss[cnt] /= q;
      else
         quant_vals->qbss[cnt] = 0.0;
      quant_vals->qzbs[cnt] = 1.2 * quant_vals->qbss[cnt];
   }

   /* Now ready to compute and store bin widths for subbands. */
   for(cnt = 0; cnt < NUM_SUBBANDS; cnt++) {
      fptr = fip + (q_tree[cnt].y * width) + q_tree[cnt].x;

      if(quant_vals->qbss[cnt] != 0.0) {

         zbin = quant_vals->qzbs[cnt] / 2.0;

         for(row = 0;
            row < q_tree[cnt].leny;
            row++, fptr += width - q_tree[cnt].lenx){
            for(col = 0; col < q_tree[cnt].lenx; col++) {
               if(-zbin <= *fptr && *fptr <= zbin)
                  *sptr = 0;
               else if(*fptr > 0.0)
                  *sptr = (short)(((*fptr-zbin)/quant_vals->qbss[cnt]) + 1.0);
               else
                  *sptr = (short)(((*fptr+zbin)/quant_vals->qbss[cnt]) - 1.0);
               sptr++;
               fptr++;
            }
         }
      }
      else if(debug > 0)
         fprintf(stderr, "%d -> %3.6f\n", cnt, quant_vals->qbss[cnt]);
   }

   *osip = sip;
   *ocmp_siz = sptr - sip;
   return(0);
}

/************************************************************************/
/* Compute quantized WSQ subband block sizes.                           */
/************************************************************************/
void quant_block_sizes(int *oqsize1, int *oqsize2, int *oqsize3,
                 QUANT_VALS *quant_vals,
                 W_TREE w_tree[], const int w_treelen,
                 Q_TREE q_tree[], const int q_treelen)
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
      if(quant_vals->qbss[node] == 0.0)
         qsize1 -= (q_tree[node].lenx * q_tree[node].leny);

   for (node = STRT_SUBBAND_2; node < STRT_SUBBAND_3; node++)
      if(quant_vals->qbss[node] == 0.0)
          qsize2 -= (q_tree[node].lenx * q_tree[node].leny);

   for (node = STRT_SUBBAND_3; node < STRT_SUBBAND_DEL; node++)
      if(quant_vals->qbss[node] == 0.0)
         qsize3 -= (q_tree[node].lenx * q_tree[node].leny);

   *oqsize1 = qsize1;
   *oqsize2 = qsize2;
   *oqsize3 = qsize3;
}

/*************************************/
/* Routine to unquantize image data. */
/*************************************/
int unquantize(
   float **ofip,         /* floating point image pointer         */
   const DQT_TABLE *dqt_table, /* quantization table structure   */
   Q_TREE q_tree[],      /* quantization table structure         */
   const int q_treelen,  /* size of q_tree                       */
   short *sip,           /* quantized image pointer              */
   const int width,      /* image width                          */
   const int height)     /* image height                         */
{
   float *fip;    /* floating point image */
   int row, col;  /* cover counter and row/column counters */
   float C;       /* quantizer bin center */
   float *fptr;   /* image pointers */
   short *sptr;
   int cnt;       /* subband counter */

   if((fip = (float *) calloc(width*height, sizeof(float))) == NULL) {
      fprintf(stderr,"ERROR : unquantize : calloc : fip\n");
      return(-91);
   }
   if(dqt_table->dqt_def != 1) {
      fprintf(stderr,
      "ERROR: unquantize : quantization table parameters not defined!\n");
      return(-92);
   }

   sptr = sip;
   C = dqt_table->bin_center;
   for(cnt = 0; cnt < NUM_SUBBANDS; cnt++) {
      if(dqt_table->q_bin[cnt] == 0.0)
         continue;
      fptr = fip + (q_tree[cnt].y * width) + q_tree[cnt].x;

      for(row = 0;
          row < q_tree[cnt].leny;
          row++, fptr += width - q_tree[cnt].lenx){
         for(col = 0; col < q_tree[cnt].lenx; col++) {
            if(*sptr == 0)
               *fptr = 0.0;
            else if(*sptr > 0)
               *fptr = (dqt_table->q_bin[cnt] * ((float)*sptr - C))
                    + (dqt_table->z_bin[cnt] / 2.0);
            else if(*sptr < 0)
               *fptr = (dqt_table->q_bin[cnt] * ((float)*sptr + C))
                    - (dqt_table->z_bin[cnt] / 2.0);
            else {
               fprintf(stderr,
               "ERROR : unquantize : invalid quantization pixel value\n");
               return(-93);
            }
            fptr++;
            sptr++;
         }
      }
   }

   *ofip = fip;
   return(0);
}

/************************************************************************/
/* WSQ decompose the image.  NOTE: this routine modifies and returns    */
/* the results in "fdata".                                              */
/************************************************************************/
int wsq_decompose(float *fdata, const int width, const int height,
                  W_TREE w_tree[], const int w_treelen,
                  float *hifilt, const int hisz,
                  float *lofilt, const int losz)
{
   int num_pix, node;
   float *fdata1, *fdata_bse;

   num_pix = width * height;
   /* Allocate temporary floating point pixmap. */
   if((fdata1 = (float *) malloc(num_pix*sizeof(float))) == NULL) {
      fprintf(stderr,"ERROR : wsq_decompose : malloc : fdata1\n");
      return(-94);
   }

   /* Compute the Wavelet image decomposition. */
   for(node = 0; node < w_treelen; node++) {
      fdata_bse = fdata + (w_tree[node].y * width) + w_tree[node].x;
      get_lets(fdata1, fdata_bse, w_tree[node].leny, w_tree[node].lenx,
               width, 1, hifilt, hisz, lofilt, losz, w_tree[node].inv_rw);
      get_lets(fdata_bse, fdata1, w_tree[node].lenx, w_tree[node].leny,
               1, width, hifilt, hisz, lofilt, losz, w_tree[node].inv_cl);
   }
   free(fdata1);

   return(0);
}

/************************************************************/
/************************************************************/
void get_lets(
   float *ne,     /* image pointers for creating subband splits */
   float *old,
   const int len1,       /* temporary length parameters */
   const int len2,
   const int pitch,      /* pitch gives next row_col to filter */
   const int  stride,    /*           stride gives next pixel to filter */
   float *hi,
   const int hsz,   /* NEW */
   float *lo,      /* filter coefficients */
   const int lsz,   /* NEW */
   const int inv)        /* spectral inversion? */
{
   float *lopass, *hipass;	/* pointers of where to put lopass
                                   and hipass filter outputs */
   float *p0,*p1;		/* pointers to image pixels used */
   int pix, rw_cl;		/* pixel counter and row/column counter */
   int i, da_ev;		/* even or odd row/column of pixels */
   int fi_ev;
   int loc, hoc, nstr, pstr;
   int llen, hlen;
   int lpxstr, lspxstr;
   float *lpx, *lspx;
   int hpxstr, hspxstr;
   float *hpx, *hspx;
   int olle, ohle;
   int olre, ohre;
   int lle, lle2;
   int lre, lre2;
   int hle, hle2;
   int hre, hre2;


   da_ev = len2 % 2;
   fi_ev = lsz % 2;

   if(fi_ev) {
      loc = (lsz-1)/2;
      hoc = (hsz-1)/2 - 1;
      olle = 0;
      ohle = 0;
      olre = 0;
      ohre = 0;
   }
   else {
      loc = lsz/2 - 2;
      hoc = hsz/2 - 2;
      olle = 1;
      ohle = 1;
      olre = 1;
      ohre = 1;

      if(loc == -1) {
         loc = 0;
         olle = 0;
      }
      if(hoc == -1) {
         hoc = 0;
         ohle = 0;
      }

      for(i = 0; i < hsz; i++)
         hi[i] *= -1.0;
   }

   pstr = stride;
   nstr = -pstr;

   if(da_ev) {
      llen = (len2+1)/2;
      hlen = llen - 1;
   }
   else {
      llen = len2/2;
      hlen = llen;
   }


   for(rw_cl = 0; rw_cl < len1; rw_cl++) {

      if(inv) {
         hipass = ne + rw_cl * pitch;
         lopass = hipass + hlen * stride;
      }
      else {
         lopass = ne + rw_cl * pitch;
         hipass = lopass + llen * stride;
      }

      p0 = old + rw_cl * pitch;
      p1 = p0 + (len2-1) * stride;

      lspx = p0 + (loc * stride);
      lspxstr = nstr;
      lle2 = olle;
      lre2 = olre;
      hspx = p0 + (hoc * stride);
      hspxstr = nstr;
      hle2 = ohle;
      hre2 = ohre;
      for(pix = 0; pix < hlen; pix++) {
         lpxstr = lspxstr;
         lpx = lspx;
         lle = lle2;
         lre = lre2;
         *lopass = *lpx * lo[0];
         for(i = 1; i < lsz; i++) {
            if(lpx == p0){
               if(lle) {
                  lpxstr = 0;
                  lle = 0;
               }
               else
                  lpxstr = pstr;
            }
            if(lpx == p1){
               if(lre) {
                  lpxstr = 0;
                  lre = 0;
               }
               else
                  lpxstr = nstr;
            }
            lpx += lpxstr;
            *lopass += *lpx * lo[i];
         }
         lopass += stride;

         hpxstr = hspxstr;
         hpx = hspx;
         hle = hle2;
         hre = hre2;
         *hipass = *hpx * hi[0];
         for(i = 1; i < hsz; i++) {
            if(hpx == p0){
               if(hle) {
                  hpxstr = 0;
                  hle = 0;
               }
               else
                  hpxstr = pstr;
            }
            if(hpx == p1){
               if(hre) {
                  hpxstr = 0;
                  hre = 0;
               }
               else
                  hpxstr = nstr;
            }
            hpx += hpxstr;
            *hipass += *hpx * hi[i];
         }
         hipass += stride;

         for(i = 0; i < 2; i++) {
            if(lspx == p0){
               if(lle2) {
                  lspxstr = 0;
                  lle2 = 0;
               }
               else
                  lspxstr = pstr;
            }
            lspx += lspxstr;
            if(hspx == p0){
               if(hle2) {
                  hspxstr = 0;
                  hle2 = 0;
               }
               else
                  hspxstr = pstr;
            }
            hspx += hspxstr;
         }
      }
      if(da_ev) {
         lpxstr = lspxstr;
         lpx = lspx;
         lle = lle2;
         lre = lre2;
         *lopass = *lpx * lo[0];
         for(i = 1; i < lsz; i++) {
            if(lpx == p0){
               if(lle) {
                  lpxstr = 0;
                  lle = 0;
               }
               else
                  lpxstr = pstr;
            }
            if(lpx == p1){
               if(lre) {
                  lpxstr = 0;
                  lre = 0;
               }
               else
                  lpxstr = nstr;
            }
            lpx += lpxstr;
            *lopass += *lpx * lo[i];
         }
         lopass += stride;
      }
   }
   if(!fi_ev) {
      for(i = 0; i < hsz; i++)
         hi[i] *= -1.0;
   }
}

/************************************************************************/
/* WSQ reconstructs the image.  NOTE: this routine modifies and returns */
/* the results in "fdata".                                              */
/************************************************************************/
int wsq_reconstruct(float *fdata, const int width, const int height,
                  W_TREE w_tree[], const int w_treelen,
                  const DTT_TABLE *dtt_table)
{
   int num_pix, node;
   float *fdata1, *fdata_bse;

   if(dtt_table->lodef != 1) {
      fprintf(stderr,
      "ERROR: wsq_reconstruct : Lopass filter coefficients not defined\n");
      return(-95);
   }
   if(dtt_table->hidef != 1) {
      fprintf(stderr,
      "ERROR: wsq_reconstruct : Hipass filter coefficients not defined\n");
      return(-96);
   }

   num_pix = width * height;
   /* Allocate temporary floating point pixmap. */
   if((fdata1 = (float *) malloc(num_pix*sizeof(float))) == NULL) {
      fprintf(stderr,"ERROR : wsq_reconstruct : malloc : fdata1\n");
      return(-97);
   }

   /* Reconstruct floating point pixmap from wavelet subband data. */
   for (node = w_treelen - 1; node >= 0; node--) {
      fdata_bse = fdata + (w_tree[node].y * width) + w_tree[node].x;
      join_lets(fdata1, fdata_bse, w_tree[node].lenx, w_tree[node].leny,
                  1, width,
                  dtt_table->hifilt, dtt_table->hisz,
                  dtt_table->lofilt, dtt_table->losz,
                  w_tree[node].inv_cl);
      join_lets(fdata_bse, fdata1, w_tree[node].leny, w_tree[node].lenx,
                  width, 1,
                  dtt_table->hifilt, dtt_table->hisz,
                  dtt_table->lofilt, dtt_table->losz,
                  w_tree[node].inv_rw);
   }
   free(fdata1);

   return(0);
}

/****************************************************************/
void  join_lets(
   float *ne,    /* image pointers for creating subband splits */
   float *old,
   const int len1,       /* temporary length parameters */
   const int len2,
   const int pitch,      /* pitch gives next row_col to filter */
   const int  stride,    /*           stride gives next pixel to filter */
   float *hi,
   const int hsz,   /* NEW */
   float *lo,      /* filter coefficients */
   const int lsz,   /* NEW */
   const int inv)        /* spectral inversion? */
{
   float *lp0, *lp1;
   float *hp0, *hp1;
   float *lopass, *hipass;	/* lo/hi pass image pointers */
   float *limg, *himg;
   int pix, cl_rw;		/* pixel counter and column/row counter */
   int i, da_ev;			/* if "scanline" is even or odd and */
   int loc, hoc;
   int hlen, llen;
   int nstr, pstr;
   int tap;
   int fi_ev;
   int olle, ohle, olre, ohre;
   int lle, lle2, lre, lre2;
   int hle, hle2, hre, hre2;
   float *lpx, *lspx;
   int lpxstr, lspxstr;
   int lstap, lotap;
   float *hpx, *hspx;
   int hpxstr, hspxstr;
   int hstap, hotap;
   int asym, fhre = 0, ofhre;
   float ssfac, osfac, sfac;

   da_ev = len2 % 2;
   fi_ev = lsz % 2;
   pstr = stride;
   nstr = -pstr;
   if(da_ev) {
      llen = (len2+1)/2;
      hlen = llen - 1;
   }
   else {
      llen = len2/2;
      hlen = llen;
   }

   if(fi_ev) {
      asym = 0;
      ssfac = 1.0;
      ofhre = 0;
      loc = (lsz-1)/4;
      hoc = (hsz+1)/4 - 1;
      lotap = ((lsz-1)/2) % 2;
      hotap = ((hsz+1)/2) % 2;
      if(da_ev) {
         olle = 0;
         olre = 0;
         ohle = 1;
         ohre = 1;
      }
      else {
         olle = 0;
         olre = 1;
         ohle = 1;
         ohre = 0;
      }
   }
   else {
      asym = 1;
      ssfac = -1.0;
      ofhre = 2;
      loc = lsz/4 - 1;
      hoc = hsz/4 - 1;
      lotap = (lsz/2) % 2;
      hotap = (hsz/2) % 2;
      if(da_ev) {
         olle = 1;
         olre = 0;
         ohle = 1;
         ohre = 1;
      }
      else {
         olle = 1;
         olre = 1;
         ohle = 1;
         ohre = 1;
      }

      if(loc == -1) {
         loc = 0;
         olle = 0;
      }
      if(hoc == -1) {
         hoc = 0;
         ohle = 0;
      }

      for(i = 0; i < hsz; i++)
         hi[i] *= -1.0;
   }


   for (cl_rw = 0; cl_rw < len1; cl_rw++)  {
      limg = ne + cl_rw * pitch;
      himg = limg;
      *himg = 0.0;
      *(himg + stride) = 0.0;
      if(inv) {
         hipass = old + cl_rw * pitch;
         lopass = hipass + stride * hlen;
      }
      else {
         lopass = old + cl_rw * pitch;
         hipass = lopass + stride * llen;
      }


      lp0 = lopass;
      lp1 = lp0 + (llen-1) * stride;
      lspx = lp0 + (loc * stride);
      lspxstr = nstr;
      lstap = lotap;
      lle2 = olle;
      lre2 = olre;

      hp0 = hipass;
      hp1 = hp0 + (hlen-1) * stride;
      hspx = hp0 + (hoc * stride);
      hspxstr = nstr;
      hstap = hotap;
      hle2 = ohle;
      hre2 = ohre;
      osfac = ssfac;

      for(pix = 0; pix < hlen; pix++) {
         for(tap = lstap; tap >=0; tap--) {
            lle = lle2;
            lre = lre2;
            lpx = lspx;
            lpxstr = lspxstr;

            *limg = *lpx * lo[tap];
            for(i = tap+2; i < lsz; i += 2) {
               if(lpx == lp0){
                  if(lle) {
                     lpxstr = 0;
                     lle = 0;
                  }
                  else
                     lpxstr = pstr;
               }
               if(lpx == lp1) {
                  if(lre) {
                     lpxstr = 0;
                     lre = 0;
                  }
                  else
                     lpxstr = nstr;
               }
               lpx += lpxstr;
               *limg += *lpx * lo[i];
            }
            limg += stride;
         }
         if(lspx == lp0){
            if(lle2) {
               lspxstr = 0;
               lle2 = 0;
            }
            else
               lspxstr = pstr;
         }
         lspx += lspxstr;
         lstap = 1;

         for(tap = hstap; tap >=0; tap--) {
            hle = hle2;
            hre = hre2;
            hpx = hspx;
            hpxstr = hspxstr;
            fhre = ofhre;
            sfac = osfac;

            for(i = tap; i < hsz; i += 2) {
               if(hpx == hp0) {
                  if(hle) {
                     hpxstr = 0;
                     hle = 0;
                  }
                  else {
                     hpxstr = pstr;
                     sfac = 1.0;
                  }
               }
               if(hpx == hp1) {
                  if(hre) {
                     hpxstr = 0;
                     hre = 0;
                     if(asym && da_ev) {
                        hre = 1;
                        fhre--;
                        sfac = (float)fhre;
                        if(sfac == 0.0)
                           hre = 0;
                     }
                  }
                  else {
                     hpxstr = nstr;
                     if(asym)
                        sfac = -1.0;
                  }
               }
               *himg += *hpx * hi[i] * sfac;
               hpx += hpxstr;
            }
            himg += stride;
         }
         if(hspx == hp0) {
            if(hle2) {
               hspxstr = 0;
               hle2 = 0;
            }
            else {
               hspxstr = pstr;
               osfac = 1.0;
            }
         }
         hspx += hspxstr;
         hstap = 1;
      }


      if(da_ev)
         if(lotap)
            lstap = 1;
         else
            lstap = 0;
      else
         if(lotap)
            lstap = 2;
         else
            lstap = 1;

      for(tap = 1; tap >= lstap; tap--) {
         lle = lle2;
         lre = lre2;
         lpx = lspx;
         lpxstr = lspxstr;

         *limg = *lpx * lo[tap];
         for(i = tap+2; i < lsz; i += 2) {
            if(lpx == lp0){
               if(lle) {
                  lpxstr = 0;
                  lle = 0;
               }
               else
                  lpxstr = pstr;
            }
            if(lpx == lp1) {
               if(lre) {
                  lpxstr = 0;
                  lre = 0;
               }
               else
                  lpxstr = nstr;
            }
            lpx += lpxstr;
            *limg += *lpx * lo[i];
         }
         limg += stride;
      }


      if(da_ev) {
         if(hotap)
            hstap = 1;
         else
            hstap = 0;

         if(hsz == 2) {
            hspx -= hspxstr;
            fhre = 1;
         }
      }
      else
         if(hotap)
            hstap = 2;
         else
            hstap = 1;


      for(tap = 1; tap >= hstap; tap--) {
         hle = hle2;
         hre = hre2;
         hpx = hspx;
         hpxstr = hspxstr;
         sfac = osfac;
         if(hsz != 2)
            fhre = ofhre;

         for(i = tap; i < hsz; i += 2) {
            if(hpx == hp0) {
               if(hle) {
                  hpxstr = 0;
                  hle = 0;
               }
               else {
                  hpxstr = pstr;
                  sfac = 1.0;
               }
            }
            if(hpx == hp1) {
               if(hre) {
                  hpxstr = 0;
                  hre = 0;
                  if(asym && da_ev) {
                     hre = 1;
                     fhre--;
                     sfac = (float)fhre;
                     if(sfac == 0.0)
                        hre = 0;
                  }
               }
               else {
                  hpxstr = nstr;
                  if(asym)
                     sfac = -1.0;
               }
            }
            *himg += *hpx * hi[i] * sfac;
            hpx += hpxstr;
         }
         himg += stride;
      }
   }			

   if(!fi_ev)
      for(i = 0; i < hsz; i++)
         hi[i] *= -1.0;
}

/*****************************************************/
/* Routine to execute an integer  sign determination */
/*****************************************************/

int int_sign(
   const int power)  /* "sign" power */
{
   int cnt, num = -1;   /* counter and sign return value */

   if(power == 0)
      return 1;

   for(cnt = 1; cnt < power; cnt++)
      num *= -1;

   return num;
}

/*************************************************************/
/* Computes size of compressed image file including headers, */
/* tables, and parameters.                                   */
/*************************************************************/
int image_size(
   const int blocklen,  /* length of the compressed blocks */
   short *huffbits1,    /* huffman table parameters */
   short *huffbits2)
{
   int tot_size, cnt;

   tot_size = blocklen;       /* size of three compressed blocks */

   tot_size += 58;       /* size of transform table */
   tot_size += 389;       /* size of quantization table */
   tot_size += 17;       /* size of frame header */
   tot_size += 3;       /* size of block 1 */
   tot_size += 3;       /* size of block 2 */
   tot_size += 3;       /* size of block 3 */

   tot_size += 3;       /* size hufftable variable and hufftable number */
   tot_size += 16;       /* size of huffbits1 */
   for(cnt = 1; cnt < 16; cnt ++)
      tot_size += huffbits1[cnt];       /* size of huffvalues1 */

   tot_size += 3;       /* size hufftable variable and hufftable number */
   tot_size += 16;       /* size of huffbits1 */
   for(cnt = 1; cnt < 16; cnt ++)
      tot_size += huffbits2[cnt];       /* size of huffvalues2 */

   tot_size += 20;       /* SOI,SOF,SOB(3),DTT,DQT,DHT(2),EOI */
   return tot_size;
}

/*************************************************************/
/* Added by MDG on 02-24-05                                  */
/* Initializes memory used by the WSQ decoder.               */
/*************************************************************/
void init_wsq_decoder_resources()
{
   /* Added 02-24-05 by MDG                      */
   /* Init dymanically allocated members to NULL */
   /* for proper memory management in:           */
   /*    read_transform_table()                  */
   /*    getc_transform_table()                  */
   /*    free_wsq_resources()                    */
   dtt_table.lofilt = (float *)NULL;
   dtt_table.hifilt = (float *)NULL;
}

/*************************************************************/
/* Added by MDG on 02-24-05                                  */
/* Deallocates memory used by the WSQ decoder.               */
/*************************************************************/
void free_wsq_decoder_resources()
{
   if(dtt_table.lofilt != (float *)NULL){
      free(dtt_table.lofilt);
      dtt_table.lofilt = (float *)NULL;
   }

   if(dtt_table.hifilt != (float *)NULL){
      free(dtt_table.hifilt);
      dtt_table.hifilt = (float *)NULL;
   }
}

/************************************************************************
             
#cat: delete_comments_wsq - Deletes all comments in a WSQ compressed file.
      
*************************************************************************/
   
   
/*****************************************************************/
int delete_comments_wsq(unsigned char **ocdata, int *oclen,
         unsigned char *idata, const int ilen)
{     
   int ret, nlen, nalloc, stp;
   unsigned short marker, length;
   unsigned char m1, m2, *ndata, *cbufptr, *ebufptr;
   
   nalloc = ilen;
   /* Initialize current filled length to 0. */
   nlen = 0;
   
   /* Allocate new compressed byte stream. */
   if((ndata = (unsigned char *)malloc(nalloc * sizeof(unsigned char)))
             == (unsigned char *)NULL){
      fprintf(stderr, "ERROR : delete_comments_wsq : malloc : ndata\n");
      return(-2);
   }
   cbufptr = idata;
   ebufptr = idata + ilen;
         
   /* Parse SOI */
   if((ret = getc_marker_wsq(&marker, SOI_WSQ, &cbufptr, ebufptr))){
      free(ndata);
      return(ret);
   }     
            
   /* Copy SOI */
   if((ret = putc_ushort(marker, ndata, nalloc, &nlen))){
      free(ndata);
      return(ret);
   }
         
   /* Read Next Marker */
   if((ret = getc_marker_wsq(&marker, ANY_WSQ, &cbufptr, ebufptr))){
      free(ndata);
      return(ret);
   }        
   while(marker != EOI_WSQ) {
      if(marker != COM_WSQ) {
         /* Copy Marker and Data */
         if((ret = putc_ushort(marker, ndata, nalloc, &nlen))){
            free(ndata);
            return(ret);
         }     
         if((ret = getc_ushort(&length, &cbufptr, ebufptr))){
            free(ndata);
            return(ret);
         }     
/*                
         printf("Copying Marker %04X and Data (Length = %d)\n", marker, length);
*/
         if((ret = putc_ushort(length, ndata, nalloc, &nlen))){
            free(ndata);
            return(ret);
         }
         if((ret = putc_bytes(cbufptr, length-2, ndata, nalloc, &nlen))){
            free(ndata);
            return(ret);
         }
         cbufptr += (length-2);
         if(marker == SOB_WSQ) {
            stp = 0;
            while(!stp) {
               if((ret = getc_byte(&m1, &cbufptr, ebufptr))) {
                  free(ndata);
                  return(ret);
               }
               if(m1 == 0xFF) {
                  if((ret = getc_byte(&m2, &cbufptr, ebufptr))){
                     free(ndata);
                     return(ret);
                  }
                  if(m2 == 0x00) {
                     if((ret = putc_byte(m1, ndata, nalloc, &nlen))){
                        free(ndata);
                        return(ret);
                     }
                     if((ret = putc_byte(m2, ndata, nalloc, &nlen))){
                        free(ndata);
                        return(ret);
                     }
                  }
                  else {
                     cbufptr -= 2;
                     stp = 1;
                  }
               }
               else {
                  if((ret = putc_byte(m1, ndata, nalloc, &nlen))){
                     free(ndata);
                     return(ret);
                  }
               }
            }
         }
      }
      else {
         /* Don't Copy Comments. Print to stdout. */
         if((ret = getc_ushort(&length, &cbufptr, ebufptr))){
            free(ndata);
            return(ret);
         }
/*
         printf("COMMENT (Length %d):\n", length-2);
         for(i = 0; i < length-2; i++)
            printf("%c", *(cbufptr+i));
         printf("\n");
*/
         cbufptr += length-2;
      }
      if((ret = getc_marker_wsq(&marker, ANY_WSQ, &cbufptr, ebufptr))){
         free(ndata);
         return(ret);
      }
   }
   /* Copy EOI Marker */
   if((ret = putc_ushort(marker, ndata, nalloc, &nlen))){
      free(ndata);
      return(ret);
   }

   *ocdata = ndata;
   *oclen = nlen;

   return(0);
}
