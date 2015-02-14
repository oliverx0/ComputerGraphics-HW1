#include "canvas.h"

#ifndef scene_h
#define scene_h

struct objecthdl;
struct camerahdl;

struct scenehdl
{
	scenehdl();
	~scenehdl();

	vector<objecthdl*> objects;
	vector<camerahdl*> cameras;
	canvashdl *canvas;

	int active_camera;
	int active_object;

	// Whether and how to render the normals
	enum
	{
		none = 0,
		face = 1,
		vertex = 2
	} render_normals;
    
    // Camera types
    enum camera_type
    {
        ortho = 0,
        frustum = 1,
        perspective = 2
        
    };

	// Whether or not to render the lights or the cameras or both
	bool render_cameras;

	void draw();

	bool active_camera_valid();
	bool active_object_valid();

	void create_camera(camera_type cam);

};

#endif
