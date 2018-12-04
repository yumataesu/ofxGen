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

			void clear() {
				layer_name = "\0";
				is_3d_scene = false;
				fbo.begin(); ofClear(0.f, 0.f); fbo.end(); //We have to blackout so avoiding to be left last frame in fbo.
			}
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
