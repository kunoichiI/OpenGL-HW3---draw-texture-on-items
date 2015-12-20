//
//  main.cpp
//  opengl hw3
//
//  Created by Mingyuan Wang on 4/13/15.
//  Copyright (c) 2015 Mingyuan Wang. All rights reserved.
//

#include <iostream>
#include <OpenGL/OpenGL.h>
#include <GLUT/GLUT.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Shader.h"
//#include "texture.hpp"
#define BUFFER_OFFSET(x) ((const void*)(x))

using namespace glm;

enum VAO_IDs { Triangles, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0 };

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];

struct tris
{
    GLfloat v0[3];
    GLfloat v1[3];
    GLfloat v2[3];
    GLfloat normal[3][3];
    GLubyte color[3];
};
int NumTris;
GLfloat ambient[3], diffuse[3], specular[3], shine, vertices[36][3], normal[36][3], text_co[36][2], Pv[36][3], Pu[36][3];
GLfloat xmax, ymax, xmin, ymin, zmax, zmin;
tris *Tris;
int axis = 3;
GLfloat size;


void read(char *FileName) {
    int material_count, color_index[3], i; char ch;
    int d;
    //    int NumTris;
    float a,b,c;
    FILE* fp = fopen(FileName,"r");
    if (fp==NULL) { printf("ERROR: unable to open TriObj [%s]!\n",FileName); exit(1); }
    fscanf(fp, "%c", &ch);
    while(ch!= '\n') // skip the first line – object’s name
        fscanf(fp, "%c", &ch); //
    fscanf(fp,"# triangles = %d\n", &NumTris); // read # of triangles
    fscanf(fp,"Material count = %d\n", &material_count); // read material count //
    
    Tris= new tris[NumTris];
    for (i=0; i<material_count; i++) {
        fscanf(fp, "ambient color %f %f %f\n", &ambient[0], &ambient[1], &ambient[2]);
        fscanf(fp, "diffuse color %f %f %f\n", &diffuse[0], &diffuse[1], &diffuse[2]);
        fscanf(fp, "specular color %f %f %f\n", &specular[0], &specular[1], &specular[2]);
        fscanf(fp, "material shine %f\n", &shine);
    } //
    fscanf(fp, "%c", &ch);
    while(ch!= '\n') // skip the first line – object’s name
        fscanf(fp, "%c", &ch); //
    fscanf(fp, "%c", &ch);
    while(ch!= '\n') // skip documentation line
        fscanf(fp, "%c", &ch);
    //
    // allocate triangles for tri model //
    printf ("Reading in %s (%d triangles). . .\n", FileName, NumTris);
    //        GLfloat Tris[NumTris][3][3]; //
    for (i=0; i<NumTris; i++) // read triangles
    {
        fscanf(fp, "v0 %f %f %f %f %f %f %d %f %f\n",
               &vertices[3*i][0], &vertices[3*i][1], &vertices[3*i][2], &normal[3*i][0] , &normal[3*i][1], &normal[3*i][2], &color_index[0], &text_co[3*i][0], &text_co[3*i][1]);
        fscanf(fp, "v1 %f %f %f %f %f %f %d %f %f\n",
               &vertices[3*i+1][0], &vertices[3*i+1][1], &vertices[3*i+1][2], &normal[3*i+1][0] , &normal[3*i+1][1], &normal[3*i+1][2], &color_index[0], &text_co[3*i+1][0], &text_co[3*i+1][1]);
        fscanf(fp, "v2 %f %f %f %f %f %f %d %f %f\n",
               &vertices[3*i+2][0], &vertices[3*i+2][1], &vertices[3*i+2][2], &normal[3*i+2][0] , &normal[3*i+2][1], &normal[3*i+2][2], &color_index[0], &text_co[3*i+2][0], &text_co[3*i+2][1]);
        fscanf(fp, "face normal %f %f %f\n", &a , &a, &a);
        
    }
    fclose(fp);
}

GLuint load(const char * imagepath){
    
    
    // Data read from the header of the BMP file
    unsigned char header[54];
    unsigned int dataPos;
    unsigned int imageSize;
    unsigned int width, height;
    // Actual RGB data
    unsigned char * data;
    
    // Open the file
    FILE * file = fopen(imagepath,"rb");
    if (!file)							    {printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar(); return 0;}
    
    // Read the header, i.e. the 54 first bytes
    
    // If less than 54 bytes are read, problem
    if ( fread(header, 1, 54, file)!=54 ){
        printf("Not a correct BMP file\n");
        return 0;
    }
    // A BMP files always begins with "BM"
    if ( header[0]!='B' || header[1]!='M' ){
        printf("Not a correct BMP file\n");
        return 0;
    }
    // Make sure this is a 24bpp file
    if ( *(int*)&(header[0x1E])!=0  )         {printf("Not a correct BMP file\n");    return 0;}
    if ( *(int*)&(header[0x1C])!=24 )         {printf("Not a correct BMP file\n");    return 0;}
    
    // Read the information about the image
    dataPos    = *(int*)&(header[0x0A]);
    imageSize  = *(int*)&(header[0x22]);
    width      = *(int*)&(header[0x12]);
    height     = *(int*)&(header[0x16]);
    size = 1.0/(width * height);
    // Some BMP files are misformatted, guess missing information
    if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
    if (dataPos==0)      dataPos=54; // The BMP header is done that way
    
    // Create a buffer
    data = new unsigned char [imageSize];
    
    // Read the actual data from the file into the buffer
    fread(data,1,imageSize,file);
    // Everything is in memory now, the file wan be closed
    fclose (file);
    
    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    
    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    
    // OpenGL has now copied the data. Free our own version
    delete [] data;
    
    printf("width: %d, height: %d", width, height);
    // Poor filtering, or ...
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    // ... nice trilinear filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Return the ID of the texture we just created
    return textureID;
}


GLuint arrayID;
GLuint vertexbuffer;
GLuint textcobuffer;
GLuint Pubuffer;
GLuint Pvbuffer;
GLfloat ldr[3] = {3, 5, 5}, ld[3] = {0.5, 0.5, 0.5};
GLuint bump, tex;
int t = 0;
int l = 1;
int b = 0;
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 0);
    glGenVertexArrays(1, &arrayID);
    glBindVertexArray(arrayID);
    read("/Users/Stephanie/Documents/Xcode_projects/opengl\ hw3/opengl hw3/cube_texture.in");
    
    for(int i = 0; i < 6; i++)
    {
        for(int j = 0; j < 6; j++)
        {
            for(int k = 0; k < 6; k++)
            {
                if( j != k)
                {
                    if(text_co[6 * i + j][1] == text_co[6 * i + k][1] && text_co[6 * i + j][0] != text_co[6 * i + k][0])
                    {
                        Pu[6 * i + j][0] = (vertices[6 * i + j][0] - vertices[6 * i + k][0]) / (text_co[6 * i + j][0] - text_co[6 * i + k][0]);
                        Pu[6 * i + j][1] = (vertices[6 * i + j][1] - vertices[6 * i + k][1]) / (text_co[6 * i + j][0] - text_co[6 * i + k][0]);
                        Pu[6 * i + j][2] = (vertices[6 * i + j][2] - vertices[6 * i + k][2]) / (text_co[6 * i + j][0] - text_co[6 * i + k][0]);
                    }
                    if(text_co[6 * i + j][0] == text_co[6 * i + k][0] && text_co[6 * i + j][1] != text_co[6 * i + k][1])
                    {
                        Pv[6 * i + j][0] = (vertices[6 * i + j][0] - vertices[6 * i + k][0]) / (text_co[6 * i + j][1] - text_co[6 * i + k][1]);
                        Pv[6 * i + j][1] = (vertices[6 * i + j][1] - vertices[6 * i + k][1]) / (text_co[6 * i + j][1] - text_co[6 * i + k][1]);
                        Pv[6 * i + j][2] = (vertices[6 * i + j][2] - vertices[6 * i + k][2]) / (text_co[6 * i + j][1] - text_co[6 * i + k][1]);
                    }
                    
                }
            }
        }
    }
    
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
                 vertices, GL_STATIC_DRAW);
    //    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    //    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(color), color);
    
    glGenBuffers(1, &textcobuffer);
    glBindBuffer(GL_ARRAY_BUFFER, textcobuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(text_co), text_co, GL_STATIC_DRAW);
    glGenBuffers(1, &Pubuffer);
    glBindBuffer(GL_ARRAY_BUFFER, Pubuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Pu), Pu, GL_STATIC_DRAW);
    glGenBuffers(1, &Pvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, Pvbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Pv), Pv, GL_STATIC_DRAW);
    
    ShaderInfo shaders[] = {
        { GL_VERTEX_SHADER, "/Users/Stephanie/Documents/Xcode_projects/opengl\ hw3/opengl hw3/simple.vert" },
        { GL_FRAGMENT_SHADER, "/Users/Stephanie/Documents/Xcode_projects/opengl\ hw3/opengl hw3/simple.frag" },
        { GL_NONE, NULL }
    };
    
    GLuint program = LoadShaders(shaders);
    glUseProgram(program);
    GLuint MVPID = glGetUniformLocation(program, "MVP");
    GLuint ViewID = glGetUniformLocation(program, "V");
    mat4 Projection = perspective(45.0f, 1.0f, 0.1f, 100.0f);
    mat4 View = lookAt(vec3(0,0,1), vec3(0,0,0), vec3(0,1,0));
    mat4 Model = mat4(1.0);
    mat4 MVP = Projection * View * Model;
    glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ViewID, 1, GL_FALSE, &View[0][0]);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, textcobuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, Pubuffer);
    glVertexAttribPointer(2, 3, GL_FLOAT,
                          GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, Pvbuffer);
    glVertexAttribPointer(3, 3, GL_FLOAT,
                          GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(3);
    
    glEnable(GL_TEXTURE_2D);
    
    
    
    
    
    glActiveTexture(GL_TEXTURE1);
    GLuint bumpID = glGetUniformLocation(program, "bump");
    glBindTexture(GL_TEXTURE_2D, bump);
    glUniform1i(bumpID, 1);
    
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    GLuint texID = glGetUniformLocation(program, "tex");
    glUniform1i(texID, 0);
    
    
    GLuint LightDiffuse = glGetUniformLocation(program, "light.diffuse");
    GLuint LightPosition = glGetUniformLocation(program, "light.position");
    glUniform3f(LightDiffuse, ld[0], ld[1], ld[2]);
    glUniform3f(LightPosition,ldr[0], ldr[1], ldr[2]);
    GLuint US = glGetUniformLocation(program, "u");
    glUniform1f(US, size);
    GLuint T = glGetUniformLocation(program, "t");
    glUniform1i(T, t);
    GLuint L = glGetUniformLocation(program, "l");
    glUniform1i(L, l);
    GLuint B = glGetUniformLocation(program, "b");
    glUniform1i(B, b);
    
    
    /*
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     GLfloat diffuse[] = {0.5, 0.5, 0.5};
     glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
     GLfloat lightposition[] = {-1, -1, -1, 0};
     glLightfv(GL_LIGHT0, GL_POSITION, lightposition);
     */
    glEnable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, NumTris*3);
    
    glutSwapBuffers();
    
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == 'q' || key == 'Q') exit(0);
    if (key == 'x')
        axis = 0;
    if (key == 'y')
        axis = 1;
    if (key == 'z')
        axis = 2;
    if (key == 'r')
        axis = 3;
    if (key == 'g')
        axis = 4;
    if (key == 'b')
        axis = 5;
    if (key == '>') {
        if (axis < 3) {
            ldr[axis] += 0.5;
        }
        else{
            if(axis < 6)
                ld[axis - 3] += 0.1;
        }
    }
    if (key == '<') {
        if (axis < 3) {
            ldr[axis] -= 0.5;
        }
        else{
            if(axis < 6)
                ld[axis - 3] -= 0.1;
        }
    }
    if (key == '3')
        bump = load("/Users/Stephanie/Documents/Xcode_projects/opengl\ hw3/opengl hw3/bump_circle.bmp");
    
    if(key == '4')
        bump = load("/Users/Stephanie/Documents/Xcode_projects/opengl\ hw3/opengl hw3/orange2.bmp");
    if(key == '0')
    {
        if( l == 1)
            l = 0;
        else
            l = 1;
    }
    if(key == '1')
    {
        if( t == 1)
            t = 0;
        else
            t = 1;
    }
    if(key == '2')
    {
        if( b == 1)
            b = 0;
        else
            b = 1;
    }
    glutPostRedisplay();
    
}

/*void display(void)
 {
 //    glBindVertexArray(arrayID);
 glShadeModel(GL_SMOOTH);
 
 }*/

int main(int argc, char * argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow(argv[0]);
    
    tex = load("/Users/Stephanie/Documents/Xcode_projects/opengl\ hw3/opengl hw3/texture_wall.bmp");
    
    
    bump = load("/Users/Stephanie/Documents/Xcode_projects/opengl\ hw3/opengl hw3/orange2.bmp");
    
    
    /*
     * Normally don't need GLEW on MacOSX.
     * But set glewExperimental to TRUE if you want to use it.
     * Otherwise some funcs like "glGenVertexArray" will be NULL.
     */
    /* glewExperimental = GL_TRUE;
     if (glewInit()) {
     cerr << "Unable to initialize GLEW... exiting" << endl;
     exit(EXIT_FAILURE);
     } */
    
    //    cout << "Supported GLSL version is: " <<
    //    glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
    
    //    init(1);
    
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    
    glutMainLoop();
}