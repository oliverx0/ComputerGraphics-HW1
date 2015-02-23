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
    
    //Set the active camera
    if(active_camera_valid())
    {
       cameras[active_camera]->view(canvas);
       cameras[active_camera]->project(canvas);
    }
    
    if(render_cameras)
    {
        //Draw all the cameras
        for(int i = 0; i < cameras.size(); ++i)
        {
           if(cameras[i]->model != NULL)
               cameras[i]->model->draw(canvas);
           else
               cout << "Camera model is null!" << endl;
        }
    }
    
    //Draw all of the objects of the scene in the current camera
    for(int i = 0; i < objects.size(); ++i)
    {
        objects[i]->draw(canvas);
    }

	/* TODO Assignment 2: Pass the lights to the shaders through canvashdl::uniform.
	 * If enabled, draw the lights.
	 */
}

void scenehdl::create_camera(camera_type cam_type)
{
    if(cam_type == ortho)
    {
        camerahdl *camera = new orthohdl;
        //camera.view(canvas);
        cameras.push_back(camera);
    }
    else if(cam_type == perspective)
    {
        camerahdl *camera = new perspectivehdl;
        //camera.view(canvas);
        cameras.push_back(camera);
    }
    else if(cam_type == frustum)
    {
        camerahdl *camera = new frustumhdl;
        //camera.view(canvas);
        cameras.push_back(camera);
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
