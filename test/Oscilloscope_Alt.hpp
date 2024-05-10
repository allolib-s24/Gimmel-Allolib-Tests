#ifndef OSCILLOSCOPE_HPP
#define OSCILLOSCOPE_HPP

#include "al/graphics/al_Mesh.hpp"
#include "../Gimmel/include/utility.hpp"
// Oscilloscope that inherits from mesh 
class Oscilloscope : public al::Mesh {
protected:
  int bufferSize;
  giml::CircularBuffer<float> buffer;

public:
  Oscilloscope(int samplerate) : bufferSize(samplerate) {
    this->primitive(Mesh::LINE_STRIP);
    buffer.allocate(this->bufferSize);
    for (int i = 0; i < bufferSize; i++) {
      this->vertex((i / static_cast<float>(bufferSize)) * 2.f - 1.f, 0);
	  this->color(al::RGB(1.f));
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

  void setColorRGB255(int red, int green, int blue) {
	//Given numbers in the range of [0, 255], convert to floating points
	for (int i = 0; i < bufferSize; i++) {
		this->colors()[i] = al::RGB(red / 255.f, green / 255.f, blue / 255.f);
  	}
  }
};
#endif