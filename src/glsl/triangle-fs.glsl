#version 430 core

precision highp float;

uniform struct{
    vec3 kd;
} material;

out vec4 FragColor;

void main(){
    FragColor = vec4(material.kd, 1.0f);
}