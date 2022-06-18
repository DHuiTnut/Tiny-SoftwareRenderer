//
// Created by dh on 2022/4/21.
//

#ifndef MYTINYRENDER_MODEL_HPP
#define MYTINYRENDER_MODEL_HPP
#include <iostream>
#include <string>
#include <sstream>
#include "MyMath.hpp"
#include "tgaimage.h"

struct Light{
    Vec3f   pos;
    Vec3f   indensity;
    Light(Vec3f pos, Vec3f indensity): pos(pos), indensity(indensity) {}
    Light() = default;
};

class Model {
private:
    vector<Vec3f> vertexs;
    vector<Vec2f> texCoords;
    vector<Vec3f> normals;
    vector<Vec3i> ver_idxs;
    vector<Vec3i> tex_idxs;
    vector<Vec3i> norm_idxs;

    TGAImage diffuseMap;
    TGAImage normalMap;
    TGAImage specularMap;

//    Light light;
//    Light eye;

public:
    Model(string filename);
    int numsOfTriangle(){
        return ver_idxs.size();
    }
    vector<Vec3f> getVertexs(int index);
    vector<Vec2f> getTexCoords(int index);
    vector<Vec3f> getNormals(int index);
    TGAColor getColor(float u, float v);
    TGAColor getSpecular(float u, float v);
    float getSpec(float u, float v);

    Vec3f getTexNormal(float u, float v, const Mat3f &TBN);

    Vec3d getTexNormal(float u, float v, const Mat3d &TBN);
};

Model::Model(string filename) {
    string objPath = "../obj/" + filename + "/" + filename + ".obj";
    string texPath = "../obj/" + filename + "/" + filename + "_diffuse.tga";
    string normalPath = "../obj/" + filename + "/" + filename + "_nm_tangent.tga";
    string specPath = "../obj/" + filename + "/" + filename + "_spec.tga";
    diffuseMap.read_tga_file(texPath);
    normalMap.read_tga_file(normalPath);
    specularMap.read_tga_file(specPath);
    ifstream file;
    file.open("../obj/" + filename + +"/" + filename + ".obj");
    if(file.fail()){
        cerr << "Fail to open file: " + filename + ".obj" << endl;
        return;
    }

    string line;
    while(getline(file, line)){
       istringstream iss(line);
       string trash;
       if(line.compare(0, 2, "v ") == 0){
           iss >> trash;
           Vec3f vertex;
           iss >> vertex[0] >> vertex[1] >> vertex[2];
           vertexs.push_back(vertex);
       }
       else if(line.compare(0, 3, "vt ") == 0){
           iss >> trash;
           Vec2f tex_coord;
           iss >> tex_coord[0] >> tex_coord[1];
           texCoords.push_back(tex_coord);
       }
       else if(line.compare(0, 3, "vn ") == 0){
           iss >> trash;
           Vec3f ver_normal;
           iss >> ver_normal[0] >> ver_normal[1] >> ver_normal[2];
           ver_normal.normalize();
           normals.push_back(ver_normal);
       }
       else if(line.compare(0, 2, "f ") == 0) {
            iss >> trash;
            char c_trash;
            Vec3i vIdx, vt_idx, vn_idx;
            for(int i = 0; i < 3; i++){
                iss >> vIdx[i] >> c_trash;
                iss >> vt_idx[i] >> c_trash;
                iss >> vn_idx[i];
                vIdx[i]--;
                vt_idx[i]--;
                vn_idx[i]--;
            }
            ver_idxs.push_back(vIdx);
            tex_idxs.push_back(vt_idx);
            norm_idxs.push_back(vn_idx);
       }
       else
           continue;
    }
    cout << "Success to load objPath:  " << objPath << endl;
    cout << "Triangle num: " << ver_idxs.size() << endl;
    cout << "Position nums: " << vertexs.size() << endl;
    cout << "normal nums: " << normals.size() << endl;
    cout << "texCoord num: " << texCoords.size() << endl;
    file.close();
}

vector<Vec3f> Model::getVertexs(int index) {
    Vec3i idx = ver_idxs[index];
    return {vertexs[idx[0]], vertexs[idx[1]], vertexs[idx[2]]};
}

vector<Vec2f> Model::getTexCoords(int index) {
    Vec3i idx = tex_idxs[index];
    return {texCoords[idx[0]], texCoords[idx[1]], texCoords[idx[2]]};
}

vector<Vec3f> Model::getNormals(int index) {
    Vec3i idx = norm_idxs[index];
    return {normals[idx[0]], normals[idx[1]], normals[idx[2]]};
}

Vec3f Model::getTexNormal(float u, float v, const Mat3f& TBN) {
    v = 1 - v;
    u = u * (diffuseMap.width()-1);
    v = v * (diffuseMap.height()-1);
    TGAColor color = normalMap.get(floor(u),floor(v));
    Vec3f normal = {color[2], color[1], color[0]};
    normal = normal * (1./255) *2. + (-1.);
    normal = TBN * normal;
    normal.normalize();
    return normal;
}

TGAColor Model::getColor(float u, float v) {
    v = 1 - v;
    u = u * (diffuseMap.width());
    v = v * (diffuseMap.height());
//    int u0 = floor(u), u1 = ceil(u);
//    int v0 = floor(v), v1 = ceil(v);
//    TGAColor c00 = diffuseMap.get(u0, v0);
//    TGAColor c01 = diffuseMap.get(u0, v1);
//    TGAColor c10 = diffuseMap.get(u1, v0);
//    TGAColor c11 = diffuseMap.get(u1, v1);
//    TGAColor interp_c0 = c00 + c01 * (u-u0);

    TGAColor color = diffuseMap.get(floor(u),floor(v));
//    cout<<float(color[0]) << " " << float(color[1]) << " " << float(color[2])<<endl;
    return color;
}

TGAColor Model::getSpecular(float u, float v) {
    v = 1 - v;
    u = u * (diffuseMap.width());
    v = v * (diffuseMap.height());
    TGAColor color = specularMap.get(floor(u),floor(v));
    return color;
}

float Model::getSpec(float u, float v){
    v = 1 - v;
    u = u * (diffuseMap.width());
    v = v * (diffuseMap.height());
    return specularMap.get(floor(u),floor(v)) [0] / 1.0f;
}

#endif //MYTINYRENDER_MODEL_HPP
