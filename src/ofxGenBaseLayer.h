#pragma once

#ifndef ofxGenBaseLayer_h
#define ofxGenBaseLayer_h

#include "ofxBaseBehavior.h"

namespace ofx {
	namespace Gen {
		class BaseLayer : public ofx::Base::Behavior {
		public:

			BaseLayer()
				: is_3d_(false)
				, alpha_(0.f)
				, target_frame_index_(-1)
			{}

			BaseLayer(std::size_t target_frame_index)
				: is_3d_(false)
				, alpha_(0.f)
				, target_frame_index_(target_frame_index)
			{}

			virtual ~BaseLayer() {};
			virtual void update(const float& delta_time) = 0;
			virtual void draw() {};
			virtual void bang() {}

			void setAlpha(const float& alpha) { alpha_ = alpha; };
			float& getAlpha() { return alpha_; }
			float getAlpha() const { return alpha_; }

			void setTarget(const int target_frame_index) { target_frame_index_ = target_frame_index; }
			int getTarget() { return target_frame_index_; }

			void set3D() { is_3d_ = true; }
			void set2D() { is_3d_ = false; }
			bool is3D() { return is_3d_; }

		protected:
			float alpha_;
			bool is_3d_;
			int target_frame_index_;
		};
	}
}
#endif /* BaseLayer_h */
