#include "ofApp.h"
#include <cmath>
#include <vector>
#include <string>

// Uncomment to switch between 2 modes
//#define GLOBAL_ARB_TEX

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetLogLevel(OF_LOG_NOTICE);
	ofSetBackgroundAuto(false);
	ofSetVerticalSync(true);
	ofSetFrameRate(60);

	this->stepper = 0.0f;

    // FBO
#ifdef GLOBAL_ARB_TEX
    // Option 1 : Set all textures to GL_TEXTURE_2D in setup (recommended)
    ofDisableArbTex();
    fbo.allocate(200,200,GL_RGBA);
#else
    // Option 2 : Specify the texture target explicitly (when you can't set yourall ofApp textures to GL_TEXTURE_2D
    ofFbo::Settings fboSettings ;
    fboSettings.width = 200;
    fboSettings.height = 200;
    fboSettings.internalformat = GL_RGBA ;
    fboSettings.textureTarget = GL_TEXTURE_2D;
    fbo.allocate(fboSettings);
#endif
    fbo.checkStatus();
    fbo.begin();
    ofClear(0,0);
    ofSetColor(10,200,10);
    ofFill();
    ofDrawCircle(100,100, 90);
    fbo.end();

    // Gui
    gui.setup(nullptr, true, ImGuiConfigFlags_None, true);
	this->guiVisible = true;
}

//--------------------------------------------------------------
void ofApp::update()
{
	this->stepper += this->speed;
	cubeSize = ofMap(sinf(this->stepper), -1.0f, 1.0f, this->sizeMin, this->sizeMax);
}

//--------------------------------------------------------------
void ofApp::draw()
{
	// We have to use ofParameter::get() since this we are using an ofFloatColor.
	ofClear(this->background.get());

	this->camera.begin();
	{
		ofEnableDepthTest();

		ofSetColor(ofColor::white);

		if (this->enabled)
		{
			ofPushStyle();
			{
				// We have to use ofParameter::get() since this we are using an ofFloatColor.
				ofSetColor(this->foreground.get());

				auto fillRender = static_cast<RenderMode>(this->fillMode.get());
				if (fillRender != RenderMode::None)
				{
					ofFill();
					if (fillRender == RenderMode::Texture && this->texture.isAllocated())
					{
						this->texture.bind();
					}
					ofDrawBox(this->cubeSize);
					if (fillRender == RenderMode::Texture && this->texture.isAllocated())
					{
						this->texture.unbind();
					}
				}

				auto strokeRender = static_cast<RenderMode>(this->strokeMode.get());
				if (strokeRender != RenderMode::None)
				{
					ofNoFill();
					if (strokeRender == RenderMode::Texture && this->texture.isAllocated())
					{
						this->texture.bind();
					}
					ofDrawBox(this->cubeSize);
					if (strokeRender == RenderMode::Texture && this->texture.isAllocated())
					{
						this->texture.unbind();
					}
				}
			}
			ofPopStyle();
		}

		ofDisableDepthTest();
	}
	this->camera.end();

	// Gui
	this->mouseOverGui = false;
	if (this->guiVisible)
	{
		this->mouseOverGui = this->imGui();
	}
	if (this->mouseOverGui)
	{
		this->camera.disableMouseInput();
	}
	else
	{
		this->camera.enableMouseInput();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

//--------------------------------------------------------------
bool ofApp::loadImage(const string & filePath)
{
	ofImage image;
	image.setUseTexture(false);
	if (!image.load(filePath))
	{
		ofLogError(__FUNCTION__) << "No image found at " << filePath;
		return false;
	}

	ofTextureData texData;
	texData.width = image.getWidth();
	texData.height = image.getHeight();
	texData.textureTarget = GL_TEXTURE_2D;
	this->texture.allocate(texData);
	this->texture.loadData(image.getPixels());
    //texData.bFlipTexture = true;

	this->imagePath = ofFilePath::makeRelative(ofToDataPath(""), filePath);
	return true;
}

//--------------------------------------------------------------
bool ofApp::imGui()
{
	auto mainSettings = ofxImGui::Settings();

	this->gui.begin();
	{
        static bool bCollapse = false;
        if (ofxImGui::BeginWindow("Helpers", mainSettings, ImGuiWindowFlags_NoCollapse, &bCollapse))
		{
			ImGui::Text("%.1f FPS (%.3f ms/frame)", ofGetFrameRate(), 1000.0f / ImGui::GetIO().Framerate);

			if (ofxImGui::BeginTree(this->colors, mainSettings))
			{
				ofxImGui::AddParameter(this->background, false);
				ofxImGui::AddParameter(this->foreground);

				ofxImGui::EndTree(mainSettings);
			}

			if (ofxImGui::BeginTree(this->mesh, mainSettings))
			{
				ofxImGui::AddParameter(this->enabled);
				ofxImGui::AddRange("Size Range", this->sizeMin, this->sizeMax, 1.0f);
				ofxImGui::AddParameter(this->speed);
				ImGui::Text("Size: %.2f", this->cubeSize);

				ofxImGui::EndTree(mainSettings);
			}

			if (ofxImGui::BeginTree(this->render, mainSettings))
			{
				if (ImGui::Button("Load Image..."))
				{
					auto dialogResult = ofSystemLoadDialog("Load Image", false, ofToDataPath(""));
					if (dialogResult.bSuccess)
					{
						this->loadImage(dialogResult.filePath);
					}
				}

				static const std::vector<std::string> labels = { "None", "Color", "Texture" };

				ofxImGui::AddRadio(this->fillMode, labels, 3);
				ofxImGui::AddRadio(this->strokeMode, labels, 3);

				if (this->texture.isAllocated())
				{
					ofxImGui::AddParameter(this->preview);
				}

				ofxImGui::EndTree(mainSettings);
			}

			// Some work has yet to be done on the Helpers.
			// For example, the line below causes a crash
			// ofxImGui::AddGroup(this->render, mainSettings);
		}
        ofxImGui::EndWindow(mainSettings);
        //ImGui::End();

		// 2nd window
		auto altSettings = ofxImGui::Settings();
		if (ofxImGui::BeginWindow("2nd Helpers", altSettings, ImGuiWindowFlags_NoCollapse, &bCollapse))
		{
			ImGui::TextWrapped("%s", "The helper settings return information about the window state : the white box around this window shows how to use them.\n"\
									 "Also, when you reopen this application, window positions and sizes should restore to their last state before exiting." );
		}
		ofxImGui::EndWindow(altSettings);

		// Draw a rectangle to check if the returned values are correct
		#define MY_MARGING 10
		ofPushStyle();
		ofNoFill();
		ofDrawRectRounded(altSettings.windowPos-ofVec2f(MY_MARGING,MY_MARGING), altSettings.windowSize.x+2*MY_MARGING, altSettings.windowSize.y+2*MY_MARGING, MY_MARGING);
		ofPopStyle();

		if (this->preview)
		{
			static const float kPreviewSize = 256.0f;
			auto previewSettings = ofxImGui::Settings();
			previewSettings.windowPos = ofVec2f(ofGetWidth() - kPreviewSize - kImGuiMargin * 3, kImGuiMargin);
			previewSettings.windowSize = ofVec2f(kPreviewSize, kPreviewSize);

			if (ofxImGui::BeginWindow(this->preview, previewSettings, false))
			{
				ofxImGui::AddImage(this->texture, previewSettings.windowSize);
			}
			ofxImGui::EndWindow(previewSettings);
		}
		if(ImGui::Begin("ofFboTexture")){
            glm::vec2 size = {fbo.getWidth(), fbo.getHeight()};
            ofxImGui::AddImage(fbo, size);
		}
		ImGui::End();
	}
	this->gui.end();

	return mainSettings.mouseOverGui;
}
