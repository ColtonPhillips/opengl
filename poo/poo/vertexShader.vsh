in vec4 s_vPosition;
in vec4 s_vColor;
out vec4 color;
void main () {
	color = s_vPosition;
	gl_Position = s_vPosition;
}