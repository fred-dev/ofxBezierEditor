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
    gui.add(meshLengthPrecisionSlider.set("Ribbon Precision", 5, 1, 50));
    gui.add(tubeRadiusSlider.set("Tube Radius", 10.0, 1.0, 150.0));
    gui.add(tubePrecisionSlider.set("Tube Precision", 10, 5, 50));
    gui.add(wireframeToggle.set("Draw wireframe", false));
    gui.add(drawNormalsToggle.set("Draw normals", false));
    gui.add(useRoundCapsToggle.set("Use round caps", true));
    
    
    // Add listener for GUI events
    fillToggle.addListener(this, &ofApp::fillToggleChanged);
    closedToggle.addListener(this, &ofApp::closedToggleChanged);
    fillColor.addListener(this, &ofApp::fillColorChanged);
    strokeColor.addListener(this, &ofApp::strokeColorChanged);
    ribbonWidthSlider.addListener(this, &ofApp::ribbonWidthSliderChanged);
    meshLengthPrecisionSlider.addListener(this, &ofApp::meshLengthPrecisionSliderChanged);
    tubeRadiusSlider.addListener(this, &ofApp::tubeRadiusSliderChanged);
    tubePrecisionSlider.addListener(this, &ofApp::tubePrecisionSliderChanged);
    useRoundCapsToggle.addListener(this, &ofApp::useRoundCapsToggleChanged);
    
    
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

            if(wireframeToggle){
                myBezier.getRibbonMesh().drawWireframe();
            }
            else{
                myBezier.getRibbonMesh().draw();
            }
            
        }
        
        else if (mode == TUBE_MESH_MODE) {
            ofEnableDepthTest();
            cam.begin();
            light.enable();
            ofScale(1, -1);
            ofTranslate(-ofGetWidth() / 2, -ofGetHeight() / 2);
            ofPushStyle();
            ofSetColor(0,255,0);
            myBezier.draw();
            ofPopStyle();
            ofSetColor(myBezier.getColorStroke());

            if(wireframeToggle){
                // Draw the tube mesh
                myBezier.getTubeMesh().drawWireframe();
            }
            else{
                // Draw the tube mesh
                myBezier.getTubeMesh().draw();
            }
           
            if(drawNormalsToggle){
                ofPushStyle();
                ofSetColor(ofColor::green);
                
                // Draw the normals
                const float normalLength = 7.0f; // Length of the normal lines
                auto mesh = myBezier.getTubeMesh(); // Get a reference to the mesh
                for (int i = 0; i < mesh.getNumVertices(); ++i) {
                    ofVec3f vertex = mesh.getVertex(i); // Get the vertex position
                    ofVec3f normal = mesh.getNormal(i) * normalLength; // Get the normal and scale it
                    ofDrawLine(vertex, vertex + normal); // Draw the line representing the normal
                }
                
                ofPopStyle();
            
            }

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

void ofApp::meshLengthPrecisionSliderChanged(int &value){
    myBezier.setMeshLengthPrecisionMultiplier(value);
}

void ofApp::tubeRadiusSliderChanged(float &value){
    myBezier.setTubeRadius(value);
}

void ofApp::tubePrecisionSliderChanged(int &value){
    myBezier.setTubeResolution(value);
}
void ofApp::useRoundCapsToggleChanged(bool &value){
    myBezier.setHasRoundCaps(value);

}
