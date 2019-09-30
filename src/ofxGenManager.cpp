#include "ofxGenManager.h"

namespace ofx {
namespace Gen {
std::map <std::string, std::shared_ptr<ofTexture>> Manager::backyard_data_map_;
float Manager::cam_speed_ = 0.1f;
float Manager::cam_distance_ = 100.f;

Manager::Manager(const std::size_t layer_num, const ofFbo::Settings& settings, const std::string& unique_name)
	: layer_num_(layer_num)
	, alpha_(1.f)
	, unique_name_(unique_name)
	, width_(settings.width)
	, height_(settings.height)
	, cam_elapsed_time_(0.f)
{
	if (!deferred_composite_shader_.load("../../../../addons/ofxGen/assets/composite/depth_composite")) {
		deferred_composite_shader_.unload();
		deferred_composite_shader_.load("../../addons/ofxGen/assets/composite/depth_composite");
	}
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
	slots_.reserve(layer_num_);
	for (std::size_t i = 0; i < layer_num_; ++i) {
		auto slot = std::make_unique<Slot>();
		slot->fbo.allocate(settings);
		slot->fbo.createAndAttachTexture(GL_RGBA32F, 1);
		slot->index = -1;
		slot->clear();
		slots_.emplace_back(std::move(slot));
	}

	ofDisableArbTex();
	fbo_2d_.allocate(settings);
	fbo_3d_.allocate(settings);
	fbo_3d_.createAndAttachTexture(GL_RGBA32F, 1);
	fbo_3d_.createAndAttachTexture(GL_RGBA32F, 2);

	ofAddListener(gen_event_args_, this, &Manager::layerAdded);
}


Manager::~Manager() {
	ofRemoveListener(gen_event_args_, this, &Manager::layerAdded);
}


void Manager::update(const double delta_time) {
	for (const auto& layer : this->process_map) {
		if (layer.second->getAlpha() > 0.f) {
			layer.second->update(delta_time);
		}
	}
	cam_.disableMouseInput();
	cam_elapsed_time_ += delta_time * cam_speed_;
	cam_.orbitRad(PI * cam_elapsed_time_, 0, cam_distance_);

	this->renderToFbo();
}


void Manager::layerAdded(GenEventArgs& args) {
	auto it = this->process_map.find(args.target_layer_name);
	if (it != this->process_map.end())
		return;

	auto& layer = add(args.target_layer_name);
	layer->setTarget(args.target_slot_index);
	layer->setName(args.target_layer_name);
	layer->beforeSetup(width_, height_);
}


void Manager::renderToFbo() {

	//render to slot fbo----------------------------
	for (std::size_t i = 0; i < layer_num_; ++i) {
		for (const auto& layer : this->process_map) {

			if (layer.second->getTarget() == i) {
				slots_[i]->layer_name = layer.first;
				slots_[i]->is_3d_scene = layer.second->is3D();

				if (slots_[i]->is_3d_scene) glEnable(GL_DEPTH_TEST);
				else glDisable(GL_DEPTH_TEST);

				slots_[i]->fbo.begin();
				slots_[i]->fbo.activateAllDrawBuffers();
				ofClear(0.f);
				if (slots_[i]->is_3d_scene) {
					cam_.begin();
					view_ = ofGetCurrentViewMatrix();
					layer.second->getRenderShader().begin();
					layer.second->getRenderShader().setUniformMatrix4f("view", view_);
					layer.second->getRenderShader().setUniformMatrix4f("projection", cam_.getProjectionMatrix());
					layer.second->getRenderShader().setUniform1f("u_alpha", layer.second->getAlpha());
					layer.second->getRenderShader().setUniform4f("u_col", layer.second->main_col);
				}

				layer.second->draw();

				if (slots_[i]->is_3d_scene) {
					layer.second->getRenderShader().end();
					cam_.end();
				}
				slots_[i]->fbo.end();
			}
		}
	}


	//composite 2D-------------------------------------------------------
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_MAX);
	fbo_2d_.begin();
	ofClear(0.f, 0.f);
	for (const auto& frm : slots_) {
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


	//composite 3d depth test in shader-------------------------------------------------------
	ofDisableAlphaBlending();
	fbo_3d_.begin();
	fbo_3d_.activateAllDrawBuffers();
	ofClear(0);
	deferred_composite_shader_.begin();
	int i = 0;
	int col_bindp = 0; 
	int depth_pindp = 1; 
	int position_bindp = 2;

	for (const auto& frm : slots_) {
		if (frm->is_3d_scene) {
			const auto& layer = this->getByName(frm->layer_name);

			if (layer != nullptr && layer->getAlpha() > 0.f) {
				std::string col_name = "layer[" + std::to_string(i) + "].color";
				std::string depth_name = "layer[" + std::to_string(i) + "].depth";
				std::string position_name = "layer[" + std::to_string(i) + "].position";
				std::string alpha_name = "layer[" + std::to_string(i) + "].alpha";

				deferred_composite_shader_.setUniformTexture(depth_name, frm->fbo.getDepthTexture(), depth_pindp);
				deferred_composite_shader_.setUniformTexture(position_name, frm->fbo.getTexture(1), position_bindp);
				deferred_composite_shader_.setUniformTexture(col_name, frm->fbo.getTexture(0), col_bindp);
				deferred_composite_shader_.setUniform1f(alpha_name, layer->getAlpha());

				i++;
				col_bindp += 3;
				depth_pindp += 3;
				position_bindp += 3;
			}
		}
	}
	deferred_composite_shader_.setUniform1i("u_layer_num", i);
	quad_.draw();
	deferred_composite_shader_.end();
	fbo_3d_.end();
	glDisable(GL_BLEND);
}


void Manager::setupBackyard() {
	ofDisableArbTex();
	for (auto& name : registered_names()) {
		auto t = std::make_unique<ofTexture>();
		if (!ofLoadImage(*t, "ofxGen/" + name + "/thumbnail.jpg"))
			if (!ofLoadImage(*t, "../../../../addons/ofxGen/assets/no-thumbnail.png"))
				ofLoadImage(*t, "../../addons/ofxGen/assets/no-thumbnail.png");
		backyard_data_map_[name] = std::move(t);
	}
	ofEnableArbTex();
}


void Manager::drawSlotGui() {

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	auto& style = ImGui::GetStyle();
	style.FramePadding = ImVec2(8.f, 2.f);
	ImVec2 push_window_padding = style.WindowPadding;

	ImVec4* colors = ImGui::GetStyle().Colors;
	auto button_col = colors[ImGuiCol_Button];
	auto button_col_hovered = colors[ImGuiCol_ButtonHovered];
	auto button_col_active = colors[ImGuiCol_ButtonActive];

	int index = 0;
	for (const auto& frm : slots_) {
		style.WindowPadding = push_window_padding;
		std::string window_title = "0" + std::to_string(index) + " | " + unique_name_;
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
				if (ImGui::ImageButton((ImTextureID)(uintptr_t)frm->fbo.getTexture(0).getTextureData().textureID, thumbnail_size, ImVec2(0, 0), ImVec2(1, 1), 0)) layer->bang();
			}
			else if (backyard_data_map_[frm->layer_name]->isAllocated()) ImGui::ImageButton((ImTextureID)(uintptr_t)backyard_data_map_[frm->layer_name]->getTextureData().textureID, thumbnail_size, ImVec2(0, 0), ImVec2(1, 1), 0);
		}
		else ImGui::ImageButton((ImTextureID)(uintptr_t)frm->fbo.getTexture().getTextureData().textureID, thumbnail_size, ImVec2(0, 0), ImVec2(1, 1), 0);

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

				GenEventArgs args;
				args.target_slot_index = index;
				args.target_layer_name = target_layer_name;
				args.thumbnail = backyard_data_map_[target_layer_name];

				ofNotifyEvent(gen_event_args_, args, this);
			}
		}

		if (layer != nullptr) ImGui::VSliderFloat("##v", slider_size, &layer->getAlpha(), 0.f, 1.f, "");
		else {
			float zero = 0.f;
			ImGui::VSliderFloat("##v", slider_size, &zero, 0.f, 1.f, "");
		}


		// Layer remove UI --------------------------------------------------
		//
		colors[ImGuiCol_Button] = ImVec4(0.00f, 0.27f, 0.62f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.00f, 0.46f, 0.62f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.83f, 1.00f, 1.00f);
		ImGui::SameLine();
		if (ImGui::Button("", ImVec2(slider_size.x, slider_size.x))) {
			this->remove(frm->layer_name);
			frm->clear();
		}


		style.WindowPadding = ImVec2(20.f, 4.f);
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::PushItemWidth(window_size.x * 0.7);
		float v = 0.f;
		ImGui::Text(std::string(frm->layer_name + " : Param").data());
		if (layer != nullptr) {
			layer->drawGui();
		}

		if (layer != nullptr) {
			ImGui::SameLine();
			if (ImGui::Button("Capture")) {
				ofPixels pixels;
				frm->fbo.readToPixels(pixels);
				pixels.resize(width_ * 0.1, height_ * 0.1);
				ofSaveImage(pixels, "ofxGen/" + frm->layer_name + "/thumbnail.jpg");
			}
		}
		ImGui::End();

		index++;
		colors[ImGuiCol_Button] = button_col;
		colors[ImGuiCol_ButtonHovered] = button_col_hovered;
		colors[ImGuiCol_ButtonActive] = button_col_active;
	}

	style.WindowPadding = push_window_padding;
}


void Manager::draw2dGui() {
	ImGui::Begin(std::string(unique_name_ + "-2d").data());
	ImGuiImageBottunFited(fbo_2d_.getTexture(0));
	ImGui::End();
}


void Manager::draw3dGui() {
	ImGui::Begin(std::string(unique_name_ + "-3d").data());
	ImGuiImageBottunFited(fbo_3d_.getTexture(0));
	ImGuiImageBottunFited(fbo_3d_.getTexture(1));
	ImGuiImageBottunFited(fbo_3d_.getTexture(2));
	ImGui::End();
}


void Manager::ImGuiImageBottunFited(const ofTexture& src) {
	ImVec2 window_size = ImGui::GetWindowSize();
	float offset = 4.0f;
	ImVec2 thumbnail_size = ImVec2(window_size.x - offset, (window_size.x - offset) * 9 / 16);
	ImGui::ImageButton((ImTextureID)(uintptr_t)src.getTextureData().textureID, thumbnail_size, ImVec2(0, 0), ImVec2(1, 1), 0);
}


void Manager::drawUtilGui() {
	ImGui::Begin("Camera");
	ImGui::SliderFloat("Distance", &cam_distance_, 10.f, 1000.f);
	ImGui::SliderFloat("Speed", &cam_speed_, 0.1f, 10.0f);
	ImGui::End();
}


void Manager::drawBackyardGui() {

	auto& style = ImGui::GetStyle();
	ImVec4* colors = ImGui::GetStyle().Colors;

	auto button_col = colors[ImGuiCol_Button];
	auto button_col_hovered = colors[ImGuiCol_ButtonHovered];
	auto button_col_active = colors[ImGuiCol_ButtonActive];

	colors[ImGuiCol_Button] = ImVec4(0.00f, 0.27f, 0.62f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.00f, 0.46f, 0.62f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.83f, 1.00f, 1.00f);

	ImGui::Begin("BACKYARD");
	style.FramePadding = ImVec2(2.f, 2.f);

	int view_index = 1;
	for (auto& data : backyard_data_map_) {
		std::string window_title = "GEN-PREVIEW" + std::to_string(view_index);
		if (data.second->isAllocated()) ImGui::ImageButton((ImTextureID)(uintptr_t)data.second->getTextureData().textureID, ImVec2(80, 45));
		if (ImGui::BeginDragDropSource()) {
			ImGui::SetDragDropPayload("_Gen", &data.first, sizeof(data.first), ImGuiCond_Once);
			ImGui::ImageButton((ImTextureID)(uintptr_t)data.second->getTextureData().textureID, ImVec2(128, 72), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), 0);
			ImGui::EndDragDropSource();
		}
		if (view_index % 9 != 0) ImGui::SameLine();
		view_index++;
	}
	ImGui::End();

	colors[ImGuiCol_Button] = button_col;
	colors[ImGuiCol_ButtonHovered] = button_col_hovered;
	colors[ImGuiCol_ButtonActive] = button_col_active;
}
}
}