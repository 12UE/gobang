#pragma once

#include "resource.h"
#define _DBG_CONSOLER_PRINT
#include<vector>
#include"Log.h"
class chess_piece{
public:
	int x;// 棋子的坐标
	int y;
	COLORREF color;
	chess_piece(int x, int y, COLORREF color) :x(x), y(y), color(color) {}
};

#define BACKGROUND_COLOR RGB(238, 180, 0)
#define BLACK RGB(0, 0, 0)
#define WHITE RGB(255, 255, 255)

class chess_board {
private:
	std::vector<chess_piece> chess_pieces;
	int row;
	int col;
	int width;
	int height;
	int x;
	int y;
	HDC hdc;
public:
	chess_board() = default;
	~chess_board() = default;
	void Init(HDC _hdc,int _x, int _y, int _width, int _height, int _row, int _col) {
		hdc = _hdc;
		x = _x;
		y = _y;
		width = _width;
		height = _height;
		row = _row;
		col = _col;
	}
	
	void DrawBackground() {
		HBRUSH hBrush = CreateSolidBrush(BACKGROUND_COLOR);
		SelectObject(hdc, hBrush);
		Rectangle(hdc, x, y, x + width, y + height);
		DeleteObject(hBrush);
	}
	inline void DrawChessBoard() {
		int x1 = x;
		int y1 = y;
		int x2 = x + width;
		int y2 = y + height;
		int x3 = x + width / col;
		int y3 = y + height / row;
		for (int i = 0; i < row; i++) {
			MoveToEx(hdc, x1, y3, NULL);
			LineTo(hdc, x2, y3);
			y3 += height / row;
		}
		for (int i = 0; i < col; i++) {
			MoveToEx(hdc, x3, y1, NULL);
			LineTo(hdc, x3, y2);
			x3 += width / col;
		}
	}
	void DrawChessPiece() {
		for (auto& piece : chess_pieces) {
			int x = this->x + piece.x * width / col;
			int y = this->y + piece.y * height / row;
			HBRUSH hBrush = CreateSolidBrush(piece.color);
			SelectObject(hdc, hBrush);
			int r = (width / col / 2)-1;
			Ellipse(hdc, x - r, y - r, x + r, y + r);
			DeleteObject(hBrush);
		}
	}
	bool AddChessPiece(int x, int y, COLORREF color) {
		// 保证棋子的坐标在棋盘内 第一行和第一列不能下
		if (x < 1 || x > col - 1 || y < 1 || y > row - 1) return false;
		chess_pieces.push_back(chess_piece(x, y, color));
		return true;
	}
	bool CheckHasPiece(int x, int y) {
		for (auto& piece : chess_pieces) {
			if (piece.x == x && piece.y == y) {
				return true;
			}
		}
		return false;
	}
	//悔棋
	bool BackChessPiece() {
		if (chess_pieces.size() > 0) {
			chess_pieces.pop_back();
			return true;
		}
		return false;
	}
	void ClearChessPiece() {
		chess_pieces.clear();
	}
	COLORREF GetChessPieceColor(int x, int y) {
		for (auto& piece : chess_pieces) {
			if (piece.x == x && piece.y == y) {
				return piece.color;
			}
		}
		return BACKGROUND_COLOR;
	}
	
	//判断是否有人赢了
	bool IsWin(int x, int y, COLORREF color) {//x,y是最后一个下的棋子的坐标
		int count = 1;
		//横向
		for (int i = x - 1; i >= 0; i--) {
			if (GetChessPieceColor(i, y) == color) {
				count++;
			}
			else {
				break;
			}
		}
		for (int i = x + 1; i < col; i++) {
			if (GetChessPieceColor(i, y) == color) {
				count++;
			}
			else {
				break;
			}
		}
		if (count >= 5) {
			return true;
		}
		//纵向
		count = 1;
		for (int i = y - 1; i >= 0; i--) {
			if (GetChessPieceColor(x, i) == color) {
				count++;
			}
			else {
				break;
			}
		}
		for (int i = y + 1; i < row; i++) {
			if (GetChessPieceColor(x, i) == color) {
				count++;
			}
			else {
				break;
			}
		}
		if (count >= 5) {
			return true;
		}
		//左上右下
		count = 1;
		for (int i = x - 1, j = y - 1; i >= 0 && j >= 0; i--, j--) {
			if (GetChessPieceColor(i, j) == color) {
				count++;
			}
			else {
				break;
			}
		}
		for (int i = x + 1, j = y + 1; i < col && j < row; i++, j++) {
			if (GetChessPieceColor(i, j) == color) {
				count++;
			}
			else {
				break;
			}
		}
		if (count >= 5) {
			return true;
		}
		//右上左下
		count = 1;
		for (int i = x + 1, j = y - 1; i < col && j >= 0; i++, j--) {
			if (GetChessPieceColor(i, j) == color) {
				count++;
			}
			else {
				break;
			}
		}
		for (int i = x - 1, j = y + 1; i >= 0 && j < row; i--, j++) {
			if (GetChessPieceColor(i, j) == color) {
				count++;
			}
			else {
				break;
			}
		}
		if (count >= 5) {
			return true;
		}
		return false;
	}
	std::pair<int, int> GetChessPiecePos(int x, int y) {
		int x1 = this->x;
		int y1 = this->y;
		int x2 = this->x + width;
		int y2 = this->y + height;
		if (x < x1 || x > x2 || y < y1 || y > y2) {
			return std::make_pair(-1, -1);
		}
		int x3 = x1 + width / col;
		int y3 = y1 + height / row;
		for (int i = 1; i < col; i++) {
			if (x < x3) {
				break;
			}
			x3 += width / col;
		}
		for (int i = 1; i < row; i++) {
			if (y < y3) {
				break;
			}
			y3 += height / row;
		}
		return std::make_pair((x3 - x1) / (width / col), (y3 - y1) / (height / row));
	}
};