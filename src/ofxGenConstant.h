#pragma once

#ifndef ofxGenConstant_h
#define ofxGenConstant_h

#include "ofFbo.h"
#include "ofEvent.h"
#include <memory>

namespace ofx {
namespace Gen {
struct Slot {
	int index;
	std::string layer_name;
	bool is_3d_scene;
	std::shared_ptr<ofTexture> thumbnail;
	ofFbo fbo;

	void clear() {
		index = -1;
		layer_name = "\0";
		is_3d_scene = false;
		fbo.begin(); ofClear(0.f, 0.f); fbo.end();
	}
};

struct GenEventArgs {
	std::string target_layer_name;
	int target_slot_index;
	std::shared_ptr<ofTexture> thumbnail;
};
}
}
#endif /* Constant_h */
