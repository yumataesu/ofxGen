#pragma once

#ifndef ofxGenBaseLayer_h
#define ofxGenBaseLayer_h

#include "ofxBaseBehavior.h"
#include "ofxImGui.h"
#include "ofxAutoReloadedShader.h"

namespace ofx {
namespace Gen {
class BaseLayer : public ofx::Base::Behavior
{
public:
	BaseLayer();
	virtual ~BaseLayer() 
	{
		ofRemoveListener(ofEvents().keyPressed, this, &BaseLayer::keyPressed);
	}
	virtual void setup() = 0;
	virtual void update(const double delta_time) = 0;
	virtual void draw() = 0;
	virtual void keyPressed(ofKeyEventArgs& args) {};
	virtual void bang() {}
	virtual void drawGui();
	virtual void drawGuiExt() {}

	void   setAlpha(const float& alpha) { alpha_ = alpha; };
	float& getAlpha() { return alpha_; } //for use imgui
	float  getAlpha() const { return alpha_; }
	void   setTarget(const int target_frame_index) { slot_index_ = target_frame_index; }
	int    getTarget() { return slot_index_; }

	void   set3D() { is_3d_ = true; }
	void   set2D() { is_3d_ = false; }
	bool   is3D() { return is_3d_; }

	ofxAutoReloadedShader& getRenderShader() { return render_shader_; }

	void beforeSetup(int width, int height);
	void beforeUpdate();
	void beforeDraw();

	ofParameter<float> speed;
	ofParameter<ofFloatColor> main_col;
protected:
	float width_, height_;
	float alpha_;
	bool is_3d_;
	int slot_index_;
	std::string current_preset_name_;
	ofxAutoReloadedShader render_shader_;

private:
	void load(const std::string& filepath);
	bool save(const std::string& filepath);
	int current_preset_index_;
	std::vector<std::string> preset_names_;
};
}
}
#endif /* BaseLayer_h */
