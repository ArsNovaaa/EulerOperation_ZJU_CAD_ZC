#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexColor;

out vec3 fragmentColor;
uniform mat4 u_MVP;

void main(){	

	
	gl_Position =  u_MVP * vec4(position,1);
	fragmentColor = vertexColor;
}

#shader fragment
#version 330 core


in vec3 fragmentColor;

out vec4 color;

void main(){

	color = vec4(fragmentColor,1);

}
