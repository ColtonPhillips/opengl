#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdio>
#include <string>
#include <sstream>
#include <fstream>
//using namespace std;

#define BUFFER_OFFSET(i) ((char *) NULL + (i))

std::string readFile(const char* filename) {
    std::stringstream ss;
    std::fstream f(filename);
    ss << f.rdbuf();
    return ss.str();
}

/*static char* readFile(const char* filename) { 
	FILE* fp = fopen(filename,"r");
	fseek(fp, 0, SEEK_END);
	long file_length = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* contents = new char[file_length+1];
	for (int i = 0; i < file_length+1;i++) {
		contents[i] = 0;
	}
	fread (contents, 1, file_length, fp);
	contents[file_length+1] = '\0';
	return contents;
}
*/
GLuint makeVertexShader(const char* shaderSource) {
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource (vertexShaderID, 1, (const GLchar**)&shaderSource, NULL);
	glCompileShader(vertexShaderID);
	return vertexShaderID;
}

GLuint makeFragmentShader(const char* shaderSource) {
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, (const GLchar**)&shaderSource, NULL);
	glCompileShader(fragmentShaderID);
	return fragmentShaderID;
}

GLuint makeShaderProgram (GLuint vertexShaderID, GLuint fragmentShaderID) {
	GLuint shaderID = glCreateProgram();
	glAttachShader(shaderID, vertexShaderID);
	glAttachShader(shaderID, fragmentShaderID);
	glLinkProgram(shaderID);
	return shaderID;
}

void changeViewport(int w, int h) {
		glViewport(0,0,w,h);
}

void render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0 , 3);
	glutSwapBuffers();
}

int amain (int argc, char** argv)
{
	glewExperimental = GL_TRUE;
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(800,600);
	glutCreateWindow("Hello GL");
	glutReshapeFunc(changeViewport);
	glutDisplayFunc(render);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cout << "shish";
		return 1;
	}

	GLfloat vertices[] = {-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f, 0.5f, 0.0f};
	GLfloat colors[] = {1.0f, 0.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f, 1.0f};


	std::string vertexShaderSourceCode = readFile("vertexShader.vsh");
	std::string fragmentShaderSourceCode = readFile("fragmentShader.fsh");
	GLuint vertexShaderID = makeVertexShader(vertexShaderSourceCode.c_str());
	GLuint fragmentShaderID = makeFragmentShader(fragmentShaderSourceCode.c_str());
	GLuint shaderProgramID = makeShaderProgram(vertexShaderID, fragmentShaderID);

	printf("vertexShaderID is %d\n", vertexShaderID);
	printf("fragmentShaderID is %d\n", fragmentShaderID);
	printf("shaderProgramID is %d\n", shaderProgramID);
	
	GLuint vao = 0;
	GLuint vbo;
	GLuint positionID, colorID;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// you can probably simplify your glBufferData/glBufferSubData by using sizeof vertices and sizeof colors instead of manually calculating it
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 7*3*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3*3*sizeof(GLfloat), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 3*3*sizeof(GLfloat),3*4*sizeof(GLfloat), colors);

	positionID = glGetAttribLocation(shaderProgramID, "position");
	colorID = glGetAttribLocation(shaderProgramID, "pixelColor");

	glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE,0,0);
	glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(3*3*sizeof(GLfloat)));
	glUseProgram(shaderProgramID);
	glEnableVertexAttribArray(positionID);
	glEnableVertexAttribArray(colorID);

	glutMainLoop();
	return 0;
}