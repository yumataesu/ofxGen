#pragma once

#ifndef ofxGen_Light_h
#define ofxGen_Light_h

#include "ofNode.h"
#include "ofxImGui.h"

namespace ofx {
namespace Gen {
struct Light : public ofNode
{
	Light()
	{
		radius = ofRandom(50, 250);
		diffuse_color = ofFloatColor(ofRandom(0.0, 1.0), ofRandom(0.0, 1.0), ofRandom(0.0, 1.0), 1.0);
		is_directional = 1;
		seed = ofRandom(0, 10);
	}

	void update(const double delta_time) {
		time += delta_time * speed;
		orbit(360 * sin(time + seed), 360 * cos(time + seed * 2), radius);
	}

	float speed;
	float time;
	float seed;
	float radius;
	ofFloatColor diffuse_color;
	int is_directional;
};
}
}
#endif
