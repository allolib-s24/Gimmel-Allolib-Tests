#include "../utility/TestTemplate.hpp"
#include "../Gimmel/include/modulation/Chorus.hpp"

class ChorusDemo : public TestTemplate {
private:
	giml::Chorus<float> chorus;

	al::ParameterBool bypass{ "bypass", "", true, 0.f, 1.f }; //False means the effect is ON
	al::Parameter rate{"rate", "", 1.f, 0.f, 20.f};
	al::Parameter depth{"depth", "", 30.f, 0.f, 1000.f};

public:
	ChorusDemo(int sampleRate = 44100, int bufferSize = 256,
	std::string deviceIn = "Microphone", std::string deviceOut = "Speaker",
	std::string inputFilepath = "") :
	TestTemplate(sampleRate, bufferSize, deviceIn, deviceOut, inputFilepath), 
	chorus(sampleRate) {}

	void onInit() override {
		TestTemplate::onInit(); //Call the base class's init() first so that `gui` is initialized
		
		//TODO: Add other parameters you'd need here
		this->panel->gui.add(bypass);
		this->panel->gui.add(rate);
		this->panel->gui.add(depth);
	}

	void onCreate() override {
		TestTemplate::onCreate(); //Call the base class' create() first in case we add anything there later
		// bypass callback 
		const std::function<void(bool)> bypassCallback = [&](bool a) { if (!a) { this->chorus.enable(); }
		else { this->chorus.disable(); } };
		bypass.registerChangeCallback(bypassCallback);

		// // effect parameter callbacks
		// const std::function<void(float)> rateCallback = [&](float a) { this->chorus.setDepth(a); 
		// 	std::cout << "a:" << a << std::endl;
		// 	std::cout << "depth:" << depth << std::endl;
		// };
		// depth.registerChangeCallback(rateCallback);

		// const std::function<void(float)> depthCallback = [&](float a) { this->chorus.setRate(a); };
		// rate.registerChangeCallback(depthCallback);
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
		chorus.setDepth(this->depth);
		chorus.setRate(this->rate);
		float out = chorus.processSample(in);
		return out;
	}
};

int main() {
	ChorusDemo app(44100, 512, "MacBook Pro Microphone", "MacBook Pro Speakers", "../../Resources/SoftGuitar.wav"); // instance of our app 
	app.start();
	return 0;
}