#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED
#include <winsock.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <vector>

#define TYPE_REQUEST 0
#define TYPE_MSG 1

using namespace std;

void gotoxy(int x, int y)
{
    COORD pos = {x, y};
    HANDLE cHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(cHandle, pos);
}
void clrScr()
{
    for(int i=0; i<120; i++)
    {
        for(int j=0; j<20; j++)
        {
            gotoxy(i, j);
            printf(" ");
        }
    }
    gotoxy(0,0);
}


vector<char *>*  split(char *str, const char *del)
{
    char *token = strtok(str, del);

    vector<char *> *result = new vector<char *>;
    int index = 0;

    result->push_back(new char[strlen(token)+1]);

    strcpy(result->at(0), token);
    token = strtok(NULL, del);

    while(token != NULL)
    {
        result->push_back(new char[strlen(token)+1]);
        strcpy(result->at(index+1), token);
        index++;
        token = strtok(NULL, del);
    }
    return result;
}

class MessageData{
private:
    char *to;
    char *from;
    int type = TYPE_REQUEST;
    char *data;

public:
    MessageData(char *serialized){
        this->deSerializeMsg((const char *)serialized);
    }

     MessageData(){
        this->to = NULL;
        this->from = NULL;
        this->data = NULL;
    }

    MessageData(const char *to, const char *from, int type, const char *data)
    {
        this->to = new char[strlen(to)];
        strcpy(this->to, to);
        this->from = new char[strlen(from)];
        strcpy(this->from, from);
        this->data = new char[strlen(data)];
        strcpy(this->data, data);
        this->type = type;
    }

    char* serializeMsg();
    MessageData* deSerializeMsg(const char *msg);
    bool isRequest(){return (this->type == TYPE_REQUEST);}
    bool isSystem(){return (strcmp(this->from, "system")==0);}
    bool isAccepted(){return (strcmp(this->data, "accept")==0);}
    char * getData(){return this->data;}
    void setData(const char *d)
    {
        this->data = new char[strlen(d)];
        strcpy(this->data , d);
    }

    char * getFrom(){return this->from;}
    void setFrom(const char *d)
    {
        this->from = new char[strlen(d)];
        strcpy(this->from , d);
    }

    char * getTo(){return this->to;}
    void setTo(const char *d)
    {
        this->to = new char[strlen(d)];
        strcpy(this->to , d);
    }
};

char* MessageData::serializeMsg()
{
    // we are going to take all the fields and write them to a long string and return in
    int sumlen = 0;
    sumlen += strlen(this->to);
    sumlen += strlen(this->from);
    sumlen += strlen(this->data);
    sumlen += 2; // for the type field and termination
    char *res = new char[sumlen];
    sprintf(res, "%s\n%s\n%d\n%s", this->to, this->from, this->type, this->data);
    return res;
}

MessageData* MessageData::deSerializeMsg(const char *msg)
{
    char *msg_cp = new char[strlen(msg)];
    strcpy(msg_cp, msg);

    vector<char *> *res = split(msg_cp, "\n");

    this->to = res->at(0);
    this->from = res->at(1);
    this->type = atoi(res->at(2));
    this->data = res->at(3);

    return this;
}

char* strcut(char *str, char cha)
{
    int len = strlen(str);
    char *newStr = new char[len];

    for(int i=0; i<len; i++)
    {
        if(str[i]==cha)
        {
            sprintf(newStr, &str[i+1]);
            break;
        }
    }
    return newStr;
}

char* strcut_name(char *str)
{
    int len = strlen(str);
    int i=8;
    char *name = new char[256];

    for(i=8; str[i]!='\n'; i++)
    {
        name[i-8] = str[i];
    }
    name[i-8] = NULL;
    return name;
}

void box(int x, int y, int width, int height)
{
    clrScr();
    gotoxy(x, y);
    printf("%c", 201);
    gotoxy(x+width, y);
    printf("%c", 187);
    gotoxy(x, y+height);
    printf("%c", 200);
    gotoxy(x+width, y+height);
    printf("%c", 188);

    for(int i=x+1; i<width; i++)
    {
        gotoxy(i, y);
        printf("%c", 205);
        gotoxy(i, y+height);
        printf("%c", 205);
    }
    for(int j=y+1; j<height; j++)
    {
        gotoxy(x, j);
        printf("%c", 186);
        gotoxy(x+width, j);
        printf("%c", 186);
    }
    cout<<endl<<endl;
}

/********************************************************/
/** Socket Object starts here
/********************************************************/
// base socket class
class Socket
{
protected:
    // Data
    int optVal=0, nRet, addressFamily, socketType;
    char buf[256];
    short PORT;
    char name[500];
    WSADATA ws;
    sockaddr_in socketAddr;
    SOCKET sock;
    TIMEVAL tv = {2, 5};
    fd_set fr, fw, fe;
    // Methods
    void init();
    Socket(const char* ip, short port, int af, int sockTyp);
    Socket(){}
    void open(const char* ip, short port, int af, int sockTyp);
    void bind_sock();
};

/** Methods concerning the base class */
void Socket::init() // initialize the environment
{
    nRet = WSAStartup(MAKEWORD(2,2), &ws);
    if(nRet == SOCKET_ERROR)
    {
        cerr<<"Couldn't start up environment"<<endl;
        exit(EXIT_FAILURE);
    }
    sock = socket(addressFamily, socketType, 0);
}

Socket::Socket(const char* ip, short port, int af, int sockTyp) // constructor 1
{
    addressFamily = af;
    socketType = sockTyp;
    PORT = port;
    socketAddr.sin_addr.s_addr = inet_addr(ip);
    socketAddr.sin_family = af;
    socketAddr.sin_port = htons(port);
    ZeroMemory(socketAddr.sin_zero, sizeof(socketAddr.sin_zero));
    init();
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&optVal, sizeof(optVal));
}

void Socket::open(const char* ip, short port, int af, int sockTyp) // constructor alt
{
    addressFamily = af;
    socketType = sockTyp;
    PORT = port;
    socketAddr.sin_addr.s_addr = inet_addr(ip);
    socketAddr.sin_family = af;
    socketAddr.sin_port = htons(port);
    ZeroMemory(socketAddr.sin_zero, sizeof(socketAddr.sin_zero));
    init();
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&optVal, sizeof(optVal));
}

void Socket::bind_sock() // bind method
{
    nRet = bind(sock, (const sockaddr *)&socketAddr, sizeof(socketAddr));
    if(nRet ==SOCKET_ERROR)
    {
        cerr<<"There was an error\n";
        //cerr<<WSAGetLastError()<<endl;
        switch(WSAGetLastError())
        {
        case WSAEADDRINUSE:
            cerr<<"Error: Address in use"<<endl;
            break;
        case WSAENETDOWN:
            cout<<"Error: Net down"<<endl;
            break;
        case WSAEADDRNOTAVAIL:
            cerr<<"Error: Address not available"<<endl;
            break;
        default:
            cerr<<WSAGetLastError()<<endl;
        }
        WSACleanup();
        exit(EXIT_FAILURE);
    }
}

/** End of Base Methods */
/********************************************************
/** Socket Object starts here
/********************************************************
/*/

/********************************************************
/** Client Object starts here
/********************************************************
/*/
// derived client class
class Client : protected Socket
{
private:
    char nameID[256];
    char mes[1024];
    int counter=0;
    char friend_Name[256] = {0, };
public:
    Client(){}
    Client(const char* ip, short port, int af, int sockTyp) : Socket(ip, port, af, sockTyp){}
    void open_c(const char* ip, short port, int af, int sockTyp){open(ip, port, af, sockTyp);} // open the socket
    int connect_s();
    void signup_or_login();
    void recieve_text();
    void open_session();
    void open_session(MessageData *md);
    void send_text();
    void poll_s();
};

/** Methods for the Client class */
int Client::connect_s()
{
    nRet = connect(sock, (const sockaddr *)&socketAddr, sizeof(socketAddr));
    if(nRet == SOCKET_ERROR)
    {
        cerr<<"Couldn't connect to server...."<<endl;
        WSACleanup();
        closesocket(sock);
        return 0;
    }
    else if(recv(sock, buf, 256, 0)!=SOCKET_ERROR){

        if(strcmp(buf, "Signup_or_Login")==0){
            signup_or_login();
        }
    }
    return 1;
}

void Client::signup_or_login()
{
    ofstream client_info("client_info.txt",ios::out);
    string name_s;
    cout<<"Enter your Name:"<<endl;

    cin>>name_s;
//    getline(cin, name_s);
    sprintf(nameID, name_s.c_str());

    client_info<<nameID<<endl;

    if(send(sock, nameID, strlen(nameID), 0) == SOCKET_ERROR){
        switch(WSAGetLastError())
        {
        case 2:
            break;
        default:
            cout<<"There was an error:\n"<<WSAGetLastError()<<endl;
            break;
        }
    }
    client_info.close();
}

void Client::recieve_text()
{
    ZeroMemory(mes, 1024);
    FD_ZERO(&fr);
    FD_SET(sock, &fr);


    nRet = select(sock+1, &fr, NULL, NULL, &tv);

    if(nRet >0)
    {

        if(FD_ISSET(sock, &fr))
        {

            char data[1024];
            recv(sock, data, 1024, 0);

            MessageData md(data);

            // this is when you are asked or asking to open a session
            if(md.isRequest())
            {
                if(md.isSystem())
                {
                    cout<<"Connected\nOpen Session with (name):"<<endl;
                    this->open_session(&md);
                }
                else // it's a message from a friend
                {
                    if(!md.isAccepted())
                    {
                        cout<<md.getFrom()<<", want's to open a channel with you, do you accept?(y/N)"<<endl;
                        this->open_session(&md);
                    }
                    else
                    {
                        cout<<md.getTo()<<"Declined your request"<<endl;
                    }
                }
            }
            else// send messages
            {
                cout<<md.getFrom()<<" : "<<md.getData()<<endl;
            }
        }
    }
    if(kbhit())
    {
        cout<<nameID<<" : ";
        send_text();
    }



}

void Client::open_session(MessageData *md)
{
    string inp;
    getline(cin, inp);

    // you have a request from a friend
    if(!md->isSystem())
    {
        if(strcmp(inp.c_str(), "y")==0 || strcmp(inp.c_str(), "Y")==0)
        {
            MessageData m_d(md->getFrom(), nameID, TYPE_REQUEST, "accept");
            send(sock, m_d.serializeMsg(), 2, 0);
            cout<<"Connected."<<endl;
        }
        else
        {
            MessageData m_d(md->getFrom(), nameID, TYPE_REQUEST, "decline");
            send(sock, m_d.serializeMsg(), 2, 0);
            cout<<"Declined"<<endl;
        }
    }
    // you have a message from the system
    else
    {
        MessageData m_d(inp.c_str(), nameID, TYPE_REQUEST, "open?");
        char *s = md->serializeMsg();
        send(sock, s, strlen(s), 0);
    }

}

void Client::send_text()
{
    // create a message object
    string friend_s;
    getline(cin, friend_s);
    MessageData md(friend_Name, nameID, TYPE_MSG, friend_s.c_str());
    char *s = md.serializeMsg();
    send(sock, s, strlen(s), 0);
}
/** End of client Methods */

/********************************************************
/** Client Object ends here
/********************************************************
/*/

/**Messages struct */
struct Message
{
    char from[500];
    char to[500];
    char body[1024];
};
#endif // SOCKET_H_INCLUDED
