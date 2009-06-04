#include <iostream>

#include "testIrmQuery.h"

using namespace std;

int main(int argc, char **argv)
{
	cout << "\n*** IRM Shell Client ***\n\n\n";
	cout << "type 'h' for available commands\n\n";

	//==========================================================
	#define BUFFER_SIZE	1024
	TestIrmQuery testIrm;

	char server[BUFFER_SIZE];
	char user[BUFFER_SIZE];
	char passwd[BUFFER_SIZE];
	char channel[BUFFER_SIZE];
	int port;

	char input = '\0';

	while(input != 'q')
	{
		cout << "> "; // need to sleep to wait server response before displaying input mark
		cin >> input;

		// help
		if(input == 'h')
		{
			cout << endl << "*** Command list ***" << endl;
			cout << "h - help" << endl;
			cout << "c - connect" << endl;
			cout << "l - list channels" << endl;
			cout << "j - join channel " << endl;
			cout << "p - part channel " << endl;
			cout << "u - list users in channel " << endl;
			cout << "m - send a private message" <<endl;
			cout << "q - quit" << endl;
			cout << endl;
		}

		// connect
		if(input == 'c')
		{
			// Clear input buffer
			char dummy;
			cin.getline(&dummy, 1);

			cout << "Host: ";
			cin.getline(server, BUFFER_SIZE);
			cout << "Port: ";
			cin >> port;
			cin.getline(&dummy, 1);
			cout << "Username: ";
			cin.getline(user, BUFFER_SIZE);
			cout << "Password: ";
			cin.getline(passwd, BUFFER_SIZE);
			
			if(testIrm.Connect(user, passwd, server, port))
			{
				cout << "Connecting to server " << server << " ..." << endl;
			}
			else
			{
				cout << "Cannot initiate connection.\n" << endl;
			}
		}

		// List channels
		if(input == 'l')
		{
			testIrm.GetChannelList();
			//testIrm.GetChannelList("#multitv-m");
		}

		// Join channel
		if(input == 'j')
		{
			char dummy;
			cin.getline(&dummy, 1);
			cout << "Channel: ";
			cin.getline(channel, BUFFER_SIZE);
			testIrm.JoinChannel(channel);
		}

		// Part channel
		if(input == 'p')
		{
			testIrm.PartChannel(channel);
		}

		// List users from channel
		if(input == 'u')
		{
			testIrm.GetUserList(channel);
		}

		// Send a private message
		if(input == 'm')
		{
			char receiver[256];
			char message[512];

			cout << "Private message receiver: ";
			cin >> receiver;
			cout << "Private message text: ";
			cin >> message;

			testIrm.SendPrivateMessage(receiver, message);
		}
	}



	cout << "Terminating ..." << endl;
	//==========================================================
	cout << endl;
	exit(0);
}
