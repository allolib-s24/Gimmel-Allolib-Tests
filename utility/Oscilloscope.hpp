#ifndef OSCILLOSCOPE_HPP
#define OSCILLOSCOPE_HPP

#include "al/graphics/al_Mesh.hpp"
#include "../Gimmel/include/utility.hpp"
class Oscilloscope : public al::Mesh {
public:
	Oscilloscope() = delete; //Remove default constructor because we do not want them instantiating it
	Oscilloscope(int sampleRate) : bufferSize(sampleRate) {
		this->buffer.allocate(sampleRate);
		this->primitive(al::Mesh::LINE_STRIP);
		for (int i = 0; i < bufferSize; i++) {
			this->vertex((i / static_cast<float>(this->bufferSize)) * 2.f - 1.f, 0);
			this->color(al::RGB(1.f));
		}
	}

	void writeSample(float sample) {
		this->buffer.writeSample(sample);
	}
	void update() {
		for (int i = 0; i < this->bufferSize; i++) {
			this->vertices()[i][1] = this->buffer.readSample(this->bufferSize - i);
		}
	}
	void setColorRGB255(int red, int green, int blue) {
		//Given numbers in the range of [0, 255], convert to floating points
		for (int i = 0; i < this->bufferSize; i++) {
			this->colors()[i] = al::RGB(red / 255.f, green / 255.f, blue / 255.f);
		}
	}

private:
	int bufferSize;
	giml::CircularBuffer<float> buffer;
};

#endif