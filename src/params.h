//
//  params.h
//  opencvExample
//
//  Created by Francois De Villiers on 2012/10/15.
//
//

#ifndef opencvExample_params_h
#define opencvExample_params_h

//params
#define FAST_UPDATE 0.8
#define SLOW_UPDATE 0.999999
#define TH 14
#define DISTANCE 2.0
#define STILL_FRAME 25
#define MERGED_BLOB_DISTANCE 50.0
#define MIN_CONTOUR_SIZE 100

#define DEXMAC_SEGMENT
//#define MOG2_SEGMENT
#define WIN_SEGMENT_DEBUG

#define IMAGE_DIFFERENCE
//#define COLOR_TRACKER
//#define OBJECT_TRACKER

//#define VERBOSE
//#define VERBOSE_MERGE

#define HSTG
//#define HSTC

#define DRAWING_TRACE

//#define RED_TRACK
//#define GREEN_TRACK
#define BLUE_TRACK
//#define YELLOW_TRACK

#ifdef RED_TRACK
#define MINB 0
#define MAXB 0
#define MING 0
#define MAXG 0
#define MINR 180
#define MAXR 200
#endif

#ifdef GREEN_TRACK
#define MINB 0
#define MAXB 0
#define MING 220
#define MAXG 240
#define MINR 0
#define MAXR 0
#endif

#ifdef BLUE_TRACK
#define MINB 200
#define MAXB 220
#define MING 0
#define MAXG 0
#define MINR 0
#define MAXR 0
#endif

#ifdef YELLOW_TRACK
#define MINB 0
#define MAXB 0
#define MING 220
#define MAXG 240
#define MINR 180
#define MAXR 200
#endif

//macros
#define MAX_VAL(a,b) a>b ? a : b;
#define MIN_VAL(a,b) a<b ? a : b;

#endif
