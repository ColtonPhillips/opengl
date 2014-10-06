//
//  Display a rotating cube with lighting
//
//  Light and material properties are sent to the shader as uniform
//    variables.  Vertex positions and normals are sent after each
//    rotation.
#include <ctime>
#include <cstdlib>
#include "Angel.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)
point4 points[NumVertices];
vec3   normals[NumVertices];
point4 vertices[8];
float spin_speed = 0.05;
float frustrum_angle = 45.0;
float frustrum_near = 0.5;
float frustrum_far = 3.0;
float viewer_x = 0.0;
float viewer_y = 0.0;
float viewer_z = 2.0;
point4 light_position;
GLuint program;
float timer = 0.0;

// Seems sorta silly of a HACK idea here? - COLTON
// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };

// Model-view and projection matrices uniform location
GLuint  ModelView, Projection;

float random_range(float min, float max) {
	return min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max-min)));
}

// Vertices of a unit cube centered at origin, sides aligned with axes
void init_vertices() {

	float init_ndc_cube[8][3] = {
		{ -0.5,		-0.5,	0.5},
		{-0.5,		0.5,	0.5},
		{0.5,		0.5,	0.5},
		{0.5,		-0.5,	0.5},
		{-0.5,		-0.5,	-0.5},
		{-0.5,		0.5,	-0.5},
		{0.5,		0.5,	-0.5},
		{0.5,		-0.5,	-0.5}
	};

	std::srand(std::time(0));
	float h;
	for (int i = 0; i < 8; i++) {
		h = random_range(1.0,5.0);
		vertices[i] = point4(init_ndc_cube[i][0],init_ndc_cube[i][1],init_ndc_cube[i][2],h);
	}
}

int qIndex = 0;
// quad generates two triangles for each face and assigns colors to the vertices
void
quad( int a, int b, int c, int d )
{
    // Initialize temporary vectors along the quad's edge to
    //   compute its face normal 
    vec4 u = vertices[b] - vertices[a];
    vec4 v = vertices[c] - vertices[b];
	
    vec3 normal = normalize( cross(u, v) );
    normals[qIndex] = normal; points[qIndex] = vertices[a]; qIndex++;
    normals[qIndex] = normal; points[qIndex] = vertices[b]; qIndex++;
    normals[qIndex] = normal; points[qIndex] = vertices[c]; qIndex++;
    normals[qIndex] = normal; points[qIndex] = vertices[a]; qIndex++;
    normals[qIndex] = normal; points[qIndex] = vertices[c]; qIndex++;
    normals[qIndex] = normal; points[qIndex] = vertices[d]; qIndex++;
}

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}

// OpenGL initialization
void
init()
{
    colorcube();

	glewExperimental = GL_TRUE;
	glewInit();

    GLuint vao = 0;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER,		sizeof(points) + sizeof(normals), NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER,	0,					sizeof(points),		points );
    glBufferSubData( GL_ARRAY_BUFFER,	sizeof(points),		sizeof(normals),	normals );

    program = InitShader( "vshader53.glsl", "fshader53.glsl" );
    glUseProgram( program );

    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(0) );

    GLuint vNormal = glGetAttribLocation( program, "vNormal" ); 
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(sizeof(points)) );

    light_position = point4( 0.0, 0.0, -1.0, 0.0 );
    color4 light_ambient( 0.2, 0.2, 0.2, 1.0 );
    color4 light_diffuse( 1.0, 1.0, 1.0, 1.0 );
    color4 light_specular( 1.0, 1.0, 1.0, 1.0 );

    color4 material_ambient( 1.0, 0.0, 1.0, 1.0 );
    color4 material_diffuse( 1.0, 0.8, 0.0, 1.0 );
    color4 material_specular( 1.0, 0.8, 0.0, 1.0 );
    float  material_shininess = 100.0;

    color4 ambient_product = light_ambient * material_ambient;
    color4 diffuse_product = light_diffuse * material_diffuse;
    color4 specular_product = light_specular * material_specular;

    glUniform4fv( glGetUniformLocation(program, "AmbientProduct"),
		  1, ambient_product );
    glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"),
		  1, diffuse_product );
    glUniform4fv( glGetUniformLocation(program, "SpecularProduct"),
		  1, specular_product );
    glUniform4fv( glGetUniformLocation(program, "LightPosition"),
		  1, light_position );
    glUniform1f( glGetUniformLocation(program, "Shininess"),
		 material_shininess );

    ModelView = glGetUniformLocation( program, "ModelView" );
    Projection = glGetUniformLocation( program, "Projection" );

    glEnable( GL_DEPTH_TEST );

    glShadeModel(GL_FLAT);
	
	std::srand(std::time(0));
	float r = random_range(0.0,1.0);
	float g = random_range(0.0,1.0);
	float b = random_range(0.0,1.0);
    glClearColor( r, g, b, 1.0 ); 
}

void changeProjection(float angle,float znear, float zfar) {
	
    GLfloat aspect = GLfloat(glutGet(GLUT_WINDOW_WIDTH))/glutGet(GLUT_WINDOW_HEIGHT);
	mat4  projection = Perspective( angle, aspect, znear, zfar );
    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
}

void
display( void )
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const vec3 viewer_pos( viewer_x, viewer_y, viewer_z );
    mat4  model_view = ( Translate( -viewer_pos ) *
			 RotateX( Theta[Xaxis] ) *
			 RotateY( Theta[Yaxis] ) *
			 RotateZ( Theta[Zaxis] ) );
    
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view );

	changeProjection(frustrum_angle,frustrum_near,frustrum_far);

    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
    glutSwapBuffers();
}

void
mouse( int button, int state, int x, int y )
{
    if ( state == GLUT_DOWN ) {
	switch( button ) {
	    case GLUT_LEFT_BUTTON:    Axis = Xaxis;  break;
	    case GLUT_MIDDLE_BUTTON:  Axis = Yaxis;  break;
	    case GLUT_RIGHT_BUTTON:   Axis = Zaxis;  break;
	}
    }
}

void
idle( void )
{
	timer+=0.02;
    Theta[Axis] += spin_speed;

    if ( Theta[Axis] > 360.0 ) {
	Theta[Axis] -= 360.0;
    }

	light_position.x = std::sin(timer);
	light_position.y = std::sin(timer);
    light_position.z = std::sin(timer);

	glUniform4fv( glGetUniformLocation(program, "LightPosition"), 1, light_position );

    glutPostRedisplay();
}

void
keyboard( unsigned char key, int x, int y )
{
    switch( key ) {
	case '1': spin_speed-=0.01;
		break;
	case '2': spin_speed+=0.01;
		break;	
	case '3': frustrum_near-=0.01;
		break;
	case '4': frustrum_near+=0.01;
		break;
	case '5': frustrum_far-=0.1;
		break;
	case '6': frustrum_far+=0.1;
		break;
	case '7': viewer_x-=0.1;
		break;
	case '8': viewer_x+=0.1;
		break;
	case '9': viewer_y-=0.1;
		break;
	case '0': viewer_y+=0.1;
		break;
	case 'q': viewer_z-=0.1;
		break;
	case 'w': viewer_z+=0.1;
		break;
	case 033: // Escape Key
	    exit( EXIT_SUCCESS );
	    break;
    }
}

void
specialInput( int key, int x, int y ) {
	switch( key ) {
	case GLUT_KEY_LEFT: spin_speed-=0.01;
		break;
	case GLUT_KEY_RIGHT: spin_speed+=0.01;
		break;
	case GLUT_KEY_UP: frustrum_angle-=2.5;
		break;
	case GLUT_KEY_DOWN: frustrum_angle+=2.5;
		break;
	}
}

void
reshape( int width, int height )
{
    glViewport( 0, 0, width, height );
   	changeProjection(frustrum_angle,frustrum_near,frustrum_far);
}

int
main( int argc, char **argv )
{
	init_vertices();

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
    glutCreateWindow( "Color Cube" );
    init();

    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutReshapeFunc( reshape );
    glutMouseFunc( mouse );
    glutIdleFunc( idle );
	glutSpecialFunc( specialInput );

    glutMainLoop();
    return 0;
}