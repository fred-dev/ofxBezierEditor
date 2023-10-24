//
// Copyright (c) 2014 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:    MIT
//


#include "ofApp.h"

void ofApp::setup()
{
    

ofSetLogLevel(OF_LOG_VERBOSE);
    
    coordinates =
    {
        { -33.971125, 151.252418 },
        { -35.14128, 150.670794 },
        { -37.80691, 145.507458 },
        { -34.732269, 139.146382 },
        { -29.088671, 134.937884 },
        { -23.89005, 133.939217 },
        { -31.117161, 130.032615 },
        { -33.5706, 122.15805 },
        { -32.109976, 116.308374 },
        { -23.26724, 114.017591 },
        { -21.000716, 119.966666 },
        { -17.943711, 122.744788 },
        { -15.883685, 128.643304 },
        { -12.607486, 130.818194 },
        { -12.755096, 135.712222 },
        { -18.286401, 139.453783 },
        { -18.554965, 141.333864 },
        { -12.835455, 141.964701 },
        { -17.274462, 145.721683 },
        { -19.768093, 147.336066 }
    };
    
    ofJson json = ofLoadJson("provider.json");
    
    tileProvider = std::make_shared<ofxMaps::MapTileProvider>(ofxMaps::MapTileProvider::fromJSON(json));
    Poco::ThreadPool::defaultPool().addCapacity(64);
    bufferCache = std::make_shared<ofxMaps::MBTilesCache>(*tileProvider, "cache/");
    tileSet = std::make_shared<ofxMaps::MapTileSet>(1024,
                                                    tileProvider,
                                                    bufferCache);
    
    tileLayer = std::make_shared<ofxMaps::MapTileLayer>(tileSet, 1 * 1920, 1 * 1080);
    
    ofxGeo::Coordinate centre(-24.978199, 133.100608);
    
    
    tileLayer->setCenter(centre, 5);
    
    myBezier.loadPoints("ofxBezierInfo.json");
    bdrawBezierInfo = false;
}


void ofApp::update()
{
    tileLayer->update();
    
    if (!ofIsFloatEqual(animation, 0.f))
        tileLayer->setCenter(tileLayer->getCenter().getNeighbor(animation, 0));
}


void ofApp::draw()
{
    ofEnableAlphaBlending();

    ofPushMatrix();
    tileLayer->draw(0, 0);
    ofPopMatrix();
    
    ofPushStyle();
    ofNoFill();
    ofSetColor(0, 255, 0);
    mapLine.clear();
    for (int i = 0; i < coordinates.size(); ++i)
    {
        auto tc = tileLayer->geoToPixels(coordinates[i]);
        ofDrawCircle(tc.x, tc.y, 20);
        
    }
    ofPopStyle();
    
    ofPushStyle();
   
    if(bdrawBezierInfo){
        ofPushStyle();
        ofSetColor(127);
        ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
        ofPopStyle();
        myBezier.drawHelp();
    }
    else{
        myBezier.getRibbonMesh().draw();

    }
    
    ofPopStyle();
        
    ofDrawBitmapStringHighlight(tileLayer->getCenter().toString(0), 14, ofGetHeight() - 32);
    ofDrawBitmapStringHighlight("Task Queue:" + ofx::TaskQueue::instance().toString(), 14, ofGetHeight() - 16);
    ofDrawBitmapStringHighlight("Connection Pool: " + bufferCache->toString(), 14, ofGetHeight() - 2);
    
}


void ofApp::keyPressed(int key)
{
    if (key == 'f' || key == 'F')
    {
        ofToggleFullscreen();
    }
    else if (key == '-')
    {
        tileLayer->setCenter(tileLayer->getCenter().getZoomedBy(-1));
        myBezier.updatePixelCoordsFromGeo();
    }
    else if (key == '=')
    {
        tileLayer->setCenter(tileLayer->getCenter().getZoomedBy(1));
        myBezier.updatePixelCoordsFromGeo();
    }
    else if (key == 'w')
    {
        tileLayer->setCenter(tileLayer->getCenter().getNeighborUp());
        myBezier.updatePixelCoordsFromGeo();
    }
    else if (key == 'a')
    {
        tileLayer->setCenter(tileLayer->getCenter().getNeighborLeft());
        myBezier.updatePixelCoordsFromGeo();
    }
    else if (key == 's')
    {
        tileLayer->setCenter(tileLayer->getCenter().getNeighborDown());
        myBezier.updatePixelCoordsFromGeo();
    }
    else if (key == 'd')
    {
        tileLayer->setCenter(tileLayer->getCenter().getNeighborRight());
        myBezier.updatePixelCoordsFromGeo();
    }
    else if (key == '1')
    {
        animation -= 0.01;;
    }
    else if (key == '2')
    {
        animation += 0.01;;
    }
    else if (key == '3')
    {
        animation = 0;
    }
    else if (key == 'h'){
        bdrawBezierInfo = !bdrawBezierInfo;
    }
    else if (key == 'L'){
        myBezier.loadPoints("ofxBezierInfo.json");
    }
    else if (key == 'P'){
        myBezier.updateGeoCoordsFromPixel();
        //myBezier.savePoints("ofxBezierInfo.json");
    }
    else if (key == 'g'){
        myBezier.updatePixelCoordsFromGeo();
    }
    else if (key == 'S')
    {
        myBezier.savePoints("ofxBezierInfo.json");
    }
}

std::shared_ptr<glm::vec2> ofApp::coordsToPixelsConvertor(glm::vec2 coords) {
    auto result = std::make_shared<glm::vec2>(tileLayer->geoToPixels(ofxGeo::Coordinate(coords)));
    return result;
}

std::shared_ptr<glm::vec2> ofApp::pixelsToCoordsConvertor(glm::vec2 pixels) {
    ofxGeo::Coordinate geoCoords = tileLayer->pixelsToGeo(pixels);
    auto result = std::make_shared<glm::vec2>(geoCoords.getLatitude(), geoCoords.getLongitude());
    return result;
}
