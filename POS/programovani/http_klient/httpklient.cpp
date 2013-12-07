#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFSIZE 1000

using namespace std;

int main(int argc, char *argv[])
{
    string text("GET /~drymld/ps/ HTTP/1.1\r\nHost: phoenix.inf.upol.cz\r\nConnection: close");      // Odesílaný a přijímaný text
    hostent *host;              // Vzdálený počítač;
    sockaddr_in serverSock;     // Vzdálený "konec potrubí"
    int mySocket;               // Soket
    int port;                   // Číslo portu
    char buf[BUFSIZE];          // Přijímací buffer
    int size;                   // Počet přijatých a odeslaných bytů
    if (argc != 3)
    {
        cerr << "Syntaxe:\n\t" << argv[0]
                  << " " << "adresa port" << endl;
        return 1;
    }
    port = atoi(argv[2]);
    // Zjistíme info o vzdáleném počítači
    if ((host = gethostbyname(argv[1])) == NULL)
    {
        cerr << "Špatná adresa" << endl;
        return -1;
    }
    // Vytvoříme soket
    if ((mySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        cerr << "Nelze vytvořit soket" << endl;
        return -1;
    }
    // Zaplníme strukturu sockaddr_in

    // 1) Rodina protokolů
    serverSock.sin_family = AF_INET;

    // 2) Číslo portu, ke kterému se připojíme
    serverSock.sin_port = htons(port);

    // 3) Nastavení IP adresy, ke které se připojíme
    memcpy(&(serverSock.sin_addr), host->h_addr, host->h_length);
    // Připojení soketu
    if (connect(mySocket, (sockaddr *)&serverSock, sizeof(serverSock)) == -1)
    {
        cerr << "Nelze navázat spojení" << endl;
        return -1;
    }
    // Odeslání dat
    if ((size = send(mySocket, text.c_str(), text.size() + 1, 0)) == -1)
    {
        cerr << "Problém s odesláním dat" << endl;
        return -1;
    }
    cout << "Odesláno " << size << endl;

    // Příjem dat
    text = "";
    while ((size = recv(mySocket, buf, BUFSIZE, 0)) != -1)
    {
        cout << "Prijato " << size << endl;
        text += buf;
        break;
    }
    int delka_hlavicky=strstr(buf, "\r\n\r\n")-buf;

    // Uzavřu spojení
    close(mySocket);
    cout << "delka_hlavicky: " << delka_hlavicky << endl << text << endl;
    return 0;
}
