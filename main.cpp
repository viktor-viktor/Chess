#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <SFML\Graphics.hpp>
#include "Connector.hpp"

using namespace sf;

enum figPossPos {
	WHITE_PAWN = 6,
	WHITE_ROOK = 1,
	WHITE_HORSE = 2,
	WHITE_OFFICER = 3,
	WHITE_QUEEN = 4,
	WHITE_KING = 5,
	BLACK_PAWN = -6,
	BLACK_ROOK = -1,
	BLACK_HORSE = -2,
	BLACK_OFFICER = -3,
	BLACK_QUEEN = -4,
	BLACK_KING = -5
};
using uint = unsigned int;
using chPair = std::pair< figPossPos, bool(*)(int, int, int, int) >;
using std::cout;
using std::endl;
using std::cin;
using std::vector;


/*
struct rail
{
	__int64 row;
	__int64 leftBound;
	__int64 rightBound;

	friend bool operator<(const rail& l, const rail& r)
	{
		return l.row < r.row;
	}
};
*/
//__int64 f(string);

uint size = 56; // size of an figure picture
Sprite figures[32];
std::string position = " ";
int board[8][8]			// virtual board that represent current figures position
{
	-1, -2, -3, -4, -5, -3, -2, -1,
	-6, -6, -6, -6, -6, -6, -6, -6,
	 0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,
	 6,  6,  6,  6,  6,  6,  6,  6,
	 1,  2,  3,  4,  5,  3,  2,  1
};
vector<chPair> checkFunc;

void SetFigures();	// function that create pictures of figures and sets positions
void MoveBack(vector<int>&);

////////////////////////////////  transform functions ///////////////////////////
std::string toChessNote(Vector2f p)			
{
	std::string s = "";
	s += char(p.x / size + 97);
	s += char(7 - p.y / size + 49);
	return s;
}	   
Vector2f toCoord(char a, char b)
{
	int x = int(a) - 97;
	int y = 7 - int(b) + 49;
	return Vector2f(x*size, y*size);
}

////////////////////////////////  check  possible position functions ////////////////////
bool WhitePawn(int, int, int, int);
bool BlackPawn(int, int, int, int);
bool BWKing(int, int, int, int);
bool BWHorse(int, int, int, int);
bool BWRook(int, int, int, int);
bool BWOfficer(int, int, int, int);
bool BWQueen(int, int, int, int);

int main()
{
	ConnectToEngine();
	std::cout << "1" << std::endl;
	RenderWindow window(VideoMode(456, 456), "Chess");
	Texture t1, t2;
	t1.loadFromFile("figures.png");
	t2.loadFromFile("board0.png");
	vector<int> log;
	log.reserve(6000);
	
	Sprite BOARD(t2);
	SetFigures();
	for (uint i = 0; i < 32; ++i)
		figures[i].setTexture(t1);

	checkFunc.push_back(chPair(BLACK_PAWN, BlackPawn));
	checkFunc.push_back(chPair(WHITE_PAWN, WhitePawn));
	checkFunc.push_back(chPair(BLACK_ROOK, BWRook));
	checkFunc.push_back(chPair(WHITE_ROOK, BWRook));
	checkFunc.push_back(chPair(BLACK_HORSE, BWHorse));
	checkFunc.push_back(chPair(WHITE_HORSE, BWHorse));
	checkFunc.push_back(chPair(BLACK_OFFICER, BWOfficer));
	checkFunc.push_back(chPair(WHITE_OFFICER, BWOfficer));
	checkFunc.push_back(chPair(BLACK_KING, BWKing));
	checkFunc.push_back(chPair(WHITE_KING, BWKing));
	checkFunc.push_back(chPair(BLACK_QUEEN, BWQueen));
	checkFunc.push_back(chPair(WHITE_QUEEN, BWQueen));


//	std::cout << t1.getSize().x << " " << t1.getSize().y << std::endl;
	
	bool isMove = false;
	int n;		//					keeps a number of currently moved figure
	int dx = 0, dy = 0;				
	Vector2i oldPos;		// keeps old position if current figure
	int boardCoord;			// keeps a correspoding board number of currentlu moved figure
	while (window.isOpen())
	{
		Event e;
		Vector2i pos = Mouse::getPosition(window);
		while (window.pollEvent(e))
		{
			if (e.type == Event::Closed)
				window.close();
			if (e.type == Event::MouseButtonPressed)
			{
				if (e.key.code == Mouse::Left)
					for (uint i = 0; i < 32; ++i)													// checking which figure stores mouse
						if (figures[i].getGlobalBounds().contains(pos.x, pos.y))					// saving it postion and boatd number
						{
							isMove = true; n = i;
							dx = pos.x - figures[i].getPosition().x;
							dy = pos.y - figures[i].getPosition().y;
							oldPos = (Vector2i)figures[i].getPosition();
							boardCoord = board[pos.y / size][pos.x / size];
							board[pos.y / size][pos.x / size] = 0;
						}
			}
			if (e.type == Event::KeyPressed)
			{
				if (e.key.code == Keyboard::BackSpace)
					if (log.size() != 0)
						MoveBack(log);
				if (e.key.code == Keyboard::Space)			// computer's move
				{
					std::string str = getNextMove(position);
					oldPos = (Vector2i)toCoord(str[0], str[1]);
					boardCoord = board[oldPos.y / size][oldPos.x / size];
					board[oldPos.y / size][oldPos.x / size] = 0;
					Vector2i newP = (Vector2i)toCoord(str[2], str[3]);
					for (uint i = 0; i < 32; ++i)			// searching for number of moved figure
						if ((Vector2i)figures[i].getPosition() == oldPos)
							n = i;
					for (uint i = 0; i < 100; ++i)			// moving animation
					{
						Vector2f v = (Vector2f)newP - (Vector2f)oldPos;
						figures[n].move(v.x / 100, v.y/100);
						window.clear();
						window.draw(BOARD);
						for (uint i = 0; i < 32; ++i)
							window.draw(figures[i]);
						window.display();
					}
				}
			}
			if (e.type == Event::MouseButtonReleased || e.type == Event::KeyReleased)
				if (e.key.code == Mouse::Left || e.key.code == Keyboard::Space)
				{
//					cout << "n=" << n << "  oldPos = " << oldPos.x << "," << oldPos.y << "  boardCoord=" << boardCoord << endl;
					int curPosX = figures[n].getPosition().x + size / 2, curPosY = figures[n].getPosition().y + size / 2; // position of cell where
					uint x = curPosX / size, y = curPosY / size;														  // where figure may be placed
//					cout << x*size << "   " << y*size << endl;
					int coord = board[y][x];						// number of cell on board where figure may be placed
					for (uint i = 0; i < checkFunc.size(); ++i)
						if (checkFunc[i].first == (figPossPos)boardCoord)
							if (!checkFunc[i].second(x, y, oldPos.x / size, oldPos.y / size))
								coord = boardCoord;
					if (coord == 0)								// if cell is empty
					{
						figures[n].setPosition(x*size, y*size);
						board[y][x] = boardCoord;
						if (oldPos.x/size != x || oldPos.y/size != y)
						{
							position += toChessNote((Vector2f)oldPos) + toChessNote(Vector2f(x*size, y*size)) + " ";
							log.push_back(oldPos.x / size);
							log.push_back(oldPos.y / size);
							log.push_back(x);
							log.push_back(y);
							log.push_back(n);
							log.push_back(33);
							log.push_back(0);
						}
					}
					else if (boardCoord*coord > 0)					// if figures of the same color
					{
						figures[n].setPosition(Vector2f(oldPos));
						board[oldPos.y / size][oldPos.x / size] = boardCoord;
					}
					else										// if  figures of diferent color
					{
						for (uint i = 0; i < 32; ++i)
							if (n != i && figures[i].getGlobalBounds().contains(curPosX, curPosY))
							{
								position += toChessNote((Vector2f)oldPos) + toChessNote(Vector2f(x*size, y*size)) + " ";
								figures[i].setPosition(8*size, 8*size); // "deleting" figure picture
								figures[n].setPosition(x*size, y*size);				// setting new figure
								board[y][x] = boardCoord;							// and new cell number
								log.push_back(oldPos.x / size);
								log.push_back(oldPos.y / size);
								log.push_back(x);
								log.push_back(y);
								log.push_back(n);
								log.push_back(i);
								log.push_back(coord);
							}
					}

					isMove = false;
					cout << position << endl;
				}
				
		}
		// chaging position of a moved figure
		if (isMove)
			figures[n].setPosition(pos.x - dx, pos.y - dy);

		// drawing
		window.clear();
		window.draw(BOARD);
		for (uint i = 0; i < 32; ++i)
			window.draw(figures[i]);
		window.display();
	}
	CloseConnection();
	return 0;
}

void SetFigures()
{
	Texture t;
	t.loadFromFile("figures.png");
	
	///////////////setting black figures
	/*Rect<int> r(0, 0, 60, 60);
	int st = 7;
	for (int i = 0; i < 3; ++i)
	{
		Sprite s(t, r);
		figures[i] = s;
		figures[i + st] = s;
		st -= 2;
		r.left += 60;
	}
	figures[3] = Sprite(t, Rect<int>(180, 0, 60, 60));
	figures[4] = Sprite(t, Rect<int>(240, 0, 60, 60));
	for (int i = 0; i < 8; ++i)
		figures[8 + i] = Sprite(t, Rect<int>(300, 0, 60, 60));

	///////////////setting white figures
	Rect<int> r(0, 60, 60, 60);
	int st = 7;
	for (int i = 24; i < 27; ++i)
	{
		Sprite s(t, r);
		figures[i] = s;
		figures[i + st] = s;
		st -= 2;
		r.left += 60;
	}
	figures[27] = Sprite(t, Rect<int>(180, 60, 60, 60));
	figures[28] = Sprite(t, Rect<int>(240, 60, 60, 60));
	for (int i = 0; i < 8; ++i)
		figures[16 + i] = Sprite(t, Rect<int>(300, 60, 60, 60));
	*/
	int k = 0;
	for (uint i = 0; i < 8; ++i)
	{
		for (uint j = 0; j < 8; ++j)
		{
			int n = board[i][j];
			if (n == 0)
				continue;
			int x = std::abs(n) - 1;
			int y = n > 0 ? 1 : 0;
			figures[k].setTextureRect(Rect<int>(size * x, size * y, size, size));
			figures[k].setPosition(size * j, size * i);
			++k;
		}
	}
}

void MoveBack(vector<int>& log)
{
	cout << "MoveBack()     pos = " <<  position << endl;
	position.erase(position.size() - 6, 5);
	int s = log.size()-1;
	int coord = log[s]; log.pop_back(); --s;
	int i = log[s]; log.pop_back(); --s;
	int n = log[s]; log.pop_back(); --s;
	int y = log[s]; log.pop_back(); --s;
	int x = log[s]; log.pop_back(); --s;
	int oY = log[s]; log.pop_back(); --s;
	int oX = log[s]; log.pop_back(); --s;
	if (i != 33)
	{
		figures[i].setPosition(x*size, y*size);
		cout << "picture" << endl;
	}
	figures[n].setPosition(oX*size, oY*size);
	board[oY][oX] = board[y][x];
	board[y][x] = coord;
	cout << "log size = " << log.size() << endl;
}

bool BlackPawn(int x, int y, int oX, int oY)
{
	int dx = std::abs(x - oX);
	if (dx > 1)	return false;
	if (dx == 0)
	{
		if (y != oY + 1 && y != oY + 2)	return false;
		if (y == oY + 2)
		{
			if (oY != 1) return false;
			if (board[y][x] == 0) return true;
			return false;
		}
		if (y == oY + 1 && board[y][x] == 0) return true;
		return false;			
	}
	if (dx == 1)
	{
		if (y != oY + 1) return false;
		if (board[y][x] == 0) return false;
		return true;
	}
}

bool WhitePawn(int x, int y, int oX, int oY)
{
	int dx = std::abs(x - oX);
	if (dx > 1)	return false;
	if (dx == 0)
	{
		if (y != oY - 1 && y != oY - 2)	return false;
		if (y == oY - 2)
		{
			if (oY != 6) return false;
			if (board[y][x] == 0) return true;
			return false;
		}
		if (y == oY - 1 && board[y][x] == 0) return true;
		return false;
	}
	if (dx == 1)
	{
		if (y != oY - 1) return false;
		if (board[y][x] == 0) return false;
		return true;
	}
}

bool BWKing(int x, int y, int oX, int oY)
{
	if (std::abs(x - oX) < 2 && std::abs(y - oY) < 2) return true;
	return false;
}

bool BWHorse(int x, int y, int oX, int oY)
{
	int dx = std::abs(x - oX);
	int dy = std::abs(y - oY);
	if (dx == 2 && dy == 1) return true;
	if (dx == 1 && dy == 2) return true;
	return false;
}

bool BWRook(int x, int y, int oX, int oY)
{
	int dx = x - oX;
	int dy = y - oY;
	if (dx == 0)
	{
		if (dy > 0)
		{
			for (int i = oY + 1; i < y; ++i)
				if (board[i][x] != 0) return false;
			return true;
		}
		if (dy < 0)
		{
			for (int i = y + 1; i < oY; ++i)
				if (board[i][x] != 0) return false;
			return true;
		}
	}
	if (dy == 0)
	{
		if (dx > 0)
		{
			for (int i = oX + 1; i < oX; ++i)
				if (board[y][i] != 0) return false;
			return true;
		}
		if (dx < 0)
		{
			for (int i = x + 1; i < oX; ++i)
				if (board[y][i] != 0) return false;
			return true;
		}
	}
	return false;
}

bool BWOfficer(int x, int y, int oX, int oY)
{
	int dx = x - oX;
	int dy = y - oY;
	if (std::abs(dx) != std::abs(dy)) return false;
	if (dx > 0)
	{
		if (dy > 0)
		{
			for (int i = 1; i < dx; ++i)
				if (board[oY + i][oX + i] != 0) return false;
			return true;
		}
		if (dy < 0)
		{
			for (int i = 1; i < dx; ++i)
				if (board[oY - i][oX + i] != 0) return false;
			return true;
		}
	}
	if (dx < 0)
	{
		if (dy > 0)
		{
			for (int i = -1; i > dx; --i)
				if (board[oY - i][oX + i] != 0) return false;
			return true;
		}
		if (dy < 0)
		{
			for (int i = -1; i > dx; --i)
				if (board[oY + i][oX + i] != 0) {
					cout << "board = " << board[oX + i][oY + i] << endl;  return false;
				}
			return true;
		}
	}
	return false;
}

bool BWQueen(int x, int y, int oX, int oY)
{
	int dx = x - oX;
	int dy = y - oY;
	if (dx == 0)
	{
		if (dy > 0)
		{
			for (int i = oY + 1; i < y; ++i)
				if (board[i][x] != 0) return false;
			return true;
		}
		if (dy < 0)
		{
			for (int i = y + 1; i < oY; ++i)
				if (board[i][x] != 0) return false;
			return true;
		}
	}
	if (dy == 0)
	{
		if (dx > 0)
		{
			for (int i = oX + 1; i < oX; ++i)
				if (board[y][i] != 0) return false;
			return true;
		}
		if (dx < 0)
		{
			for (int i = x + 1; i < oX; ++i)
				if (board[y][i] != 0) return false;
			return true;
		}
	}
	if (std::abs(dx) != std::abs(dy)) return false;
	if (dx > 0)
	{
		if (dy > 0)
		{
			for (int i = 1; i < dx; ++i)
				if (board[oY + i][oX + i] != 0) return false;
			return true;
		}
		if (dy < 0)
		{
			for (int i = 1; i < dx; ++i)
				if (board[oY - i][oX + i] != 0) return false;
			return true;
		}
	}
	if (dx < 0)
	{
		if (dy > 0)
		{
			for (int i = -1; i > dx; --i)
				if (board[oY - i][oX + i] != 0) return false;
			return true;
		}
		if (dy < 0)
		{
			for (int i = -1; i > dx; --i)
				if (board[oY + i][oX + i] != 0) {
					cout << "board = " << board[oX + i][oY + i] << endl;  return false;
				}
			return true;
		}
	}
	return false;
}

/*
__int64 f(string name)
{
	string line;						// reading first line data from file 
	__int64 row, column, tracks;
	ifstream file(name);
	file >> line; row = stoi(line);
	file >> line; column = stoi(line);
	file >> line; tracks = stoi(line);
	vector<rail> vec;
	vec.reserve(tracks);

	for (uint i = 0; i < tracks; ++i)		// reading information about each track
	{
		rail r;
		file >> line; r.row = stoi(line);
		file >> line; r.leftBound = stoi(line);
		file >> line; r.rightBound = stoi(line);
		vec.push_back(r);
	}
	file.close();

	
	__int64 ans = 0;
	std::sort(vec.begin(), vec.end());			// sortin track according to theirs track number
	vector<rail>::iterator it = vec.begin();
	__int64 num = 0;							// counter for an amount of row with tracks
	__int64 value = it->row;					// curren value of row where track exist
	while (it != vec.end())
	{
		vector<pair<__int64, __int64> > ch;		// vector which keeps tracks coordinate on certain row
		ch.reserve(tracks);
		while (it != vec.end() && value == it->row)
		{
			ch.push_back(pair<__int64, __int64>(it->leftBound, it->rightBound));
			++it;
		}
		
		std::sort(ch.begin(), ch.end());					// sorting all track corresponding to their leftBound value
		__int64 curAns = ch[0].second - ch[0].first + 1;	// curAns is amount of columns taken under the track
		for (uint i = 1; i < ch.size(); ++i)				// cicle of counting of columns taken under the tracks
		{
			if (ch[i].first > ch[i - 1].second)
			{
				curAns += ch[i].second - ch[i].first + 1;
				continue;
			}
			if (ch[i].second > ch[i - 1].second)
			{
				curAns += ch[i].second - ch[i - 1].second;
				continue;
			}
			ch[i].second = ch[i - 1].second;
		}
		if (it != vec.end())
			value = it->row;
		ans += column - curAns;
		++num;
	}
	ans += column*(row - num);						// adding row with no tracks on them



	return ans;
}
*/


/*  make castling as at example
	think of blocking unexceptable moves,
	much more tests is required

*/