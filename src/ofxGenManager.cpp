#include "ofxGenManager.h"

namespace ofx {
namespace Gen {
std::map <std::string, std::shared_ptr<ofTexture>> Manager::backyard_data_map_;

Manager::Manager(const std::size_t layer_num, const ofFbo::Settings& settings)
	: layer_num_(layer_num)
	, time_(0.f)
	, alpha_(1.f)
	, cam_speed_(0.1f)
	, cam_distance_(10.f)
{
	deferred_composite_shader_.load("composite/depth_composite");
	lighting_shader_.load("composite/lighting");
	quad_.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
	quad_.addVertex(ofVec3f(1.0, 1.0, 0.0)); // top-right
	quad_.addTexCoord(ofVec2f(1.0f, 1.0f));
	quad_.addVertex(ofVec3f(1.0, -1.0, 0.0)); //bottom-right
	quad_.addTexCoord(ofVec2f(1.0f, 0.0f));
	quad_.addVertex(ofVec3f(-1.0, -1.0, 0.0)); //bottom-left
	quad_.addTexCoord(ofVec2f(0.0f, 0.0f));
	quad_.addVertex(ofVec3f(-1.0, 1.0, 0.0)); //top-left
	quad_.addTexCoord(ofVec2f(0.0f, 1.0f));

	if (layer_num_ > 8) {
		ofLogError("Max layer num is 8.");
		layer_num_ = 8;
	}
	frames_.reserve(layer_num_);
	for (std::size_t i = 0; i < layer_num_; ++i) {
		auto frm = std::make_unique<Frame>();
		frm->fbo.allocate(settings);
		frm->fbo.createAndAttachTexture(GL_RGBA32F, 1);
		frm->index = -1;
		frm->clear();

		frames_.emplace_back(std::move(frm));
	}

	ofDisableArbTex();
	fbo_2d_.allocate(settings);
	fbo_3d_.allocate(settings);
	fbo_3d_.createAndAttachTexture(GL_RGBA32F, 1);
	fbo_3d_.createAndAttachTexture(GL_RGBA32F, 2);
	final_fbo_.allocate(settings);

	result_fbo_ = std::make_shared<ofFbo>();
	result_fbo_->allocate(settings);

	int num = 5;
	lights.reserve(num);
	for (int i = 0; i < num; ++i) {
		lights.emplace_back(ofx::Gen::Light());
	}

	ssao = std::make_unique<ofx::Ssao>(settings.width, settings.height);

	ofAddListener(gen_event_, this, &Manager::layerAdded);
	ofAddListener(ofEvents().update, this, &Manager::update, OF_EVENT_ORDER_APP);
}




Manager::~Manager() {
	ofRemoveListener(gen_event_, this, &Manager::layerAdded);
	ofRemoveListener(ofEvents().update, this, &Manager::update, OF_EVENT_ORDER_APP);	
}




void Manager::update(ofEventArgs& arg) {
	delta_time_ = ofGetLastFrameTime();
	for (const auto& layer : this->process_map) {
		if (layer.second->getAlpha() > 0.f) {
			layer.second->update(delta_time_);
		}
	}

	time_ += delta_time_ * cam_speed_;

	cam_.orbit(time_, time_, cam_distance_);

	for (auto& light : lights) {
		light.update(delta_time_);
	}

	this->renderToFbo();
	this->composite();
	this->lightingPass();
}


void Manager::layerAdded(GenEvent & event) {
	auto it = this->process_map.find(event.target_layer_name);
	if (it != this->process_map.end())
		return;

	auto layer = add(event.target_layer_name, (int)result_fbo_->getWidth(), (int)result_fbo_->getHeight());
	layer->setTarget(event.taeget_fbo_index);
	layer->setName(event.target_layer_name);
}



void Manager::drawFrameGui(const std::string & parent_name) {

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	//window_flags |= ImGuiWindowFlags_NoTitleBar;

	auto& style = ImGui::GetStyle();
	style.FramePadding = ImVec2(8.f, 2.f);
	ImVec2 push_window_padding = style.WindowPadding;

	ImVec4* colors = ImGui::GetStyle().Colors;

	int index = 0;
	for (const auto& frm : frames_) {
		colors[ImGuiCol_Button] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
		style.WindowPadding = push_window_padding;
		std::string window_title = "0" + std::to_string(index) + " | " + parent_name;
		style.FramePadding = ImVec2(2.f, 3.f);
		ImGui::Begin(window_title.data(), 0, window_flags);
		style.FramePadding = ImVec2(2.f, 2.f);

		auto layer = this->getByName(frm->layer_name);

		ImVec2 window_size = ImGui::GetWindowSize();
		float offset = 45.0f;
		ImVec2 slider_size = ImVec2(offset - 22.5f, (window_size.x - offset * 2) * 9 / 16);
		ImVec2 thumbnail_size = ImVec2(window_size.x - offset * 2, (window_size.x - offset * 2) * 9 / 16);

		if (layer != nullptr) {
			if (layer->getAlpha() > 0.f) {
				if (ImGui::ImageButton((ImTextureID)(uintptr_t)frm->fbo.getTexture(0).getTextureData().textureID, thumbnail_size, ImVec2(0, 0), ImVec2(1, 1), 0)) {
					layer->bang();
				}
			}
			else {
				if (backyard_data_map_[frm->layer_name]->isAllocated()) ImGui::ImageButton((ImTextureID)(uintptr_t)backyard_data_map_[frm->layer_name]->getTextureData().textureID, thumbnail_size, ImVec2(0, 0), ImVec2(1, 1), 0);
			}
		}
		else {
			ImGui::ImageButton((ImTextureID)(uintptr_t)frm->fbo.getTexture().getTextureData().textureID, thumbnail_size, ImVec2(0, 0), ImVec2(1, 1), 0);
		}


		ImGui::SameLine();
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("_Gen")) {
				IM_ASSERT(payload->DataSize == sizeof(std::string));

				this->remove(frm->layer_name); //remove previous layer resource
				frm->layer_name = "\0";
				frm->index = -1;
				frm->is_3d_scene = false;

				std::string target_layer_name = "\0";
				memcpy((void*)& target_layer_name, payload->Data, sizeof(target_layer_name));

				GenEvent new_event;
				new_event.taeget_fbo_index = index;
				new_event.target_layer_name = target_layer_name;
				new_event.thumbnail = backyard_data_map_[target_layer_name];

				ofNotifyEvent(gen_event_, new_event, this);
			}
		}

		if (layer != nullptr) ImGui::VSliderFloat("##v", slider_size, &layer->getAlpha(), 0.f, 1.f, "");
		else {
			float zero = 0.f;
			ImGui::VSliderFloat("##v", slider_size, &zero, 0.f, 1.f, "");
		}


		// Layer remove UI --------------------------------------------------
		//
		//
		ImGui::SameLine();
		if (ImGui::Button("", ImVec2(slider_size.x, slider_size.x))) {
			this->remove(frm->layer_name);
			frm->clear();
		}

		colors[ImGuiCol_Button] = ImVec4(0.00f, 0.10f, 0.38f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.00f, 0.59f, 0.66f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		//ImGui::BeginChild(window_title.data(), ImVec2(thumbnail_size.x + slider_size.x + style.FramePadding.x * 2, 62), true);
		//ImGui::BeginChild(window_title.data(), ImVec2(window_size.x-8.f, 62), true);
		style.WindowPadding = ImVec2(20.f, 4.f);
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::PushItemWidth(window_size.x * 0.7);
		float v = 0.f;
		ImGui::Text("Parameters:");
		if (layer != nullptr) {
			layer->drawGui();
		}
		//ImGui::EndChild();
		ImGui::End();

		index++;
	}

	style.WindowPadding = push_window_padding;
}




void Manager::renderToFbo() {

	//3D-------------------------------------------------------
	for (std::size_t i = 0; i < layer_num_; ++i) {
		for (const auto& layer : this->process_map) {

			if (layer.second->getTarget() == i) {
				frames_[i]->layer_name = layer.first;
				frames_[i]->is_3d_scene = layer.second->is3D();
				
				if (frames_[i]->is_3d_scene) glEnable(GL_DEPTH_TEST);
				else glDisable(GL_DEPTH_TEST);

				frames_[i]->fbo.begin();
				frames_[i]->fbo.activateAllDrawBuffers();
				ofClear(0.f);
				if (frames_[i]->is_3d_scene) {
					cam_.begin();
					view_ = ofGetCurrentViewMatrix();
				}
				layer.second->render_shader.begin();
				layer.second->render_shader.setUniformMatrix4f("view", view_);
				layer.second->render_shader.setUniformMatrix4f("projection", cam_.getProjectionMatrix());
				layer.second->render_shader.setUniform1f("u_alpha", layer.second->getAlpha());
				layer.second->draw();
				layer.second->render_shader.end();
				if (frames_[i]->is_3d_scene) cam_.end();
				frames_[i]->fbo.end();
			}
		}
	}


	//2D-------------------------------------------------------
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_MAX);
	fbo_2d_.begin();
	ofClear(0.f, 0.f);
	for (const auto& frm : frames_) {
		if (!frm->is_3d_scene) {
			auto layer = this->getByName(frm->layer_name);

			float alpha = 0.f;
			if (layer != nullptr) alpha = layer->getAlpha();
			else alpha = 0.f;

			ofSetColor(255 * alpha, 255 * alpha);
			frm->fbo.draw(0, 0);
		}
	}
	fbo_2d_.end();
	glDisable(GL_BLEND);


	//depth composite deferred textures-------------------------------------------------------
	fbo_3d_.begin();
	fbo_3d_.activateAllDrawBuffers();
	ofClear(0);
	deferred_composite_shader_.begin();
	int i = 0;
	int col_index = 0; 
	int depth_index = 1; 
	int position_index = 2;

	for (const auto& frm : frames_) {
		if (frm->is_3d_scene) {
			std::string col_name = "u_col" + std::to_string(i);
			std::string depth_name = "u_depth" + std::to_string(i);
			std::string position_name = "u_position" + std::to_string(i);

			const auto& layer = this->getByName(frm->layer_name);
			float alpha = 0.f;
			if (layer != nullptr) {
				alpha = layer->getAlpha();
				deferred_composite_shader_.setUniform1f("u_alpha" + std::to_string(i), alpha);
			}
			else {
				deferred_composite_shader_.setUniform1f("u_alpha" + std::to_string(i), 0.f);
			}
			deferred_composite_shader_.setUniformTexture(depth_name, frm->fbo.getDepthTexture(), depth_index);
			deferred_composite_shader_.setUniformTexture(position_name, frm->fbo.getTexture(1), position_index);
			deferred_composite_shader_.setUniformTexture(col_name, frm->fbo.getTexture(0), col_index);

			col_index += 3;
			depth_index += 3;
			position_index += 3;
			i++;
		}
	}
	deferred_composite_shader_.setUniform1i("u_layer_num", i);
	quad_.draw();
	deferred_composite_shader_.end();
	fbo_3d_.end();
}



void Manager::drawUtilGui() {
	ImGui::Begin("Camera");
	ImGui::SliderFloat("Distance", &cam_distance_, 10.f, 1000.f);
	ImGui::SliderFloat("Speed", &cam_speed_, 0.1f, 30.0f);
	ImGui::End();
}




void Manager::composite() {

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_MAX);

	result_fbo_->begin();
	ofClear(0.f);
	ofSetColor(255, 255 * alpha_);
	final_fbo_.draw(0, 0);
	fbo_2d_.draw(0, 0);
	result_fbo_->end();

	glDisable(GL_BLEND);
}


void Manager::lightingPass() {

	ssao->process(fbo_3d_.getTexture(1), fbo_3d_.getTexture(0), view_, cam_.getProjectionMatrix());

	final_fbo_.begin();
	ofClear(0);
	lighting_shader_.begin();
	lighting_shader_.setUniformTexture("color_tex", fbo_3d_.getTexture(0), 0);
	lighting_shader_.setUniformTexture("position_tex", fbo_3d_.getTexture(1), 1);
	lighting_shader_.setUniformTexture("ssao", ssao->getTexture(), 2);
	lighting_shader_.setUniformMatrix4f("view", view_);

	lighting_shader_.setUniform1i("light_num", lights.size());
	for (int i = 0; i < lights.size(); ++i) {
		lighting_shader_.setUniform4fv("lights[" + to_string(i) + "].diffuse", &lights[i].diffuse_color[0], 1);
		lighting_shader_.setUniform3fv("lights[" + to_string(i) + "].position", &lights[i].getGlobalPosition()[0], 1);
		//lighting_shader_.setUniform1iv("lights[" + to_string(i) + "].isDirectinal", &lights[i].is_directional, 1);
	}
	quad_.draw();
	lighting_shader_.end();
	final_fbo_.end();
};


void Manager::setupBackyard() {
	ofDisableArbTex();
	for (auto& name : registered_names()) {
		auto t = std::make_unique<ofTexture>();
		ofLoadImage(*t, "generative/" + name + ".png");
		backyard_data_map_[name] = std::move(t);
	}
	ofEnableArbTex();
}




void Manager::drawBackyardGui() {

	auto& style = ImGui::GetStyle();
	style.FramePadding = ImVec2(2.f, 3.f);

	ImGui::Begin("BACKYARD");
	style.FramePadding = ImVec2(2.f, 2.f);

	int view_index = 1;
	for (auto& data : backyard_data_map_) {
		std::string window_title = "GEN-PREVIEW" + std::to_string(view_index);
		//ImGui::BeginChild(window_title.data(), ImVec2(75, 42));
		if (data.second->isAllocated()) ImGui::ImageButton((ImTextureID)(uintptr_t)data.second->getTextureData().textureID, ImVec2(80, 45));
		if (ImGui::BeginDragDropSource()) {
			ImGui::SetDragDropPayload("_Gen", &data.first, sizeof(data.first), ImGuiCond_Once);
			ImGui::ImageButton((ImTextureID)(uintptr_t)data.second->getTextureData().textureID, ImVec2(128, 72), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), 0);
			ImGui::EndDragDropSource();
		}
		//ImGui::EndChild();
		if (view_index % 9 != 0) ImGui::SameLine();
		view_index++;
	}
	ImGui::End();
}



void Manager::draw() const { result_fbo_->draw(0, 0); }
void Manager::draw(int x, int y) const { result_fbo_->draw(x, y); }
void Manager::draw(int x, int y, int w, int h) const { result_fbo_->draw(x, y, w, h); }

void Manager::drawFinal() { final_fbo_.draw(0, 0); }

void Manager::draw3D(int attachpoint) const { fbo_3d_.getTexture(attachpoint).draw(0, 0); }
void Manager::draw3D(int x, int y, int attachpoint) const { fbo_3d_.getTexture(attachpoint).draw(x, y); }
void Manager::draw3D(int x, int y, int w, int h, int attachpoint) const { fbo_3d_.getTexture(attachpoint).draw(x, y, w, h); }

void Manager::draw2D() const { fbo_2d_.draw(0, 0); }
void Manager::draw2D(int x, int y) const { fbo_2d_.draw(x, y); }
void Manager::draw2D(int x, int y, int w, int h) const { fbo_2d_.draw(x, y, w, h); }

}
}