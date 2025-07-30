#pragma once
#pragma once

#include <stdio.h>
#include <glm/glm.hpp>
#include <corecrt_math_defines.h>

struct SCamera
{
	enum Camera_Movement
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;

	glm::vec3 WorldUp;

	float Yaw;
	float Pitch;

	const float MovementSpeed = 5.5f;
	float MouseSensitivity = 1.f;



};

float DEG2RAD(float x) {
	x = x * (M_PI / 180.f);

	return x;
}


void InitCamera(SCamera& in)
{
	in.Front = glm::vec3(0.0f, 0.0f, -1.0f);
	in.Position = glm::vec3(0.0f, 0.0f, 0.0f);
	in.Up = glm::vec3(0.0f, 1.0f, 0.0f);
	in.WorldUp = in.Up;
	in.Right = glm::normalize(glm::cross(in.Front, in.WorldUp));

	in.Yaw = -90.f;
	in.Pitch = 0.f;
}

float cam_dist = 2.f;

void MoveAndOrientCamera(SCamera& in, glm::vec3 target, float distance, float xoffset, float yoffset)
{


	in.Yaw -= xoffset * in.MovementSpeed;
	in.Pitch -= yoffset * in.MovementSpeed;

	float y = DEG2RAD(in.Yaw);
	float p = DEG2RAD(in.Pitch);


	if (in.Pitch > 89.0f) in.Pitch =  89.0f;
	if (in.Pitch < -89.0f)  in.Pitch = -89.0f;
		
	


	in.Position = glm::vec3(cos(y) * cos(p) , sin(p) , sin(y) * cos(p) ) * cam_dist ;
	in.Right = glm::normalize(glm::cross(in.Front, in.WorldUp));
	in.Up = glm::normalize(glm::cross(in.Right, in.Front));
	in.Front = glm::normalize(target - in.Position);


}