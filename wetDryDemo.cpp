#include "Utility/TestTemplate.hpp"
#include "Gimmel/include/gimmel.hpp"

class wetDryDemo : public TestTemplate {
public:
	giml::Detune<float> mDetune{44100}; 
	al::Parameter wetDry{"wetDry", "", 0.f, 0.f, 1.f};

	wetDryDemo(int sampleRate = 44100, int bufferSize = 256,
	std::string deviceIn = "Microphone", std::string deviceOut = "External Headphones",
	std::string inputFilepath = "") :
	TestTemplate(sampleRate, bufferSize, deviceIn, deviceOut, inputFilepath) {}

	void onInit() override {
		TestTemplate::onInit(); //Call the base class's init() first so that `gui` is initialized
		mDetune.setPitchRatio(0.993);
		mDetune.enable();
		this->panel->gui.add(wetDry);
	}

	float sampleLoop(float in) override {
		return giml::powMix<float>(in, mDetune.processSample(in), wetDry);
	}
};

int main() {
	wetDryDemo app(44100, 128, "MacBook Pro Microphone", "MacBook Pro Speakers", "../Resources/SoftGuitar.wav"); // instance of our app 
	app.start();
	return 0;
}