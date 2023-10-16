#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    myBezier.loadPoints("ofxBezierInfo.json");
    
    gui.setup();
    gui.setPosition(0, 0);
        
    // Add GUI elements for Bezier properties with default settings and ranges
    gui.add(fillToggle.set("Fill", false));
    gui.add(strokeSlider.set("Stroke", 1.0, 1.0, 11.0));
    gui.add(fillColor.set("Fill Color", ofColor(128, 128, 128, 128), ofColor(0, 0), ofColor(255, 255)));
    gui.add(strokeColor.set("Stroke Color", ofColor(128, 128, 128, 128), ofColor(0, 0), ofColor(255, 255)));
    
    // Add listener for GUI events
    fillToggle.addListener(this, &ofApp::fillToggleChanged);
    strokeSlider.addListener(this, &ofApp::strokeSliderChanged);
    fillColor.addListener(this, &ofApp::fillColorChanged);
    strokeColor.addListener(this, &ofApp::strokeColorChanged);
    
    gui.loadFromFile("settings.xml");
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    myBezier.draw();
    if(bdrawBezierInfo){
        myBezier.drawHelp();
    }
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'h'){
        bdrawBezierInfo = !bdrawBezierInfo;
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

void ofApp::strokeSliderChanged(float &value){
    myBezier.setStrokeWeight(value);
}

void ofApp::fillColorChanged(ofColor &value){
    myBezier.setColorFill(value);
}

void ofApp::strokeColorChanged(ofColor &value){
    myBezier.setColorStroke(value);
}
