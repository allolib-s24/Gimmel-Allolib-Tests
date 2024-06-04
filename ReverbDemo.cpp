#include "Utility/ImpulseResponseTemplate.hpp"
#include "Gimmel/include/Reverb.hpp"

class IRTest : public IRTemplate  {
private:
	giml::Reverb<float> reverb;

	al::ParameterBool bypass{ "bypass", "", true, 0.f, 1.f }; //False means the effect is ON
	al::Parameter reverbTime{"time", "", 0.02f, 0.f, 10.f}; //Sec
	al::Parameter reverbSpace{"space", "", 10.f, 0.f, 1000.f}; //ft
	al::Parameter reverbDamping{ "damping", "", 0.f, 0.f, 0.99f }; //ratio

public:
	IRTest(int sampleRate = 44100, int bufferSize = 256,
	std::string deviceIn = "Microphone", std::string deviceOut = "Speaker",
	std::string inputFilepath = "") :
	IRTemplate(sampleRate, bufferSize, deviceIn, deviceOut, inputFilepath), 
	reverb(sampleRate) {}

	void onInit() override {
		IRTemplate::onInit(); //Call the base class's init() first so that `gui` is initialized
		
		//TODO: Add other parameters you'd need here
		this->panel->gui.add(bypass);
		this->panel->gui.add(reverbTime);
		this->panel->gui.add(reverbSpace);
		this->panel->gui.add(reverbDamping);
	}

	void onCreate() override {
		IRTemplate::onCreate(); //Call the base class' create() first in case we add anything there later
		// bypass callback 
		const std::function<void(bool)> bypassCallback = [&](bool a) { if (!a) { this->reverb.enable(); }
		else { this->reverb.disable(); } };
		bypass.registerChangeCallback(bypassCallback);

		this->reverb.setTime(reverbTime); //in sec
		this->reverb.setRoom(reverbSpace); //in ft
		this->reverb.setDamping(reverbDamping); //0 means no low pass added

		// effect parameter callbacks
		const std::function<void(float)> timeCallback = [&](float a) { this->reverb.setTime(a); };
		reverbTime.registerChangeCallback(timeCallback);

		const std::function<void(float)> spaceCallback = [&](float a) { this->reverb.setRoom(a); };
		reverbSpace.registerChangeCallback(spaceCallback);

		const std::function<void(float)> dampingCallback = [&](float a) { this->reverb.setDamping(a); };
		reverbDamping.registerChangeCallback(dampingCallback);
	}

	bool onKeyDown(const al::Keyboard &k) override {
		bool returnVal = IRTemplate::onKeyDown(k);
		//TODO: Add any other keyboard logic you want here
		if (k.key() == 'b') { // <- on b, toggle effect bypass
			bypass = !bypass;
			std::cout << "Bypass Status: " << bypass << std::endl;
		}
		if (k.key() == '2') { // <- on b, toggle effect bypass
			last = 1.f;
			std::cout << "BANG!" << std::endl;
		}
		return returnVal;
	}

	float last = 0.f;
	float sampleLoop(float in) override {
		// DSP logic goes here
		float out = reverb.processSample(in);
		// gain -= decrement rewritten as gain *=
		//
		// float decrement = 1.f / (reverbTime * this->sampleRate);
		// //float gain = 1.f - decrement;
		// float gain = 0.2;
		// float out = in + last * -gain;
		// last = out;
		return out;
	}
};

int main() {
	IRTest app(48000, 128, "MacBook Pro Microphone", "MacBook Pro Speakers", "../Resources/vocals.wav"); // instance of our app 
	app.start();
	return 0;
}