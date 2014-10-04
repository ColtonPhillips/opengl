
#include "Angel.h"
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdexcept>
namespace Angel {

// Create a NULL-terminated string by reading the provided file
static char*
readShaderSource(const char* shaderFile)
{
    FILE* fp = fopen(shaderFile, "r");

    if ( fp == NULL ) { return NULL; }

    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);

    fseek(fp, 0L, SEEK_SET);
    char* buf = new char[size + 1];
    fread(buf, 1, size, fp);

    buf[size] = '\0';
    fclose(fp);

    return buf;
}

static char* readShaderSource2(const char* filename) { 
	FILE* fp = fopen(filename,"r");
	fseek(fp, 0, SEEK_END);
	long file_length = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* contents = new char[file_length+1];
	for (int i = 0; i < file_length+1;i++) {
		contents[i] = 0;
	}
	fread (contents, 1, file_length, fp);
	contents[file_length] = '\0';
	fclose(fp);
	return contents;
}

std::string
readFile(const char* filename) {
    std::stringstream ss;
    std::fstream f(filename);
    ss << f.rdbuf();
	return ss.str();
}

static std::string readFile2(const std::string& filename){
    std::ifstream file(filename);
    if (!file) { throw std::runtime_error("failed to open " + filename); }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

// Create a GLSL program object from vertex and fragment shader files
GLuint
InitShader(const char* vShaderFile, const char* fShaderFile)
{
	std::string sources[2] = { readFile2(vShaderFile), readFile2(fShaderFile) };
	GLenum types[2] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };

    GLuint program = glCreateProgram();
    
    for ( int i = 0; i < 2; ++i ) {
		if ( sources[i].c_str() == NULL ) {
			std::cerr << "Failed to read " << i << std::endl;
			exit( EXIT_FAILURE );
		}

		GLuint shader = glCreateShader( types[i] );
		const char *src = sources[i].c_str();
		glShaderSource( shader, 1, (const GLchar**) &src, NULL );
		glCompileShader( shader );

		GLint  compiled;
		glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled ) {
			std::cerr << " failed to compile:" << std::endl;
			GLint  logSize;
			glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logSize );
			char* logMsg = new char[logSize];
			glGetShaderInfoLog( shader, logSize, NULL, logMsg );
			std::cerr << logMsg << std::endl;
			delete [] logMsg;

			exit( EXIT_FAILURE );
		}

		//delete [] s.source;

		glAttachShader( program, shader );
    }

    /* link  and error check */
    glLinkProgram(program);

    GLint  linked;
    glGetProgramiv( program, GL_LINK_STATUS, &linked );
    if ( !linked ) {
	std::cerr << "Shader program failed to link" << std::endl;
	GLint  logSize;
	glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logSize);
	char* logMsg = new char[logSize];
	glGetProgramInfoLog( program, logSize, NULL, logMsg );
	std::cerr << logMsg << std::endl;
	delete [] logMsg;

	exit( EXIT_FAILURE );
    }

    /* use program object */
    glUseProgram(program);

    return program;
}

}  // Close namespace Angel block