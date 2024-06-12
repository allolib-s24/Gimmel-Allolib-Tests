// Gimmel Includes 
#include "Utility/AllosphereTemplate.hpp"
#include "Gimmel/include/Gimmel.hpp" //Includes all effects at once

// Allolib include for GUI
#include "al/ui/al_ParameterGUI.hpp"

class AlloFx : public SphereTemplate {
private:
	giml::Tremolo<float> tremolo;
	al::ParameterBool tremoloBypass{ "tremoloBypass", "", true, 0.f, 1.f }; // False means the effect is ON
	al::Parameter speed{ "speed", "", 271.678f, 0.1f, 1000.f };
	al::Parameter depth{ "depth", "", 0.9f, 0.f, 1.f };

	// each effect will have instantiation and params here...
	giml::Saturation<float> saturation;
	al::ParameterBool saturationBypass{ "saturationBypass", "", true, 0.f, 1.f }; // False means the effect is ON
	al::Parameter preAmpGain{"preAmpGain", "", 31.75f, -96.f, 50.f};
	al::Parameter drive{"drive", "", 1.f, 1.f, 75.f};
	al::Parameter volume{"volume", "", -20.f, -96.f, 0.f};
	
	giml::Biquad<float> filter; // lo-pass filter for a cheap cab sim
	al::ParameterBool filterBypass{ "filterBypass", "", true, 0.f, 1.f }; //False means the effect is ON
	al::Parameter cutoffFreq{"cutoffFreq", "", 4000.f, 1.f, 20000.f};
	
	giml::Chorus<float> chorus;
	al::ParameterBool chorusBypass{ "chorusBypass", "", true, 0.f, 1.f }; // False means the effect is ON
	al::Parameter rate{ "rate", "", 0.185f, 0.1f, 15.f };
	al::Parameter chorusDepth{ "depth", "", 10.f, 5.f, 50.f };
	al::Parameter blend{ "blend", "", 0.5f, 0.f, 1.f };


	// vector of detune
	std::vector<giml::Detune<float>> detuneBank; // <- initialize vector of detune
	al::ParameterBool detuneBypass{ "detuneBypass", "", true, 0.f, 1.f }; // False means the effect is ON
	al::Parameter pRatio{ "pRatio", "", 0.007f, 0.0f, 1.f };
	al::Parameter wSize{ "wSize", "", 22.f, 5.f, 50.f };


	giml::Reverb<float> reverb;
	al::ParameterBool reverbBypass{ "reverbBypass", "", true, 0.f, 1.f }; //False means the effect is ON
	al::Parameter time{"time", "", 0.02f, 0.f, 1.f}; //Sec
	al::Parameter space{"space", "", 5.f, 0.f, 50.f}; //ft
	al::Parameter regen{ "regen", "", 0.5f, 0.f, 0.99f }; //ratio
	al::Parameter damping{ "damping", "", 0.5f, 0.f, 0.99f }; //ratio
	al::ParameterMenu reverbRoomType{ "roomType", "", 0};
	al::Parameter reverbAbsorptionCoefficient{ "absorptionCoefficient", "", 0.9, 0.f, 1.f};
	al::Parameter reverbRatio{ "wet/dry", "", 0.25f, 0.f, 1.f }; //ratio

	giml::Compressor<float> compressor;
	al::ParameterBool compressorBypass{ "compressorBypass", "", true, 0.f, 1.f }; //False means the effect is ON
	al::Parameter thresh{"thresh", "", -49.417, -96.f, 6.f};
	al::Parameter ratio{"ratio", "", 14.472f, 1.f, 30.f};
	al::Parameter knee{"knee", "", 4.387f, 0.1f, 10.f};
	al::Parameter gain{"gain", "", 30.f, -96.f, 30.f};
	al::Parameter attack{"attack", "", 3.5f, 1.f, 50.f};
	al::Parameter release{"release", "", 100.f, 1.f, 300.f};

public:
	AlloFx(int sampleRate = 44100, int bufferSize = 256,
	std::string deviceIn = "Microphone", std::string deviceOut = "Speaker",
	std::string inputFilepath = "") :
	SphereTemplate(sampleRate, bufferSize, deviceIn, deviceOut, inputFilepath), reverb(sampleRate), 
	compressor(sampleRate), saturation(sampleRate), filter(sampleRate), tremolo(sampleRate), chorus(sampleRate) {
		filter.setType(giml::Biquad<float>::BiquadUseCase::LPF_1st);
		for (int i = 0; i < static_cast<int>(audioIO().channelsOut()); i++) {
			detuneBank.push_back(giml::Detune<float>(sampleRate)); // <- constructor goes here? 
		}
	}

	void onInit() override {
		auto cuttleboneDomain = al::CuttleboneStateSimulationDomain<State>::enableCuttlebone(this);
		if (!cuttleboneDomain) {
		std::cerr << "ERROR: Could not start Cuttlebone. Quitting." << std::endl;
		quit();
		}
	if (isPrimary()) {
		al::imguiInit(); // GUI init	
	}
	}

	void onCreate() override {
		SphereTemplate::onCreate(); // Call the base class' create() first 

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


		// detune bypass callback 
		const std::function<void(bool)> detuneBypassCallback = [&](bool a) { if (!a) { 
			//this->detune.enable(); 
			for (int i = 0; i < static_cast<int>(audioIO().channelsOut()); i++) {
				detuneBank[i].enable(); // <- constructor goes where? 
			}
		}
		else { 
			//this->detune.disable(); 
			for (int i = 0; i < static_cast<int>(audioIO().channelsOut()); i++) {
				detuneBank[i].disable(); // <- constructor goes where? 
			}
		} };
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
		out = filter.processSample(out);
		out = chorus.processSample(out);
		//out = (detune.processSample(out) * 0.5) + (out * 0.5);
		out = tremolo.processSample(out);
		out = reverb.processSample(out)*reverbRatio + out*(1-reverbRatio);
		out = compressor.processSample(out);
		return out;
		
		/* Signal Chain
		Saturation 
		Lo-Pass
		Chorus
		Tremolo
		Reverb
		Compressor
		*/
	}

	void onSound(al::AudioIOData& io) override {
	if (isPrimary()) {
		SphereTemplate::onSound(io);
		for (int sample = 0; sample < static_cast<int>(io.framesPerBuffer()); sample++) {
			// pitchshift outputs
			for (int channel = 0; channel < io.channelsOut(); channel++) {
				io.out(channel, sample) = this->detuneBank[channel].processSample(io.out(channel, sample));
			}
		}
	}
	}

	void onAnimate(double dt) override { // called ~60fps
		SphereTemplate::onAnimate(dt);
	if (isPrimary()) {
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
		for (int i = 0; i < static_cast<int>(audioIO().channelsOut()); i++) {
			if (i % 2 == 0) {
				// float rat = 1 - ( pRatio / (i + 1) );
				// detuneBank[i].setPitchRatio(rat); // <- goes here?
				// std::cout << "pRatio 0: " << rat << std::endl;
				detuneBank[i].setPitchRatio(1 - ( pRatio / (i + 1) )); // <- goes here?
			}
			else {
				detuneBank[i].setPitchRatio(1 + ( pRatio / i )); // <- goes here?
			}
			detuneBank[i].setWindowSize(wSize);
		}

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
	}

  	void DrawPanel(std::string name, std::vector<al::ParameterMeta*> params) {
		al::ParameterGUI::beginPanel(name);
		for (const auto& param : params) {
			al::ParameterGUI::drawParameterMeta(param);
		}
		al::ParameterGUI::endPanel();
	}

	void onDraw(al::Graphics &g) override {
		SphereTemplate::onDraw(g); //Call the base class's init() first so that oscilloscope is shown
	if (isPrimary()) {
		al::imguiDraw();
	}
	}
};

int main() {
	std::string deviceIn = "";
    std::string deviceOut = "";
  	if (al::Socket::hostName() == "ar01.1g") { // if in AlloSphere...
    	deviceIn = "ECHO X5";
    	deviceOut = "ECHO X5";
 	 } 
  	else { // if not... 
    	deviceIn = "Volt 276";
    	deviceOut = "Volt 276";
  		}
	AlloFx app(44100, 128, deviceIn, deviceOut); // instance of our app 
	app.start();
	return 0;
}