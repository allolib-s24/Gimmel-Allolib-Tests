#include "Utility/TestTemplate.hpp"
#include "Gimmel/include/Phaser.hpp"

class PhaserDemo : public TestTemplate {
private:
	giml::Phaser<float> phaser;

	al::ParameterBool bypass{ "bypass", "", true, 0.f, 1.f }; //False means the effect is ON
	al::Parameter rate{"rate", "", 1.f, 0.f, 20.f};

public:
	PhaserDemo(int sampleRate = 44100, int bufferSize = 256,
	std::string deviceIn = "Microphone", std::string deviceOut = "Speaker",
	std::string inputFilepath = "") :
	TestTemplate(sampleRate, bufferSize, deviceIn, deviceOut, inputFilepath), 
	phaser(sampleRate) {}

	void onInit() override {
		TestTemplate::onInit(); //Call the base class's init() first so that `gui` is initialized
		
		//TODO: Add other parameters you'd need here
		this->panel->gui.add(bypass);
		this->panel->gui.add(rate);
	}

	void onCreate() override {
		TestTemplate::onCreate(); //Call the base class' create() first in case we add anything there later
		// bypass callback 
		const std::function<void(bool)> bypassCallback = [&](bool a) { if (!a) { this->phaser.enable(); }
		else { this->phaser.disable(); } };
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
		phaser.setRate(this->rate);
		float out = phaser.processSample(in);
		return out;
	}
};

int main() {
	PhaserDemo app(44100, 512, "MacBook Pro Microphone", "Headphones", "../Resources/SoftGuitar.wav"); // instance of our app 
	app.start();
	return 0;
}