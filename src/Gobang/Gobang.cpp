#include <iostream>
#include <conio.h>
#include "GobangService.hpp"
#include "GobangAI.hpp"
#include <unordered_map>

class Scene {
public:
	virtual void game() = 0;
};

Scene* currentScene = nullptr;

void changeScene(Scene* newScene) {
	if (currentScene != nullptr) {
		delete currentScene;
	}
	currentScene = newScene;
}

void clearScreen() {
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}

class MenuScene;
class PvpScene;

class MenuScene : public Scene {
public:
	void printMenu() {
		std::cout << "########################################" << '\n';
		std::cout << "#                                      #" << '\n';
		std::cout << "#                五子棋                #" << '\n';
		std::cout << "#           ----------------           #" << '\n';
		std::cout << "#             [1].双人对战             #" << '\n';
		std::cout << "#             [2]. AI 对战             #" << '\n';
		std::cout << "#             [0].退出游戏             #" << '\n';
		std::cout << "#                                      #" << '\n';
		std::cout << "#                                      #" << '\n';
		std::cout << "#      请输入你想要进行的操作序号      #" << '\n';
		std::cout << "#                                      #" << '\n';
		std::cout << "########################################" << '\n';
	}
	void game() override;
};

class PvpScene : public Scene {
public:
	GobangService gobangService;
	Vector2i cursorPos;

	GobangService::CoordType currentPlayer = GobangService::BLACK;

	void printMap() {
		std::cout << "WASD 控制下棋位置. 空格确定下棋. 黑子为 '*', 白子为 'o'\n";
		std::cout << '|';
		for (int i = 0; i < gobangService.board.size() * 4 - 1; i++) {
			std::cout << '-';
		}
		std::cout << '|' << '\n';

		for (int i = 0; i < gobangService.board.size(); i++) {
			std::cout << '|';
			for (int j = 0; j < gobangService.board[i].size(); j++) {
				GobangService::CoordType col = gobangService.board[i][j];
				if (col == GobangService::NONE) {
					std::cout << "   ";
				}

				if (col == GobangService::BLACK) {
					std::cout << " * ";
				}

				if (col == GobangService::WHITE) {
					std::cout << " o ";
				}

				std::cout << '|';
			}
			std::cout << '\n' << '|';
			for (int j = 0; j < gobangService.board.size(); j++) {
				if (j == gobangService.board.size() - 1) {
					if (i == cursorPos.y && cursorPos.x == j) std::cout << "AAA";
					else std::cout << "---";
					continue;
				}
				if (i == cursorPos.y && cursorPos.x == j) std::cout << "AAA-";
				else std::cout << "----";
			}
			std::cout << '|' << '\n';
		}
		std::cout << "当前执棋者: " << (currentPlayer == GobangService::WHITE ? "白子" : "黑子") << "." << '\n';
	}

	const std::unordered_map<char, Vector2i> KEY_TO_DIRECTION = {
		{ 'w', Vector2i(0, -1) },
		{ 's', Vector2i(0, 1) },
		{ 'a', Vector2i(-1, 0) },
		{ 'd', Vector2i(1, 0) },
	};

	void game() override {
		while (true) {
			clearScreen();
			printMap();
			char key = _getch();
			if (KEY_TO_DIRECTION.find(key) != KEY_TO_DIRECTION.end()) {
				Vector2i next = cursorPos + KEY_TO_DIRECTION.at(key);
				if (gobangService.inBounds(next)) {
					cursorPos = next;
				}
				continue;
			}
			if (key == ' ') {
				if (gobangService.board[cursorPos.y][cursorPos.x] != GobangService::NONE) {
                    continue;
				}
				auto result = gobangService.setPosition({ cursorPos.y, cursorPos.x }, currentPlayer);
				if (result != GobangService::PLAYING) {
					clearScreen();
					printMap();
					if (result == GobangService::DRAW) {
						std::cout << "平局." << '\n';
					}
					else {
						std::cout << "游戏结束, 获胜者: " << (result == GobangService::WHITE_WIN ? "白子" : "黑子") << "." << '\n';
					}
					std::cout << "请按任意键回到主页..." << '\n';
					_getch();
					clearScreen();
					changeScene(new MenuScene());
					return;
				}
				currentPlayer = currentPlayer == GobangService::BLACK ? GobangService::WHITE : GobangService::BLACK;
			}
		}
	}
};

class PvcScene : public Scene {
public:
	GobangService gobangService;
	GobangAI gobangAI = GobangAI(gobangService, GobangService::WHITE);
	Vector2i cursorPos;
	GobangService::CoordType currentPlayer = GobangService::BLACK;

	void printMap() {
		std::cout << "WASD 控制下棋位置. 空格确定下棋. 黑子为 '*', 白子为 'o'\n";
		std::cout << '|';
		for (int i = 0; i < gobangService.board.size() * 4 - 1; i++) {
			std::cout << '-';
		}
		std::cout << '|' << '\n';

		for (int i = 0; i < gobangService.board.size(); i++) {
			std::cout << '|';
			for (int j = 0; j < gobangService.board[i].size(); j++) {
				GobangService::CoordType col = gobangService.board[i][j];
				if (col == GobangService::NONE) {
					std::cout << "   ";
				}

				if (col == GobangService::BLACK) {
					std::cout << " * ";
				}

				if (col == GobangService::WHITE) {
					std::cout << " o ";
				}

				std::cout << '|';
			}
			std::cout << '\n' << '|';
			for (int j = 0; j < gobangService.board.size(); j++) {
				if (j == gobangService.board.size() - 1) {
					if (i == cursorPos.y && cursorPos.x == j) std::cout << "AAA";
					else std::cout << "---";
					continue;
				}
				if (i == cursorPos.y && cursorPos.x == j) std::cout << "AAA-";
				else std::cout << "----";
			}
			std::cout << '|' << '\n';
		}
		std::cout << "当前执棋者: " << (currentPlayer == GobangService::WHITE ? "AI" : "玩家") << "." << '\n';
	}

	const std::unordered_map<char, Vector2i> KEY_TO_DIRECTION = {
		{ 'w', Vector2i(0, -1) },
		{ 's', Vector2i(0, 1) },
		{ 'a', Vector2i(-1, 0) },
		{ 'd', Vector2i(1, 0) },
	};

	void game() override {
		while (true) {
			clearScreen();
			printMap();
			char key = _getch();
			if (KEY_TO_DIRECTION.find(key) != KEY_TO_DIRECTION.end()) {
				Vector2i next = cursorPos + KEY_TO_DIRECTION.at(key);
				if (gobangService.inBounds(next)) {
					cursorPos = next;
				}
				continue;
			}
			if (key == ' ') {
				if (gobangService.board[cursorPos.y][cursorPos.x] != GobangService::NONE) {
					continue;
				}
				auto result = gobangService.setPosition({ cursorPos.y, cursorPos.x }, currentPlayer);
				if (result != GobangService::PLAYING) {
					clearScreen();
					printMap();

					if (result == GobangService::DRAW) {
						std::cout << "平局." << '\n';
					}
					else {
						std::cout << "游戏结束, 获胜者: " << (result == GobangService::WHITE_WIN ? "AI" : "玩家") << "." << '\n';
					}
					std::cout << "请按任意键回到主页..." << '\n';
					_getch();
					clearScreen();
					changeScene(new MenuScene());
					return;
				}
				currentPlayer = currentPlayer == GobangService::BLACK ? GobangService::WHITE : GobangService::BLACK;
			}
			clearScreen();
			printMap();
			std::cout << "AI 思考中.." << '\n';
            Vector2i bestMove = gobangAI.getBestMove();
            auto result = gobangService.setPosition(bestMove, currentPlayer);
			if (result != GobangService::PLAYING) {
				clearScreen();
				printMap();
				if (result == GobangService::DRAW) {
					std::cout << "平局." << '\n';
				}
				else {
					std::cout << "游戏结束, 获胜者: " << (result == GobangService::WHITE_WIN ? "AI" : "玩家") << "." << '\n';
				}
				std::cout << "请按任意键回到主页..." << '\n';
				_getch();
				clearScreen();
				changeScene(new MenuScene());
				return;
			}
			currentPlayer = currentPlayer == GobangService::BLACK ? GobangService::WHITE : GobangService::BLACK;
		}
	}
};

void MenuScene::game()
{
	printMenu();
	while (true) {
		switch (_getch()) {
		case '1':
			clearScreen();
			changeScene(new PvpScene());
			return;
		case '2':
			clearScreen();
			changeScene(new PvcScene());
			return;
		case '0':
			clearScreen();
			changeScene(nullptr);
			return;
		}
	}
}




int main()
{
	currentScene = new MenuScene();
	while (currentScene != nullptr) {
		currentScene->game();
	}
}
