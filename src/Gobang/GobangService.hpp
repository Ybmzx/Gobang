#ifndef GOBANG_SERVICE_HPP
#define GOBANG_SERVICE_HPP

#include <vector>

struct Vector2i {
	int x, y;
	Vector2i(int x = 0, int y = 0) : x(x), y(y) {}
	Vector2i operator+(const Vector2i& other) const {
		return Vector2i{ x + other.x, y + other.y };
	}
	Vector2i operator*(int v) const {
		return Vector2i{ this->x * v, this->y * v };
	}
	Vector2i operator-(const Vector2i& other) const {
		return this->operator+(other.operator*(-1));
	}
};

const Vector2i DIRECTION[4] = {
	Vector2i { 1, 0 },
	Vector2i { 0, 1 },
	Vector2i { 1, 1 },
	Vector2i { 1, -1 }
};

class GobangService {
public:
	enum CoordType {
		WHITE,
		BLACK,
		NONE
	};

	enum GameState {
		WHITE_WIN,
		BLACK_WIN,
		DRAW,
		PLAYING
	};

	bool inBounds(Vector2i pos) {
		if (board.empty() || board[0].empty()) return false;
		return pos.x >= 0 && pos.x < board.size() && pos.y >= 0 && pos.y < board[0].size();
	}

	GameState setPosition(Vector2i position, CoordType coordType)
	{
		if (!inBounds(position)) return PLAYING;

        board[position.x][position.y] = coordType;
		
		if (coordType == NONE) {
			chessCount--;
			return PLAYING;
		}

		if (coordType == WHITE || coordType == BLACK) chessCount++;
		
		for (auto&& dir : DIRECTION) {
			int count = 1;
            Vector2i curr = position;
			for (int i = 1; i < 5; i++) {
				Vector2i nextPos = curr + dir * i;
				if (!inBounds(nextPos) || board[nextPos.x][nextPos.y] != coordType) break;
				count++;
			}

			for (int i = 1; i < 5; i++) {
				Vector2i prevPos = curr - dir * i;
				if (!inBounds(prevPos) || board[prevPos.x][prevPos.y] != coordType) break;
				count++;
			}
			if (count >= 5) {
				return coordType == WHITE ? WHITE_WIN : BLACK_WIN;
			}
		}
		if (chessCount >= 15 * 15) {
			return DRAW;
		}
		return PLAYING;
	}

	void clear() {
		board = std::vector<std::vector<CoordType>>(15, std::vector<CoordType>(15, NONE));
	}
	int chessCount = 0;
	std::vector<std::vector<CoordType>> board = std::vector<std::vector<CoordType>>(15, std::vector<CoordType>(15, NONE));
};

#endif