#include "socket.h"

int main()
{

    // Draw intro box
    box(0, 0, 60, 6);
    gotoxy(3, 2);
    cout<<"Welcome to M4U";
    gotoxy(3, 3);
    cout<<"Instructions ";
    gotoxy(3, 4);
    cout<<"1) Press any Key to reply to a text or open a session";

    string addr = "";

    gotoxy(3, 8);
    cout<<"Enter Server IP address: ";
    cin>>addr;
    cout<<endl;

    Client cl(addr.c_str(), 9090, AF_INET, SOCK_STREAM);
    gotoxy(0, 8);
    cout<<"Connecting ..."<<endl;
    if(cl.connect_s())
    {
        while(1)
        {
            cl.recieve_text();
        }
    }
    return 0;
}
