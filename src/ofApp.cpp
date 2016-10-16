/*
 *  10.16.2016
 *  1.2
 */
#include "ofApp.h"
//控制台上色
#define FOREGROUND_BLUE      0x0001 // text color contains blue.
#define FOREGROUND_GREEN     0x0002 // text color contains green.
#define FOREGROUND_RED       0x0004 // text color contains red.
#define FOREGROUND_INTENSITY 0x0008 // text color is intensified.
#define BACKGROUND_BLUE      0x0010 // background color contains blue.
#define BACKGROUND_GREEN     0x0020 // background color contains green.
#define BACKGROUND_RED       0x0040 // background color contains red.
#define BACKGROUND_INTENSITY 0x0080 // background color is intensified.
BOOL SetConsoleColor(WORD wAttributes)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE)
        return FALSE;

    return SetConsoleTextAttribute(hConsole, wAttributes);
}
//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetWindowTitle("SeamlessPainter");
    //初始化中文显示
    ios::sync_with_stdio(false);    // Linux gcc.
    locale::global(locale(""));
    setlocale(LC_CTYPE, "");    // MinGW gcc.
    wcout.imbue(locale(""));

    readInfo();

    ofDisableArbTex();
    tiledTexture.loadImage("test.png");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    tiledTexture.getTextureReference().setTextureWrap(GL_REPEAT, GL_REPEAT);


    colorPicker0.setSize(0,100,100,200);
    colorPicker0.setColorRadius( 0.0 );
    colorPicker0.setColorAngle( 0. );
    colorPicker0.setColorScale(0.5);

    colorPicker1.setSize(0,300,100,200);
    colorPicker1.setColorRadius( 0.0 );
    colorPicker1.setColorAngle( 0.83 );


    ofFbo::Settings s = ofFbo::Settings();
    s.width = infoWidth;
    s.height = infoHeight;
    s.internalformat = GL_RGBA;
    s.useDepth = false;
    // depthStencilAsTexture is not supported!
    s.depthStencilAsTexture = false;
    s.minFilter = GL_NEAREST;//GL_NEAREST
    s.maxFilter = GL_NEAREST;//GL_LINEAR
    drawCanvas.allocate(s);



    drawCanvas.begin();
    ofClear(0,0,0,255);//这个才是黑色
    tiledTexture.draw(0,0);
    drawCanvas.end();

    //重复绘制的image初始化
    ofTexture::isSmooth = false;
    ofPixels pixel;
    drawCanvas.readToPixels(pixel);
    canvas.setFromPixels(pixel);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    canvas.getTextureReference().setTextureWrap(GL_REPEAT, GL_REPEAT);

    scaleNum = scaleNum*16/infoWidth;//防止变更大小的时候一开始就铺满全屏



    SetConsoleColor(FOREGROUND_RED  | FOREGROUND_INTENSITY /*| BACKGROUND_RED*/);
    wcout << L"\n说明 此控制台窗口为提示窗口\n请不要关闭（关闭会导致程序崩溃）退出程序请关闭主窗口，或者键盘esc键"<<endl;
    SetConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY /*| BACKGROUND_RED*/);
    wcout << L"下面是操作说明"<<endl;
    wcout << L"左键绘制颜色为第一个调色器中的"<<endl;
    wcout << L"右键绘制颜色为第二个调色器中的"<<endl;
    wcout << L"中键按住上下拖拽为缩放"<<endl;
    wcout << L"按住a键不放，滑动鼠标为预览平铺纹理"<<endl;
    wcout << L"ctrl+鼠标左键为左键颜色吸取下方颜色"<<endl;
    wcout << L"ctrl+鼠标右键为右键颜色吸取下方颜色"<<endl;
    wcout << L"shift+鼠标左键为清空为第一个调色器中颜色"<<endl;
    wcout << L"s为保存，弹出窗口中记得保存名字和路径不能有中文或者特殊字符"<<endl;
}

//--------------------------------------------------------------
void ofApp::update()
{
    colorPicker0.update();
    colorPicker1.update();
}

//--------------------------------------------------------------
void ofApp::draw()
{
    //将中间中标换算到左上角的 用于取色 画色
    ofPoint transformPoint =ofPoint(ofGetWidth()/2/scaleNum-drawCanvas.getWidth()/2,ofGetHeight()/2/scaleNum-drawCanvas.getHeight()/2);
    ofPoint mousePoint = ofPoint(ofGetMouseX()/scaleNum,ofGetMouseY()/scaleNum);
    drawPoint = mousePoint-transformPoint;



    //float scaleNum = ofMap(ofGetMouseX(),0,ofGetWidth(),0.5,15);

    ofPushMatrix();
    ofScale(scaleNum,scaleNum);//这里是反的 先放大好理解
    ofTranslate(ofGetWidth()/2/scaleNum-drawCanvas.getWidth()/2,ofGetHeight()/2/scaleNum-drawCanvas.getHeight()/2);
    drawCanvas.draw(0,0);
    canvas.draw(-drawCanvas.getWidth()/2*2,0);
    canvas.draw(-drawCanvas.getWidth()/2*2,-drawCanvas.getHeight()/2*2);
    canvas.draw(-drawCanvas.getWidth()/2*2,drawCanvas.getHeight()/2*2);

    canvas.draw(0,-drawCanvas.getHeight()/2*2);
    canvas.draw(0,drawCanvas.getHeight()/2*2);

    canvas.draw(drawCanvas.getWidth()/2*2,0);
    canvas.draw(drawCanvas.getWidth()/2*2,-drawCanvas.getHeight()/2*2);
    canvas.draw(drawCanvas.getWidth()/2*2,drawCanvas.getHeight()/2*2);

    ofPushStyle();
    ofSetHexColor(0xff0000);
    ofNoFill();
    ofRect(0,0,drawCanvas.getWidth(),drawCanvas.getHeight());
    ofPopStyle();

    ofPopMatrix();

    //绘制ui界面
    tiledTexture.draw(0,0);

    ofPushStyle();
    ofSetHexColor(0xff0000);
    ofCircle(drawPoint,1);
    ofPopStyle();


    colorPicker0.draw();
    colorPicker1.draw();

    if(ofGetKeyPressed('a'))
    {
        float tempScaleNum = ofMap(ofGetMouseX(),0,ofGetWidth(),1,10);
        float tx0 = 0;
        float ty0 = 0;
        float tx1 = ofGetWidth()/canvas.getWidth()/tempScaleNum;//横向重复
        float ty1 = ofGetHeight()/canvas.getHeight()/tempScaleNum;//纵向重复
        float px0 = 0;
        float py0 = 0;
        float px1 = ofGetWidth();
        float py1 = ofGetHeight();

        canvas.bind();
        glBegin( GL_QUADS );//可用mesh替代
        glTexCoord2f(tx0,ty0);
        glVertex3i(px0, py0,0);
        glTexCoord2f(tx1,ty0);
        glVertex3i(px1, py0,0);
        glTexCoord2f(tx1,ty1);
        glVertex3i(px1, py1,0);
        glTexCoord2f(tx0,ty1);
        glVertex3i(px0, py1,0);
        glEnd();
        canvas.unbind();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if (key == 's')
    {
        processedImages.clear();
        processedImages.push_back(canvas);
        if (processedImages.size()==0)
        {
            //User is trying to save without anything to output - bail
            return;
        }

        //ofFileDialogResult saveFileResult = ofSystemSaveDialog(ofGetTimestampString() + "." + ofToLower(originalFileExtension), "Save your file");
        ofFileDialogResult saveFileResult = ofSystemSaveDialog("", "Save your file");
        if (saveFileResult.bSuccess)
        {
            processedImages[0].saveImage(saveFileResult.filePath+".png");
            cout <<"\n"<<endl;
            wcout << L"已经成功保存在"<<endl;
            cout << saveFileResult.filePath+".png\n"<<endl;
        }
    }


    shiftDown = ((GetKeyState( VK_SHIFT ) & 0x80) > 0);
    ctrlDown = ((GetKeyState( VK_CONTROL ) & 0x80) > 0);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
    shiftDown = ((GetKeyState( VK_SHIFT ) & 0x80) > 0);
    ctrlDown = ((GetKeyState( VK_CONTROL ) & 0x80) > 0);
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y )
{

}
void ofApp::mousePressOrDrag(int x, int y, int button)
{
    //防止出边界到界面区
    if(x>100)
    {
        //超出边界折回
        if(drawPoint.x>=0)drawPoint.x = (int)drawPoint.x % (int)drawCanvas.getWidth();
        else drawPoint.x = ((int)drawCanvas.getWidth()+(int)ceil(drawPoint.x)% (int)drawCanvas.getWidth()-1) ;
        if(drawPoint.y>=0)drawPoint.y = (int)drawPoint.y % (int)drawCanvas.getHeight();
        else drawPoint.y = ((int)drawCanvas.getHeight()+(int)ceil(drawPoint.y)% (int)drawCanvas.getHeight()-1) ;




        if(shiftDown)//填充色
        {
            ofPushStyle();
            drawCanvas.begin();
            ofSetColor(colorPicker0.getColor());
            ofRect(0,0,drawCanvas.getWidth(),drawCanvas.getHeight());
            drawCanvas.end();
            ofPixels pixel;
            drawCanvas.readToPixels(pixel);
            canvas.setFromPixels(pixel);
            ofPopStyle();
        }
        else
        {
            ofPixels pixel;
            drawCanvas.readToPixels(pixel);


//            if(ctrlDown)colorPicker0.setColor(pixel.getColor(drawPoint.x,drawPoint.y));
            if(ctrlDown)
            {
                if(button == 0)colorPicker0.setColor(pixel.getColor(drawPoint.x,drawPoint.y));//设置前景色
                if(button == 2)colorPicker1.setColor(pixel.getColor(drawPoint.x,drawPoint.y));//设置背景色
            }
            else
            {
                if(button == 0)pixel.setColor(drawPoint.x,drawPoint.y,colorPicker0.getColor());//前景色
                if(button == 2)pixel.setColor(drawPoint.x,drawPoint.y,colorPicker1.getColor());//背景色
                canvas.setFromPixels(pixel);
                drawCanvas.begin();
                canvas.draw(0,0);
                drawCanvas.end();
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
    //中键推拉缩放
    if(button == 1)scaleNum = ofMap(y,0,ofGetHeight(),1,50)/infoWidth*16;

    mousePressOrDrag(x,y,button);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
    mousePressOrDrag(x,y,button);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{

}
void ofApp::readInfo()
{
    wcout << L"本程序适用于像素平铺纹理"<<endl;
    wcout << L"下面是修改info.txt文本的行列数 行列数可以不相等，但建议不要相差太大"<<endl;
    wcout << L"info.txt文本中第一个是行，第二个是列，可自行修改保存，建议数值为8~64"<<endl;
    bool isExist = false;
    vector <string> urlInfo;
    // this is our buffer to stroe the text data
    ofBuffer buffer = ofBufferFromFile("info.txt");
    if(buffer.size())
    {
        // we now keep grabbing the next line
        // until we reach the end of the file
        while(buffer.isLastLine() == false)
        {

            // move on to the next line
            string line = buffer.getNextLine();

            // copy the line to draw later
            // make sure its not a empty line
            if(line.empty() == false)
            {
                urlInfo.push_back(line);
            }

            // print out the line
            cout << line << endl;

        }
        isExist = true;
    }
    if(isExist)
    {
        infoWidth = ofToInt(urlInfo[0]);
        infoHeight = ofToInt(urlInfo[1]);
    }

}
