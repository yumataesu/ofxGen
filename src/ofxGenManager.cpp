#include "ofxGenManager.h"

namespace ofx {
namespace Gen {
	std::vector<BackyardData> Manager::backyard_data_;

	Manager::Manager(const std::size_t layer_num, const ofFbo::Settings& settings)
		: layer_num_(layer_num)
		, time_(0.f) 
		, alpha_(1.f)
	{
		depth_composite_shader_.load("composite/depth_composite");
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
			frm->index = -1;
			frm->clear();

			frames_.emplace_back(std::move(frm));
		}

		fbo_2d_.allocate(settings);
		fbo_3d_.allocate(settings);

		result_fbo_ = std::make_shared<ofFbo>();
		result_fbo_->allocate(settings);

		ofAddListener(gen_event_, this, &Manager::layerAdded);
	}




	Manager::~Manager() {
		ofRemoveListener(gen_event_, this, &Manager::layerAdded);
	}




	void Manager::update(const float& delta_time) {
		for (const auto& layer : this->process_map) {
			if (layer.second->getAlpha() > 0.f) {
				layer.second->update(delta_time);
			}
		}

		time_ += delta_time * 0.02f;
		cam_.orbit(360 * sin(time_ + ofNoise(time_)), 360 * cos(time_ + 2 + ofNoise(time_ + 10.f)), 750 + 500 * sin(time_));

		this->renderToFbo();
		this->composite();
		this->postprocess();
	}




	void Manager::draw() const {
		result_fbo_->draw(0., 0.);
	}


	void Manager::bang() {
		for (const auto& layer : this->process_map) {
			if (layer.second->getAlpha() > 0.f) {
				layer.second->bang();
			}
		}
	}


	void Manager::layerAdded(GenEvent& event) {
		auto it = this->process_map.find(event.target_layer_name);
		if (it != this->process_map.end())
			return;

		auto layer = add(event.target_layer_name);
		layer->setTarget(event.taeget_fbo_index);
	}




	void Manager::drawFrameGui(const std::string& parent_name) {
		auto& style = ImGui::GetStyle();
		style.FramePadding = ImVec2(2.f, 2.f);

		int index = 0;
		for (const auto& frm : frames_) {
			std::string window_title = "0" + std::to_string(index) + "|" + parent_name;
			ImGui::Begin(window_title.data());
			auto layer = this->getByName(frm->layer_name);

			if (ImGui::ImageButton((ImTextureID)(uintptr_t) frm->fbo.getTexture().getTextureData().textureID, ImVec2(128, 128 / 16 * 9), ImVec2(0, 0), ImVec2(1, 1), 0)) {
				if (layer != nullptr) layer->bang();
			}

			ImGui::SameLine();
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_Gen")) {
					IM_ASSERT(payload->DataSize == sizeof(std::string));

					this->remove(frm->layer_name); //remove previous layer resource
					frm->layer_name = "\0";
					frm->index = -1;
					frm->is_3d_scene = false;

					std::string target_layer_name = "\0";
					memcpy((void*)&target_layer_name, payload->Data, sizeof(target_layer_name));

					GenEvent new_event;
					new_event.taeget_fbo_index = index;
					new_event.target_layer_name = target_layer_name;

					ofNotifyEvent(gen_event_, new_event, this);
				}
			}

			if (layer != nullptr) {
				ImGui::VSliderFloat("##v", ImVec2(24, 128 / 16 * 9), &layer->getAlpha(), 0.f, 1.f, "");
			}
			else {
				float zero = 0.f;
				ImGui::VSliderFloat("##v", ImVec2(24, 128 / 16 * 9), &zero, 0.f, 1.f, "");
			}

			// Layer remove UI --------------------------------------------------
			//
			//
			ImGui::BeginChild(window_title.data(), ImVec2(160, 62), true);
			ImGui::PushItemWidth(50.5f);
			if (ImGui::Button("", ImVec2(20, 20))) {
				this->remove(frm->layer_name);
				frm->clear();
			}
			ImGui::EndChild();
			ImGui::End();

			index++;
				//pos.x += size.x;
				//if(index % 4 == 0) {
				//    pos.x = 2;
				//    pos.y += size.y + 2;
				//}
			}
		}




		void Manager::renderToFbo() {
			for (std::size_t i = 0; i < layer_num_; ++i) {
				for (const auto& layer : this->process_map) {

					if (layer.second->getTarget() == i) {
						frames_[i]->layer_name = layer.first;
						frames_[i]->is_3d_scene = layer.second->is3D();
						if (frames_[i]->is_3d_scene) glEnable(GL_DEPTH_TEST);
						else glDisable(GL_DEPTH_TEST);

						frames_[i]->fbo.begin();
						ofClear(0.f);
						if (frames_[i]->is_3d_scene) cam_.begin();
						layer.second->draw();
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


			//3D-------------------------------------------------------
			// Render 3d layer to fbo

			fbo_3d_.begin();
			ofClear(0);
			depth_composite_shader_.begin();
			int col_index = 0; int depth_index = 1; int i = 0;

			for (const auto& frm : frames_) {
				if (frm->is_3d_scene) {
					std::string col_name = "u_col" + std::to_string(i);
					std::string depth_name = "u_depth" + std::to_string(i);
					auto layer = this->getByName(frm->layer_name);
					float alpha = 0.f;
					if (layer != nullptr) {
						alpha = layer->getAlpha();
						depth_composite_shader_.setUniform1f("u_alpha" + std::to_string(i), alpha);
					}
					else {
						depth_composite_shader_.setUniform1f("u_alpha" + std::to_string(i), 0.f);
					}
					depth_composite_shader_.setUniformTexture(col_name, frm->fbo.getTexture(), col_index);
					depth_composite_shader_.setUniformTexture(depth_name, frm->fbo.getDepthTexture(), depth_index);
					col_index += 2;
					depth_index += 2;
					i++;
				}
			}
			depth_composite_shader_.setUniform1i("u_layer_num", i);
			quad_.draw();
			depth_composite_shader_.end();
			fbo_3d_.end();
		}




		void Manager::composite() {

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_MAX);

			result_fbo_->begin();
			ofClear(0.f);
			ofSetColor(255, 255 * alpha_);
			fbo_3d_.draw(0, 0);
			fbo_2d_.draw(0, 0);
			result_fbo_->end();

			glDisable(GL_BLEND);
		}




		void Manager::setupBackyard() {
			ofDisableArbTex();
			for (auto& name : registered_names()) {
				BackyardData data;
				std::unique_ptr<ofTexture> t = std::make_unique<ofTexture>();
				ofLoadImage(*t, "generative/" + name + ".png");
				data.thumbnail = std::move(t);
				data.layer_name = name;
				backyard_data_.emplace_back(data);
			}
			ofEnableArbTex();
		}




		void Manager::drawBackyardGui() {
			ImGui::Begin("BACKYARD");
			int view_index = 1;
			for (auto& data : backyard_data_) {
				std::string window_title = "GEN-PREVIEW" + std::to_string(view_index);
				ImGui::BeginChild(window_title.data(), ImVec2(77, 77 * 9 / 16));
				if(data.thumbnail->isAllocated()) ImGui::ImageButton((ImTextureID)(uintptr_t)data.thumbnail->getTextureData().textureID, ImVec2(63, 63 * 9 / 16));
				if (ImGui::BeginDragDropSource()) {

					ImGui::SetDragDropPayload("_Gen", &data.layer_name, sizeof(data.layer_name), ImGuiCond_Once);

					//ImGui::BeginDragDropTooltip();
					ImGui::ImageButton((ImTextureID)(uintptr_t)data.thumbnail->getTextureData().textureID, ImVec2(128, 72), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), 0);
					//ImGui::EndDragDropTooltip();
					ImGui::EndDragDropSource();
				}
				ImGui::EndChild();
				if (view_index % 6 != 0) ImGui::SameLine();
				view_index++;
			}
			ImGui::End();
		}
	}
}