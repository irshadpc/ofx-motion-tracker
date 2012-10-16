//
//  blob.cpp
//  opencvExample
//
//  Created by Francois De Villiers on 2012/10/15.
//
//

#include "blob.h"


#include <stdio.h>
#include <iostream>
#include <vector>
#include <list>
#include <vector>

using namespace std;


blob::blob(){
    min_x=0;
    min_y=0;
    max_x=0;
    max_y=0;
    id=0;
    bar_x=0;
    bar_y=0;
    live_frame=0;
    still_frame=0;
    area=0;
    coords.clear();
}
/*
blob::~blob(){
#ifdef HSTG
	delete hst;
#endif
#ifdef HSTC
	delete hstr;
	delete hstg;
	delete hstb;
#endif

}*/
    
