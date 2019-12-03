#pragma once

#ifndef ofxGenManager_h
#define ofxGenManager_h

#include "ofMain.h"
#include "ofxBasement.h"
#include "ofxAutoReloadedShader.h"

#include "ofxGenBaselayer.h"
#include "ofxGenConstant.h"

namespace ofx {
namespace Gen {
class Manager : public ofx::Base::BaseManager<BaseLayer>
{

public:
	Manager() {}
	Manager(const std::size_t layer_num, const ofFbo::Settings& settings, const std::string& unique_name);
	~Manager();
	Manager(const Manager& mng) = delete;
	Manager& operator=(const Manager& mng) = delete;

	void update(const double delta_time);

	void drawSlotGui();
	void draw2dGui();
	void draw3dGui();

	static void setupBackyard();
	static void drawBackyardGui();
	static void drawUtilGui();

	ofTexture& get3DTexture(int bindpoint = 0) { return fbo_3d_.getTexture(bindpoint); }
	ofTexture& get2DTexture() { return fbo_2d_.getTexture(); }

	const glm::vec3& getCameraPosition() { return cam_.getPosition(); }
	const glm::mat4& getProjectionMatrix() { return cam_.getProjectionMatrix(); }
	glm::mat4& getViewMatrix() { return view_; }	

	std::map<std::string, std::shared_ptr<BaseLayer>>& getProcessMap() { return process_map; };
	std::function<void(int)> solo = [&](int index) {
		for (std::size_t i = 0; i < slots_.size(); ++i) {
			if (i == index) {
				auto layer = this->getByName(slots_[i]->layer_name);
				if (layer) { layer->setAlpha(1.f); }
			}
			else {
				auto layer = this->getByName(slots_[i]->layer_name);
				if (layer) { layer->setAlpha(0.f); }
			}
		}
	};

protected:
	void renderToFbo();
	void layerAdded(GenEventArgs& event );
	void ImGuiImageBottunFited(const ofTexture& src);
	static std::map<std::string, std::shared_ptr<ofTexture>> backyard_data_map_;
	static float cam_speed_;
	static float cam_distance_;
	int width_, height_;
	std::string unique_name_;

	ofEvent<GenEventArgs> gen_event_args_;

	std::size_t layer_num_;
	std::vector<std::unique_ptr<Slot>> slots_;

	ofEasyCam cam_;
	glm::mat4 view_;
	float cam_elapsed_time_;

	ofVboMesh quad_;
	ofFbo fbo_2d_, fbo_3d_;
	float alpha_;
	ofxAutoReloadedShader deferred_composite_shader_;
};
}
}
#endif
