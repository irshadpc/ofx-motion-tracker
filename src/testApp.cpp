#include "testApp.h"
#include "blobTracker.h"

BlobTracker blobTracker;
//--------------------------------------------------------------
void testApp::setup(){

	#ifdef _USE_LIVE_VIDEO
        vidGrabber.setVerbose(true);
        vidGrabber.initGrabber(320,240);
	#else
        vidPlayer.loadMovie("fingers.mov");
        vidPlayer.play();
	#endif
    
    mainOutputSyphonServer.setName("Syphon Output");

    colorImg.allocate(320,240);
	grayImage.allocate(320,240);
	grayBg.allocate(320,240);
	grayDiff.allocate(320,240);

	bLearnBakground = true;
	threshold = 80;
    
    bool frame = false;
    while (!frame) {
        vidGrabber.grabFrame();
        testImg.setFromPixels(vidGrabber.getPixels(), 320, 240);
        frame = vidGrabber.isFrameNew();
    }
    vidGrabber.grabFrame();
    testImg.setFromPixels(vidGrabber.getPixels(), 320, 240);
    
    
    blobTracker.initialize(testImg.getCvImage());
        
    
}

//--------------------------------------------------------------
void testApp::update(){
	//ofBackground(100,100,100);
    
    bool bNewFrame =false;
    
    vidGrabber.grabFrame();
    bNewFrame = vidGrabber.isFrameNew();
    
    //if (bNewFrame) {
        colorImg.setFromPixels(vidGrabber.getPixels(), 320,240);
        
#ifdef IMAGE_DIFFERENCE
		blobTracker.image_difference(colorImg.getCvImage());
#endif
        
#ifdef COLOR_TRACKER
		blobTracker.color_track(colorImg.getCvImage());
#endif
        
#ifndef OBJECT_TRACKER
		//find blob
		blobTracker.find_blob(colorImg.getCvImage());
#endif
		
#ifdef OBJECT_TRACKER
		blobTracker.find_object_blob(colorImg.getCvImage());
#endif
        //merging overlapped blobs in actual blob list
		blobTracker.merge_overlapped_blobs();
		
		//merging near blob in actual blob list
		blobTracker.merge_blobs();
		
		//add lost blobs
		blobTracker.add_lost_blobs();
		
		//delete lost blobs
		blobTracker.delete_lost_blobs();
		
		//update blob list
		blobTracker.update_blob_list();
		
        //blobTracker.update_backgroud();
        
        list<blob> bl = blobTracker.get_blob_list();
        blobTracker.draw_blobs(colorImg.getCvImage(),bl);
        
        
        IplImage *test = blobTracker.get_background();
        unsigned char *pixelData = new unsigned char[test->width*test->height];
        
        memcpy(pixelData,test->imageData,test->width*test->height);
        
        grayBg.setFromPixels(pixelData, test->width, test->height);
        
        
        //blobTracker.update_backgroud();
        
        blobTracker.draw_mask();
        IplImage *testTwo = blobTracker.get_mask();
        unsigned char *pixelDataTwo = new unsigned char[testTwo->width*testTwo->height];
        
        memcpy(pixelDataTwo,testTwo->imageData,testTwo->width*testTwo->height);
        
        grayDiff.setFromPixels(pixelDataTwo, testTwo->width, testTwo->height);
        
        delete []pixelData;
        delete []pixelDataTwo;

   // }

    /*bool bNewFrame = false;

	#ifdef _USE_LIVE_VIDEO
       vidGrabber.grabFrame();
	   bNewFrame = vidGrabber.isFrameNew();
    #else
        vidPlayer.idleMovie();
        bNewFrame = vidPlayer.isFrameNew();
	#endif

	if (bNewFrame){

		#ifdef _USE_LIVE_VIDEO
            colorImg.setFromPixels(vidGrabber.getPixels(), 320,240);
	    #else
            colorImg.setFromPixels(vidPlayer.getPixels(), 320,240);
        #endif

        grayImage = colorImg;
		if (bLearnBakground == true){
			grayBg = grayImage;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
			bLearnBakground = false;
		}

		// take the abs value of the difference between background and incoming and then threshold:
		grayDiff.absDiff(grayBg, grayImage);
		grayDiff.threshold(threshold);

		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		contourFinder.findContours(grayDiff, 20, (340*240)/3, 10, true);	// find holes
	}*/

}

//--------------------------------------------------------------
void testApp::draw(){
    //show blob list
   
    
    //show blob list
	/*list<blob>::iterator iter;
	iter = bl.begin();
	unsigned int counter_iter=0;
	
	while(counter_iter<bl.size())
	{
		blob tmp_blob=*iter;
		
        std::cout << " [ " << std::endl;
        std::cout << tmp_blob.min_x << " " << tmp_blob.max_x << std::endl;
        std::cout << tmp_blob.min_y << " " << tmp_blob.max_y << std::endl;
        std::cout << " ] " << std::endl;


		
        iter++;
		counter_iter++;
	}*/
    
        
   
    
    colorImg.draw(0, 0);
    grayBg.draw(360, 0);
    grayDiff.draw(0, 280);
    
    blobTracker.update_backgroud();
    
  


    
	// draw the incoming, the grayscale, the bg and the thresholded difference
	//ofSetHexColor(0xffffff);
	//colorImg.draw(20,20);
	//grayImage.draw(360,20);
	//grayBg.draw(20,280);
	//grayDiff.draw(360,280);

	// then draw the contours:

	//ofFill();
	//ofSetHexColor(0x333333);
	//ofRect(360,540,320,240);
	//ofSetHexColor(0xffffff);

	// we could draw the whole contour finder
	//contourFinder.draw(360,540);

	// or, instead we can draw each blob individually,
	// this is how to get access to them:
    //for (int i = 0; i < contourFinder.nBlobs; i++){
    //    contourFinder.blobs[i].draw(360,540);
    //}

	// finally, a report:

	/*ofSetHexColor(0xffffff);
	char reportStr[1024];
	sprintf(reportStr, "bg subtraction and blob detection\npress ' ' to capture bg\nthreshold %i (press: +/-)//\nnum blobs found %i, fps: %f", threshold, contourFinder.nBlobs, ofGetFrameRate());
	ofDrawBitmapString(reportStr, 20, 600);*/
    
    mainOutputSyphonServer.publishScreen();

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	switch (key){
		case ' ':
			bLearnBakground = true;
			break;
		case '+':
			threshold ++;
			if (threshold > 255) threshold = 255;
			break;
		case '-':
			threshold --;
			if (threshold < 0) threshold = 0;
			break;
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
