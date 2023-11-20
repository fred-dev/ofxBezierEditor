#include "ofxBezierEditor.h"

ofxBezierEditor::ofxBezierEditor(){
    
    setRadiusVertex(8);
    setRadiusControlPoints(6);
    
    currentPointToMove = 0;
    lastVertexSelected = 0;
    
    draggableVertex vtx;
    curveVertices.assign(0, vtx);
    controlPoint1.assign(0, vtx);
    controlPoint2.assign(0, vtx);
    
    
    bfillBezier = true;
    colorFill = ofColor(188,4,62, 100);
    colorStroke = ofColor(2,189,190, 100);
    
    boundingBox.set(0,0,0,0);
    bshowBoundingBox = false;
    center.set(0,0);
    
    beditBezier = false;
    
    
    jsonFileName = "ofxBezierInfo.json";
    
    setReactToMouseAndKeyEvents(true);
    polyLineFromPoints.setClosed(false);
}


//--------------------------------------------------------------
void ofxBezierEditor::drawWithNormals(const ofPolyline& polyline) {
    
    ofPushStyle(); // Push the current style settings
    for (int i = 0; i < polyline.size(); i++) {
        const ofPoint& cur = polyline[i];
        const ofPoint& normal = polyline.getNormalAtIndex(i);
        
        ofPushMatrix();
        ofTranslate(cur.x, cur.y, 0);
        
        ofSetColor(255, 0, 255); // Set color before drawing lines
        
        ofDrawLine(0, 0, normal.x * 20, normal.y * 20); // Draw normals
        
        ofPopMatrix();
    }
    
    
}

ofxBezierEditor::~ofxBezierEditor(){
    setReactToMouseAndKeyEvents(false);
}



//--------------------------------------------------------------
void ofxBezierEditor::setReactToMouseAndKeyEvents(bool b){
    if(b == true){
        ofRegisterMouseEvents(this);
        ofRegisterKeyEvents(this);
    }
    else{
        ofUnregisterMouseEvents(this);
        ofUnregisterKeyEvents(this);
    }
}

void ofxBezierEditor::registerToEvents(){
    ofRegisterMouseEvents(this);
    ofRegisterKeyEvents(this);
}
void ofxBezierEditor::unregisterFromEvents(){
    ofUnregisterMouseEvents(this);
    ofUnregisterKeyEvents(this);
}
//--------------------------------------------------------------
void ofxBezierEditor::loadPoints(string filename){
    ofLogVerbose("ofxBezierEditor") << "loadPoints: " << filename;
    jsonFileName = filename;
    
    ofFile jsonFile(jsonFileName);
    if (jsonFile.exists()) {
        jsonFile >> JSONBezier;
        // Read data from JSON
        bUseRibbonMesh = JSONBezier["bezier"]["useRibbonMesh"].get<bool>();
        ofLogVerbose("ofxBezierEditor") << "bUseRibbonMesh: " << bUseRibbonMesh;
        ribbonWidth = JSONBezier["bezier"]["ribbonWidth"].get<float>();
        ofLogVerbose("ofxBezierEditor") << "ribbonWidth: " << ribbonWidth;
        meshLengthPrecisionMultiplier = JSONBezier["bezier"]["meshLengthPrecision"].get<int>();
        ofLogVerbose("ofxBezierEditor") << "meshLengthPrecisionMultiplier: " << meshLengthPrecisionMultiplier;
        
        bUseTubeMesh = JSONBezier["bezier"]["useTubeMesh"].get<bool>();
        ofLogVerbose("ofxBezierEditor") << "bUseTubeMesh: " << bUseTubeMesh;
        tubeRadius = JSONBezier["bezier"]["tubeRadius"].get<float>();
        ofLogVerbose("ofxBezierEditor") << "tubeRadius: " << tubeRadius;
        tubeResolution = JSONBezier["bezier"]["tubeResolution"].get<int>();
        ofLogVerbose("ofxBezierEditor") << "tubeResolution: " << tubeResolution;
        
        
        bIsClosed = JSONBezier["bezier"]["closed"].get<bool>();
        ofLogVerbose("ofxBezierEditor") << "bIsClosed: " << bIsClosed;
        
        polyLineFromPoints.setClosed(bIsClosed);
        bfillBezier = JSONBezier["bezier"]["fill"].get<bool>();
        colorFill.set(
                      JSONBezier["bezier"]["colorFill"]["r"].get<int>(),
                      JSONBezier["bezier"]["colorFill"]["g"].get<int>(),
                      JSONBezier["bezier"]["colorFill"]["b"].get<int>(),
                      JSONBezier["bezier"]["colorFill"]["a"].get<int>()
                      );
        
        colorStroke.set(
                        JSONBezier["bezier"]["colorStroke"]["r"].get<int>(),
                        JSONBezier["bezier"]["colorStroke"]["g"].get<int>(),
                        JSONBezier["bezier"]["colorStroke"]["b"].get<int>(),
                        JSONBezier["bezier"]["colorStroke"]["a"].get<int>()
                        );
        
        
        curveVertices.clear();
        for (const auto& vertexJson : JSONBezier["bezier"]["vertices"]) {
            draggableVertex vtx;
            vtx.pos.x = vertexJson["x"].get<float>();
            vtx.pos.y = vertexJson["y"].get<float>();
            vtx.bOver = false;
            vtx.bBeingDragged = false;
            vtx.bBeingSelected = false;
            curveVertices.push_back(vtx);
        }
        
        // Read control points from JSON
        controlPoint1.clear();
        for (const auto& cpJson : JSONBezier["bezier"]["cp1"]) {
            draggableVertex cp;
            cp.pos.x = cpJson["x"].get<float>();
            cp.pos.y = cpJson["y"].get<float>();
            cp.bOver = false;
            cp.bBeingDragged = false;
            controlPoint1.push_back(cp);
        }
        
        controlPoint2.clear();
        for (const auto& cpJson : JSONBezier["bezier"]["cp2"]) {
            draggableVertex cp;
            cp.pos.x = cpJson["x"].get<float>();
            cp.pos.y = cpJson["y"].get<float>();
            cp.bOver = false;
            cp.bBeingDragged = false;
            controlPoint2.push_back(cp);
        }
        
        
        
    } else {
        ofLogVerbose() << "ofxBezierEditor::loadPoints(): File does not exist.";
    }
    updateAllFromVertices();
}

//--------------------------------------------------------------
void ofxBezierEditor::savePoints(string filename){
    JSONBezier.clear();
    
    // Create the JSON structure
    JSONBezier["bezier"]["fill"] = bfillBezier;
    JSONBezier["bezier"]["closed"] = bIsClosed;
    
    JSONBezier["bezier"]["useRibbonMesh"] = bUseRibbonMesh;
    JSONBezier["bezier"]["meshLengthPrecision"] =  meshLengthPrecisionMultiplier;
    JSONBezier["bezier"]["ribbonWidth"] = ribbonWidth;
    
    JSONBezier["bezier"]["useTubeMesh"] = bUseTubeMesh;
    JSONBezier["bezier"]["tubeRadius"] = tubeRadius;
    JSONBezier["bezier"]["tubeResolution"] = tubeResolution;
    
    JSONBezier["bezier"]["colorFill"]["r"] = colorFill.r;
    JSONBezier["bezier"]["colorFill"]["g"] = colorFill.g;
    JSONBezier["bezier"]["colorFill"]["b"] = colorFill.b;
    JSONBezier["bezier"]["colorFill"]["a"] = colorFill.a;
    
    JSONBezier["bezier"]["colorStroke"]["r"] = colorStroke.r;
    JSONBezier["bezier"]["colorStroke"]["g"] = colorStroke.g;
    JSONBezier["bezier"]["colorStroke"]["b"] = colorStroke.b;
    JSONBezier["bezier"]["colorStroke"]["a"] = colorStroke.a;
    
    for (int i = 0; i < curveVertices.size(); i++) {
        JSONBezier["bezier"]["vertices"][i]["x"] = curveVertices.at(i).pos.x ;
        JSONBezier["bezier"]["vertices"][i]["y"] = curveVertices.at(i).pos.y ;
    }
    
    for (int i = 0; i < controlPoint1.size(); i++) {
        JSONBezier["bezier"]["cp1"][i]["x"] = controlPoint1.at(i).pos.x ;
        JSONBezier["bezier"]["cp1"][i]["y"] = controlPoint1.at(i).pos.y ;
    }
    
    for (int i = 0; i < controlPoint2.size(); i++) {
        JSONBezier["bezier"]["cp2"][i]["x"] = controlPoint2.at(i).pos.x ;
        JSONBezier["bezier"]["cp2"][i]["y"] = controlPoint2.at(i).pos.y ;
    }
    
    // Save JSON to a file
    ofSavePrettyJson(filename, JSONBezier);
    
}

//--------------------------------------------------------------
void ofxBezierEditor::draw(){
    
    if(curveVertices.size() > 0){
        ofPushMatrix();
        if(bfillBezier && bIsClosed){
            ofFill();
            ofSetColor(colorFill);
            ofBeginShape();
            for (int i = 0; i < curveVertices.size(); i++){
                if (i == 0){
                    ofVertex(curveVertices.at(0).pos.x, curveVertices.at(0).pos.y); // we need to duplicate 0 for the curve to start at point 0
                }
                else {
                    ofBezierVertex(controlPoint1.at(i).pos.x, controlPoint1.at(i).pos.y, controlPoint2.at(i).pos.x, controlPoint2.at(i).pos.y, curveVertices.at(i).pos.x, curveVertices.at(i).pos.y);
                }
            }
            
            ofBezierVertex(controlPoint1.at(0).pos.x, controlPoint1.at(0).pos.y, controlPoint2.at(0).pos.x, controlPoint2.at(0).pos.y, curveVertices.at(0).pos.x, curveVertices.at(0).pos.y);
            
            ofEndShape(bIsClosed);
        }
        
        
        ofSetColor(colorStroke);
        ofNoFill();
        ofBeginShape();
        for (int i = 0; i < curveVertices.size(); i++){
            if (i == 0){
                ofVertex(curveVertices.at(0).pos.x, curveVertices.at(0).pos.y); // we need to duplicate 0 for the curve to start at point 0
            }
            else {
                ofBezierVertex(controlPoint1.at(i).pos.x, controlPoint1.at(i).pos.y, controlPoint2.at(i).pos.x, controlPoint2.at(i).pos.y, curveVertices.at(i).pos.x, curveVertices.at(i).pos.y);
            }
        }
        if(bIsClosed){
            
            ofBezierVertex(controlPoint1.at(0).pos.x, controlPoint1.at(0).pos.y, controlPoint2.at(0).pos.x, controlPoint2.at(0).pos.y, curveVertices.at(0).pos.x, curveVertices.at(0).pos.y);
        }
        ofEndShape(bIsClosed);
        ofPopMatrix();
    }
    
    ofPopStyle(); // Restore the previous style settings
    
}

//--------------------------------------------------------------
void ofxBezierEditor::drawOutline(){
    
    if(curveVertices.size() > 0){
        
        ofSetColor(colorStroke);
        ofNoFill();
        ofBeginShape();
        for (int i = 0; i < curveVertices.size(); i++){
            if (i == 0){
                ofVertex(curveVertices.at(0).pos.x, curveVertices.at(0).pos.y); // we need to duplicate 0 for the curve to start at point 0
            }
            else {
                ofBezierVertex(controlPoint1.at(i).pos.x, controlPoint1.at(i).pos.y, controlPoint2.at(i).pos.x, controlPoint2.at(i).pos.y, curveVertices.at(i).pos.x, curveVertices.at(i).pos.y);
            }
        }
        if(bIsClosed){
            ofBezierVertex(controlPoint1.at(0).pos.x, controlPoint1.at(0).pos.y, controlPoint2.at(0).pos.x, controlPoint2.at(0).pos.y, curveVertices.at(0).pos.x, curveVertices.at(0).pos.y);
        }
        ofEndShape(bIsClosed);
        ofPopMatrix();
    }
    
}

//--------------------------------------------------------------
void ofxBezierEditor::drawHelp(){
    ofSetBackgroundColor(200);
    if(curveVertices.size() > 0){
        draw();
        
        drawWithNormals(polyLineFromPoints);
        ofPushMatrix();
        ofSetLineWidth(1);
        ofNoFill();
        ofSetColor(0,0,0,40);
        for (int i = 0; i < curveVertices.size(); i++){
            ofDrawLine(curveVertices.at(i).pos.x, curveVertices.at(i).pos.y, controlPoint2.at(i).pos.x, controlPoint2.at(i).pos.y);
        }
        for (int i = 1; i < curveVertices.size(); i++){
            ofDrawLine(curveVertices.at(i-1).pos.x, curveVertices.at(i-1).pos.y, controlPoint1.at(i).pos.x, controlPoint1.at(i).pos.y);
        }
        ofDrawLine(curveVertices.at(curveVertices.size()-1).pos.x, curveVertices.at(curveVertices.size()-1).pos.y, controlPoint1.at(0).pos.x, controlPoint1.at(0).pos.y); // =
        
        
        for (int i = 0; i < curveVertices.size(); i++){
            ofSetColor(vertexColour);
            ofNoFill();
            if(curveVertices.at(i).bOver == true){
                ofSetColor(vertexHoverColor);
                ofFill();
            }
            if(curveVertices.at(i).bBeingDragged == true){
                ofSetColor(vertexDraggedColour);
                ofFill();
            }
            ofDrawCircle(curveVertices.at(i).pos.x, curveVertices.at(i).pos.y, radiusVertex);
            if(curveVertices.at(i).bBeingSelected == true){
                ofSetColor(vertexSelectedColour);
                ofFill();
                ofDrawCircle(curveVertices.at(i).pos.x, curveVertices.at(i).pos.y, radiusVertex);
                ofNoFill();
                ofDrawCircle(curveVertices.at(i).pos.x, curveVertices.at(i).pos.y, 2*radiusVertex);
            }
            ofSetColor(vertexLabelColour);
            ofDrawBitmapString("v_" + ofToString(i), curveVertices.at(i).pos.x+3, curveVertices.at(i).pos.y+3);
        }
        
        
        for (int i = 0; i < controlPoint1.size(); i++){
            ofSetColor(ctrPtColour);
            ofNoFill();
            if(controlPoint1.at(i).bOver == true){
                ofSetColor(ctrPtHoverColor);
                ofFill();
            }
            if(controlPoint1.at(i).bBeingDragged == true){
                ofSetColor(ctrPtDraggedColour);
                ofFill();
            }
            ofDrawCircle(controlPoint1.at(i).pos.x, controlPoint1.at(i).pos.y, radiusControlPoints);
            ofSetColor(ctrPtLabelColour);
            ofDrawBitmapString("cp1_" + ofToString(i), controlPoint1.at(i).pos.x+3, controlPoint1.at(i).pos.y+3);
        }
        
        for (int i = 0; i < controlPoint2.size(); i++){
            ofSetColor(ctrPtColour);
            ofNoFill();
            if(controlPoint2.at(i).bOver == true){
                ofSetColor(ctrPtHoverColor);
                ofFill();
            }
            if(controlPoint2.at(i).bBeingDragged == true){
                ofSetColor(ctrPtDraggedColour);
                ofFill();
            }
            ofDrawCircle(controlPoint2.at(i).pos.x, controlPoint2.at(i).pos.y, radiusControlPoints);
            ofSetColor(ctrPtLabelColour);
            ofDrawBitmapString("cp2_" + ofToString(i), controlPoint2.at(i).pos.x+3, controlPoint2.at(i).pos.y+3);
        }
        
        int range = currentPointToMove / curveVertices.size();
        int mod = currentPointToMove % curveVertices.size();
        ofNoFill();
        if(range == 0){
            ofSetColor(255,255,0);
            ofDrawCircle(curveVertices.at(mod).pos.x, curveVertices.at(mod).pos.y, 2*radiusControlPoints);
            ofSetColor(255,255,0);
            ofDrawBitmapString("fine tune with arrows", curveVertices.at(mod).pos.x+3, curveVertices.at(mod).pos.y+3);
        }
        else if(range == 1){
            ofSetColor(255,0,255);
            ofDrawCircle(controlPoint1.at(mod).pos.x, controlPoint1.at(mod).pos.y, 2*radiusControlPoints);
            ofSetColor(255,0,255);
            ofDrawBitmapString("fine tune with arrows", controlPoint1.at(mod).pos.x+3, controlPoint1.at(mod).pos.y+3);
        }
        else if(range == 2){
            ofSetColor(255,0,0);
            ofDrawCircle(controlPoint2.at(mod).pos.x, controlPoint2.at(mod).pos.y, 2*radiusControlPoints);
            ofSetColor(255,0,0);
            ofDrawBitmapString("fine tune with arrows", controlPoint2.at(mod).pos.x+3, controlPoint2.at(mod).pos.y+3);
        }
        
        if(bshowBoundingBox){
            ofFill();
            ofSetColor(10,250,255,150);
            ofDrawRectangle(boundingBox);
            ofNoFill();
            ofSetLineWidth(2);
            ofSetColor(10,250,255);
            ofDrawRectangle(boundingBox);
        }
        ofPopMatrix();
        
        
        
    }
    
    ofSetColor(0,0,0);
    ofDrawBitmapString("VERTEX: " + ofToString(curveVertices.size()) + "PRESS e to EDIT the BEZIER: " + ofToString(beditBezier), 20,20);
    ofDrawBitmapString("mouse left button to add a point at the end", 20,40);
    ofDrawBitmapString("backspace to delete last point added", 20,60);
    ofDrawBitmapString("drag mouse to move vertex and control points", 20,80);
    ofDrawBitmapString("mouse right button to select two vertex", 20,100);
    ofDrawBitmapString("mouse right button to add a new vertex between the two selected vertex", 20,120);
    ofDrawBitmapString("supr to delete last vertex added", 20,140);
    ofDrawBitmapString("currentPointToMove [n++|m--]: " + ofToString(currentPointToMove) + "\n fine tune with arrows", 20,160);
    ofDrawBitmapString("[l] load stored bezier", 20,200);
    ofDrawBitmapString("[s] save current bezier", 20,220);
    ofDrawBitmapString("[f] toogle fill: " + ofToString(bfillBezier), 20,240);
    ofDrawBitmapString("[b] show/hide bounding box: " + ofToString(bshowBoundingBox), 20,260);
    ofDrawBitmapString("drag bounding box to move all the bezier", 20,280);
    ofDrawBitmapString("COLORS:\nfill " + ofToString((float)colorFill.r) + "(r) " + ofToString((float)colorFill.g) + "(g) " + ofToString((float)colorFill.b) + "(b)" + "\nstroke " + ofToString((float)colorStroke.r) + "(r) " + ofToString((float)colorStroke.g) + "(g) " + ofToString((float)colorStroke.b) + "(b)", 20,300);
    
}

//--------------------------------------------------------------
void ofxBezierEditor::mouseMoved(ofMouseEventArgs &args){
    if(beditBezier == true){
        for (int i = 0; i < curveVertices.size(); i++){
            float diffx = args.x - curveVertices.at(i).pos.x;
            float diffy = args.y - curveVertices.at(i).pos.y;
            float dist = sqrt(diffx*diffx + diffy*diffy);
            if (dist < radiusVertex){
                curveVertices.at(i).bOver = true;
            } else {
                curveVertices.at(i).bOver = false;
            }
        }
        
        for (int i = 0; i < controlPoint1.size(); i++){
            float diffx = args.x - controlPoint1.at(i).pos.x;
            float diffy = args.y - controlPoint1.at(i).pos.y;
            float dist = sqrt(diffx*diffx + diffy*diffy);
            if (dist < radiusControlPoints){
                controlPoint1.at(i).bOver = true;
            } else {
                controlPoint1.at(i).bOver = false;
            }
        }
        
        for (int i = 0; i < controlPoint2.size(); i++){
            float diffx = args.x - controlPoint2.at(i).pos.x;
            float diffy = args.y - controlPoint2.at(i).pos.y;
            float dist = sqrt(diffx*diffx + diffy*diffy);
            if (dist < radiusControlPoints){
                controlPoint2.at(i).bOver = true;
            } else {
                controlPoint2.at(i).bOver = false;
            }
        }
    }
}

//--------------------------------------------------------------
void ofxBezierEditor::mouseDragged(ofMouseEventArgs &args){
    if(beditBezier){
        if (bshowBoundingBox) {
            int deltaX = args.x - mouseX;
            int deltaY = args.y - mouseY;
            
            for (int i = 0; i < curveVertices.size(); i++) {
                curveVertices.at(i).pos.x += deltaX;
                curveVertices.at(i).pos.y += deltaY;
            }
            
            for (int i = 0; i < controlPoint1.size(); i++) {
                controlPoint1.at(i).pos.x += deltaX;
                controlPoint1.at(i).pos.y += deltaY;
            }
            
            for (int i = 0; i < controlPoint2.size(); i++) {
                controlPoint2.at(i).pos.x += deltaX;
                controlPoint2.at(i).pos.y += deltaY;
            }
            
            mouseX = args.x;
            mouseY = args.y;
        }
        
        for (int i = 0; i < curveVertices.size(); i++){
            if (curveVertices.at(i).bBeingDragged == true){
                curveVertices.at(i).pos.x = args.x ;
                curveVertices.at(i).pos.y = args.y ;
            }
        }
        
        for (int i = 0; i < controlPoint1.size(); i++){
            if (controlPoint1.at(i).bBeingDragged == true){
                controlPoint1.at(i).pos.x = args.x ;
                controlPoint1.at(i).pos.y = args.y ;
            }
        }
        
        for (int i = 0; i < controlPoint2.size(); i++){
            if (controlPoint2.at(i).bBeingDragged == true){
                controlPoint2.at(i).pos.x = args.x ;
                controlPoint2.at(i).pos.y = args.y ;
            }
        }
        updateAllFromVertices();
    }
    
}

//--------------------------------------------------------------
void ofxBezierEditor::mousePressed(ofMouseEventArgs &args){
    if(beditBezier){
        if(args.button == OF_MOUSE_BUTTON_LEFT){
            if(bshowBoundingBox){
                mouseX = args.x ;
                mouseY = args.y ;
            }
            else{
                bool bAnyVertexDragged = false;
                // MOVE vertex
                for (int i = 0; i < curveVertices.size(); i++){
                    float diffx = args.x  - curveVertices.at(i).pos.x;
                    float diffy = args.y  - curveVertices.at(i).pos.y;
                    float dist = sqrt(diffx*diffx + diffy*diffy);
                    if (dist < radiusVertex){
                        curveVertices.at(i).bBeingDragged = true;
                        bAnyVertexDragged = true;
                    } else {
                        curveVertices.at(i).bBeingDragged = false;
                    }
                }
                
                for (int i = 0; i < controlPoint1.size(); i++){
                    float diffx = args.x  - controlPoint1.at(i).pos.x;
                    float diffy = args.y  - controlPoint1.at(i).pos.y;
                    float dist = sqrt(diffx*diffx + diffy*diffy);
                    if (dist < radiusControlPoints){
                        controlPoint1.at(i).bBeingDragged = true;
                        bAnyVertexDragged = true;
                    } else {
                        controlPoint1.at(i).bBeingDragged = false;
                    }
                }
                
                for (int i = 0; i < controlPoint2.size(); i++){
                    float diffx = args.x  - controlPoint2.at(i).pos.x;
                    float diffy = args.y  - controlPoint2.at(i).pos.y;
                    float dist = sqrt(diffx*diffx + diffy*diffy);
                    if (dist < radiusControlPoints){
                        controlPoint2.at(i).bBeingDragged = true;
                        bAnyVertexDragged = true;
                    } else {
                        controlPoint2.at(i).bBeingDragged = false;
                    }
                }
                
                // ADD vertex to the end
                if(bAnyVertexDragged == false){
                    draggableVertex vtx;
                    vtx.pos.x = args.x ;
                    vtx.pos.y = args.y ;
                    vtx.bOver             = true;
                    vtx.bBeingDragged     = false;
                    vtx.bBeingSelected  = false;
                    curveVertices.push_back(vtx);
                    
                    draggableVertex cp;
                    int nEnd = curveVertices.size()-1;
                    cp.pos.x = ofLerp(curveVertices.at(0).pos.x , curveVertices.at(nEnd).pos.x , 0.66);
                    cp.pos.y = ofLerp(curveVertices.at(0).pos.y , curveVertices.at(nEnd).pos.y , 0.66);
                    cp.bOver             = false;
                    cp.bBeingDragged     = false;
                    cp.bBeingSelected     = false;
                    controlPoint1.push_back(cp);
                    cp.pos.x = ofLerp(curveVertices.at(0).pos.x , curveVertices.at(nEnd).pos.x , 0.33);
                    cp.pos.y = ofLerp(curveVertices.at(0).pos.y , curveVertices.at(nEnd).pos.y , 0.33);
                    controlPoint2.push_back(cp);
                    
                    updateAllFromVertices();
                    
                }
            }
        }
        if(args.button == OF_MOUSE_BUTTON_RIGHT){
            // SELECT several vertex
            bool bAnyVertexSelected = false;
            for (int i = 0; i < curveVertices.size(); i++){
                float diffx = args.x  - curveVertices.at(i).pos.x;
                float diffy = args.y  - curveVertices.at(i).pos.y;
                float dist = sqrt(diffx*diffx + diffy*diffy);
                if (dist < radiusVertex){
                    curveVertices.at(i).bBeingSelected = !curveVertices.at(i).bBeingSelected;
                    bAnyVertexSelected = true;
                }
            }
            
            if(bAnyVertexSelected == false){
                int numVertexSelected = 0;
                lastVertexSelected = 0;
                for (int i = 0; i < curveVertices.size(); i++){
                    if(curveVertices.at(i).bBeingSelected == true){
                        numVertexSelected++;
                        lastVertexSelected = i;
                    }
                }
                // ADD vertex between two points
                if(numVertexSelected >= 2){
                    draggableVertex vtx;
                    vtx.pos.x = args.x ;
                    vtx.pos.y = args.y ;
                    vtx.bOver             = true;
                    vtx.bBeingDragged     = false;
                    vtx.bBeingSelected     = false;
                    curveVertices.insert(curveVertices.begin()+lastVertexSelected, vtx);
                    
                    draggableVertex cp;
                    cp.pos.x = ofLerp(curveVertices.at(lastVertexSelected-1).pos.x , curveVertices.at(lastVertexSelected).pos.x , 0.66);
                    cp.pos.y = ofLerp(curveVertices.at(lastVertexSelected-1).pos.y , curveVertices.at(lastVertexSelected).pos.y , 0.66);
                    cp.bOver             = false;
                    cp.bBeingDragged     = false;
                    cp.bBeingSelected     = false;
                    controlPoint1.insert(controlPoint1.begin()+lastVertexSelected,cp);
                    cp.pos.x = ofLerp(curveVertices.at(lastVertexSelected).pos.x , curveVertices.at(lastVertexSelected).pos.x , 0.33);
                    cp.pos.y = ofLerp(curveVertices.at(lastVertexSelected-1).pos.y , curveVertices.at(lastVertexSelected).pos.y , 0.33);
                    controlPoint2.insert(controlPoint2.begin()+lastVertexSelected,cp);
                    
                    updateAllFromVertices();
                }
            }
        }
    }
}

//--------------------------------------------------------------
void ofxBezierEditor::mouseReleased(ofMouseEventArgs &args){
    if(beditBezier){
        for (int i = 0; i < curveVertices.size(); i++){
            curveVertices.at(i).bBeingDragged = false;
        }
        for (int i = 0; i < controlPoint1.size(); i++){
            controlPoint1.at(i).bBeingDragged = false;
        }
        for (int i = 0; i < controlPoint2.size(); i++){
            controlPoint2.at(i).bBeingDragged = false;
        }
        
    }
}
//--------------------------------------------------------------
void ofxBezierEditor::mouseScrolled(ofMouseEventArgs &args){
}
//--------------------------------------------------------------
void ofxBezierEditor::mouseEntered(ofMouseEventArgs &args){
}
//--------------------------------------------------------------
void ofxBezierEditor::mouseExited(ofMouseEventArgs &args){
}
//--------------------------------------------------------------
void ofxBezierEditor::keyPressed(ofKeyEventArgs &args){
    if(args.key == 'e'){
        beditBezier = !beditBezier;
    }
    
    if(beditBezier){
        if(args.key == 's'){
            savePoints(jsonFileName);
        }
        
        else if(args.key == 'l'){
            loadPoints(jsonFileName);
        }
        else if(args.key == 'f'){
            bfillBezier = !bfillBezier;
        }
        else if(args.key == 'b'){
            bshowBoundingBox = !bshowBoundingBox;
        }
        else if(args.key == 'c'){
            bIsClosed = !bIsClosed;
            updatePolyline();
        }
        
        else if(args.key == 'n'){
            currentPointToMove++;
            if(currentPointToMove > curveVertices.size() + controlPoint1.size() + controlPoint2.size() -1){
                currentPointToMove = 0;
            }
        }
        else if(args.key == 'm'){
            currentPointToMove--;
            if(currentPointToMove < 0){
                currentPointToMove = curveVertices.size() + controlPoint1.size() + controlPoint2.size() -1;
            }
        }
        else if(args.key == OF_KEY_UP){
            int range = currentPointToMove / curveVertices.size();
            int mod = currentPointToMove % curveVertices.size();
            if(range == 0){
                curveVertices.at(mod).pos.y--;
            }
            else if(range == 1){
                controlPoint1.at(mod).pos.y--;
            }
            else if(range == 2){
                controlPoint2.at(mod).pos.y--;
            }
        }
        else if(args.key == OF_KEY_DOWN){
            int range = currentPointToMove / curveVertices.size();
            int mod = currentPointToMove % curveVertices.size();
            if(range == 0){
                curveVertices.at(mod).pos.y++;
            }
            else if(range == 1){
                controlPoint1.at(mod).pos.y++;
            }
            else if(range == 2){
                controlPoint2.at(mod).pos.y++;
            }
        }
        else if(args.key == OF_KEY_LEFT){
            int range = currentPointToMove / curveVertices.size();
            int mod = currentPointToMove % curveVertices.size();
            if(range == 0){
                curveVertices.at(mod).pos.x--;
            }
            else if(range == 1){
                controlPoint1.at(mod).pos.x--;
            }
            else if(range == 2){
                controlPoint2.at(mod).pos.x--;
            }
        }
        else if(args.key == OF_KEY_RIGHT){
            int range = currentPointToMove / curveVertices.size();
            int mod = currentPointToMove % curveVertices.size();
            if(range == 0){
                curveVertices.at(mod).pos.x++;
            }
            else if(range == 1){
                controlPoint1.at(mod).pos.x++;
            }
            else if(range == 2){
                controlPoint2.at(mod).pos.x++;
            }
        }
        if(args.key == OF_KEY_BACKSPACE){
            // REMOVE last vertex
            curveVertices.pop_back();
            controlPoint1.pop_back();
            controlPoint2.pop_back();
            
            updateAllFromVertices();
        }
        if(args.key == OF_KEY_DEL){
            // REMOVE last intermediate vertex added
            curveVertices.erase(curveVertices.begin()+lastVertexSelected);
            controlPoint1.erase(controlPoint1.begin()+lastVertexSelected);
            controlPoint2.erase(controlPoint2.begin()+lastVertexSelected);
            
            updateAllFromVertices();
        }
    }
}

//--------------------------------------------------------------
void ofxBezierEditor::keyReleased(ofKeyEventArgs &args){
    
}

//--------------------------------------------------------------
void ofxBezierEditor::updateBoundingBox(){
    boundingBox = polyLineFromPoints.getBoundingBox();
}

//--------------------------------------------------------------
void ofxBezierEditor::calculateCenter(){
    center.set(boundingBox.x + 0.5f*boundingBox.width, boundingBox.y + 0.5f*boundingBox.height);
}

//--------------------------------------------------------------
void ofxBezierEditor::updatePolyline(){
    polyLineFromPoints.clear();
    if(curveVertices.size() > 0){
        
        for (int i = 0; i < curveVertices.size(); i++){
            if (i == 0){
                polyLineFromPoints.addVertex(curveVertices.at(0).pos.x, curveVertices.at(0).pos.y); // we need to duplicate 0 for the curve to start at point 0
            }
            else{
                polyLineFromPoints.bezierTo(controlPoint1.at(i).pos.x, controlPoint1.at(i).pos.y, controlPoint2.at(i).pos.x, controlPoint2.at(i).pos.y, curveVertices.at(i).pos.x, curveVertices.at(i).pos.y);
            }
        }
        if(bIsClosed){
            polyLineFromPoints.bezierTo(controlPoint1.at(0).pos.x, controlPoint1.at(0).pos.y, controlPoint2.at(0).pos.x, controlPoint2.at(0).pos.y, curveVertices.at(0).pos.x, curveVertices.at(0).pos.y);
            polyLineFromPoints.setClosed(bIsClosed);
        }
        
    }
}

void ofxBezierEditor::generateTriangleStripFromPolyline(ofPolyline inPoly) {
    if (inPoly.getVertices().size() > 2) {
        // Clear mesh (triangle strip)
        ribbonMesh.clear();
        ribbonMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
        
        ofPolyline workerLine = inPoly;
        //lets add extra points at the start and end of the line using reflection so we get good normals for the actual start and end
        glm::vec3 firstPoint = workerLine.getVertices()[0];
        glm::vec3 secondPoint = workerLine.getVertices()[1];
        glm::vec3 reflectedFirst = firstPoint - (secondPoint - firstPoint);
        
        // Reflect the second-to-last point over the last for the end of the line
        glm::vec3 lastPoint = workerLine.getVertices()[workerLine.getVertices().size() - 1];
        glm::vec3 secondLastPoint = workerLine.getVertices()[workerLine.getVertices().size() - 2];
        glm::vec3 reflectedLast = lastPoint - (secondLastPoint - lastPoint);
        
        //add the firstPoint to the start of the line
        workerLine.insertVertex(reflectedFirst, 0);
        workerLine.addVertex(reflectedLast);
        
        // Create vectors to store points and tangents
        vector<ofVec3f> points;
        vector<ofVec2f> tangents;
        
        for (int i = 1; i < workerLine.size() - 1; i++) {
            points.push_back(ofVec3f(workerLine[i].x, workerLine[i].y, 0));
            ofVec2f tangent = workerLine.getTangentAtIndex(i);
            tangents.push_back(tangent);
            if (i < workerLine.size() - 2) {
                // Add intermediate points and tangents based on precisionMultiplier
                for (int j = 1; j < meshLengthPrecisionMultiplier; j++) {
                    float t = static_cast<float>(j) / static_cast<float>(meshLengthPrecisionMultiplier);
                    ofVec3f interpolatedPoint = workerLine.getPointAtIndexInterpolated(i + t);
                    ofVec2f interpolatedTangent = workerLine.getTangentAtIndexInterpolated(i + t);
                    points.push_back(interpolatedPoint);
                    tangents.push_back(interpolatedTangent);
                }
            }
            
        }
        std::vector<float> segmentDistances;
        
        float totalLineLength = 0;
        
        if(roundCap){
            segmentDistances.push_back(ribbonWidth/2);
            totalLineLength +=  ribbonWidth;
        }
        else{
            segmentDistances.push_back(0);
        }
        
        //get the distance between each point on the line
        for(int i = 0; i < points.size()-1; i++){
            float distance = points[i].distance(points[i+1]);
            
            segmentDistances.push_back(distance + segmentDistances.back());
            
            totalLineLength += distance;
        }
        
        if(roundCap){
            //get the poiunts on a hald circle for the cap, the circle centre is the first point and the direction is the tangent. There should be 20 pounts on the half circle.
            generateCurvedRibbonCap(points[0], tangents[0], true, totalLineLength);
        }
        
        for (int i = 0; i < points.size(); i++) {
            // Calculate the perpendicular vector
            ofVec2f perpendicular(-tangents[i].y, tangents[i].x); // Perpendicular vector
            perpendicular.normalize();
            
            // Calculate the vertices for both sides
            ofVec3f currentPoint = points[i];
            ofVec3f leftVertex = currentPoint - perpendicular * (ribbonWidth * 0.5);
            ofVec3f rightVertex = currentPoint + perpendicular * (ribbonWidth * 0.5);
            
            // Add vertices to the mesh in a zigzag manner, suitable for a triangle strip
            ribbonMesh.addVertex(leftVertex); // Add left vertex
            ribbonMesh.addTexCoord(ofVec2f(0, segmentDistances[i] / totalLineLength)); // Add left texture coordinate
            ribbonMesh.addVertex(rightVertex); // Add right vertex
            ribbonMesh.addTexCoord(ofVec2f(1, segmentDistances[i] / totalLineLength)); // Add left texture coordinate
        }
        
        if(roundCap){
            //get the poiunts on a hald circle for the cap, the circle centre is the first point and the direction is the tangent. There should be 20 pounts on the half circle.
            generateCurvedRibbonCap(points[points.size()-1], tangents[tangents.size()-1], false, totalLineLength);
        }
    }
}

void ofxBezierEditor::generateCurvedRibbonCap(ofVec3f centre,  ofVec3f tangent, bool forwards, float totalLineLength){
    // Number of points to create the half circle
    const int numPointsHalfCircle = 20;
    float angleStep = 180.0f / numPointsHalfCircle; // Half circle so we step through 180 degrees
    
    ofVec3f circleCenter = centre;
    ofVec2f circleTangent = tangent.normalize(); // Ensure the tangent is normalized
    
    // Calculate the normal to the tangent
    ofVec2f circleNormal(-circleTangent.y, circleTangent.x); // Perpendicular to the tangent
    
    std::vector<ofVec3f> halfCirclePoints;
    float textureCoordX = 0.5;
    float textureCoordY = 0; // Top of the curve
    for(int i = 0; i <= numPointsHalfCircle; ++i) {
        float angle;
        if(forwards){
            angle = ofDegToRad(90 + angleStep * i); // Start from -90 degrees to make a semicircle
            //cout << "forwards angle: " << ofRadToDeg(angle) << endl;
        }
        else{
            angle = ofDegToRad(angleStep * i - 90); // Start from -90 degrees to make a semicircle
            //cout << "backwards angle: " << ofRadToDeg(angle) << endl;
            
        }
        
        // Calculate the point on the circle using cosine and sine for the respective axes
        ofVec3f circlePoint = circleCenter + circleTangent * cos(angle) * (ribbonWidth * 0.5) + circleNormal * sin(angle) * (ribbonWidth * 0.5);
        
        
        // Add the calculated point to the half circle points vector
        ribbonMesh.addVertex(circlePoint);
        ribbonMesh.addVertex(circleCenter);
        
        
        // Now calculate distances relative to the equator
        float distanceToEquator = ribbonWidth * 0.5  * 0.5  + ((ribbonWidth * 0.5  * 0.5) * sin(angle));
        float distanceToEquatorPerp = abs((ribbonWidth * 0.5) * cos(angle));
        
        
        float texCoordX = distanceToEquator / (ribbonWidth * 0.5); // Normalize between 0 and 1
        
        float texCoordY;
        if(forwards){
            texCoordY  = (ribbonWidth / 2 - distanceToEquatorPerp)/totalLineLength; // Normalize angle between 0 and 1
            
        }
        else{
            texCoordY  = (distanceToEquatorPerp + (totalLineLength - ribbonWidth/2)) /totalLineLength; // Normalize angle between 0 and 1
        }
        
        // Add texture coordinates for the semicircle point
        ribbonMesh.addTexCoord(ofVec2f(texCoordX, texCoordY));
        
        //the next texCoord is always the circle centre, this is easy to find
        if(forwards){
            ribbonMesh.addTexCoord(ofVec2f(0.5, (ribbonWidth/2) /totalLineLength));
            
        }
        else{
            ribbonMesh.addTexCoord(ofVec2f(0.5, (totalLineLength - (ribbonWidth/2)) /totalLineLength));
            
        }
    }
}

void ofxBezierEditor::updateAllFromVertices(){
    
    updatePolyline();
    
    updateBoundingBox();
    
    calculateCenter();
    
    if(bUseRibbonMesh){
        generateTriangleStripFromPolyline(polyLineFromPoints);
    }
    if(bUseTubeMesh){
        generateTubeMeshFromPolyline(polyLineFromPoints);
    }
    if(bUseRibbonMesh || bUseTubeMesh){
        
    }
    
}

void ofxBezierEditor::generateTubeMeshFromPolyline(ofPolyline inPoly){
    if(inPoly.size() > 1){
        
        tubeMesh.clear();
        tubeMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
        ofPolyline workerLine = inPoly;
        //lets add extra points at the start and end of the line using reflection so we get good normals for the actual start and end
        glm::vec3 firstPoint = workerLine.getVertices()[0];
        glm::vec3 secondPoint = workerLine.getVertices()[1];
        glm::vec3 reflectedFirst = firstPoint - (secondPoint - firstPoint);
        
        // Reflect the second-to-last point over the last for the end of the line
        glm::vec3 lastPoint = workerLine.getVertices()[workerLine.getVertices().size() - 1];
        glm::vec3 secondLastPoint = workerLine.getVertices()[workerLine.getVertices().size() - 2];
        glm::vec3 reflectedLast = lastPoint - (secondLastPoint - lastPoint);
        
        //add the firstPoint to the start of the line
        workerLine.insertVertex(reflectedFirst, 0);
        workerLine.addVertex(reflectedLast);
        
        vector<ofVec3f> points;
        vector<ofVec3f> tangents;
        vector<ofVec3f> normals;
        
        for (int i = 1; i < workerLine.size() - 1; i++) {
            points.push_back(ofVec3f(workerLine[i].x, workerLine[i].y, 0));
            
            tangents.push_back(workerLine.getTangentAtIndex(i));
            normals.push_back(workerLine.getNormalAtIndex(i));
            
            // Add intermediate points and tangents based on precisionMultiplier
            if (i < workerLine.size() - 2 ) {
                
                for (int j = 1; j < meshLengthPrecisionMultiplier; j++) {
                    float t = float(j) / float(meshLengthPrecisionMultiplier);
                    ofVec3f interpolatedPoint = workerLine.getPointAtIndexInterpolated(i + t);
                    ofVec2f interpolatedTangent = workerLine.getTangentAtIndexInterpolated(i + t);
                    ofVec2f interpolatedNormal = workerLine.getNormalAtIndexInterpolated(i + t);
                    
                    points.push_back(interpolatedPoint);
                    tangents.push_back(interpolatedTangent);
                    normals.push_back(interpolatedNormal);
                }
            }
        }
        
        std::vector<float> segmentDistances;
        
        float totalLineLength = 0;
        
        
        segmentDistances.push_back(tubeRadius);
        totalLineLength +=  tubeRadius * 2;
        
        
        
        //get the distance between each point on the line
        for(int i = 0; i < points.size()-1; i++){
            float distance = points[i].distance(points[i+1]);
            
            segmentDistances.push_back(distance + segmentDistances.back());
            
            totalLineLength += distance;
        }
        
        vector<vector<ofVec3f>> allCircles;
        vector<vector<ofVec3f>> allCircleNormals;
        vector<vector<ofVec2f>> allCircleTexCoords;
        
        
        if(roundCap){
            generateCurvedTubeCap(allCircles, allCircleNormals, allCircleTexCoords, points[0], tangents[0], normals[0], true, totalLineLength);
        }
        if(!roundCap){
            generateFlatTubeCap(allCircles, allCircleNormals, allCircleTexCoords, points[0], tangents[0], normals[0], true, totalLineLength);
        }
        
        
        // Generate vertices for the circles
        for (int i = 0; i < points.size(); i++) {
            const ofVec3f &p0 = points[i];
            const ofVec3f &n0 = normals[i];
            const ofVec3f &t0 = tangents[i];
            
            vector<ofVec3f> circleVertices;
            vector<ofVec3f> circleNormals;
            vector<ofVec2f> circleTexCoords;
            
            for (int j = 0; j <= tubeResolution; j++) { // Use <= to include the last point in the circle
                float p = j / static_cast<float>(tubeResolution);
                float a = p * 360;
                ofVec3f v0 = n0.getRotated(a, t0) * tubeRadius + p0;
                circleVertices.push_back(v0);
                
                ofVec3f normal = v0 - p0;
                normal.normalize();
                circleNormals.push_back(normal);
                
                ofVec2f texCoord(a / 360.0, segmentDistances[i] / totalLineLength);
                circleTexCoords.push_back(texCoord);
                
                
                
            }
            
            allCircles.push_back(circleVertices);
            allCircleNormals.push_back(circleNormals);
            allCircleTexCoords.push_back(circleTexCoords);
            
        }
        if(roundCap){
            generateCurvedTubeCap(allCircles, allCircleNormals, allCircleTexCoords, points[points.size()-1], tangents[tangents.size()-1], normals[normals.size()-1], false, totalLineLength);
            
        }
        if(!roundCap){
            generateFlatTubeCap(allCircles, allCircleNormals, allCircleTexCoords, points[points.size()-1], tangents[tangents.size()-1], normals[normals.size()-1], false, totalLineLength);
        }
        
        
        //go through the circles and build the mesh
        for (int i = 0; i < allCircles.size() - 1; i++) {
            const vector<ofVec3f>& circle1 = allCircles[i];
            const vector<ofVec3f>& circle2 = allCircles[i + 1];
            const vector<ofVec3f>& normals1 = allCircleNormals[i];
            const vector<ofVec3f>& normals2 = allCircleNormals[i + 1];
            
            const vector<ofVec2f>& texCoords1 = allCircleTexCoords[i];
            const vector<ofVec2f>& texCoords2 = allCircleTexCoords[i + 1];
            
            
            for (int j = 0; j < circle1.size(); j++) {
                
                // Add two vertices at a time: one from the current circle, one from the next
                tubeMesh.addVertex(circle1[j]);
                tubeMesh.addVertex(circle2[j]);
                tubeMesh.addNormal(normals1[j]);
                tubeMesh.addNormal(normals2[j]);
                tubeMesh.addTexCoord(texCoords1[j]);
                tubeMesh.addTexCoord(texCoords2[j]);
            }
        }
    }
}

void ofxBezierEditor::setColorFill(ofColor c){
    colorFill = c;
}
void ofxBezierEditor::setColorFill(float r, float g, float b, float a){
    colorFill = ofColor(r,g,b,a);
}
void ofxBezierEditor::setColorFillR(float c){
    colorFill.r = c;
}
void ofxBezierEditor::setColorFillG(float c){
    colorFill.g = c;
}
void ofxBezierEditor::setColorFillB(float c){
    colorFill.b = c;
}
void ofxBezierEditor::setColorFillA(float a){
    colorFill.a = a;
}
void ofxBezierEditor::setColorStroke(ofColor c){
    colorStroke = c;
}
void ofxBezierEditor::setColorStroke(float r, float g, float b, float a){
    colorStroke = ofColor(r,g,b,a);
}
void ofxBezierEditor::setColorStrokeR(float c){
    colorStroke.r = c;
}
void ofxBezierEditor::setColorStrokeG(float c){
    colorStroke.g = c;
}
void ofxBezierEditor::setColorStrokeB(float c){
    colorStroke.b = c;
}
void ofxBezierEditor::setColorStrokeA(float a){
    colorStroke.a = a;
}


void ofxBezierEditor::generateCurvedTubeCap(vector<vector<ofVec3f>>& allCircles,
                                            vector<vector<ofVec3f>>& allCircleNormals,
                                            vector<vector<ofVec2f>>& allCircleTexCoords,
                                            const ofVec3f& centre,
                                            const ofVec3f& tangent,
                                            const ofVec3f& normal, bool forwards, float _totalLineLength) {
    float step = tubeRadius / (tubeResolution + 1);
    ofVec3f poleCenter;
    if(forwards){
        poleCenter = centre - tangent * tubeRadius;
        
        addRing(allCircles, allCircleNormals, allCircleTexCoords, poleCenter, 0, tangent, normal, centre, 0,_totalLineLength);
        // Loop to generate each ring, starting from the one closest to the equator and moving towards the pole
        for (int i = tubeResolution; i >= 1; i--) {
            // Calculate the vertical distance from the center to the current ring
            float verticalDistance = step * i;
            // Calculate the actual distance from the sphere center to the ring center on the surface of the hemisphere
            float ringDistance = tubeRadius * cos(asin(verticalDistance / tubeRadius));
            // Calculate the ring center point
            ofVec3f ringCenter = centre - tangent * verticalDistance;
            
            // Calculate the ring radius
            float ringRadius = sqrt(tubeRadius * tubeRadius - verticalDistance * verticalDistance);
            
            // Add the calculated ring to the vectors
            addRing(allCircles, allCircleNormals, allCircleTexCoords, ringCenter, ringRadius, tangent, normal, centre, tubeRadius - verticalDistance,_totalLineLength );
        }
    }
    
    
    // Add the special ring at the pole with zero radius
    // The center of this ring is the point on the hemisphere's surface directly above the sphere's center
    
    if(!forwards){
        
        for (int i = 1; i <= tubeResolution; i++) {
            // Calculate the vertical distance from the center to the current ring
            float verticalDistance = step * i;
            // Calculate the actual distance from the sphere center to the ring center on the surface of the hemisphere
            float ringDistance = tubeRadius * cos(asin(verticalDistance / tubeRadius));
            // Calculate the ring center point
            ofVec3f ringCenter = centre + tangent * verticalDistance;
            
            // Calculate the ring radius
            float ringRadius = sqrt(tubeRadius * tubeRadius - verticalDistance * verticalDistance);
            
            // Add the calculated ring to the vectors
            addRing(allCircles, allCircleNormals, allCircleTexCoords, ringCenter, ringRadius, tangent, normal, centre,  (_totalLineLength - tubeRadius) + verticalDistance, _totalLineLength );
        }
        ofVec3f southPoleCenter = centre + tangent * tubeRadius;
        addRing(allCircles, allCircleNormals, allCircleTexCoords, southPoleCenter, 0, tangent, normal, centre , _totalLineLength,_totalLineLength);
    }
    
}

void ofxBezierEditor::generateFlatTubeCap(vector<vector<ofVec3f>>& allCircles,
                                          vector<vector<ofVec3f>>& allCircleNormals,
                                          vector<vector<ofVec2f>>& allCircleTexCoords,
                                          const ofVec3f& flatCapCentre,
                                          const ofVec3f& tangent,
                                          const ofVec3f& normal, bool forwards, float _totalLineLength) {
    
    if(forwards){
        for(int i = 0; i < tubeResolution; i++){
            
            vector<ofVec3f> circleVertices;
            vector<ofVec3f> circleNormals;
            vector<ofVec2f> circleTexCoords;
            
            float ringRadius = (tubeRadius/tubeResolution) * i;
            
            for (int j = 0; j <= tubeResolution; j++) { // Use <= to include the last point in the circle
                float p = j / static_cast<float>(tubeResolution);
                float a = p * 360;
                ofVec3f v0 = normal.getRotated(a, tangent) * ringRadius + flatCapCentre;
                circleVertices.push_back(v0);
                
                ofVec3f normal = tangent * -1;
                normal.normalize();
                circleNormals.push_back(normal);
                
                ofVec2f texCoord(a / 360.0, ringRadius / _totalLineLength);
                circleTexCoords.push_back(texCoord);
            }
            
            allCircles.push_back(circleVertices);
            allCircleNormals.push_back(circleNormals);
            allCircleTexCoords.push_back(circleTexCoords);
        }
    }
    else{
        for(int i = tubeResolution - 1; i >= 0; i--){
            
            vector<ofVec3f> circleVertices;
            vector<ofVec3f> circleNormals;
            vector<ofVec2f> circleTexCoords;
            
            float ringRadius = (tubeRadius/tubeResolution) * i;
            
            for (int j = 0; j <= tubeResolution; j++) { // Use <= to include the last point in the circle
                float p = j / static_cast<float>(tubeResolution);
                float a = p * 360;
                ofVec3f v0 = normal.getRotated(a, tangent) * ringRadius + flatCapCentre;
                circleVertices.push_back(v0);
                
                ofVec3f normal = tangent;
                normal.normalize();
                circleNormals.push_back(normal);
                
                ofVec2f texCoord(a / 360.0, (ringRadius + (_totalLineLength - tubeRadius)) / _totalLineLength);
                circleTexCoords.push_back(texCoord);
            }
            
            allCircles.push_back(circleVertices);
            allCircleNormals.push_back(circleNormals);
            allCircleTexCoords.push_back(circleTexCoords);
        }
    }
    
}


void ofxBezierEditor::addRing(vector<vector<ofVec3f>>& allCircles,
                              vector<vector<ofVec3f>>& allCircleNormals,
                              vector<vector<ofVec2f>>& allCircleTexCoords, const ofVec3f& ringCenter, float radius, const ofVec3f& tangent, const ofVec3f& normal, const ofVec3f& sphereCenter, float distanceFromStart, float _totalLineLength) {
    std::vector<ofVec3f> circleVertices;
    std::vector<ofVec3f> circleNormals;
    std::vector<ofVec2f> circleTexCoords;
    
    for (int j = 0; j <= tubeResolution; j++) { // Use <= to include the last point in the circle
        float p = j / static_cast<float>(tubeResolution);
        float a = p * 360;
        ofVec3f v0 = normal.getRotated(a, tangent) * radius + ringCenter;
        circleVertices.push_back(v0);
        
        ofVec3f normalAtVertex = (v0 - sphereCenter).getNormalized();
        circleNormals.push_back(normalAtVertex);
        
        circleTexCoords.push_back(ofVec2f(p, distanceFromStart / _totalLineLength));
    }
    
    allCircles.push_back(circleVertices);
    allCircleNormals.push_back(circleNormals);
    allCircleTexCoords.push_back(circleTexCoords);
}

