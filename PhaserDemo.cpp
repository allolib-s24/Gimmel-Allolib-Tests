#include "Utility/TestTemplate.hpp"
#include "Gimmel/include/phaser.hpp"

class PhaserDemo : public TestTemplate {
private:
	giml::Phaser<float> phaser;

	al::ParameterBool bypass{ "bypass", "", true, 0.f, 1.f }; //False means the effect is ON
	al::Parameter rate{"rate", "", 0.2f, 0.f, 20.f};
	al::Parameter feedback{"feedback", "", 0.85f, -1.f, 1.f};

public:
	PhaserDemo(int sampleRate = 44100, int bufferSize = 256,
	std::string deviceIn = "Microphone", std::string deviceOut = "Speaker",
	std::string inputFilepath = "") :
	TestTemplate(sampleRate, bufferSize, deviceIn, deviceOut, inputFilepath), 
	phaser(sampleRate) {}

	void onInit() override {
		TestTemplate::onInit(); // Call the base class's init() first so that `gui` is initialized
		
		this->panel->gui.add(bypass);
		this->panel->gui.add(rate);
		this->panel->gui.add(feedback);
	}

	void onCreate() override {
		TestTemplate::onCreate(); // Call the base class' create() first in case we add anything there later
		// bypass callback 
		const std::function<void(bool)> bypassCallback = [&](bool a) { if (!a) { this->phaser.enable(); }
		else { this->phaser.disable(); } };
		bypass.registerChangeCallback(bypassCallback);
	}

	bool onKeyDown(const al::Keyboard &k) override {
		bool returnVal = TestTemplate::onKeyDown(k);
		if (k.key() == 'b') { // <- on b, toggle effect bypass
			bypass = !bypass;
			std::cout << "Bypass Status: " << bypass << std::endl;
		}
		return returnVal;
	}

	float sampleLoop(float in) override {
		// DSP logic goes here
		phaser.setParams(rate, feedback);
		return phaser.processSample(in);
	}
};

int main() {
	PhaserDemo app(44100, 512, "MacBook Pro Microphone", "Headphones", "../Resources/3xGmaj.wav"); // instance of our app 
	app.start();
	return 0;
}