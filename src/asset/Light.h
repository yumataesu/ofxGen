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
		speed = ofRandom(0, 20);

		radius = ofRandom(50, 250);
		diffuse_color = ofFloatColor(ofRandom(0.0, 1.0), ofRandom(0.0, 1.0), ofRandom(0.0, 1.0), 1.0);
		is_directional = 1;
		seed = ofRandom(0, 24);
		id_ = id;
		id++;
	}

	void update(const double delta_time) {
		time += delta_time * speed;
		orbit(360 * sin(time + seed), 360 * cos(time + seed * 2), radius);
	}

	static void drawUtilGuiShared() {
		ImGui::Text("Light Util");
		//ImGui::SliderFloat("Light|Rad", &radius, 0.1, 100.f);
	}

	void drawGui() {
		ImGui::ColorEdit4(std::string(std::to_string(id_) + "| Col").data(), &diffuse_color.r);
		ImGui::SliderFloat("Light|Spd", &speed, 0.1, 100.f);
	}

	int id_;
	static int id;
	float radius;
	float speed;
	std::string name;
	float time;
	float seed;
	ofFloatColor diffuse_color;
	int is_directional;
};
}
}
#endif
