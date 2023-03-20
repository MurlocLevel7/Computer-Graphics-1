#include <windows.h>
#include <gl/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"
#include "stb-master/stb_easy_font.h"

#define nameLen 20

typedef void (*TTexProc) (unsigned char *data, int width, int height, int cnt);

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

int textureId;
char anim=0;
float TypeOfAnim=0;
float tick=0;
float mouseX,mouseY;

typedef struct
{
    char name[nameLen];
    float vert[8];
    char isHover,isDown;
    float buffer[50*nameLen];
    int num_quads;
    float textPosX,textPosY,textScale;
} TBtn;
TBtn *btn = 0;
int btnCnt = 0;

int Menu_AddButton(char *name, float x, float y, float width, float height, float textScale)
{
    btnCnt++;
    btn = realloc(btn, sizeof(btn[0]) * btnCnt);

    snprintf(btn[btnCnt-1].name, nameLen, "%s", name);
    float *vert = btn[btnCnt-1].vert;
    vert[0]=vert[6]=x;
    vert[2]=vert[4]=x+width;
    vert[1]=vert[3]=y;
    vert[5]=vert[7]=y+height;
    btn[btnCnt-1].isHover=0;
    btn[btnCnt-1].isDown=0;

    TBtn *b = btn + btnCnt-1;
    b->num_quads = stb_easy_font_print(0,0, name, 0, b->buffer, sizeof(b->buffer));
    b->textPosX = x + (width - stb_easy_font_width(name)*textScale) /2.0;
    b->textPosY = y + (height - stb_easy_font_height(name)*textScale) /2.0;
    b->textPosY += textScale*2;
    b->textScale = textScale;

    return btnCnt-1;
}

void ShowButton(int buttonId)
{
    TBtn btn1=btn[buttonId];
    glVertexPointer(2,GL_FLOAT,0,btn1.vert);
    glEnableClientState(GL_VERTEX_ARRAY);
        if(btn1.isDown) glColor3f(0.88,0.24,0.24);
        else if(btn1.isHover)   glColor3f(0.72,0.6,0.48);
        else glColor3f(0.8,0.8,0.8);
        glDrawArrays(GL_TRIANGLE_FAN,0,4);
        glColor3f(1,1,1);
        glLineWidth(1);
        glDrawArrays(GL_LINE_LOOP,0,4);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPushMatrix();
        glColor3f(0,0,0);
        glTranslatef(btn1.textPosX, btn1.textPosY, 0);
        glScalef(btn1.textScale, btn1.textScale, 1);
        glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(2,GL_FLOAT,16,btn1.buffer);
            glDrawArrays(GL_QUADS,0,btn1.num_quads*4);
        glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix();
}

void Texture_LoadTexture(char *file_name, int *textureId, int wrap, int filter, TTexProc proc)
{
    int width,height,cnt;
    unsigned char *data = stbi_load(file_name, &width,&height,&cnt,0);
    if(!data)   printf("%s \"%s\" %s\n","File",file_name, "not found.");
    if(proc) proc(data,width,height,cnt);
    glGenTextures(1, textureId);
    glBindTexture(GL_TEXTURE_2D, *textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,cnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D,0);
    stbi_image_free(data);
}


void InitMain()
{
    anim=0;
    btnCnt=0;
    free(btn);
    btn=0;
    Menu_AddButton("Game",10,10,300,90,6);
    Menu_AddButton("Test",10,110,300,90,6);
    Menu_AddButton("Quit",10,210,300,90,6);
}

void InitTest()
{
    tick=0;
    TypeOfAnim=0;
    anim =1;
    btnCnt=0;
    free(btn);
    btn=0;
    Menu_AddButton("Anim1",10,10,300,90,6);
    Menu_AddButton("Anim2",10,110,300,90,6);
    Menu_AddButton("Anim3",10,210,300,90,6);
    Menu_AddButton("Back",10,310,300,90,6);
}


void MouseDown()
{
    int downBtn = -1;
    for (int i=0;i<btnCnt;i++)
    {
        float *vert=btn[i].vert;
        if ((mouseX>vert[0])&&(mouseY>vert[1])&&(mouseX<vert[4])&&(mouseY<vert[5]))
        {
            btn[i].isDown = 1;
            downBtn = i;
        }
    }
    int buttonId = downBtn;
    if (buttonId<0) return;
    char *name = btn[buttonId].name;
    if (strcmp(name,"Quit")==0) PostQuitMessage(0);
    if (strcmp(name,"Test")==0) InitTest();
    if (strcmp(name,"Back")==0) InitMain();
    if (strcmp(name,"Anim1")==0)
    {
        TypeOfAnim=0;
        tick=0;
    }
    if (strcmp(name,"Anim2")==0)
    {
        TypeOfAnim=1;
        tick=0;
    }
    if (strcmp(name,"Anim3")==0)
    {
        TypeOfAnim=2;
        tick=0;
    }
}

float TextureVertex[] = {320,10,620,10,620,316,320,316};
float TextureCord[] = {0,0,1,0,1,1,0,1};

void ShowTexture(float NumberOfSprite, float TypeOfAnim)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,textureId);

    glColor3f(1,1,1);
    glPushMatrix();
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        static float spriteXsize=3200; //переменные с размерами текстуры и отдельного кадра
        static float spriteYsize=1224;
        static float charsizey=408;
        static float charsizex=400;
        float left=(charsizex*NumberOfSprite)/spriteXsize; //вычисление координат кадра на изображении от
        float right=left+(charsizex/spriteXsize); //номера кадра
        float top=(charsizey*TypeOfAnim)/spriteYsize;
        float bottom=top+(charsizey/spriteYsize);

        TextureCord[5]=TextureCord[7]=bottom;
        TextureCord[1]=TextureCord[3]=top;
        TextureCord[0]=TextureCord[6]=left;
        TextureCord[2]=TextureCord[4]=right;
        glVertexPointer(2,GL_FLOAT,0,TextureVertex);
        glTexCoordPointer(2,GL_FLOAT,0,TextureCord);
        glDrawArrays(GL_TRIANGLE_FAN,0,4);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glPopMatrix();
}


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          1024,
                          1024,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    Texture_LoadTexture("Gebura.png",&textureId,GL_REPEAT,GL_NEAREST, NULL);

    RECT rct;
    GetClientRect(hwnd, &rct);
    glOrtho(0, rct.right, rct.bottom, 0, 1,-1);

    InitMain();

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */

            glClearColor(0.4f, 0.16f, 0.04f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            if (anim)
                ShowTexture(tick,TypeOfAnim);

            for (int i=0;i<btnCnt;i++) ShowButton(i);

            SwapBuffers(hDC);
            tick++;
            if (tick>7)    tick=0;
            Sleep (100);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_MOUSEMOVE:
            mouseX = LOWORD(lParam);
            mouseY = HIWORD(lParam);
            int moveBtn = -1;
            for (int i=0;i<btnCnt;i++)
            {
                float *vert=btn[i].vert;
                if ((mouseX>vert[0])&&(mouseY>vert[1])&&(mouseX<vert[4])&&(mouseY<vert[5]))
                {
                    btn[i].isHover = 1;
                    moveBtn = i;
                }
                else
                {
                    btn[i].isHover=0;
                    btn[i].isDown=0;
                }
            }
        break;

        case WM_LBUTTONDOWN:
            MouseDown();
        break;

        case WM_LBUTTONUP:
            for (int i=0; i<btnCnt;i++)
            btn[i].isDown=0;
        break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

