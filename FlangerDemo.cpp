#include "Utility/TestTemplate.hpp"
#include "Gimmel/include/Flanger.hpp"

class FlangerDemo : public TestTemplate {
private:
	giml::Flanger<float> Flanger;

	al::ParameterBool bypass{ "bypass", "", true, 0.f, 1.f }; // False means the effect is ON
	al::Parameter rate{"rate", "", 0.2f, 0.f, 20.f};
	al::Parameter depth{"depth", "", 5.f, 0.f, 5.f};
	al::Parameter blend{"blend", "", 0.5f, 0.f, 1.f};

public:
	FlangerDemo(int sampleRate = 44100, int bufferSize = 256,
	std::string deviceIn = "Microphone", std::string deviceOut = "Speaker",
	std::string inputFilepath = "") :
	TestTemplate(sampleRate, bufferSize, deviceIn, deviceOut, inputFilepath), 
	Flanger(sampleRate) {}

	void onInit() override {
		TestTemplate::onInit(); // Call the base class's init() first so that `gui` is initialized
		
		// params
		this->panel->gui.add(bypass);
		this->panel->gui.add(rate);
		this->panel->gui.add(depth);
		this->panel->gui.add(blend);
	}

	void onCreate() override {
		TestTemplate::onCreate(); // Call the base class' create() first in case we add anything there later
		
		// bypass callback 
		const std::function<void(bool)> bypassCallback = [&](bool a) { if (!a) { this->Flanger.enable(); }
		else { this->Flanger.disable(); } };
		bypass.registerChangeCallback(bypassCallback);
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
		Flanger.setParams(rate, depth, blend);
		float out = Flanger.processSample(in);
		return out;
	}
};

int main() {
	FlangerDemo app(44100, 128, "MacBook Pro Microphone", "External Headphones", "../Resources/likeAStone.wav"); // instance of our app 
	app.start();
	return 0;
}