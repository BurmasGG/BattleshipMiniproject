#include<iostream>
#include<WS2tcpip.h>
#include<Winsock2.h>
#include<qos.h>
#pragma comment (lib, "ws2_32.lib")
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

using namespace std;

enum States {NILL, SHIP, DAMAGEDSHIP};


//For testing SFML
void drawWindow() {
	sf::Font font;
	if (!font.loadFromFile("BebasNeue.otf"))
	{
		cout << "cannot load font" << endl;
	}

	//Set width and height for the window
	float windowWidth = 600;
	float windowHeight = 600;
	
	//Set the offset 
	int offSet = 10;
	
	//Calculate the rect width and height
	float rectWidth = (((windowWidth - 10) - (10 * offSet)) / 11);
	float rectHeight = (((windowHeight - 10) - (10 * offSet)) / 11);

	// create the window
	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "My window");
	
	sf::RectangleShape rectangle(sf::Vector2f(rectWidth, rectHeight));
	std::vector<sf::RectangleShape> rectangleVector;
	
	sf::Text text;
	text.setCharacterSize(30);
	text.setFillColor(sf::Color::Black);
	text.setFont(font);
	sf::Vector2i mousePos;
	
	
	//Make the position of the rectangles and save them into a vector
	
	for (int x = 7; x < windowWidth - rectWidth + offSet; x += rectWidth + offSet)
	{
		for (int y = 7; y < windowHeight - rectWidth + offSet; y += rectHeight + offSet)
		{
			rectangle.setPosition(x, y);
			rectangleVector.push_back(rectangle);
		}
	}
	
	// run the program a long as the window is open
	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;


		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
		}
		// clear the window with black color
		window.clear(sf::Color::Black);
		
		//Get the position of the mouse
		mousePos = sf::Mouse::getPosition(window);

		//For loop that draws the squares
		
			for (int i = 0; i < rectangleVector.size(); i++)
			{
				sf::FloatRect rectbound = rectangleVector[i].getGlobalBounds();
				
				/*Tjecks if the mouse is with-in the rectangle if it change the color to red
				else it fills it with white*/
				if (rectbound.left<mousePos.x && rectbound.left + rectbound.width>mousePos.x && rectbound.top<mousePos.y && rectbound.top + rectbound.height>mousePos.y) {
					cout << to_string(i) <<endl;
					rectangleVector[i].setFillColor(sf::Color::Red);
				}
				else
				{
					rectangleVector[i].setFillColor(sf::Color::White);
				}
			

				window.draw(rectangleVector[i]);

			}
			
			
			int x =  3+ ((rectWidth + offSet) / 2)+( rectWidth + offSet);
			for (int i = 1; i <= 10; i++) 
			{
				int y =(3+(rectHeight + offSet)/2);
				
					text.setString(to_string(i));
					sf::FloatRect textRect = text.getLocalBounds();
					text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
					text.setPosition(x,y);
					window.draw(text);
					x += rectWidth + offSet;
			}
			string letters[10] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J" };
			int y = 3 + ((rectHeight + offSet) / 2) + (rectHeight + offSet);
			for (int i = 0; i<10; i++) 
			{
				string letter = letters[i];

				int x = (3 + (rectHeight + offSet) / 2);
				text.setString(letter);
				sf::FloatRect textRect = text.getLocalBounds();
				text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
				text.setPosition(x, y);
				window.draw(text);
				y += rectWidth + offSet;

			}

			window.display();
		}

	}

	int main()
{

	//SLET DRAWWINDOW() FOR AT KORE RESTEN AF PROGRAMMET 
	drawWindow();
	
	//initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2); //request version 2,2

		int wsOK = WSAStartup(ver, &wsData);

		if (wsOK != 0)
		{
			cerr << "Cannot initialize winsock, quitting" << endl;
			return 0;
		}


		//create socket
		SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
		if (listening == INVALID_SOCKET)
		{
			cerr << "Cannot create a socket, quitting" << endl;
			return 0;
		}
		//bind the ip address and port to a socket
		sockaddr_in hint;
		hint.sin_family = AF_INET;
		//htons = host to network short. here we try to bind port 54000 to any address
		hint.sin_port = htons(54000);
		hint.sin_addr.S_un.S_addr = INADDR_ANY;

		bind(listening, (sockaddr*)&hint, sizeof(hint));

		//tell winsock the socket is for listening
		listen(listening, SOMAXCONN); //SOMAXCONN limits the amount of outstanding connections 
		//in the sockets listen queue


		//wait for connection
		sockaddr_in client;
		int clientSize = sizeof(client);

		SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
		if (clientSocket == INVALID_SOCKET)
		{
			cerr << "Cannot connect to socket , quitting" << endl;
			return 0;
		}

		char host[NI_MAXHOST];		//client's remote name, if not available IP will be used
		char service[NI_MAXSERV];	//Service (i.e port) the client is connected on

		memset(host, 0, NI_MAXHOST);
		memset(service, 0, NI_MAXSERV);

		if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
		{
			//If we can't get the client info, we display the port number it connected on.
			cout << host << "Connected on port" << service << endl;
		}
		else
		{

			inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
			cout << host << "connected on port" <<
				ntohs(client.sin_port) << endl;
		}

		//close listening socket
		closesocket(listening);
		//while loop: accept echo message back to client
		char buf[4096];

		while (true)
		{
			memset(buf, 0, 4096);

			//wait for client to send data
			int bytesReceived = recv(clientSocket, buf, 4096, 0);
			if (bytesReceived == SOCKET_ERROR)
			{
				cerr << "Error in recv(). quitting" << endl;
				break;
			}

			if (bytesReceived == 0)
			{
				cout << "Client disconnected" << endl;
				break;
			}
			//Echo message back to client

			send(clientSocket, buf, bytesReceived + 1, 0);

		}
		//close socket
		closesocket(clientSocket);

		//clean up winsock
		WSACleanup();
		return 0;
	}

