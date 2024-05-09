// Joel A. Jaffe 2024-05-08
// AlloApp Demonstrating Tremolo

#include "al/app/al_App.hpp"
#include "al/graphics/al_Mesh.hpp"
#include "al/app/al_GUIDomain.hpp"
using namespace al;

#include <iostream>
using namespace std;

#include "../Gimmel/include/modulation/Tremolo.hpp"
#include "Gamma/SamplePlayer.h"

// Oscilliscope that inherits from mesh 
class Oscilliscope : public Mesh {
public:
  Oscilliscope (int samplerate) : bufferSize(samplerate) {
    this->primitive(Mesh::LINE_STRIP);
    for (int i = 0; i < bufferSize; i++) {
      this->vertex((i / static_cast<float>(bufferSize)) * 2.f - 1.f, 0);
    }
  }

  void writeSample (float sample) {
    buffer.writeSample(sample);
  }

  void update() {
    for (int i = 0; i < bufferSize; i++) {
      this->vertices()[i][1] = buffer.readSample(bufferSize - i);
    }
  }
    
protected:
  int bufferSize;
  giml::CircularBuffer buffer;
};

// app struct
struct Tremolo_Test : public App {
  Parameter volControl{"volControl", "", 0.f, -96.f, 6.f};
  Parameter rmsMeter{"rmsMeter", "", -96.f, -96.f, 0.f};
  ParameterBool audioOutput{"audioOutput", "", false, 0.f, 1.f};

  Parameter rate{"rate", "", 1.f, 0.1f, 1000.f};
  Parameter depth{"depth", "", 1.f, 0.f, 1.f};

  Oscilliscope scope{static_cast<int>(AudioIO().framesPerSecond())};
  giml::Tremolo<float> myTrem{static_cast<int>(AudioIO().framesPerSecond())};
  gam::SamplePlayer<float, gam::ipl::Linear, gam::phsInc::Loop> player;

  void onInit() {
    // set up GUI
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto &gui = GUIdomain->newGUI();
    gui.add(volControl); // add parameter to GUI
    gui.add(rmsMeter);

    gui.add(rate);
    gui.add(depth);

    //load file to player
    player.load("../../Resources/HuckFinn.wav");
  }

  void onCreate() {}

  void onAnimate(double dt) {
    myTrem.setDepth(depth);
    myTrem.setSpeed(rate);
    scope.update();
  }

  bool onKeyDown(const Keyboard &k) override {
    if (k.key() == 'm') { // <- on m, muteToggle
      audioOutput = !audioOutput;
      cout << "Mute Status: " << audioOutput << endl;
    }
    return true;
  }

  void onSound(AudioIOData& io) override {
    // variables reset for each call
    float bufferPower = 0; // for measuring output RMS
    float volFactor = giml::dBtoA(volControl); // vol control

    // sample loop. variables declared inside reset for each sample
    while(io()) { 
      // capture input sample
      float input = player(0) * volFactor * audioOutput;

      // transform input for output (put your DSP here!)
      float output = input;
      if (audioOutput) {
        output = myTrem.processSample(input);
      }
      // float output = g(f(input)) etc... 
      //float output = input * volFactor * audioOutput; 

      // for each channel, write output to speaker
      for (int channel = 0; channel < io.channelsOut(); channel++) {
        io.out(channel) = output; 
      }

      // feed to oscilliscope
      scope.writeSample(output);

      // feed to analysis buffer
      bufferPower += output;

      // overload detector (assuming 2 output channels)
      if (io.out(0) > 1.f || io.out(1) > 1.f) {
        cout << "CLIP!" << endl;
      }
    }

    bufferPower /= io.framesPerBuffer(); // calculate bufferPower
    rmsMeter = giml::aTodB(bufferPower); // print to GUI display
  }

  void onDraw(Graphics &g) {
    g.clear(0);
    g.color(1);
    g.camera(Viewpoint::IDENTITY); // Ortho [-1:1] x [-1:1]
    g.draw(scope);
  }
};
  
int main() {
  Tremolo_Test app; // instance of our app 
  
  // Allows for manual declaration of input and output devices, 
  // but causes unpredictable behavior. Needs investigation.
  app.audioIO().deviceIn(AudioDevice("MacBook Pro Microphone")); // change for your device
  app.audioIO().deviceOut(AudioDevice("BH+Speakers")); // change for your device
  cout << "outs: " << app.audioIO().channelsOutDevice() << endl;
  cout << "ins: " << app.audioIO().channelsInDevice() << endl;
  app.configureAudio(48000, 128, app.audioIO().channelsOutDevice(), app.audioIO().channelsInDevice());
  // ^ samplerate, buffersize, channels out, channels in
  //app.player.rate(1.0 / app.audioIO().channelsOutDevice());

  app.start();
  return 0;
}