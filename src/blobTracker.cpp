//
//  blobTracker.cpp
//  opencvExample
//
//  Created by Francois De Villiers on 2012/10/15.
//
//

#include "blobTracker.h"

using namespace std;

//String cascadeName = "../../data/haarcascades/haarcascade_frontalface_alt.xml";
//String nestedCascadeName = "../../data/haarcascades/haarcascade_eye_tree_eyeglasses.xml";


//check for blob duplication
int BlobTracker::blob_dup(list<blob> a,blob bl)
{
	list<blob>::iterator it=a.begin();
    
	unsigned int counter_tmp=0;
	int find_blob=0;
	
	while(counter_tmp<a.size())
	{
		blob tb=*it;
		if(tb.id==bl.id)
		{
			find_blob=1;
			break;
		}
		it++;
		counter_tmp++;
	}
	
	return find_blob;
}
//merge overlapped blobs
void BlobTracker::merge_overlapped_blobs()
{
	if(tmp_blob_list.size()>1)//at least 2 elements
	{
        
#ifdef VERBOSE_MERGE
		printf("Merging OB\n");
#endif
		
		unsigned int counter_loop=0;
		unsigned int blob_list_size=tmp_blob_list.size();
		
		while(counter_loop<blob_list_size)
		{
			int overlapped=0;
			unsigned int counter_blob=0;
			unsigned int overlap_blob=0;
			
			list<blob>::iterator iter_blob=tmp_blob_list.begin();
			list<blob>::iterator ib=tmp_blob_list.begin();
			
			list<blob> a;
			a.clear();
            
			blob tmp_blob=*iter_blob;
			ib++;
			
			while(counter_blob<(tmp_blob_list.size()-1))
			{
				blob bl=*ib;
				
				blob nb=bl;
				
				overlapped=1;
				
				if(bl.min_x>=tmp_blob.max_x)
					overlapped=0;
				
				if(tmp_blob.min_x>=bl.max_x)
					overlapped=0;
				
				if(bl.min_y>=tmp_blob.max_y)
					overlapped=0;
				
				if(tmp_blob.min_y>=bl.max_y)
					overlapped=0;
#ifdef VERBOSE_MERGE
				printf("((%d,%d,%d)-->",tmp_blob.id,bl.id,overlapped);
#endif
				if(overlapped==1)
				{
					nb.min_x=MIN_VAL(bl.min_x,tmp_blob.min_x);
					nb.min_y=MIN_VAL(bl.min_y,tmp_blob.min_y);
					nb.max_x=MAX_VAL(bl.max_x,tmp_blob.max_x);
					nb.max_y=MAX_VAL(bl.max_y,tmp_blob.max_y);
					
					//nb.live_frame=MAX_VAL(bl.live_frame,tmp_blob.live_frame);
					//nb.still_frame=MAX_VAL(bl.still_frame,tmp_blob.still_frame);
					
					//new id
					if(bl.area>tmp_blob.area)
					{
						nb.id=bl.id;
						nb.live_frame=bl.live_frame;
						nb.still_frame=0;
#ifdef HSTG
						nb.hst=bl.hst;
#endif
						
#ifdef HSTC
						nb.hstr=bl.hstr;
						nb.hstg=bl.hstg;
						nb.hstb=bl.hstb;
#endif
						nb.coords=bl.coords;
					}
					else
					{
						nb.id=tmp_blob.id;
						nb.live_frame=tmp_blob.live_frame;
						nb.still_frame=0;
#ifdef HSTG
						nb.hst=tmp_blob.hst;
#endif
						
#ifdef HSTC
						nb.hstr=tmp_blob.hstr;
						nb.hstg=tmp_blob.hstg;
						nb.hstb=tmp_blob.hstb;
#endif
						nb.coords=tmp_blob.coords;
					}
					
					nb.area=(nb.max_x-nb.min_x)*(nb.max_y-nb.min_y);
					
					//new gowed blob in list
#ifdef VERBOSE_MERGE
					printf("M %d)",nb.id);
#endif
					if(blob_dup(a,nb)==0)
						a.push_back(nb);
					
					overlap_blob=1;
				}
				else
				{
					//not overlapped blob in list
#ifdef VERBOSE_MERGE
					printf("%d)",bl.id);
#endif
					if(blob_dup(a,bl)==0)
						a.push_back(bl);
				}
				
				ib++;
				
				counter_blob++;
			}
            
			if(overlap_blob==0)
			{
				if(blob_dup(a,tmp_blob)==0)
					a.push_back(tmp_blob);
				
				tmp_blob_list.clear();
				tmp_blob_list=a;
			}
			else
			{
				//add not merged to lost blobs
				if(blob_dup(a,tmp_blob)==0)
					lost_blob_list.push_back(tmp_blob);
			}
			
			tmp_blob_list.clear();
			
			tmp_blob_list=a;
			
			counter_loop++;
			
#ifdef VERBOSE_MERGE
			printf("\n");
#endif
		}
	}
}
//merge overlapped blobs
void BlobTracker::merge_blobs()
{
	if(tmp_blob_list.size()>1)//at least 2 elements
	{
#ifdef VERBOSE_MERGE
		printf("Merging DB\n");
#endif
		unsigned int counter_loop=0;
		unsigned int blob_list_size=tmp_blob_list.size();
		
		while(counter_loop<blob_list_size)
		{
			unsigned int counter_blob=0;
			unsigned int overlap_blob=0;
			
			list<blob>::iterator iter_blob=tmp_blob_list.begin();
			list<blob>::iterator ib=tmp_blob_list.begin();
			
			list<blob> a;
			a.clear();
            
			blob tmp_blob=*iter_blob;
			ib++;
			
			while(counter_blob<(tmp_blob_list.size()-1))
			{
				blob bl=*ib;
				
				blob nb=bl;
				
				int bl_bx=(bl.min_x+tmp_blob.max_x)/2;
				int bl_by=(bl.min_y+tmp_blob.max_y)/2;
				
				int tmp_blob_bx=(tmp_blob.min_x+tmp_blob.max_x)/2;
				int tmp_blob_by=(tmp_blob.min_y+tmp_blob.max_y)/2;
				
				double dist=sqrt((double)((bl_bx-tmp_blob_bx)*(bl_bx-tmp_blob_bx))+
                                 (double)((bl_by-tmp_blob_by)*(bl_by-tmp_blob_by)));
                
#ifdef VERBOSE_MERGE
				printf("((%d,%d,%f)-->",tmp_blob.id,bl.id,dist);
#endif
				
				if(dist<MERGED_BLOB_DISTANCE)
				{
					nb.min_x=MIN_VAL(bl.min_x,tmp_blob.min_x);
					nb.min_y=MIN_VAL(bl.min_y,tmp_blob.min_y);
					nb.max_x=MAX_VAL(bl.max_x,tmp_blob.max_x);
					nb.max_y=MAX_VAL(bl.max_y,tmp_blob.max_y);
					
					//nb.live_frame=MAX_VAL(bl.live_frame,tmp_blob.live_frame);
					//nb.still_frame=MAX_VAL(bl.still_frame,tmp_blob.still_frame);
					
					//new id
					if(bl.area>tmp_blob.area)
					{
						nb.id=bl.id;
						nb.live_frame=bl.live_frame;
						nb.still_frame=0;
#ifdef HSTG
						nb.hst=bl.hst;
#endif
						
#ifdef HSTC
						nb.hstr=bl.hstr;
						nb.hstg=bl.hstg;
						nb.hstb=bl.hstb;
#endif
						nb.coords=bl.coords;
					}
					else
					{
						nb.id=tmp_blob.id;
						nb.live_frame=tmp_blob.live_frame;
						nb.still_frame=0;
#ifdef HSTG
						nb.hst=tmp_blob.hst;
#endif
						
#ifdef HSTC
						nb.hstr=tmp_blob.hstr;
						nb.hstg=tmp_blob.hstg;
						nb.hstb=tmp_blob.hstb;
#endif
						
						nb.coords=tmp_blob.coords;
					}
					
					nb.area=(nb.max_x-nb.min_x)*(nb.max_y-nb.min_y);
					
					//new gowed blob in list
#ifdef VERBOSE_MERGE
					printf("M %d)",nb.id);
#endif
					if(blob_dup(a,nb)==0)
						a.push_back(nb);
					
					overlap_blob=1;
				}
				else
				{
					//not distance merged blob in list
#ifdef VERBOSE_MERGE
					printf("%d)",bl.id);
#endif
					
					if(blob_dup(a,bl)==0)
						a.push_back(bl);
				}
				
				ib++;
				
				counter_blob++;
			}
            
			if(overlap_blob==0)
			{
				if(blob_dup(a,tmp_blob)==0)
					a.push_back(tmp_blob);
				
				tmp_blob_list.clear();
				tmp_blob_list=a;
			}
			else
			{
				//add not merged to lost blobs
				if(blob_dup(a,tmp_blob)==0)
					lost_blob_list.push_back(tmp_blob);
			}
			
			tmp_blob_list.clear();
			
			tmp_blob_list=a;
			
			counter_loop++;
#ifdef VERBOSE_MERGE
			printf("\n");
#endif
		}
	}
}
//add lost blob
void BlobTracker::add_lost_blobs()
{
	unsigned int counter_iter=0;
	list<blob>::iterator iter_blob;
	
	if(blob_list.size()!=0)
	{
		list<blob> a;
		a.clear();
        
		//insert old lost blob
		iter_blob=lost_blob_list.begin();
		counter_iter=0;
		
		while(counter_iter<lost_blob_list.size())
		{
			blob tmp_blob=*iter_blob;
            
			if(blob_dup(a,tmp_blob)==0)
			{
				a.push_back(tmp_blob);
			}
			
			counter_iter++;
			iter_blob++;
		}
		
		//insert new lost blob
		iter_blob=blob_list.begin();
		counter_iter=0;
		
		while(counter_iter<blob_list.size())
		{
			blob tmp_blob=*iter_blob;
            
			if(blob_dup(a,tmp_blob)==0)
			{
				a.push_back(tmp_blob);
			}
			
			counter_iter++;
			iter_blob++;
		}
		
		lost_blob_list.clear();
		
		lost_blob_list=a;
	}
}
//delete lost blob
void BlobTracker::delete_lost_blobs()
{
	unsigned int counter_iter=0;
	
	list<blob> a;
	a.clear();
	
	list<blob>::iterator iter_blob=lost_blob_list.begin();
    
#ifdef VERBOSE
	if(lost_blob_list.size()!=0)
	{
		printf("Removing: ");
	}
#endif
	
	//if old lost blob, remove it
	while(counter_iter<lost_blob_list.size())
	{
		blob tmp_blob=*iter_blob;
        
#ifdef VERBOSE
		printf("LB(%d,%d)",tmp_blob.id,tmp_blob.live_frame);
#endif
		
		if(tmp_blob.live_frame>0)
		{
			//not removed
			tmp_blob.live_frame--;
			
			if(blob_dup(a,tmp_blob)==0)
			{
				a.push_back(tmp_blob);
			}
#ifdef VERBOSE
			printf("NR ");
#endif
		}
		else
		{
			//removed
#ifdef VERBOSE
			printf("R ");
#endif
		}
		
		counter_iter++;
		iter_blob++;
	}
	
#ifdef VERBOSE
	if(lost_blob_list.size()!=0)
		printf("\n");
#endif
	
	lost_blob_list.clear();
	
	lost_blob_list=a;
}
//draw blob
void BlobTracker::draw_blobs(IplImage *img,list<blob> bl)
{
	//show blob list
	list<blob>::iterator iter;
	iter = bl.begin();
	unsigned int counter_iter=0;
	while(counter_iter<bl.size())
	{
		blob tmp_blob=*iter;
		CvFont font;
		
		cvDrawRect(img,cvPoint(tmp_blob.min_x,tmp_blob.min_y),
                   cvPoint(tmp_blob.max_x,tmp_blob.max_y),CV_RGB(0,255,0),1,CV_AA,0);
		
		cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC,1.0,1.0,0,1);
		
		char id_val[256];
		sprintf(id_val,"%d,%d,%d",tmp_blob.id,tmp_blob.live_frame,tmp_blob.still_frame);
		
		cvPutText(img,id_val,cvPoint(tmp_blob.bar_x,tmp_blob.bar_y),
                  &font,cvScalar(255,255,0));
		
		//cvCircle(img,cvPoint(tmp_blob.bar_x,tmp_blob.bar_y),10,cvScalar(0,0,255,255),-1);
#ifdef DRAWING_TRACE
		list<coord>::iterator coords;
        
		coords=tmp_blob.coords.begin();
        
		int counter_coords=1;
        
		coord old_crd=*coords;
        
		coords++;
		
		while(counter_coords<(int)(tmp_blob.coords.size()-1))
		{
			coord crd_pb = *coords;
			coords++;
			counter_coords++;
            
			coord crd_pc = *coords;
			coords++;
			counter_coords++;
            
			if(crd_pb.px!=0 && crd_pb.py!=0 && crd_pc.px!=0 && crd_pc.py!=0 && old_crd.px!=0 && old_crd.py!=0)
			{
				cvCircle(img,cvPoint(crd_pb.px,crd_pb.py),10,cvScalar(255,0,0,255),-1);
				cvCircle(img,cvPoint(crd_pc.px,crd_pc.py),10,cvScalar(255,0,0,255),-1);
                
				cvLine(img,cvPoint(old_crd.px,old_crd.py),cvPoint(crd_pb.px,crd_pb.py),cvScalar(0,255,0,255),1);
				cvLine(img,cvPoint(crd_pb.px,crd_pb.py),cvPoint(crd_pc.px,crd_pc.py),cvScalar(0,255,0,255),1);
			}
            
			old_crd=crd_pc;
		}
#endif
		iter++;
		counter_iter++;
	}
    
#ifdef VERBOSE
	if(counter_iter!=0)
		printf("\n");
#endif
	
}


//draw mask
void BlobTracker::draw_mask()
{
#ifdef DEXMAC_SEGMENT
	list<blob>::iterator iter;
	iter = blob_list.begin();
	
	unsigned int counter_elem=0;
	while(counter_elem<blob_list.size())
	{
		int counter=0;
		
		blob tmp_blob = *iter;
		
		int min_x=tmp_blob.min_x;
		int min_y=tmp_blob.min_y;
		int max_x=tmp_blob.max_x;
		int max_y=tmp_blob.max_y;
        
		for(int i=min_y-25;i<max_y+25;i++)
		{
			for(int j=min_x-25;j<max_x+25;j++)
			{
				counter=i*removeBackgrnd->width+j;
				
				if((counter>0 && counter<(removeBackgrnd->width*removeBackgrnd->height)))
				{
					if(tmp_blob.still_frame<STILL_FRAME)//frame timeout
					{
						removeBackgrnd->imageData[counter]=255;//slow marker
					}
					else
					{
						removeBackgrnd->imageData[counter]=128;//fast marker (no object, release it)
					}
				}
			}
		}
        
		counter_elem++;
		iter++;
	}
#endif
}


//update background
void BlobTracker::update_backgroud()
{
#ifdef DEXMAC_SEGMENT
	double a=FAST_UPDATE;
	double b=SLOW_UPDATE;
	
	unsigned char *bk=new unsigned char [grayImg->height*grayImg->width];
	unsigned char *act=new unsigned char[grayFrame->height*grayFrame->width];
	
	memcpy(bk,grayImg->imageData,grayImg->height*grayImg->width);
	memcpy(act,grayFrame->imageData,grayFrame->height*grayImg->width);
	
	int counter=0;
	for(int i=0;i<grayImg->height;i++)
	{
		for(int j=0;j<grayImg->width;j++)
		{
			if(removeBackgrnd->imageData[counter]==-1)
			{
				//slow update
				bk[counter]=(unsigned char)(((double)act[counter] -
                                             b*(double)(act[counter]-bk[counter])+0.5));
                
			}
			else
			{
				//fast update
				bk[counter]=(unsigned char)(((double)act[counter] -
                                             a*(double)(act[counter]-bk[counter])+0.5));
			}
			
			counter++;
		}
	}
	
	memcpy(grayImg->imageData,bk,grayFrame->width*grayFrame->height);
	
	delete []act;
	delete []bk;
#endif
	
}


//image difference
void BlobTracker::image_difference(IplImage *img)
{
	//Color to gray
	cvCvtColor( img , grayFrame , CV_BGR2GRAY );
    
#ifdef DEXMAC_SEGMENT
	//Frame difference (actual and dynamic backgroud)
	cvAbsDiff( grayFrame , grayImg , grayDiff );
#endif
	
#ifdef MOG2_SEGMENT
	Mat img_gray(grayFrame);
	Mat bgimg(grayImg);
	Mat fgimg(grayDiff);
	Mat fgmask;
	
	fgimg=Scalar::all(0);
	
	bg_model(img_gray,fgmask,-1);
	img_gray.copyTo(fgimg,fgmask);
	
	bg_model.getBackgroundImage(bgimg);
    
#endif
	
	//Thresholding
	cvThreshold(grayDiff,grayDiff,TH,255,CV_THRESH_BINARY);
	
#ifdef WIN_SEGMENT_DEBUG
	//cvShowImage("grayDiff",grayDiff);
#endif
}
//color track (sperimental)
void BlobTracker::color_track(IplImage *img)
{
	cvZero(img_red);
	cvZero(img_green);
	cvZero(img_blue);
	cvZero(grayImg);
	cvZero(grayDiff);
	
	unsigned char *vid_data=new unsigned char[img->width*img->height*3];
	
	unsigned char *rdata=new unsigned char[img->width*img->height];
	unsigned char *gdata=new unsigned char[img->width*img->height];
	unsigned char *bdata=new unsigned char[img->width*img->height];
	unsigned char *gray=new unsigned char[img->width*img->height];
	
	unsigned char r,g,b;
	
	memcpy(vid_data,img->imageData,img->width*img->height*3);
	
	int counter=0;
	for(int i=0;i<(img->width*img->height);i++)
	{
		b=vid_data[counter];
		counter++;
		
		g=vid_data[counter];
		counter++;
		
		r=vid_data[counter];
		counter++;
		
		bdata[i]=b;
		gdata[i]=g;
		rdata[i]=r;
		
		gray[i]=(0.1*(double)r+0.6*(double)g+0.3*(double)b);
	}
	
	memcpy(img_blue->imageData,bdata,img->width*img->height);
	memcpy(img_green->imageData,gdata,img->width*img->height);
	memcpy(img_red->imageData,rdata,img->width*img->height);
	memcpy(grayFrame->imageData,gray,img->width*img->height);
	
	delete []vid_data;
	delete []rdata;
	delete []gdata;
	delete []bdata;
	delete []gray;
	
	//cvSplit(img,img_blue,img_green,img_red,NULL);
	//cvCvtColor(img,grayFrame,CV_BGR2GRAY);
	
	cvAbsDiff(img_blue,grayFrame,img_blue);
	cvAbsDiff(img_green,grayFrame,img_green);
	cvAbsDiff(img_red,grayFrame,img_red);
	
	//cvShowImage("green",img_green);
	//cvShowImage("blue",img_blue);
	//cvShowImage("red",img_red);
	
	cvNot(img_blue,img_blue);
	cvNot(img_green,img_green);
	cvNot(img_red,img_red);
	
	cvInRangeS(img_blue,cvScalar(MINB),cvScalar(MAXB),img_blue);
	cvMorphologyEx(img_blue,img_blue,NULL,NULL,CV_MOP_CLOSE);
	
	cvInRangeS(img_green,cvScalar(MING),cvScalar(MAXG),img_green);
	cvMorphologyEx(img_green,img_green,NULL,NULL,CV_MOP_CLOSE);
	
	cvInRangeS(img_red,cvScalar(MINR),cvScalar(MAXR),img_red);
	cvMorphologyEx(img_red,img_red,NULL,NULL,CV_MOP_CLOSE);
    
	cvAdd(img_blue,img_green,grayDiff,NULL);
	cvAdd(grayDiff,img_red,grayDiff,NULL);
}

//find blob
// should start searching here for memory leak
void BlobTracker::find_blob(IplImage *img)
{
	tmp_blob_list.clear();
    
	//Segmentation
	cvFindContours(grayDiff, storage, &contours, sizeof(CvContour),
                   CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );
	
	//reset mask image
	cvZero(removeBackgrnd);
    
	CvRect max_rect=cvRect(0,0,0,0);
	
    if(contours)
	{
		contours = cvApproxPoly( contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 3, 1);
		
		do
		{
			if(contours)
			{
				double area=fabs(cvContourArea(contours,CV_WHOLE_SEQ));
                
				//use big objects
				if(area>MIN_CONTOUR_SIZE)
				{
					CvRect rect = cvBoundingRect(contours);
                    
					//create blob
					blob bl;
					
					bl.min_x=rect.x-5;
					bl.min_y=rect.y-5;
					bl.max_x=rect.x+(rect.width+5);
					bl.max_y=rect.y+(rect.height+5);
					
					bl.bar_x=(bl.min_x+bl.max_x)/2;
					bl.bar_y=(bl.min_y+bl.max_y)/2;
					
					bl.area=(rect.width+10)*(rect.height+10);
					
					bl.id=0;
					
					bl.coords.clear();
#ifdef HSTG
					//histogram gray
					CvSize sz_si = cvSize(rect.width,rect.height);
					
					cv::Ptr<IplImage> histFrame = cvCreateImage( sz_si , IPL_DEPTH_8U ,1 );
					
					TrExtract_SubImage(grayFrame, histFrame, rect.x, rect.y, rect.x+rect.width, rect.y+rect.height);
                    
					int size_hist=256;
					
					bl.hst=cvCreateHist(1,&size_hist,CV_HIST_ARRAY,0,1);
                    
					cvCalcHist((IplImage**)&histFrame,bl.hst,0,0);
					
					cvReleaseImage((IplImage**)&histFrame);
    
                    
					//cvNormalizeHist(bl.hst,1.0);
#endif
                    
#ifdef HSTC
					//histogram color
					CvSize sz_si = cvSize(rect.width,rect.height);
					
					cv::Ptr<IplImage> histFrameRed = cvCreateImage( sz_si , IPL_DEPTH_8U,1);
					cv::Ptr<IplImage> histFrameGreen = cvCreateImage(sz_si,IPL_DEPTH_8U,1);
					cv::Ptr<IplImage> histFrameBlue = cvCreateImage(sz_si,IPL_DEPTH_8U,1);
					cv::Ptr<IplImage> histFrameClr = cvCreateImage(sz_si,IPL_DEPTH_8U,3);
                    
					TrExtract_SubImage(img, histFrameClr, rect.x, rect.y, rect.x+rect.width, rect.y+rect.height);
                    
					cvSplit(histFrameClr,histFrameRed,histFrameGreen,histFrameBlue,0);
					
					int size_hist=32;
					
					bl.hstr=cvCreateHist(1,&size_hist,CV_HIST_ARRAY,0,1);
					cvCalcHist((IplImage**)&histFrameRed,bl.hstr,0,0);
					
					bl.hstg=cvCreateHist(1,&size_hist,CV_HIST_ARRAY,0,1);
					cvCalcHist((IplImage**)&histFrameGreen,bl.hstg,0,0);
					
					bl.hstb=cvCreateHist(1,&size_hist,CV_HIST_ARRAY,0,1);
					cvCalcHist((IplImage**)&histFrameBlue,bl.hstb,0,0);
					
					cvReleaseImage((IplImage**)&histFrameRed);
					cvReleaseImage((IplImage**)&histFrameGreen);
					cvReleaseImage((IplImage**)&histFrameBlue);
					cvReleaseImage((IplImage**)&histFrameClr);
#endif
                    
					unsigned int counter_size=0;
					
					list<blob>::iterator it = blob_list.begin();
					list<blob>::iterator it_max;
					
					double distance_max=-1.0;
					
					if(blob_list.size()>0)//if there are old blobs
					{
#ifdef VERBOSE
						printf("Compare: ");
#endif
						int lost_blob=0;
						//check for similar histogram
						while(counter_size<blob_list.size())
						{
							blob old_blob = *it;
#ifdef HSTG
							double dist=cvCompareHist(bl.hst,old_blob.hst,CV_COMP_CORREL);
#endif
                            
#ifdef HSTC
							double dist=cvCompareHist(bl.hstr,old_blob.hstr,CV_COMP_CORREL);
							dist+=cvCompareHist(bl.hstg,old_blob.hstg,CV_COMP_CORREL);
							dist+=cvCompareHist(bl.hstb,old_blob.hstb,CV_COMP_CORREL);
#endif
							
#ifdef VERBOSE
							printf("(%f,%d) ",dist,old_blob.id);
#endif
							
							if(distance_max<dist)
							{
								distance_max=dist;
								bl.id=old_blob.id;
								it_max=it;
								bl.live_frame=old_blob.live_frame;
								bl.still_frame=old_blob.still_frame;
								bl.coords=old_blob.coords;
                                
							}
							
							it++;
							counter_size++;
						}
						
						counter_size=0;
						it=lost_blob_list.begin();
						while(counter_size<lost_blob_list.size())
						{
							blob old_blob = *it;
#ifdef HSTG
							double dist=cvCompareHist(bl.hst,old_blob.hst,CV_COMP_CORREL);
#endif
							
#ifdef HSTC
							double dist=cvCompareHist(bl.hstr,old_blob.hstr,CV_COMP_CORREL);
							dist+=cvCompareHist(bl.hstg,old_blob.hstg,CV_COMP_CORREL);
							dist+=cvCompareHist(bl.hstb,old_blob.hstb,CV_COMP_CORREL);
#endif
							
#ifdef VERBOSE
							printf("LB (%f,%d) ",dist,old_blob.id);
#endif
							if(distance_max<dist)
							{
								distance_max=dist;
								bl.id=old_blob.id;
								it_max=it;
								bl.live_frame=old_blob.live_frame;
								bl.still_frame=old_blob.still_frame;
								bl.coords=old_blob.coords;
								lost_blob=1;
							}
                            
							counter_size++;
							it++;
						}
#ifdef VERBOSE
						printf("--> Blob: %f,%d,%d\n",distance_max,bl.id,bl.live_frame);
#endif
						if(distance_max<-0.8)//new blob
						{
							bl.id=counter_id++;
							bl.live_frame=0;
							bl.still_frame=0;
							bl.coords.clear();
#ifdef VERBOSE
							printf("New Blob %d,%f\n",bl.id,distance_max);
#endif
						}
						else// blob reassign
						{
							blob old_blob=*it_max;
							
							if(lost_blob==0)
							{
                                cvReleaseHist(&old_blob.hst);
								blob_list.erase(it_max);//erase old blob
							}
							else
							{
                                cvReleaseHist(&old_blob.hst);
								lost_blob_list.erase(it_max);//erase lost blob
							}
							
							bl.live_frame++;
							
							if(sqrt((double)((bl.bar_x-old_blob.bar_x)*(bl.bar_x-old_blob.bar_x)) +
                                    (double)((bl.bar_y-old_blob.bar_y)*(bl.bar_y-old_blob.bar_y)))<DISTANCE)
							{
								bl.still_frame++;
							}
							
							bl.coords=old_blob.coords;
						}
					}
					else//no old blobs, then new blob
					{
						bl.id=counter_id++;
						bl.live_frame=0;
						bl.still_frame=0;
						bl.coords.clear();
#ifdef VERBOSE
						printf("New Blob %d\n",bl.id);
#endif
					}
					
#ifdef VERBOSE
					printf("\n");
#endif
					
					coord crd;
					
					crd.px=bl.bar_x;
					crd.py=bl.bar_y;
					
					bl.coords.push_back(crd);
					
					list<blob>::iterator iter_blob=tmp_blob_list.begin();
					
					unsigned int counter_iter=0;
					int find_item=0;
					
					while(counter_iter<tmp_blob_list.size())
					{
						blob tmp_blob=*iter_blob;
						
						if(tmp_blob.id==bl.id)
						{
							find_item=1;
							break;
						}
						
						iter_blob++;
						counter_iter++;
					}
					
					if(find_item==0)
					{
						tmp_blob_list.push_back(bl);
					}
				}
			}
            //oldContour = contours;
            
            //cvReleaseMemStorage(&oldContour->storage);
			contours=contours->h_next;
            
		} while(contours!=NULL);
	}
}

//find object blob
void BlobTracker::find_object_blob(IplImage *img)
{
#ifdef OBJECT_TRACKER
	tmp_blob_list.clear();
	
	cvCvtColor( img , grayFrame , CV_BGR2GRAY );
	
	Mat gray;
	Mat mat_img(img);
	vector<Rect> faces;
	
	Mat frame( cvRound(mat_img.rows), cvRound(mat_img.cols), CV_8UC1 );
	
	cvtColor( mat_img, gray, CV_BGR2GRAY );
	
	resize( gray, frame, frame.size(), 0, 0, INTER_LINEAR );
	
	equalizeHist( frame, frame );
	
	cascade.detectMultiScale( frame, faces,
                             1.1, 2, 0
                             | CV_HAAR_FIND_BIGGEST_OBJECT
                             //| CV_HAAR_DO_ROUGH_SEARCH
                             | CV_HAAR_SCALE_IMAGE
                             ,Size(30, 30));
    
	CvRect max_rect=cvRect(0,0,0,0);
	
    if(faces.size()!=0)
	{
        for( vector<Rect>::const_iterator r = faces.begin(); r != faces.end(); r++)
		{
			double area=r->width*r->height;
            
			//use big objects
			if(area>MIN_CONTOUR_SIZE)
			{
                
				//create blob
				blob bl;
				
				bl.min_x=r->x-5;
				bl.min_y=r->y-5;
				bl.max_x=r->x+(r->width+5);
				bl.max_y=r->y+(r->height+5);
				
				bl.bar_x=(bl.min_x+bl.max_x)/2;
				bl.bar_y=(bl.min_y+bl.max_y)/2;
                
				bl.area=(r->width+10)*(r->height+10);
				
				bl.id=0;
                
				bl.coords.clear();
#ifdef HSTG
				//histogram gray
				CvSize sz_si = cvSize(r->width,r->height);
				
				cv::Ptr<IplImage> histFrame = cvCreateImage( sz_si , IPL_DEPTH_8U ,1 );
				
				TrExtract_SubImage(grayFrame, histFrame, r->x, r->y, r->x+r->width, r->y+r->height);
                
				int size_hist=256;
				
				bl.hst=cvCreateHist(1,&size_hist,CV_HIST_ARRAY,0,1);
                
				cvCalcHist((IplImage**)&histFrame,bl.hst,0,0);
				
				cvReleaseImage((IplImage**)&histFrame);
                
				//cvNormalizeHist(bl.hst,1.0);
#endif
                
#ifdef HSTC
				//histogram color
				CvSize sz_si = cvSize(r->width,r->height);
				
				cv::Ptr<IplImage> histFrameRed = cvCreateImage( sz_si , IPL_DEPTH_8U,1);
				cv::Ptr<IplImage> histFrameGreen = cvCreateImage(sz_si,IPL_DEPTH_8U,1);
				cv::Ptr<IplImage> histFrameBlue = cvCreateImage(sz_si,IPL_DEPTH_8U,1);
				cv::Ptr<IplImage> histFrameClr = cvCreateImage(sz_si,IPL_DEPTH_8U,3);
                
				TrExtract_SubImage(img, histFrameClr, r->x, r->y, r->x+r->width, r->y+r->height);
                
				cvSplit(histFrameClr,histFrameRed,histFrameGreen,histFrameBlue,0);
				
				int size_hist=32;
				
				bl.hstr=cvCreateHist(1,&size_hist,CV_HIST_ARRAY,0,1);
				cvCalcHist((IplImage**)&histFrameRed,bl.hstr,0,0);
				
				bl.hstg=cvCreateHist(1,&size_hist,CV_HIST_ARRAY,0,1);
				cvCalcHist((IplImage**)&histFrameGreen,bl.hstg,0,0);
				
				bl.hstb=cvCreateHist(1,&size_hist,CV_HIST_ARRAY,0,1);
				cvCalcHist((IplImage**)&histFrameBlue,bl.hstb,0,0);
				
				cvReleaseImage((IplImage**)&histFrameRed);
				cvReleaseImage((IplImage**)&histFrameGreen);
				cvReleaseImage((IplImage**)&histFrameBlue);
				cvReleaseImage((IplImage**)&histFrameClr);
#endif
                
				unsigned int counter_size=0;
				
				list<blob>::iterator it = blob_list.begin();
				list<blob>::iterator it_max;
				
				double distance_max=-1.0;
                
				if(blob_list.size()>0)//if there are old blobs
				{
#ifdef VERBOSE
					printf("Compare: ");
#endif
					int lost_blob=0;
					//check for similar histogram
					while(counter_size<blob_list.size())
					{
						blob old_blob = *it;
                        
#ifdef HSTG
						double dist=cvCompareHist(bl.hst,old_blob.hst,CV_COMP_CORREL);
#endif
                        
#ifdef HSTC
						double dist=cvCompareHist(bl.hstr,old_blob.hstr,CV_COMP_CORREL);
						dist+=cvCompareHist(bl.hstg,old_blob.hstg,CV_COMP_CORREL);
						dist+=cvCompareHist(bl.hstb,old_blob.hstb,CV_COMP_CORREL);
#endif
						
#ifdef VERBOSE
						printf("(%f,%d) ",dist,old_blob.id);
#endif
						if(distance_max<dist)
						{
							distance_max=dist;
							bl.id=old_blob.id;
							it_max=it;
							bl.live_frame=old_blob.live_frame;
							bl.still_frame=old_blob.still_frame;
							bl.coords=old_blob.coords;
						}
						
						it++;
						counter_size++;
					}
					
					counter_size=0;
					it=lost_blob_list.begin();
					while(counter_size<lost_blob_list.size())
					{
						blob old_blob = *it;
						
#ifdef HSTG
						double dist=cvCompareHist(bl.hst,old_blob.hst,CV_COMP_CORREL);
#endif
                        
#ifdef HSTC
						double dist=cvCompareHist(bl.hstr,old_blob.hstr,CV_COMP_CORREL);
						dist+=cvCompareHist(bl.hstg,old_blob.hstg,CV_COMP_CORREL);
						dist+=cvCompareHist(bl.hstb,old_blob.hstb,CV_COMP_CORREL);
#endif
						
#ifdef VERBOSE
						printf("LB (%f,%d) ",dist,old_blob.id);
#endif
						if(distance_max<dist)
						{
							distance_max=dist;
							bl.id=old_blob.id;
							it_max=it;
							bl.live_frame=old_blob.live_frame;
							bl.still_frame=old_blob.still_frame;
							bl.coords=old_blob.coords;
							lost_blob=1;
						}
                        
						counter_size++;
						it++;
					}
#ifdef VERBOSE
					printf("--> Blob: %f,%d,%d\n",distance_max,bl.id,bl.live_frame);
#endif
					if(distance_max<-0.8)//new blob
					{
						bl.id=counter_id++;
						bl.live_frame=0;
						bl.still_frame=0;
						bl.coords.clear();
					}
					else// blob reassign
					{
						blob old_blob=*it_max;
						
						if(lost_blob==0)
						{
							blob_list.erase(it_max);//erase old blob
						}
						else
						{
							lost_blob_list.erase(it_max);//erase lost blob
						}
						
						bl.live_frame++;
						
						if(sqrt((double)((bl.bar_x-old_blob.bar_x)*(bl.bar_x-old_blob.bar_x)) +
                                (double)((bl.bar_y-old_blob.bar_y)*(bl.bar_y-old_blob.bar_y)))<DISTANCE)
						{
							bl.still_frame++;
						}
					}
				}
				else//no old blobs, then new blob
				{
					bl.id=counter_id++;
					bl.live_frame=0;
					bl.still_frame=0;
					bl.coords.clear();
				}
				
#ifdef VERBOSE
				printf("\n");
#endif
				coord crd;
				
				crd.px=bl.bar_x;
				crd.py=bl.bar_y;
				
				bl.coords.push_back(crd);
                
				list<blob>::iterator iter_blob=tmp_blob_list.begin();
				
				unsigned int counter_iter=0;
				int find_item=0;
				
				while(counter_iter<tmp_blob_list.size())
				{
					blob tmp_blob=*iter_blob;
					
					if(tmp_blob.id==bl.id)
					{
						find_item=1;
						break;
					}
					
					iter_blob++;
					counter_iter++;
				}
                
				if(find_item==0)
				{
					tmp_blob_list.push_back(bl);
				}
			}
		}
	}
#endif
}
//update blob list
void BlobTracker::update_blob_list()
{
	blob_list.clear();
	blob_list=tmp_blob_list;
	tmp_blob_list.clear();
}
//initialize blob tracker
void BlobTracker::initialize(IplImage *img)
{
	CvSize sz = cvGetSize(img);
	
	grayImg = cvCreateImage( sz , IPL_DEPTH_8U , 1 );
	grayDiff = cvCreateImage( sz , IPL_DEPTH_8U ,1 );
	removeBackgrnd = cvCreateImage( sz , IPL_DEPTH_8U ,1 );
	grayFrame = cvCreateImage( sz , IPL_DEPTH_8U ,1 );
	img_red = cvCreateImage( sz, 8 , 1 );
	img_green = cvCreateImage( sz, 8 , 1 );
	img_blue = cvCreateImage( sz, 8 , 1 );
	addedImg = cvCreateImage( sz, 8 , 1  );
	
	cvCvtColor( img, grayImg , CV_BGR2GRAY );
	
	storage = cvCreateMemStorage(0);
	contours=0;
	blob_list.clear();
	tmp_blob_list.clear();
	lost_blob_list.clear();
	
#ifdef OBJECT_TRACKER
    if( !cascade.load( cascadeName ) )
	{
		cerr << "ERROR: Could not load cascade classifier \"" << cascadeName << "\"" << endl;
		return;
	}
#endif
    
#ifdef MOG2_SEGMENT
	bg_model=BackgroundSubtractorMOG2(100,10,false);
	//bg_model.backgroundRatio=0.5;
#endif
}
//get blob list
list<blob> BlobTracker::get_blob_list()
{
	return blob_list;
}
//get background
IplImage *BlobTracker::get_background()
{
	return grayImg;
}

//get mask
IplImage *BlobTracker::get_mask()
{
	return removeBackgrnd;
}

//get foreground
IplImage *BlobTracker::get_foreground()
{
	return grayDiff;
}

//release blob tracker
void BlobTracker::release()
{
	cvReleaseImage((IplImage **)&removeBackgrnd);
	cvReleaseImage((IplImage **)&grayImg);
	cvReleaseImage((IplImage **)&grayFrame);
	cvReleaseImage((IplImage **)&img_red);
	cvReleaseImage((IplImage **)&img_green);
	cvReleaseImage((IplImage **)&img_blue);
	cvReleaseImage((IplImage **)&addedImg);
}

void BlobTracker::TrExtract_SubImage(IplImage* srcimg, IplImage* dstimg, int xmin, int ymin, int xmax, int ymax)
{
	uchar *src=NULL;
	uchar *dst=NULL;
    
	int i,j,q, src_step, dst_step;
    
	src_step=(srcimg)->widthStep;
	dst_step=(dstimg)->widthStep;
    
	src = (uchar*)(srcimg->imageData)+(ymin*src_step);
	dst = (uchar*)(dstimg->imageData);
    
	if(srcimg->nChannels==3 && dstimg->nChannels==3)
	{
		for(i=ymin; i<ymax; i++, src+=src_step, dst+=dst_step)
			for(j=3*xmin, q=0; j<3*xmax; j+=3, q+=3)
			{
				dst[q]=src[j];
				dst[q+1]=src[j+1];
				dst[q+2]=src[j+2];
			}
	}
    
	if(srcimg->nChannels==3 && dstimg->nChannels==1)
	{
		for(i=ymin; i<ymax; i++, src+=src_step, dst+=dst_step)
			for(j=3*xmin, q=0; j<3*xmax; j+=3, q+=1)
			{
				dst[q]=(uchar)(0.212671*src[j]+0.715160*src[j+1]+0.072169*src[j+2]);
			}
	}
    
	if(srcimg->nChannels==1 && dstimg->nChannels==1)
	{
		for(i=ymin; i<ymax; i++, src+=src_step, dst+=dst_step)
			for(j=xmin, q=0; j<xmax; j+=1, q+=1)
			{
				dst[q]=src[j];
			}
	}
    
	if(srcimg->nChannels==1 && dstimg->nChannels==3)
	{
		for(i=ymin; i<ymax; i++, src+=src_step, dst+=dst_step)
			for(j=xmin, q=0; j<xmax; j+=1, q+=3)
			{
				dst[q]=src[j];
				dst[q+1]=src[j];
				dst[q+2]=src[j];
			}
	}
}

