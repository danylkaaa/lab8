#include <iostream>
#include <Server.h>

using namespace std;

int main(void) {
    Server * server = Server::create(4321);
    server->start();
    string s;
    cin >> s;
    server->stop();
    delete server;
}