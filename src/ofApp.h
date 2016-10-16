#pragma once

#include "ofMain.h"
#include "ofxColorPicker.h"
#include "ColorRect.h"
#define trace(x) std::cout << #x << " = " << x << std::endl;
class ofApp : public ofBaseApp
{

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);

    void mousePressOrDrag(int x, int y, int button);

    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void readInfo();
    int infoWidth = 16;
    int infoHeight = 16;
    bool ctrlDown = false;
    bool shiftDown = false;

    ofImage tiledTexture;//保存图片
    float scaleNum = 15;
    ofPoint drawPoint;
    ofFbo drawCanvas;
    ofImage canvas;

    ofxColorPicker	colorPicker0;
    ofxColorPicker	colorPicker1;

    //save img
    vector<ofImage>processedImages;
    string originalFileExtension;
};
