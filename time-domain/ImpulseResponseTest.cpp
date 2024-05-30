#include "../utility/ImpulseResponseTemplate.hpp"
#include "../Gimmel/include/time-domain/Delay.hpp"

class IRTest : public IRTemplate  {
private:
	giml::Delay<float> delay;

	al::ParameterBool bypass{ "bypass", "", true, 0.f, 1.f }; //False means the effect is ON
	al::Parameter feedback{"feedback", "", 0.f, 0.f, 2.f};
	al::Parameter timeMillis{"timeMillis", "", 0.f, 0.f, 1000.f};

public:
	IRTest(int sampleRate = 44100, int bufferSize = 256,
	std::string deviceIn = "Microphone", std::string deviceOut = "Speaker",
	std::string inputFilepath = "") :
	IRTemplate(sampleRate, bufferSize, deviceIn, deviceOut, inputFilepath), 
	delay(sampleRate) {}

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
		const std::function<void(bool)> bypassCallback = [&](bool a) { if (!a) { this->delay.enable(); }
		else { this->delay.disable(); } };
		bypass.registerChangeCallback(bypassCallback);

		// effect parameter callbacks
		const std::function<void(float)> rateCallback = [&](float a) { this->delay.setFeedback(feedback); };
		feedback.registerChangeCallback(rateCallback);

		const std::function<void(float)> depthCallback = [&](float a) { this->delay.setDelayTime(timeMillis); };
		timeMillis.registerChangeCallback(depthCallback);
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
		//float out = delay.processSample(in);
		return in;
	}
};

int main() {
	IRTest app(44100, 128, "MacBook Pro Microphone", "Headphones"); // instance of our app 
	app.start();
	return 0;
}