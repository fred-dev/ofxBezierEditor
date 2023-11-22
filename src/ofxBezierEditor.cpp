#include "ofxBezierEditor.h"

#ifdef GEO_LINE
#include "MapsManager.h"
#endif

ofxBezierEditor::ofxBezierEditor()
:   settings(*curveVertices, *controlPoint1, *controlPoint2, *curveVerticesGeo, *controlPoint1Geo, *controlPoint2Geo),
    tubeMeshBuilder(settings),
    ribbonMeshBuilder(settings),
    bezierDraw(settings, *curveVertices, *controlPoint1, *controlPoint2),
    bezierUI(settings, *curveVertices, *controlPoint1, *controlPoint2){
        
    settings.setRadiusVertex(8);
    settings.setRadiusControlPoints(6);
    
//    currentPointToMove = 0;
//    lastVertexSelected = 0;
    
    draggableVertex vtx;
    curveVertices->assign(0, vtx);
    controlPoint1->assign(0, vtx);
    controlPoint2->assign(0, vtx);
       
#ifdef GEO_LINE
    curveVerticesGeo->assign(0, vtx);
    controlPoint1Geo->assign(0, vtx);
    controlPoint2Geo->assign(0, vtx);
#endif

    
    settings.colorFill = ofColor(188,4,62, 100);
    settings.colorStroke = ofColor(2,189,190, 100);
    
    settings.boundingBox.set(0,0,0,0);
    settings.bshowBoundingBox = false;
    center.set(0,0);
    
    settings.beditBezier = false;
    
    
    settings.jsonFileName = "ofxBezierInfo.json";
    
    polyLineFromPoints.setClosed(false);
    ofAddListener(triggerUpdateEvent, this, &ofxBezierEditor::onTriggerUpdate);

}

ofxBezierEditor::~ofxBezierEditor(){
    ofRemoveListener(triggerUpdateEvent, this, &ofxBezierEditor::onTriggerUpdate);

}

void ofxBezierEditor::updateBoundingBox(){
    settings.boundingBox = polyLineFromPoints.getBoundingBox();
}

void ofxBezierEditor::calculateCenter(){
    center.set(settings.boundingBox.x + 0.5f*settings.boundingBox.width, settings.boundingBox.y + 0.5f*settings.boundingBox.height);
}

void ofxBezierEditor::updatePolyline(){
    polyLineFromPoints.clear();
    if(curveVertices->size() > 0){
        
        for (int i = 0; i < curveVertices->size(); i++){
            if (i == 0){
                polyLineFromPoints.addVertex(curveVertices->at(0).pos.x, curveVertices->at(0).pos.y); // we need to duplicate 0 for the curve to start at point 0
            }
            else{
                polyLineFromPoints.bezierTo(controlPoint1->at(i).pos.x, controlPoint1->at(i).pos.y, controlPoint2->at(i).pos.x, controlPoint2->at(i).pos.y, curveVertices->at(i).pos.x, curveVertices->at(i).pos.y);
            }
        }
        if(settings.bIsClosed){
            polyLineFromPoints.bezierTo(controlPoint1->at(0).pos.x, controlPoint1->at(0).pos.y, controlPoint2->at(0).pos.x, controlPoint2->at(0).pos.y, curveVertices->at(0).pos.x, curveVertices->at(0).pos.y);
            polyLineFromPoints.setClosed(settings.bIsClosed);
        }
        
    }
}

#ifdef GEO_LINE

void ofxBezierEditor::updateAllFromGeo(){
    
    updatePolyline();
     
     updateBoundingBox();
     
     calculateCenter();
     
    if(settings.bUseRibbonMesh){
        ribbonMeshBuilder.generateTriangleStripFromPolyline(polyLineFromPoints);
        
    }
    if(settings.bUseTubeMesh){
        tubeMeshBuilder.generateTubeMeshFromPolyline(polyLineFromPoints);
    }
}
#endif


void ofxBezierEditor::updateAllFromVertices(){
#ifdef GEO_LINE
    updateGeoCoordsFromPixel();
#endif
    updatePolyline();
    
    updateBoundingBox();
    
    calculateCenter();
    
    if(settings.bUseRibbonMesh){
        ribbonMeshBuilder.generateTriangleStripFromPolyline(polyLineFromPoints);
        
    }
    if(settings.bUseTubeMesh){
        tubeMeshBuilder.generateTubeMeshFromPolyline(polyLineFromPoints);
    }
    bezierDraw.updateNormalLine(polyLineFromPoints);
    
}


ofColor ofxBezierEditor::getColorStroke(){
    return settings.colorStroke;
}

ofVboMesh ofxBezierEditor::getRibbonMeshFromPolyline(ofPolyline polyline){
    return ribbonMeshBuilder.getRibbonMeshFromPolyline(polyline);
}

ofVboMesh ofxBezierEditor::getTubeMeshFromPolyline(ofPolyline polyline){
    return tubeMeshBuilder.getTubeMeshFromPolyline(polyline);
}

ofVboMesh ofxBezierEditor::getRibbonMesh(){
    return ribbonMeshBuilder.getRibbonMesh();
}

ofVboMesh ofxBezierEditor::getTubeMesh(){
    return tubeMeshBuilder.getTubeMesh();
}
void ofxBezierEditor::draw(){
    bezierDraw.draw();
}
void ofxBezierEditor::drawHelp(){
    bezierDraw.drawHelp();
}
void ofxBezierEditor::loadPoints(string filename){
    settings.loadPoints(filename);
}
void ofxBezierEditor::setFillBezier(bool value){
    settings.bfillBezier = value;
    updateAllFromVertices();

}
void ofxBezierEditor::setClosed(bool value){
    settings.bIsClosed = value;
}
void ofxBezierEditor::setRibbonWidth(float value){
    settings.ribbonWidth = value;
    updateAllFromVertices();

}
void ofxBezierEditor::setColorFill(ofColor value){
    settings.colorFill = value;
    updateAllFromVertices();

}
void ofxBezierEditor::setColorStroke(ofColor value){
    settings.colorStroke = value;
    updateAllFromVertices();

}
void ofxBezierEditor::setMeshLengthPrecisionMultiplier(int value){
    settings.meshLengthPrecisionMultiplier = value;
    updateAllFromVertices();

}
void ofxBezierEditor::setTubeRadius(float value){
    settings.tubeRadius = value;
    updateAllFromVertices();

}
void ofxBezierEditor::setTubeResolution(int value){
    settings.tubeResolution = value;
    updateAllFromVertices();

}
void ofxBezierEditor::setHasRoundCaps(bool value){
    settings.roundCap = value;
    updateAllFromVertices();

}

void ofxBezierEditor::setReactToMouseAndKeyEvents(bool value){
    bezierUI.setReactToMouseAndKeyEvents(value);
}


void ofxBezierEditor::onTriggerUpdate(TriggerUpdateEventArgs& args) {
    updateAllFromVertices();
}
void ofxBezierEditor::setUseRibbonMesh(bool value){
    settings.bUseRibbonMesh = value;
    updateAllFromVertices();

}
void ofxBezierEditor::setUseTubeMesh(bool value){
    settings.bUseTubeMesh = value;
}

#ifdef GEO_LINE
void ofxBezierEditor::updatePixelCoordsFromGeo(){
    
    curveVertices->clear();
    for(int i = 0; i < curveVerticesGeo->size(); i++){
        std::shared_ptr<glm::vec2> tempPixVtx = MapsManager::getInstance().coordsToPixelsConvertor(curveVerticesGeo->at(i).pos);
        if (tempPixVtx) {
            draggableVertex tmpDVtx;
            tmpDVtx.pos = *tempPixVtx;
            curveVertices->push_back(tmpDVtx);
        } else {
            ofLogVerbose()<<"ofxBezierEditor::updatePixelCoordsFromGeo() - tempPixVtx is null";
        }
    }
    
    controlPoint1->clear();
    for(int i = 0; i < controlPoint1Geo->size(); i++){
        std::shared_ptr<glm::vec2> tempPixVtx = MapsManager::getInstance().coordsToPixelsConvertor(controlPoint1Geo->at(i).pos);
        if (tempPixVtx) {
            draggableVertex tmpDVtx;
            tmpDVtx.pos = *tempPixVtx;
            controlPoint1->push_back(tmpDVtx);
        } else {
            ofLogVerbose()<<"ofxBezierEditor::updatePixelCoordsFromGeo() - tempPixVtx is null";
        }
    }
    
    controlPoint2->clear();
    for(int i = 0; i < controlPoint2Geo->size(); i++){
        std::shared_ptr<glm::vec2> tempPixVtx = MapsManager::getInstance().coordsToPixelsConvertor(controlPoint2Geo->at(i).pos);
        if (tempPixVtx) {
            draggableVertex tmpDVtx;
            tmpDVtx.pos = *tempPixVtx;
            controlPoint2->push_back(tmpDVtx);
        } else {
            ofLogVerbose()<<"ofxBezierEditor::updatePixelCoordsFromGeo() - tempPixVtx is null";
        }
    }
    updateAllFromGeo();
    
}

void ofxBezierEditor::updateGeoCoordsFromPixel(){
    curveVerticesGeo->clear();
    for(int i = 0; i < curveVertices->size(); i++){
        std::shared_ptr<glm::vec2> tempGeoVtx = MapsManager::getInstance().pixelsToCoordsConvertor(curveVertices->at(i).pos);
        if (tempGeoVtx) {
            draggableVertex tmpDVtx;
            tmpDVtx.pos = *tempGeoVtx;
            curveVerticesGeo->push_back(tmpDVtx);
        } else {
            ofLogVerbose()<<"ofxBezierEditor::updateGeoCoordsFromPixel() - tempGeoVtx is null";
        }
    }
    
    controlPoint1Geo->clear();
    for(int i = 0; i < controlPoint1->size(); i++){
        std::shared_ptr<glm::vec2> tempGeoVtx = MapsManager::getInstance().pixelsToCoordsConvertor(controlPoint1->at(i).pos);
        if (tempGeoVtx) {
            draggableVertex tmpDVtx;
            tmpDVtx.pos = *tempGeoVtx;
            controlPoint1Geo->push_back(tmpDVtx);
        } else {
            ofLogVerbose()<<"ofxBezierEditor::updateGeoCoordsFromPixel() - tempGeoVtx is null";        }
    }
    
    controlPoint2Geo->clear();
    for(int i = 0; i < controlPoint2->size(); i++){
        std::shared_ptr<glm::vec2> tempGeoVtx = MapsManager::getInstance().pixelsToCoordsConvertor(controlPoint2->at(i).pos);
        if (tempGeoVtx) {
            draggableVertex tmpDVtx;
            tmpDVtx.pos = *tempGeoVtx;
            controlPoint2Geo->push_back(tmpDVtx);
        } else {
            ofLogVerbose()<<"ofxBezierEditor::updateGeoCoordsFromPixel() - tempGeoVtx is null";        }
    }
}

#endif
