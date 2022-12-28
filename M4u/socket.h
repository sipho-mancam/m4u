#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED

#include <winsock.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#define TYPE_REQUEST 0
#define TYPE_MSG 1
#define SYSTEM system

using namespace std;

void gotoxy(int x, int y)
{
    COORD pos = {x, y};
    HANDLE cHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(cHandle, pos);
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
    char *addr_raw;
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
    addr_raw = new char[strlen(ip)];
    strcpy(addr_raw, ip);
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

/**Messages struct */
struct Message
{
    char from[500];
    char to[500];
    char body[1024];
};

/********************************************************
/** Clients Object starts here
/********************************************************
/*/
// clients class for the server
class Clients
{
private:
    Clients *next=NULL, *prev=NULL;
    SOCKET ID;
    int s_value, session_open=0, sign_in=0, accepted=0;
    int nRet;
    char name[500]={0, }, buf[1024]={0,};
    char *intro = "Signup_or_Login";
    fd_set fr;
    TIMEVAL tv = {0, 1};
    Message mess;
    SOCKET session;
public:
    void init(char *);
    void addElement(Clients *elem, Clients*);
    void createElement(SOCKET id, Clients*);
    int poll_c(Clients *head);
    void remove_node(SOCKET soc_id, Clients *head);
    SOCKET who_has(char *name);

    void setID(int id){ID = id;}
    SOCKET getID(){return ID;}
    void isset();
    void processMessage();
    void sendTextMes();
    void readSet();
    void sendText(char* buffer, SOCKET ses);
    void receiveText();


};

/** Methods for the clients objects */
void Clients::init(char *nam)
{
    sprintf(name, nam);
}

void Clients::createElement(SOCKET id, Clients *head)
{
    Clients *temp = new Clients;
    temp->ID = id;
    s_value = id;
    if(!head)
    {
        if(send(temp->ID, intro, strlen(intro), 0)==SOCKET_ERROR)
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
    s_value = elem->ID;
    if(next || prev)elem->prev = (next?next->prev:prev->next);
    else elem->prev = head;
    if(!next)
    {
        if(send(elem->ID, intro,strlen(intro), 0)==SOCKET_ERROR)
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

int Clients::poll_c(Clients *head)
{
    FD_ZERO(&fr);
    FD_SET(ID, &fr);

    ZeroMemory(buf, sizeof(buf));
    nRet = select(ID+1, &fr, NULL, NULL, &tv);
    if(nRet>0)
    {
        if(FD_ISSET(ID, &fr))
        {
            if(sign_in==0)
            {
                char data[1024];
                if(recv(ID, data, 1023, 0)!=SOCKET_ERROR)
                {
                    strcpy(name, data);
//                    sprintf(buf, "Connected.\nopen session with:");
                    MessageData md(name, "system", TYPE_REQUEST, "Connected.\nopen session with:");
                    cout<<md.serializeMsg()<<endl;
                    char *s = md.serializeMsg();
                    send(ID, s,strlen(s), 0);
                    sign_in = 1;
                }
            }

            else if(accepted == 0)//open a new session
            {
                if(recv(ID, buf, 1023, 0)!=SOCKET_ERROR)
                {
                    MessageData md(buf);

                    session = head->who_has(md.getTo());
                    if(session == 0) // if we couldn't find the friend you requested
                    {
                        MessageData m_d(md.getFrom(), "system", TYPE_REQUEST, "user not found");
                        char *s  = m_d.serializeMsg();
                        send(ID, s, strlen(s), 0);
                    }
                    else // relay the message we have received the to client
                    {
                        char *s = md.serializeMsg();
                        send(session, s, strlen(s), 0);
                        accepted = 1;
                    }
                }
            }

            // if it's ready to be read... check if it has a session opened, and with who
            else if(session != 0)
            {
                ZeroMemory(buf, sizeof(buf));
                if(recv(ID, buf, 1023, 0)!=SOCKET_ERROR)
                {
                    send(session, buf, strlen(buf), 0);
                }
            }
        }
    }
    if(next)next->poll_c(head);
}

void Clients::remove_node(SOCKET soc_id, Clients *head)
{
    Clients *temp, *temp_prev;
    if(ID==soc_id)
    {
        if(!prev){
            temp = head->next;
            delete head;
            head = temp;
        }
        else {
            temp = next;
            temp_prev = prev;
            delete (temp->prev?temp->prev: temp_prev->next);
            temp_prev->next = temp;
        }
    }
    else if(next)next->remove_node(soc_id, head);
}

void Clients::sendText(char *buff, SOCKET ses)
{
    // if session open is false, who is protocol
    // else communication
}

SOCKET Clients::who_has(char *prot)
{
    int temp;

    if(strcmp(name, prot)==0)
    {
//        char intro_text[100];
//        sprintf(intro_text, "Is trying to open session with you, do you accept (1. yes 2. No)?");
//        send(ID, prot, strlen(prot), 0);
//        send(ID, intro_text, strlen(intro_text), 0);
        return ID;
    }
    else if(next) temp = next->who_has(prot);
    else return 0;
    return (temp);
}
/** End of Methods for clients objects */
/********************************************************
/** Clients Object ends here
/********************************************************
/*/


/********************************************************
/** Server Object starts here
/********************************************************
/*/
// Derived Server class
class Server: protected Socket
{
private:
    Clients *cHead=NULL;

public:
    Server():Socket(){}
    Server(const char* ip, short port, int af, int sockTyp):Socket(ip, port, af, sockTyp){}
    void open_c(const char* ip, short port, int af, int sockTyp){open(ip, port, af, sockTyp);}
    void listen_s(int backlog);
    void accept_s();
    void bind_s();
    void session_s();
    void poll_s();
};

/** Methods for the server object */
void Server::listen_s(int backlog)
{
    nRet = listen(sock, backlog);
    if(nRet==SOCKET_ERROR)
    {
        cerr<<"Couldn't set socket to listening mode... exit."<<endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else {
            cout<<"Connected on IP: "<<addr_raw<<endl;
            cout<<"Listening on Port:"<<ntohs(socketAddr.sin_port)<<endl;
    }
}

void Server::accept_s()
{
    FD_ZERO(&fr);
    FD_SET(sock, &fr);
    char *intro = new char[32];

    sprintf(intro, "Signup_or_Login");
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
                if(send(temp->getID(), intro,strlen(intro), 0)==SOCKET_ERROR)
                {
                    cout<<"There was an error sending message"<<endl;
                    cout<<WSAGetLastError()<<endl;
                }
                cHead = temp;
            }
        }else cHead->createElement(accept(sock, NULL, NULL), cHead);
    }
}

void Server::bind_s()
{
    bind_sock();
}

void Server::session_s()
{

}

void Server::poll_s()
{
    if(cHead)cHead->poll_c(cHead);
}
/** End of Methods for the server objects */

/********************************************************
/** Server Object ends here
/********************************************************
/*/
#endif // SOCKET_H_INCLUDED
