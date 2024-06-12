#ifndef OSCILLOSCOPE_HPP
#define OSCILLOSCOPE_HPP

#include "al/graphics/al_Mesh.hpp"
#include "../Gimmel/include/Utility.hpp"
class Oscilloscope : public al::Mesh {
public:
	Oscilloscope() = delete; //Remove default constructor because we do not want them instantiating it
	Oscilloscope(int sampleRate, float speed = 0.5f, float startAngle = M_PI_4) : bufferSize(sampleRate / speed), startAngle(startAngle) {
		this->buffer.allocate(this->bufferSize);
		this->dTheta = GIML_TWO_PI / (float)this->bufferSize; //increment as we wrap around the sphere
		this->primitive(al::Mesh::LINE_STRIP);
		for (int i = 0; i < bufferSize; i++) {
			// initialize vertices
			this->vertex(
				radius * cosf(i * this->dTheta - this->startAngle), // x
				0, // y
				radius * sinf(i * this->dTheta - this->startAngle) // z
			);
			
			this->color(al::RGB(1.f)); // color each vertex white
		}

		
	}

	void writeSample(float sample) {
		this->buffer.writeSample(sample);
	}
public:
	void update() {
		float theta, phi, currentY,
			cartesianX, cartesianY, cartesianZ;
		for (int i = 0; i < this->bufferSize; i++) { // loops through all y samples
			/*
			For each vertex, we need to set an x,y,z, coordinate for the allosphere

			We need to take into account radius & the height of the current y sample

			x = rsin(\phi)cos(@)
			y = rsin(\phi)sin(@)
			z = rcos(\phi)


			    z

				|
				|
				|
				|
				|______________ y
			   /
			  /
			 /
			/
			x

			alloZ = x;
			alloX = y;
			alloY = z;
			*/
			theta = i * this->dTheta - this->startAngle;
			currentY = this->buffer.readSample(this->bufferSize - i);
			phi = M_PI_2 - currentY / this->radius;

			cartesianX = this->radius * sinf(phi) * cosf(theta);
			cartesianY = this->radius * sinf(phi) * sinf(theta);
			cartesianZ = this->radius * cosf(phi);

			this->vertices()[i] = al::Vec3f(cartesianX, cartesianZ, cartesianY);

			 //this->vertices()[i][0] = cartesianX; //alloX
			 //this->vertices()[i][1] = cartesianZ; //alloY
			 //this->vertices()[i][2] = cartesianY; //alloZ
			 
			 /*this->vertices()[i][1] = currentY;*/
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
	float radius = 0.5f; //Radius of how big we want to draw it in the sphere
	float dTheta; //precalculated increment as we wrap around the sphere
	float startAngle; //offset from 0 radians on unit circle where new audio should come from and old audio should leave
};
#endif