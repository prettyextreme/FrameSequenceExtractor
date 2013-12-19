#include "FrameSequenceExtractor.h"

//--------------------------------------------------------------
void FrameSequenceExtractor::setup(){
	ofBackground(255,255,255);
	ofSetVerticalSync(true);
	frameByframe = false;
    
    passNum = 0;

	// Uncomment this to show movies with alpha channels
	// fingerMovie.setPixelFormat(OF_PIXELS_RGBA);
    
    gui.setup();
    gui.add(fadeFrames.setup("Fade Frames",120,0,360));
    gui.add(inAtPercent.setup("In at %",0,0,100));
    gui.add(outAtPercent.setup("Out at %",100,0,100));
    gui.add(testAtPercent.setup("Test at %",1,0,100));
    gui.add(brightnessSlider.setup("Brightness",50,0,100));
    gui.add(contrastSlider.setup("Contrast",50,0,100));
    
    gui.add(blackSlider.setup("BlackLevel",0,0,90));
    gui.add(whiteSlider.setup("WhiteLevel",100,10,100));
    
    
    gui.add(redSlider.setup("R Scale",100,0,100));
    gui.add(greenSlider.setup("G Scale",100,0,100));
    gui.add(blueSlider.setup("B Scale",100,0,100));
    
    
    //gui.add(srcPath.setup("Source File",""));
    gui.add(startButton.setup("START"));
    
	startButton.addListener(this,&FrameSequenceExtractor::startPlayback);
    
    
    // Setup post-processing chain
    post.init(1280,720);
    post.createPass<FxaaPass>()->setEnabled(false);
    post.createPass<RGBShiftPass>()->setEnabled(false);
    
    contrastPass = post.createPass<ContrastPass>();
    contrastPass->setEnabled(false);
    
    rgbScalePass = post.createPass<RGBScalePass>();
    rgbScalePass->setEnabled(true);
    
    levelsPass = post.createPass<BWLevelsPass>();
    levelsPass->setEnabled(true);
    
    post.createPass<OffsetPass>()->setEnabled(true);
    post.createPass<FakeSSSPass>()->setEnabled(false);
    post.createPass<NoiseWarpPass>()->setEnabled(false);
    post.createPass<LimbDarkeningPass>()->setEnabled(false);
    post.createPass<RimHighlightingPass>()->setEnabled(false);
    post.createPass<SSAOPass>()->setEnabled(false);
    post.createPass<ToonPass>()->setEnabled(false);

    post.setFlip(false);
    
    lastJumpedTestFrame = 0;

}

//--------------------------------------------------------------
void FrameSequenceExtractor::update(){
    
    if(ofGetFrameNum() == 1)
        loadMovie("/Users/Josh/Desktop/IMG_2348.MOV",1);
    
    if(passNum == 0){
        
        int testAtFrame = 0;
        if(fingerMovie.getTotalNumFrames()>0)
            testAtFrame = testAtPercent * fingerMovie.getTotalNumFrames() / 100;
        if(testAtFrame != lastJumpedTestFrame){
            fingerMovie.setFrame(testAtFrame);
            fingerMovie.update();
            lastJumpedTestFrame = testAtFrame;
        }
        
        return;
        
    }

    //fingerMovie.update();
    fingerMovie.nextFrame();
    if(fingerMovie.isFrameNew()){
        fingerMovie.update();
        
        int currentReadFrame = fingerMovie.getCurrentFrame();
        if(currentReadFrame >= firstReadFrameNum){
            if( currentReadFrame < (finalReadFrameNum - fadeFrames) ){
                    //do it up regular. Just save the frame
                
                char filename[100];
                sprintf(filename,"%s/%06d.jpg", destFolder.c_str(), frameNumToSave++);
                
                
                
                tempFbo.begin();
                setPostVals();
                post.begin();
                
                ofSetColor(255,255,255,255);
                fingerMovie.draw(0,0);
                
                post.end();
                tempFbo.end();
                turboJPEG.save(&tempFbo, filename, 100);
                
                
                
                
                //turboJPEG.save(fingerMovie.getPixels(), filename, fingerMovie.getWidth(), fingerMovie.getHeight());
                
            } else if(currentReadFrame < finalReadFrameNum){
                if(currentReadFrame == (finalReadFrameNum - fadeFrames)){
                    frameNumToSave = 0;//START OVER! We will resave these!
                }
            
                char filename[100];
                sprintf(filename,"%s/%06d.jpg", destFolder.c_str(), frameNumToSave++);
                
                
                setPostVals();
                post.begin();
                ofSetColor(255,255,255,255);
                fingerMovie.draw(0,0);
                post.end(false);
                
                
                ofImage tempImage;
                turboJPEG.load(filename, &tempImage);
                
                tempFbo.begin();
                
                ofSetColor(255,255,255,255);
                tempImage.draw(0,0);
                
                ofSetColor(255,255,255,ofMap(currentReadFrame,finalReadFrameNum - fadeFrames-1,finalReadFrameNum,255,0));
                post.draw();
                
                tempFbo.end();
                
                turboJPEG.save(&tempFbo, filename, 100);
                
            } else {
                ofExit();
            }
        //Save the frame!
        }
        
    }
}

void FrameSequenceExtractor::startPlayback(){
    
    if(destFolder.length()==0){
        printf("Error! Bad Destination\n");
        return;
    }
    passNum = 1;
    
    loadMovie(sourceFile);
}

void FrameSequenceExtractor::setPostVals(){
    
    contrastPass->setBrightness(brightnessSlider/100.0f);
    contrastPass->setContrast(contrastSlider/100.0f);

    levelsPass->setWhiteLevel(whiteSlider/100.0f);
    levelsPass->setBlackLevel(blackSlider/100.0f);
    
    rgbScalePass->setRedScale(redSlider/100.0f);
    rgbScalePass->setGreenScale(greenSlider/100.0f);
    rgbScalePass->setBlueScale(blueSlider/100.0f);
}

//--------------------------------------------------------------
void FrameSequenceExtractor::draw(){
    
    
    setPostVals();
    
    // begin scene to post process
    post.begin();
    
	ofSetHexColor(0xFFFFFF);
    fingerMovie.draw(0,0);
    ofSetHexColor(0x000000);
    
    post.end();
    
    
    if(passNum == 0)
    {
		gui.draw();
        
        
        ofSetHexColor(0xFF0000);
        ofDrawBitmapString("Source File: " + sourceFile,20,200);
        
        ofDrawBitmapString("Destination Folder: " + destFolder,20,240);
        
        //return;

    }

//    
//    unsigned char * pixels = fingerMovie.getPixels();
//    
//    
//    int nChannels = fingerMovie.getPixelsRef().getNumChannels();
//    
//    // let's move through the "RGB(A)" char array
//    // using the red pixel to control the size of a circle.
//    for (int i = 4; i < 320; i+=8){
//        for (int j = 4; j < 240; j+=8){
//            unsigned char r = pixels[(j * 320 + i)*nChannels];
//            float val = 1 - ((float)r / 255.0f);
//            ofCircle(400 + i,20+j,10*val);
//        }
//    }


    ofSetHexColor(0x000000);

    ofDrawBitmapString("frame: " + ofToString(fingerMovie.getCurrentFrame()) + "/"+ofToString(fingerMovie.getTotalNumFrames()),20,380);
    ofDrawBitmapString("duration: " + ofToString(fingerMovie.getPosition()*fingerMovie.getDuration(),2) + "/"+ofToString(fingerMovie.getDuration(),2),20,400);
    ofDrawBitmapString("speed: " + ofToString(fingerMovie.getSpeed(),2),20,420);
    ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate(),2),20,440);

    if(fingerMovie.getIsMovieDone()){
        ofSetHexColor(0xFF0000);
        ofDrawBitmapString("end of movie",20,440);
    }
}

//--------------------------------------------------------------
void FrameSequenceExtractor::keyPressed  (int key){
    
    
    unsigned idx = key - '0';
    if (idx < post.size()) post[idx]->setEnabled(!post[idx]->getEnabled());
//    
//    switch(key){
//        case 'f':
//            frameByframe=!frameByframe;
//            fingerMovie.setPaused(frameByframe);
//        break;
//        case OF_KEY_LEFT:
//            fingerMovie.previousFrame();
//        break;
//        case OF_KEY_RIGHT:
//            fingerMovie.nextFrame();
//        break;
//        case '0':
//            fingerMovie.firstFrame();
//        break;
//    }
}

//--------------------------------------------------------------
void FrameSequenceExtractor::keyReleased(int key){

}

//--------------------------------------------------------------
void FrameSequenceExtractor::mouseMoved(int x, int y ){
	if(!frameByframe){
        int width = ofGetWidth();
        float pct = (float)x / (float)width;
        float speed = (2 * pct - 1) * 5.0f;
        fingerMovie.setSpeed(speed);
	}
}

//--------------------------------------------------------------
void FrameSequenceExtractor::mouseDragged(int x, int y, int button){
	if(!frameByframe){
        int width = ofGetWidth();
        float pct = (float)x / (float)width;
        fingerMovie.setPosition(pct);
	}
}

//--------------------------------------------------------------
void FrameSequenceExtractor::mousePressed(int x, int y, int button){
	if(!frameByframe){
        fingerMovie.setPaused(true);
	}
}


//--------------------------------------------------------------
void FrameSequenceExtractor::mouseReleased(int x, int y, int button){
    
    if(passNum == 0){
        ofMouseEventArgs args;
        args.x = x;
        args.y = y;
        args.button = button;
        gui.mouseReleased(args);
        return;
    }
    
        
	if(!frameByframe){
        fingerMovie.setPaused(false);
	}
}

//--------------------------------------------------------------
void FrameSequenceExtractor::windowResized(int w, int h){

}

//--------------------------------------------------------------
void FrameSequenceExtractor::gotMessage(ofMessage msg){

}

void FrameSequenceExtractor::loadMovie(string path, int jumpToFrame){
    
    if(fingerMovie.getWidth()>0)
        fingerMovie.close();
    
    fingerMovie.loadMovie(path);
    //fingerMovie.play();
    fingerMovie.setPaused(true);
    
    tempFbo.allocate(fingerMovie.getWidth(), fingerMovie.getHeight());
    tempFboCrossfade.allocate(fingerMovie.getWidth(), fingerMovie.getHeight());
    
    totalReadFrameCt = fingerMovie.getTotalNumFrames();
    firstReadFrameNum = ofMap(inAtPercent,0,100,0,totalReadFrameCt);
    finalReadFrameNum = ofMap(outAtPercent,0,100,0,totalReadFrameCt);
    
    sourceFile = path;
    
    if(jumpToFrame > 0){
        fingerMovie.setFrame(jumpToFrame);
        fingerMovie.nextFrame();
        fingerMovie.update();
    }

}
//--------------------------------------------------------------
void FrameSequenceExtractor::dragEvent(ofDragInfo dragInfo){ 
    printf("Drop?\n");
    if(dragInfo.position.y<220){
        
        loadMovie(dragInfo.files[0],100);
        
    }
    else
        destFolder = dragInfo.files[0];
}
