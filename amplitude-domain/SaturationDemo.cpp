#include "../utility/TestTemplate.hpp"
#include "../Gimmel/include/amplitude-domain/Saturation.hpp"

class SaturationDemo : public TestTemplate {
private:
	giml::Saturation<float> saturation;
	giml::SinOsc osc;

	al::ParameterBool bypass{ "bypass", "", true, 0.f, 1.f }; //False means the effect is ON
	al::Parameter gain{"gain", "", 0.f, -96.f, 30.f};
	al::Parameter volume{"volume", "", 0.f, -96.f, 30.f};
	al::Parameter drive{"drive", "", 1.f, 0.f, 20.f};


public:
	SaturationDemo(int sampleRate = 44100, int bufferSize = 256,
	std::string deviceIn = "Microphone", std::string deviceOut = "Speaker",
	std::string inputFilepath = "") :
	TestTemplate(sampleRate, bufferSize, deviceIn, deviceOut, inputFilepath),
	saturation(), osc(sampleRate) {}

	void onInit() override {
		TestTemplate::onInit(); //Call the base class's init() first so that `gui` is initialized
		
		//TODO: Add other parameters you'd need here
		this->panel->gui.add(bypass);
		this->panel->gui.add(gain);
		this->panel->gui.add(volume);
		this->panel->gui.add(drive);
		osc.setFrequency(1.f);
	}

	void onCreate() override {
		TestTemplate::onCreate(); //Call the base class' create() first in case we add anything there later
		// bypass callback 
		const std::function<void(bool)> bypassCallback = [&](bool a) { if (!a) { this->saturation.enable(); }
		else { this->saturation.disable(); } };
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
		this->saturation.setGain(giml::dBtoA(this->gain));
		this->saturation.setVolume(giml::dBtoA(this->volume));
		this->saturation.setDrive(giml::dBtoA(this->drive));

		float out = this->saturation.processSample(this->osc.processSample() * giml::dBtoA(this->gain));
		//float out = osc.processSample();
		return out;
	}
};

int main() {
	SaturationDemo app(44100, 128, "MacBook Pro Microphone", "MacBook Pro Speakers"); // instance of our app 
	app.start();
	return 0;
}