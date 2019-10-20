#include <stdio.h>
#include "BobsNetwork.h"

void BobsServer();
void BobsClient();

void BobsClientGame();

using BasicDataStructures::List;

int main()
{		
	while(1)
	{
		char S_OR_C[10];
		printf("Host(h) or Join(j) a Game: ");
		gets(S_OR_C);
		if (S_OR_C[0] == 'J' || S_OR_C[0] == 'j' || S_OR_C[0] == 'c' || S_OR_C[0] == 'C')
			BobsClient();
		else if (S_OR_C[0] == 'H' || S_OR_C[0] == 'h' || S_OR_C[0] == 's' || S_OR_C[0] == 'S')
			BobsServer(); 
	}			
}


// GET PACKETS 
unsigned char GetPacketIdentifier(Packet *p)
{
	if (p==0)
		return 255;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
	{
		assert(p->length > sizeof(unsigned char) + sizeof(unsigned long));
		return (unsigned char) p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	}
	else
		return (unsigned char) p->data[0];
}

//DRAW MAP
void DrawMap(int mapHeight, int mapLength, List<char> bobplayer, List<int> bobID, List<int> bobx, List<int> boby, List<int> bobhp, List<int> bobap, List<int> avx, List<int> avy, List<int> avcost, List<int> blockedX, List<int> blockedY)
{
	system("cls");
	int z;
	int x, y;
	int mh = mapHeight;
	int ml = mapLength;

	cout << endl;
	cout << "    ";
	for(x = 0; x < ml; x++)
	{
		cout << "   " << x << "    ";
		if(x < 10)
			cout << " ";
	}
	cout << endl;
	
	for(y = mh-1; y > -1; y--)
	{
		cout << "   ";
		for(x = 0; x < ml; x++)
			cout << "+--------";	
		cout << "+\n";
		
		cout << "   ";
		bool nobob3 = true;
		for(x = 0; x < ml; x++)
		{
			nobob3 = true;


			for(z =0; z < avx.size(); z++)
			{	
				if (x == avx[z] && y == avy[z])
				{
					if (avx[z] < 10)
					{
						if (avy[z] < 10)
						{
							if (avcost[z] < 10)
							{
								cout << "|" << avx[z] << "," << avy[z] << " *" << avcost[z] << "  ";
								nobob3 = false;
							}
							else
							{
								cout << "|" << avx[z] << "," << avy[z] << "*" << avcost[z] << "  ";
								nobob3 = false;
							}

						}
						else
						{
							if (avcost[z] < 10)
							{
								cout << "|" << avx[z] << "," << avy[z] << " *" << avcost[z] << " ";
								nobob3 = false;
							}
							else
							{
								cout << "|" << avx[z] << "," << avy[z] << "*" << avcost[z] << " ";
								nobob3 = false;

							}
						}
					}
					else
					{
						if(avy[z] < 10)
						{
							if (avcost[z] < 10)
							{
								cout << "|" << avx[z] << "," << avy[z] << " *" << avcost[z] << " ";
								nobob3 = false;
							}
							else
							{
								cout << "|" << avx[z] << "," << avy[z] << "*" << avcost[z] << " ";
								nobob3 = false;

							}
						}
						else
						{					
							if (avcost[z] < 10)
							{
								cout << "|" << avx[z] << "," << avy[z] << "*" << avcost[z] << " ";
								nobob3 = false;
							}
							else
							{
								cout << "|" << avx[z] << "," << avy[z] << "*" << avcost[z] << "";
								nobob3 = false;

							}
						}
					}
				}	
			}
			
			if (nobob3)
				cout << "|        ";
		}
		cout << "|\n";
		cout << "   ";
		bool nobob4 = true;
		for(x = 0; x < ml; x++)
		{
			nobob4 = true;

			for(z = 0; z< bobx.size(); z++)
			{
				if(x == bobx[z] && y == boby[z])
				{
					cout << "| PL: " << bobplayer[z] << "  ";
					nobob4 = false;
				}
			}

			if(nobob4 == true)
			{
				for(z = 0; z < blockedX.size(); z++)
				{
					if(blockedX[z] == x && blockedY[z] == y)
					{
						cout << "| XXXXXX ";
						nobob4 = false;
					}
				}
			}

			
			if(nobob4)
				cout << "|        ";	
		}
		cout << "|\n";
		cout << " ";
		if(y < 10)
			cout << " ";
		cout << y;		

		bool nobob2 = true;
		for(x = 0; x < ml; x++)
		{
			nobob2 = true;


			for(z = 0; z< bobx.size(); z++)
			{
				if(x == bobx[z] && y == boby[z])
				{
					cout << "| B#: " << bobID[z] << " ";
					nobob2 = false;
				}
			}
			if(nobob2)
				cout << "|        ";	
		}
		
		cout << "| " << y << endl;
		cout << "   ";
		bool nobob1 = true;
		for(x = 0; x < ml; x++)
		{
			nobob1 = true;
			for(z = 0; z< bobx.size(); z++)
			{
				if(x == bobx[z] && y == boby[z])
				{	
					if (bobhp[z] > 99)
					{
						cout << "| HP:";
						cout << bobhp[z] << " ";
						nobob1 = false;
					}
					else if ( bobhp[z] >= 10 && bobhp[z] <= 99)
					{
						cout << "| HP: ";
						cout << bobhp[z] << " ";
						nobob1 = false;
					}
					else
					{
						cout << "| HP:  ";
						cout << bobhp[z] << " ";
						nobob1 = false;
					}

				}	
				
			}
			if(nobob1)
				cout << "|        ";	
		}
		cout << "|\n";
		
		cout << "   ";
		bool nobob = true;
		for(x = 0; x < ml; x++)
		{
			nobob = true;
			for(z = 0; z< bobx.size(); z++)
			{
				if(x == bobx[z] && y == boby[z])
				{
					if (bobap[z] >= 10)
					{                    
						cout << "| AP: ";
						cout << bobap[z] << " ";
						nobob = false;
					}
					else
					{
						cout << "|  AP: ";
						cout << bobap[z] << " ";
						nobob = false;
					}
				}		
			}
			if(nobob)
			{
				cout << "|        ";
			}	
		}
		cout << "|\n";
	}
	
	cout << "   ";
	for(x = 0; x < ml; x++)
		cout << "+--------";	
	cout << "+\n";
	
	cout << "    ";
	for(x = 0; x < ml; x++)
	{
		cout << "   " << x << "    ";
		if(x < 10)
			cout << " ";
	}
	cout << endl << endl;
		
}

