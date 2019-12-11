#include "ofxGenBaseLayer.h"

namespace ofx {
namespace Gen {
BaseLayer::BaseLayer()
	: is_3d_(false)
	, alpha_(0.f)
	, slot_index_(-1)
	, width_(0)
	, height_(0)
	, elapsed_time(0.f)
	, current_preset_name_("Default")
{
	ofAddListener(ofEvents().keyPressed, this, &BaseLayer::keyPressed);
}


void BaseLayer::drawGui() {
	if (ofxImGui::VectorCombo("Preset", &current_preset_index_, preset_names_)) {
		load("ofxGen/" + parameter_group_.getName() + "/" + preset_names_[current_preset_index_] + ".json");
	}
	ImGui::Separator();
	ofxImGui::AddGroup(parameter_group_);
	drawGuiExt();
	if (ImGui::Button("Update")) {
		save("ofxGen/" + parameter_group_.getName() + "/" + preset_names_[current_preset_index_] + ".json");
	}
	ImGui::SameLine();
	if (ImGui::Button("Save")) {
		ofFileDialogResult save_file_result = ofSystemSaveDialog("preset.json", "Save your file");
		if (save_file_result.bSuccess) {
			if (save(save_file_result.filePath)) {
				preset_names_.emplace_back(ofSplitString(save_file_result.getName(), ".", true)[0]);
			}
		}
	}
}


void BaseLayer::beforeSetup(int width, int height) {
	this->width_ = width;
	this->height_ = height;

	parameter_group_.add(speed.set("Spd", 1.f, 0.01, 10.f));
	parameter_group_.add(main_col.set("Col1", ofFloatColor(1), ofFloatColor(0), ofFloatColor(1)));
	render_shader_.load("ofxGen/" + parameter_group_.getName() + "/render");

	this->setup();

	ofDirectory dir;
	dir.allowExt("json");
	dir.listDir("ofxGen/" + parameter_group_.getName());
	dir.sort();

	if (dir.size() > 0) {
		for (auto& path : dir.getFiles()) {
			preset_names_.emplace_back(ofSplitString(path.getFileName(), ".", true)[0]);
		}
		load(dir.getPath(0));
	}
}

void BaseLayer::load(const std::string& filepath) {
	ofJson json = ofLoadJson(filepath);
	ofDeserialize(json, parameter_group_);
}


bool BaseLayer::save(const std::string& filepath) {
	ofJson json;
	ofSerialize(json, parameter_group_);
	return ofSaveJson(filepath, json);
}

void BaseLayer::beforeUpdate(const double delta_time) {
	auto result = std::find(bang_intervals.begin(), bang_intervals.end(), frame_num);
	frame_num += 1;
	//if (result != bang_intervals.end()) { bang(); }
	if (frame_num > bang_intervals.back()) frame_num = 0;
}
void BaseLayer::beforeDraw() {}
}
}