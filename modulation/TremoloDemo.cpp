#include "../utility/TestTemplate.hpp"
#include "../Gimmel/include/modulation/Tremolo.hpp"

class TremoloDemo : public TestTemplate {
private:
	giml::Tremolo<float> tremolo;

	al::ParameterBool bypass{ "bypass", "", true, 0.f, 1.f }; //False means the effect is ON
	al::Parameter depth{"depth", "", 1.f, 0.f, 1.f};
	al::Parameter speed{"speed", "", 300.f, 0.1f, 1000.f};

public:
	TremoloDemo(int sampleRate = 44100, int bufferSize = 256,
	std::string deviceIn = "Microphone", std::string deviceOut = "Speaker",
	std::string inputFilepath = "") :
	TestTemplate(sampleRate, bufferSize, deviceIn, deviceOut, inputFilepath),
	tremolo(sampleRate) {}

	void onInit() override {
		TestTemplate::onInit(); //Call the base class's init() first so that `gui` is initialized
		
		//TODO: Add other parameters you'd need here
		this->panel->gui.add(bypass);
		this->panel->gui.add(depth);
		this->panel->gui.add(speed);
	}

	void onCreate() override {
		TestTemplate::onCreate(); //Call the base class' create() first in case we add anything there later
		// bypass callback 
		const std::function<void(bool)> bypassCallback = [&](bool a) { if (!a) { this->tremolo.enable(); }
		else { this->tremolo.disable(); } };
		bypass.registerChangeCallback(bypassCallback);

		// effect parameter callbacks
		const std::function<void(float)> rateCallback = [&](float a) { this->tremolo.setDepth(depth); };
		depth.registerChangeCallback(rateCallback);

		const std::function<void(float)> depthCallback = [&](float a) { this->tremolo.setSpeed(speed); };
		speed.registerChangeCallback(depthCallback);
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
		// DSP logic goes here
		float out = tremolo.processSample(in);
		return out;
	}
};

int main() {
	TremoloDemo app(44100, 512, "MacBook Pro Microphone", "MacBook Pro Speakers", "../../Resources/3xGmaj.wav"); // instance of our app 
	app.start();
	return 0;
}