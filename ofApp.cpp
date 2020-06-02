#include "ofApp.h"

using namespace ofxCv;

//line, place you want the squiggles (which index), how many squiggles
ofPolyline addPerpendiculars(ofPolyline line, int index, int count=1, float length=20) {
    vector<ofPoint> points; //full of contour points
    for (int i = 0; i < line.size(); i++) {
        points.push_back(line.getVertices()[i]);
    }
    if (index > points.size()) { index = points.size()-1; }
    for (int i = 0; i < count; i ++) {
        ofVec2f perpendicular = ofVec2f((points[index+1] - points[index])).getPerpendicular() * length;
        if ( i%2 == 0 ) {
            ofPoint p = points[index]-perpendicular;
            line.getVertices()[index] = p;
        } else {
            ofPoint p = points[index]+perpendicular;
            line.getVertices()[index] = p;
        }
        index -= 1;
    }
    return line;
}

ofPolyline movePointsByLerp(ofPolyline line, int index, float x, float y) {
    vector<ofPoint> points; //full of contour points
    for (int i = 0; i < line.size(); i++) {
        points.push_back(line.getVertices()[i]);
    }
    if (index > points.size()) { index = points.size()-1; }
    ofPoint p = ofPoint(x, y);
    line.getVertices()[index] = p;
    return line;
}

ofPolyline dropPoints(ofPolyline line, int index, int stretch) {
    vector <ofPoint> points;
    for (int i = 0; i < line.size(); i++) {
        points.push_back(line.getVertices()[i]);
    }
    if (index > points.size()) { index = points.size()-1; }
    if (stretch > ofGetHeight()) { stretch = ofGetHeight()-1; }
    ofPoint p = points[index];
    ofPoint prev = points[index-2];
    ofPoint next = points[index+2];
    p.y += stretch;
    prev.y += stretch/2;
    next.y += stretch/2;
    line.getVertices()[index] = p;
    line.getVertices()[index-2] = prev;
    line.getVertices()[index+2] = next;
    return line;
}

// still working on this! //
ofPolyline melt(ofPolyline line, int place, float velocity) {
    vector <ofPoint> points;
    for (int i = 0; i < line.size(); i++) {
        points.push_back(line.getVertices()[i]);
    }
    ofPoint p = points[place];
    if (p.y == 0) { velocity = -velocity;}
    p.y = velocity;
    line.getVertices()[place] = p;
    return line;
}


//pt - centroid
ofPolyline scale(ofPolyline line, float scale) {
    ofPoint centroid = line.getCentroid2D();
    for (int i = 0; i < line.size(); i++) {
        ofPoint p = line[i] - centroid;
        ofPoint n = p*scale + centroid;

        line[i] = n;
    }
    return line;
}

//--------------------------------------------------------------------------------//
void ofApp::setup() {
	tracker.setup();
    cam.setDeviceID(1);
//    cam.setGrabber(std::make_shared<ofxPS3EyeGrabber>());
    cam.setup(600, 450);
    input.allocate(600, 450, OF_IMAGE_GRAYSCALE);
//    rotatedPixels.allocate(640, 480,OF_IMAGE_COLOR);
    startTime = ofGetElapsedTimef();
    
    dragPos.x = 0;
    dragPos.y = 0;
    
    velocity = 0.0;
    faceFoundPct = 0;
    
    ofPoint a = ofPoint(0, ofGetHeight()/2);
    ofPoint b = ofPoint(ofGetWidth(), ofGetHeight()/2);
    
    singlelinePreLineFade.addVertex(a);
    singlelinePreLineFade.addVertex(b);
    
    faceSizeScale = 2;
}
//--------------------------------------------------------------------------------//
void ofApp::update() {
	cam.update();
//    rotatedPixels = cam.getPixels();
//    rotatedPixels.rotate90(-1);
//    cam.getPixels().rotate90(-1);
//    cout << ofGetFrameNum() << endl;
	if(cam.isFrameNew()) {
		tracker.update(cam);
        
	}
    if (velocity > ofGetHeight()/2) {
        velocity = velocity - gravity;
    }
    if (velocity < ofGetHeight()/2) {
        velocity = velocity + gravity;
    }
}
//--------------------------------------------------------------------------------//
void ofApp::draw() {
    
    ofSeedRandom(5);
	ofBackground(0);
//    cam.draw(0,0);
	ofSetColor(255);
    ofSetLineWidth(2);
    ofPolyline baseline;
    ofPoint a = ofPoint(0, ofGetHeight()/2);
    ofPoint b = ofPoint(ofGetWidth(), ofGetHeight()/2);
    baseline.addVertex(a);
    baseline.addVertex(b);
    
    cout << tracker.getInstances().size() << endl;
    
    for(auto face : tracker.getInstances()){
        faceFoundPct = faceFoundPct * 0.98 + 0.02 * 1.0;
//left eye
        ofPolyline leftEyeBase = face.getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::LEFT_EYE);
        ofPoint leftEyePoint = leftEyeBase.getCentroid2D();
        
        ofPolyline leftEyePupil;
        for (float i = -1.2; i < TWO_PI-1.2; i += 0.1) {
            ofPoint p = leftEyePoint;
            float rX = 1.5;
            float rY = 2.5;
            p.x += cos(i)*rX;
            p.y += sin(i)*rY;
            leftEyePupil.addVertex(p);
        }
        
        leftEyeBase = leftEyeBase.getResampledBySpacing(5);
        leftEyeBase = leftEyeBase.getSmoothed(3);
        leftEyeBase.addVertices(leftEyePupil.getVertices());

//right eye
        ofPolyline rightEyeBase = face.getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::RIGHT_EYE);
        ofPoint rightEyePoint = rightEyeBase.getCentroid2D();
        
        ofPolyline rightEyePupil;
        for (float i = -1.2; i < TWO_PI-1.2; i += 0.1) {
            ofPoint p = rightEyePoint;
            float rX = 1.5;
            float rY = 2.5;
            p.x += cos(i)*rX;
            p.y += sin(i)*rY;
            rightEyePupil.addVertex(p);
        }
        cout << rightEyePupil.size() << endl;
        
        
        rightEyeBase = rightEyeBase.getResampledBySpacing(5);
        rightEyeBase = rightEyeBase.getSmoothed(3);
        rightEyeBase.addVertices(rightEyePupil.getVertices());
        
//        float opennessL = tracker.getGesture(ofxFaceTracker::LEFT_EYE_OPENNESS);
//        float opennessR = tracker.getGesture(ofxFaceTracker::RIGHT_EYE_OPENNESS);
//
    
            for (int i = 0; i < leftEyeBase.getVertices().size(); i++) {
                ofVec2f pt = leftEyeBase.getVertices()[i];
                pt.y += ofMap(sin(leftEyePoint.x*.06), -1, 1, 0, 30);
                leftEyeBase.getVertices()[i] = glm::vec3(pt.x, pt.y, 0);  ////bounce!
            }
        leftEyeBase = scale(leftEyeBase, sin(ofGetElapsedTimef()*.3) + 1.5);
    //        leftEyeBase = scale(leftEyeBase, leftEyeBase[0].x/150);
            for (int i = 0; i < leftEyeBase.size(); i++){
                ofPoint pt = leftEyeBase[i];
                float amtx = ofSignedNoise(pt.x*0.005, pt.y*0.005) * sin(ofGetElapsedTimef())*20 + 1.5;
                float amty = ofSignedNoise(pt.x*0.01, pt.y*0.01, 1000)* sin(ofGetElapsedTimef())*20 + 1.5;
                leftEyeBase[i] += ofPoint(amtx, amty);
            }

            for (int i = 0; i < rightEyeBase.getVertices().size(); i++) {
                ofVec2f pt = rightEyeBase.getVertices()[i];
                pt.y += ofMap(sin(rightEyePoint.x*.06), -1, 1, 0, 30);
                rightEyeBase.getVertices()[i] = glm::vec3(pt.x, pt.y, 0); ////bounce!
            }
        rightEyeBase = scale(rightEyeBase, sin(ofGetElapsedTimef()*.3) + 1.5);
    //        rightEyeBase = scale(rightEyeBase, rightEyeBase[0].x/150);
            for (int i = 0; i < rightEyeBase.size(); i++){
                ofPoint pt = rightEyeBase[i];
                float amtx = ofSignedNoise(pt.x*0.005, pt.y*0.005) * sin(ofGetElapsedTimef())*20 + 2;
                float amty = ofSignedNoise(pt.x*0.01, pt.y*0.01, 1000)* sin(ofGetElapsedTimef())*20 + 2;
                rightEyeBase[i] += ofPoint(amtx, amty);
            }
        
        
        leftEyePoint = leftEyeBase.getCentroid2D();
        rightEyePoint = rightEyeBase.getCentroid2D();
        


        
//        rightEyePupil = scale(rightEyePupil, rightEyeBase[0].x/200);


        ofPoint browdist;
        ofPolyline leftEyebrow = face.getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::LEFT_EYEBROW);
        leftEyebrow = leftEyebrow.getResampledBySpacing(5);
        ofPolyline reverseLEyebrow;
        for (int i = leftEyebrow.size()-1; i>=0; i--) {
            reverseLEyebrow.addVertex(leftEyebrow[i]);
        }
        reverseLEyebrow = reverseLEyebrow.getSmoothed(3);
        ofPolyline rightEyebrow = face.getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::RIGHT_EYEBROW);
        rightEyebrow = rightEyebrow.getResampledBySpacing(5);
        ofPolyline reverseREyebrow;
        for (int i = rightEyebrow.size()-1; i>=0; i--) {
            reverseREyebrow.addVertex(rightEyebrow[i]);
        }
        reverseREyebrow = reverseREyebrow.getSmoothed(3);
        browdist.x = (rightEyebrow.getCentroid2D().x + leftEyebrow.getCentroid2D().x)/2;
        browdist.y = (rightEyebrow.getCentroid2D().y + leftEyebrow.getCentroid2D().y)/2;
        ofPolyline jaw = face.getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::JAW);
    ofPolyline outerMouth = face.getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::OUTER_MOUTH);
        ofPolyline innerMouth = face.getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::INNER_MOUTH);
        ofPolyline nose;
       nose.addVertices(face.getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::NOSE_BRIDGE).getVertices()); nose.addVertices(face.getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::NOSE_BASE).getVertices());
        
        nose.getResampledBySpacing(5);
        nose.getSmoothed(3);
//        ofPolyline noseBase = face.getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::NOSE_BASE);
//        ofPolyline noseBridge = face.getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::NOSE_BRIDGE);
        ofPoint foreheadmid;
        foreheadmid.x = nose.getVertices()[0].x;
        foreheadmid.y = leftEyebrow.getCentroid2D().y - 110;
        ofVec3f center = (jaw.getVertices()[jaw.getVertices().size()-1] + jaw.getVertices()[0]) * 0.5;
        float xradius = (jaw.getVertices()[jaw.getVertices().size()-1].x - jaw.getVertices()[0].x) * 0.5;
        jaw.arc(center, xradius, foreheadmid.y-browdist.y+25, 0, 180);
        jaw = jaw.getResampledBySpacing(6);
        ofPolyline foreheadChunk;
        for (int i = jaw.size()-1; i > jaw.size()/2; i--) {
            foreheadChunk.addVertex(jaw[i].x+10, jaw[i].y);
        }
        foreheadChunk = scale(foreheadChunk, .8);
        for (int i = 0; i < foreheadChunk.size(); i++){
            ofPoint pt = foreheadChunk[i];
            float amtx = ofSignedNoise(pt.x*0.01, pt.y*0.01, 0) * sin(ofGetElapsedTimef())*20;
            float amty = ofSignedNoise(pt.x*0.01, pt.y*0.01, 0 + 10000)* sin(ofGetElapsedTimef())*20;
            foreheadChunk[i] += ofPoint(amtx, amty);
        }
        jaw = jaw.getResampledBySpacing(1);
        jaw = jaw.getSmoothed(3);
        foreheadChunk.getSmoothed(3);

//        float timef = ofGetElapsedTimef();

        for (int i = 0; i < jaw.size(); i++){
            ofPoint pt = jaw[i];
            float amtx = ofSignedNoise(pt.x*0.01, pt.y*0.01, 0) * sin(ofGetElapsedTimef())*20;
            float amty = ofSignedNoise(pt.x*0.01, pt.y*0.01, 0 + 10000)* sin(ofGetElapsedTimef())*20;
            jaw[i] += ofPoint(amtx, amty);
        }
        
//        jaw = addPerpendiculars(jaw, 10, 5);
// default order -->
// right jaw
// to left jaw
// to right eyebrow
// to left eyebrow
// to bridge of nose
// to base of nose
// to right eye
// to left eye
// to outer mouth
// to inner mouth
        
        singleline.clear(); //necessary because singleline is a global variable now
        


        singleline.addVertices(leftEyeBase.getVertices());
//        singleline.addVertices(leftEyePupil.getVertices());
        singleline.addVertices(reverseLEyebrow.getVertices());
        singleline.addVertices(jaw.getVertices());
        singleline.addVertices(foreheadChunk.getVertices());
//        singleline.addVertices(rightEyePupil.getVertices());
        singleline.addVertices(rightEyeBase.getVertices());
        singleline.addVertices(reverseREyebrow.getVertices());
//        singleline.addVertices(noseBridge.getVertices());
//        singleline.addVertices(noseBase.getVertices());
        singleline.addVertices(nose.getVertices());
        singleline.addVertices(innerMouth.getVertices());
        singleline.addVertices(outerMouth.getVertices());
        //ofRandomize(singleline.getVertices());
        
        
        singleline = singleline.getResampledBySpacing(1);

        
        singlelinePreLineFade = singleline;
        
//        for (int i = 0; i < singleline.size(); i++){
//            ofPoint pt = singleline[i];
//            float amtx = ofSignedNoise(pt.x*0.01, pt.y*0.01, timef) * mouseX;
//            float amty = ofSignedNoise(pt.x*0.01, pt.y*0.01, timef + 10000)* mouseX;
//            singleline[i] += ofPoint(amtx, amty);
//        }
        
        
        
        if (true){ //tracker.getDirection() != 0) {
//            float pct = ofMap(mouseX, 0, ofGetWidth(), 0, 1);
//        float pct = ofMap(sin(ofGetElapsedTimef()), -1, 1, 0, 1);
        ofPolyline newline;
        for (int i = 0; i < singleline.getVertices().size(); i++) {
//            float pct = ofNoise(.001 * i, ofGetElapsedTimef());
            float pct = faceFoundPct; //ofMap(sin(2 * PI * ofNoise(0.0001 * i) + ofGetElapsedTimef()), -1, 1, 0, 1);
            ofPoint facept = singleline.getVertices()[i];
            ofPoint linept = ofPoint(ofMap(i, 0, singleline.getVertices().size(), 0, ofGetWidth()), ofGetHeight()/2);
            newline.addVertex(pct*facept + (1 - pct)*linept);
        }
        singleline = newline;
        }


    lerpline.clear(); //necessary because lerpline is a global variable now
    lerpline.addVertices(singleline.getVertices());
    //now you can play with lerp-related things! like this... -->
        
        if(dragPos.x != 0) {
            singleline = movePointsByLerp(singleline, findex, dragPos.x, dragPos.y);
        }


//    singleline = addPerpendiculars(singleline, 30, 5, 40);
    //singleline = dropPoints(singleline, mouseX, mouseY);
//    singleline = melt(singleline, mouseX, velocity);

        
// * draw that shit * //
// -----> as points:
//      for (int i =0; i < singleline.getVertices().size(); i++) {
//          ofPoint p = singleline.getVertices()[i];
//          ofDrawCircle(p, 3);
//      }
// -----> as one line:
        singleline.getResampledByCount(200);
        singleline = singleline.getSmoothed(5);
        
        
        ofRectangle camRect(0,0,cam.getWidth(),cam.getHeight());
        ofRectangle screenRect(0,0,ofGetWidth(), ofGetHeight());
        camRect.alignTo(screenRect);
        ofPushMatrix();
        ofTranslate(camRect.getPosition());
        ofTranslate(singleline.getCentroid2D().x, singleline.getCentroid2D().y);
        ofScale(faceSizeScale, faceSizeScale);
        ofTranslate(-singleline.getCentroid2D().x, -singleline.getCentroid2D().y);
        ofSetLineWidth(2);
        singleline.draw();
        ofPopMatrix();
    }
    
    
    if (tracker.getInstances().size() == 0) {
        velocity = 0;
            //            float pct = ofMap(mouseX, 0, ofGetWidth(), 0, 1);
            //        float pct = ofMap(sin(ofGetElapsedTimef()), -1, 1, 0, 1);
            ofPolyline newline;
            for (int i = 0; i < singlelinePreLineFade.getVertices().size(); i++) {
                //            float pct = ofNoise(.001 * i, ofGetElapsedTimef());
                float pct = faceFoundPct; //ofMap(sin(2 * PI * ofNoise(0.0001 * i) + ofGetElapsedTimef()), -1, 1, 0, 1);
                ofPoint facept = singlelinePreLineFade.getVertices()[i];
                ofPoint linept = ofPoint(ofMap(i, 0, singlelinePreLineFade.getVertices().size(), 0, ofGetWidth()), ofGetHeight()/2);
                newline.addVertex(pct*facept + (1 - pct)*linept);
            }
        baseline = newline;
        ofRectangle camRect(0,0,cam.getWidth(),cam.getHeight());
        ofRectangle screenRect(0,0,ofGetWidth(), ofGetHeight());
        camRect.alignTo(screenRect);
        ofPushMatrix();
        ofTranslate(camRect.getPosition());
        ofTranslate(baseline.getCentroid2D().x, baseline.getCentroid2D().y);
        ofScale(mouseY/ 100, mouseY/100);
        ofTranslate(-baseline.getCentroid2D().x, -baseline.getCentroid2D().y);
        ofSetLineWidth(2);
        baseline.draw();
        ofPopMatrix();
        faceFoundPct = faceFoundPct * 0.98 + 0.02 * 0.0;
        
        
//        ofRectangle camRect(0,0,cam.getWidth(),cam.getHeight());
//        ofRectangle screenRect(0,0,ofGetWidth(), ofGetHeight());
//        camRect.alignTo(screenRect);
//        ofPushMatrix();
//        ofTranslate(camRect.getPosition());
//        ofTranslate(lastFace.getCentroid2D().x, lastFace.getCentroid2D().y);
//        ofScale(mouseY/ 100, mouseY/100);
//        ofTranslate(-lastFace.getCentroid2D().x, -lastFace.getCentroid2D().y);
//        for (int i = 0; i < lastFace.size(); i++) {
//            lastFace[i].x += sin(i);
//        }
//        lastFace.draw();
//        ofPopMatrix();

    }
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'a'){
        img.grabScreen(0, 0 , ofGetWidth(), ofGetHeight());
        img.save("screenshot" + ofGetTimestampString() + ".png");
    }
    if(key == 'b') {
        if (faceSizeScale < 7) {
            faceSizeScale += .5;
        }
    }
    if(key == 'c') {
        if (faceSizeScale > 0) {
            faceSizeScale -= .5;
        }
    }
}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    dragPos.x = x;
    dragPos.y = y;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    ofVec2f mousePos = ofVec2f(x, y);

    findex = lerpline.getLerpIndexAtClosestPoint(mousePos);
    
//    cout << findex << endl;

//    unsigned int index;
//    actualLinePosition = lerpline.getClosestPoint(mousePos, &index);
//    lengthOfLineAtVertex = lerpline.getLengthAtIndex(int(index));
//    vertexPosition = lerpline.getPointAtIndexInterpolated(float(index));
//
//    lengthOfLineAtPoint = lerpline.getLengthAtIndexInterpolated(findex);
//    linePercent = lengthOfLineAtPoint/lerpline.getPerimeter();
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}






// * gravity * //
//        if (velocities.size() == 0) {
//            for (int i = 0; i < singleline.size(); i++) {
//                velocities.push_back(ofPoint(0,0));
//            }
//        }
//        now we have a list of velocities!
//

// * OG * //
//        singleline = singleline.getResampledBySpacing(1);
//        vector < ofPoint > points;
//        points = singleline.getVertices();
//        for (int i = 0; i < singleline.size(); i++) {
//            float ratio = (float)i/singleline.size();
//            float angle = 0;
//            ofPoint p = points[i];
//            if (i != singleline.size()) {
//                ofPoint p2 = points[i+1];
//                angle = p2.angleRad(p);
//            }

// * old gravity * //
//            ofPoint v = velocities[i];
//            v.y += 1.83;
//            p += v;g33333
//            if (p.y < 0 || p.y > ofGetHeight()) {
//                v.y = -v.y * 0.5;
//            }
//
//            velocities[i] = v;

// * messing with angles & sine * //
//          p.x += cos(angle * mouseX) * sin(ratio) * mouseY;
//          p.y += sin(angle * mouseX) * sin(ratio) * mouseY;
//          p.x += sin(ratio * mouseX)*mouseY;
//            points[i] = p;
//        }
//        singleline.clear();
//        singleline.addVertices(points);




