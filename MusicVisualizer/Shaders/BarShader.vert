#version 450

layout (location = 0) in vec3 position;
out vec3 fs_position;

void main(){
	fs_position = vec3(1,2,1)*position-vec3(0, 1, 0);
	gl_Position = vec4(fs_position, 1.0);
}