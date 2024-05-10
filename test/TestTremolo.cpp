// Joel A. Jaffe 2024-05-08
// AlloApp Demonstrating (Effect)

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"

#include "TestTemplate.hpp"
#include "../Gimmel/include/modulation/Tremolo.hpp"


//#define BENCHMARK_CODE(code) { \
//        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now(); \
//        code \
//        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now(); \
//        timeElapsed += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count(); \
//        iterations++; \
//        if (iterations > 48000 * 3) { \
//			std::cout << timeElapsed / iterations << "ns avg every 3 sec" << std::endl; \
//			timeElapsed = iterations = 0; \
//		} \
//   }



class MyTremoloTest : public TestTemplate {
private:
	int sampleRate;
	giml::Tremolo<float> tremolo;

	al::ParameterBool tremoloBypass{ "tremoloBypass", "", false, 0.f, 1.f }; //False means the effect is ON
	al::Parameter rate{ "rate", "", 1.f, 0.1f, 1000.f };
	al::Parameter depth{ "depth", "", 1.f, 0.f, 1.f };

public:
	MyTremoloTest() :
	sampleRate(static_cast<int>(al::AudioIO().framesPerSecond())), //Sets the sample rate so that we can set everything else to use it
	tremolo(sampleRate) {

	}
	void onInit() override {
		TestTemplate::onInit(); //Call the base class's init() first so that `gui` is initialized
		
		//TODO: Add other parameters you'd need here
		this->panel->gui.add(tremoloBypass);
		this->panel->gui.add(rate);
		this->panel->gui.add(depth);
	}

	void onCreate() override {
		TestTemplate::onCreate(); //Call the base class' create() first in case we add anything there later
		//TODO: Add the proper parameter callbacks here
		const std::function<void(bool)> tremoloBypassCallback = [&](bool a) { if (a) { this->tremolo.enable(); }
		else { this->tremolo.disable(); } };
		tremoloBypass.registerChangeCallback(tremoloBypassCallback);
		const std::function<void(float)> rateCallback = [&](float a) { this->tremolo.setSpeed(rate); };
		rate.registerChangeCallback(rateCallback);
		const std::function<void(float)> depthCallback = [&](float a) { this->tremolo.setDepth(depth); };
		depth.registerChangeCallback(depthCallback);
	}

	bool onKeyDown(const al::Keyboard &k) override {
		bool returnVal = TestTemplate::onKeyDown(k);
		//TODO: Add any other keyboard logic you want here
		return returnVal;
	}

	float sampleLoop(float in) override {
		//TODO: DSP logic goes here
		inputScope.writeSample(in);
		float out = tremolo.processSample(in);
		outputScope.writeSample(out);
		return out;
	}
};

int main() {
	MyTremoloTest app; // instance of our app 
	
	// Allows for manual declaration of input and output devices, 
	// but causes unpredictable behavior. Needs investigation.
	// app.audioIO().deviceIn(al::AudioDevice("MacBook Pro Microphone")); // change for your device
	// app.audioIO().deviceOut(al::AudioDevice("BH+Speakers")); // change for your device
	app.audioIO().deviceIn(al::AudioDevice("Microphone")); // change for your device
	app.audioIO().deviceOut(al::AudioDevice("Speaker")); // change for your device
	std::cout << "outs: " << app.audioIO().channelsOutDevice() << std::endl;
	std::cout << "ins: " << app.audioIO().channelsInDevice() << std::endl;
	app.configureAudio(48000, 128, app.audioIO().channelsOutDevice(), app.audioIO().channelsInDevice());
	// ^ samplerate, buffersize, channels out, channels in
	app.start();
	return 0;
}