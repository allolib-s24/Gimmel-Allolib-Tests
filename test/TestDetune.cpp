#include "TestTemplate.hpp"
#include "../Gimmel/include/modulation/Detune.hpp"

class TestDetune : public TestTemplate {
private:
	giml::Detune<float> detune;

	al::ParameterBool bypass{ "bypass", "", true, 0.f, 1.f }; //False means the effect is ON
	al::Parameter pRatio{ "pRatio", "", 1.f, 0.5f, 2.f };
	al::Parameter wSize{ "wSize", "", 22.f, 5.f, 50.f };

public:
	TestDetune(int sampleRate = 44100, int bufferSize = 256,
	std::string deviceIn = "Microphone", std::string deviceOut = "Speaker") :
	TestTemplate(sampleRate, bufferSize, deviceIn, deviceOut),
	detune(sampleRate) {}

	void onInit() override {
		TestTemplate::onInit(); //Call the base class's init() first so that `gui` is initialized
		
		//TODO: Add other parameters you'd need here
		this->panel->gui.add(bypass);
		this->panel->gui.add(pRatio);
		this->panel->gui.add(wSize);
	}

	void onCreate() override {
		TestTemplate::onCreate(); //Call the base class' create() first in case we add anything there later
		// bypass callback 
		const std::function<void(bool)> bypassCallback = [&](bool a) { if (!a) { this->detune.enable(); }
		else { this->detune.disable(); } };
		bypass.registerChangeCallback(bypassCallback);

		// effect parameter callbacks
		const std::function<void(float)> rateCallback = [&](float a) { this->detune.setPitchRatio(pRatio); };
		pRatio.registerChangeCallback(rateCallback);

		const std::function<void(float)> depthCallback = [&](float a) { this->detune.setWindowSize(wSize); };
		wSize.registerChangeCallback(depthCallback);
	}

	bool onKeyDown(const al::Keyboard &k) override {
		bool returnVal = TestTemplate::onKeyDown(k);
		//TODO: Add any other keyboard logic you want here
		if (k.key() == 'b') { // <- on b, toggle effect bypass
			bypass = !bypass;
			std::cout << "Bypass Status: " << bypass << std::endl;
		}
		return returnVal;
	}

	float sampleLoop(float in) override {
		//TODO: DSP logic goes here
		float out = detune.processSample(in);
		return out;
	}
};

int main() {
	TestDetune app(44100, 512, "MacBook Pro Microphone", "MacBook Pro Speakers"); // instance of our app 
	app.start();
	return 0;
}