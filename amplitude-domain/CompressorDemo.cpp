#include "../utility/TestTemplate.hpp"
#include "../Gimmel/include/amplitude-domain/Compressor.hpp"

class CompressorDemo : public TestTemplate {
private:
	giml::Compressor<float> compressor;

	al::ParameterBool bypass{ "bypass", "", true, 0.f, 1.f }; //False means the effect is ON
	al::Parameter thresh{"thresh", "", 0.0, 6.f, -96.f};
	al::Parameter ratio{"ratio", "", 1.f, 1.f, 30.f};

public:
	CompressorDemo(int sampleRate = 44100, int bufferSize = 256,
	std::string deviceIn = "Microphone", std::string deviceOut = "Speaker",
	std::string inputFilepath = "") :
	TestTemplate(sampleRate, bufferSize, deviceIn, deviceOut, inputFilepath),
	compressor(sampleRate) {}

	void onInit() override {
		TestTemplate::onInit(); //Call the base class's init() first so that `gui` is initialized
		
		//TODO: Add other parameters you'd need here
		this->panel->gui.add(bypass);
		this->panel->gui.add(thresh);
		this->panel->gui.add(ratio);
	}

	void onCreate() override {
		TestTemplate::onCreate(); //Call the base class' create() first in case we add anything there later
		// bypass callback 
		const std::function<void(bool)> bypassCallback = [&](bool a) { if (!a) { this->compressor.enable(); }
		else { this->compressor.disable(); } };
		bypass.registerChangeCallback(bypassCallback);

		// effect parameter callbacks
		const std::function<void(float)> rateCallback = [&](float a) { this->compressor.setRatio(ratio); };
		ratio.registerChangeCallback(rateCallback);

		const std::function<void(float)> depthCallback = [&](float a) { this->compressor.setThreshold(thresh); };
		thresh.registerChangeCallback(depthCallback);
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
		float out = compressor.processSample(in);
		return out;
	}
};

int main() {
	CompressorDemo app(44100, 512, "MacBook Pro Microphone", "MacBook Pro Speakers", "../../Resources/3xGmaj.wav"); // instance of our app 
	app.start();
	return 0;
}