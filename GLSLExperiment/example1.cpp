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
GLuint model_loc;
mat4 model;
mat4 instance;
mat4 transl;
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
	model_loc = glGetUniformLocation(program, "Model");
	glEnable(GL_DEPTH_TEST);
    glClearColor( 1.0, 1.0, 1.0, 1.0 );        /* Thiết lập màu trắng là màu xóa màn hình*/
}
mat4 quay_tua_lung;
mat4 quay_ghe;
mat4 nang_ghe;
mat4 di_chuyen_ghe;
GLfloat theta[16];
void Ghe(GLfloat TranX, GLfloat TranY, GLfloat TranZ, GLfloat width, GLfloat height, GLfloat depth)
{
	mat4 instance = Translate(TranX, TranY, TranZ) * quay_tua_lung * Scale(width, height, depth);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * transl * di_chuyen_ghe * nang_ghe * quay_ghe * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

GLfloat de_ghe_x = 0.1f, de_ghe_y = 0.015f, de_ghe_z = 0.1f;
GLfloat chan_ghe_x = 0.015f, chan_ghe_y = 0.1f, chan_ghe_z = 0.015f;

GLfloat than_de_x = 0.15f, than_de_y = 0.015f, than_de_z = 0.12f;
GLfloat tua_lung_x = 0.15f, tua_lung_y = 0.2f, tua_lung_z = 0.015f;
GLfloat tua_tay_x = 0.02f, tua_tay_y = 0.02f, tua_tay_z = 0.1f;
void Build_Ghe()
{
	GLfloat x = 0.0f, y = 0.5 * de_ghe_y, z = 0.0f;
	di_chuyen_ghe = Translate(theta[9] + theta[10], 0.0f, theta[11] + theta[12]); //  cái theta trong keyboard này
	// đế ghế (đáy ghế) -> cha
	Ghe(x, y, z, de_ghe_x, de_ghe_y, de_ghe_z);

	// chân ghế  -> con của thằng đáy ghế
	x = 0.0f, y = de_ghe_y + 0.5 * chan_ghe_y, z = 0.0f;
	Ghe(x, y, z, chan_ghe_x, chan_ghe_y, chan_ghe_z);

	// code quay ghế 
	quay_ghe = Translate(0.0f, (de_ghe_y + chan_ghe_y + 0.5 * than_de_y), 0.0f)
		* RotateY(theta[7]) * RotateY(theta[8]) * Translate(0.0f, -(de_ghe_y + chan_ghe_y + 0.5 * than_de_y), 0.0f);

	// đế ngồi --> con của thằng chân ghế, cháu của đáy ghế
	x = 0.0f, y = de_ghe_y + chan_ghe_y + 0.5 * than_de_y, z = 0.0f;
	Ghe(x, y, z, than_de_x, than_de_y, than_de_z);

	// tựa lưng  -> con của thằng chân ghế, cháu của đáy ghế
	quay_tua_lung = Translate(0.0f, -0.5 * tua_lung_y, 0.0f) * RotateX(20) * Translate(0.0f, 0.5 * tua_lung_y, 0.0f);
	x = 0.0f, y = de_ghe_y + chan_ghe_y + 0.5 * than_de_y + 0.5 * tua_lung_y, z = 0.5 * de_ghe_z;
	Ghe(x, y, z, tua_lung_x, tua_lung_y, tua_lung_z);
	quay_tua_lung = identity();

	// tựa tay -->con của thằng chân ghế, cháu của đáy ghế
	x = 0.5 * tua_lung_x - 0.5 * tua_tay_x, y = de_ghe_y + chan_ghe_y + than_de_y + 0.3 * tua_lung_y, z = tan(M_PI / 9) * (0.3 * tua_lung_y);
	Ghe(x, y, z, tua_tay_x, tua_tay_y, tua_tay_z);

	Ghe(-x, y, z, tua_tay_x, tua_tay_y, tua_tay_z);
	quay_ghe = identity();
	nang_ghe = identity();
}

void display( void )
{
	
    glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );                
	Build_Ghe();
	glutSwapBuffers();									   
}


void keyboard( unsigned char key, int x, int y )
{
	// keyboard handler

    switch ( key ) {
    case 033:			// 033 is Escape key octal value
        exit(1);		// quit program
        break;
	case 'q':
		//di chuyển sang trái
		theta[9] -= 0.05f;
		glutPostRedisplay();
		break;
	case 'w':
		//di chuyển sang phải
		theta[10] += 0.05f;
		glutPostRedisplay();
		break;
	case 'e':
		//di chuyển về phía trước
		theta[11] -= 0.05f;
		glutPostRedisplay();
		break;
	case 'r':
		//di chuyển ra sau
		theta[12] += 0.05f;
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
