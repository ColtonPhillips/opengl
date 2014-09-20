in vec4 s_vPosition;
in vec4 s_vColor;
out vec4 color;
void main () {
	color = s_vColor;
	gl_Position = s_vPosition;
}