#include <windows.h>
#include <gl/gl.h>
#include <string.h>
#include "menu.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"
#include "texture.h"

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

float height=1024;
float width=1024;
int textureId;
char anim=0;
char Game=0;
char test=0;
char TextureReflection=0;
float TypeOfAnim=0;
float NumbOfFrame=0;
float currentframe=0;

float TextureVertex[] = {320,10,620,10,620,316,320,316};
float TextureCord[] = {320,10,620,10,620,316,320,316};

float speed = 2.5;
float gravity = 0.2;
int State=1;
typedef struct
{
    float x, y, dx, dy; //обе координаты и скорость изменения
} Hero;

Hero Gebura;

void Hero_Init(Hero *obj, float x1, float y1, float dx1, float dy1)
{
    obj->x=x1;
    obj->y=y1;
    obj->dx=dx1;
    obj->dy=dy1;
}

void Heero_Show(Hero obj)
{
    float HeroVertex[] = {obj.x-100,obj.y-100,obj.x+100,obj.y-100,obj.x+100,obj.y+100,obj.x-100,obj.y+100};
    ShowTexture(HeroVertex);
}

void Reflect (float *da, float *a, BOOL cond, float wall)
{
    if (!cond) return;
    *da *= -0.1;
    *a = wall;
}

void Hero_Move(Hero *obj)
{
    obj->dy+=gravity;
    if (GetKeyState(VK_LEFT)<0 && State==1)
    {
        TextureReflection=1;
        TypeOfAnim=0;
        currentframe+=0.15;
        obj->dx-=speed;
        obj->x+=obj->dx;
        Reflect(&obj->dx, &obj->x, (obj->x-100 < 0), 100);
        obj->dx=0;
        if (currentframe>8) currentframe-=7;
    }
    if (GetKeyState(VK_RIGHT)<0 && State==1)
    {
        TextureReflection=0;
        TypeOfAnim=0;
        currentframe+=0.15;
        obj->dx+=speed;
        obj->x+=obj->dx;
        Reflect(&obj->dx, &obj->x, (obj->x+100 > width), width-100);
        obj->dx=0;
        if (currentframe>8) currentframe-=7;
    }
    if (GetKeyState(VK_UP)<0 && (obj->y>452) && State==1)
    {
        TypeOfAnim=1;
        if (currentframe<4)currentframe+=0.15;
        currentframe+=0.15;
        obj->dy =speed*1.2;
        obj->y-=obj->dy;
        //if (currentframe>8) currentframe-=7;
    }
    if(!(GetKeyState(VK_UP)<0) && (obj->y<512))
    {
        if (obj->y>452) State=1;
        obj->dy =speed*1.2;
        obj->y+=obj->dy;
        if (currentframe>0) currentframe-=0.15;
    }
    if(!(GetKeyState(VK_UP)<0) && (obj->y==512) && !(GetKeyState(VK_RIGHT)<0) && !(GetKeyState(VK_LEFT)<0))
    {
        TypeOfAnim=2;
        currentframe+=0.15;
        if (currentframe>8) currentframe-=7;
    }
}

void InitButton()
{
    ChooseBTN(0,3);
    Menu_AddButton("start",10,10,300,90,6);
    Menu_AddButton("test",10,110,300,90,6);
    Menu_AddButton("quit",10,210,300,90,6);
    Menu_AddButton("anim1",10,10,300,90,6);
    Menu_AddButton("anim2",10,110,300,90,6);
    Menu_AddButton("anim3",10,210,300,90,6);
    Menu_AddButton("back",10,310,300,90,6);
    Menu_AddButton("back",0,0,300,90,6);
}

void InitMain()
{
    ChooseBTN(0,3);
    anim=0;
    TypeOfAnim=0;
    currentframe=0;
    test=0;
    Game=0;
}

void InitTest()
{
    ChooseBTN(3,7);
    currentframe=0;
    TypeOfAnim=0;
    anim=1;
    test=1;
    Game=0;
}

void InitGame()
{
    Hero_Init(&Gebura,height/2,width/2,0.5,0.5);
    ChooseBTN(7,8);
    currentframe=0;
    TypeOfAnim=0;
    anim=1;
    test=0;
    Game=1;
}

void MouseDown()
{
    int buttonId = Menu_MouseDown();
    if (buttonId<0) return;
    char *name = Menu_GetButtonName(buttonId);
    if (strcmp(name,"quit")==0) PostQuitMessage(0);
    if (strcmp(name,"test")==0) InitTest();
    if (strcmp(name,"start")==0) InitGame();
    if (strcmp(name,"back")==0)
    {
        InitMain();
    }
    if (strcmp(name,"anim1")==0)
    {
        TypeOfAnim=0;
        currentframe=0;
    }
    if (strcmp(name,"anim2")==0)
    {
        TypeOfAnim=1;
        currentframe=0;
    }
    if (strcmp(name,"anim3")==0)
    {
        TypeOfAnim=2;
        currentframe=0;
    }
}

void ShowTexture(TextureVertex)
{
    float NumberOfSprite=round(currentframe);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);//
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

        if (!TextureReflection)
        {
            TextureCord[5]=TextureCord[7]=bottom;
            TextureCord[1]=TextureCord[3]=top;
            TextureCord[0]=TextureCord[6]=right;
            TextureCord[2]=TextureCord[4]=left;
        }
        else
        {
            TextureCord[5]=TextureCord[7]=bottom;
            TextureCord[1]=TextureCord[3]=top;
            TextureCord[0]=TextureCord[6]=left;
            TextureCord[2]=TextureCord[4]=right;
        }
        glVertexPointer(2,GL_FLOAT,0,TextureVertex);
        glTexCoordPointer(2,GL_FLOAT,0,TextureCord);
        glDrawArrays(GL_TRIANGLE_FAN,0,4);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//
        glDisable(GL_BLEND);
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
                          height,
                          width,
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

    InitButton();

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

            glClearColor(0.4f, 0.4f, 0.4f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            Menu_ShowMenu();
            if (anim)
            {
                if (test)
                {
                    ShowTexture(TextureVertex);
                    currentframe+=0.25;
                    if (currentframe>7)    currentframe=0;
                }
                else    if(Game)
                {
                    Hero_Move(&Gebura);
                    Heero_Show(Gebura);
                }
            }


            SwapBuffers(hDC);
            Sleep (1);
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
            Menu_MouseMove(LOWORD(lParam),HIWORD(lParam));
        break;

        case WM_LBUTTONDOWN:
            MouseDown();
        break;

        case WM_LBUTTONUP:
            Menu_MouseUp();
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

