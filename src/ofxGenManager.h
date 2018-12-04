#pragma once

#ifndef ofxGenManager_h
#define ofxGenManager_h

#include "ofMain.h"
#include "ofxBasement.h"

#include "ofxGenBaselayer.h"
#include "ofxGenConstant.h"

//#include "ui/ImGuiKnob.h"
//#include "ui/ImGuiTab.h"

namespace ofx {
namespace Gen {
class Manager : public ofx::Base::BaseManager<BaseLayer> {

public:
	Manager(const std::size_t layer_num, const ofFbo::Settings& settings);
	~Manager();
	void update(const float& delta_time);
	void draw() const;
	void bang();

	void drawFrameGui();

	static void setupBackyard();
	static void drawBackyardGui();

	void draw3D() const { fbo_3d_.draw(0, 0); }
	void draw2D() const { fbo_2d_.draw(0, 0); }

	ofFbo* get3Dfbo() { return &fbo_3d_; }
	ofFbo* get2Dfbo() { return &fbo_2d_; }
	std::shared_ptr<ofFbo>& getResultFbo() { return result_fbo_; }

protected:
	void renderToFbo();
	void composite();

private:
	void layerAdded(GenEvent& event);
	static std::vector<BackyardData> backyard_data_;
	ofEvent<GenEvent> gen_event_;

	std::size_t layer_num_;
	std::vector<std::unique_ptr<Frame>> frames_;
	
	ofEasyCam cam_;
	float time_;


	ofVboMesh quad_;
	ofShader depth_composite_shader_;
	ofFbo fbo_2d_, fbo_3d_;
	std::shared_ptr<ofFbo> result_fbo_;
	float alpha_;
};
}
}
#endif
