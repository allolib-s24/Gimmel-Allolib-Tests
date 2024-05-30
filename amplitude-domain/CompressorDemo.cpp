#include "../utility/TestTemplate.hpp"
#include "../Gimmel/include/amplitude-domain/Compressor.hpp"

class CompressorDemo : public TestTemplate {
private:
	giml::Compressor<float> compressor;

	al::ParameterBool bypass{ "bypass", "", true, 0.f, 1.f }; //False means the effect is ON
	al::Parameter thresh{"thresh", "", 0, -96.f, 6.f};
	al::Parameter ratio{"ratio", "", 1.f, 1.f, 30.f};
	al::Parameter knee{"knee", "", 1.f, 0.f, 10.f};
	al::Parameter gain{"gain", "", 0.f, -96.f, 30.f};
	al::Parameter attack{"attack", "", 1.f, 1.f, 50.f};
	al::Parameter release{"release", "", 1.f, 1.f, 300.f};


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
		this->panel->gui.add(knee);
		this->panel->gui.add(gain);
		this->panel->gui.add(attack);
		this->panel->gui.add(release);
	}

	void onCreate() override {
		TestTemplate::onCreate(); //Call the base class' create() first in case we add anything there later
		// bypass callback 
		const std::function<void(bool)> bypassCallback = [&](bool a) { if (!a) { this->compressor.enable(); }
		else { this->compressor.disable(); } };
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
		this->compressor.setRatio(ratio);
		this->compressor.setThresh(thresh);
		this->compressor.setKnee(knee);
		this->compressor.setAttackTime(attack);
		this->compressor.setReleaseTime(release);
		float out = this->compressor.processSample(in * giml::dBtoA(this->gain));
		return out;
	}
};

int main() {
	CompressorDemo app(44100, 128, "MacBook Pro Microphone", "Headphones", "../../Resources/HuckFinn.wav"); // instance of our app 
	app.start();
	return 0;
}