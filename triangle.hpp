//
// Created by dh on 2022/4/24.
//

#ifndef MYTINYRENDER_TRIANGLE_HPP
#define MYTINYRENDER_TRIANGLE_HPP

#include "MyMath.hpp"

class Triangle{
    vector<Vec3f> normal;
    vector<Vec3f> ver_world;
    vector<Vec4f> ver_screen;
    vector<Vec2f> tex_coord;
public:
    Triangle(vector<Vec3f>& ver_world, vector<Vec4f>& ver_screen, vector<Vec3f>& normals, vector<Vec2f>& tex_coords):normal(normals), ver_screen(ver_screen), ver_world(ver_world), tex_coord(tex_coords){}
    vector<Vec3f> getNormals() {return normal;}
    vector<Vec4f> getVerScreen() {return ver_screen;}
    vector<Vec3f> getVerWorld() {return ver_world;}
    vector<Vec2f> getTexCoord() {return tex_coord;}
};

#endif //MYTINYRENDER_TRIANGLE_HPP
