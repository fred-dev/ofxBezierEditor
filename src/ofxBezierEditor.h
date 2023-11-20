#pragma once

#include "ofMain.h"
#include "ofEvents.h"

class draggableVertex {
public:
    glm::vec2 pos = glm::vec2(0,0);
    bool bBeingDragged = false;
    bool bOver = false;
    bool bBeingSelected = false;
    
};

class ofxBezierEditor {

public:
    // Constructors and Destructor
    ~ofxBezierEditor();

    ofxBezierEditor();


    // Vertex and Control Point Containers
    vector<draggableVertex> curveVertices;
    vector<draggableVertex> controlPoint1;
    vector<draggableVertex> controlPoint2;

    // File Operations
    string jsonFileName;
    void loadPoints(string filename);
    void savePoints(string filename);

    // Getters and Setters
    int getCurrentPointToMove() { return currentPointToMove; };
    void setCurrentPointToMove(int p) { currentPointToMove = p; };
    
    int getLastVertexSelected() { return lastVertexSelected; };
    void setLastVertexSelected(int v) { lastVertexSelected = v; };

    // Drawing Functions
    void draw();
    void drawOutline();
    void drawHelp();

    // Mouse and Key Event Handlers
    virtual void mouseMoved(ofMouseEventArgs &args);
    virtual void mouseDragged(ofMouseEventArgs &args);
    virtual void mousePressed(ofMouseEventArgs &args);
    virtual void mouseReleased(ofMouseEventArgs &args);
    virtual void mouseScrolled(ofMouseEventArgs &args);
    virtual void mouseEntered(ofMouseEventArgs &args);
    virtual void mouseExited(ofMouseEventArgs &args);
    virtual void keyPressed(ofKeyEventArgs &args);
    virtual void keyReleased(ofKeyEventArgs &args);

    // Visual Attributes
    int getRadiusVertex() { return radiusVertex; };
    void setRadiusVertex(int r) { radiusVertex = r; };
    
    int getRadiusControlPoints() { return radiusControlPoints; };
    void setRadiusControlPoints(int r) { radiusControlPoints = r; };
    
    bool getFillBezier() { return bfillBezier; };
    void setFillBezier(bool b) { bfillBezier = b; updateAllFromVertices();};

    // Color Control
    void setColorFill(ofColor c);
    void setColorFill(float r, float g, float b, float a = 255.0);
    void setColorFillR(float c);
    void setColorFillG(float c);
    void setColorFillB(float c);
    void setColorFillA(float a);
    
    void setColorStroke(ofColor c);
    void setColorStroke(float r, float g, float b, float a = 255.0);
    void setColorStrokeR(float c);
    void setColorStrokeG(float c);
    void setColorStrokeB(float c);
    void setColorStrokeA(float a);
    
    ofColor getColorFill() { return colorFill; };
    ofColor getColorStroke() { return colorStroke; };

    
    void setClosed(bool b) { bIsClosed = b; updateAllFromVertices(); };
    bool getClosed() { return bIsClosed; };

    ofPoint getCenter() { return center; };
    ofPolyline getPolyline() { return polyLineFromPoints; }
    
    ofVboMesh getRibbonMesh() { return ribbonMesh; };
    bool getUseRibbonMesh() { return bUseRibbonMesh; };
    void setUseRibbonMesh(bool b) { bUseRibbonMesh = b; };
    void setMeshLengthPrecisionMultiplier(int m) { meshLengthPrecisionMultiplier = m; updateAllFromVertices();};
    int getMeshLengthPrecisionMultiplier() { return meshLengthPrecisionMultiplier; };
    float getRibbonWidth() { return ribbonWidth; };
    void setRibbonWidth(float w) { ribbonWidth = w; updateAllFromVertices();};
    
    ofVboMesh getTubeMesh() { return tubeMesh; };
    void setTubeResolution(int r) { tubeResolution = r; updateAllFromVertices();};
    int getTubeResolution() { return tubeResolution; };
    void setTubeRadius(float r) { tubeRadius = r; updateAllFromVertices();};
    float getTubeRadius() { return tubeRadius; };
    
    bool getHasRoundCaps() { return roundCap; };
    void setHasRoundCaps(bool b) { roundCap = b; updateAllFromVertices();};
    
    
    // Event Handling
    void setReactToMouseAndKeyEvents(bool b);
    void registerToEvents();
    void unregisterFromEvents();
    
private:
    // Private Variables
    int currentPointToMove;
    int lastVertexSelected;

    ofJson JSONBezier;
    ofVboMesh ribbonMesh;
    ofVboMesh tubeMesh;
    
    int radiusVertex;
    int radiusControlPoints;

    bool bfillBezier;
    ofColor colorFill;
    ofColor colorStroke;

    ofRectangle boundingBox;
    void updateBoundingBox();
    bool bshowBoundingBox;

    ofPoint center;
    void calculateCenter();

    float translateX, translateY;
    float mouseX, mouseY;

    bool beditBezier;
    bool bIsClosed;
    bool bUseRibbonMesh = false;
    bool bUseTubeMesh = false;
    void drawWithNormals(const ofPolyline& polyline);

    // Colors
    ofColor vertexColour;           // Bright yellow for default vertices
    ofColor vertexHoverColor;       // Semi-transparent yellow when hovering
    ofColor vertexLabelColour;      // Slightly less intense yellow for labels
    ofColor vertexSelectedColour;   // Bright green for selected vertices
    ofColor vertexDraggedColour;    // Slightly different green for dragged vertices
    
    ofColor ctrPtColour;            // Bright purple for default control points
    ofColor ctrPtHoverColor;        // Semi-transparent purple when hovering
    ofColor ctrPtLabelColour;       // Slightly less intense purple for labels
    ofColor ctrPtSelectedColour;    // Bright blue for selected control points
    ofColor ctrPtDraggedColour;     // Slightly different blue for dragged control points

    ofPolyline polyLineFromPoints;
    void updatePolyline();
    
    // Mesh Generation
    void updateAllFromVertices();
    int meshLengthPrecisionMultiplier = 1;
    void generateTriangleStripFromPolyline(ofPolyline inPoly);
    float ribbonWidth = 10.0;
    
    void generateTubeMeshFromPolyline(ofPolyline inPoly);
    int tubeResolution = 10;
    float tubeRadius = 10.0;

    void createLatitudeRings(vector<vector<ofVec3f>>& allCircles, vector<vector<ofVec3f>>& allCircleNormals, vector<vector<ofVec2f>>& allCircleTexCoords, const ofVec3f& center, const ofVec3f& extension, float tubeResolution, float tubeRadius,bool forwards);
    
    bool roundCap = true;
    
    void generateCurvedRibbonCap(ofVec3f centre,  ofVec3f tangent, bool forwards);
    
    void generateCurvedTubeCap(vector<vector<ofVec3f>>& allCircles,
                     vector<vector<ofVec3f>>& allCircleNormals,
                     vector<vector<ofVec2f>>& allCircleTexCoords, const ofVec3f& center, const ofVec3f& tangent, const ofVec3f& normal, bool forwards);
    
    
    void generateFlatTubeCap(vector<vector<ofVec3f>>& allCircles,
                     vector<vector<ofVec3f>>& allCircleNormals,
                     vector<vector<ofVec2f>>& allCircleTexCoords, const ofVec3f& flatCapCentre, const ofVec3f& tangent, const ofVec3f& normal, bool forwards);
    
    void addRing(vector<vector<ofVec3f>>& allCircles,
                 vector<vector<ofVec3f>>& allCircleNormals,
                 vector<vector<ofVec2f>>& allCircleTexCoords, const ofVec3f& ringCenter, float radius,const ofVec3f& tangent, const ofVec3f& norma, const ofVec3f& sphereCenter);
    
//    std::vector<float> segmentDistances;
//    
//    void updateSegmentDistances(ofPolyline inPoly);
};

