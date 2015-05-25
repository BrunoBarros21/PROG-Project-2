#include "BOMB.h"
#include "PLAYER.h"
#include "BOARD.h"
#include <iostream>

Player::Player(string playerName, string boardFilename) //define os privates da classe Player
{
	name = playerName;
	board = Board::Board(boardFilename);
}

void Player::showBoard() const // mostra o tabuleiro do jogador em quest�o
{
	board.display();
}

Bomb Player::getBomb() const // Pergunta as coordenadas da trajetoria da bomba e cria a mesma
{
	string coordenates;
	Position<char> char_coordenates;
	bool valid = true;

	do // enaquanto as coordenadas n�o forem v�lidas o programa volta a pedir as mesmas at� que lhe sejam fornecidas coordenadas v�lidas
	{
		std::cout << "Introduce your bomb coordenates (lin, col): ";
		std::cin >> coordenates;
		if (coordenates.size() != 2 || cin.eof()) //Apenas aceita dois char e n�o vai abaixo com o "^Z"
		{
			std::cout << "Invalid input! \n";
			cin.clear();
			valid = false;
		}
		else if (toupper(coordenates[0]) > (char)((int)'A' + board.getLines() - 1) || tolower(coordenates[1]) >(char)((int)'a' + board.getColumns() - 1) || toupper(coordenates[0]) < 'A' || tolower(coordenates[1]) < 'a') // verifica se as posicoes inseridas estao contidas no tabuleiro
		{
			valid = false;
			std::cout << "Inserted position out of range \n";
		}
		else
		{
			valid = true; //define num Position<char> as coordenadas da bomba 
			char_coordenates.lin = coordenates[0];
			char_coordenates.col = coordenates[1];
		}

	} while (!valid);
	

	return Bomb::Bomb(char_coordenates);
}

void Player::attackBoard(const Bomb &b) // "recebe" a bomba vinda do oponente;
{
	board.moveShips(); //move os barcos e de seguida a bomba � lan�ada 
	board.attack(b);
	
}

Board& Player::getBoard() //devolve o tabuleiro do jogador em quest�o
{
	return board;
}