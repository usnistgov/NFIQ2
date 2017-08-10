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


/* Source proted from Jaguar by Michael D. Garris onto Bell Box */
/* magi display.h */

#include <math.h>
#include <memory.h>

#ifndef TRUE
#define TRUE	1
#define FALSE	0
#endif

#define ON	TRUE
#define OFF	FALSE
#define START_X 0
#define START_Y 0
#define WIDTH	8.5
#define HEIGHT	11.0
#define BORDER_WIDTH 1
#define BYTE_SIZE 8.0
#define SCREEN_HEIGHT       900    /* sun moniter settings */
#define SCREEN_WIDTH       1152    /* sun moniter settings */
#define TOP_WINDOW_BORDER    29    /* sun moniter settings */
#define BOTTOM_WINDOW_BORDER 15    /* sun moniter settings */
#define SIDE_WINDOW_BORDER    7    /* sun moniter settings */
#define WD_SIZE	16.0
#define BPI	100
#define ORIG_BPI 300
#define PPI	83.0
#define word_align(_t) \
       (int)((double)ceil((double)((_t)/(float)WD_SIZE))*WD_SIZE) 
#define PIX_WIDTH(bpi) 	((int)(word_align(bpi * WIDTH)))
#define PIX_HEIGHT(bpi)	((int)(bpi * HEIGHT))
#define WORD_WIDTH(bpi)	((int)(PIX_WIDTH(bpi) / WD_SIZE))
#define BYTE_WIDTH(bpi)	((int)(PIX_WIDTH(bpi) / BYTE_SIZE))
#define FILE_SIZE(bpi)	(BYTE_WIDTH(bpi) * PIX_HEIGHT(bpi))
#define VertexAbsolute	0x0000
#define CONSOLE	""

#define XMOpenDisplay(display) \
   if((display = XOpenDisplay(CONSOLE)) == NULL) { \
      fprintf(stderr,"Unable to open display.\n"); \
      exit(-1); \
   }

#define XMWindowDefaults(display,rtwindow,screen,s_id,visual); \
{  screen = XDefaultScreenOfDisplay(display); \
   rtwindow = DefaultRootWindow(display); \
   s_id = DefaultScreen(display); \
   visual = XDefaultVisualOfScreen(screen); \
}

#define XMSetHints(hints,_x,_y,_w,_h,_flags); \
{  hints.x = _x; \
   hints.y = _y; \
   hints.width = _w; \
   hints.height = _h; \
   hints.flags = _flags; \
}

#define XMCreateSimpleWindow(display,rtwindow,s_id,hints) \
  XCreateSimpleWindow(display,rtwindow, \
                        hints.x,hints.y, \
                        hints.width,hints.height,BORDER_WIDTH, \
                        BlackPixel(display,s_id), \
                        WhitePixel(display,s_id))

#define XMLoadImageFromBitmapFile(display,visual,fname,bpi,image); \
{  FILE *_fp; \
   char *_data; \
   if((_fp = fopen(fname,"r")) == NULL){ \
      fprintf(stderr,"Unable to open file: %s\n",fname); \
      exit(-1); \
   } \
   _data = (char *)malloc(FILE_SIZE(bpi)); \
   fread(_data,sizeof(unsigned char), FILE_SIZE(bpi),_fp); \
   fclose(_fp); \
   image = XCreateImage(display,visual,1,XYBitmap,0,_data,PIX_WIDTH(bpi), \
                        PIX_HEIGHT(bpi),((int)WD_SIZE),BYTE_WIDTH(bpi)); \
}

#define XMDragRectangle(_disp,_win,_gc,_ox,_oy,_w,_h,_sim,_ix,_iy,_iw,_ih) \
{  int _x,_y,_rx,_ry,_rel = FALSE; \
   int _dx,_dy,_dw,_dh; \
   unsigned int _k; \
   Window _rptr,_cptr; \
   XEvent _event; \
   XQueryPointer(_disp,_win,&_rptr,&_cptr,&_rx,&_ry,&_ox,&_oy,&_k); \
   _x = _ox; _y = _oy; \
   while(!_rel){ \
      XNextEvent(_disp,&_event); \
      switch(_event.type){ \
         case ButtonRelease: \
            _rel = TRUE; \
         break; \
         case MotionNotify: \
            if((_x != _ox) || (_y != _oy)) \
               XPutImage(_disp,_win,_gc,_sim,0,0,_dx-1,_dy-1,_dw+2,_dh+2); \
            XQueryPointer(_disp,_win,&_rptr,&_cptr,&_rx,&_ry,&_x,&_y,&_k); \
            _w = _x - _ox; _h = _y - _oy; \
            _dx = (_w > 0) ? _ox : (_ox + _w -1); \
            _dy = (_h > 0) ? _oy : (_oy + _h -1); \
            _dw = abs(_w); \
            _dh = abs(_h); \
            _ix = _dx-1; _iy = _dy-1;_iw = _dw+2;_ih = _dh+2; \
            _sim = XGetImage(_disp,_win,_ix,_iy,_iw,_ih,1,XYPixmap); \
            XDrawRectangle(_disp,_win,_gc,_dx,_dy,_dw,_dh); \
            XSync(_disp,TRUE); \
         break; \
      } \
   } \
  _ox=_dx; \
  _oy=_dy; \
  _w=_dw; \
  _h=_dh; \
}

#define XMCreateImageFromHeader(_image,_disp,_vis,_data,_hdr) \
{ \
   int _d,_w,_h,_a,_u,_bpl; \
   sscanf(_hdr->depth,"%d",&_d); \
   sscanf(_hdr->width,"%d",&_w); \
   sscanf(_hdr->height,"%d",&_h); \
   sscanf(_hdr->align,"%d",&_a); \
   sscanf(_hdr->unitsize,"%d",&_u); \
   _bpl = (int)(_w/BYTE_SIZE); \
   _image = XCreateImage(_disp,_vis,_d,XYBitmap,0,_data,_w,_h,_a,_bpl); \
   _image->bitmap_unit = _u; \
   switch(_hdr->byte_order){ \
      case HILOW: \
           _image->byte_order = MSBFirst; \
           break; \
      case LOWHI: \
           _image->byte_order = LSBFirst; \
           break;\
   } \
}

#define XMCreateBellImage(_im,_disp,_vis,_data,_w,_h,_a) \
{ \
   _im=XCreateImage(_disp,_vis,1,XYBitmap,0,((char *)_data), \
              _w,_h,_a,((int)(_w/BYTE_SIZE))); \
   _im->byte_order=MSBFirst; \
   _im->bitmap_unit=(int) WD_SIZE; \
}


#define XMGetSubImageData(_sdata,_sx,_sy,_sw,_sh,_ddata,_dw,_dh) \
{ \
int _sxb,_dbw,_sbw,_dindex=0,_sindex,_i; \
    _sbw=(int)(_sw/BYTE_SIZE); \
    _dbw=(int)(_dw/BYTE_SIZE); \
    _sx=(int)(((int)(_sx/BYTE_SIZE))*BYTE_SIZE); \
    if(_sx<0) \
	_sx=0; \
    else{ \
	if(_sx > (_sw - _dw)) \
	   _sx = (_sw - _dw); \
    } \
    _sxb=(int)(_sx/BYTE_SIZE); \
    if(_sy<0) \
	_sy=0; \
    else{ \
	if(_sy > (_sh - _dh)) \
	   _sy = _sh - _dh; \
    } \
    _sindex=(_sy*_sbw)+_sxb; \
    for(_i=0;_i<_dh;_i++){ \
	memcpy(&(_ddata[_dindex]),&(_sdata[_sindex]),_dbw); \
	_dindex+=_dbw; \
	_sindex+=_sbw; \
    } \
}

#define XMImageScrollLoop(_dsp,_win,_gc,_im,_ww,_wh,_sdata,_sw,_sh,_rx,_ry,_ax,_ay) \
{ int _mx,_my,_rel = 0; \
  XEvent _event; \
  Window _rwin,_cwin; \
  unsigned int _k; \
  while(!_rel){ \
     XNextEvent(_dsp,&_event); \
     switch(_event.type){ \
        case ButtonPress: \
             switch(XMButton(_event)){ \
                case 1: \
                     XQueryPointer(_dsp,_win,&_rwin,&_cwin,&_mx,&_my, \
                                   &_rx,&_ry,&_k); \
                     _rx+=_ax; _ry+=_ay; \
                     XMGetSubImageData(_sdata,_rx,_ry,_sw,_sh, \
                                       _im->data,_ww,_wh); \
                     _ax=_rx; _ay=_ry; \
                     XClearWindow(_dsp,_win); \
                     XPutImage(_dsp,_win,_gc,_im,0,0,0,0,_ww,_wh); \
                break; \
                case 3: \
                     XQueryPointer(_dsp,_win,&_rwin,&_cwin, \
                                   &_mx,&_my,&_rx,&_ry,&_k); \
                     _rx+=(_ax-_ww); _ry+=(_ay-_wh); \
                     XMGetSubImageData(_sdata,_rx,_ry,_sw,_sh, \
                                       _im->data,_ww,_wh); \
                     _ax=_rx; _ay=_ry; \
                     XClearWindow(_dsp,_win); \
                     XPutImage(_dsp,_win,_gc,_im,0,0,0,0,_ww,_wh); \
                break; \
             } \
        break; \
        case KeyRelease: \
             _rel = 1; \
        break; \
     } \
  } \
}
