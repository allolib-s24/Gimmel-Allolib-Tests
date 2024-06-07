#include "Utility/ImpulseResponseTemplate.hpp"
#include "Gimmel/include/Reverb.hpp"

class IRTest : public IRTemplate {
private:
	giml::Reverb<float> reverb;

	al::ParameterBool bypass{ "bypass", "", true, 0.f, 1.f }; //False means the effect is ON
	al::Parameter reverbTime{ "time", "", 0.02f, 0.00001f, 10.f }; //Sec
	al::Parameter reverbSpace{ "space", "", 10.f, 0.f, 1000.f }; //ft
	al::Parameter reverbRegen{ "regen", "", 0.f, 0.f, 0.99f }; //ratio
	al::Parameter reverbDamping{ "damping", "", 0.f, 0.f, 0.99f }; //ratio
	al::Parameter reverbAPFFeedback{ "apfFeedback", "", 0.f, 0.f, 0.99f }; //ratio
	al::Parameter reverbAPFDelay{ "apfDelay (ms)", "", 0.f, 0.f, 100.f }; //ratio
	al::ParameterMenu reverbRoomType{ "roomType", "", 0 };
	al::Parameter reverbAbsorptionCoefficient{ "absorptionCoefficient", "", 0.75, 0, 1. };

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
		this->panel->gui.add(reverbRoomType);
		reverbRoomType.setElements({ "sphere", "cube", "square_pyramid", "cylinder"});
		this->panel->gui.add(reverbSpace);
		this->panel->gui.add(reverbRegen);
		this->panel->gui.add(reverbDamping);
		//this->panel->gui.add(reverbAPFFeedback);
		//this->panel->gui.add(reverbAPFDelay);
		this->panel->gui.add(reverbAbsorptionCoefficient);
	}

	void onCreate() override {
		IRTemplate::onCreate(); //Call the base class' create() first in case we add anything there later
		// bypass callback 
		const std::function<void(bool)> bypassCallback = [&](bool a) { if (!a) { this->reverb.enable(); }
		else { this->reverb.disable(); } };
		bypass.registerChangeCallback(bypassCallback);

		//this->reverb.setTime(reverbTime); //in sec
		//this->reverb.setRoom(reverbSpace); //in ft
		//this->reverb.setRegen(reverbRegen); //0 means no low pass added

		// effect parameter callbacks
		const std::function<void(float)> timeCallback = [&](float a) {
			giml::Reverb<float>::RoomType type;
			switch (reverbRoomType) {
				case 1: //Cube
					type = giml::Reverb<float>::RoomType::CUBE;
					break;
				case 2: //Square_Pyramid
					type = giml::Reverb<float>::RoomType::SQUARE_PYRAMID;
					break;
				case 3:
					type = giml::Reverb<float>::RoomType::CYLINDER;
					break;
				default:
				case 0: //Sphere
					type = giml::Reverb<float>::RoomType::SPHERE;
					break;
			}
			
			this->reverb.setParams(reverbTime, reverbRegen, reverbDamping, reverbSpace, type, reverbAbsorptionCoefficient);
			//this->reverb.setAPFFeedback(reverbAPFFeedback);
			//this->reverb.setAPFDelay(reverbAPFDelay);
			};
		reverbTime.registerChangeCallback(timeCallback);

		//const std::function<void(float)> spaceCallback = [&](float a) { this->reverb.setRoom(a); };
		reverbSpace.registerChangeCallback(timeCallback);
		reverbRoomType.registerChangeCallback(timeCallback);
		reverbAbsorptionCoefficient.registerChangeCallback(timeCallback);

		//const std::function<void(float)> dampingCallback = [&](float a) { this->reverb.setRegen(a); };
		reverbRegen.registerChangeCallback(timeCallback);
		reverbDamping.registerChangeCallback(timeCallback);
		reverbAPFFeedback.registerChangeCallback(timeCallback);
		//reverbAPFDelay.registerChangeCallback(timeCallback);
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
		float out = 0.75 * reverb.processSample(in) + 0.25 * in;
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
	IRTest app(48000, 128, "Microphone", "Speakers"/*, "../Resources/likeAStone.wav"*/); // instance of our app 
	app.start();
	return 0;
}