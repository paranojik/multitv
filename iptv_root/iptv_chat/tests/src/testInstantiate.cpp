#include <iostream>

#include "chat/DllChat.h"
#include "chat/MessageData.h"

using namespace std;

int main(int argc, char **argv)
{
	cout << "\nTest the instantiation of iptv_chat classes.\n\n" << endl;

	//==========================================================
	cxProxyChat _cxProxyChat;
	cout << "cxProxyChat instantiated in the stack.\n" << endl;

	cxProxyChat *pcxProxyChat;
	cout << "cxProxyChat pointer instantiated in the stack." << endl;
	pcxProxyChat = new cxProxyChat();
	cout << "cxProxyChat instantiated in the heap." << endl;
	delete pcxProxyChat;
	cout << "cxProxyChat deleted.\n" << endl;

	//==========================================================
	MessageData _MessageData;
	cout << "MessageData instantiated in the stack.\n" << endl;

	MessageData *pMessageData;
	cout << "MessageData pointer instantiated in the stack." << endl;
	pMessageData = new MessageData();
	cout << "MessageData instantiated in the heap." << endl;
	delete pMessageData;
	cout << "MessageData deleted.\n" << endl;

	//==========================================================
	cxString _cxString;
	cout << "cxString instantiated in the stack.\n" << endl;

	cxString *pcxString;
	cout << "cxString pointer instantiated in the stack." << endl;
	pcxString = new cxString();
	cout << "cxString instantiated in the heap." << endl;
	delete pcxString;
	cout << "cxString deleted.\n" << endl;

	//==========================================================
	MediaRequest _MediaRequest;
	cout << "MediaRequest instantiated in the stack.\n" << endl;

	MediaRequest *pMediaRequest;
	cout << "MediaRequest pointer instantiated in the stack." << endl;
	pMediaRequest = new MediaRequest();
	cout << "MediaRequest instantiated in the heap." << endl;
	delete pMediaRequest;
	cout << "MediaRequest deleted.\n" << endl;

	//==========================================================
	MediaQuery _MediaQuery;
	cout << "MediaQuery instantiated in the stack.\n" << endl;

	MediaQuery *pMediaQuery;
	cout << "MediaQuery pointer instantiated in the stack." << endl;
	pMediaQuery = new MediaQuery();
	cout << "MediaQuery instantiated in the heap." << endl;
	delete pMediaQuery;
	cout << "MediaQuery deleted.\n" << endl;

	//==========================================================
	MediaRcvUser _MediaRcvUser;
	cout << "MediaRcvUser instantiated in the stack.\n" << endl;

	MediaRcvUser *pMediaRcvUser;
	cout << "MediaRcvUser pointer instantiated in the stack." << endl;
	pMediaRcvUser = new MediaRcvUser();
	cout << "MediaRcvUser instantiated in the heap." << endl;
	delete pMediaRcvUser;
	cout << "MediaRcvUser deleted.\n" << endl;

	//==========================================================
	MediaPtP _MediaPtP;
	cout << "MediaPtP instantiated in the stack.\n" << endl;

	MediaPtP *pMediaPtP;
	cout << "MediaPtP pointer instantiated in the stack." << endl;
	pMediaPtP = new MediaPtP();
	cout << "MediaPtP instantiated in the heap." << endl;
	delete pMediaPtP;
	cout << "MediaPtP deleted.\n" << endl;

	//==========================================================
	cout << endl;
	return(0);
}
