//
//  main.cpp
//  LearnOpenGL
//
//  Created by changbo on 2019/10/12.
//  Copyright © 2019 CB. All rights reserved.
//

#include "GLTools.h"
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLGeometryTransform.h"
#include "StopWatch.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif


GLShaderManager      shaderManager;
//模型视图矩阵堆栈
GLMatrixStack        modelViewMatrix;

//投影视图矩阵堆栈
GLMatrixStack        projectionMatrix;

//观察者位置
GLFrame              cameraFrame;

//世界坐标位置
GLFrame              objectFrame;

//视景体，用来构造投影矩阵
GLFrustum            viewFrustum;

//三角形批次类
GLTriangleBatch      CC_Triangle;

//球
GLTriangleBatch      sphereBatch;
//环
GLTriangleBatch      torusBatch;
//圆柱
GLTriangleBatch      cylinderBatch;
//锥
GLTriangleBatch      coneBatch;
//磁盘
GLTriangleBatch      diskBatch;

//管道，用来管理投影视图矩阵堆栈和模型视图矩阵堆栈的
GLGeometryTransform    transformPipeline;

// 木色
GLfloat vBurlywood[] = {0.87f, 0.72f, 0.53f, 1.0f};
// 黑色
GLfloat vBlack[] = {0.0f, 0.0f, 0.0f, 1.0f};

//空格的标记
int nStep = 0;



// 将上下文中，进行必要的初始化
void SetupRC()
{
    // 固定管线--用存储着色器
    // 可编程管线--用相关的顶点或片源着色器
    
    
    // 设置背景颜色
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    
    // 初始化固定着色器
    shaderManager.InitializeStockShaders();
    
    //开启深度测试
    glEnable(GL_DEPTH_TEST);
    
    //通过变换管道来管理矩阵堆栈
    //参数一：模型视图矩阵，参数二：投影矩阵
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
    
    // 移动观察者
    cameraFrame.MoveForward(-15.0f);
    
    //利用三角形批次类绘制图形
    
    //球
    //参数，sphereBatch：三角形批次类，fRadius：球体半径，
    // iSlices：球体底部堆积的三角形数量，iStacks：围绕球体排列的三角形对数 （一般建议：iStacks=2*iSlices）
    gltMakeSphere(sphereBatch, 3.0, 20, 40);
    
    // 游泳圈
    gltMakeTorus(torusBatch, 3.0f, 0.75f, 15, 15);
    
    // 圆柱体
    // 参数 baseRadius：底部半径,topRadius:顶部半径，fLength：长度
    // numSlices：围绕z轴三角形对数   numStacks：堆叠三角形个数
    // 圆柱体，从0开始向Z轴正方向延伸。
    // 圆锥体，是一端的半径为0，另一端半径可指定。
    gltMakeCylinder(cylinderBatch, 2.0f, 2.0f, 3.0f, 15, 2);
    
    // 圆锥
    gltMakeCylinder(coneBatch, 2.0f, 0.0f, 3.0f, 12, 2);
    
    //磁盘
    gltMakeDisk(diskBatch, 1.0f, 3.0f, 12, 2);
    
}


void DrawWireFramedBatch(GLTriangleBatch* pBatch)
{
    //平面着色器
    shaderManager.UseStockShader(GLT_SHADER_FLAT,transformPipeline.GetModelViewProjectionMatrix(),vBurlywood);
    
    pBatch->Draw();
    
    //画黑色边框
    glPolygonOffset(-1.0f, -1.0f);
    //开启线圆滑
    glEnable(GL_LINE_SMOOTH);
    //开启混合功能
    glEnable(GL_BLEND);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnable(GL_POLYGON_OFFSET_LINE);
    
    //线框模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    glLineWidth(0.5f);
    
    shaderManager.UseStockShader(GLT_SHADER_FLAT,transformPipeline.GetModelViewProjectionMatrix(),vBlack);
    
    pBatch->Draw();
    
    //恢复
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glLineWidth(1.0f);
    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);
    
}


//召唤场景
void RenderScene(void)
{
    // 清理屏幕(不知道的情况下 就都清楚掉)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    //压栈
    modelViewMatrix.PushMatrix();
    
    //获取摄像头观察者矩阵
    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);//(相当于给mCamera赋值)
    modelViewMatrix.MultMatrix(mCamera);
    
    //获取世界坐标系的位置变换矩阵
    M3DMatrix44f mObjectFrame;
    //从 objectFrame 获取矩阵到 mObjectFrame
    objectFrame.GetMatrix(mObjectFrame);
    //模型视图
    modelViewMatrix.MultMatrix(mObjectFrame);
    
    //判断目前绘制的是第几个图形
    switch (nStep) {
        case 0:
            DrawWireFramedBatch(&sphereBatch); // 球
            break;
        case 1:
            DrawWireFramedBatch(&torusBatch);  // 环
            break;
        case 2:
            DrawWireFramedBatch(&cylinderBatch); // 圆柱
            break;
        case 3:
            DrawWireFramedBatch(&coneBatch);    // 锥
            break;
        case 4:
            DrawWireFramedBatch(&diskBatch);    // 磁盘
            break;
        default:
            break;
    }
    
    // 画完之后pop出去，不影响下一个
    modelViewMatrix.PopMatrix();
    glutSwapBuffers();
}


void ChangeSize(int w, int h)
{
    // 设置窗口
    glViewport(0, 0, w, h);
    
    // 透视投影 SetPerspective
    // 参数 fFov：视觉角度, fAspect：纵横比w/h, fNear fFar
    viewFrustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 500.0f);
    
    // 往矩阵堆栈中添加透视投影矩阵
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    // 模型视图矩阵--(旋转，平移),先加载一个单元矩阵
    modelViewMatrix.LoadIdentity();
    
}


//上下左右，移动图形
void SpecialKeys(int key, int x, int y)
{
    if(key == GLUT_KEY_UP)
        //移动世界坐标系，而不是去移动物体。
        //将世界坐标系在X方向移动-5.0
        objectFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_DOWN)
        objectFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_LEFT)
        objectFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
    
    if(key == GLUT_KEY_RIGHT)
        objectFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
    
    glutPostRedisplay();
}




//点击空格，切换渲染图形
void KeyPressFunc(unsigned char key, int x, int y)
{
    if(key == 32)
    {
        nStep++;
        
        if(nStep > 4)
            nStep = 0;
    }
    
    switch(nStep)
    {
        case 0:
            glutSetWindowTitle("Sphere");
            break;
        case 1:
            glutSetWindowTitle("Torus");
            break;
        case 2:
            glutSetWindowTitle("Cylinder");
            break;
        case 3:
            glutSetWindowTitle("Cone");
            break;
        case 4:
            glutSetWindowTitle("Disk");
            break;
    }
    
    glutPostRedisplay();
}




int main(int argc, char* argv[])
{
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Sphere");
    glutReshapeFunc(ChangeSize);
    glutKeyboardFunc(KeyPressFunc);
    glutSpecialFunc(SpecialKeys);
    glutDisplayFunc(RenderScene);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    SetupRC();
    
    glutMainLoop();
    return 0;
}
