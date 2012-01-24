#include "ofxCircularRecorder.h"
#include <assert.h>
#include "ofMain.h"
#include "ofEvents.h"

using Poco::FastMutex;

ofxCircularRecorder::ofxCircularRecorder(int _recorder_buffer_size) 
	: head_frame_moved(false),
	  head_frame_valid(false),
	  is_playing(false),
	  loop_type(OF_LOOP_NORMAL),
	  play_buffer(_recorder_buffer_size),
	  is_pre_allocated(false),
	  tail_frame_valid(false),
	  timecode(0)
{
	ofAddListener(ofEvents.update, this, &ofxCircularRecorder::update);
}

void ofxCircularRecorder::addFrame(ofxCvColorImagePtr& _img, double _duration)
{
	FastMutex::ScopedLock lock(m);

	if (is_pre_allocated) 
		ofLog(OF_LOG_WARNING, "Using addFrame() on a pre-allocated ofxCircularRecorder, you should be using getTailFrame() instead");

	// construct the frame
	FramePtr frame  = FramePtr(new Frame);
	frame->img      = ofxCvColorImagePtr(_img);
	frame->duration = _duration;

	// stick it in the buffer
	play_buffer.push_back(frame);
}

uint_fast32_t ofxCircularRecorder::capacity(void)
{
	FastMutex::ScopedLock lock(m);
	return play_buffer.capacity();
}

void ofxCircularRecorder::clear(void)
{
	FastMutex::ScopedLock lock(m);

	if (!is_pre_allocated)
		play_buffer.clear();
	
	// invalidate the iterators
	head_frame_valid = false;
	tail_frame_valid = false;
}

void ofxCircularRecorder::draw(void)
{
	draw(0, 0);
}

void ofxCircularRecorder::draw(float x, float y)
{
	FastMutex::ScopedLock lock(m);

	// can't draw if there's nuthin' ta draw
	if (_empty())
		return;
	else if (!head_frame_valid)
		_firstFrame();

	(*head_frame)->img->draw(x, y);

	head_frame_moved = false;
}

void ofxCircularRecorder::draw(float x, float y, float w, float h)
{
	FastMutex::ScopedLock lock(m);

	// can't draw if there's nuthin' ta draw
	if (_empty())
		return;
	else if (!head_frame_valid)
		_firstFrame();

	(*head_frame)->img->draw(x, y, w, h);

	head_frame_moved = false;
}

bool ofxCircularRecorder::empty(void)
{
	FastMutex::ScopedLock lock(m);
	return _empty();
}

bool ofxCircularRecorder::_empty(void)
{
	if (is_pre_allocated)
		return (!head_frame_valid && !tail_frame_valid);
	else
		return play_buffer.empty();
}

void ofxCircularRecorder::firstFrame(void)
{
	FastMutex::ScopedLock lock(m);
	_firstFrame();
}

void ofxCircularRecorder::_firstFrame(void)
{
	// we can't rewind if the play_buffer is empty
	if (_empty()) return;

	// start back at the beggining
	head_frame = play_buffer.begin();
	head_frame_valid = true;

	// update with new duration
	timecode = (*head_frame)->duration;

	head_frame_moved = true;
}

void ofxCircularRecorder::setTailFrame(ofxCvColorImagePtr& img, double& duration)
{
	FastMutex::ScopedLock lock(m);

	assert(is_pre_allocated);

	if (!tail_frame_valid) {
		tail_frame = play_buffer.begin();
	} else if (tail_frame == play_buffer.end()) {
		/*PlayBuffer::iterator frame_it = ;
		++frame_it;*/
		play_buffer.rotate(++play_buffer.begin());
		--tail_frame;
	}

	img      = ofxCvColorImagePtr((*tail_frame)->img);
	duration = (*tail_frame)->duration;
}

uint_fast32_t ofxCircularRecorder::getTotalNumFrames(void)
{
	FastMutex::ScopedLock lock(m);

	if (is_pre_allocated) {
		if (!head_frame_valid || !tail_frame_valid)
			return 0;
		else
			return (uint_fast32_t) std::distance(head_frame, tail_frame);
	} else {
		return (uint_fast32_t) play_buffer.size();
	}
}

bool ofxCircularRecorder::isPlaying(void)
{
	FastMutex::ScopedLock lock(m);
	return is_playing && !_empty();
}

void ofxCircularRecorder::nextFrame(void)
{
	FastMutex::ScopedLock lock(m);
	_nextFrame();
}

void ofxCircularRecorder::_nextFrame(void)
{
	if (_empty()) return;

	if (!head_frame_valid) {
		// we have an invalid play head, restart
		_firstFrame();
		return;
	}
		
	// advance the play head to the next frame
	++head_frame;

	// see if we're at the end of the play_buffer
	if (head_frame == play_buffer.end()) {
		switch(loop_type) {
		case OF_LOOP_NORMAL:
			// start back at the beggining
			_firstFrame();
			return;
		case OF_LOOP_PALINDROME:
			assert(false); // not implemented
			//head_frame = play_buffer.rbegin();
			break;
		default:
			// do nothing, we'll just keep redrawing this frame
			break;
		}
	}

	// update with new duration
	timecode = (*head_frame)->duration;

	head_frame_moved = true;

}

void ofxCircularRecorder::play(void)
{
	FastMutex::ScopedLock lock(m);
	is_playing = true;
}

void ofxCircularRecorder::preAllocate(int _width, int _height)
{
	FastMutex::ScopedLock lock(m);

	// flag our recorder as "special"
	is_pre_allocated = true;

	bool empty = play_buffer.empty();

	if (!empty) {
		// set our tail_frame iterator to the right spot
		tail_frame = play_buffer.begin();
		while ((*tail_frame)->img->bAllocated)
			++tail_frame;
	}

	// get the number of spots we need to fill
	int reserve = play_buffer.reserve();

	// fill up the entire buffer with fully allocated frames
	for (int i = 0; i < reserve; ++i) {
		// construct the frame
		FramePtr frame  = FramePtr(new Frame);
		frame->img      = ofxCvColorImagePtr(new ofxCvColorImage);
		frame->duration = 0;

		frame->img->allocate(_width, _height);

		// stick it in the buffer
		play_buffer.push_back(frame);
	}

}

void ofxCircularRecorder::setLoopState(ofLoopType state)
{
	FastMutex::ScopedLock lock(m);
	loop_type = state;
}

uint_fast32_t ofxCircularRecorder::size(void)
{
	FastMutex::ScopedLock lock(m);
	return play_buffer.size();
}


void ofxCircularRecorder::update(ofEventArgs& args)
{
	FastMutex::ScopedLock lock(m);

	// bail if there's nothing to do
	if (!is_playing || _empty() || head_frame_moved) return;

	// update our countdown on how long the current frame should be displayed
	timecode -= ofGetLastFrameTime();

	// have we displayed the last frame long enough?
	if (timecode <= 0) {
		double slop = timecode;
		// advance the head_frame iterator
		_nextFrame();
		timecode += slop;
	}
}

void ofxCircularRecorder::stop(void)
{
	FastMutex::ScopedLock lock(m);
	is_playing = false;
}

