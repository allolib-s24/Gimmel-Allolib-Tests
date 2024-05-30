#include "../utility/ImpulseResponseTemplate.hpp"
#include "../Gimmel/include/Reverb.hpp"

class IRTest : public IRTemplate  {
private:
	giml::Reverb<float> reverb;

	al::ParameterBool bypass{ "bypass", "", true, 0.f, 1.f }; //False means the effect is ON
	al::Parameter feedback{"feedback", "", 0.f, 0.f, 2.f};
	al::Parameter timeMillis{"timeMillis", "", 0.f, 0.f, 1000.f};

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
		this->panel->gui.add(feedback);
		this->panel->gui.add(timeMillis);
	}

	void onCreate() override {
		IRTemplate::onCreate(); //Call the base class' create() first in case we add anything there later
		// bypass callback 
		const std::function<void(bool)> bypassCallback = [&](bool a) { if (!a) { this->reverb.enable(); }
		else { this->reverb.disable(); } };
		bypass.registerChangeCallback(bypassCallback);

		this->reverb.setTime(0.02); //in ms
		this->reverb.setRoom(10); //in ft
		this->reverb.setDamping(0); //0 means no low pass added

		//// effect parameter callbacks
		//const std::function<void(float)> rateCallback = [&](float a) { this->reverb.setFeedback(feedback); };
		//feedback.registerChangeCallback(rateCallback);

		//const std::function<void(float)> depthCallback = [&](float a) { this->reverb.setreverbTime(timeMillis); };
		//timeMillis.registerChangeCallback(depthCallback);
	}

	bool onKeyDown(const al::Keyboard &k) override {
		bool returnVal = IRTemplate::onKeyDown(k);
		//TODO: Add any other keyboard logic you want here
		if (k.key() == 'b') { // <- on b, toggle effect bypass
			bypass = !bypass;
			std::cout << "Bypass Status: " << bypass << std::endl;
		}
		return returnVal;
	}

	float sampleLoop(float in) override {
		// DSP logic goes here
		float out = reverb.processSample(in);
		return out;
	}
};

int main() {
	IRTest app(44100, 128, "Microphone", "Headphones"); // instance of our app 
	app.start();
	return 0;
}