//
//  blobTracker.h
//  opencvExample
//
//  Created by Francois De Villiers on 2012/10/15.
//
//

#ifndef __opencvExample__blobTracker__
#define __opencvExample__blobTracker__

#include <stdio.h>

#include <iostream>
#include <vector>
#include <list>
#include <vector>

#include "ofxOpenCv.h"

#include "params.h"
#include "blob.h"

class BlobTracker
{
public:
    BlobTracker(){
        counter_id=0;
    };
	
public:
    void initialize();
    void merge_overlapped_blobs();
    void merge_blobs();
    void add_lost_blobs();
    void delete_lost_blobs();
    void draw_blobs(IplImage *img,list<blob> bl);
    void draw_mask();
    void update_backgroud();
    void image_difference(IplImage *img);
    void color_track(IplImage *img);
    void find_object_blob(IplImage *img);
    void find_blob(IplImage *img);
    void update_blob_list();
    void initialize(IplImage *img);
    list<blob> get_blob_list();
    IplImage *get_background();
    IplImage *get_mask();
    IplImage *get_foreground();
    int blob_dup(list<blob> ls,blob bl);
    void release();
    void TrExtract_SubImage(IplImage* srcimg, IplImage* dstimg, int xmin, int ymin, int xmax, int ymax);
private:
    list<blob> blob_list;
    list<blob> tmp_blob_list;
    list<blob> lost_blob_list;
    
    cv::Ptr<IplImage> grayImg;
    cv::Ptr<IplImage> grayDiff;
    cv::Ptr<IplImage> removeBackgrnd;
    cv::Ptr<IplImage> grayFrame;
    cv::Ptr<IplImage> cpyImg;
    cv::Ptr<IplImage> img_red;
    cv::Ptr<IplImage> img_green;
    cv::Ptr<IplImage> img_blue;
    cv::Ptr<IplImage> addedImg;
	
    CvMemStorage *storage;
    CvSeq *contours;
    CvSeq *oldContour;
    int counter_id;
	
public:
    
#ifdef OBJECT_TRACKER
    CascadeClassifier cascade;
#endif
	
#ifdef MOG2_SEGMENT
    BackgroundSubtractorMOG2 bg_model;
#endif
	
};


#endif /* defined(__opencvExample__blobTracker__) */
