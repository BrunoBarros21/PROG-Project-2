#include "BOARD.h"
#include "SHIP.h"
#include "BOMB.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "LAYOUT.h"

Board::Board()
{
	numColumns = 0;
	numLines = 0;
}

Board::Board(const string &filename) // loads board from file 'filename'
{
	string leitura;
	char temp;

	ifstream fich(filename);
	
	fich >> numLines >> temp >> numColumns;  // le do ficheiro as dimensoes do tabuleiro

	while (!fich.eof())  // ciclo que le o ficheiro e coloca no vector de ships um navio com a informacao lida 
	{
		char symbol, orientation;
		Position<char> position;  
		unsigned int size, color; 
		
		fich >> symbol >> size >> position.lin >> position.col >> orientation >> color; 

		ships.push_back(Ship(symbol, position, orientation, size, color));
	}
	fich.close();
	
	for (int i = 0; i < numLines; i++) // cria o board (vector bidimensional) com as dimensoes lidas
	{
		vector<int> temp (numColumns, -1);
		board.push_back(temp);
	}
	
}

bool Board::putShip(const Ship &s, unsigned int num) // adds ship to the board, if possible
{
	int line = s.getPosition().lin;
	int column = s.getPosition().col;
	bool free = true;
	if (s.getOrientation() == 'H') // orientacao horizontal
	{
		if (column + s.getSize() > numColumns || line < 0 || column < 0 || line >= numLines) // verifica se o navio esta fora do tabuleiro
			return false;
		for (int j = 0; j < s.getSize(); j++) // verifica o numero de posicoes correspondentes ao tamanho do navio
		{
			if (board[line][column + j] != -1 && board[line][column + j] != num)
			{
				free = false;
				break;
			}
		}
	}
	else                           // orientacao vertical
	{
		if (line + s.getSize() > numLines || line < 0 || column < 0 || column >= numColumns) // verifica se o navio esta fora do tabuleiro
			return false;
		for (int j = 0; j < s.getSize(); j++) // verifica o numero de posicoes correspondentes ao tamanho do navio
		{
			if (board[line + j][column] != -1 && board[line + j][column] != num)
			{
				free = false;	
				break;
			}
		}
	} 
	if (free)
	{
		if (s.getOrientation() == 'H') // orientacao horizontal
		{
			for (int j = 0; j < s.getSize(); j++) // preencher o numero de posicoes correspondentes ao tamanho do navio
			{
				board[line][column + j] = num;
			}
		}
		else                           // orientacao vertical
		{
			for (int j = 0; j < s.getSize(); j++) // preencher o numero de posicoes correspondentes ao tamanho do navio
			{
				board[line + j][column] = num;
			}
		}
	}
	return free;
}

void Board::putShips() 
{
	for (unsigned int i = 0; i < ships.size(); i++) // vai colocar todos os navios presentes no vector no tabuleiro
		putShip(ships[i], i);
}

void Board::moveShips() // tries to randmonly move all the ships of the fleet
{
	int count = 0;
	for (unsigned int i = 0; i < ships.size(); i++) // movimenta um navio de cada vez
	{
		if (!ships[i].isDestroyed())
		{
			Ship temp = ships[i]; // guarda a informacao do navio
			bool move = ships[i].moveRand(0, 0, numLines - 1, numColumns - 1); // verifica se o navio se movimentou
			if (move || ships[i].getOrientation() != temp.getOrientation()) // executa se o navio se movimentou ou rodou
			{
				deleteShip(temp); // apaga o navio original do tabuleiro
				while (count <= 20) // o programa tentara 20 vezes colocar o navio nas novas posicoes
				{
					if (putShip(ships[i], i))
						break;
					else if (count == 20) // caso no final de 20 iteracoes nao tiver colocado o navio, volta a posicao original
					{
						ships[i] = temp;
						putShip(temp, i);
					}
					else
					{
						ships[i] = temp; // coloca no vector o navio original
						ships[i].moveRand(0, 0, numLines - 1, numColumns - 1); // movimento de rotacao do navio novamente
					}
					count++;
				}
			}
			else
			{
				ships[i] = temp;
			}
		}
	}
}

void Board::deleteShip(const Ship &s)
{
	int line = s.getPosition().lin;
	int column = s.getPosition().col;
	if (s.getOrientation() == 'H') // orientacao horizontal
		{
			for (int j = 0; j < s.getSize(); j++) // apagar o numero de posicoes correspondentes ao tamanho do navio
			{
				board[line][column + j] = -1;
			}
		}
	else                           // orientacao vertical
	{
		for (int j = 0; j < s.getSize(); j++) // apagar o numero de posicoes correspondentes ao tamanho do navio
		{
			board[line + j][column] = -1;
		}
	}
}

bool Board::attack(const Bomb &b)
{
	bool hit = true;
	Position<int> coordenates;
	int partNumber;
	coordenates.lin = (int)(b.getTargetPosition().lin - 'A'); // conversao das coordenadas para inteiros
	coordenates.col = (int)(b.getTargetPosition().col - 'a'); // conversao das coordenadas para inteiros

	clrscr();
	gotoxy(0, 0);

	if (coordenates.lin < 0 || coordenates.col < 0 || coordenates.lin >  numLines - 1 || coordenates.col > numColumns - 1) // verifica se a bomba caiu fora do tabuleiro
		hit = false;
	else if (board[coordenates.lin][coordenates.col] == -1 ) //caso a bomba tenha acertado no "mar" nao atinge um navio
		hit = false;

	if (hit)
	{
		if (ships[board[coordenates.lin][coordenates.col]].getOrientation() == 'H')
			partNumber = coordenates.col - ships[board[coordenates.lin][coordenates.col]].getPosition().col; // calcula o indice do navio que foi atingido
		else if (ships[board[coordenates.lin][coordenates.col]].getOrientation() == 'V')
			partNumber = coordenates.lin - ships[board[coordenates.lin][coordenates.col]].getPosition().lin; // calcula o indice do navio que foi atingido

		char h = ships[board[coordenates.lin][coordenates.col]].getStatus()[partNumber]; // guarda o caracter do indice da posicao atingida
		if (islower(h))  // se a posicao ja tiver sido atingida avisa
		{
			cout << "Position has already been hit! \n";
		}

		ships[board[coordenates.lin][coordenates.col]].attack(partNumber); // vai mudar para minuscula a posicao atingida
		setcolor(10, 0);
		cout << "\nHit! \n";
		if (ships[board[coordenates.lin][coordenates.col]].isDestroyed()) // avisa quando o navio for destruido
		{
			setcolor(7, 0);
			cout << "Ship "; 
			setcolor(ships[board[coordenates.lin][coordenates.col]].getColor(), 0);
			cout << ships[board[coordenates.lin][coordenates.col]].getSymbol();
			setcolor(7, 0);
			cout << " has been destroyed!";
			deleteShip(ships[board[coordenates.lin][coordenates.col]]); // elimina o navio do board
		}
		setcolor(7, 0);
	}
	
	else
	{
		setcolor(12, 0);
		cout << "\nMiss! \n";
		setcolor(7, 0);
	}

	return hit;
}

void Board::display() const // displays the colored board during the game
{
	for (int i = 'a'; i < 'a' + numColumns; i++) // Identificação das colunas
	{
		setcolor(7, 0);
		if (i == 'a')
			cout << "  " << (char)i;
		else cout << fixed << setw(2) << (char)i;
	}
	cout << endl;

	for (int i = 0; i < numLines; i++)
	{
		for (int k = -1; k < numColumns; k++)
		{
			if (k == -1)
			{
				setcolor(7, 0);
				cout << (char)('A' + i); // Identificação das linhas
			}
			else if (board[i][k] == -1) // se no vector estiver -1 imprime um ponto (mar)
			{
				setcolor(9, 15);
				std::cout << setw(2) << ".";
			}
			else
			{
				setcolor(ships[board[i][k]].getColor(), 15); // selecciona a cor a imprimir correspondente 
				if (ships[board[i][k]].getOrientation() == 'H')
					std::cout << setw(2) << ships[board[i][k]].getStatus()[k - ships[board[i][k]].getPosition().col]; //imprime a letra correspondente a posicao do navio
				else if (ships[board[i][k]].getOrientation() == 'V')
					std::cout << setw(2) << ships[board[i][k]].getStatus()[i - ships[board[i][k]].getPosition().lin]; //imprime a letra correspondente a posicao do navio
			}
		}
		std::cout << endl;
	}
	setcolor(7, 0);
}

void Board::show() const// shows the attributes of the board (for debugging)
{
	for (int i = 0; i < numLines; i++) 
	{
		for (int k = 0; k < numColumns; k++)
		{
			std::cout << setw(3) << board[i][k];
		}
		std::cout << endl;
	}

}

vector<Ship> Board::getShips() const
{
	return ships;
}

bool Board::areDestroyed() const
{
	bool destroyed = true;
	for (unsigned int i = 0; i < ships.size(); i++) // verifica se todos os navios estao destruidos
	{
		if (ships[i].isDestroyed() == false)
			destroyed = false;
	}
	return destroyed;
}

int Board::getColumns() const
{
	return numColumns;
}

int Board::getLines() const
{
	return numLines;
}

ostream& operator<<(ostream& output, const Board &b)
{
	for (int i = 'a'; i < 'a' + b.getColumns(); i++) // Identificação das colunas
	{
		setcolor(7, 0);
		if (i == 'a')
			output << "  " << (char)i;
		else output << fixed << setw(2) << (char)i;
	}
	output << endl;

	for (int i = 0; i < b.numLines; i++)
	{
		for (int k = -1; k < b.numColumns; k++)
		{
			if (k == -1)
			{
				setcolor(7, 0);
				output << (char)('A' + i); // Identificação das linhas
			}
			else if (b.board[i][k] == -1)
			{
				setcolor(9, 15);
				output << setw(2) << ".";
			}
			else
			{
				setcolor(b.ships[b.board[i][k]].getColor(), 15);
				if (b.ships[b.board[i][k]].getOrientation() == 'H')
					output << setw(2) << b.ships[b.board[i][k]].getStatus()[k - b.ships[b.board[i][k]].getPosition().col];
				else if (b.ships[b.board[i][k]].getOrientation() == 'V')
					output << setw(2) << b.ships[b.board[i][k]].getStatus()[i - b.ships[b.board[i][k]].getPosition().lin];
			}
		}
		output << endl;
	}
	setcolor(7, 0);
	return output;
}

bool operator!=(const Board &board1, const Board &board2)
{
	return{
		board1.numLines != board2.numLines ||
		board1.numColumns != board2.numColumns
	};
}