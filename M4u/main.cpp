#include "socket.h"

int main()
{
    Server srvr("192.168.1.51", 9090, AF_INET, SOCK_STREAM);
    srvr.bind_s();
    srvr.listen_s(5);

    while(1)
    {
        srvr.accept_s();
        srvr.poll_s();
    }
    return 0;
}
