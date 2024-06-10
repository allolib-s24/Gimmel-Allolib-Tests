// Gimmel Includes 
#include "Utility/SphereTemplate.hpp"
#include "Gimmel/include/Gimmel.hpp" //Includes all effects at once

// Allolib include for GUI
#include "al/ui/al_ParameterGUI.hpp"

class MultiFxDemo : public TestTemplate {
private:
	giml::Tremolo<float> tremolo;
	al::ParameterBool tremoloBypass{ "tremoloBypass", "", true, 0.f, 1.f }; // False means the effect is ON
	al::Parameter speed{ "speed", "", 100.f, 0.1f, 1000.f };
	al::Parameter depth{ "depth", "", 0.9f, 0.f, 1.f };

	// each effect will have instantiation and params here...
	giml::Saturation<float> saturation;
	al::ParameterBool saturationBypass{ "saturationBypass", "", true, 0.f, 1.f }; // False means the effect is ON
	al::Parameter preAmpGain{"preAmpGain", "", 20.f, -96.f, 50.f};
	al::Parameter drive{"drive", "", 26.f, 1.f, 75.f};
	al::Parameter volume{"volume", "", -20.f, -96.f, 0.f};
	
	giml::Biquad<float> filter; // lo-pass filter for a cheap cab sim
	al::ParameterBool filterBypass{ "filterBypass", "", true, 0.f, 1.f }; //False means the effect is ON
	al::Parameter cutoffFreq{"cutoffFreq", "", 4000.f, 1.f, 20000.f};
	
	giml::Chorus<float> chorus;
	al::ParameterBool chorusBypass{ "chorusBypass", "", true, 0.f, 1.f }; // False means the effect is ON
	al::Parameter rate{ "rate", "", 1.f, 0.1f, 15.f };
	al::Parameter chorusDepth{ "depth", "", 20.f, 5.f, 50.f };
	al::Parameter blend{ "blend", "", 0.5f, 0.f, 1.f };

	giml::Detune<float> detune;
	al::ParameterBool detuneBypass{ "detuneBypass", "", true, 0.f, 1.f }; // False means the effect is ON
	al::Parameter pRatio{ "pRatio", "", 0.993f, 0.5f, 2.f };
	al::Parameter wSize{ "wSize", "", 22.f, 5.f, 50.f };

	giml::Reverb<float> reverb;
	al::ParameterBool reverbBypass{ "reverbBypass", "", true, 0.f, 1.f }; //False means the effect is ON
	al::Parameter time{"time", "", 0.02f, 0.f, 1.f}; //Sec
	al::Parameter space{"space", "", 5.f, 0.f, 50.f}; //ft
	al::Parameter regen{ "regen", "", 0.5f, 0.f, 0.99f }; //ratio
	al::Parameter damping{ "damping", "", 0.5f, 0.f, 0.99f }; //ratio
	al::ParameterMenu reverbRoomType{ "roomType", "", 0 };
	al::Parameter reverbAbsorptionCoefficient{ "absorptionCoefficient", "", 0.75, 0, 1. };
	al::Parameter reverbRatio{ "wet/dry", "", 0.25f, 0.f, 1.f }; //ratio

	giml::Compressor<float> compressor;
	al::ParameterBool compressorBypass{ "compressorBypass", "", true, 0.f, 1.f }; //False means the effect is ON
	al::Parameter thresh{"thresh", "", -20, -96.f, 6.f};
	al::Parameter ratio{"ratio", "", 4.f, 1.f, 30.f};
	al::Parameter knee{"knee", "", 2.f, 0.1f, 10.f};
	al::Parameter gain{"gain", "", 10.f, -96.f, 30.f};
	al::Parameter attack{"attack", "", 3.5f, 1.f, 50.f};
	al::Parameter release{"release", "", 100.f, 1.f, 300.f};

public:
	MultiFxDemo(int sampleRate = 44100, int bufferSize = 256,
	std::string deviceIn = "Microphone", std::string deviceOut = "Speaker",
	std::string inputFilepath = "") :
	TestTemplate(sampleRate, bufferSize, deviceIn, deviceOut, inputFilepath),
	detune(sampleRate), reverb(sampleRate), compressor(sampleRate), saturation(sampleRate), filter(sampleRate), tremolo(sampleRate), chorus(sampleRate) {
		filter.setType(giml::Biquad<float>::BiquadUseCase::LPF_1st);
	}

	void onInit() override {
		al::imguiInit(); // GUI init
	}

	void onCreate() override {
		TestTemplate::onCreate(); // Call the base class' create() first 

		// bypass callback 
		const std::function<void(bool)> tremoloBypassCallback = [&](bool a) { if (!a) { this->tremolo.enable(); }
		else { this->tremolo.disable(); } };
		tremoloBypass.registerChangeCallback(tremoloBypassCallback);

		// bypass callback 
		const std::function<void(bool)> saturationBypassCallback = [&](bool a) { if (!a) { this->saturation.enable(); }
		else { this->saturation.disable(); } };
		saturationBypass.registerChangeCallback(saturationBypassCallback);

		// bypass callback 
		const std::function<void(bool)> filterBypassCallback = [&](bool a) { if (!a) { this->filter.enable(); }
		else { this->filter.disable(); } };
		filterBypass.registerChangeCallback(filterBypassCallback);

		// bypass callback 
		const std::function<void(bool)> chorusBypassCallback = [&](bool a) { if (!a) { this->chorus.enable(); }
		else { this->chorus.disable(); } };
		chorusBypass.registerChangeCallback(chorusBypassCallback);

		// bypass callback 
		const std::function<void(bool)> detuneBypassCallback = [&](bool a) { if (!a) { this->detune.enable(); }
		else { this->detune.disable(); } };
		detuneBypass.registerChangeCallback(detuneBypassCallback);

		// bypass callback 
		const std::function<void(bool)> reverbBypassCallback = [&](bool a) { if (!a) { this->reverb.enable(); }
		else { this->reverb.disable(); } };
		reverbBypass.registerChangeCallback(reverbBypassCallback);

		// bypass callback 
		const std::function<void(bool)> compressorBypassCallback = [&](bool a) { if (!a) { this->compressor.enable(); }
		else { this->compressor.disable(); } };
		compressorBypass.registerChangeCallback(compressorBypassCallback);
	}

	float sampleLoop(float in) override { // called inside onSound
		// DSP logic goes here

		float out = in;
		out = saturation.processSample(out);
		out = tremolo.processSample(out);
		out = filter.processSample(out);
		out = chorus.processSample(out);
		out = (detune.processSample(out) * 0.5) + (out * 0.5);
		out = reverb.processSample(out)*reverbRatio + out*(1-reverbRatio);
		out = compressor.processSample(out);
		return out;
		
		/* Signal Chain
		Saturation 
		Lo-Pass
		Detune
		Reverb
		Compressor
		*/
	
	}

	void onAnimate(double dt) override { // called ~60fps
		TestTemplate::onAnimate(dt);
		// GUI Setup
		al::imguiBeginFrame();
		// Add a Panel for each effect
		this->DrawPanel("Tremolo", { &tremoloBypass, &speed, &depth });
		this->DrawPanel("Saturation", {&saturationBypass, &preAmpGain, &drive, &volume});
		this->DrawPanel("Filter", {&filterBypass, &cutoffFreq});
		this->DrawPanel("Chorus", {&chorusBypass, &rate, &chorusDepth, &blend});
		this->DrawPanel("Detune", {&detuneBypass, &pRatio, &wSize});
		this->DrawPanel("Reverb", {&reverbBypass, &time, &space, &regen, &damping, &reverbRoomType, &reverbAbsorptionCoefficient, &reverbRatio});
		reverbRoomType.setElements({ "sphere", "cube", "square_pyramid", "cylinder" });
		this->DrawPanel("Compressor", {&compressorBypass, &thresh, &ratio, &knee, &gain, &attack, &release});
		al::imguiEndFrame();

		// saturation setters
		tremolo.setSpeed(speed);
		tremolo.setDepth(depth);

		// saturation setters
		saturation.setPreAmpGain(preAmpGain);
		saturation.setDrive(drive);
		saturation.setVolume(volume);

		// filter setters
		filter.setParams(cutoffFreq);

		// chorus setters
		chorus.setBlend(blend);
		chorus.setDepth(chorusDepth);
		chorus.setRate(rate);

		// detune setters
		detune.setPitchRatio(pRatio);
		detune.setWindowSize(wSize);

		// reverb setters 
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
		reverb.setParams(time, regen, damping, space, reverbAbsorptionCoefficient, type);

		// compressor setters 
		compressor.setThresh(thresh);
		compressor.setRatio(ratio);
		compressor.setKnee(knee);
		compressor.setMakeupGain(gain);
		compressor.setAttack(attack);
		compressor.setRelease(release);
	}

  	void DrawPanel(std::string name, std::vector<al::ParameterMeta*> params) {
		al::ParameterGUI::beginPanel(name);
		for (const auto& param : params) {
			al::ParameterGUI::drawParameterMeta(param);
		}
		al::ParameterGUI::endPanel();
	}

	void onDraw(al::Graphics &g) override {
		TestTemplate::onDraw(g);
		al::imguiDraw();
	}
};

int main() {
	MultiFxDemo app(48000, 128, "Microphone", "Speaker", "../Resources/likeAStone.wav"); // instance of our app 
	app.start();
	return 0;
}