//
// Created by Tnut on 2022/4/20.
//

#ifndef MYTYNIRENDER_RASTERZATER_HPP
#define MYTYNIRENDER_RASTERZATER_HPP

#include <iostream>
#include <limits>
#include <cfloat>
#include "model.hpp"
#include "tgaimage.h"
#include "mymath.hpp"
#include "triangle.hpp"
#include "shader.hpp"

Vec3f computeBarycentric2D(float x, float y, vector<Vec4f> v){
    float c1 = (x*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*y + v[1].x()*v[2].y() - v[2].x()*v[1].y()) / (v[0].x()*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*v[0].y() + v[1].x()*v[2].y() - v[2].x()*v[1].y());
    float c2 = (x*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*y + v[2].x()*v[0].y() - v[0].x()*v[2].y()) / (v[1].x()*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*v[1].y() + v[2].x()*v[0].y() - v[0].x()*v[2].y());
    float c3 = (x*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*y + v[0].x()*v[1].y() - v[1].x()*v[0].y()) / (v[2].x()*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*v[2].y() + v[0].x()*v[1].y() - v[1].x()*v[0].y());
    return {c1,c2,c3};
}

float getVisibility(Vec3f pointsWorld, vector<float>& depthBuffer, const Mat4f& lightTrans, int filterSize, int width, int height){
    float bios = 0.015;
    Vec4f point = {pointsWorld[0], pointsWorld[1], pointsWorld[2], 1.};
    point = lightTrans * point;
    point = point * (1 / point[3]);
    int X = ceil(point[0]);
    int Y = ceil(point[1]);
    float totalCount = 0;
    float count = 0;
    int k = filterSize / 2;
    for (int x = X - k; x <= X + k; ++x)
    {
        for (int y = Y - k; y <= Y + k; ++y)
        {
            if (x >= 0 && y >= 0 && x < width && y < width)
            {
                totalCount++;
                float z = depthBuffer[x + y * width];
                if (point[2] - bios < z)
                    count++;
            }
        }
    }
    return count / totalCount;

}

void rasterize_triangle(Triangle& triangle, TGAImage& image, vector<float>& z_buffer, Model& model, Light& light, vector<float>& depthBuffer, const Mat4f& lightTrans){
    vector<Vec4f> v = triangle.getVerScreen();
    vector<Vec3f> ver_world = triangle.getVerWorld();
    vector<Vec3f> normal = triangle.getNormals();
    vector<Vec2f> tex_coord = triangle.getTexCoord();
    int xmax = std::ceil(std::max(v[0].x(), std::max(v[1].x(), v[2].x())) + 1);
    int xmin = std::floor(std::min(v[0].x(), std::min(v[1].x(), v[2].x())) - 1);
    int ymax = std::ceil(std::max(v[0].y(), std::max(v[1].y(), v[2].y())) + 1);
    int ymin = std::floor(std::min(v[0].y(), std::min(v[1].y(), v[2].y())) - 1);
    xmax = std::min(xmax, image.width() - 1);
    ymax = std::min(ymax, image.height() - 1);
    xmin = std::max(0, xmin);
    ymin = std::max(0, ymin);
    for(int i = xmin; i <= xmax; i++)
        for(int j = ymin; j <= ymax; j++){
            Vec3f barycentric = computeBarycentric2D(i + 0.5, j + 0.5, v);
            float alpha = barycentric.x(), beta = barycentric.y(), gamma = barycentric.z();
            if(alpha < 0 || beta < 0 || gamma < 0) continue;

            float z_interpolated = alpha * v[0].z() + beta * v[1].z() + gamma * v[2].z();

            int index = i + j * image.width();
            if(z_interpolated > z_buffer[index]){
                float pw = v[0].w() * v[1].w() * v[2].w() / (alpha * v[1].w() * v[2].w() + beta * v[0].w() * v[2].w() + gamma * v[0].w() * v[1].w());
                alpha = alpha * pw / v[0].w();
                beta = beta * pw / v[1].w();
                gamma = gamma * pw /v[2].w();


                Vec3f n =  normal[0] * alpha + normal[1] * beta + normal[2] * gamma;
                Vec2f uv = tex_coord[0] * alpha + tex_coord[1] * beta + tex_coord[2] * gamma;
                Vec3f pos = ver_world[0] * alpha + ver_world[1] * beta + ver_world[2] * gamma;

                // 计算切线空间基向量TBN
                Vec3f v1 = ver_world[1] - ver_world[0], v2 = ver_world[2] - ver_world[0];
                float du1 = tex_coord[1][0] - tex_coord[0][0], du2 = tex_coord[2][0] - tex_coord[0][0];
                float dv1 = tex_coord[1][1] - tex_coord[0][1], dv2 = tex_coord[2][1] - tex_coord[0][1];
                float f = 1.0f / (du1 * dv2 - du2 * dv1);
                Vec3f t = v1 * (dv2 / f) + v2 * (- dv1 / f), b = v1 * (- du2 / f) + v2 * (du1 / f);

                n.normalize();
                // 施密特正交化
//                Vec3f T = t - n * (t * n);
//                T.normalize();
//                Vec3f B = b - T * (b * T) - n * (b * n);
//                B.normalize();
//                Mat3f TBN = {
//                        {T[0], B[0], n[0]},
//                        {T[1], B[1], n[1]},
//                        {T[2], B[2], n[2]}
//                };

                Mat3d tmpMat3d;
                tmpMat3d[0] = ver_world[1] - ver_world[0],
                tmpMat3d[1] = ver_world[2] - ver_world[0],
                tmpMat3d[2] = n;
                tmpMat3d = tmpMat3d.getInvert();
                Vec3d tmpVec3d1 = {tex_coord[1][0] - tex_coord[0][0], tex_coord[2][0] - tex_coord[0][0], 0.0f};
                Vec3d tmpVec3d2 = {tex_coord[1][1] - tex_coord[0][1], tex_coord[2][1] - tex_coord[0][1], 0.0f};
                Vec3d T = tmpMat3d * tmpVec3d1;
                T.normalize();
                ///TODO B=T cross N
                Vec3d B = tmpMat3d * tmpVec3d2;
                B.normalize();
                Mat3d TBN
                        {
                                {T[0], B[0], n[0]},
                                {T[1], B[1], n[1]},
                                {T[2], B[2], n[2]}
                        };

                Shader shader;
                shader.position = pos;
                shader.normal = model.getTexNormal(uv[0], uv[1], TBN);
                shader.diffuse = model.getColor(uv[0], uv[1]);
                shader.specular = model.getSpecular(uv[0], uv[1]);
                shader.light = light;
                shader.spec = model.getSpec(uv[0], uv[1]);
                shader.visibility = getVisibility(pos, depthBuffer, lightTrans, 7, image.width(), image.height());

                TGAColor color = shader.Phong_shading();
                image.set(i,j,color);
                z_buffer[index] = z_interpolated;
            }
        }
}

void rasterize(vector<Vec4f>& v, TGAImage& image, vector<float>& z_buffer, TGAColor color){
    int xmax = std::ceil(std::max(v[0].x(), std::max(v[1].x(), v[2].x())) + 1);
    int xmin = std::floor(std::min(v[0].x(), std::min(v[1].x(), v[2].x())) - 1);
    int ymax = std::ceil(std::max(v[0].y(), std::max(v[1].y(), v[2].y())) + 1);
    int ymin = std::floor(std::min(v[0].y(), std::min(v[1].y(), v[2].y())) - 1);
    xmax = std::min(xmax, image.width() - 1);
    ymax = std::min(ymax, image.height() - 1);
    xmin = std::max(0, xmin);
    ymin = std::max(0, ymin);
    for(int i = xmin; i <= xmax; i++)
        for(int j = ymin; j <= ymax; j++){
            Vec3f barycentric = computeBarycentric2D(i + 0.5, j + 0.5, v);
            float alpha = barycentric.x(), beta = barycentric.y(), gamma = barycentric.z();
            if(alpha < 0 || beta < 0 || gamma < 0) continue;
            float z_interpolated = alpha * v[0].z() + beta * v[1].z() + gamma * v[2].z();

            int index = i + j * image.width();
            if(z_interpolated > z_buffer[index]){
                image.set(i,j,color * ((z_interpolated + 1.f)*2.f));
                z_buffer[index] = z_interpolated;
            }
        }
}
#endif

