// Joel A. Jaffe 2024-04-21
// Basic Audio Input/Output App for Testing DSP Code

#include "al/app/al_App.hpp"
#include "al/graphics/al_Mesh.hpp"
#include "al/app/al_GUIDomain.hpp"
using namespace al;

#include <iostream>
using namespace std;

#include "Gimmel/include/amplitude-domain/Compressor.hpp"
#include "Gimmel/include/utility.hpp"

// handy functions in audio
float dBtoA(float dBVal) { return powf(10.f, dBVal / 20.f); }
float ampTodB(float ampVal) { return 20.f * log10f(fabs(ampVal)); }

//#define CIRCLE //Use this to toggle between old oscilloscope implementation and new oscilloscope implementation

#ifdef CIRCLE
//#include <mutex>
#endif
// Oscilloscope that inherits from mesh 
class Oscilloscope : public Mesh {
private:
	int bufferSize;
#ifdef CIRCLE
	giml::CircularBufferForOscilloscope<float> Buffer;
#else
	std::vector<float> buffer;
#endif

public:
	//std::mutex mtx;
	Oscilloscope(int samplerate) : bufferSize(samplerate) {
		this->primitive(Mesh::LINE_STRIP);
#ifdef CIRCLE
		this->Buffer.allocate(bufferSize);
#endif
		for (int i = 0; i < bufferSize; i++) {
			this->vertex((i / static_cast<float>(bufferSize)) * 2.f - 1.f, 0);

#ifdef CIRCLE
			this->Buffer.insertValue(0.f);
#else
			buffer.push_back(0.f);
#endif
		}
	}

	void writeSample(float sample) {
#ifdef CIRCLE
		this->Buffer.insertValue(sample);
#else
		for (int i = 0; i < bufferSize - 1; i++) {
			buffer[i] = buffer[i + 1];
		}
		buffer[bufferSize - 1] = sample;
#endif

	}

	void update() {
#ifdef CIRCLE
		this->Buffer.resetReadHeadIndex(); //Catch up the read head to the right location
#endif
		for (int i = 0; i < bufferSize; i++) {

#ifdef CIRCLE
			this->vertices()[i][1] = this->Buffer.readNextValue();
#else
			this->vertices()[i][1] = buffer[i];
#endif
		}
	}


};

// app struct
struct Gimmel_Test : public App {
	Parameter volControl{ "volControl", "", 0.f, -96.f, 6.f };
	Parameter rmsMeter{ "rmsMeter", "", -96.f, -96.f, 0.f };

	//Compressor-related controls
	ParameterBool compressorBypass{ "compressorBypass", "", false, 0.f, 1.f };
	Parameter compressorThreshold{ "compressorThreshold", "", 0.f, -96.f, 0.f };
	Parameter compressorRatio{ "compressorRatio", "", 1.f, 1.f, 20.f };
	Parameter compressorAttackTime{ "compressorAttack", "", 10.f, 1.f, 50.f };
	Parameter compressorReleaseTime{ "compressorRelease", "", 50.f, 1.f, 200.f };


	ParameterBool audioOutput{ "audioOutput", "", false, 0.f, 1.f };

	Oscilloscope scope{ static_cast<int>(AudioIO().framesPerSecond()) };

	giml::Compressor<float> myComp{ static_cast<int>(AudioIO().framesPerSecond()) };

	void onInit() {
		// set up GUI
		auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
		auto& gui = GUIdomain->newGUI();
		gui.add(volControl); // add parameter to GUI

		gui.add(rmsMeter);

		//Compressor controls
		gui.add(compressorBypass);
		gui.add(compressorThreshold);
		gui.add(compressorRatio);
		gui.add(compressorAttackTime);
		gui.add(compressorReleaseTime);
	}

	void onCreate() {
		//Compressor Control Callbacks (asynchronously change the values without affecting latency)

		const std::function<void(float)> compressorBypassCallback = [&](bool a) { if (a) { myComp.enable(); } else { myComp.disable(); } };
		compressorBypass.registerChangeCallback(compressorBypassCallback);

		const std::function<void(float)> compressorThresholdCallback = [&](float a) { myComp.setThreshold(a); };
		compressorThreshold.registerChangeCallback(compressorThresholdCallback);
		const std::function<void(float)> compressorRatioCallback = [&](float a) { myComp.setRatio(a); };
		compressorRatio.registerChangeCallback(compressorRatioCallback);
		const std::function<void(float)> compressorAttackTimeCallback = [&](float a) { myComp.setAttackTime(a); };
		compressorAttackTime.registerChangeCallback(compressorAttackTimeCallback);
		const std::function<void(float)> compressorReleaseTimeCallback = [&](float a) { myComp.setReleaseTime(a); };
		compressorReleaseTime.registerChangeCallback(compressorReleaseTimeCallback);
	}

	void onAnimate(double dt) {
		//std::unique_lock<std::mutex> lock(scope.mtx);
		scope.update();
	}

	bool onKeyDown(const Keyboard& k) override {
		if (k.key() == 'm') { // <- on m, muteToggle
			audioOutput = !audioOutput;
			cout << "Mute Status: " << audioOutput << endl;
		}
		return true;
	}

	void onSound(AudioIOData& io) override {
		// variables reset for each call
		float bufferPower = 0; // for measuring output RMS
		float volFactor = dBtoA(volControl); // vol control

		auto maxFrames = io.framesPerBuffer();
		//std::unique_lock<std::mutex> lock(scope.mtx);
		float input, output;
		// sample loop. variables declared inside reset for each sample
		for (int i = 0; i < maxFrames; i++) {
			// capture input sample
			input = io.in(0, i);
			scope.writeSample(input);

			//// transform input for output (put your DSP here!)
			//float output = input;
			//if (audioOutput) {
			output = myComp.processSample(input);
			//}
			//// float output = g(f(input)) etc... 
			//float output = input * volFactor * audioOutput; 

			//// for each channel, write output to speaker
			//for (int channel = 0; channel < io.channelsOut(); channel++) {
			//	io.out(channel) = input;
			//}

			io.out(0, i) = output; //Left channel
			io.out(1, i) = -output; //Right channel

			// feed to Oscilloscope


			// feed to analysis buffer
			bufferPower += output;

			//// overload detector (assuming 2 output channels)
			//if (io.out(0, i) > 1.f || io.out(1, i) > 1.f) {
			//	cout << "CLIP!" << endl;
			//}
		}

		bufferPower /= maxFrames; // calculate bufferPower
		rmsMeter = ampTodB(bufferPower); // print to GUI display
	}

	void onDraw(Graphics& g) {
		g.clear(0);
		g.color(1);
		g.camera(Viewpoint::IDENTITY); // Ortho [-1:1] x [-1:1]
		g.draw(scope);
	}
};



//#include <random>

int main() {

	//static const int size = 5;
	//giml::CircularBufferForOscilloscope<float> Buffer2;
	//Buffer2.allocate(size);
	//float buffer2[size] = { 0.f };
	//for (int i = 0; i < size; i++) {
	//	Buffer2.insertValue(0.f);
	//}


	////Code to uniformly spawn random values between [-1.f, 1.f]
	//std::random_device rd;
	//std::mt19937 gen(rd());
	//std::uniform_real_distribution<float> distribution(-1.f, 1.f);
	//float rand;
	//for (int i = 0; i < 10000000000; i++) {
	//	rand = distribution(gen);

	//	for (int j = 0; j < size; j++) {
	//		buffer2[j] = buffer2[j + 1];
	//	}
	//	buffer2[size - 1] = rand;
	//	Buffer2.insertValue(rand);

	//} //Breakpoint here per iteration to see where new value inserted


	Gimmel_Test app; // instance of our app 

	// Allows for manual declaration of input and output devices, 
	// but causes unpredictable behavior. Needs investigation.
	app.audioIO().deviceIn(AudioDevice("Microphone")); // change for your device
	app.audioIO().deviceOut(AudioDevice("Speaker")); // change for your device
	cout << "outs: " << app.audioIO().channelsOutDevice() << endl;
	cout << "ins: " << app.audioIO().channelsInDevice() << endl;
	app.configureAudio(48000, 128, app.audioIO().channelsOutDevice(), app.audioIO().channelsInDevice());
	// ^ samplerate, buffersize, channels out, channels in

	app.start();
	return 0;
}