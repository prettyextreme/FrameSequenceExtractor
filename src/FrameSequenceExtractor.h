#pragma once

#include "ofMain.h"
#include "ofxTurboJpeg.h"
#include "ofxGui.h"
#include "ofxPostProcessing.h"

class FrameSequenceExtractor : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);		

		ofVideoPlayer 		fingerMovie;
		bool                frameByframe;
    
        ofxTurboJpeg turboJPEG;
    int frameNumToSave;
    
    int passNum; //0 to set, 1 to start, 2 to fade, 3 to exit
    void startPlayback();
    void loadMovie(string path, int jumpToFrame = 0);
    
    
    
	ofxIntSlider inAtPercent;
	ofxIntSlider outAtPercent;
    
    ofxFloatSlider testAtPercent;
    int lastJumpedTestFrame;
    
	ofxIntSlider fadeFrames;
	ofxButton startButton;
    
    ofxFloatSlider brightnessSlider;
    ofxFloatSlider contrastSlider;
    ofxFloatSlider blackSlider;
    ofxFloatSlider whiteSlider;
    
    ofxFloatSlider redSlider;
    ofxFloatSlider greenSlider;
    ofxFloatSlider blueSlider;
    
    
	ofxPanel gui;
    
    int totalReadFrameCt;
    int firstReadFrameNum;
    int finalReadFrameNum;
    
    string sourceFile;
    string destFolder;
    
    ofFbo tempFbo;
    ofFbo tempFboCrossfade;
    
    
    ofxPostProcessing post;
    ContrastPass::Ptr contrastPass;
    BWLevelsPass::Ptr levelsPass;
    RGBScalePass::Ptr rgbScalePass;
    
    void setPostVals();
};

