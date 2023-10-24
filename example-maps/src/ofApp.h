#pragma once

#include "ofMain.h"
#include "ofxMaps.h"
#include "ofxBezierEditor.h" 


class ofApp: public ofBaseApp
{
public:
    void setup() override;
    void update() override;
    void draw() override;
    
    void keyPressed(int key) override;
    
    std::shared_ptr<ofxMaps::MBTilesCache> bufferCache;
    std::shared_ptr<ofxMaps::MapTileLayer> tileLayer;
    std::shared_ptr<ofxMaps::MapTileSet> tileSet;
    std::shared_ptr<ofxMaps::MapTileProvider> tileProvider;
    
    std::vector<ofxGeo::Coordinate> coordinates;
    
    std::vector<std::string> sets;
    int setsIndex = 0;
    
    float animation = 0;
    
    ofEasyCam cam;
    ofPolyline mapLine;
    
    
    std::shared_ptr<glm::vec2> coordsToPixelsConvertor(glm::vec2 coords);
    
    std::shared_ptr<glm::vec2> pixelsToCoordsConvertor(glm::vec2 pixels);
    
    
    ofxBezierEditor myBezier;
    bool bdrawBezierInfo;
    ofFbo mapFbo;
    
    
};
