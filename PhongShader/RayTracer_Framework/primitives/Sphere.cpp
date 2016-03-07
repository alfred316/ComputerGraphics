#include "sphere.h"
#include "../Imageio/Imageio.h"
#include <math.h>

Intersect_Cond	Sphere::intersection_check(const M3DVector3f start, const M3DVector3f dir, float & distance, M3DVector3f intersection_p)
{
	M3DVector3f ray_to_center;
	m3dSubtractVectors3(ray_to_center,_pos,start);
	float ray_center_length = m3dDotProduct(ray_to_center, ray_to_center); 

	float closest_point = m3dDotProduct( ray_to_center, dir);
	if (closest_point < 0) 
	{
		return _k_miss;
	}

	float halfCord2 = (_rad * _rad) - (ray_center_length - (closest_point * closest_point));   
	if(halfCord2 < 0) 
	{
		return _k_miss;
	}

	Intersect_Cond type;
	M3DVector3f tmp;
	m3dSubtractVectors3(tmp,start,_pos);
	float length = m3dDotProduct(tmp,tmp);
	if (length < _rad2)
	{
		type = _k_inside;
		distance = closest_point + sqrt(halfCord2);
	}else
	{
		type = _k_hit;
		distance = closest_point - sqrt(halfCord2);
	}

	M3DVector3f tmp_v;
	m3dCopyVector3(tmp_v, dir);
	m3dScaleVector3(tmp_v,distance);
	m3dAddVectors3(intersection_p,start,tmp_v);

	return type;
}

//Alfred Shaker
//November 1st 2015
//Phong Shading

void	Sphere::shade(M3DVector3f view,M3DVector3f intersect_p,const Light & sp_light, M3DVector3f am_light, M3DVector3f color, bool shadow)
{
	//shininess coefficient 5-10 plastics 100-200 metals
	float shininess = 8;

	//calculate the normal
	M3DVector3f normal;
	m3dSubtractVectors3(normal, intersect_p, _pos);
	for (int q = 0; q < 3; q++)
	{
		normal[q] /= _rad;
	}
	
	//n
	m3dNormalizeVector(normal);

	//get the light properties (position and color)
	M3DVector3f light_dir, light_pos, light_color;
	sp_light.get_light(light_pos, light_color);

	//l. light direction
	m3dSubtractVectors3(light_dir, light_pos, intersect_p);
	m3dNormalizeVector(light_dir);

	//v. viewer direction
	M3DVector3f view_dir;
	m3dSubtractVectors3(view_dir, view, intersect_p);
	m3dNormalizeVector(view_dir);

	//h. halfway vector
	M3DVector3f HalfwayVector;
	m3dAddVectors3(HalfwayVector, view_dir, normal);
	m3dNormalizeVector(HalfwayVector);

	//diffuse coefficient. cosThetai
	float diffuse_coef = m3dDotProduct(light_dir, normal);

	//make sure it's within bounds of 0 and 1
	if (diffuse_coef < 0)
	{
		diffuse_coef = 0;
	}
	else if (diffuse_coef > 1)
	{
		diffuse_coef = 1;
	}
	
	//calculate perfect reflection
	// r = (2*(l*n)n)-l
	M3DVector3f tempVec;
	M3DVector3f tempVec2;
	m3dCopyVector3(tempVec2, normal);
	M3DVector3f perfectReflector;
	float tempFloat = m3dDotProduct(light_dir, normal);
	tempFloat *= 2;
	for (int j = 0; j < 3; j++)
	{
		tempVec2[j] *= tempFloat;
	}
	m3dSubtractVectors3(tempVec, tempVec2, light_dir);
	//r 
	m3dCopyVector3(perfectReflector, tempVec);
	m3dNormalizeVector(perfectReflector);

	//specular coefficient
	float temp1 = m3dDotProduct(HalfwayVector, normal);
	float sp_coeff = powf(temp1, shininess);

	//add all coefficeints up with the comonents needed to produce final color
	for (int i = 0; i < 3; i++)
	{
		//color[i] = (_color[i] + light_color[i]) * (diffuse_coef + sp_coeff + am_light[i]);
		color[i] = (_kd * _color[i] * light_color[i] * diffuse_coef) + (_ks * _color[i] * light_color[i] * sp_coeff ) + (_ka * _color[i] * am_light[i]);
		//color[i] = _color[i] + (_kd * light_color[i] * diffuse_coef) + (_ks * light_color[i] * sp_coeff) + (_ka  * am_light[i]);
		//color[i] =_kd * _color[i] * light_color[i]*diffuse_coef+_ks * _color[i] * light_color[i] * sp_coeff +_color[i] * _ka * am_light[i];
	}

	shadow = true;
}

//Alfred Shaker
//December 4th 2015
//Part 2
void	Sphere::get_reflect_direct(const M3DVector3f direct, const M3DVector3f intersect_p, M3DVector3f reflect_direct)
{
	//calculate the normal
	M3DVector3f normal;
	m3dSubtractVectors3(normal, intersect_p, _pos);
	for (int q = 0; q < 3; q++)
	{
		normal[q] /= _rad;
	}

	//n
	m3dNormalizeVector(normal);

	//R = 1 - 2(V . N)N

	float temp = m3dDotProduct(direct, normal);

	for (int i = 0; i < 3; ++i)
	{
		normal[i] = 2 * temp * normal[i];
	}

	m3dSubtractVectors3(reflect_direct, direct, normal);
	m3dNormalizeVector(reflect_direct);
}

bool	Sphere::get_refract_direct(const M3DVector3f direct, const M3DVector3f intersect_p, M3DVector3f refract_direct, float delta, bool is_in)
{
	//calculate the normal
	M3DVector3f normal;
	m3dSubtractVectors3(normal, intersect_p, _pos);
	for (int q = 0; q < 3; q++)
	{
		normal[q] /= _rad;
	}

	//n
	m3dNormalizeVector(normal);

	/*
	M3DVector3f tempNorm, vectorM;

	for (int i = 0; i < 3; i++)
	{
		tempNorm[i] = normal[i];
		tempNorm[i] *= cos(delta);
		tempNorm[i] /= sin(delta);
	}

	m3dSubtractVectors3(vectorM, tempNorm, direct);
	m3dNormalizeVector(vectorM);

	for (int j = 0; j < 3; j++)
	{
		vectorM[j] *= sin(delta);
		normal[j] *= cos(delta);
	}
	m3dSubtractVectors3(refract_direct, vectorM, normal);
	*/

	//Nr
	float refractiveIndex = 1 / 1.52;

	//temp variables to store our numbers
	M3DVector3f temp1, temp2, temp3;
	float tempf1, tempf2, tempf3;

	//solver for values in each part of the equation

	//sqrt(1-Nr^2(1-(N . V)^2))
	tempf1 = m3dDotProduct(normal, direct);
	tempf1 *= tempf1;
	tempf1 = 1 - tempf1;
	tempf2 = refractiveIndex * refractiveIndex;
	tempf3 = tempf1 * tempf2;
	tempf3 = 1 - tempf3;
	tempf3 = sqrtf(tempf3);

	//Nr(N . V)
	float tempf4, tempf5;
	tempf4 = refractiveIndex * m3dDotProduct(normal, direct);
	
	//Nr(N . V) - sqrt(1-Nr^2(1-(N . V)^2))
	tempf5 = -tempf4 - tempf3;

	//(Nr(N . V) - sqrt(1-Nr^2(1-(N . V)^2)))N + NrV
	for (int q = 0; q < 3; ++q)
	{
		temp1[q] = refractiveIndex * direct[q];
		temp2[q] = tempf5 * normal[q];
	}
	

	if (is_in == true)
	{
		m3dAddVectors3(refract_direct, temp1, temp2);
	}
	return true;
}