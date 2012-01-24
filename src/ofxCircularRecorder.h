#pragma once

#include <boost/circular_buffer.hpp>
#include <stdint.h>
#include "ofxOpenCv.h"
//#include "ofxTwBar.h"
#include "Poco/Mutex.h"

typedef ofPtr<ofxCvColorImage> ofxCvColorImagePtr;

struct Frame {
	ofxCvColorImagePtr img;
	long               duration;
};

typedef ofPtr<Frame> FramePtr;


class ofxCircularRecorder
{
public:

	ofxCircularRecorder(int recorder_buffer_size);

	void          addFrame(ofxCvColorImagePtr& img, double duration = 0);
	void          clear(void);
	uint_fast32_t capacity(void);
	void          draw(void);
	void          draw(float x, float y);
	void          draw(float x, float y, float w, float h);
	bool          empty(void);
	void          firstFrame(void);
	uint_fast32_t getTotalNumFrames(void);
	bool          isPlaying(void);
	//bool          isRecording(void);
	void          nextFrame(void);
	void          play(void);
	void          preAllocate(int width, int height);
	void          setLoopState(ofLoopType state);
	void          setTailFrame(ofxCvColorImagePtr& img, double& duration);
	uint_fast32_t size(void);
	void          stop(void);
	void          update(ofEventArgs& args);

	//static void TW_CALL clearRecorderCallback(void *clientData);
	//static void TW_CALL getTotalNumFramesCallback(void *value, void *clientData);

private:
	
	typedef boost::circular_buffer<FramePtr> PlayBuffer;

	bool _empty(void);
	void _firstFrame(void);
	void _nextFrame(void);
	
	bool                   is_playing;
	bool                   is_pre_allocated;
	ofLoopType             loop_type;
	bool                   head_frame_moved;
	bool                   head_frame_valid;
	PlayBuffer             play_buffer;
	PlayBuffer::iterator   head_frame;
	PlayBuffer::iterator   tail_frame;
	bool                   tail_frame_valid;
	double                 timecode;

	Poco::FastMutex        m;

};