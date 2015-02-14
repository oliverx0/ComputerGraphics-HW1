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
    
    vec8f geometry(1,1,1,1,1,1,1,1);
    vector<vec8f>vec;
    vec.push_back(geometry);
    canvas->draw_points(vec);
    
    //Draw objects
    for(int i = 0; i < objects.size(); i++)
    {
        objects[i]->draw(canvas);
    }
    
    //Draw cameras
    if(render_cameras)
    {
        for(int i = 0; i < cameras.size(); i++)
        {
            cameras[i]->model->draw(canvas);
        }
    }
    
    
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
