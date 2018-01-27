#version 450

in vec3 fs_position;

out vec4 color;

void main(){
	if(fs_position.y < 0)
		color = mix(vec4(1, 1, 0, 1), vec4(0, 0, 1, 1), -fs_position.y);
	else
		color = mix(vec4(1, 1, 0, 1), vec4(1, 0, 0, 1), fs_position.y);
	//color = vec4(0, fs_position.y, 1, 1.0);
}
