#include<iostream>
#include<math.h>
#include<stdlib.h>
#include<gl/freeglut.h> //Freeglut library

#define PI 3.14159265

using namespace std;

// Globals.
int isAnimate = 0; // Animated?
int animationPeriod = 50; // Time interval between frames.
float time_param = 0.0; // Time parameter.
float horizontal = 0.5; // Horizontal component of initial velocity.
float vertical = 4.0; // Vertical component of initial velocity.
float gravitational = 0.2;  // Gravitational accelaration.
char theStringBuffer[10]; // String buffer.
long font = (long)GLUT_BITMAP_TIMES_ROMAN_10; // Font selection.

void writeData(void);
void writeBitmapString(void *font, char *string);
void floatToString(char * destStr, int precision, float val);
void display(void);
void animate(int value);
void resize(int w, int h);
void keyInput(unsigned char key, int x, int y);
void specialKeyInput(int key, int x, int y);

//This is the main function!
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(600,600);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("Ball!");

	glClearColor(1.0, 1.0, 1.0, 0.0);
	glutDisplayFunc(display); 
	   glutReshapeFunc(resize);  
	   glutKeyboardFunc(keyInput);
	   glutTimerFunc(5, animate, 1);
	   glutSpecialFunc(specialKeyInput);

	glutMainLoop();

	return 0;
}

// Drawing routine.
void display(void)
{
   glClear (GL_COLOR_BUFFER_BIT);
   glLoadIdentity();

   writeData();

   //This function places the scene in the frustum.
   glTranslatef(-15.0, -15.0, -25.0);

   //Sphere transformation
   glTranslatef(horizontal*time_param, horizontal*time_param - (gravitational/2.0)*time_param*time_param, 0.0);

   //Draws the sphere
   glColor3f(0.5f, 0.3f, 1.0f);
   glutSolidSphere(3.0, 10, 10);

   glutSwapBuffers();
}

// Write data.
void writeData(void)
{
   glColor3f(0.0, 0.0, 0.0);
   
   floatToString(theStringBuffer, 4, horizontal);
   glRasterPos3f(-4.5, 4.5, -5.1);
   writeBitmapString((void*)font, "Horizontal component of initial velocity: ");
   writeBitmapString((void*)font, theStringBuffer);
   
   floatToString(theStringBuffer, 4, vertical);
   glRasterPos3f(-4.5, 4.2, -5.1);
   writeBitmapString((void*)font, "Vertical component of initial velocity: ");  
   writeBitmapString((void*)font, theStringBuffer);

   floatToString(theStringBuffer, 4, gravitational);
   glRasterPos3f(-4.5, 3.9, -5.1);
   writeBitmapString((void*)font, "Gravitation: ");  
   writeBitmapString((void*)font, theStringBuffer);
}

// Routine to draw a bitmap character string.
void writeBitmapString(void *font, char *string)
{  
   char *c;

   for (c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}

// Routine to onvert floating point to char string.
void floatToString(char * destStr, int precision, float val) 
{
   sprintf(destStr,"%f",val);
   destStr[precision] = '\0';
}

// Timer function.
void animate(int value)
{
   if (isAnimate) 
   {
      time_param += 1.0;
   }
   glutTimerFunc(animationPeriod, animate, 1);
   glutPostRedisplay();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
   glViewport(0, 0, (GLsizei)w, (GLsizei)h); 
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);

   glMatrixMode(GL_MODELVIEW);
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
   switch(key) 
   {
      case 27:
         exit(0);
         break;
	  case ' ': 
         if (isAnimate) isAnimate = 0;
		 else isAnimate = 1;
         glutPostRedisplay();
		 break;
	  case 'r':
         isAnimate = 0;
		 time_param = 0.0;
         glutPostRedisplay();
		 break;
      default:
         break;
   }
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
   if(key == GLUT_KEY_UP) vertical += 0.05;;
   if(key == GLUT_KEY_DOWN) if (vertical > 0.1) vertical -= 0.05;
   if(key == GLUT_KEY_RIGHT) horizontal += 0.05; 
   if(key == GLUT_KEY_LEFT) if (horizontal > 0.1) horizontal -= 0.05;  
   if(key == GLUT_KEY_PAGE_UP) gravitational += 0.05;  
   if(key == GLUT_KEY_PAGE_DOWN) if (gravitational > 0.1) gravitational -= 0.05;

   glutPostRedisplay();
}
