#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "PolylineWithLerpFunction.h"
#include "ofxFaceTracker2.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
    void keyPressed(int key);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);

    
    ofImage input;
    ofImage output;
    
    
//    ofParameter<float> sigma1, sigma2, tau;
//    ofParameter<int> halfw, smoothPasses, black, thresh, cannyParam1, cannyParam2;
//    ofParameter<bool> doFDoG, doThresh, doThin, doCanny;
    
	ofxFaceTracker2 tracker;
	ofVideoGrabber cam;
    ofPixels rotatedPixels;
    
    
    float velocity;
    float gravity;

    ofPolyline singleline;
    ofPolyline singlelinePreLineFade;
    ofPolyline lastFace;
    
    PolylineWithLerpFunction lerpline;
    int findex;
    
    ofImage img;
    
    ofVec3f vertexPosition;
    float lengthOfLineAtPoint;
    ofPoint dragPos;
    
    ofVec3f actualLinePosition;
    float lengthOfLineAtVertex;
    float linePercent;
    
    float startTime;
    
    float faceFoundPct;
    float faceSizeScale;
    
};

