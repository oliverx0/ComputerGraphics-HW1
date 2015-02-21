#include "scene.h"
#include "camera.h"
#include "object.h"

#include "primitive.h"
#include "model.h"

scenehdl::scenehdl()
{
	canvas = NULL;
	active_camera = -1;
	active_object = -1;
	render_normals = none;
	render_cameras = false;
}

scenehdl::~scenehdl()
{

}

/* draw
 *
 * Update the locations of all of the lights, draw all of the objects, and
 * if enabled, draw the normals, the lights, the cameras, etc.
 */
void scenehdl::draw()
{ 
	/* TODO Assignment 1: Draw all of the objects, and
	 * if enabled, draw the normals and the cameras.
	 */
    
    vec8f geometry;
    geometry[0] = 0;
    geometry[1] = 0;
    geometry[2] = 0;
    
    vec8f geometry2(0,0.05,1,1,1,1,1,1);
    geometry2[0] = 1;
    geometry2[1] = 1;
    geometry2[2] = 0;
    //cout << geometry;
    
    vector<vec8f>vec;
    vector<int>vec2;
    vec.push_back(geometry);
    vec.push_back(geometry2);
    
    canvas->draw_points(vec);
    canvas->draw_lines(vec,vec2);
    
    
	/* TODO Assignment 2: Pass the lights to the shaders through canvashdl::uniform.
	 * If enabled, draw the lights.
	 */
}

void scenehdl::create_camera(camera_type cam_type)
{
    if(cam_type == ortho)
    {
        orthohdl camera;
        camera.view(canvas);
        cameras.push_back(&camera);
    }
    else if(cam_type == perspective)
    {
        perspectivehdl camera;
        camera.view(canvas);
        cameras.push_back(&camera);
    }
    else if(cam_type == frustum)
    {
        frustumhdl camera;
        camera.view(canvas);
        cameras.push_back(&camera);
    }
    
    active_camera++;
}

bool scenehdl::active_camera_valid()
{
	return (active_camera >= 0 && active_camera < cameras.size() && cameras[active_camera] != NULL);
}

bool scenehdl::active_object_valid()
{
	return (active_object >= 0 && active_object < objects.size() && objects[active_object] != NULL);
}
