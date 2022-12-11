#ifndef GUIOBJECTS_H_INCLUDED
#define GUIOBJECTS_H_INCLUDED
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

#define SEND_BUTTON 100



class WindowSection
{
protected:
    int x, y, width, height, ID;
    HDC hdc;
    HBRUSH hBrush;
    HPEN hPen;
    WindowSection(int x, int y, int width, int height, int Id);
    void _showMe(HWND hwnd, HBRUSH hBrush, HPEN hPen);
    RECT secRect;
public:
    void rectInit();
    //WindowSection(){}
    void _redraw(int x, int y, int width, int height, HWND hwnd);
    void getWindRect(LPRECT myRect){*myRect = secRect;}
    void setWindRect(int x, int y, int x2, int y2);
};


/*-------------------------------------*/
/*Methods for the Window Section Class */
/*-------------------------------------*/

void WindowSection::rectInit()
{
    secRect.left = x;
    secRect.top = y;
    secRect.right = width;
    secRect.bottom = height;
}

WindowSection::WindowSection(int x1, int y1, int width1, int height1, int Id)
{

    x = x1;
    y = y1;
    width = width1;
    height = height1;
    ID = Id;
    rectInit();
    return;
}

void WindowSection::_showMe(HWND hwnd, HBRUSH hBrush, HPEN hPen)
{
    hdc = GetDC(hwnd);
    SelectObject(hdc, hPen);
    SelectObject(hdc, hBrush);
    Rectangle(hdc, x, y, width, height);

    DeleteObject(hPen);
    DeleteObject(hBrush);
    ReleaseDC(hwnd, hdc);

}

void WindowSection::_redraw(int x1, int y1, int width1, int height1, HWND hwnd)
{
    x = x1;
    y = y1;
    width = width1;
    height = height1;

    _showMe(hwnd, hBrush, hPen);
}

void WindowSection::setWindRect(int x1, int y1, int x2, int y2)
{
    x = x1;
    y = y1;
    width = x2;
    height = y2;
}

/*--------------------------------------------*/
/*End of Methods for the Window Section Class */
/*--------------------------------------------*/

class TextBase
{
private:
    int x, y, id, height, len, slen;
    bool rec;
    RECT textRect, sRect;
    wchar_t *text_w;
    char *text_c;
    HDC hdc;
    HBRUSH hBrush;
    HPEN hPen;
    HFONT hFont;
    TextBase *next=NULL, *prev=NULL;

public:
    TextBase(int xPos, int yPos, int ID, bool flag);
    TextBase(){}
    void init(int x, int y);
    void writeText(const char *data);
    void writeText(const wchar_t *data);
    void ShowText(HWND hwnd, int secWidth, int secHeight);
    int stringBreak(const wchar_t *data, wchar_t *outPut, int wCount);
    void append(TextBase *elem, TextBase *head);
    void getRect(LPRECT rect, int wCount, HWND hwnd);
    void test();
    void shiftUp();
    void shiftDown();

};


/*-------------------------------------*/
/*Methods for the Textba Section Class */
/*-------------------------------------*/

void TextBase::test()
{
    cout<<"My ID: "<<id<<endl;
}

TextBase::TextBase(int xPos, int yPos, int ID, bool flag)
{
    x = xPos;
    y = yPos;
    id = ID;
    rec = flag;
    textRect.left = x;
    textRect.top = y;
    textRect.bottom = 0;
    textRect.right = 0;
}

void TextBase::init(int x1, int y1)
{
    x = x1;
    y = y1;
}

void TextBase::writeText(const char *data)
{
    int len;
    len = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, data, strlen(data),text_w, 0);
    text_w = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, data, strlen(data), text_w, sizeof(text_w));
}

void TextBase::writeText(const wchar_t *data)
{
    int len = wcslen(data);
    text_w = new wchar_t[len];
    wcscpy(text_w, data);
}
void TextBase::ShowText(HWND hwnd, int secWidth, int secHeight)
{
    hdc = GetDC(hwnd);
    SetTextColor(hdc, RGB(250, 250, 250));
    SetBkColor(hdc, RGB(20,230,0));
    hFont = CreateFont(0, 0, 0, 0, FW_LIGHT, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, TEXT("Cavolini"));

    SelectObject(hdc, hFont);
    hBrush = CreateSolidBrush(RGB(20,230,0));
    hPen = CreatePen(PS_SOLID, 1, RGB(0,0,0));

    wchar_t tempTxt[wcslen(text_w)];

    // calculate the max number of characters
    slen = secWidth/9; // number of characters we can print
    len = stringBreak(text_w, tempTxt, slen);
    wcscpy(text_w, tempTxt);

    textRect.top = y;
    textRect.left = x;

    len = DrawTextW(hdc, text_w, wcslen(text_w), &textRect, DT_CALCRECT);

    if(textRect.right>secWidth)
    {
        textRect.right = secWidth-secWidth*0.3;
    }
    SelectObject(hdc, hPen);
    SetRect(&sRect, textRect.left-10, textRect.top-10, textRect.right+10, textRect.bottom+10);
    FillRect(hdc, &sRect, hBrush);
    len = DrawTextW(hdc, text_w, wcslen(text_w), &textRect, DT_WORDBREAK);

    DeleteObject(hBrush);
    DeleteObject(hPen);
    ReleaseDC(hwnd, hdc);

    if(next)next->ShowText(hwnd, secWidth, secHeight);

}

int TextBase::stringBreak(const wchar_t *data, wchar_t *outPut, int wCount)
{
    int i, j, len, breaks=0;

    len = wcslen(data);
    wcscpy(outPut, data);
    if(wCount > len)return 0;
    if(outPut)
    {
         for(j=0; j<len; j++)
        {
            if(outPut[j]==' ' && j%wCount==0)
            {
                outPut[j] = '\n';
                breaks ++;

            }
            else if(outPut[j]!=' ' && j%wCount==0)
            {
                for(i=j; outPut[i]!=' '; i--)
                {
                    // if you find a break point where ever you are going, start going forward until you reach an empty space
                    if(outPut[i-1] == '\n') // this means you are going back repeating the same thing.
                    {
                        while(outPut[i]!=' ')i++; // go forward till you find the next break point
                        break;
                    }
                }
                outPut[i] = '\n';
                breaks++;
            }
        }
    }
    return breaks-1;
}

void TextBase::append(TextBase *elem, TextBase *head)
{
    if(next)next->append(elem, head);
    else
    {
        if(!prev)
        {
            elem->prev = head;
            next = elem;
        }
        else{
            elem->next = NULL;
            elem->prev = prev->next;
            next = elem;
        }

    }
}

void TextBase::getRect(LPRECT rect, int wCount, HWND hwnd)
{

    wchar_t *tempStr = new wchar_t[wcslen(text_w)+1];
    if(!next)
    {
        hdc = GetDC(hwnd);
        stringBreak(text_w, tempStr, wCount);
        wcscpy(text_w, tempStr);
        DrawTextW(hdc, text_w, wcslen(text_w), &textRect, DT_CALCRECT);
        *rect = textRect;
        ReleaseDC(hwnd, hdc);
    }
    else if(next)next->getRect(rect, wCount, hwnd);
}
/*--------------------------------------------*/
/*End of Methods for the Window Section Class */
/*--------------------------------------------*/

class NavPane : public WindowSection
{
private:
public:
    NavPane(int x1, int y1, int w, int h, int id) : WindowSection(x1, y1, w, h, id){}
    void init(HWND hwnd);
    void showMe(HWND hwnd){_showMe(hwnd, hBrush, hPen);};
};

void NavPane::init(HWND hwnd)
{
    hBrush = CreateSolidBrush(RGB(250,250,250));
    hPen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
}



class EditSection: public WindowSection
{
private:
    HWND edit, sendButton;
    HDC hdc;
    HFONT hFont;
public:
    EditSection(int x1, int y1, int w, int h, int id, HWND hwnd) : WindowSection(x1, y1, w, h, id){}
    void init(HWND hwnd);
    void showMe(HWND hwnd);
    void getWinText(wchar_t *text);
};

void EditSection::init(HWND hwnd)
{
    edit = CreateWindowW(L"edit",
                             L"Type message here",
                             WS_CHILD|WS_VISIBLE|WS_VSCROLL|ES_MULTILINE|ES_AUTOVSCROLL,
                             x+3,
                             y+3,
                             (width-x)*0.8,
                             (height-y)*0.8,
                             hwnd,
                             (HMENU)0,
                             NULL,
                             NULL);

    sendButton = CreateWindowW(L"Button",
                               L"Send",
                               WS_CHILD|WS_VISIBLE,
                               x+(width-x)*0.8,
                               y+3,
                               (width-x)*0.2-10,
                               (height-y)*0.8,
                               hwnd,
                               (HMENU)SEND_BUTTON,
                               NULL,
                               NULL);

        ShowWindow(sendButton, SW_HIDE);
        ShowWindow(edit, SW_HIDE);
        hBrush = CreateSolidBrush(RGB(250, 250, 250));
        hPen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
}

//-------------------------------/
void EditSection::showMe(HWND hwnd)
{
    _showMe(hwnd, hBrush, hPen);
    ShowWindow(sendButton, SW_SHOW);
    ShowWindow(edit, SW_SHOW);
}

void EditSection::getWinText(wchar_t *text)
{
    int len;
    len = GetWindowTextLengthW(edit);
    GetWindowTextW(edit, text, len+1);
    SetWindowTextW(edit, L" ");
}

//--------------------------------

class Display : public WindowSection
{
private:
    HBRUSH hBrush;
    HPEN hPen;
public:
    Display(int x, int y, int h, int w, int id): WindowSection(x,y, h, w, id)
    {
        hBrush = CreateSolidBrush(RGB(250, 250, 250));
        hPen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
    }
    void ShowMe(HWND hwnd);
};


void Display::ShowMe(HWND hwnd)
{
    _showMe(hwnd, hBrush, hPen);
}


#endif // GUIOBJECTS_H_INCLUDED
