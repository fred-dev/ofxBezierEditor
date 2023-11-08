#include "ofxBezierEditor.h"

#ifdef GEO_LINE
#include "MapsManager.h"
#endif


ofxBezierEditor::ofxBezierEditor(){
    
    setRadiusVertex(8);
    setRadiusControlPoints(6);
    
    currentPointToMove = 0;
    lastVertexSelected = 0;
    
    draggableVertex vtx;
    curveVertices.assign(0, vtx);
    controlPoint1.assign(0, vtx);
    controlPoint2.assign(0, vtx);
    
#ifdef GEO_LINE
    curveVerticesGeo.assign(0, vtx);
    controlPoint1Geo.assign(0, vtx);
    controlPoint2Geo.assign(0, vtx);
#endif
    
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
    
    ofPopStyle(); // Restore the previous style settings
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
        
        
#ifdef GEO_LINE
        // Read vertices from JSON
        curveVerticesGeo.clear();
        ofLogVerbose() << "vertices" << JSONBezier["bezier"]["vertices"].dump();
        
        for (const auto& vertexJson : JSONBezier["bezier"]["vertices"]) {
            draggableVertex vtx;
            vtx.pos.x = vertexJson["x"];
            vtx.pos.y = vertexJson["y"];
            vtx.bOver = false;
            vtx.bBeingDragged = false;
            vtx.bBeingSelected = false;
            curveVerticesGeo.push_back(vtx);
        }
        ofLogVerbose("ofxBezierEditor") << "curveVerticesGeo.size(): " << curveVerticesGeo.size();
        
        // Read control points from JSON
        controlPoint1Geo.clear();
        //dump the json to the console
        for (const auto& cpJson : JSONBezier["bezier"]["cp1"]) {
            draggableVertex cp;
            cp.pos.x = cpJson["x"];
            cp.pos.y = cpJson["y"];
            cp.bOver = false;
            cp.bBeingDragged = false;
            controlPoint1Geo.push_back(cp);
        }
        ofLogVerbose("ofxBezierEditor") << "controlPoint1Geo.size(): " << controlPoint1Geo.size();
        
        controlPoint2Geo.clear();
        for (const auto& cpJson : JSONBezier["bezier"]["cp2"]) {
            draggableVertex cp;
            cp.pos.x = cpJson["x"];
            cp.pos.y = cpJson["y"];
            cp.bOver = false;
            cp.bBeingDragged = false;
            controlPoint2Geo.push_back(cp);
        }
        ofLogVerbose("ofxBezierEditor") << "controlPoint2Geo.size(): " << controlPoint2Geo.size();
        
        updatePixelCoordsFromGeo();
#else
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
#endif
        
        
    } else {
        // Handle the case where the JSON file doesn't exist.
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
    
#ifdef GEO_LINE
    for (int i = 0; i < curveVerticesGeo.size(); i++) {
        JSONBezier["bezier"]["vertices"][i]["x"] = curveVerticesGeo.at(i).pos.x ;
        JSONBezier["bezier"]["vertices"][i]["y"] = curveVerticesGeo.at(i).pos.y ;
    }
    
    for (int i = 0; i < controlPoint1Geo.size(); i++) {
        JSONBezier["bezier"]["cp1"][i]["x"] = controlPoint1Geo.at(i).pos.x ;
        JSONBezier["bezier"]["cp1"][i]["y"] = controlPoint1Geo.at(i).pos.y ;
    }
    
    for (int i = 0; i < controlPoint2Geo.size(); i++) {
        JSONBezier["bezier"]["cp2"][i]["x"] = controlPoint2Geo.at(i).pos.x ;
        JSONBezier["bezier"]["cp2"][i]["y"] = controlPoint2Geo.at(i).pos.y ;
    }
#endif
    
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
        
        
        
    } // end of if(curveVertices.size() > 0){
    
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
                    vtx.bOver 			= true;
                    vtx.bBeingDragged 	= false;
                    vtx.bBeingSelected  = false;
                    curveVertices.push_back(vtx);
                    
                    draggableVertex cp;
                    int nEnd = curveVertices.size()-1;
                    cp.pos.x = ofLerp(curveVertices.at(0).pos.x , curveVertices.at(nEnd).pos.x , 0.66);
                    cp.pos.y = ofLerp(curveVertices.at(0).pos.y , curveVertices.at(nEnd).pos.y , 0.66);
                    cp.bOver 			= false;
                    cp.bBeingDragged 	= false;
                    cp.bBeingSelected 	= false;
                    controlPoint1.push_back(cp);
                    cp.pos.x = ofLerp(curveVertices.at(0).pos.x , curveVertices.at(nEnd).pos.x , 0.33);
                    cp.pos.y = ofLerp(curveVertices.at(0).pos.y , curveVertices.at(nEnd).pos.y , 0.33);
                    controlPoint2.push_back(cp);
                    
                    updateAllFromVertices();
                    
                }
            } // end else bshowBoundingBox = false
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
                    vtx.bOver 			= true;
                    vtx.bBeingDragged 	= false;
                    vtx.bBeingSelected 	= false;
                    curveVertices.insert(curveVertices.begin()+lastVertexSelected, vtx);
                    
                    draggableVertex cp;
                    cp.pos.x = ofLerp(curveVertices.at(lastVertexSelected-1).pos.x , curveVertices.at(lastVertexSelected).pos.x , 0.66);
                    cp.pos.y = ofLerp(curveVertices.at(lastVertexSelected-1).pos.y , curveVertices.at(lastVertexSelected).pos.y , 0.66);
                    cp.bOver 			= false;
                    cp.bBeingDragged 	= false;
                    cp.bBeingSelected 	= false;
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

void ofxBezierEditor::generateTriangleStripFromPolyline(ofPolyline bezierLine) {
    if (curveVertices.size() > 0) {
        // Clear mesh (triangle strip)
        ribbonMesh.clear();
        ribbonMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
        
        // Create vectors to store points and tangents
        vector<ofVec3f> points;
        vector<ofVec2f> tangents;
        
        for (int i = 0; i < bezierLine.size(); i++) {
            points.push_back(ofVec3f(bezierLine[i].x, bezierLine[i].y, 0));
            if (i < bezierLine.size() - 1) {
                ofVec2f tangent = bezierLine.getTangentAtIndex(i);
                tangents.push_back(tangent);
                
                // Add intermediate points and tangents based on precisionMultiplier
                for (int j = 1; j < meshLengthPrecisionMultiplier; j++) {
                    float t = static_cast<float>(j) / static_cast<float>(meshLengthPrecisionMultiplier);
                    ofVec3f interpolatedPoint = bezierLine.getPointAtIndexInterpolated(i + t);
                    ofVec2f interpolatedTangent = bezierLine.getTangentAtIndexInterpolated(i + t);
                    points.push_back(interpolatedPoint);
                    tangents.push_back(interpolatedTangent);
                }
            }
        }
        
        for (int i = 0; i < points.size(); i++) {
            // Calculate the perpendicular vector
            ofVec2f perpendicular(-tangents[i].y, tangents[i].x); // Perpendicular vector
            perpendicular.normalize();
            
            // Calculate the vertices for both sides
            ofVec3f currentPoint = points[i];
            ofVec3f leftVertex = currentPoint - perpendicular * (ribbonWidth * 0.5);
            ofVec3f rightVertex = currentPoint + perpendicular * (ribbonWidth * 0.5);
            
            // Add vertices to the mesh
            ribbonMesh.addVertex(leftVertex);
            ribbonMesh.addVertex(rightVertex);
            
            // Define the triangle strip indices
            if (i > 0 && i < points.size() - 1) {
                // Create triangles connecting the current and previous vertices
                ribbonMesh.addIndex((i - 1) * 2); // Previous left point
                ribbonMesh.addIndex((i - 1) * 2 + 1); // Previous right point
                ribbonMesh.addIndex(i * 2);     // Current left point
                
                ribbonMesh.addIndex((i - 1) * 2 + 1); // Previous right point
                ribbonMesh.addIndex(i * 2);     // Current left point
                ribbonMesh.addIndex(i * 2 + 1); // Current right point
            }
        }
    }
}


void ofxBezierEditor::updateAllFromVertices(){
#ifdef GEO_LINE
    updateGeoCoordsFromPixel();
#endif
    updatePolyline();
    
    updateBoundingBox();
    
    calculateCenter();
    
    if(bUseRibbonMesh){
        generateTriangleStripFromPolyline(polyLineFromPoints);;
    }
    if(bUseTubeMesh){
        generateTubeMeshFromPolyline(polyLineFromPoints);
    }
    
}
#ifdef GEO_LINE
void ofxBezierEditor::updateAllFromGeo(){
    
    
    updatePolyline();
    
    updateBoundingBox();
    
    calculateCenter();
    
    if(bUseRibbonMesh){
        generateTriangleStripFromPolyline(polyLineFromPoints);;
    }
    if(bUseTubeMesh){
        generateTubeMeshFromPolyline(polyLineFromPoints);
    }
}
#endif
ofVboMesh ofxBezierEditor::getTubeMeshFromPolyline(ofPolyline bezierLine){
    generateTubeMeshFromPolyline(bezierLine);
    return tubeMesh;
}

void ofxBezierEditor::generateTubeMeshFromPolyline(ofPolyline bezierLine){
    if(polyLineFromPoints.size() > 1){
        
        tubeMesh.clear();
        tubeMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
        
        float tubeCircumference = 2.0 * PI * tubeRadius;
        
        vector<ofVec3f> points;
        vector<ofVec3f> tangents;
        vector<ofVec3f> normals;
        for (int i = 0; i < polyLineFromPoints.size(); i++) {
            points.push_back(ofVec3f(polyLineFromPoints[i].x, polyLineFromPoints[i].y, 0));
            
            tangents.push_back(polyLineFromPoints.getTangentAtIndex(i));
            normals.push_back(polyLineFromPoints.getNormalAtIndex(i));
            
            // Add intermediate points and tangents based on precisionMultiplier
            for (int j = 1; j < meshLengthPrecisionMultiplier; j++) {
                float t = float(j) / float(meshLengthPrecisionMultiplier);
                ofVec3f interpolatedPoint = polyLineFromPoints.getPointAtIndexInterpolated(i + t);
                ofVec2f interpolatedTangent = polyLineFromPoints.getTangentAtIndexInterpolated(i + t);
                ofVec2f interpolatedNormal = polyLineFromPoints.getNormalAtIndexInterpolated(i + t);
                
                points.push_back(interpolatedPoint);
                tangents.push_back(interpolatedTangent);
                normals.push_back(interpolatedNormal);
            }
        }
        int tubeLength = points.size();
        vector<ofVec3f> circleVertices;
        vector<ofVec3f> allCircleVertices;
        
        // Generate vertices for the circles
        for (int i = 0; i < tubeLength; i++) {
            const ofVec3f &p0 = points[i];
            const ofVec3f &n0 = normals[i];
            const ofVec3f &t0 = tangents[i];
            
            for (int j = 0; j <= tubeResolution; j++) { // Use <= to include the last point in the circle
                float p = j / static_cast<float>(tubeResolution);
                float a = p * 360;
                ofVec3f v0 = n0.getRotated(a, t0) * tubeRadius + p0;
                
                // Calculate normals, texture coords, and colors for each vertex
                ofVec3f normal = v0 - p0;
                normal.normalize();
                float u = j / static_cast<float>(tubeResolution);
                float v = i / static_cast<float>(tubeLength);
                
                // Add the vertex, normal, texcoord, and color to the mesh
                tubeMesh.addVertex(v0);
                tubeMesh.addNormal(normal);
                tubeMesh.addTexCoord(ofVec2f(u, v));
                tubeMesh.addColor(colorStroke);
                
                if (i != tubeLength - 1) {
                    // Connect the current circle to the next one
                    int nextCircleIdx = (i + 1) * (tubeResolution + 1) + j;
                    tubeMesh.addIndex(i * (tubeResolution + 1) + j); // current vertex
                    tubeMesh.addIndex(nextCircleIdx); // corresponding vertex on the next circle
                }
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
#ifdef GEO_LINE
void ofxBezierEditor::updatePixelCoordsFromGeo(){
    
    curveVertices.clear();
    for(int i = 0; i < curveVerticesGeo.size(); i++){
        std::shared_ptr<glm::vec2> tempPixVtx = MapsManager::getInstance().coordsToPixelsConvertor(curveVerticesGeo[i].pos);
        if (tempPixVtx) {
            draggableVertex tmpDVtx;
            tmpDVtx.pos = *tempPixVtx;
            curveVertices.push_back(tmpDVtx);
        } else {
            ofLogVerbose()<<"ofxBezierEditor::updatePixelCoordsFromGeo() - tempPixVtx is null";
        }
    }
    
    controlPoint1.clear();
    for(int i = 0; i < controlPoint1Geo.size(); i++){
        std::shared_ptr<glm::vec2> tempPixVtx = MapsManager::getInstance().coordsToPixelsConvertor(controlPoint1Geo[i].pos);
        if (tempPixVtx) {
            draggableVertex tmpDVtx;
            tmpDVtx.pos = *tempPixVtx;
            controlPoint1.push_back(tmpDVtx);
        } else {
            ofLogVerbose()<<"ofxBezierEditor::updatePixelCoordsFromGeo() - tempPixVtx is null";
        }
    }
    
    controlPoint2.clear();
    for(int i = 0; i < controlPoint2Geo.size(); i++){
        std::shared_ptr<glm::vec2> tempPixVtx = MapsManager::getInstance().coordsToPixelsConvertor(controlPoint2Geo[i].pos);
        if (tempPixVtx) {
            draggableVertex tmpDVtx;
            tmpDVtx.pos = *tempPixVtx;
            controlPoint2.push_back(tmpDVtx);
        } else {
            ofLogVerbose()<<"ofxBezierEditor::updatePixelCoordsFromGeo() - tempPixVtx is null";
        }
    }
    updateAllFromGeo();
    
}

void ofxBezierEditor::updateGeoCoordsFromPixel(){
    curveVerticesGeo.clear();
    for(int i = 0; i < curveVertices.size(); i++){
        std::shared_ptr<glm::vec2> tempGeoVtx = MapsManager::getInstance().pixelsToCoordsConvertor(curveVertices[i].pos);
        if (tempGeoVtx) {
            draggableVertex tmpDVtx;
            tmpDVtx.pos = *tempGeoVtx;
            curveVerticesGeo.push_back(tmpDVtx);
        } else {
            ofLogVerbose()<<"ofxBezierEditor::updateGeoCoordsFromPixel() - tempGeoVtx is null";
        }
    }
    
    controlPoint1Geo.clear();
    for(int i = 0; i < controlPoint1.size(); i++){
        std::shared_ptr<glm::vec2> tempGeoVtx = MapsManager::getInstance().pixelsToCoordsConvertor(controlPoint1[i].pos);
        if (tempGeoVtx) {
            draggableVertex tmpDVtx;
            tmpDVtx.pos = *tempGeoVtx;
            controlPoint1Geo.push_back(tmpDVtx);
        } else {
            ofLogVerbose()<<"ofxBezierEditor::updateGeoCoordsFromPixel() - tempGeoVtx is null";        }
    }
    
    controlPoint2Geo.clear();
    for(int i = 0; i < controlPoint2.size(); i++){
        std::shared_ptr<glm::vec2> tempGeoVtx = MapsManager::getInstance().pixelsToCoordsConvertor(controlPoint2[i].pos);
        if (tempGeoVtx) {
            draggableVertex tmpDVtx;
            tmpDVtx.pos = *tempGeoVtx;
            controlPoint2Geo.push_back(tmpDVtx);
        } else {
            ofLogVerbose()<<"ofxBezierEditor::updateGeoCoordsFromPixel() - tempGeoVtx is null";        }
    }
}

#endif
