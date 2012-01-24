#pragma once

#include "ofMain.h"
#include "ofxCircularRecorder.h"

class testApp : public ofBaseApp{

	public:
		testApp() : recorder1(10), recorder2(10) {};
		void setup();
		void update();
		void draw();

		void addFrame();
		void nextFrame();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		ofFbo               fbo;
		ofPixels            pix;
		int                 frame_number;
		ofxCircularRecorder recorder1;
		ofxCircularRecorder recorder2;
		
};
