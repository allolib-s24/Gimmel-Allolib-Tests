#ifndef TESTTEMPLATE_HPP
#define TESTTEMPLATE_HPP

#include <iostream>
#include <string>

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"

#include "Oscilloscope.hpp"

class TestTemplate : public al::App {
protected:
	int sampleRate;
	std::shared_ptr<al::GUIPanelDomain> panel;
	inline virtual float sampleLoop(float in) { return in; } // Default mute so that we know if anything is wrong

private:
	// Basic parameters all test apps should inherit
	al::Parameter volControl{ "volControl", "", 0.f, -96.f, 6.f };
	float volFactor = 1;
	al::Parameter rmsMeter{ "rmsMeter", "", -96.f, -96.f, 0.f };
	al::ParameterBool audioOutput{ "audioOutput", "", false, 0.f, 1.f };
	
	Oscilloscope inputScope, outputScope;
	std::vector<Oscilloscope*> oscilloscopes;


public:
	TestTemplate(int sampleRate, int bufferSize, std::string deviceIn, std::string deviceOut) : sampleRate(sampleRate),
	inputScope(sampleRate), outputScope(sampleRate) {
		// configure audio device 
		this->audioIO().deviceIn(al::AudioDevice(deviceIn)); // change for your device
		this->audioIO().deviceOut(al::AudioDevice(deviceOut)); // change for your device
		this->configureAudio(sampleRate, bufferSize, this->audioIO().channelsOutDevice(), this->audioIO().channelsInDevice());

		// configure oscilloscopes 
		outputScope.setColorRGB255(4, 217, 255); //Light blue
		this->oscilloscopes.push_back(&inputScope);// need to store pointer to oscilloscopes so that we don't make copies
		this->oscilloscopes.push_back(&outputScope);
	}
	
	virtual void onInit() {
		// Set up the GUI
		auto GUIdomain = al::GUIDomain::enableGUI(defaultWindowDomain());
		this->panel = GUIdomain->newPanel();
		// add io params
		this->panel->gui.add(volControl);
		this->panel->gui.add(rmsMeter);
		this->panel->gui.add(audioOutput);
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
		auto bufferSize = io.framesPerBuffer();
		for (int sample = 0; sample < bufferSize; sample++) {
			// capture input sample
			float input = io.in(0, sample); //Support only mono channels for now
			this->inputScope.writeSample(input);
			// transform input for output (they will override this function to do stuff)
			float output = this->sampleLoop(input);

			// apply volume, muteToggle 
			output *= volFactor * audioOutput;
			
			// feed to analysis buffer and oscilloscope
			bufferPower += output;
			this->outputScope.writeSample(output);

			// overload detector (assuming 2 output channels)
			if (output > 1.f) {
				std::cout << "CLIP!" << std::endl;
			}

			// write output to all channels
			for (int channel = 0; channel < io.channelsOut(); channel++) {
				io.out(channel, sample) = output;
			}
		}

		bufferPower /= bufferSize; // calculate bufferPower
		rmsMeter = giml::aTodB(bufferPower); // print to GUI display
	}

	void onDraw(al::Graphics& g) {
		g.clear(0);
		g.meshColor();
		g.camera(al::Viewpoint::IDENTITY); // Ortho [-1:1] x [-1:1]
		for (Oscilloscope* oscilloscope : oscilloscopes) {
			g.draw(*oscilloscope);
		}
	}
};
#endif