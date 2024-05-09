#ifndef TESTTEMPLATE_HPP
#define TESTTEMPLATE_HPP
#include <iostream>
#include "Oscilloscope.hpp"

class TestTemplate : public al::App {
private:
	// Basic parameters all test apps should inherit
	al::Parameter volControl{ "volControl", "", 0.f, -96.f, 6.f };
	float volFactor = 1;
	al::Parameter rmsMeter{ "rmsMeter", "", -96.f, -96.f, 0.f };
	al::ParameterBool audioOutput{ "audioOutput", "", false, 0.f, 1.f };
protected:
	std::vector<Oscilloscope*> oscilloscopes;
	std::shared_ptr<al::GUIPanelDomain> panel;
	virtual float sampleLoop(float in) { return 0.f; } //Default mute so that we know if anything is wrong
public:
	TestTemplate() {
		
	}
	virtual void onInit() {
		//Set up the GUI
		auto GUIdomain = al::GUIDomain::enableGUI(defaultWindowDomain());
		this->panel = GUIdomain->newPanel();
		// add io params
		this->panel->gui.add(volControl);
		this->panel->gui.add(rmsMeter);
	}

	virtual void onCreate() {
		//Callback function for changing the volume control
		const std::function<void(float)> volControlCallback = [&](float a) { this->volFactor = giml::dBtoA(a); };
		volControl.registerChangeCallback(volControlCallback);
	}

	virtual void onAnimate(double dt) {
		// update all the oscilloscopes
		for (Oscilloscope* oscilloscope : oscilloscopes) {
			oscilloscope->update();
		}
	}

	virtual bool onKeyDown(const al::Keyboard& k) override {
		if (k.key() == 'm') { // <- on m, muteToggle
			audioOutput = !audioOutput;
			std::cout << "Mute Status: " << audioOutput << std::endl;
		}
		return true;
	}

	void onSound(al::AudioIOData& io) override {
		// variables reset for each call
		float bufferPower = 0; // for measuring output RMS
		auto maxFrames = io.framesPerBuffer();
		for (int i = 0; i < maxFrames; i++) {
			// capture input sample
			float input = io.in(0, i); //TODO

			// transform input for output (they will override this function to do stuff)
			float output = this->sampleLoop(input);
			// apply volume, muteToggle 
			output *= volFactor * audioOutput;
			// for each channel, write output to speaker
			for (int channel = 0; channel < io.channelsOut(); channel++) {
				io.out(channel, i) = output;
			}

			// feed to analysis buffer
			bufferPower += output;

			// overload detector (assuming 2 output channels)
			if (io.out(0, i) > 1.f || io.out(1, i) > 1.f) {
				std::cout << "CLIP!" << std::endl;
			}
		}

		bufferPower /= maxFrames; // calculate bufferPower
		rmsMeter = giml::aTodB(bufferPower); // print to GUI display
	}

	void onDraw(al::Graphics& g) {
		g.clear(0);
		g.color(1);
		g.camera(al::Viewpoint::IDENTITY); // Ortho [-1:1] x [-1:1]
		for (Oscilloscope* oscilloscope : oscilloscopes) {
			g.color(oscilloscope->r, oscilloscope->g, oscilloscope->b);
			g.draw(*oscilloscope);
		}
	}
};

#endif