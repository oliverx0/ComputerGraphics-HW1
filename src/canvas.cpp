#include "canvas.h"
#include "core/geometry.h"
#include "core/color.h"
#include "camera.h"

canvashdl::canvashdl(int w, int h)
{
    width = w;
    height = h;
    last_reshape_time = -1.0;
    reshape_width = w;
    reshape_height = h;
    initialized = false;
    
    color_buffer = new unsigned char[width*height*3];
    // TODO Assignment 2: Initialize the depth buffer
    
    screen_texture = 0;
    screen_geometry = 0;
    screen_shader = 0;
    
    active_matrix = modelview_matrix;
    
    for (int i = 0; i < 3; i++)
        matrices[i] = identity<float, 4, 4>();
    
    polygon_mode = line;
    culling = backface;
    
    cout << "Canvas Initialized.";
}

canvashdl::~canvashdl()
{
    if (color_buffer != NULL)
    {
        delete [] color_buffer;
        color_buffer = NULL;
    }
    
    // TODO Assignment 2: Clean up the depth buffer
}

void canvashdl::clear_color_buffer()
{
    memset(color_buffer, 0, width*height*3*sizeof(unsigned char));
}

void canvashdl::clear_depth_buffer()
{
    // TODO Assignment 2: Clear the depth buffer
}

void canvashdl::resize(int w, int h)
{
    // TODO Assignment 2: resize the depth buffer
    
    last_reshape_time = -1.0;
    
    if (color_buffer != NULL)
    {
        delete [] color_buffer;
        color_buffer = NULL;
    }
    
    width = w;
    height = h;
    
    color_buffer = new unsigned char[w*h*3];
    
    glActiveTexture(GL_TEXTURE0);
    check_error(__FILE__, __LINE__);
    glBindTexture(GL_TEXTURE_2D, screen_texture);
    check_error(__FILE__, __LINE__);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, color_buffer);
    check_error(__FILE__, __LINE__);
}

/* set_matrix
 *
 * Change which matrix is active.
 */
void canvashdl::set_matrix(matrix_id matid)
{
    // TODO Assignment 1: Change which matrix is active.
    active_matrix = matid;
}

/* load_identity
 *
 * Set the active matrix to the identity matrix.
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/glLoadIdentity.xml
 */
void canvashdl::load_identity()
{
    // TODO Assignment 1: Set the active matrix to the identity matrix.
    matrices[active_matrix] = identity<float, 4, 4>();
}

/* rotate
 *
 * Multiply the active matrix by a rotation matrix.
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/glRotate.xml
 */
void canvashdl::rotate(float angle, vec3f axis)
{
    // TODO Assignment 1: Multiply the active matrix by a rotation matrix.
    if (mag(axis) != 1)
        axis = norm(axis);
    
    mat4f rotation_matrix = identity<float, 4, 4>();
    float sin_val = sin(degtorad(angle));
    float cos_val = cos(degtorad(angle));
    float a = axis[0];
    float b = axis[1];
    float c = axis[2];
    
    rotation_matrix[0][0] = a*a*(1-cos_val)+cos_val;
    rotation_matrix[0][1] = a*b*(1-cos_val)-c*sin_val;
    rotation_matrix[0][2] = a*c*(1-cos_val)+b*sin_val;
    rotation_matrix[1][0] = a*b*(1-cos_val)+c*sin_val;
    rotation_matrix[1][1] = b*b*(1-cos_val)+cos_val;
    rotation_matrix[1][2] = c*b*(1-cos_val)-a*sin_val;
    rotation_matrix[2][0] = a*c*(1-cos_val)-b*sin_val;
    rotation_matrix[2][1] = c*b*(1-cos_val)+a*sin_val;
    rotation_matrix[2][2] = c*c*(1-cos_val)+cos_val;
    
    matrices[active_matrix] = rotation_matrix*matrices[active_matrix];
}

/* translate
 *
 * Multiply the active matrix by a translation matrix.
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/glTranslate.xml
 */
void canvashdl::translate(vec3f direction)
{
    // TODO Assignment 1: Multiply the active matrix by a translation matrix.
    
    mat4f translation_matrix = identity<float, 4, 4>();
    vec4f direction2 = homogenize(direction);
    translation_matrix.set_col(3, direction2);
    matrices[active_matrix] = translation_matrix*matrices[active_matrix];
}

/* scale
 *
 * Multiply the active matrix by a scaling matrix.
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/glScale.xml
 */
void canvashdl::scale(vec3f size)
{
    // TODO Assignment 1: Multiply the active matrix by a scaling matrix.
    mat4f scaling_matrix = identity<float, 4, 4>();
    for (int i = 0; i < 3; ++i)
    {
        scaling_matrix[i][i] = size[i];
    }
    matrices[active_matrix] = scaling_matrix*matrices[active_matrix];
    
}

/* perspective
 *
 * Multiply the active matrix by a perspective projection matrix.
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/gluPerspective.xml
 */
void canvashdl::perspective(float fovy, float aspect, float nearVal, float farVal)
{
    // TODO Assignment 1: Multiply the active matrix by a perspective projection matrix.
    
    mat4f perspective_matrix = identity<float, 4, 4>();
   // float f = 1/tan(degtorad(fovy/2));
    float f = 1/tan(fovy/2);
    
    perspective_matrix[0][0] = f/aspect;
    perspective_matrix[1][1] = f;
    perspective_matrix[2][2] = (nearVal+farVal)/(nearVal-farVal);
    perspective_matrix[2][3] = (2*nearVal*farVal)/(nearVal-farVal);
    perspective_matrix[3][2] = -1;
    perspective_matrix[3][3] = 0;
    matrices[active_matrix] = perspective_matrix*matrices[active_matrix];
    
}

/* frustum
 *
 * Multiply the active matrix by a frustum projection matrix.
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/glFrustum.xml
 */
void canvashdl::frustum(float left, float right, float bottom, float top, float nearVal, float farVal)
{
    // TODO Assignment 1: Multiply the active matrix by a frustum projection matrix.
    mat4f frustum_matrix = identity<float, 4, 4>();
    
    frustum_matrix[0][0] = (2*nearVal)/(right-left);
    frustum_matrix[0][2] = (right+left)/(right-left);
    frustum_matrix[1][1] = (2*nearVal)/(top-bottom);
    frustum_matrix[1][2] = (top+bottom)/(top-bottom);
    frustum_matrix[2][2] = -(farVal+nearVal)/(farVal-nearVal);
    frustum_matrix[2][3] = -(2*farVal*nearVal)/(farVal-nearVal);
    frustum_matrix[3][2] = -1;
    frustum_matrix[3][3] = 0;
    matrices[active_matrix] = frustum_matrix*matrices[active_matrix];
}

/* ortho
 *
 * Multiply the active matrix by an orthographic projection matrix.
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/glOrtho.xml
 */
void canvashdl::ortho(float left, float right, float bottom, float top, float nearVal, float farVal)
{
    // TODO Assignment 1: Multiply the active matrix by an orthographic projection matrix.
    
    mat4f ortho_matrix = identity<float, 4, 4>();
    
    ortho_matrix[0][0] = 2/(right-left);
    ortho_matrix[1][1] = 2/(top-bottom);
    ortho_matrix[2][2] = -2/(farVal-nearVal);
    ortho_matrix[0][3] = -(right+left)/(right-left);
    ortho_matrix[1][3] = -(top+bottom)/(top-bottom);
    ortho_matrix[2][3] = -(farVal+nearVal)/(farVal-nearVal);
    matrices[active_matrix] = ortho_matrix*matrices[active_matrix];
    
}

/* look_at
 *
 * Move and orient the modelview so the camera is at the 'at' position focused on the 'eye'
 * position and rotated so the 'up' vector is up
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/gluLookAt.xml
 */
void canvashdl::look_at(vec3f eye, vec3f at, vec3f up)
{
    // TODO Assignment 1: Emulate the functionality of gluLookAt
    
    //Matrix M
    mat4f M = identity<float, 4, 4>();
    
    //Line of sight vector
    vec3f los = eye - at;
    
    //Normalize line of sight and up vectors
    los = norm(los);
    up = norm(up);
    
    //Calculate vector perpendicular to line of sight and up
    vec3f s = cross(los, up);
    s = norm(s);
    
    //Recompute up vector, NOT SURE WHY!
    up = cross(los, up);
    
    //Calculate M matrix
    M[0][0] = s[0];
    M[0][1] = s[1];
    M[0][2] = s[2];
    M[1][0] = up[0];
    M[1][1] = up[1];
    M[1][2] = up[2];
    M[2][0] = -los[0];
    M[2][1] = -los[1];
    M[2][2] = -los[2];
    
    matrices[active_matrix] = M*matrices[active_matrix];
    
    //Translate the camera to -at
    translate(-at);
    
}

/* change_scale
 *
 * Changes the scale of a given value.
 */
float canvashdl::change_scale(float current_val, float current_min, float current_max, float final_min, float final_max)
{
    return (((final_max-final_min)*(current_val-current_min))/(current_max-current_min))+final_min;
}

/* to_window
 *
 * Pixel coordinates to window coordinates.
 */
vec3f canvashdl::to_window(vec2i pixel)
{
    /* TODO Assignment 1: Given a pixel coordinate (x from 0 to width and y from 0 to height),
     * convert it into window coordinates (x from -1 to 1 and y from -1 to 1).
     */
    float x = change_scale(pixel[0], 0, width, -1, 1);
    float y = change_scale(pixel[1], 0, height, -1, 1);
    vec3f result(x,y);
    result[2] = 1;
    return result;
}

/* to_pixel
 *
 * Window coordinates to pixel coordinates.
 */
vec2i canvashdl::to_pixel(vec3f window)
{ 
    /* TODO Assignment 1: Given a pixel coordinate (x from 0 to width and y from 0 to height),
     * convert it into window coordinates (x from -1 to 1 and y from -1 to 1).
     */
    int x = change_scale(window[0], -1, 1, 0, width);
    int y = change_scale(window[1], -1, 1, 0, height);
    cout << "(" << window[0] << "," << window[0] << ") to " << "(" << x << "," << y << ")" << endl;
    vec2i result(x,y);
    return result;
}



/* unproject
 *
 * Unproject a window coordinate into world coordinates.
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/gluUnProject.xml
 */
vec3f canvashdl::unproject(vec3f window)
{
    // TODO Assignment 1: Unproject a window coordinate into world coordinates.
    
    vec4f window_coords = homogenize(window);
    window_coords[0] = (2*(window[0]-reshape_width)/width)-1;
    window_coords[1] = (2*(window[1]-reshape_height)/width)+1;
    window_coords[3] = 2*window[2] - 1;
    
    mat4f M = matrices[projection_matrix]*matrices[modelview_matrix];
    M = inverse(M);
    window_coords = M*window_coords;
    
    vec3f final_result(window_coords[0], window_coords[1], window_coords[3]);
    
    return final_result;
}

/* shade_vertex
 *
 * This is the vertex shader.
 */
vec8f canvashdl::shade_vertex(vec8f v)
{
    // TODO Assignment 1: Do all of the necessary transformations (normal, projection, modelview, etc)
    
    // v8f = x,y,z, normal_z, normal_y, normal_z, u, v -> we split it into 4 x,y,z,w and apply the transforms, we dont care about the rest for now. (we keep them the same)
    
    vec4f my_vector(v[0], v[1], v[2]);
    my_vector[3] = 1;
    mat4f M = matrices[normal_matrix]*matrices[projection_matrix]*matrices[modelview_matrix];
    my_vector = M*my_vector;
    
    v[0] = my_vector[0];
    v[1] = my_vector[1];
    v[2] = my_vector[2];
    
    // TODO Assignment 2: Implement Flat and Gouraud shading.
    return v;
}

/* shade_fragment
 *
 * This is the fragment shader. The pixel color is determined here.
 */
vec3f canvashdl::shade_fragment(vec8f v)
{
    // TODO Assignment 1: Pick a color, any color (as long as it is distinguishable from the background color).
    
    return white;
    
    /* TODO Assignment 2: Figure out the pixel color due to lighting and materials
     * and implement phong shading.
     */
    
}

/* plot
 *
 * Plot a pixel given in screen coordinates.
 */
void canvashdl::plot(vec2i xy, vec8f v)
{
    // TODO Assignment 1: Plot a pixel, calling the fragment shader.
    vec3f color = shade_fragment(v);
    int x = xy[0];
    int y = xy[1];
    
    if ((x >= 0 && x < width) && (y >= 0 && y < height))
    {
        for (int i = 0; i < 3; ++i)
        {
        
            color_buffer[3*(width*y+x)+i]=color[i];
        }
    }
    else
    {
        cout << "Error! Impossible value assigned to color buffer" << endl;
        
    }
    
    // TODO Assignment 2: Check the pixel depth against the depth buffer.
}

/* plot_point
 *
 * Plot a point given in window coordinates.
 */
void canvashdl::plot_point(vec8f v)
{
    // TODO Assignment 1: Plot a point given in window coordinates.
    
    //We then transform the window coordinates to pixel coordinates
    vec3f window_coordinates(v[0], v[1], v[2]);
    vec2i pixel_coords = to_pixel(window_coordinates);
    
    plot(pixel_coords, v);
    

}

/* plot_line
 *
 * Plot a line defined by two points in window coordinates.
 */
void canvashdl::plot_line(vec8f v1, vec8f v2)
{
    // TODO Assignment 1: Implement Bresenham's Algorithm.
    
    //Get the points in pixel coordinates
    vec3f window_coordinates(v1[0], v1[1], v1[2]);
    vec2i xy = to_pixel(window_coordinates);
    int x1 = xy[0];
    int y1 = xy[1];
    
    vec3f window_coordinates2(v2[0], v2[1], v2[2]);
    vec2i xy2 = to_pixel(window_coordinates2);
    int x2 = xy2[0];
    int y2 = xy2[1];
    
    // find the slope
    int dy = abs(y2-y1);
    int dx = abs(x2-x1);
    int s1 = sign(x2 - x1);
    int s2 = sign(y2 - y1);
    int x = x1;
    int y = y1;
    bool swap = false;
    if (dy > dx)
    {
        int temp = dx;
        dx = dy;
        dy = temp;
        swap = true;
    }
    int D = 2*dy - dx;
    for (int i = 0; i < dx; i++)
    {
        vec2i point1(x,y);
        plot(point1,v1);
        while (D >= 0)
        {
            D = D - 2*dx;
            if (swap)
                x += s1;
            else
                y += s2;
        }
        D = D + 2*dy;
        if (swap)
            y += s2;
        else
            x += s1;
    }
    
    
    
    // TODO Assignment 2: Add interpolation for the normals and texture coordinates as well.
}

int canvashdl::sign(int x){
    if(x<0){
        return -1;
    }
    else if(x>0){
        return 1;
    }
    return 0;
}

mat4f canvashdl::clip_frustum()
{
    mat4f clip_matrix = identity<float, 4, 4>();
    clip_matrix = matrices[projection_matrix]*matrices[modelview_matrix];
    mat4f frustum_matrix;
    
    /* Extract the numbers for the RIGHT plane */
    frustum_matrix[0][0] = clip_matrix[0][3] - clip_matrix[0][0];
    frustum_matrix[0][1] = clip_matrix[1][3] - clip_matrix[1][0];
    frustum_matrix[0][2] = clip_matrix[2][3] - clip_matrix[2][0];
    frustum_matrix[0][3] = clip_matrix[3][3] - clip_matrix[3][0];
    
    /* Normalize the result */
    float t = sqrt( frustum_matrix[0][0] * frustum_matrix[0][0] + frustum_matrix[0][1] * frustum_matrix[0][1] + frustum_matrix[0][2]    * frustum_matrix[0][2] );
    
    frustum_matrix[0][0] /= t;
    frustum_matrix[0][1] /= t;
    frustum_matrix[0][2] /= t;
    frustum_matrix[0][3] /= t;
    
    /* Extract the numbers for the LEFT plane */
    frustum_matrix[1][0] = clip_matrix[0][3] + clip_matrix[0][0];
    frustum_matrix[1][1] = clip_matrix[1][3] + clip_matrix[1][0];
    frustum_matrix[1][2] = clip_matrix[2][3] + clip_matrix[2][0];
    frustum_matrix[1][3] = clip_matrix[3][3] + clip_matrix[3][0];
    
    /* Normalize the result */
    t = sqrt( frustum_matrix[1][0] * frustum_matrix[1][0] + frustum_matrix[1][1] * frustum_matrix[1][1] + frustum_matrix[1][2]    * frustum_matrix[1][2] );
    
    frustum_matrix[1][0] /= t;
    frustum_matrix[1][1] /= t;
    frustum_matrix[1][2] /= t;
    frustum_matrix[1][3] /= t;
    
    /* Extract the BOTTOM plane */
    frustum_matrix[2][0] = clip_matrix[0][3] + clip_matrix[0][1];
    frustum_matrix[2][1] = clip_matrix[1][3] + clip_matrix[1][1];
    frustum_matrix[2][2] = clip_matrix[2][3] + clip_matrix[2][1];
    frustum_matrix[2][3] = clip_matrix[3][3] + clip_matrix[3][1];
    
    /* Normalize the result */
    t = sqrt( frustum_matrix[2][0] * frustum_matrix[2][0] + frustum_matrix[2][1] * frustum_matrix[2][1] + frustum_matrix[2][2]    * frustum_matrix[2][2] );
    
    frustum_matrix[2][0] /= t;
    frustum_matrix[2][1] /= t;
    frustum_matrix[2][2] /= t;
    frustum_matrix[2][3] /= t;
    
    /* Extract the TOP plane */
    frustum_matrix[3][0] = clip_matrix[0][3] - clip_matrix[0][1];
    frustum_matrix[3][1] = clip_matrix[1][3] - clip_matrix[1][1];
    frustum_matrix[3][2] = clip_matrix[2][3] - clip_matrix[2][1];
    frustum_matrix[3][3] = clip_matrix[3][3] - clip_matrix[3][1];
    /* Normalize the result */
    t = sqrt( frustum_matrix[3][0] * frustum_matrix[3][0] + frustum_matrix[3][1] * frustum_matrix[3][1] + frustum_matrix[3][2]    * frustum_matrix[3][2] );
    frustum_matrix[3][0] /= t;
    frustum_matrix[3][1] /= t;
    frustum_matrix[3][2] /= t;
    frustum_matrix[3][3] /= t;
    /* Extract the FAR plane */
    frustum_matrix[4][0] = clip_matrix[0][3] - clip_matrix[0][2];
    frustum_matrix[4][1] = clip_matrix[1][3] - clip_matrix[1][2];
    frustum_matrix[4][2] = clip_matrix[2][3] - clip_matrix[2][2];
    frustum_matrix[4][3] = clip_matrix[3][3] - clip_matrix[3][2];
    /* Normalize the result */
    t = sqrt( frustum_matrix[4][0] * frustum_matrix[4][0] + frustum_matrix[4][1] * frustum_matrix[4][1] + frustum_matrix[4][2]    * frustum_matrix[4][2] );
    frustum_matrix[4][0] /= t;
    frustum_matrix[4][1] /= t;
    frustum_matrix[4][2] /= t;
    frustum_matrix[4][3] /= t;
    /* Extract the NEAR plane */
    frustum_matrix[5][0] = clip_matrix[0][3] + clip_matrix[0][2];
    frustum_matrix[5][1] = clip_matrix[1][3] + clip_matrix[1][2];
    frustum_matrix[5][2] = clip_matrix[2][3] + clip_matrix[2][2];
    frustum_matrix[5][3] = clip_matrix[3][3] + clip_matrix[3][2];
    /* Normalize the result */
    t = sqrt( frustum_matrix[5][0] * frustum_matrix[5][0] + frustum_matrix[5][1] * frustum_matrix[5][1] + frustum_matrix[5][2]    * frustum_matrix[5][2] );
    frustum_matrix[5][0] /= t;
    frustum_matrix[5][1] /= t;
    frustum_matrix[5][2] /= t;
    frustum_matrix[5][3] /= t;
    
    return frustum_matrix;
}




/* plot_half_triangle
 *
 * Plot half of a triangle defined by three points in window coordinates (v1, v2, v3).
 * The remaining inputs are as follows (s1, s2, s3) are the pixel coordinates of (v1, v2, v3),
 * and (ave) is the average value of the normal and texture coordinates for flat shading.
 */
void canvashdl::plot_half_triangle(vec2i s1, vec2i s2, vec2i s3, vec8f v1, vec8f v2, vec8f v3, vec5f ave)
{
    /* TODO Assignment 2: Implement Bresenham's algorithm. You may plot the horizontal
     * half or the vertical half. Add interpolation for the normals and texture coordinates as well.
     */
}

/* plot_triangle
 *
 * Plot a triangle. (v1, v2, v3) are given in window coordinates.
 */
void canvashdl::plot_triangle(vec8f v1, vec8f v2, vec8f v3)
{
    /* TODO Assignment 1: Use the above functions to plot a whole triangle. Don't forget to
     * take into account the polygon mode. You should be able to render the
     * triangle as 3 points or 3 lines.
     */
    
    if(polygon_mode == point)
    {
        plot_point(v1);
        plot_point(v2);
        plot_point(v3);
    }
    else
    {
        plot_line(v1, v2);
        plot_line(v2, v3);
        plot_line(v3, v1);
    }
    
    
    // TODO Assignment 2: Add in the fill polygon mode.
}

/* draw_points
 *
 * Draw a set of 3D points on the canvas. Each point in geometry is
 * formatted (vx, vy, vz, nx, ny, nz, s, t).
 */
void canvashdl::draw_points(const vector<vec8f> &geometry)
{
    // TODO Assignment 1: Clip the points against the frustum, call the vertex shader, and then draw them.
    
    for(int i = 0; i < geometry.size(); ++i)
    {
        //We receive the points in window coordinates and apply the transformations
        vec8f point = shade_vertex(geometry[i]);
        
        //Plot the point
        plot_point(point);
    }
}

/* Draw a set of 3D lines on the canvas. Each point in geometry
 * is formatted (vx, vy, vz, nx, ny, nz, s, t).
 */
void canvashdl::draw_lines(const vector<vec8f> &geometry, const vector<int> &indices)
{
    // TODO Assignment 1: Clip the lines against the frustum, call the vertex shader, and then draw them.

    if(indices.size() < 2 || indices.size()%2 != 0)
    {
        cout << "Error drawing lines." << endl;
    }
    else
    {
        for(int i = 0; i < indices.size(); i+= 2)
        {
            vec8f point1 = shade_vertex(geometry[indices[i]]);
            vec8f point2 = shade_vertex(geometry[indices[i+1]]);
            plot_line(point1, point2);
        }
    }
    
    
}

/* Draw a set of 3D triangles on the canvas. Each point in geometry is
 * formatted (vx, vy, vz, nx, ny, nz, s, t). Don't forget to clip the
 * triangles against the clipping planes of the projection. You can't
 * just not render them because you'll get some weird popping at the
 * edge of the view. Also, this is where font/back face culling is implemented.
 */
void canvashdl::draw_triangles(const vector<vec8f> &geometry, const vector<int> &indices)
{
    /* TODO Assignment 1: Clip the triangles against the frustum, call the vertex shader,
     * break the resulting polygons back into triangles, implement front and back face
     * culling, and then draw the remaining triangles.
     */
    
    if(indices.size() < 3 || indices.size()%3 != 0)
    {
        cout << "Error drawing triangles." << endl;
    }
    else
    {
        for(int i = 0; i < indices.size()-2; i+= 3)
        {
            vec8f point1 = shade_vertex(geometry[indices[i]]);
            vec8f point2 = shade_vertex(geometry[indices[i+1]]);
            vec8f point3 = shade_vertex(geometry[indices[i+2]]);
            plot_triangle(point1, point2, point3);
        }
    }

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Do not edit anything below here, that code just sets up OpenGL to render a single
 * quad that covers the whole screen, applies the color_buffer as a texture to it, and
 * keeps the buffer size and texture up to date.
 */
void canvashdl::load_texture()
{
    glGenTextures(1, &screen_texture);
    check_error(__FILE__, __LINE__);
    glActiveTexture(GL_TEXTURE0);
    check_error(__FILE__, __LINE__);
    glBindTexture(GL_TEXTURE_2D, screen_texture);
    check_error(__FILE__, __LINE__);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    check_error(__FILE__, __LINE__);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    check_error(__FILE__, __LINE__);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    check_error(__FILE__, __LINE__);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    check_error(__FILE__, __LINE__);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    check_error(__FILE__, __LINE__);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, color_buffer);
    check_error(__FILE__, __LINE__);
}

void canvashdl::load_geometry()
{
    // x, y, s, t
    const GLfloat geometry[] = {
        -1.0, -1.0, 0.0, 0.0,
        1.0, -1.0, 1.0, 0.0,
        -1.0,  1.0, 0.0, 1.0,
        -1.0,  1.0, 0.0, 1.0,
        1.0, -1.0, 1.0, 0.0,
        1.0,  1.0, 1.0, 1.0
    };
    
    glGenBuffers(1, &screen_geometry);
    glBindBuffer(GL_ARRAY_BUFFER, screen_geometry);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*4*6, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*4*6, geometry);
}

void canvashdl::load_shader()
{
    GLuint vertex = load_shader_file(working_directory + "res/canvas.vx", GL_VERTEX_SHADER);
    GLuint fragment = load_shader_file(working_directory + "res/canvas.ft", GL_FRAGMENT_SHADER);
    
    screen_shader = glCreateProgram();
    glAttachShader(screen_shader, vertex);
    glAttachShader(screen_shader, fragment);
    glLinkProgram(screen_shader);
}

void canvashdl::init_opengl()
{
    glEnable(GL_TEXTURE_2D);
    glViewport(0, 0, width, height);
    
    load_texture();
    load_geometry();
    load_shader();
    initialized = true;
}

void canvashdl::check_error(const char *file, int line)
{
    GLenum error_code;
    error_code = glGetError();
    if (error_code != GL_NO_ERROR)
        cerr << "error: " << file << ":" << line << ": " << gluErrorString(error_code) << endl;
}

double canvashdl::get_time()
{
    timeval gtime;
    gettimeofday(&gtime, NULL);
    return gtime.tv_sec + gtime.tv_usec*1.0E-6;
}

void canvashdl::viewport(int w, int h)
{
    glViewport(0, 0, w, h);
    last_reshape_time = get_time();
    reshape_width = w;
    reshape_height = h;
}

void canvashdl::swap_buffers()
{
    if (!initialized)
        init_opengl();
    
    if (last_reshape_time > 0.0 && get_time() - last_reshape_time > 0.125)
        resize(reshape_width, reshape_height);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(screen_shader);
    check_error(__FILE__, __LINE__);
    
    glActiveTexture(GL_TEXTURE0);
    check_error(__FILE__, __LINE__);
    glBindTexture(GL_TEXTURE_2D, screen_texture);
    check_error(__FILE__, __LINE__);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, color_buffer);
    check_error(__FILE__, __LINE__);
    glUniform1i(glGetUniformLocation(screen_shader, "tex"), 0);
    check_error(__FILE__, __LINE__);
    
    glBindBuffer(GL_ARRAY_BUFFER, screen_geometry);
    check_error(__FILE__, __LINE__);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    check_error(__FILE__, __LINE__);
    glEnableClientState(GL_VERTEX_ARRAY);
    check_error(__FILE__, __LINE__);
    
    glTexCoordPointer(2, GL_FLOAT, sizeof(GLfloat)*4, (float*)(sizeof(GLfloat)*2));
    check_error(__FILE__, __LINE__);
    glVertexPointer(2, GL_FLOAT, sizeof(GLfloat)*4, NULL);
    check_error(__FILE__, __LINE__);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    check_error(__FILE__, __LINE__);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    check_error(__FILE__, __LINE__);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    check_error(__FILE__, __LINE__);
    
    glutSwapBuffers();
    check_error(__FILE__, __LINE__);
}

int canvashdl::get_width()
{
    return width;
}

int canvashdl::get_height()
{
    return height;
}