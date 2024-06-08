#ifndef OSCILLOSCOPE_HPP
#define OSCILLOSCOPE_HPP

#include "al/graphics/al_Mesh.hpp"
#include "../Gimmel/include/Utility.hpp"
class Oscilloscope : public al::Mesh {
public:
	Oscilloscope() = delete; //Remove default constructor because we do not want them instantiating it
	Oscilloscope(int sampleRate, float speed = 1.f) : bufferSize(sampleRate / speed) {
		this->buffer.allocate(this->bufferSize);
		this->primitive(al::Mesh::LINE_STRIP);
		for (int i = 0; i < bufferSize; i++) {
			// initialize vertices
			this->vertex(
				radius * cosf(M_PI * (i / static_cast<float>(this->bufferSize)) * 2.f - 1.f), // x
				0, // y
				radius * sinf(M_PI * (i / static_cast<float>(this->bufferSize)) * 2.f - 1.f) // z
			);
			
			this->color(al::RGB(1.f)); // color each vertex white
		}
	}

	void writeSample(float sample) {
		this->buffer.writeSample(sample);
	}
	void update() {
		for (int i = 0; i < this->bufferSize; i++) { // loops through all y samples
			/*
			For each vertex, we need to set an x,y,z, coordinate for the allosphere

			We need to take into account radius & the height of the current y sample

			x = rsin(\phi)cos(@)
			y = rsin(\phi)sin(@)
			z = rcos(\phi)

			\phi = y/(rsin(@))

			*/
			// float currentY = this->buffer.readSample((size_t)(this->bufferSize - i));
			// float theta = M_PI * (i / static_cast<float>(this->bufferSize)) * 2.f - 1.f;
			// float phi = M_PI_4 - currentY / this->radius;

			// this->vertices()[i][0] = this->radius * sinf(phi) * cosf(theta);
			// this->vertices()[i][1] = this->radius * cosf(phi);
			// this->vertices()[i][2] = this->radius * sinf(phi) * sinf(theta);
			float currentY = this->buffer.readSample((size_t)(this->bufferSize - i));
			this->vertices()[i][1] = currentY;

			// spring stuff 
			float euclidianDistanceFromOrigin = sqrt(powf(position[i][0], 2) + powf(position[i][1], 2) + powf(position[i][2], 2));
        	// for each vertex, Calculate the spring force
        	float springForceMagnitude = springConstant * (euclidianDistanceFromOrigin - this->radius);
        	// for each dimension, Calculate acceleration
        	for (int dim = 0; dim < 3; dim++) {
          		float direction = -position[i][dim] / euclidianDistanceFromOrigin;
          		acceleration[i][dim] += springForceMagnitude * direction;
        	}

			// "semi-implicit" Euler integration
      		velocity[i] += acceleration[i] / mass[i] * dt;
      		position[i] += velocity[i] * dt;
			
		}
	}
	void setColorRGB255(int red, int green, int blue) {
		//Given numbers in the range of [0, 255], convert to floating points
		for (int i = 0; i < this->bufferSize; i++) {
			this->colors()[i] = al::RGB(red / 255.f, green / 255.f, blue / 255.f);
		}
	}
	void setRadius (float r) {
		this->radius = r;
	}

private:
	int bufferSize;
	giml::CircularBuffer<float> buffer;
	float radius = 1.f;
	float springConstant = 0.4;
};
#endif