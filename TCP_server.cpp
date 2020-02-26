#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <iostream>

using namespace std;

int     main()
{
   //creat a socket
   int listening = socket(AF_INET, SOCK_STREAM, 0);
   if (listening == -1)
   {
       cerr << "can't creat a socket!";
       return -1;
   }
   // bind the socket to a IP/ port
   sockaddr_in hint;
   hint.sin_family = AF_INET;
   hint.sin_port = htons(54000);
   inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

   bind(listening, (sockaddr*)&hint, sizeof(hint));
//    {
//        cerr << "can't bind to IP/port";
//        return -2;
//    }

   // mark socket fot listening in
   if (listen(listening, SOMAXCONN) == -1)
   {
       cerr << "can't listen!";
       return -3;
   }
   // accept a call 
   sockaddr_in client;
   socklen_t clientSize = sizeof(client);
   char host[NI_MAXHOST];
   char service[NI_MAXSERV];

   int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
    if (clientSize == -1)
    {
        cerr << "problem with client connecting!";
        return -4;
    }
    close(listening);
    memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);

    int result = getnameinfo((sockaddr*)&client, sizeof(client),
                            host,
                            NI_MAXHOST,
                            service,
                            NI_MAXSERV,
                            0);
    if (result)
    {
        cout << host << " connected on " << service << endl;
    }
    else 
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << host << " connected on " << ntohs(client.sin_port) << endl;
    }
    // while receiving dislay message echo message
    char buf[4096];
    while (true)
    {
        // clear the buffer
        memset(buf, 0, 4096);
        //wait for message
        int bytesRecv = recv(clientSocket, buf, 4096, 0);
        if (bytesRecv == -1)
        {
            cerr << "there was a connection issue" << endl;
            break;
        }
        if (bytesRecv == 0)
        {
            cout << "the client disconnected" << endl;
            break;
        }
        //display message
        cout << "Received: " << string(buf, 0, bytesRecv) << endl;
        //resend message
        send(clientSocket, buf, bytesRecv + 1, 0);
    }
    
    close(clientSocket);
}