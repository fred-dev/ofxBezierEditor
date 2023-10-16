#pragma once

#include "ofMain.h"
#include "ofxBezierEditor.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{
    
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
    void exit();
    
    
    ofxBezierEditor myBezier;
    bool bdrawBezierInfo;
    
    ofxPanel gui;
    
    ofParameter<bool> fillToggle;
    ofParameter<float> strokeSlider;
    ofParameter<ofColor> fillColor;
    ofParameter<ofColor> strokeColor;
    
    void fillToggleChanged(bool &value);
    void strokeSliderChanged(float &value);
    void fillColorChanged(ofColor &value);
    void strokeColorChanged(ofColor &value);
    
    
};
