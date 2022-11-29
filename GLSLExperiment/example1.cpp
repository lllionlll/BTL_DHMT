//Chương trình vẽ 1 hình lập phương đơn vị theo mô hình lập trình OpenGL hiện đại

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/


// remember to prototype
void generateGeometry( void );
void initGPUBuffers( void );
void shaderSetup( void );
void display( void );
void keyboard( unsigned char key, int x, int y );


typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/


point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;

GLuint model_loc, view_loc, projection_loc;
mat4 ctm;
mat4 instance;
mat4 model;

void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 1.0, 1.0, 1.0); // white
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void generateGeometry( void )
{
	initCube();
	makeColorCube();
}


void initGPUBuffers( void )
{
	// Tạo một VAO - vertex array object
	GLuint vao;
    glGenVertexArrays( 1, &vao );     
    glBindVertexArray( vao );

    // Tạo và khởi tạo một buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points)+sizeof(colors), NULL, GL_STATIC_DRAW );

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors); 
	
}


void shaderSetup( void )
{
	// Nạp các shader và sử dụng chương trình shader
    program = InitShader( "vshader1.glsl", "fshader1.glsl" );   // hàm InitShader khai báo trong Angel.h
    glUseProgram( program );

    // Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
    GLuint loc_vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( loc_vPosition );
    glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));


	model_loc = glGetUniformLocation(program, "model");
	projection_loc = glGetUniformLocation(program, "projection");
	view_loc = glGetUniformLocation(program, "view");

	glEnable(GL_DEPTH_TEST);

    glClearColor( 1.0, 1.0, 1.0, 1.0 );        /* Thiết lập màu trắng là màu xóa màn hình*/
}

GLfloat CANH_DAI = 0.4, CANH_RONG = 0.2, CANH_CAO = 0.02;
GLfloat DINH_DAI = 0.05, DINH_RONG = 0.05, DINH_CAO = 0.05;
GLfloat DAY_DAI = 0.05, DAY_RONG = 0.05, DAY_CAO = 0.05;
GLfloat TRUC_DAI = 0.05, TRUC_RONG = 0.04, TRUC_CAO = 0.2;
GLfloat NOI_DAI = 0.05, NOI_RONG = 0.02, NOI_CAO = 0.05;
GLfloat Theta[3] = { 0.0, 0.0, 0.0 };

int xoayquat = 0;
void truc()
{
	instance = Translate(0, DAY_CAO + TRUC_CAO / 2, 0) * Scale(TRUC_DAI, TRUC_CAO, TRUC_RONG);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void dinh()
{
	instance = Translate(0, DAY_CAO + TRUC_CAO + DINH_CAO / 2, 0) * Scale(DINH_DAI, DINH_CAO, DINH_RONG);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void day()
{
	instance = RotateY(xoayquat) * Translate(0, DAY_CAO / 2, 0) * Scale(DAY_DAI * 3, DAY_CAO, DAY_RONG * 3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void canh_1()
{
	instance = RotateY(xoayquat) * Translate(DAY_DAI / 2 + NOI_DAI + CANH_DAI / 2, DAY_CAO / 2, 0)
		* Scale(CANH_DAI, CANH_CAO, CANH_RONG/2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void canh_2()
{
	// xoay quanh trục y 120 + độ xoay quạt
	instance = RotateY(120 + xoayquat) * Translate(DAY_DAI / 2 + NOI_DAI + CANH_DAI / 2, DAY_CAO / 2, 0)
		* Scale(CANH_DAI, CANH_CAO, CANH_RONG/2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void canh_3()
{
	instance = RotateY(240 + xoayquat) * Translate(DAY_DAI / 2 + NOI_DAI + CANH_DAI / 2, DAY_CAO / 2, 0)
		* Scale(CANH_DAI, CANH_CAO, CANH_RONG/2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

bool isSpin = false;


void canhquat()
{
	if (isSpin) {   // nếu đang quay
		xoayquat += 1;
		if (xoayquat > 360) xoayquat = 0;
		canh_1();
		canh_2();
		canh_3();
		glutPostRedisplay();
	}
	else {
		canh_1();
		canh_2();
		canh_3();
	}
}

GLfloat eye_x = 2.0, eye_y = 1.0, eye_z = -1.5;
GLfloat l = -0.8, r = 0.8;
GLfloat bottom = -0.8, top = 0.8;
GLfloat zNear = 1.9, zFar = 40.0;
int quayX = 0;
int quayY = 0;
int quayZ = 0;
float dichx = 0;
float dichy = 0;
float dichz = 0;
void display( void )
{
	
    glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
	truc();
	dinh();
	day();
	canh_1();
	canh_2();
	canh_3();
	canhquat();
	mat4 view = identity();
	point4 eye(eye_x, eye_y, eye_z, 1.0);
	point4 at(0.0, 0.0, 0.0, 1.0);
	vec4 up(0.0, 1.0, 0.0, 0.0);
	view = LookAt(eye, at, up);
	view = view * Translate(dichx, dichy, dichz) * RotateX(quayX) * RotateY(quayY) * RotateZ(quayZ);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, view);


	mat4 p = Frustum(l, r, bottom, top, zNear, zFar);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, p);
	glutSwapBuffers();									   
}


void keyboard( unsigned char key, int x, int y )
{
	// keyboard handler

    switch ( key ) {
		case 033:			// 033 is Escape key octal value
			exit(1);		// quit program
			break;
		case '1':
			xoayquat += 50;
			glutPostRedisplay();
			break;
		case '2':
			isSpin = !isSpin;   // thay đổi giá trị true - false
			glutPostRedisplay();
			break;
		case 'x':
			l *= 1.1f; r *= 1.1f;
			bottom *= 1.1f; top *= 1.1f;
			glutPostRedisplay();
			break;
		case 'X':
			l *= 0.9f; r *= 0.9f;
			bottom *= 0.9f; top *= 0.9f;
			glutPostRedisplay();
			break;
		case 'z':
			zNear *= 1.1f; zFar *= 1.1f;
			glutPostRedisplay();
			break;
		case 'Z':
			zNear *= 0.9f; zFar *= 0.9f;
			glutPostRedisplay();
			break;
		case '7':
			quayX += 5;
			glutPostRedisplay();
			break;
		case '8':
			quayY += 5;
			glutPostRedisplay();
			break;
		case '9':
			quayZ += 5;
			break;
		case 'u':
			dichx += 0.01;
			glutPostRedisplay();
			break;
		case 'U':
			dichx -= 0.01;
			glutPostRedisplay();
			break;
		case 'i':
			dichy += 0.01;
			glutPostRedisplay();
			break;
		case 'I':
			dichy -= 0.01;
			glutPostRedisplay();
			break;
		case 'o':
			dichz += 0.01;
			glutPostRedisplay();
			break;
		case 'O':
			dichz -= 0.01;
			glutPostRedisplay();
			break;
		case ' ':
			// reset values to their defaults
			l = -0.8;
			r = 0.8;
			bottom = -0.8;
			top = 0.8;
			zNear = 1.9;
			zFar = 40.0;
			quayX = 0;
			quayY = 0;
			quayZ = 0;
			dichx = 0;
			dichy = 0;
			dichz = 0;
			glutPostRedisplay();
			break;
    }
}


int main( int argc, char **argv )
{
	// main function: program starts here

    glutInit( &argc, argv );                       
    glutInitDisplayMode( GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowSize( 640, 640 );                 
	glutInitWindowPosition(100,150);               
    glutCreateWindow( "Drawing a Cube" );            

   
	glewInit();										

    generateGeometry( );                          
    initGPUBuffers( );							   
    shaderSetup( );                               

    glutDisplayFunc( display );                   
    glutKeyboardFunc( keyboard );                  

	glutMainLoop();
    return 0;
}
