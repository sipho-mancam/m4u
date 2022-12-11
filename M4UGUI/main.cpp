#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include "GUIOBJECTS.h"
#include <iostream>

using namespace std;

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
char* ConvertTextToANS(const wchar_t *data);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("Code::Blocks Template Windows App"),       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           544,                 /* The programs width */
           375,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */
int ID = 0;
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND but1;

    static NavPane navBar(10, 10,544*0.3, 330, 0);
    static Display dispSec(554*0.3, 10, 500,285, 1);
    static EditSection editBar(544*0.31, 290, 500,330, 0, hwnd);
    static TextBase *textHead = NULL, *temp=NULL;
    static wchar_t temptxt[1500];
    static RECT wordRect, winRect;
    static char ttext[10000] = "Hello there, I'm sthe I want to make this string relatively long ";
    static HDC hdc;

    static int nRet;
    switch (message)                  /* handle the messages */
    {
    case WM_CREATE:
        but1 = CreateWindow("button", "ClickMe",WS_CHILD|WS_VISIBLE, 10, 10, 70, 20, hwnd, (HMENU)0, NULL, NULL);
        editBar.init(hwnd);

        break;
    case WM_COMMAND:
        if(HIWORD(wParam)== BN_CLICKED)
        {

            switch(LOWORD(wParam))
            {
            case 0:
//                GetWindowRect(hwnd, &winRect);
//                cout<<winRect.left<<endl<<winRect.top<<endl<<winRect.right<<endl<<winRect.bottom<<endl;
                editBar.showMe(hwnd);
                dispSec.ShowMe(hwnd);
                navBar.showMe(hwnd);
                break;
            case SEND_BUTTON:
                editBar.getWinText(temptxt);
                dispSec.getWindRect(&wordRect);
                if(!textHead && temptxt && wcscmp(temptxt, L"Type message here")!= 0)
                {
                    textHead = new TextBase(178, 40, ID, FALSE);
                    textHead->writeText(temptxt);
                    ID++;
                }
                else if(temptxt && wcscmp(temptxt, L"Type message here")!= 0)
                {
                    textHead->getRect(&wordRect, 500/9, hwnd);
                    temp = new TextBase(178, wordRect.bottom+25, ID, FALSE);
                    temp->writeText(temptxt);
                    ID++;
                    textHead->append(temp, textHead);
                }
                if(textHead)textHead->ShowText(hwnd, 500, 200);
                break;
            }
        }
        break;

        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        case WM_SIZING:
//            GetWindowRect(hwnd, &winRect);
//            cout<<winRect.left<<endl<<winRect.top<<endl<<winRect.right<<endl<<winRect.bottom<<endl;
//            navBar.setWindRect(winRect.left+10, winRect.top+10, winRect.right*0.3, winRect.bottom-5);
//            dispSec.setWindRect((winRect.right+10)*0.3, winRect.top+10, winRect.right-10, winRect.bottom*0.76);
//            editBar.setWindRect((winRect.right+10)*0.3, (winRect.top+10)*0.76, winRect.right-10, winRect.bottom-5);
//
//            editBar.showMe(hwnd);
//            dispSec.ShowMe(hwnd);
//            navBar.showMe(hwnd);
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

char* ConvertTextToANS(const wchar_t *data)
{
    char *convStr;
    int len = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK|WC_SEPCHARS, data, wcslen(data), convStr,0, NULL, NULL);
    cout<<len<<" :String Length"<<endl;
    convStr = new char[len+1];

    len = WideCharToMultiByte(CP_INSTALLED, WC_COMPOSITECHECK|WC_SEPCHARS, data, wcslen(data), convStr, sizeof(convStr), NULL, NULL);
    //cout<<convStr[strlen(convStr)-1]<<endl;

    return convStr;
}
