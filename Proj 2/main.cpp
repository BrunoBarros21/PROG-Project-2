#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <fstream>
#include <cstdlib>
#include <windows.h>
#include <sstream>
#include <stdlib.h>
#include "BOMB.h"
#include "BOARD.h"
#include "SHIP.h"
#include "PLAYER.h"
#include "SCORE.h"
#include "TABLECREATOR.h"
#include "LAYOUT.h"

using namespace std;

int calculateScore(time_t playerTime, Player opponent)
{
	int score;
	int boardArea = opponent.getBoard().getColumns() * opponent.getBoard().getLines();
	int shipsArea = 0;

	for (unsigned int i = 0; i < opponent.getBoard().getShips().size(); i++)
	{
		shipsArea = shipsArea + opponent.getBoard().getShips()[i].getSize();
	}

	score = (int)round(playerTime * ((double)shipsArea / boardArea));

	return score;
}

void readScores(vector<Score> &scores)
{
	fstream file("Highscores.txt");
	string line, temp, name;
	int score;
	getline(file, line);
	if (line == "") // se o ficheiro estiver vazio vai preencher o vector de scores com scores irrealistas
	{
		for (unsigned int i = 0; i < scores.size(); i++)
		{
			scores[i].name = "Player";
			scores[i].score = 999999;
		}

		/*file << "Battleship Highscores" << endl;
		for (int i = 0; i < 10; i++) 
			file << i + 1 << " - Player - 999999" << endl;
		getline(file, line);
		for (int i = 0; i < 10; i++)
		{
			getline(file, line);
			stringstream ss(line);
			ss >> temp >> temp >> name >> temp >> score;
			scores[i].name = name;
			scores[i].score = score;
		}*/
	}
	else
	{
		for (int i = 0; i < 10; i++)
		{
			getline(file, line);
			stringstream ss(line);
			ss >> temp >> temp >> name >> temp >> score;
			scores[i].name = name;
			scores[i].score = score;
		}
	}
}

void showScores(vector<Score> &scores)
{
	cout << "\n    Battleship Highscores \n\n";
	for (int i = 0; i < 10; i++)
		cout << fixed << setw(2) << i + 1 << " - " << setw(13) << scores[i].name << " - " << setw(6) << scores[i].score << endl;
}

bool updateScore(vector<Score> &scores, unsigned int playerScore, string playerName) 
{
	unsigned int i = 0;

	for (i; i < scores.size(); i++)
	{
		if (playerScore < (unsigned) scores[i].score)
			break;
	}

	if (i == scores.size())
		return false;

	for (unsigned int j = scores.size() - 1; j > i; j--)
	{
		scores[j] = scores[j - 1];
	}
	
	scores[i].name = playerName;
	scores[i].score = playerScore;

	
	fstream file("Highscores.txt");
	string line, temp, name;

	file << "Battleship Highscores" << endl;
	for (unsigned int j = 0; j < 10; j++)
	{
		file << j + 1 << "  -  " << scores[j].name << " - " << scores[j].score << endl;	
	}
	file.close();

	return true;
}

void game(vector<Score> &scores)
{
	
	string name1, name2;
	string fich1, fich2;
	bool validfile = true;
	int j = rand() % 2;
	time_t timeP1 = 0;
	time_t timeP2 = 0;
	int winnerScore;
	char cont = 'Y';

	do
	{
		cout << "Player 1 write your name: ";
		getline(cin, name1);
		cin.clear();
		
	} while (name1 == "");
	do
	{
		cout << name1 << " introduce the file name you want to use: ";
		cin >> fich1;
		ifstream file(fich1);
		if (file.fail() )
		{
			validfile = false;
			cin.clear();
			cin.ignore(1000, '\n');
		}
		else validfile = true;

	} while (!validfile);
	Player p1(name1, fich1);
	cin.ignore(1000, '\n');
	
	do
	{
		cout << "Player 2 write your name: ";
		getline(cin, name2);
		cin.clear();

	} while (name2 == "");
	do
	{
		cout << name2 << " introduce the file name you want to use: ";
		cin >> fich2;
		ifstream file(fich2);
		if (file.fail())
		{
			validfile = false;
			cin.clear();
			cin.ignore(1000, '\n');
		}
		else validfile = true;
	} while (!validfile);
	Player p2(name2, fich2);
	cin.ignore(1000, '\n');
	
	
	if (p1.getBoard() != p2.getBoard())
	{
			do
			{
			cout << "Different board sizes. Do you want to continue? (Y/N)";
			cin >> cont;
			cont = toupper(cont);
			} while (cont != 'Y' && cont != 'N');
	}
	
	if (cont == 'N')
		return;

	p1.getBoard().putShips();
	p2.getBoard().putShips();
	while (!p1.getBoard().areDestroyed() || !p2.getBoard().areDestroyed())
	{
		if (p1.getBoard().areDestroyed())
		{
			cout << name2 << " is the winner. Congratulations!" << endl;
			winnerScore = calculateScore(timeP2, p1);
			if (updateScore(scores, winnerScore, name2))
				cout << "Your score is on the top 10! Congratulations! \n";
			showScores(scores);
			break;
		}
		else if (p2.getBoard().areDestroyed())
		{
			cout << name1 << " is the winner. Congratulations!" << endl;
			winnerScore = calculateScore(timeP1, p2);
			if(updateScore(scores, winnerScore, name1))
				cout << "Your score is on the top 10! Congratulations! \n";
			showScores(scores);
			break;
		}

		if (j % 2 == 0)
		{
			time_t startTime = time(NULL);
			// p1 a atacar
			cout << name1 << " it's your turn. \n\n";
			cout << p2.getBoard(); //p2.showBoard();
			Bomb atck = p1.getBomb();
			p2.attackBoard(atck);
			cout << "\n\n";
			time_t endTime = time(NULL);
			timeP1 = timeP1 + (endTime - startTime);

		}
		else if (j % 2 != 0)
		{
			time_t startTime = time(NULL);
			// p2 a atacar
			cout << name2 << " it's your turn. \n\n";
			cout << p1.getBoard(); //p1.showBoard();
			Bomb atck = p2.getBomb();
			p1.attackBoard(atck);
			cout << "\n\n";
			time_t endTime = time(NULL);
			timeP2 = timeP2 + (endTime - startTime);
		}
		j++;

	}
}

int main()
{
	srand((unsigned)time(NULL));
	int option;
	bool validoption = true;
	char cont = 'Y';
	do
	{
		vector<Score> scores(10);
		readScores(scores);


		gotoxy(35, 0);
		std::cout << "BATTLESHIP \n\n";
		gotoxy(20, 22);
		std::cout << "Developed by Bruno Barros and Fabio Caramelo - FEUP - MIEIC";
		gotoxy(0, 2);
		std::cout << "1 - Play Game" << endl;
		std::cout << "2 - View Highscores" << endl;
		std::cout << "3 - Create a new table" << endl;
		std::cout << "4 - Exit \n\n";
		do
		{
			std::cout << "Please select an option: ";
			std::cin >> option;
			std::cin.clear();
			
			if (std::cin.fail() || std::cin.eof() || option > 4 || option < 1)
			{
				validoption = false;
				std::cin.ignore(1000, '\n');
				std::cin.clear();
			}
			else
			{
				validoption = true;
				//std::cin.ignore(1000, '\n');
			}
		} while (!validoption);
		if (option == 1)
		{
			clrscr();
			game(scores);
		}
		else if (option == 2)
		{
			clrscr();
			showScores(scores);
		}
		else if (option == 3)
		{
			clrscr();
			createTable();
		}
		else if (option == 4)
			exit(1);

		std::cout << "\n\nDo you want to continue? (Y/N)";
		std::cin >> cont;
		clrscr();
	} while (cont == 'Y' || cont == 'y');
	

	return 0;
	
}