#include < stdio.h > // Printf and gets
#include < string.h > // strcpy
#include "RakClientInterface.h"
#include "RakNetworkFactory.h"
#include "RakServerInterface.h"
#include "PacketEnumerations.h"

// Moved out of main - needs global scope
RakClientInterface *rakClientInterface;
RakServerInterface *rakServerInterface;

void PrintMessage(char *input, int numberOfBitsOfData, PlayerID sender)
{
	printf("%s\n",input);

	if (rakServerInterface)
		rakServerInterface->RPC("PrintMessage", input, numberOfBitsOfData, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, true, false);
}

int main(void)
{
	char str[512];
	Packet *packet;

	printf("(C)lient or (S)erver?\n");
	gets(str);
	if (str[0]=='c' || str[0]=='C')
	{
		rakClientInterface=RakNetworkFactory::GetRakClientInterface();
		rakServerInterface=0;
	}
	else
	{
		rakClientInterface=0;
		rakServerInterface=RakNetworkFactory::GetRakServerInterface();
	}

	if (rakServerInterface)
	{
		// Running in server mode on port 60000
		if (rakServerInterface->Start(32, 0, 0, 60000))
			printf("Starting the server.\n");
		else
			printf("Failed to start the server.\n");

		REGISTER_AS_REMOTE_PROCEDURE_CALL(rakServerInterface, PrintMessage);
	}
	else
	{
		// Running in client mode
		printf("Enter server IP or hit enter for 127.0.0.1\n");
		gets(str);
		// 127.0.0.1 designates the feedback loop so we can test on one computer
		if (str[0]==0)
			strcpy(str, "127.0.0.1");
		if (rakClientInterface->Connect(str, 60000, 0, 0, 0))
			printf("Starting the client.\n");
		else
			printf("Failed to start the client.\n");

		REGISTER_AS_REMOTE_PROCEDURE_CALL(rakClientInterface, PrintMessage);
	}

	while (1)
	{
		if (rakClientInterface)
		{
			printf("Enter a string or hit enter to display incoming strings\n");
			gets(str);
			
			// Two tricky things here.  First, you have to remember to send the NULL terminator so you need strlen(str)+1
			// Second, if you didn't read the docs you might not realize RPC takes the number of bits rather than the number of bytes.
			// You have to multiply the number of bytes by 8
			if (str[0])
				rakClientInterface->RPC("PrintMessage", str, (strlen(str)+1)*8, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false);
		}

		if (rakServerInterface)
			packet=rakServerInterface->Receive();
		else
			packet=rakClientInterface->Receive();

		while (packet)
		{
			switch (packet->data[0])
			{
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
				printf("Another client has disconnected.\n");
				break;
			case ID_REMOTE_CONNECTION_LOST:
				printf("Another client has lost the connection.\n");
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
				printf("Another client has connected.\n");
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				printf("Our connection request has been accepted.\n");
				break;
			case ID_NEW_INCOMING_CONNECTION:
				printf("A connection is incoming.\n");
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				if (rakServerInterface)
					printf("A client has disconnected.\n");
				else
					printf("We have been disconnected.\n");
				break;
			case ID_CONNECTION_LOST:
				if (rakServerInterface)
					printf("A client lost the connection.\n");
				else
					printf("Connection lost.\n");
				break;
			case ID_RECEIVED_STATIC_DATA:
				printf("Got static data.\n");
				break;
			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}

			if (rakServerInterface)
				rakServerInterface->DeallocatePacket(packet);
			else
				rakClientInterface->DeallocatePacket(packet);

			// Stay in the loop as long as there are more packets.
			if (rakServerInterface)
				packet=rakServerInterface->Receive();
			else
				packet=rakClientInterface->Receive();
		}
	}

	if (rakClientInterface)
		RakNetworkFactory::DestroyRakClientInterface(rakClientInterface);
	else if (rakServerInterface)
		RakNetworkFactory::DestroyRakServerInterface(rakServerInterface);
	return 0;
}