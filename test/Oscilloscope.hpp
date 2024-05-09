#ifndef OSCILLOSCOPE_HPP
#define OSCILLOSCOPE_HPP

#include "al/graphics/al_Mesh.hpp"
#include "../Gimmel/include/utility.hpp"
class Oscilloscope : public al::Mesh {
public:
	Oscilloscope() = delete; //Remove default constructor because we do not want them instantiating it
	Oscilloscope(int samplerate);

	void writeSample(float sample);
	void update();
	void setColorRGB255(int r, int g, int b);
	//Allow these to be public so that they can set the color to whatever they want
	float r = 1.f, g = 1.f, b = 1.f; //Color of what we want this oscilloscope to be

private:
	int bufferSize;
	//giml::CircularBufferForOscilloscope<float> buffer;
	giml::CircularBuffer<float> buffer;
};

Oscilloscope::Oscilloscope(int sampleRate) : bufferSize(sampleRate) {
	this->buffer.allocate(sampleRate);
	this->primitive(al::Mesh::LINE_STRIP);
	for (int i = 0; i < bufferSize; i++) {
		this->vertex((i / static_cast<float>(this->bufferSize)) * 2.f - 1.f, 0);
	}
}

void Oscilloscope::writeSample(float sample) {
	//this->buffer.insertValue(sample);
	this->buffer.writeSample(sample);
}
#include <chrono>
static long long timeElapsed = 0L;
static long long iterations = 0L;

//#define BENCHMARK_CODE(code) { \
//        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now(); \
//        code \
//        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now(); \
//        timeElapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count(); \
//		std::cout << timeElapsed << "ns" << std::endl; \
//	}

#define BENCHMARK_CODE(code) { \
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now(); \
        code \
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now(); \
        timeElapsed += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count(); \
        iterations++; \
        if (iterations > 48000 * 3) { \
			std::cout << timeElapsed / iterations << "ns avg every 3 sec" << std::endl; \
			timeElapsed = iterations = 0; \
		} \
   }


void Oscilloscope::update() {
	//this->buffer.resetReadHeadIndex();
	for (int i = 0; i < this->bufferSize; i++) {
		//this->vertices()[i][1] = this->buffer.readNextValue();
		this->vertices()[i][1] = this->buffer.readSample(bufferSize - i);
	}
	//this->buffer.resetReadHeadIndex();
	//for (int i = 0; i < this->bufferSize; i++) {
	//	BENCHMARK_CODE(this->buffer.readSample(bufferSize - i);)
	//	//this->vertices()[i][1] = this->buffer.readSample(bufferSize - i);
	//}
}

void Oscilloscope::setColorRGB255(int red, int green, int blue) {
	//Given numbers in the range of [0, 255], convert to floating points
	this->r = red / 255.f;
	this->g = green / 255.f;
	this->b = blue / 255.f;
}

#endif