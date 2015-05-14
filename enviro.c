/* ENVIRO.C
   Basic environment with two walls, a floor, and a pot.

   Note: To be expanded into a full explorable environment.
   Also note to add two more walls and the ceiling.
*/
	

#include<GL/glut.h>

void wall(double thickness)    // function to create the walls with given thickness
{
    glPushMatrix();
    glTranslated(0.5,0.5*thickness,0.5);
    glScaled(1.0,thickness,1.0);
    glutSolidCube(1.0);
    glPopMatrix();
}

void displaySolid(){
    GLfloat mat_ambient[]={0.7f,0.7f,0.7f,0.1f};
    GLfloat mat_diffuse[]={0.5f,0.5f,0.5f,1.0f};
    GLfloat mat_specular[]={1.0f,1.0f,1.0f,1.0f};
    GLfloat mat_shininess[]={50.0f};
    glMaterialfv(GL_FRONT,GL_AMBIENT,mat_ambient);
    glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
    glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);
    GLfloat light_Intensity[]={0.7f,0.7f,0.7f,1.0f};
    GLfloat light_Position[]={2.0f,6.0f,3.0f,0.0f};
    glLightfv(GL_LIGHT0,GL_POSITION,light_Position);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,light_Intensity);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double winlet=1.0;
    glOrtho(-winlet*64/48,winlet*64/48.0,-winlet*64/48,winlet*64/48,0.6,100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(2.3,1.38,2.0,0.0,0.25,0.0,0.0,1.0,0.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glTranslated(0.08,0.08,0.08);
    glPushMatrix();
    glTranslated(0.6,0.38,0.5);
    glRotated(30,0,1,0);
    glutSolidTeapot(0.08);
    glPopMatrix();
    glPushMatrix();
    glTranslated(0.25,0.42,0.35);
    glPopMatrix();
    glPushMatrix();
    glTranslated(0.4,0,0.4);    
    glPopMatrix();
    wall(0.2);
    glPushMatrix();
    glRotated(-90.0,1.0,0.0,0.0);
    wall(0.02);
    glPopMatrix();
    glRotated(90.0,0.0,0.0,180.0);
    wall(0.02);
    glPopMatrix();
    glFlush();
}

void main(int argc,char **argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(0,0);
    glutCreateWindow("the pot");
    glutDisplayFunc(displaySolid);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glClearColor(0.1,0.1,0.1,0.0);
    glViewport(0,0,640,480);
    glutMainLoop();
}
//end
