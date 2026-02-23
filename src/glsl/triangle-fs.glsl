#version 330 core

uniform vec3 kd;
out vec4 FragColor;

void main(){
    FragColor = vec4(kd, 1.0f);
}