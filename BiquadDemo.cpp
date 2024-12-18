#include "utility/TestTemplate.hpp"
#include "Gimmel/include/Biquad.hpp"
#include "al/math/al_Random.hpp"

class BiquadDemo : public TestTemplate {
private:
	giml::Biquad<float> filter;

	al::ParameterBool bypass{ "bypass", "", true, 0.f, 1.f }; //False means the effect is ON
	al::Parameter freq{"freq", "", 20000.f, 0.f, 20000.f};
	al::Parameter q{"q", "", 1.f, 0.f, 10.f};
	al::Parameter gain{"gain", "", 0.f, -96.f, 30.f};
	al::ParameterMenu filterType{"filterType", "", 0};


public:
	BiquadDemo(int sampleRate = 44100, int bufferSize = 256,
	std::string deviceIn = "Microphone", std::string deviceOut = "Speaker",
	std::string inputFilepath = "") :
	TestTemplate(sampleRate, bufferSize, deviceIn, deviceOut, inputFilepath),
	filter(sampleRate) {}

	void onInit() override {
		TestTemplate::onInit(); //Call the base class's init() first so that `gui` is initialized
		
		//TODO: Add other parameters you'd need here
		this->panel->gui.add(bypass);
		this->panel->gui.add(freq);
		this->panel->gui.add(q);
		this->panel->gui.add(gain);
		this->panel->gui.add(filterType);
	}

	void onCreate() override {
		TestTemplate::onCreate(); //Call the base class' create() first in case we add anything there later
		
		// bypass callback 
		const std::function<void(bool)> bypassCallback = [&](bool a) { if (!a) { this->filter.enable(); }
		else { this->filter.disable(); } };
		bypass.registerChangeCallback(bypassCallback);

		std::vector<std::string> filterTypes = {
            //Basic RC
            "LPF_1st",        // First-Order Lowpass Filter (LPF)
            "HPF_1st",        // First-Order Highpass Filter (HPF)
            
            //Second-order filters
            "LPF_2nd",        // Second-Order LPF
            "HPF_2nd",        // Second-Order HPF
            "BPF",            // Bandpass Filter (BPF)
            "BSF",            // Bandstop Filter (BSF)
            
            //Butterworth Filters
            "LPF_Butterworth",// Butterworth LPF
            "HPF_Butterworth",// Butterworth HPF
            "BPF_Butterworth",// Butterworth BPF
            "BSF_Butterworth",// Butterworth BSF

            //Linkwitz-Riley - steeper than Butterworth
            "LPF_LR",         // Linkwitz-Riley LPF
            "HPF_LR",         // Linkwitz-Riley HPF

            //All-pass filters (no frequency changes, only phase shift)
            "APF_1st",        // First-Order Allpass Filter (APF)
            "APF_2nd",        // Second-Order APF -> 2nd-Order APF has double the phase shift

            //Shelf filters
            "LSF",        // First-Order Low Shelf Filter (LSF)
            "HSF",        // First-Order High Shelf Filter (HSF)

            //Parametric EQ Filters
            "PEQ",            //(non-const Q)
            "PEQ_constQ"      // Parametric EQ Filter (const Q)
		};

		filterType.setElements(filterTypes);
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
		// Print noise for testing
		float noise = al::rnd::uniformS();

		// set filterType 
		switch (filterType) {
			// First-Order Filters
			case 0: {
				this->filter.setType(giml::Biquad<float>::BiquadUseCase::LPF_1st); // "LPF_1st" // First-Order Lowpass Filter (LPF)
				break;
			}
			case 1: {
				this->filter.setType(giml::Biquad<float>::BiquadUseCase::HPF_1st); // "HPF_1st"  // First-Order Highpass Filter (HPF)
				break;
			}

			case 2: {
				this->filter.setType(giml::Biquad<float>::BiquadUseCase::LPF_2nd); // "LPF_2nd" // Second-Order LPF
				break;
			}
			case 3: {
				this->filter.setType(giml::Biquad<float>::BiquadUseCase::HPF_2nd); // "HPF_2nd"  // Second-Order HPF
				break;
			}
			case 4: {
				this->filter.setType(giml::Biquad<float>::BiquadUseCase::BPF); // "BPF" // Bandpass Filter (BPF)
				break;
			}
			case 5: {
				this->filter.setType(giml::Biquad<float>::BiquadUseCase::BSF); // "BSF" // Bandstop Filter (BSF)
				break;
			}
			// Butterworth Filters
			case 6: {
				this->filter.setType(giml::Biquad<float>::BiquadUseCase::LPF_Butterworth); // "LPF_Butterworth" // Butterworth LPF
				break;
			}
			case 7: {
				this->filter.setType(giml::Biquad<float>::BiquadUseCase::HPF_Butterworth); // "HPF_Butterworth" // Butterworth HPF
				break;
			}	
			case 8: {
				this->filter.setType(giml::Biquad<float>::BiquadUseCase::BPF_Butterworth); // "BPF_Butterworth" // Butterworth BPF
				break;
			}
			case 9: {
				this->filter.setType(giml::Biquad<float>::BiquadUseCase::BSF_Butterworth); // "BSF_Butterworth" // Butterworth BSF
				break;
			}
			// Linkwitz-Riley - steeper than Butterworth
			case 10: {
				//this->filter.setParams__LPF_LR(freq, q); // "LPF_LR" // Linkwitz-Riley LPF
				break;
			}	
			case 11: {
				//this->filter.setParams__LPF_LR(freq, q); // "HPF_LR" // Linkwitz-Riley HPF
				break;
			}
			// All-pass filters (no frequency changes, only phase shift)
			case 12: {
				this->filter.setType(giml::Biquad<float>::BiquadUseCase::APF_1st); // "APF_1st" // First-Order Allpass Filter (APF)
				break;
			}		
			case 13: {
				this->filter.setType(giml::Biquad<float>::BiquadUseCase::APF_2nd); // "APF_2nd" // Second-Order APF -> 2nd-Order APF has double the phase shift
				break;
			}	
			// Shelf filters
			case 14: {
				this->filter.setType(giml::Biquad<float>::BiquadUseCase::LSF); // "LSF" // First-Order Low Shelf Filter (LSF)
				break;
			}	
			case 15: {
				this->filter.setType(giml::Biquad<float>::BiquadUseCase::HSF); // "HSF" // First-Order High Shelf Filter (HSF)
				break;
			}	
			// Parametric EQ Filters
			case 16: {
				this->filter.setType(giml::Biquad<float>::BiquadUseCase::PEQ); // "PEQ" //(non-const Q)
				break;
			}	
			case 17: {
				this->filter.setType(giml::Biquad<float>::BiquadUseCase::PEQ_constQ); // "PEQ_constQ" // Parametric EQ Filter (const Q)
				break;
			}					
		}

		this->filter.setParams(freq, q, gain);

		float out = this->filter.processSample(noise);
		return out;
	}
};

int main() {
	BiquadDemo app(44100, 512, "MacBook Pro Microphone", "BlackHole 2ch"); // instance of our app 
	app.start();
	return 0;
}