/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2001             *
 * by the XIPHOPHORUS Company http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: predefined encoding modes; 44kHz stereo ~64kbps true VBR
 last mod: $Id: mode_44c_B.h,v 1.1 2001/08/13 07:43:15 xiphmont Exp $

 ********************************************************************/

#ifndef _V_MODES_44c_B_H_
#define _V_MODES_44c_B_H_

#include <stdio.h>
#include "vorbis/codec.h"
#include "backends.h"

#include "books/line_128x19_class1.vqh"
#include "books/line_128x19_class2.vqh"

#include "books/line_128x19_0sub0.vqh"
#include "books/line_128x19_1sub1.vqh"
#include "books/line_128x19_1sub2.vqh"
#include "books/line_128x19_1sub3.vqh"
#include "books/line_128x19_2sub1.vqh"
#include "books/line_128x19_2sub2.vqh"
#include "books/line_128x19_2sub3.vqh"

#include "books/line_1024x31_class0.vqh"
#include "books/line_1024x31_class1.vqh"
#include "books/line_1024x31_class2.vqh"
#include "books/line_1024x31_class3.vqh"

#include "books/line_1024x31_0sub0.vqh"
#include "books/line_1024x31_0sub1.vqh"
#include "books/line_1024x31_1sub0.vqh"
#include "books/line_1024x31_1sub1.vqh"
#include "books/line_1024x31_2sub1.vqh"
#include "books/line_1024x31_2sub2.vqh"
#include "books/line_1024x31_2sub3.vqh"
#include "books/line_1024x31_3sub1.vqh"
#include "books/line_1024x31_3sub2.vqh"
#include "books/line_1024x31_3sub3.vqh"

#include "books/res_44c_B_128aux.vqh"
#include "books/res_44c_B_1024aux.vqh"

#include "books/res_Bc_1.vqh"
#include "books/res_Bc_2.vqh"
#include "books/res_Bc_3.vqh"
#include "books/res_Bc_4.vqh"
#include "books/res_Bc_5.vqh"
#include "books/res_Bc_6.vqh"
#include "books/res_Bc_7.vqh"
#include "books/res_Bc_7a.vqh"
#include "books/res_Bc_8.vqh"
#include "books/res_Bc_8a.vqh"
#include "books/res_Bc_9.vqh"
#include "books/res_Bc_9a.vqh"
#include "books/res_Bc_9b.vqh"

#include "maskadj_A.h"

static vorbis_info_psy_global _psy_set_44c_BG={
  0, /* decaydBpms */
  8,   /* lines per eighth octave */
  
  /* thresh sample period, preecho clamp trigger threshhold, range, minenergy */
  256, {26.f,26.f,26.f,30.f}, {-90.f,-90.f,-90.f,-90.f}, -90.f,
  -6.f, 
  
  0,
};

static struct vp_couple_pass _psy_pass_44c_B0[]={
  {1.f,1.f,
    {{24,     0,0,       0,0,      0,0},
     {64,    4.5,0,      0,12,      0,0},
     {9999,  7.5,0,     4.5,12,   1.5,0}}
  },
};

static vp_couple_pass _psy_pass_44c_B[]={
  {1.f,1.f,
    {{288,   0,0,       0,0,      0,0},
     {512,  4.5,0,      0,12,     0,0},
     {9999, 7.5,0,     4.5,12,  1.5,0}}
  }
};

static vorbis_info_psy _psy_set_44c_B0={
  ATH_Bark_dB_lineaggressive,
  -100.,-140.,

  /* tonemaskp */
  -3.f, -40.f,&_vp_tonemask_consbass_A,
  /* peakattp, curvelimitp */
  1, 4, &_vp_peakatt_B,

  /*noisemaskp */
  1,-30.f,     /* suppress any noise curve over maxspec+n */
  .6f, .6f,   /* low/high window */
  5, 5, 10,

  /*63     125     250     500      1k      2k      4k       8k     16k*/
  {-30,-30,-30,-30,-30,-30,-20,-10,  0,  0,  0,  0,  0,  1,  1,  2,  3},
  {.6f,.6f,.6f,.6f,.6f,.5f,.5f,.5f,.5f,.5f,.5f,.5f,.6f,.75f,.85f,.85f,.85f},


  105.f,  /* even decade + 5 is important; saves an rint() later in a
            tight loop) */
  1,
  _psy_pass_44c_B0
};

static vorbis_info_psy _psy_set_44c_BT={
  ATH_Bark_dB_lineaggressive,
  -100.f,-140.f,

  /* tonemask */
  -3.f,-40.f,&_vp_tonemask_consbass_A,
  /* peakattp,curvelimitp */
  1, 4,  &_vp_peakatt_B,

  /*noisemaskp */
  1,  -30.f,     /* suppress any noise curve over maxspec+n */
      .4f,.4f,   /* low/high window */
      10,10,100,

  /*63     125     250     500      1k      2k      4k       8k     16k*/
  {-30,-30,-30,-30,-30,-30,-30,-20, -6, -6, -6, -6,  0,   0,  0,  1,  2},
  {.6f,.6f,.6f,.6f,.6f,.6f,.5f,.5f,.5f,.5f,.5f,.5f,.5f,.6f,.7f,.75f,.8f},

  105.f,  /* even decade + 5 is important; saves an rint() later in a
            tight loop) */
  1,_psy_pass_44c_B
};

static vorbis_info_psy _psy_set_44c_B={
  ATH_Bark_dB_lineaggressive,
  -100.f,  -140.f,

  /* tonemask */
  -3.f,-40.f,&_vp_tonemask_consbass_A,
  /* peakattp, curvelimitp */
  1, 4, &_vp_peakatt_B,

  /*noisemaskp */
  1,  -30.f,     /* suppress any noise curve over maxspec+n */
      .4f,.4f,   /* low/high window */
      10,10,100,

  /*63     125     250     500      1k      2k      4k       8k     16k*/
  {-20,-20,-20,-20,-20,-20,-20,  0, -6, -6, -6, -6,  0,   0,  0,  1,  2},
  {.6f,.6f,.6f,.6f,.6f,.6f,.5f,.5f,.5f,.5f,.5f,.5f,.5f,.6f,.7f,.75f,.8f},


  105.f,  /* even decade + 5 is important; saves an rint() later in a
            tight loop) */
  1,_psy_pass_44c_B
};

static vorbis_info_time0 _time_set_44c_B={0};

static vorbis_info_floor1 _floor_set_44c_B0={
  6,
  {0,1,1,1,2,2},
  
  {4,3,3},
  {0,2,2},
  {-1,0,1},
  {{2},{-1,3,4,5},{-1,6,7,8}},
  
  4,
  
  {0,128,  
   
   6,17,30,58,
   
   2,1,4, 11,8,14, 23,20,26,
   41,35,48, 84,69,103},
  
  60,30,500,
  999,999,1,18.,
  8,70,
  112};


static vorbis_info_floor1 _floor_set_44c_B={
  10,
  {0,1,2,2,2,2,2, 3,3,3},
  
  {3,4,3,3},
  {1,1,2,2},
  {9,10,11,12},
  {{13,14},
   {15,16},
   {-1,17,18,19},
   {-1,20,21,22},
  },
  
  4,
  {0,1024,
   
   88,31,243,
   
   14,54,143,460,
   
   6,3,10, 22,18,26, 41,36,47, 
   69,61,78, 112,99,126, 185,162,211,  
   329,282,387, 672,553,825
  },
  
  60,30,400,
  20,8,1,18.,
  20,600,
  896};

static vorbis_info_residue0 _residue_set_44c_B0={
  0,224,16,10,23,
  {0,1,1,1,1,1,1,3,3,7},
  {25,
   26,
   27,
   28,
   29,
   30,
   31,32,
   33,34,
   35,36,37},
  {9999,
   7,9999,9999,
   11,9999,
   9999,9999,9999,9999},
  {.5f,
   1.5f,1.5f,1.5f,
   2.5f,2.5f,
   4.5f,16.5f,84.5f},
  {0},
  {99,
   8,8,99,
   99,99,
   99,99,99,99},
  {3}};

static vorbis_info_residue0 _residue_set_44c_B={
  0,1792, 32,10,24,
  {0,1,1,1,1,1,1,3,3,7},
  {25,
   26,
   27,
   28,
   29,
   30,
   31,32,
   33,34,
   35,36,37},
  {9999,
   10,9999,9999,
   20,9999,
   9999,9999,9999,9999},
  {.5f,
   1.5f,1.5f,1.5f,
   2.5f,2.5f,
   4.5f,16.5f,84.f},
  {0},
  {99,
   32,32,99,
   99,99,
   99,99,99,99},
  {3}};

static vorbis_info_mapping0 _mapping_set_44c_B0={
  1, {0,0}, {0}, {0}, {0}, {0,0}, 1,{0},{1}};
static vorbis_info_mapping0 _mapping_set_44c_B={
  1, {0,0}, {0}, {1}, {1}, {1,2}, 1,{0},{1}};

static vorbis_info_mode _mode_set_44c_B0={0,0,0,0};
static vorbis_info_mode _mode_set_44c_B={1,0,0,1};

codec_setup_info info_44c_B={

  /* smallblock, largeblock */
  {256, 2048}, 
  /* modes,maps,times,floors,residues,books,psys */
  2,          2,    1,     2,       2,   38,   3,
  /* modes */
  {&_mode_set_44c_B0,&_mode_set_44c_B},
  /* maps */
  {0,0},{&_mapping_set_44c_B0,&_mapping_set_44c_B},
  /* times */
  {0,0},{&_time_set_44c_B},
  /* floors */
  {1,1},{&_floor_set_44c_B0,&_floor_set_44c_B},
  /* residue */
  {2,2},{&_residue_set_44c_B0,&_residue_set_44c_B},
  /* books */
  
  {  
    &_huff_book_line_128x19_class1,
    &_huff_book_line_128x19_class2, /* 1 */
   
    &_huff_book_line_128x19_0sub0,  /* 2 */
    &_huff_book_line_128x19_1sub1,  /* 3 */
    &_huff_book_line_128x19_1sub2,
    &_huff_book_line_128x19_1sub3,  /* 5 */
    &_huff_book_line_128x19_2sub1,
    &_huff_book_line_128x19_2sub2,  /* 7 */
    &_huff_book_line_128x19_2sub3, 
    
    &_huff_book_line_1024x31_class0,
    &_huff_book_line_1024x31_class1, /* 10 */
    &_huff_book_line_1024x31_class2,
    &_huff_book_line_1024x31_class3, /* 12 */
    
    &_huff_book_line_1024x31_0sub0,
    &_huff_book_line_1024x31_0sub1, /* 14 */
    &_huff_book_line_1024x31_1sub0, 
    &_huff_book_line_1024x31_1sub1,
    &_huff_book_line_1024x31_2sub1,  
    &_huff_book_line_1024x31_2sub2, /* 18 */
    &_huff_book_line_1024x31_2sub3, 
    &_huff_book_line_1024x31_3sub1,
    &_huff_book_line_1024x31_3sub2,
    &_huff_book_line_1024x31_3sub3, /* 22 */

    &_huff_book_res_44c_B_128aux, 
    &_huff_book_res_44c_B_1024aux,
    
    &_vq_book_res_Bc_1,
    &_vq_book_res_Bc_2,
    &_vq_book_res_Bc_3,
    &_vq_book_res_Bc_4,
    &_vq_book_res_Bc_5,
    &_vq_book_res_Bc_6,
    &_vq_book_res_Bc_7,
    &_vq_book_res_Bc_7a,
    &_vq_book_res_Bc_8,
    &_vq_book_res_Bc_8a,
    &_vq_book_res_Bc_9,
    &_vq_book_res_Bc_9a,
    &_vq_book_res_Bc_9b,


  },
  /* psy */
  {&_psy_set_44c_B0,&_psy_set_44c_BT,&_psy_set_44c_B},
  &_psy_set_44c_BG
};

#endif
