#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"

#include "TestTemplate.hpp"
#include "../Gimmel/include/modulation/Tremolo.hpp"

class TestTremolo : public TestTemplate {
private:
	int sampleRate;
	giml::Tremolo<float> tremolo;

	al::ParameterBool tremoloBypass{ "tremoloBypass", "", true, 0.f, 1.f }; //False means the effect is ON
	al::Parameter rate{ "rate (ms)", "", 1.f, 0.1f, 1000.f };
	al::Parameter depth{ "depth", "", 1.f, 0.f, 1.f };

public:
	TestTremolo(int sampleRate = 44100, int bufferSize = 256,
	std::string deviceIn = "Microphone", std::string deviceOut = "Speaker") :
	TestTemplate(sampleRate, bufferSize, deviceIn, deviceOut),
	tremolo(sampleRate) {}

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
		const std::function<void(bool)> tremoloBypassCallback = [&](bool a) { if (!a) { this->tremolo.enable(); }
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
		if (k.key() == 'b') { // <- on b, toggle effect bypass
			tremoloBypass = !tremoloBypass;
			std::cout << "Bypass Status: " << tremoloBypass << std::endl;
		}
		return returnVal;
	}

	float sampleLoop(float in) override {
		//TODO: DSP logic goes here
		float out = tremolo.processSample(in);
		return out;
	}
};

int main() {
	TestTremolo app(44100, 512, "MacBook Pro Microphone", "MacBook Pro Speakers"); // instance of our app 
	app.start();
	return 0;
}