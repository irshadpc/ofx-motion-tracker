//
//  blob.h
//  opencvExample
//
//  Created by Francois De Villiers on 2012/10/15.
//
//

#ifndef __opencvExample__blob__
#define __opencvExample__blob__

#include <stdio.h>
#include <iostream>
#include <vector>
#include <list>
#include <vector>

#include "ofxOpenCv.h"

#include "params.h"

using namespace std;

typedef struct _coord {
	int px;
	int py;
} coord;

//blob class
class blob
{
public:
	blob();
    
public:
	int min_x;
	int min_y;
	int max_x;
	int max_y;
	int id;
	int bar_x;
	int bar_y;
	int live_frame;
	int still_frame;
#ifdef HSTG
	CvHistogram *hst;
#endif
#ifdef HSTC
	CvHistogram *hstr;
	CvHistogram *hstg;
	CvHistogram *hstb;
#endif
	int area;
	list <coord> coords;
};


#endif /* defined(__opencvExample__blob__) */
