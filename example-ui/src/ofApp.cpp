#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    myBezier.loadPoints("ofxBezierInfo.json");
    bdrawBezierInfo = false;
    
    ofSetSmoothLighting(true);
    
    light.setDiffuseColor(ofColor(0.f, 255.f, 0.f));
    light.setSpecularColor(ofColor(255.f, 255.f, 255.f));
    light.setPosition(0, 300, 0);
    cam.disableMouseInput();
    ofSetBackgroundColor(255);
    
    gui.setup();
    gui.setPosition(0, 0);
    
    // Add GUI elements for Bezier properties with default settings and ranges
    gui.add(fillToggle.set("Fill", false));
    gui.add(closedToggle.set("Closed", false));
    gui.add(fillColor.set("Fill Color", ofColor(128, 128, 128, 128), ofColor(0, 0), ofColor(255, 255)));
    gui.add(strokeColor.set("Stroke Color", ofColor(128, 128, 128, 128), ofColor(0, 0), ofColor(255, 255)));
    gui.add(ribbonWidthSlider.set("Ribbon width", 10.0, 1.0, 150.0));
    gui.add(ribbonPrecisionSlider.set("Ribbon Precision", 5, 1, 20));
    gui.add(tubeRadiusSlider.set("Tube Radius", 10.0, 1.0, 150.0));
    gui.add(tubePrecisionSlider.set("Tube Precision", 5, 1, 150));
    
    // Add listener for GUI events
    fillToggle.addListener(this, &ofApp::fillToggleChanged);
    closedToggle.addListener(this, &ofApp::closedToggleChanged);
    fillColor.addListener(this, &ofApp::fillColorChanged);
    strokeColor.addListener(this, &ofApp::strokeColorChanged);
    ribbonWidthSlider.addListener(this, &ofApp::ribbonWidthSliderChanged);
    ribbonPrecisionSlider.addListener(this, &ofApp::ribbonPrecisionSliderChanged);
    tubeRadiusSlider.addListener(this, &ofApp::tubeRadiusSliderChanged);
    tubePrecisionSlider.addListener(this, &ofApp::tubePrecisionSliderChanged);
    
    gui.loadFromFile("settings.xml");
}

//--------------------------------------------------------------
void ofApp::update(){
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    if(bdrawBezierInfo){
        myBezier.drawHelp();
    }
    else{
        if(mode == POLYLINE_MODE){
            myBezier.draw();
        }
        
        else if(mode == FAT_LINE_MODE){
            ofSetColor(myBezier.getColorStroke());
            myBezier.getRibbonMesh().drawWireframe();
        }
        
        else if(mode == TUBE_MESH_MODE){
            ofEnableDepthTest();
            cam.begin();
            light.enable();
            ofScale(1, -1);
            ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2);
            myBezier.getTubeMesh().draw();
            cam.end();
            ofDisableDepthTest();
        }
    }
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == '1'){
        mode = POLYLINE_MODE;
        cam.disableMouseInput();
        cam.reset();
    }
    else if(key == '2'){
        mode = FAT_LINE_MODE;
        cam.disableMouseInput();
        cam.reset();
    }
    else if(key == '3'){
        mode = TUBE_MESH_MODE;
        cam.reset();
        cam.enableMouseInput();
    }
    else if(key == 'h'){
        bdrawBezierInfo = !bdrawBezierInfo;
        myBezier.setReactToMouseAndKeyEvents(bdrawBezierInfo);
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}

//--------------------------------------------------------------
void ofApp::exit(){
}

void ofApp::fillToggleChanged(bool &value){
    myBezier.setFillBezier(value);
}

void ofApp::closedToggleChanged(bool &value){
    myBezier.setClosed(value);
}

void ofApp::ribbonWidthSliderChanged(float &value){
    myBezier.setRibbonWidth(value);
}

void ofApp::fillColorChanged(ofColor &value){
    myBezier.setColorFill(value);
}

void ofApp::strokeColorChanged(ofColor &value){
    myBezier.setColorStroke(value);
}

void ofApp::ribbonWidthSliderChnaged(float &value){
    myBezier.setRibbonWidth(value);
}

void ofApp::ribbonPrecisionSliderChanged(int &value){
    myBezier.setMeshPrecisionMultiplier(value);
}

void ofApp::tubeRadiusSliderChanged(float &value){
    myBezier.setTubeRadius(value);
}

void ofApp::tubePrecisionSliderChanged(int &value){
    myBezier.setTubeResolution(value);
}
