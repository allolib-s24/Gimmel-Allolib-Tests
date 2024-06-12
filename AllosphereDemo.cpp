// This implementation is optimized for use in UCSB's Allosphere

// TODO:
// -tune background color for sphere (hide projector borders)

#include "Utility/AllosphereTemplate.hpp"

int main() {
  std::string deviceIn = "";
    std::string deviceOut = "";
  if (al::Socket::hostName() == "ar01.1g") { // if in AlloSphere...
    deviceIn = "ECHO X5";
    deviceOut = "ECHO X5";
  } 
  else { // if not... 
    deviceIn = "MacBook Pro Microphone";
    deviceOut = "Headphones";
  }
  SphereTemplate app(44100, 128, deviceIn, deviceOut);
  app.start();
}