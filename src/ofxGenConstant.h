#pragma once

#ifndef ofxGenConstant_h
#define ofxGenConstant_h

#include "ofFbo.h"
#include "ofEvent.h"
#include <memory>

namespace ofx {
	namespace Gen {

		struct Frame {
			int index;
			std::string layer_name;
			bool is_3d_scene;
			ofFbo fbo;
		};

		struct BackyardData {
			std::shared_ptr<ofTexture> thumbnail;
			std::string layer_name;
		};

		struct GenEvent {
			std::string target_layer_name;
			int taeget_fbo_index;
		};
	}
}
#endif /* Constant_h */
