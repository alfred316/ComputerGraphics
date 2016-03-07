#include "Wall.h"
#include <math.h>
#include "../Imageio/Imageio.h"

Intersect_Cond Wall::intersection_check(const M3DVector3f start, const M3DVector3f dir, float &distance, M3DVector3f intersection_p)
{
	if (_tr1.intersection_check(start,dir,distance,intersection_p) == _k_hit)
	{
		return _k_hit;
	}else
	{
		return _tr2.intersection_check(start,dir,distance,intersection_p);
	}
}

//Alfred Shaker
//November 1st 2015
//Phong Shading

void	Wall::shade(M3DVector3f view, M3DVector3f intersect_p,const Light & sp_light, M3DVector3f am_light, M3DVector3f color,bool shadow)
{
	//shininess coefficient 5-10 plastics 100-200 metals
	float shininess = 120;

	//get properties of one of the triangles used to make a wall
	M3DVector3f top_left, top_right, bottom_left;
	_tr1.get_vertex(top_left, top_right, bottom_left);

	M3DVector3f normal;
	
	//get the light properties (position and color)
	M3DVector3f light_dir, light_pos, light_color;
	sp_light.get_light(light_pos, light_color);

	//get light direction
	m3dSubtractVectors3(light_dir, light_pos, intersect_p);
	//l. light direction
	m3dNormalizeVector(light_dir);

	//get view direction
	M3DVector3f view_dir;
	m3dSubtractVectors3(view_dir, intersect_p, view);
	//v. view direction
	m3dNormalizeVector(view_dir);

	

	//check which triangle is being interacted with to produce the correct normal vector
	float distanceLightPoint;
	if (intersection_check(light_pos, light_dir, distanceLightPoint, intersect_p) == _k_hit)
	{
		_tr1.normal(normal);
		m3dNormalizeVector(normal);
	}
	else
	{
		_tr2.normal(normal);
		m3dNormalizeVector(normal);
	}

	//h. halfway vector
	M3DVector3f HalfwayVector;
	m3dAddVectors3(HalfwayVector, view_dir, normal);
	m3dNormalizeVector(HalfwayVector);

	//diffuse. cosThetai
	float diffuse_coef = m3dDotProduct(light_dir, normal);

	//make sure diffuse coefficient is between 0 and 1
	if (diffuse_coef < 0)
	{
		diffuse_coef = 0;
	}
	else if (diffuse_coef > 1)
	{
		diffuse_coef = 1;
	}

	//get the perfect reflection
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
	//r. perfect reflection
	m3dCopyVector3(perfectReflector, tempVec);
	m3dNormalizeVector(perfectReflector);
	

	//specular coefficient. 
	float temp1 = m3dDotProduct(HalfwayVector, normal);
	float sp_coeff = powf(temp1, shininess);

	//add them all together with the right components to get the final color
	for (int i = 0; i < 3; i++)
	{
		//color[i] = _color[i];
		//color[i] = _color[i] + light_color[i] * (diffuse_coef + sp_coeff + am_light[i]);
		color[i] = (_kd * _color[i] * light_color[i] * diffuse_coef) + (_ks * light_color[i] * sp_coeff) + (_ka * _color[i] * am_light[i]);
		//color[i] = _color[i] + (_kd * light_color[i] * diffuse_coef) + (_ks * light_color[i] * sp_coeff) + (_ka  * am_light[i]);
		//color[i] = _color[i] * light_color[i] * diffuse_coef + _color[i]  * light_color[i] * sp_coeff + _color[i]+am_light[i];
	}

	shadow = true;
}


void	Wall::get_reflect_direct(const M3DVector3f direct,const M3DVector3f intersect_p, M3DVector3f reflect_direct)
{
	
}


void Wall::load_texture(std::string file_name)
{
	
}

void Wall::texture_color(M3DVector3f pos, M3DVector3f color)
{
}

void	Wall::get_texel(float x, float y, M3DVector3f color)
{
	return;
}