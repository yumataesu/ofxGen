#pragma once

#ifndef ofxGenManager_h
#define ofxGenManager_h

#include "ofMain.h"
#include "ofxBasement.h"
#include "ofxAutoReloadedShader.h"
#include "ofxSSAO.h"

#include "ofxGenBaselayer.h"
#include "ofxGenConstant.h"
#include "asset/Light.h"
#include "asset/Camera.h"

namespace ofx {
namespace Gen {
class Manager : public ofx::Base::BaseManager<BaseLayer> 
{

public:
	Manager() {}
	Manager(const std::size_t layer_num, const ofFbo::Settings& settings);
	virtual ~Manager();
	Manager(const Manager& mng) = delete;
	Manager& operator=(const Manager& mng) = delete;

	void drawFrameGui(const std::string& parent_name);
	void drawUtilGui();
	static void drawUtilGuiShared();

	static void setupBackyard();
	static void drawBackyardGui();

	void draw() const;
	void draw(int x, int y) const;
	void draw(int x, int y, int w, int h) const;

	void drawFinal();
	
	void draw3D(int attachpoint = 0) const;
	void draw3D(int x, int y, int attachpoint = 0) const;
	void draw3D(int x, int y, int w, int h, int attachpoint = 0) const;

	void draw2D() const;
	void draw2D(int x, int y) const;
	void draw2D(int x, int y, int w, int h) const;

	ofFbo& getPostProcessedFbo() { return final_fbo_; }
	ofFbo& get3Dfbo() { return fbo_3d_; }
	ofFbo& get2Dfbo() { return fbo_2d_; }
	std::shared_ptr<ofFbo>& getResultFbo() { return result_fbo_; }
	ofTexture& drawSSAO() { return ssao->getTexture(); }

	std::map<std::string, std::shared_ptr<BaseLayer>>& getProcessMap() { return process_map; };
	std::function<void(int)> solo = [&](int index) {
		for (std::size_t i = 0; i < frames_.size(); ++i) {
			if (i == index) {
				auto layer = this->getByName(frames_[i]->layer_name);
				if (layer) { layer->setAlpha(1.f); }
			}
			else {
				auto layer = this->getByName(frames_[i]->layer_name);
				if (layer) { layer->setAlpha(0.f); }
			}
		}
	};

protected:
	void update(ofEventArgs& arg);
	void renderToFbo();
	void composite();
	void lightingPass();
	void layerAdded(GenEvent& event);
	static std::map<std::string, std::shared_ptr<ofTexture>> backyard_data_map_;
	ofEvent<GenEvent> gen_event_;

	std::size_t layer_num_;
	std::vector<std::unique_ptr<Frame>> frames_;

	ofFbo final_fbo_;
	ofVboMesh quad_;
	ofxAutoReloadedShader lighting_shader_;
	ofxAutoReloadedShader deferred_composite_shader_;
	ofFbo fbo_2d_, fbo_3d_;
	std::shared_ptr<ofFbo> result_fbo_;
	float alpha_;

	//camera
	ofEasyCam cam_;
	glm::mat4 view_;
	float cam_speed_, cam_distance_;
	float time_;

	//postprocess
	std::vector<ofx::Gen::Light> lights;
	std::unique_ptr<ofx::Ssao> ssao;
};
}
}
#endif
