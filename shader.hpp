//
// Created by dh on 2022/4/24.
//

#ifndef MYTINYRENDER_SHADER_HPP
#define MYTINYRENDER_SHADER_HPP

#include "MyMath.hpp"
#include "model.hpp"

class Shader{
public:
    Shader() = default;

    Vec3f normal;
    Vec3f position;
    Light light;
    Vec3f eyePos;
    TGAColor diffuse;
    TGAColor specular;
    double spec;
    float visibility;

    TGAColor Phong_shading(){
        Vec3f eyedir = eyePos - position;
        Vec3f lightdir = light.pos - position;
        float r = lightdir * lightdir;

        lightdir.normalize();
        eyedir.normalize();

        Vec3f h = (lightdir + eyedir) * (1/2.);
        h.normalize();
//        Vec3f kd = {diffuse[2] / 255., diffuse[1] / 255., diffuse[0] / 255.};
//        Vec3f ks = {specular[0] / 255., specular[0] / 255., specular[0] / 255.};
        Vec3f kd = {0.75, 0.75, 0.75};
        Vec3f ks = {0.10, 0.10, 0.10};
        Vec3f ka = {0.005, 0.005, 0.005};
        Vec3f Ia = {30, 30, 30};
        float p = 50;

        Vec3f L_d = kd.cwiseProduct(light.indensity * (1/r) * std::max(0.0f, normal * lightdir)) * visibility;
        Vec3f L_a = ka.cwiseProduct(Ia);
        Vec3f L_s = ks.cwiseProduct(light.indensity * (1/r)) * std::pow(std::max(0.0f, normal * h), 15) * spec * visibility;

        Vec3f L = L_a + L_s + L_d;
        Vec3f res;
        for (int i = 0; i < 3; i++)
        {
            res[i] = float(diffuse[i]) * L[i];
        }
        TGAColor resColor;
        resColor[3] = diffuse[3];
        for (int i = 0; i < 3; i++)
        {
            resColor[i] = min(255, int(res[i]));
        }
        return resColor;\
    }
};

#endif //MYTINYRENDER_SHADER_HPP
