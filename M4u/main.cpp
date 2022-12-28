#include "socket.h"

int main()
{
    Server srvr("127.0.0.1", 9090, AF_INET, SOCK_STREAM);
    srvr.bind_s();
    srvr.listen_s(5);

    while(1)
    {
        srvr.accept_s();
        srvr.poll_s();
    }
    return 0;
}
