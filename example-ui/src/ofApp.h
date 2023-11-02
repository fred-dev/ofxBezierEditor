#pragma once

#include "ofMain.h"
#include "ofxBezierEditor.h"
#include "ofxGui.h"
#include "ofxBranchesPrimitive.h"

enum OperationMode {
    POLYLINE_MODE,
    FAT_LINE_MODE,
    TUBE_MESH_MODE,
};

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
    
    ofxBranchesPrimitive tube;
    ofxBezierEditor myBezier;
    bool bdrawBezierInfo;
        
    ofEasyCam cam;
    ofLight light;
    int mode = POLYLINE_MODE;
    
    ofxPanel gui;
    
    ofParameter<bool> fillToggle;
    ofParameter<bool> closedToggle;
    ofParameter<ofColor> fillColor;
    ofParameter<ofColor> strokeColor;
    ofParameter<float> ribbonWidthSlider;
    ofParameter<int> ribbonPrecisionSlider;
    ofParameter<float> tubeRadiusSlider;
    ofParameter<int> tubePrecisionSlider;

    
    void fillToggleChanged(bool &value);
    void closedToggleChanged(bool &value);
    void fillColorChanged(ofColor &value);
    void strokeColorChanged(ofColor &value);
    
    void ribbonWidthSliderChanged(float &value);
    void ribbonWidthSliderChnaged(float &value);
    void ribbonPrecisionSliderChanged(int &value);
    void tubeRadiusSliderChanged(float &value);
    void tubePrecisionSliderChanged(int &value);
    

    
    
};
