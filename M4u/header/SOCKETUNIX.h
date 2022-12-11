#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <strings.h>
#include <

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)

using namespace std;

void gotoxy(int x, int y)
{
    COORD pos = {x, y};
    HANDLE cHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(cHandle, pos);
}

struct Message
{
    char from[500];
    char to[500];
    char body[1024];
};

class Clients
{
private:
    Clients *next=NULL, *prev=NULL;
    SOCKET ID;
    int nRet;
    char name[500]={0, }, buf[256]={0,};
    fd_set fr;
    TIMEVAL tv = {2, 5};
    Message mess;
public:
    void addElement(Clients *elem, Clients*);
    void createElement(SOCKET id, Clients*);
    void setID(int id){ID = id;}
    SOCKET getID(){return ID;}
    void isset();
    void processMessage();
    void sendTextMes();
    void readSet();
    void sendText(char* buffer);
    void init(char *);
    void receiveText();
};

class SocketObj
{
private:
    fd_set fr, fe, fw;
    int optVal=0, nRet, addressFamily, socketType;
    char buf[256];
    short PORT;
    char name[500];
    WSADATA ws;
    sockaddr_in socketAddr;
    SOCKET sock;
    Clients *cHead=NULL;
    TIMEVAL tv = {2, 5};
    Message TextMes;

public:
    void init();
    SocketObj(const char *, short, int, int);
    void bindSocket();
    int connectSoc();
    void listenSoc(int backlog);
    void AcceptCon();
    void receiveFile();
    void sendFile(char *);
    void isset();
    void readSet();
    void sendText(char *message, char *name);
};

void Clients::init(char *nam)
{
    sprintf(name, nam);
}

void SocketObj::init()
{
    nRet = WSAStartup(MAKEWORD(2,2), &ws);
    if(nRet == SOCKET_ERROR)
    {
        cerr<<"Couldn't start up environment"<<endl;
        exit(EXIT_FAILURE);
    }
    sock = socket(addressFamily, socketType, 0);
}

SocketObj::SocketObj(const char* ip, short port, int af, int sockTyp)
{
    addressFamily = af;
    socketType = sockTyp;
    PORT = port;
    socketAddr.sin_addr.s_addr = inet_addr(ip);
    socketAddr.sin_family = af;
    socketAddr.sin_port = htons(port);
    ZeroMemory(socketAddr.sin_zero, sizeof(socketAddr.sin_zero));
    init();
}

void SocketObj::bindSocket()
{
    nRet = bind(sock, (const sockaddr *)&socketAddr, sizeof(socketAddr));
    if(nRet ==SOCKET_ERROR)
    {
        cerr<<"Couldn't bind socket... exiting"<<endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
}
int SocketObj::connectSoc()
{
    cout<<"Enter name: ";
    cin>>buf;
    sprintf(name, buf);

    nRet = connect(sock, (const sockaddr *)&socketAddr, sizeof(socketAddr));
    if(nRet == SOCKET_ERROR)
    {
        cerr<<"Couldn't connect to server...."<<endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else
    {
        send(sock, buf, sizeof(buf), 0);
    }
    return 1;
}

void SocketObj::sendText(char* mes, char *to)
{
    sprintf(TextMes.from, name);
    sprintf(TextMes.to, to);

    sprintf(TextMes.body, buf);

    send(sock, TextMes.to, sizeof(TextMes.to), 0);
    send(sock, TextMes.from, sizeof(TextMes.from), 0);
    send(sock, TextMes.body, sizeof(TextMes.body), 0);
}

void SocketObj::listenSoc(int backlog)
{
    nRet = listen(sock, backlog);
    if(nRet==SOCKET_ERROR)
    {
        cerr<<"Couldn't set socket to listening mode... exit."<<endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else cout<<"Listening on Port:"<<ntohs(socketAddr.sin_port)<<endl;
}

void SocketObj::AcceptCon()
{
    FD_ZERO(&fr);
    FD_SET(sock, &fr);

    select(sock+1, &fr, NULL, NULL, &tv);
    if(FD_ISSET(sock, &fr))
    {
        if(!cHead)
        {
            Clients *temp = new Clients;
            temp->setID(accept(sock, NULL, NULL));
            if(!cHead)
            {
                recv(sock, buf, 256, 0);
                temp->init(buf);
                if(send(temp->getID(), "Hello there, connection made...",32, 0)==SOCKET_ERROR)
                {
                    cout<<"There was an error sending message"<<endl;
                    cout<<WSAGetLastError()<<endl;
                }
                cHead = temp;
            }
        }else cHead->createElement(accept(sock, NULL, NULL), cHead);
    }
    else
    {
        gotoxy(0, 2);
        cout<<"Waiting for connection."<<endl;
        Sleep(1000);
        gotoxy(0, 2);
        cout<<"Waiting for connection.."<<endl;
        Sleep(1000);
        gotoxy(0, 2);
        cout<<"Waiting for connection..."<<endl;
        Sleep(1000);
        gotoxy(0, 2);
        cout<<"Waiting for connection...."<<endl;
        Sleep(1000);
        gotoxy(0, 2);
        cout<<"Waiting for connection     "<<endl;
    }
}

void Clients::createElement(SOCKET id, Clients *head)
{
    Clients *temp = new Clients;
    temp->ID = id;
    if(!head)
    {
        if(send(temp->ID, "Hello there, connection made...",32, 0)==SOCKET_ERROR)
        {
            cout<<"There was an error sending message"<<endl;
            cout<<WSAGetLastError()<<endl;
        }
        head = temp;
    }
    else addElement(temp, head);

}
void Clients::addElement(Clients* elem, Clients *head)
{
    if(next || prev)elem->prev = next?next->prev:prev->next;
    else elem->prev = head;

    if(!next)
    {
        if(send(elem->ID, "Hello there, connection made...",32, 0)==SOCKET_ERROR)
        {
            cout<<"There was an error sending message"<<endl;
            cout<<WSAGetLastError()<<endl;
        }
        next = elem;
    }
    else next->addElement(elem, head);
}

void Clients::readSet()
{
    FD_ZERO(&fr);
    FD_SET(ID, &fr);

    nRet = select(ID+1, &fr, NULL, NULL, &tv);
    if(nRet>0)
    {
        if(FD_ISSET(ID, &fr))
        {
            if(recv(ID, buf, 256, 0)!=SOCKET_ERROR)
            {
                cout<<buf<<endl;
            }
        }
    }
    else cout<<"No set"<<endl;
    if(next)next->readSet();
}

void SocketObj::readSet()
{
    if(cHead)cHead->readSet();
}

void Clients::processMessage()
{
    // steps to process the message
    /*
    1) Read Message to a buffer
    2) Write Message to a File
     */
}

void Clients::sendText(char *buffer)
{
    // Steps to Process Message from one client to the next
    /*
    1) Open File for reading if changes happened to the file
    2) Read file, check the name
    3) Find the intended client
    4) Check if ready for writing...
    5) Write to client.
     */
     if(send(ID, buffer,strlen(buffer), 0)!=SOCKET_ERROR)
     {
         if(next)next->sendText(buffer);
     }
     else
     {
         cout<<"Couldn't send data to client: "<<ID<<endl;
         if(next)next->sendText(buffer); // move to the next client in the list
     }
}

void SocketObj::sendFile(char *fileName)
{
    // distribute contents of the file to the clients
    if(cHead)
    {
       ifstream in_file(fileName, ios::in);
       while(!in_file.eof())
        {
            in_file>>buf;

            cHead->sendText(buf);
        }
        in_file.close();
    }

}
void SocketObj::receiveFile()
{
    if(recv(sock, buf, 256, 0)!=SOCKET_ERROR)
    {
        ofstream out_file("data.txt", ios::app);

        cout<<buf<<endl;
        out_file<<buf;

    }
    else cout<<"Could not receive data....error code: "<<WSAGetLastError()<<endl;
}

#endif // SOCKET_H_INCLUDED
