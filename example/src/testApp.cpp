#include "testApp.h"

#define WIDTH 100.0f
#define HEIGHT 30.0f

//--------------------------------------------------------------
void testApp::setup()
{
	ofSetFrameRate(30);
	fbo.allocate(WIDTH, HEIGHT, GL_RGB, 0);
	frame_number = 0;
	recorder1.play();
	/*recorder2.preAllocate(WIDTH, HEIGHT);
	recorder2.play();*/
	addFrame();
}

//--------------------------------------------------------------
void testApp::update()
{

}

//--------------------------------------------------------------
void testApp::draw()
{

	ofSetHexColor(0x000000);
	ofDrawBitmapString("       frame_num: " + ofToString(frame_number), 10, 10);

	ofPushMatrix();
	ofTranslate(10, 30);

	ofSetHexColor(0xffffff);
	recorder1.draw(0, 0);

	ofSetHexColor(0x000000);
	ofDrawBitmapString("total num frames: " + ofToString(recorder1.getTotalNumFrames()), 0, 80);
	ofDrawBitmapString("            size: " + ofToString(recorder1.size()), 0, 100);
	ofDrawBitmapString("        capacity: " + ofToString(recorder1.capacity()), 0, 120);
	
	//ofPushMatrix();
	//ofTranslate(200, 0);

	//ofSetHexColor(0xffffff);
	//recorder2.draw(0, 0);

	//ofSetHexColor(0x000000);
	//ofDrawBitmapString("total num frames: " + ofToString(recorder2.getTotalNumFrames()), 0, 80);
	//ofDrawBitmapString("            size: " + ofToString(recorder2.size()), 0, 100);
	//ofDrawBitmapString("        capacity: " + ofToString(recorder2.capacity()), 0, 120);

	//ofPopMatrix();

	ofDrawBitmapString("'a' : add a new frame",     0, 160);
	ofDrawBitmapString("'c' : clear the recorder",  0, 180);
	ofDrawBitmapString("'n' : next frame",          0, 200);
	ofDrawBitmapString("'f' : first frame",         0, 220);

	ofPopMatrix();
}

void testApp::addFrame()
{
	++frame_number;

	// draw some stuff into a frame buffer object
	ofPushStyle();
	fbo.begin();

	// render a grey background
	ofSetHexColor(0x999999);
	ofRect(0, 0, WIDTH, HEIGHT);
	
	// render the frame number 
	ofSetHexColor(0x00ff00);
	ofDrawBitmapString(ofToString(frame_number), 10, 20);

	fbo.end();
	ofPopStyle();

	// read the frame buffer object back out into an ofxCvColorImage
	fbo.readToPixels(pix);

	// prepare the image to be added to recorder 1
	ofxCvColorImagePtr img1 = ofxCvColorImagePtr(new ofxCvColorImage);
	img1->allocate(WIDTH, HEIGHT);
	img1->setFromPixels(pix.getPixels(), WIDTH, HEIGHT);

	// add the image to the recorder w/1 second duration
	recorder1.addFrame(img1, 1.0);

	/*ofxCvColorImagePtr img2;
	double duration;
	recorder2.setTailFrame(img2, duration);

	img2->setFromPixels(pix.getPixels(), WIDTH, HEIGHT);
	duration = 1.0;*/
}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key)
{
	switch(key) {
	case 'a':
		addFrame();
		break;
	case 'c':
		recorder1.clear();
		break;
	case 'f':
		recorder1.firstFrame();
		break;
	case 'n':
		recorder1.nextFrame();
		break;
	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y )
{

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo)
{ 

}