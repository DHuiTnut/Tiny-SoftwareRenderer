//
// Created by dh on 2022/4/22.
//

#ifndef MYTINYRENDER_TRANSFORM_HPP
#define MYTINYRENDER_TRANSFORM_HPP

#include "mymath.hpp"
#define MY_PI 3.1415926

Mat4f getModelMatrix(){
    return Mat4f::identity();
}

Mat4f getViewMatrix(Vec3f eye, Vec3f dir, Vec3f up){
    Vec3f z = dir - eye;
    z.normalize();
    up.normalize();
    Vec3f y = up;
    Vec3f x = z.cross(y);

    x.normalize();
    y = x.cross(z);
    y.normalize();

    Mat4f Translation({
                       {1, 0, 0, (-eye.x())},
                       {0, 1, 0, (-eye.y())},
                       {0, 0, 1, (-eye.z())},
                       {0, 0, 0, 1}});
    Mat4f View({
                {(x[0]), (x[1]), (x[2]), 0},
                {(y[0]), (y[1]), (y[2]), 0},
                {(-z[0]), (-z[1]), (-z[2]), 0},
                {0, 0, 0, 1}});
    return View * Translation;
}

Mat4f getProjectionMatrix(float near, float far, float fovY, float aspect){
    Mat4f PerspToOrtho({
                        {near, 0, 0, 0},
                        {0, near, 0, 0},
                        {0, 0, near + far, - near * far},
                        {0, 0, 1, 0}});

    float t = (-near) * tan((fovY / 2) * MY_PI / 180.0f);
    float r = aspect * t;
    float l = - r;
    float b = - t;
    Mat4f Ortho_Translation({
                            {1, 0, 0, - (r + l) / 2},
                            {0, 1, 0, - (t + b) / 2},
                            {0, 0, 1, - (near + far) / 2},
                            {0, 0, 0, 1}});
    Mat4f Ortho_scale({
                        {2 / (r - l), 0, 0, 0},
                        {0, 2 / (t - b), 0, 0},
                        {0, 0, 2 / (near - far), 0},
                        {0, 0, 0, 1}});
    return  Ortho_scale * Ortho_Translation * PerspToOrtho;
}

Mat4f getOrthoMatrix(float near, float far, float fovY, float aspect){
    float t = (-near) * tan((fovY / 2) * MY_PI / 180.0f);
    float r = aspect * t;
    float l = - r;
    float b = - t;
    Mat4f Ortho_Translation({
                                    {1, 0, 0, - (r + l) / 2},
                                    {0, 1, 0, - (t + b) / 2},
                                    {0, 0, 1, - (near + far) / 2},
                                    {0, 0, 0, 1}});
    Mat4f Ortho_scale({
                              {2 / (r - l), 0, 0, 0},
                              {0, 2 / (t - b), 0, 0},
                              {0, 0, 2 / (near - far), 0},
                              {0, 0, 0, 1}});
    return Ortho_scale * Ortho_Translation;
}

Mat4f getViewportMatrix(int width, int height){
    Mat4f Viewport({
                    {static_cast<float>(width/2), 0, 0, static_cast<float>(width/2)},
                    {0, static_cast<float>(height/2), 0, static_cast<float>(height/2)},
                    {0, 0, 1, 0},
                    {0, 0, 0, 1}});
    return Viewport;
}

#endif //MYTINstatic_cast<float>(YRENDER_TRANSFOR)M_HPP
