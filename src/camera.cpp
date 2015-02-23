#include "camera.h"
#include "object.h"

camerahdl::camerahdl()
{
	position = vec3f(0.0, 0.0, -10.0);
	orientation = vec3f(0.0, 0.0, 0.0);
	model = NULL;
	type = "camera";
	focus = NULL;
	radius = 10.0f;
}

camerahdl::~camerahdl()
{

}

void camerahdl::view(canvashdl *canvas)
{
	/* TODO Assignment 1: Do the necessary modelview transformations to move
	 * the camera into place.
	 */
    
    //Restart Canvas ModelView
    canvas->active_matrix = canvashdl::modelview_matrix;
    canvas->load_identity();
    
    //Apply translation
    canvas->translate(-position);
    
    //Apply rotation
    vec3f x;
    x[0] = 1;
    x[1] = 0;
    x[2] = 0;
    
    vec3f y;
    y[0] = 0;
    y[1] = 1;
    y[2] = 0;
    
    vec3f z;
    z[0] = 0;
    z[1] = 0;
    z[2] = 1;
    
    canvas->rotate(-orientation[0], x);
    canvas->rotate(-orientation[1], y);
    canvas->rotate(-orientation[2], z);
    
}

orthohdl::orthohdl()
{
	left = -10.0;
	right = 10.0;
	bottom = -10.0;
	top = 10.0;
	front = 2.0;
	back = 101.0;
	type = "ortho";
}

orthohdl::~orthohdl()
{
}

void orthohdl::project(canvashdl *canvas)
{
	// TODO Assignment 1: Use the canvashdl::ortho function to set up an orthographic projection
    
    //Restart Canvas Projection
    canvas->active_matrix = canvashdl::projection_matrix;
    canvas->load_identity();
    canvas->ortho(left, right, bottom, top, front, back);
}

frustumhdl::frustumhdl()
{
	left = -1.0;
	right = 1.0;
	bottom = -1.0;
	top = 1.0;
	front = 2.0;
	back = 101.0;
	type = "frustum";
}

frustumhdl::~frustumhdl()
{

}

void frustumhdl::project(canvashdl *canvas)
{
	// TODO Assignment 1: Use the canvashdl::frustum function to set up a perspective projection
    //Restart Canvas Projection
    canvas->active_matrix = canvashdl::projection_matrix;
    canvas->load_identity();
    canvas->frustum(left, right, bottom, top, front, back);
}

perspectivehdl::perspectivehdl()
{
	fovy = m_pi/4.0;
	aspect = 1.0;
	front = 2.0;
	back = 101.0;
	type = "perspective";
}

perspectivehdl::~perspectivehdl()
{

}

void perspectivehdl::project(canvashdl *canvas)
{
	// TODO Assignment 1: Use the canvashdl::perspective function to set up a perspective projection
    
    //Restart Canvas Projection
    canvas->active_matrix = canvashdl::projection_matrix;
    canvas->load_identity();
    canvas->perspective(fovy, aspect, front, back);
}
