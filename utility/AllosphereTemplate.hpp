#ifndef SPHERE_TEMPLATE_HPP
#define SPHERE_TEMPLATE_HPP

#include <iostream>
#include <string>

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"
#include "al/app/al_DistributedApp.hpp"
#include "al_ext/statedistribution/al_CuttleboneDomain.hpp"
#include "al_ext/statedistribution/al_CuttleboneStateSimulationDomain.hpp"

#include "SphereScope.hpp"
#include "../Gimmel/Test/wav.h"

struct State {
	// state() member variables
	static const int numVertices = 44100 * 2;
	al::Vec3f inScopeVertices[numVertices];
	al::Vec3f outScopeVertices[numVertices];

	// state() methods
	inline void writeInputVertices(int index, al::Vec3f input) {
		this->inScopeVertices[index] = input;
	}
	inline void writeOutputVertices(int index, al::Vec3f input) {
		this->outScopeVertices[index] = input;
	}

	inline al::Vec3f readInputVertices(int index) {
		return this->inScopeVertices[index];
	}
	inline al::Vec3f readOutputVertices(int index) {
		return this->outScopeVertices[index];
	}

};

class SphereTemplate : public al::DistributedAppWithState<State> {
protected:
	int sampleRate;
	std::shared_ptr<al::GUIPanelDomain> panel;
	inline virtual float sampleLoop(float in) { return in; } // Default passthru 

private:
	// Basic parameters all test apps should inherit
	al::Parameter volControl{ "volControl", "", 0.f, -96.f, 6.f };
	float volFactor = 1.f;
	al::Parameter rmsMeter{ "rmsMeter", "", -96.f, -96.f, 0.f };
	al::ParameterBool audioOutput{ "audioOutput", "", false, 0.f, 1.f };
	
	Oscilloscope inputScope, outputScope;

	WAVLoader* pLoader = nullptr;

protected:
	std::vector<Oscilloscope*> oscilloscopes;
public:
	SphereTemplate(int sampleRate, int bufferSize, std::string deviceIn, std::string deviceOut, std::string inputFilepath="") : sampleRate(sampleRate),
	inputScope(sampleRate), outputScope(sampleRate) {
	if (isPrimary()) { // <- if we are the simulator 
		// configure audio device 
		this->audioIO().deviceIn(al::AudioDevice(deviceIn)); // change for your device
		this->audioIO().deviceOut(al::AudioDevice(deviceOut)); // change for your device
		this->configureAudio(sampleRate, bufferSize, this->audioIO().channelsOutDevice(), this->audioIO().channelsInDevice());

		// and file playback
		if (inputFilepath.size() > 0) { // Not empty
			pLoader = new WAVLoader{ inputFilepath.c_str() }; //Pick an input sound to test
			if (!pLoader) {
				//Failed to allocated heap memory for WAV class
			}
		}

		// and oscilloscopes 
		outputScope.setColorRGB255(4, 217, 255); //Light blue
		this->oscilloscopes.push_back(&inputScope);// need to store pointer to oscilloscopes so that we don't make copies
		this->oscilloscopes.push_back(&outputScope);
	}
	else { // if we are not the simulator
		// only configure oscilloscopes 
		outputScope.setColorRGB255(4, 217, 255); //Light blue
		this->oscilloscopes.push_back(&inputScope); // need to store pointer to oscilloscopes so that we don't make copies
		this->oscilloscopes.push_back(&outputScope);
	}
	}
	
	virtual void onInit() override {
	auto cuttleboneDomain = al::CuttleboneStateSimulationDomain<State>::enableCuttlebone(this);
    if (!cuttleboneDomain) {
      std::cerr << "ERROR: Could not start Cuttlebone. Quitting." << std::endl;
      quit();
    }
	if (isPrimary()) {
		// Set up the GUI
		auto GUIdomain = al::GUIDomain::enableGUI(defaultWindowDomain());
		this->panel = GUIdomain->newPanel();
		// add io params
		this->panel->gui.add(volControl);
		this->panel->gui.add(rmsMeter);
		this->panel->gui.add(audioOutput);
	}
	}

	// virtual void onCreate() {
	// if (isPrimary()) {}
	// else {}
	// }

	virtual void onAnimate(double dt) override {
	if (isPrimary()) {
		// update all the oscilloscopes
		for (Oscilloscope* oscilloscope : oscilloscopes) {
			oscilloscope->update();
		}

		// // write input scope to state
		// for (int i = 0; i < oscilloscopes[0]->vertices().size(); i++) {
		// 	state().writeInputVertices(i, oscilloscopes[0]->vertices()[i]);
		// }
		// write output scope to state
		for (int i = 0; i < oscilloscopes[1]->vertices().size(); i++) {
			state().writeOutputVertices(i, oscilloscopes[1]->vertices()[i]);
		}
	} 
	else {
		// read input scope from state
		// for (int i = 0; i < oscilloscopes[0]->vertices().size(); i++) {
		// 	oscilloscopes[0]->vertices()[i] = state().readInputVertices(i);
		// }
		// read output scope from state
		for (int i = 0; i < oscilloscopes[1]->vertices().size(); i++) {
			oscilloscopes[1]->vertices()[i] = state().readOutputVertices(i);
		}
	}
	}

	virtual bool onKeyDown(const al::Keyboard& k) override {
	if (isPrimary()) {
		if (k.key() == 'm') { // <- on m, muteToggle
			audioOutput = !audioOutput;
			std::cout << "Mute Status: " << audioOutput << std::endl;
		}
		return true;
	}
	}

	void onSound(al::AudioIOData& io) override {
	if (isPrimary()) {
		// variables reset for each call
		float bufferPower = 0; // for measuring output RMS
		auto bufferSize = io.framesPerBuffer();
		for (int sample = 0; sample < bufferSize; sample++) {
			// capture input sample
			float input;
			if (pLoader) {
				pLoader->readSample(&input);
			}
			else {
				input = io.in(0, sample); //Support only mono channels for now
			}
			
			this->inputScope.writeSample(input);
			// transform input for output (they will override this function to do stuff)
			float output = this->sampleLoop(input);

			// apply volume, muteToggle 
			output *= volFactor * audioOutput;
			
			// feed to analysis buffer and oscilloscope
			bufferPower += output;
			this->outputScope.writeSample(output);

			// write output to all channels
			for (int channel = 0; channel < io.channelsOut(); channel++) {
				io.out(channel, sample) = output;
			}
		}

		bufferPower /= bufferSize; // calculate bufferPower
		rmsMeter = giml::aTodB(bufferPower); // print to GUI display
	}
	}

	virtual void onDraw(al::Graphics& g) override {
		g.lens().eyeSep(0); // disable stereo rendering 
		g.clear(0); // maybe make brighter
		g.meshColor(); // color scopes

		for (Oscilloscope* oscilloscope : oscilloscopes) { // draw scopes
			g.draw(*oscilloscope);
		}

	}
};
#endif