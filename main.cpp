//
// Created by dh on 2022/4/22.
//

#include <cfloat>
#include "model.hpp"
#include "tgaimage.h"
#include "mymath.hpp"
#include "rasterizater.hpp"
#include "transform.hpp"
#include "triangle.hpp"

constexpr int width  = 1500; // output image size
constexpr int height = 1500;

const Vec3f     lightPos({1., 1., 2.}); // light source
const Vec3f     lightDir({0., 0., 0.});
const Vec3f     lightIndensity({8, 8, 8});
const Vec3f       eye({1., 0., 2.}); // camera position
const Vec3f    center({0., 0., 0.}); // camera direction
const Vec3f        up({0., 1., 0.}); // camera up vector
const float    FovY = 60;
const float    near = -0.1;
const float    far  = -10;
const float    aspect = 1;


int main(){
    TGAImage frame(width, height, TGAImage::RGB);
    TGAImage depthImage(width, height, TGAImage::RGB);
    vector<float> z_buffer(height * width, -FLT_MAX);
    vector<float> depthBuffer(height * width, -FLT_MAX);
    Light light(lightPos, lightIndensity);

    Model model("diablo3_pose");
    int numTriangles = model.numsOfTriangle();

    Mat4f modelTrans = getModelMatrix();
    Mat4f viewTrans = getViewMatrix(eye, center, up);
    Mat4f projTrans = getProjectionMatrix(near, far, FovY, aspect);
    Mat4f viewportTrans = getViewportMatrix(width, height);
//    Mat4f mvp =  projTrans * viewTrans * modelTrans;
//    Mat4f mv = viewTrans * modelTrans;
    Mat4f vpv = viewportTrans * projTrans * viewTrans;

    Mat4f lightViewTrans = getViewMatrix(lightPos, lightDir, up);
    Mat4f lightProjTrans = getProjectionMatrix(near, far, FovY, aspect);
//    Mat4f lightOrthoTrans = getOrthoMatrix(near, far, FovY, aspect);
    Mat4f lightTrans = viewportTrans * lightProjTrans * lightViewTrans * modelTrans;

    for(int i = 0; i < model.numsOfTriangle(); i++) {
        vector<Vec3f> vertices = model.getVertexs(i);
        vector<Vec4f> verScreen;
        for (auto &v: vertices) {
            Vec4f ver_homo = {v[0], v[1], v[2], 1.};
            ver_homo = lightTrans * ver_homo;
            verScreen.push_back({ver_homo[0] / ver_homo[3], ver_homo[1] / ver_homo[3], ver_homo[2] / ver_homo[3], 1.});
        }
        rasterize(verScreen, depthImage, depthBuffer, TGAColor(255., 255., 255.));
    }
    depthImage.write_tga_file("depthImage.tga");

    for(int i = 0; i < model.numsOfTriangle(); i++){
        vector<Vec3f> vertices = model.getVertexs(i);
        vector<Vec3f> normals = model.getNormals(i);
        vector<Vec2f> texCoords = model.getTexCoords(i);
        vector<Vec3f> verWorld;
        vector<Vec4f> verScreen; // 保留齐次坐标中的w的值，用于三维空间计算重心坐标，再进行插值。
        for(auto& n : normals){
            Vec4f n_homo = {n[0], n[1], n[2], 0.};
//            n_homo = viewTrans * modelTrans * n_homo;
            n_homo = modelTrans.getInvert().getTranspose() * n_homo;
            n = {n_homo[0], n_homo[1], n_homo[2]};
            n.normalize();
        }
        for(auto& v : vertices){
            Vec4f ver_homo = {v[0], v[1], v[2], 1.};
            ver_homo =  modelTrans * ver_homo;
            verWorld.push_back({ver_homo[0] / ver_homo[3], ver_homo[1] / ver_homo[3], ver_homo[2] / ver_homo[3]});
            ver_homo = vpv * ver_homo;
            verScreen.push_back({ver_homo[0] / ver_homo[3], ver_homo[1] / ver_homo[3], ver_homo[2] / ver_homo[3], ver_homo[3]});
        }
        Triangle triangle(verWorld, verScreen, normals, texCoords);
        rasterize_triangle(triangle, frame, z_buffer, model, light, depthBuffer, lightTrans);
    }
//    frame.flip_vertically();
    frame.write_tga_file("framebuffer.tga");
    return 0;
}