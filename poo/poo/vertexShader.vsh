in vec4 position;
in vec4 pixelColor;
out vec4 color;
void main () {
	color = pixelColor;
	gl_Position = position;
}