#include "pch.h"
#include "Game.h"
#include <algorithm>
Game::Game(const Window& window)
	: m_Grid{ 8,8, window.width, window.height },
	m_SelectedIndex{-1},
	m_SelectedMove{-1}
{
	Initialize();

}
Game::~Game()
{
	Cleanup();
}
void Game::InitializeTextures()
{
	for (int i = -6; i < 7; i++)
	{
		if (i != 0)
		{
			m_Textures.emplace(i, Texture("Resources/" + std::to_string(i) + ".png"));
		}
	}
}

void Game::Initialize()
{
	InitializeTextures();
	PrintBoard();
}
void Game::Cleanup()
{

}
void Game::Update(float elapsedSec)
{
	// Check keyboard state
	//const Uint8 *pStates = SDL_GetKeyboardState( nullptr );
	//if ( pStates[SDL_SCANCODE_RIGHT] )
	//{
	//	std::cout << "Right arrow key is down\n";
	//}
	//if ( pStates[SDL_SCANCODE_LEFT] && pStates[SDL_SCANCODE_UP])
	//{
	//	std::cout << "Left and up arrow keys are down\n";
	//}
	if (!m_WhiteToPlay)
	{
		UpdateTempBoard();
		auto bestMove = MiniMaxNoAB(2, m_TempBoard, false);
		if (bestMove.second.fromIdx != -1)
		{
			m_SelectedIndex = bestMove.second.fromIdx;
			m_SelectedMove = bestMove.second.LegalMove;
		}
	}
	PlayMove(m_SelectedIndex, m_SelectedMove);
}

void Game::CheckMate()
{
	if (m_GameOver) return;
	bool whiteInMate{ true }, blackInMate{ true };
	// Check if white's in mate
	for (int i{}; i < 63; ++i)
	{
		if (m_Board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] > 0)
		{
			if (!GetAllLegalMoves(i).empty())
				whiteInMate = false;
		}
	}
	if (whiteInMate)
	{
		m_GameOver = true;
		std::cout << "White is in checkmate\n";
		return;
	}
	// Check if black's in mate
	for (int i{}; i < 63; ++i)
	{
		if (m_Board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] < 0)
		{
			if (!GetAllLegalMoves(i).empty())
				blackInMate = false;
		}
	}
	if (blackInMate)
	{
		m_GameOver = true;
		std::cout << "Black is in checkmate\n";
		return;
	}

	// Print if in check
	int whiteKing{};
	for (int i{}; i < 63; ++i)
	{
		// Find White's king index
		if (m_Board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] == 5)
		{
			whiteKing = i;
			break;
		}
	}
	// Als de witte king niet meer in een threat tile staat true returnen
	if (m_BlackThreats[m_Grid.GetRowFromIdx(whiteKing)][m_Grid.GetColFromIdx(whiteKing)] != 0)
	{
		m_WhiteInCheck = true;
		std::cout << "White's king is in check\n";
		return;
	}
	m_WhiteInCheck = false;

		int blackKing{};
	for (int i{}; i < 63; ++i)
	{
		// Find Black's king index
		if (m_Board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] == -5)
		{
			blackKing = i;
			break;
		}
	}
	if (m_WhiteThreats[m_Grid.GetRowFromIdx(blackKing)][m_Grid.GetColFromIdx(blackKing)] != 0)
	{
		m_BlackInCheck = true;
		std::cout << "Black's king is in check\n";
		return;
	}
	m_BlackInCheck = false;
}

void Game::PlayMove(int startIdx, int destIdx)
{
	if (m_SelectedIndex == -1) return;
	if (m_WhiteToPlay && m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] < 0)
	{
		m_SelectedIndex = -1;
		m_SelectedMove = -1;
		return;
	}
	if (!m_WhiteToPlay && m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] > 0)
	{

		m_SelectedIndex = -1;
		m_SelectedMove = -1;
		return;
	}
	auto legalMoves = GetAllLegalMoves(m_SelectedIndex,m_Board);
	if (legalMoves.empty())
	{
		m_SelectedIndex = -1;
		m_SelectedMove = -1;
		return;
	}
	if (std::find(legalMoves.begin(), legalMoves.end(), m_SelectedMove) == legalMoves.end())
	{
		if (m_SelectedMove > 0)
		{
			m_SelectedIndex = -1;
			m_SelectedMove = -1;
		}
		return;
	}
	MovePiece(m_SelectedIndex, m_SelectedMove);
	
	CheckCastlingMovement();

	UpdateThreats();
	//PrintThreatMatrices();
	
	m_SelectedIndex = -1;
	m_SelectedMove = -1;
	m_WhiteToPlay = !m_WhiteToPlay;

	CheckMate();
}

void Game::CheckCastlingMovement()
{
	// Castling checks
	// White king has moved
	if (m_Board[m_Grid.GetRowFromIdx(m_SelectedMove)][m_Grid.GetColFromIdx(m_SelectedMove)] == 5)
		if (!m_HasWhiteKingMoved)
			m_HasWhiteKingMoved = true;
	// Black king has moved
	if (m_Board[m_Grid.GetRowFromIdx(m_SelectedMove)][m_Grid.GetColFromIdx(m_SelectedMove)] == -5)
		if (!m_HasBlackKingMoved)
			m_HasBlackKingMoved = true;
	// White right rook has moved
	if (m_Board[m_Grid.GetRowFromIdx(m_SelectedMove)][m_Grid.GetColFromIdx(m_SelectedMove)] == 1 && m_SelectedIndex == 7)
		if (!m_HasWhiteRightRookMoved)
			m_HasWhiteRightRookMoved = true;
	// White left rook has moved
	if (m_Board[m_Grid.GetRowFromIdx(m_SelectedMove)][m_Grid.GetColFromIdx(m_SelectedMove)] == 1 && m_SelectedIndex == 0)
		if (!m_HasWhiteLeftRookMoved)
			m_HasWhiteLeftRookMoved = true;
	// Black right rook has moved
	if (m_Board[m_Grid.GetRowFromIdx(m_SelectedMove)][m_Grid.GetColFromIdx(m_SelectedMove)] == -1 && m_SelectedIndex == 63)
		if (!m_HasBlackRightRookMoved)
			m_HasBlackRightRookMoved = true;
	// Black left rook has moved
	if (m_Board[m_Grid.GetRowFromIdx(m_SelectedMove)][m_Grid.GetColFromIdx(m_SelectedMove)] == -1 && m_SelectedIndex == 56)
		if (!m_HasBlackLeftRookMoved)
			m_HasBlackLeftRookMoved = true;
}

void Game::UpdateThreats()
{
	// Update threat matrices:
	// First reset the threat matrices to zero matrices
	for (int i{}; i < 8; ++i)
	{
		for (int j{}; j < 8; ++j)
		{
			m_WhiteThreats[i][j] = 0;
			m_BlackThreats[i][j] = 0;
		}
	}
	// Then refill it with new values
	for (int i{}; i < 63; ++i)
	{
		int piece = m_Board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)];
		// Black threat
		if (piece < 0)
		{
			// If piece is a pawn treat it differently (they can only take diagonally)
			if (piece == -6)
			{
				if(i - 9 >= 0)
					if (m_Grid.GetColFromIdx(i - 9) < m_Grid.GetColFromIdx(i))
						if (i - 9 >= 0 && m_Board[m_Grid.GetRowFromIdx(i - 9)][m_Grid.GetColFromIdx(i - 9)] >= 0)
							m_BlackThreats[m_Grid.GetRowFromIdx(i - 9)][m_Grid.GetColFromIdx(i - 9)] = -1;
				if(i - 7 >= 0)
					if (m_Grid.GetColFromIdx(i - 7) > m_Grid.GetColFromIdx(i))
						if (i - 7 >= 0 && m_Board[m_Grid.GetRowFromIdx(i - 7)][m_Grid.GetColFromIdx(i - 7)] >= 0)
							m_BlackThreats[m_Grid.GetRowFromIdx(i - 7)][m_Grid.GetColFromIdx(i - 7)] = -1;
				continue;
			}
			for (auto legalMove : GetAllLegalMoves(i))
			{
				m_BlackThreats[m_Grid.GetRowFromIdx(legalMove)][m_Grid.GetColFromIdx(legalMove)] = -1;
			}
		}
		// White threat
		else if (piece > 0)
		{
			if (piece == 6)
			{
				if(i + 9 <= 63)
					if (m_Grid.GetColFromIdx(i + 9) > m_Grid.GetColFromIdx(i))
						if (i + 9 <= 63 && m_Board[m_Grid.GetRowFromIdx(i + 9)][m_Grid.GetColFromIdx(i + 9)] <= 0)
							m_WhiteThreats[m_Grid.GetRowFromIdx(i + 9)][m_Grid.GetColFromIdx(i + 9)] = 1;
				if(i + 7 <= 63)
					if (m_Grid.GetColFromIdx(i + 7) < m_Grid.GetColFromIdx(i))
						if (i + 7 <= 63 && m_Board[m_Grid.GetRowFromIdx(i + 7)][m_Grid.GetColFromIdx(i + 7)] <= 0)
							m_WhiteThreats[m_Grid.GetRowFromIdx(i + 7)][m_Grid.GetColFromIdx(i + 7)] = 1;
				continue;
			}
			for (auto legalMove : GetAllLegalMoves(i))
			{
				m_WhiteThreats[m_Grid.GetRowFromIdx(legalMove)][m_Grid.GetColFromIdx(legalMove)] = 1;
			}
		}
	}
}

bool Game::EvaluateThreats(int startIdx, int newIdx, bool isWhite) const
{
	int blackThreats[8][8]{ 0 };
	int whiteThreats[8][8]{ 0 };
	int board[8][8]{ 0 };
	for (int i{}; i < 8; ++i)
	{
		for (int j{}; j < 8; ++j)
		{
			board[i][j] = m_Board[i][j];
		}
	}
	// Now adjust the piece based on its legal move to evaluate threats
	board[m_Grid.GetRowFromIdx(newIdx)][m_Grid.GetColFromIdx(newIdx)] = board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)];
	board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] = 0;

	for (int i{}; i < 63; ++i)
	{
		int piece = board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)];
		// Black threat
		if (piece < 0)
		{
			// If piece is a pawn treat it differently (they can only take diagonally)
			if (piece == -6)
			{
				if(i - 9 >= 0)
					if (m_Grid.GetColFromIdx(i - 9) < m_Grid.GetColFromIdx(i))
						if (i - 9 >= 0 && board[m_Grid.GetRowFromIdx(i - 9)][m_Grid.GetColFromIdx(i - 9)] >= 0)
							blackThreats[m_Grid.GetRowFromIdx(i - 9)][m_Grid.GetColFromIdx(i - 9)] = -1;
				if(i - 7 >= 0)
					if (m_Grid.GetColFromIdx(i - 7) > m_Grid.GetColFromIdx(i))
						if (i - 7 >= 0 && board[m_Grid.GetRowFromIdx(i - 7)][m_Grid.GetColFromIdx(i - 7)] >= 0)
							blackThreats[m_Grid.GetRowFromIdx(i - 7)][m_Grid.GetColFromIdx(i - 7)] = -1;
				continue;
			}
			for (auto legalMove : GetAllLegalMoves(i, board))
			{
				blackThreats[m_Grid.GetRowFromIdx(legalMove)][m_Grid.GetColFromIdx(legalMove)] = -1;
			}
		}
		// White threat
		else if (piece > 0)
		{
			if (piece == 6)
			{
				if(i + 9 <= 63)
					if (m_Grid.GetColFromIdx(i + 9) > m_Grid.GetColFromIdx(i))
						if (i + 9 <= 63 && board[m_Grid.GetRowFromIdx(i + 9)][m_Grid.GetColFromIdx(i + 9)] <= 0)
							whiteThreats[m_Grid.GetRowFromIdx(i + 9)][m_Grid.GetColFromIdx(i + 9)] = 1;
				if(i + 7 <= 63)
					if (m_Grid.GetColFromIdx(i + 7) < m_Grid.GetColFromIdx(i))
						if (i + 7 <= 63 && board[m_Grid.GetRowFromIdx(i + 7)][m_Grid.GetColFromIdx(i + 7)] <= 0)
							whiteThreats[m_Grid.GetRowFromIdx(i + 7)][m_Grid.GetColFromIdx(i + 7)] = 1;
				continue;
			}
			for (auto legalMove : GetAllLegalMoves(i, board))
			{
				whiteThreats[m_Grid.GetRowFromIdx(legalMove)][m_Grid.GetColFromIdx(legalMove)] = 1;
			}
		}
	}

	if (isWhite)
	{
		int whiteKing{};
		for (int i{}; i < 63; ++i)
		{
			// Find White's king index
			if (board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] == 5)
			{
				whiteKing = i;
				break;
			}
		}
		// Als de witte king niet meer in een threat tile staat true returnen
		if (blackThreats[m_Grid.GetRowFromIdx(whiteKing)][m_Grid.GetColFromIdx(whiteKing)] != 0)
		{
			return false;
		}
	}

	if (!isWhite)
	{
		int blackKing{};
		for (int i{}; i < 63; ++i)
		{
			// Find Black's king index
			if (board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] == -5)
			{
				blackKing = i;
				break;
			}
		}
		if (whiteThreats[m_Grid.GetRowFromIdx(blackKing)][m_Grid.GetColFromIdx(blackKing)] != 0)
		{
			return false;
		}
	}

	return true;
}

void Game::MovePiece(int startIdx, int destIdx)
{
	if (Castle(startIdx, destIdx))
		return;

	if (Promote(startIdx, destIdx))
		return;

	CheckEnPassant(startIdx, destIdx);

	m_Board[m_Grid.GetRowFromIdx(destIdx)][m_Grid.GetColFromIdx(destIdx)] = m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)];
	m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] = 0;
}
void Game::MovePieceAI(int startIdx, int destIdx)
{
	m_TempBoard[m_Grid.GetRowFromIdx(destIdx)][m_Grid.GetColFromIdx(destIdx)] = m_TempBoard[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)];
	m_TempBoard[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] = 0;
}
bool Game::Castle(int startIdx, int destIdx)
{
	// Check for castle:
	// White king
	if (m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] == 5)
	{
		// Right castle
		if (destIdx - startIdx == 2)
		{
			m_Board[m_Grid.GetRowFromIdx(destIdx)][m_Grid.GetColFromIdx(destIdx)] = m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)];
			m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] = 0;
			m_Board[m_Grid.GetRowFromIdx(destIdx - 1)][m_Grid.GetColFromIdx(destIdx - 1)] = 1;
			m_Board[m_Grid.GetRowFromIdx(7)][m_Grid.GetColFromIdx(7)] = 0;
			return true;
		}
		// Left castle
		else if (startIdx - destIdx == 2)
		{
			m_Board[m_Grid.GetRowFromIdx(destIdx)][m_Grid.GetColFromIdx(destIdx)] = m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)];
			m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] = 0;
			m_Board[m_Grid.GetRowFromIdx(destIdx + 1)][m_Grid.GetColFromIdx(destIdx + 1)] = 1;
			m_Board[m_Grid.GetRowFromIdx(0)][m_Grid.GetColFromIdx(0)] = 0;
			return true;
		}
	}
	// Black king
	if (m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] == -5)
	{
		// Right castle
		if (destIdx - startIdx == 2)
		{
			m_Board[m_Grid.GetRowFromIdx(destIdx)][m_Grid.GetColFromIdx(destIdx)] = m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)];
			m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] = 0;
			m_Board[m_Grid.GetRowFromIdx(destIdx - 1)][m_Grid.GetColFromIdx(destIdx - 1)] = -1;
			m_Board[m_Grid.GetRowFromIdx(63)][m_Grid.GetColFromIdx(63)] = 0;
			return true;
		}
		// Left castle
		else if (startIdx - destIdx == 2)
		{
			m_Board[m_Grid.GetRowFromIdx(destIdx)][m_Grid.GetColFromIdx(destIdx)] = m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)];
			m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] = 0;
			m_Board[m_Grid.GetRowFromIdx(destIdx + 1)][m_Grid.GetColFromIdx(destIdx + 1)] = -1;
			m_Board[m_Grid.GetRowFromIdx(56)][m_Grid.GetColFromIdx(56)] = 0;
			return true;
		}
	}
	return false;
}

bool Game::Promote(int startIdx, int destIdx)
{
	// Check if piece is a pawn and check if it can promote if yes
	// White pawn
	if (m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] == 6)
	{
		// If pawn has reached the top row
		if (m_Grid.GetRowFromIdx(destIdx) == 7)
		{
			int input{};
			std::cout << "Promotion:\n1 for Rook\n2 for Knight\n3 for Bishop\n4 for Queen\n";
			while (input <= 0 || input > 4)
			{
				std::cin >> input;
			}
			m_Board[m_Grid.GetRowFromIdx(destIdx)][m_Grid.GetColFromIdx(destIdx)] = input;
			m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] = 0;
			return true;
		}
	}
	// Black paw,
	if (m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] == -6)
	{
		// If pawn has reached the top row
		if (m_Grid.GetRowFromIdx(destIdx) == 0)
		{
			int input{};
			std::cout << "Promotion:\n1 for Rook\n2 for Knight\n3 for Bishop\n4 for Queen\n";
			while (input <= 0 || input > 4)
			{
				std::cin >> input;
			}
			m_Board[m_Grid.GetRowFromIdx(destIdx)][m_Grid.GetColFromIdx(destIdx)] = -input;
			m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] = 0;
			return true;
		}
	}
	return false;
}

void Game::CheckEnPassant(int startIdx, int destIdx)
{
	// En passant
	// White pawn
	if (m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] == 6)
	{
		// Take in passing
		if (m_EnPassant)
		{
			if (destIdx - 8 == m_EnPassantIdx)
			{
				m_Board[m_Grid.GetRowFromIdx(m_EnPassantIdx)][m_Grid.GetColFromIdx(m_EnPassantIdx)] = 0;
			}
		}

		if (destIdx - startIdx == 16)
		{
			// Activate en passant
			m_EnPassant = true;
			m_EnPassantIdx = destIdx;
		}
		else
			m_EnPassant = false;
	}
	// Black paw,
	else if (m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] == -6)
	{
		if (m_EnPassant)
		{
			if (destIdx + 8 == m_EnPassantIdx)
			{
				m_Board[m_Grid.GetRowFromIdx(m_EnPassantIdx)][m_Grid.GetColFromIdx(m_EnPassantIdx)] = 0;
			}
		}

		if (startIdx - destIdx == 16)
		{
			m_EnPassant = true;
			m_EnPassantIdx = destIdx;
		}
		else
			m_EnPassant = false;
	}
	else
	{
		m_EnPassant = false;
	}
}

std::pair<int, MoveStruct> Game::MiniMaxNoAB(int depth, int(*board)[8], bool IsPlayer, bool isMaximizingPlayer)
{
	if (depth == 0)
		return { -1,MoveStruct{-1,-1} };
	//Going recursive search possible moves
	MoveStruct bestMoveIdx;
	std::vector<MoveStruct> possibleMoves{}; //all possible moves for all idx
	for (int i{ 0 }; i < 63; i++)
	{
		std::vector<int> legalMovesFromidx = GetAllLegalMoves(i);
		for (int i = 0; i < legalMovesFromidx.size(); i++)
			possibleMoves.push_back(MoveStruct{ i,legalMovesFromidx[i] });
	}
	std::sort(begin(possibleMoves), end(possibleMoves), [](MoveStruct a, MoveStruct b) //setting random order for all possible moves
		{
			int random = rand() % 1;
			if (random == 0)
				return a.LegalMove <= b.LegalMove;
			else
				return a.LegalMove > b.LegalMove;
		});
	int bestMoveValue = isMaximizingPlayer ? 10000 : -10000; //setting best value to max depending if player is trying to max or min his score
	//searching through all possible moves
	for (int i = 0; i < possibleMoves.size(); i++)
	{
		int moveFrom = possibleMoves.at(i).fromIdx;
		int moveTo = possibleMoves.at(i).LegalMove;
		MovePieceAI(moveFrom, moveTo);
		int value = MiniMaxNoAB(depth - 1, board, IsPlayer, !isMaximizingPlayer).first;

		if (isMaximizingPlayer)
		{
			//Look for moves that maximize position
			if (value > bestMoveValue)
			{
				bestMoveValue = value;
				bestMoveIdx = { moveFrom,moveTo };
			}
		}
		else
		{
			if (value < bestMoveValue)
			{
				bestMoveValue = value;
				bestMoveIdx = { moveFrom,moveTo };
			}
		}
	}
	return { bestMoveValue,bestMoveIdx };

}

void Game::PrintBoard()
{
	for (int i = 7; i >= 0; i--)
	{
		for (int j = 0; j < 8; j++) //reverse because the 0,0 index of the array is bottom left of the visual board
		{
			if (m_Board[i][j] >= 0)
				std::cout << " ";
			std::cout << m_Board[i][j] << " ";
		}
		std::cout << "\n";
	}
}

void Game::PrintThreatMatrices()
{
	// Print threat matrices
	std::cout << "White threats:\n";
	for (int i = 7; i >= 0; i--)
	{
		for (int j = 0; j < 8; j++) //reverse because the 0,0 index of the array is bottom left of the visual board
		{
			if (m_WhiteThreats[i][j] >= 0)
				std::cout << " ";
			std::cout << m_WhiteThreats[i][j] << " ";
		}
		std::cout << "\n";
	}
	std::cout << "Black threats:\n";
	for (int i = 7; i >= 0; i--)
	{
		for (int j = 0; j < 8; j++) //reverse because the 0,0 index of the array is bottom left of the visual board
		{
			if (m_BlackThreats[i][j] >= 0)
				std::cout << " ";
			std::cout << m_BlackThreats[i][j] << " ";
		}
		std::cout << "\n";
	}
}

void Game::Draw() const
{
	ClearBackground();
	m_Grid.DrawGrid(true);
	// Draw selected tile red
	utils::SetColor(Color4f{ 1.f,0.f,0.f,1.f });
	if (m_SelectedIndex >= 0)
		utils::FillRect(m_Grid.GetCell(m_SelectedIndex));
	DrawPieces();
	DrawLegalMoves();
	DrawCheckedKing();
}

void Game::DrawCheckedKing() const
{
	// Draw king's tile red if in check
	int kingIdx{};
	if (m_BlackInCheck)
	{
		for (int i{}; i < 63; ++i)
		{
			if (m_Board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] == -5)
			{
				kingIdx = i;
				break;
			}
		}
	}
	else if (m_WhiteInCheck)
	{
		for (int i{}; i < 63; ++i)
		{
			if (m_Board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] == 5)
			{
				kingIdx = i;
				break;
			}
		}
	}
	if (m_WhiteInCheck || m_BlackInCheck)
	{
		utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 100.f / 255.f });
		utils::FillRect(m_Grid.GetCell(kingIdx));
	}
}

void Game::DrawPieces() const
{
	for (int i{ 0 }; i < 8; i++)
	{
		for (int j{ 0 }; j < 8; j++)
		{
			if (m_Board[i][j] != 0)
				m_Textures.at(m_Board[i][j]).Draw(Rectf{ m_Grid.GetPosFromIdx(convert2DTo1D(i,j)).x,m_Grid.GetPosFromIdx(convert2DTo1D(i,j)).y,size * 2,size * 2 }, Rectf{ 0,0,68,68 });
		}
	}
}
void Game::DrawLegalMoves() const
{
	for (int idx : GetAllLegalMoves(m_SelectedIndex))
	{
		utils::SetColor(Color4f{ 0.f, 0.f, 0.f, 100.f / 255.f });
		utils::FillEllipse(m_Grid.GetPosFromIdx(idx).x + (m_Grid.GetCellWidth() / 2), m_Grid.GetPosFromIdx(idx).y + (
			m_Grid.GetCellWidth() / 2), 20.f, 20.f);
	}
}

void Game::ProcessKeyDownEvent(const SDL_KeyboardEvent& e)
{
	//std::cout << "KEYDOWN event: " << e.keysym.sym << std::endl;
}

void Game::ProcessKeyUpEvent(const SDL_KeyboardEvent& e)
{

}

void Game::ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
{
	//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;
}

void Game::ProcessMouseDownEvent(const SDL_MouseButtonEvent& e)
{
	//std::cout << "MOUSEBUTTONDOWN event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
}

void Game::ProcessMouseUpEvent(const SDL_MouseButtonEvent& e)
{
	switch (e.button)
	{
	case SDL_BUTTON_LEFT:
		Point2f mousePos = Point2f{ float(e.x), float(e.y) };
		if (m_SelectedIndex == -1)
		{
			m_SelectedIndex = m_Grid.GetIndexFromPos(mousePos);
			//std::cout << "Index " << m_SelectedIndex << " selected\n";
		//	auto moves = GetAllLegalMoves(m_Grid.GetIndexFromPos(mousePos));
		//	std::cout << "Legal moves: ";
		//	std::copy(moves.begin(), moves.end(), std::ostream_iterator<int>{std::cout, " "});
		//	std::cout << '\n';
		}
		else
		{
			m_SelectedMove = m_Grid.GetIndexFromPos(mousePos);
		}
		break;
	}
}

void Game::ClearBackground() const
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Game::CheckCheck(int index, int king, std::vector<int>& moves) const
{
	// Check for check(mate)
	int kingIdx{};
	for (int i{}; i < 63; ++i)
	{
		// Find White's king index
		if (m_Board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] == king)
		{
			kingIdx = i;
			break;
		}
	}
	for (int i{}; i < 63; ++i)
	{
		// Als op deze tile een zwarte piece staat:
		int attacker = m_Board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)];
		if (king > 0 && attacker < 0)
		{
			//auto attackerMoves = GetAllLegalMoves(i);
			// Als White's king in check staat
			//if (std::find(attackerMoves.begin(), attackerMoves.end(), kingIdx) != attackerMoves.end())
			//if(m_BlackThreats[m_Grid.GetRowFromIdx(kingIdx)][m_Grid.GetColFromIdx(kingIdx)] != 0)
			//{
				// Hier alle huidige legal moves van de pawn evalueren:
				std::vector<int>::iterator it{ moves.begin() };
				while (it != moves.end())
				{
					if (!EvaluateThreats(index, *it, true))
					{
						it = moves.erase(it);
						continue;
					}
					++it;
				}
			//}
		}
		else if (king < 0 && attacker > 0)
		{
			//auto attackerMoves = GetAllLegalMoves(i);
			// Als White's king in check staat
			//if (std::find(attackerMoves.begin(), attackerMoves.end(), kingIdx) != attackerMoves.end())
			//if(m_WhiteThreats[m_Grid.GetRowFromIdx(kingIdx)][m_Grid.GetColFromIdx(kingIdx)] != 0)
			//{
				// Hier alle huidige legal moves van de pawn evalueren:
				std::vector<int>::iterator it{ moves.begin() };
				while (it != moves.end())
				{
					if (!EvaluateThreats(index, *it, false))
					{
						it = moves.erase(it);
						continue;
					}
					++it;
				}
			//}
		}
	}
}

// -------------------------------------------------------
//					LEGAL MOVE GETTERS
// -------------------------------------------------------

void Game::UpdateTempBoard()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			m_TempBoard[i][j] = m_Board[i][j];
		}
	}
}

std::vector<int> Game::GetAllLegalMoves(int index) const
{
	std::vector<int> moves;
	if (index < 0 || index > 63) return moves;
	// Checken of er op die indexpositie effectief een piece staat:
	int pieceOnIdx = m_Board[m_Grid.GetRowFromIdx(index)][m_Grid.GetColFromIdx(index)];
	if (pieceOnIdx == 0) return moves;

	switch (pieceOnIdx)
	{
		// White Pawn
	case 6:
		moves = GetWhitePawnMoves(index);
		break;
		// Black Pawn
	case -6:
		moves = GetBlackPawnMoves(index);
		break;
		// White Rook
	case 1:
		moves = GetWhiteRookMoves(index);
		break;
		// Black Rook
	case -1:
		moves = GetBlackRookMoves(index);
		break;
		// White Knight
	case 2:
		moves = GetWhiteKnightMoves(index);
		break;
		// Black Knight
	case -2:
		moves = GetBlackKnightMoves(index);
		break;
		// White Bishop
	case 3:
		moves = GetWhiteBishopMoves(index);
		break;
		// Black Bishop
	case -3:
		moves = GetBlackBishopMoves(index);
		break;
		// White Queen
	case 4:
		moves = GetWhiteQueenMoves(index);
		break;
		// Black Queen
	case -4:
		moves = GetBlackQueenMoves(index);
		break;
		// White King
	case 5:
		moves = GetWhiteKingMoves(index);
		break;
		// Black King
	case -5:
		moves = GetBlackKingMoves(index);
		break;
	}

	return moves;
}
std::vector<int> Game::GetAllLegalMoves(int index, int board[8][8]) const
{
	std::vector<int> moves;
	if (index < 0 || index > 63) return moves;
	// Checken of er op die indexpositie effectief een piece staat:
	int pieceOnIdx = board[m_Grid.GetRowFromIdx(index)][m_Grid.GetColFromIdx(index)];
	if (pieceOnIdx == 0) return moves;

	switch (pieceOnIdx)
	{
		// White Pawn
	case 6:
		moves = GetWhitePawnMoves(index, board);
		break;
		// Black Pawn
	case -6:
		moves = GetBlackPawnMoves(index, board);
		break;
		// White Rook
	case 1:
		moves = GetWhiteRookMoves(index, board);
		break;
		// Black Rook
	case -1:
		moves = GetBlackRookMoves(index, board);
		break;
		// White Knight
	case 2:
		moves = GetWhiteKnightMoves(index, board);
		break;
		// Black Knight
	case -2:
		moves = GetBlackKnightMoves(index, board);
		break;
		// White Bishop
	case 3:
		moves = GetWhiteBishopMoves(index, board);
		break;
		// Black Bishop
	case -3:
		moves = GetBlackBishopMoves(index, board);
		break;
		// White Queen
	case 4:
		moves = GetWhiteQueenMoves(index, board);
		break;
		// Black Queen
	case -4:
		moves = GetBlackQueenMoves(index, board);
		break;
		// White King
	case 5:
		moves = GetWhiteKingMoves(index, board);
		break;
		// Black King
	case -5:
		moves = GetBlackKingMoves(index, board);
		break;
	}

	return moves;
}

// ------------------- PAWNS -------------------------

std::vector<int> Game::GetWhitePawnMoves(int index) const
{
	std::vector<int> moves;
	// Als de pawn op een rij staat boven de 2de rij (hier 1 want rijen beginnen op rij 0) betekent dat dat die pawn al heeft bewogen
	if (m_Grid.GetRowFromIdx(index) > 1 && m_Grid.GetRowFromIdx(index) < 7)
	{
		// Als er vlak boven de pawn niets staat kan die daar bewegen:
		if (m_Board[m_Grid.GetRowFromIdx(index + 8)][m_Grid.GetColFromIdx(index + 8)] == 0)
			moves.push_back(index + 8);
		// Als er op de diagonalen boven de pawn een zwarte piece staat kan die daar bewegen:
		if(m_Grid.GetColFromIdx(index + 7) < m_Grid.GetColFromIdx(index))
			if (m_Board[m_Grid.GetRowFromIdx(index + 7)][m_Grid.GetColFromIdx(index + 7)] < 0)
				moves.push_back(index + 7);
		if (m_Grid.GetColFromIdx(index + 9) > m_Grid.GetColFromIdx(index))
			if (m_Board[m_Grid.GetRowFromIdx(index + 9)][m_Grid.GetColFromIdx(index + 9)] < 0)
				moves.push_back(index + 9);
	}
	// Hieronder staan de legal moves van de pawn als die nog niet bewogen is en dus 2 tiles omhoog kan gaan
	else
	{
		if (index + 8 <= 63)
		{
			if (m_Board[m_Grid.GetRowFromIdx(index + 8)][m_Grid.GetColFromIdx(index + 8)] == 0)
			{
				moves.push_back(index + 8);
				if (m_Board[m_Grid.GetRowFromIdx(index + 16)][m_Grid.GetColFromIdx(index + 16)] == 0)
					moves.push_back(index + 16);
			}
		}
		if(index + 7 <= 63)
			if (m_Grid.GetColFromIdx(index + 7) < m_Grid.GetColFromIdx(index))
				if (m_Board[m_Grid.GetRowFromIdx(index + 7)][m_Grid.GetColFromIdx(index + 7)] < 0)
					moves.push_back(index + 7);
		if(index + 9 <= 63)
			if (m_Grid.GetColFromIdx(index + 9) > m_Grid.GetColFromIdx(index))
				if (m_Board[m_Grid.GetRowFromIdx(index + 9)][m_Grid.GetColFromIdx(index + 9)] < 0)
					moves.push_back(index + 9);
	}

	if (m_EnPassant)
	{
		if (m_EnPassantIdx == index + 1)
			moves.push_back(index + 9);
		if (m_EnPassantIdx == index - 1)
			moves.push_back(index + 7);
	}

	CheckCheck(index, 5, moves);

	return moves;
}

std::vector<int> Game::GetBlackPawnMoves(int index) const
{
	std::vector<int> moves;
	if (m_Grid.GetRowFromIdx(index) < 6 && m_Grid.GetRowFromIdx(index) > 0)
	{
		if (m_Board[m_Grid.GetRowFromIdx(index - 8)][m_Grid.GetColFromIdx(index - 8)] == 0)
			moves.push_back(index - 8);
		if (m_Grid.GetColFromIdx(index - 7) > m_Grid.GetColFromIdx(index))
			if (m_Board[m_Grid.GetRowFromIdx(index - 7)][m_Grid.GetColFromIdx(index - 7)] > 0)
				moves.push_back(index - 7);
		if (m_Grid.GetColFromIdx(index - 9) < m_Grid.GetColFromIdx(index))
			if (m_Board[m_Grid.GetRowFromIdx(index - 9)][m_Grid.GetColFromIdx(index - 9)] > 0)
				moves.push_back(index - 9);
	}
	else
	{
		if (index - 8 >= 0)
		{
			if (m_Board[m_Grid.GetRowFromIdx(index - 8)][m_Grid.GetColFromIdx(index - 8)] == 0)
			{
				moves.push_back(index - 8);
				if (m_Board[m_Grid.GetRowFromIdx(index - 16)][m_Grid.GetColFromIdx(index - 16)] == 0)
					moves.push_back(index - 16);
			}
		}
		if(index - 7 >= 0)
			if (m_Grid.GetColFromIdx(index - 7) > m_Grid.GetColFromIdx(index))
				if (m_Board[m_Grid.GetRowFromIdx(index - 7)][m_Grid.GetColFromIdx(index - 7)] > 0)
					moves.push_back(index - 7);
		if(index - 9 >= 0)
			if (m_Grid.GetColFromIdx(index - 9) < m_Grid.GetColFromIdx(index))
				if (m_Board[m_Grid.GetRowFromIdx(index - 9)][m_Grid.GetColFromIdx(index - 9)] > 0)
					moves.push_back(index - 9);
	}

	if (m_EnPassant)
	{
		if (m_EnPassantIdx == index + 1)
			moves.push_back(index - 7);
		if (m_EnPassantIdx == index - 1)
			moves.push_back(index - 9);
	}

	CheckCheck(index, -5, moves);

	return moves;
}

std::vector<int> Game::GetWhitePawnMoves(int index, int board[8][8]) const
{
	std::vector<int> moves;
	// Als de pawn op een rij staat boven de 2de rij (hier 1 want rijen beginnen op rij 0) betekent dat dat die pawn al heeft bewogen
	if (m_Grid.GetRowFromIdx(index) > 1 && m_Grid.GetRowFromIdx(index) < 7)
	{
		// Als er vlak boven de pawn niets staat kan die daar bewegen:
		if (board[m_Grid.GetRowFromIdx(index + 8)][m_Grid.GetColFromIdx(index + 8)] == 0)
			moves.push_back(index + 8);
		// Als er op de diagonalen boven de pawn een zwarte piece staat kan die daar bewegen:
		if (m_Grid.GetColFromIdx(index + 7) < m_Grid.GetColFromIdx(index))
			if (board[m_Grid.GetRowFromIdx(index + 7)][m_Grid.GetColFromIdx(index + 7)] < 0)
				moves.push_back(index + 7);
		if (m_Grid.GetColFromIdx(index + 9) > m_Grid.GetColFromIdx(index))
			if (board[m_Grid.GetRowFromIdx(index + 9)][m_Grid.GetColFromIdx(index + 9)] < 0)
				moves.push_back(index + 9);
	}
	// Hieronder staan de legal moves van de pawn als die nog niet bewogen is en dus 2 tiles omhoog kan gaan
	else
	{
		if (board[m_Grid.GetRowFromIdx(index + 8)][m_Grid.GetColFromIdx(index + 8)] == 0)
		{
			moves.push_back(index + 8);
			if (board[m_Grid.GetRowFromIdx(index + 16)][m_Grid.GetColFromIdx(index + 16)] == 0)
				moves.push_back(index + 16);
		}
		if (m_Grid.GetColFromIdx(index + 7) < m_Grid.GetColFromIdx(index))
			if (board[m_Grid.GetRowFromIdx(index + 7)][m_Grid.GetColFromIdx(index + 7)] < 0)
				moves.push_back(index + 7);
		if (m_Grid.GetColFromIdx(index + 9) > m_Grid.GetColFromIdx(index))
			if (board[m_Grid.GetRowFromIdx(index + 9)][m_Grid.GetColFromIdx(index + 9)] < 0)
				moves.push_back(index + 9);
	}

	//CheckCheck(index, 5, moves);

	return moves;
}

std::vector<int> Game::GetBlackPawnMoves(int index, int board[8][8]) const
{
	std::vector<int> moves;
	if (m_Grid.GetRowFromIdx(index) < 6 && m_Grid.GetRowFromIdx(index) > 0)
	{
		if (board[m_Grid.GetRowFromIdx(index - 8)][m_Grid.GetColFromIdx(index - 8)] == 0)
			moves.push_back(index - 8);
		if (m_Grid.GetColFromIdx(index - 7) > m_Grid.GetColFromIdx(index))
			if (board[m_Grid.GetRowFromIdx(index - 7)][m_Grid.GetColFromIdx(index - 7)] > 0)
				moves.push_back(index - 7);
		if (m_Grid.GetColFromIdx(index - 9) < m_Grid.GetColFromIdx(index))
			if (board[m_Grid.GetRowFromIdx(index - 9)][m_Grid.GetColFromIdx(index - 9)] > 0)
				moves.push_back(index - 9);
	}
	else
	{
		if (board[m_Grid.GetRowFromIdx(index - 8)][m_Grid.GetColFromIdx(index - 8)] == 0)
		{
			moves.push_back(index - 8);
			if (board[m_Grid.GetRowFromIdx(index - 16)][m_Grid.GetColFromIdx(index - 16)] == 0)
				moves.push_back(index - 16);
		}
		if (m_Grid.GetColFromIdx(index - 7) > m_Grid.GetColFromIdx(index))
			if (board[m_Grid.GetRowFromIdx(index - 7)][m_Grid.GetColFromIdx(index - 7)] > 0)
				moves.push_back(index - 7);
		if (m_Grid.GetColFromIdx(index - 9) < m_Grid.GetColFromIdx(index))
			if (board[m_Grid.GetRowFromIdx(index - 9)][m_Grid.GetColFromIdx(index - 9)] > 0)
				moves.push_back(index - 9);
	}

	//CheckCheck(index, -5, moves);

	return moves;
}

// ------------------- ROOKS -------------------------

std::vector<int> Game::GetWhiteRookMoves(int index) const
{
	std::vector<int> moves;

	// Go up
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index + i > 63) break;
		if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go down
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index - i < 0) break;
		if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	// Go right
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index + 1 > 63) break;
		if (m_Grid.GetRowFromIdx(index + i) != m_Grid.GetRowFromIdx(index)) break;
		if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go left
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index - i < 0) break;
		if (m_Grid.GetRowFromIdx(index - i) != m_Grid.GetRowFromIdx(index)) break;
		if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}

	CheckCheck(index, 5, moves);

	return moves;
}

std::vector<int> Game::GetBlackRookMoves(int index) const
{
	std::vector<int> moves;
	
	// Go up
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index + i > 63) break;
		if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go down
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index - i < 0) break;
		if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	// Go right
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index + i > 63) break;
		if (m_Grid.GetRowFromIdx(index + i) != m_Grid.GetRowFromIdx(index)) break;
		if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go left
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index - i < 0) break;
		if (m_Grid.GetRowFromIdx(index - i) != m_Grid.GetRowFromIdx(index)) break;
		if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}

	CheckCheck(index, -5, moves);

	return moves;
}

std::vector<int> Game::GetWhiteRookMoves(int index, int board[8][8]) const
{
	std::vector<int> moves;

	// Go up
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index + i > 63) break;
		if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go down
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index - i < 0) break;
		if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	// Go right
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index + 1 > 63) break;
		if (m_Grid.GetRowFromIdx(index + i) != m_Grid.GetRowFromIdx(index)) break;
		if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go left
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index - i < 0) break;
		if (m_Grid.GetRowFromIdx(index - i) != m_Grid.GetRowFromIdx(index)) break;
		if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}

	//CheckCheck(index, 5, moves);

	return moves;
}

std::vector<int> Game::GetBlackRookMoves(int index, int board[8][8]) const
{
	std::vector<int> moves;

	// Go up
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index + i > 63) break;
		if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go down
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index - i < 0) break;
		if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	// Go right
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index + i > 63) break;
		if (m_Grid.GetRowFromIdx(index + i) != m_Grid.GetRowFromIdx(index)) break;
		if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go left
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index - i < 0) break;
		if (m_Grid.GetRowFromIdx(index - i) != m_Grid.GetRowFromIdx(index)) break;
		if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}

	//CheckCheck(index, -5, moves);

	return moves;
}

// ------------------ KNIGHTS ------------------------

std::vector<int> Game::GetWhiteKnightMoves(int index) const
{
	std::vector<int> moves;

	std::vector<int> offsets{ 6, 10, 15, 17 };
	for (int offset : offsets)
	{
		if (index + offset <= 63 && abs(m_Grid.GetColFromIdx(index + offset) - m_Grid.GetColFromIdx(index)) <= 2)
		{
			if (m_Board[m_Grid.GetRowFromIdx(index + offset)][m_Grid.GetColFromIdx(index + offset)] <= 0)
				moves.push_back(index + offset);
		}
		if (index - offset >= 0 && abs(m_Grid.GetColFromIdx(index - offset) - m_Grid.GetColFromIdx(index)) <= 2)
		{
			if (m_Board[m_Grid.GetRowFromIdx(index - offset)][m_Grid.GetColFromIdx(index - offset)] <= 0)
				moves.push_back(index - offset);
		}
	}

	CheckCheck(index, 5, moves);

	return moves;
}

std::vector<int> Game::GetBlackKnightMoves(int index) const
{
	std::vector<int> moves;

	std::vector<int> offsets{ 6, 10, 15, 17 };
	for (int offset : offsets)
	{
		if (index + offset <= 63 && abs(m_Grid.GetColFromIdx(index + offset) - m_Grid.GetColFromIdx(index)) <= 2)
		{
			if (m_Board[m_Grid.GetRowFromIdx(index + offset)][m_Grid.GetColFromIdx(index + offset)] >= 0)
				moves.push_back(index + offset);
		}
		if (index - offset >= 0 && abs(m_Grid.GetColFromIdx(index - offset) - m_Grid.GetColFromIdx(index)) <= 2)
		{
			if (m_Board[m_Grid.GetRowFromIdx(index - offset)][m_Grid.GetColFromIdx(index - offset)] >= 0)
				moves.push_back(index - offset);
		}
	}

	CheckCheck(index, -5, moves);

	return moves;
}

std::vector<int> Game::GetWhiteKnightMoves(int index, int board[8][8]) const
{
	std::vector<int> moves;

	std::vector<int> offsets{ 6, 10, 15, 17 };
	for (int offset : offsets)
	{
		if (index + offset <= 63 && abs(m_Grid.GetColFromIdx(index + offset) - m_Grid.GetColFromIdx(index)) <= 2)
		{
			if (board[m_Grid.GetRowFromIdx(index + offset)][m_Grid.GetColFromIdx(index + offset)] <= 0)
				moves.push_back(index + offset);
		}
		if (index - offset >= 0 && abs(m_Grid.GetColFromIdx(index - offset) - m_Grid.GetColFromIdx(index)) <= 2)
		{
			if (board[m_Grid.GetRowFromIdx(index - offset)][m_Grid.GetColFromIdx(index - offset)] <= 0)
				moves.push_back(index - offset);
		}
	}

	//CheckCheck(index, 5, moves);

	return moves;
}

std::vector<int> Game::GetBlackKnightMoves(int index, int board[8][8]) const
{
	std::vector<int> moves;

	std::vector<int> offsets{ 6, 10, 15, 17 };
	for (int offset : offsets)
	{
		if (index + offset <= 63 && abs(m_Grid.GetColFromIdx(index + offset) - m_Grid.GetColFromIdx(index)) <= 2)
		{
			if (board[m_Grid.GetRowFromIdx(index + offset)][m_Grid.GetColFromIdx(index + offset)] >= 0)
				moves.push_back(index + offset);
		}
		if (index - offset >= 0 && abs(m_Grid.GetColFromIdx(index - offset) - m_Grid.GetColFromIdx(index)) <= 2)
		{
			if (board[m_Grid.GetRowFromIdx(index - offset)][m_Grid.GetColFromIdx(index - offset)] >= 0)
				moves.push_back(index - offset);
		}
	}

	//CheckCheck(index, -5, moves);

	return moves;
}

// ------------------ BISHOPS ------------------------

std::vector<int> Game::GetWhiteBishopMoves(int index) const
{
	std::vector<int> moves;

	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) &&
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) &&
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) &&
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) &&
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}

	CheckCheck(index, 5, moves);

	return moves;
}

std::vector<int> Game::GetBlackBishopMoves(int index) const
{
	std::vector<int> moves;

	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}

	CheckCheck(index, -5, moves);

	return moves;
}

std::vector<int> Game::GetWhiteBishopMoves(int index, int board[8][8]) const
{
	std::vector<int> moves;

	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}

	//CheckCheck(index, 5, moves);

	return moves;
}

std::vector<int> Game::GetBlackBishopMoves(int index, int board[8][8]) const
{
	std::vector<int> moves;

	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}

	//CheckCheck(index, -5, moves);

	return moves;
}

// ------------------ QUEENS -------------------------

std::vector<int> Game::GetWhiteQueenMoves(int index) const
{
	std::vector<int> moves;

	// Go up
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index + i > 63) break;
		if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go down
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index - i < 0) break;
		if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	// Go right
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index + i > 63) break;
		if (m_Grid.GetRowFromIdx(index + i) != m_Grid.GetRowFromIdx(index)) break;
		if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go left
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index - i < 0) break;
		if (m_Grid.GetRowFromIdx(index - i) != m_Grid.GetRowFromIdx(index)) break;
		if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) &&
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) &&
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) &&
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) &&
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}

	CheckCheck(index, 5, moves);

	return moves;
}

std::vector<int> Game::GetBlackQueenMoves(int index) const
{
	std::vector<int> moves;

	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index + i > 63) break;
		if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go down
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index - i < 0) break;
		if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	// Go right
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index + i > 63) break;
		if (m_Grid.GetRowFromIdx(index + i) != m_Grid.GetRowFromIdx(index)) break;
		if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go left
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index - i < 0) break;
		if (m_Grid.GetRowFromIdx(index - i) != m_Grid.GetRowFromIdx(index)) break;
		if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) &&
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) &&
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) &&
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) &&
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}

	CheckCheck(index, -5, moves);

	return moves;
}

std::vector<int> Game::GetWhiteQueenMoves(int index, int board[8][8]) const
{
	std::vector<int> moves;

	// Go up
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index + i > 63) break;
		if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go down
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index - i < 0) break;
		if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	// Go right
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index + i > 63) break;
		if (m_Grid.GetRowFromIdx(index + i) != m_Grid.GetRowFromIdx(index)) break;
		if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go left
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index - i < 0) break;
		if (m_Grid.GetRowFromIdx(index - i) != m_Grid.GetRowFromIdx(index)) break;
		if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}

	//CheckCheck(index, 5, moves);

	return moves;
}

std::vector<int> Game::GetBlackQueenMoves(int index, int board[8][8]) const
{
	std::vector<int> moves;

	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index + i > 63) break;
		if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go down
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index - i < 0) break;
		if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	// Go right
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index + i > 63) break;
		if (m_Grid.GetRowFromIdx(index + i) != m_Grid.GetRowFromIdx(index)) break;
		if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go left
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index - i < 0) break;
		if (m_Grid.GetRowFromIdx(index - i) != m_Grid.GetRowFromIdx(index)) break;
		if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}

	//CheckCheck(index, -5, moves);

	return moves;
}

// ------------------ KINGS --------------------------

std::vector<int> Game::GetWhiteKingMoves(int index) const
{
	std::vector<int> moves;

	std::vector<int> offsets{ 1, 7, 8, 9 };
	for (int offset : offsets)
	{
		if (index + offset <= 63 && abs(m_Grid.GetColFromIdx(index + offset) - m_Grid.GetColFromIdx(index)) <= 1)
		{
			if (m_Board[m_Grid.GetRowFromIdx(index + offset)][m_Grid.GetColFromIdx(index + offset)] <= 0)
				moves.push_back(index + offset);
		}
		if (index - offset >= 0 && abs(m_Grid.GetColFromIdx(index - offset) - m_Grid.GetColFromIdx(index)) <= 1)
		{
			if (m_Board[m_Grid.GetRowFromIdx(index - offset)][m_Grid.GetColFromIdx(index - offset)] <= 0)
				moves.push_back(index - offset);
		}
	}

	// Checken of je kan castlen als je niet in check staat
	// Dit nog in een if steken die checked of een hasmoved bool false is
	if (!m_HasWhiteKingMoved)
	{
		if (m_BlackThreats[m_Grid.GetRowFromIdx(index)][m_Grid.GetColFromIdx(index)] == 0)
		{
			if (!m_HasWhiteRightRookMoved)
			{
				if (m_Board[m_Grid.GetRowFromIdx(index + 1)][m_Grid.GetColFromIdx(index + 1)] == 0 &&
					m_Board[m_Grid.GetRowFromIdx(index + 2)][m_Grid.GetColFromIdx(index + 2)] == 0 &&
					m_BlackThreats[m_Grid.GetRowFromIdx(index + 1)][m_Grid.GetColFromIdx(index + 1)] == 0 &&
					m_BlackThreats[m_Grid.GetRowFromIdx(index + 2)][m_Grid.GetColFromIdx(index + 2)] == 0)
				{
					moves.push_back(index + 2);
				}
			}
			if (!m_HasWhiteLeftRookMoved)
			{
				if (m_Board[m_Grid.GetRowFromIdx(index - 1)][m_Grid.GetColFromIdx(index - 1)] == 0 &&
					m_Board[m_Grid.GetRowFromIdx(index - 2)][m_Grid.GetColFromIdx(index - 2)] == 0 &&
					m_BlackThreats[m_Grid.GetRowFromIdx(index - 1)][m_Grid.GetColFromIdx(index - 1)] == 0 &&
					m_BlackThreats[m_Grid.GetRowFromIdx(index - 2)][m_Grid.GetColFromIdx(index - 2)] == 0)
				{
					moves.push_back(index - 2);
				}
			}
		}
	}

	CheckCheck(index, 5, moves);

	return moves;
}

std::vector<int> Game::GetBlackKingMoves(int index) const
{
	std::vector<int> moves;

	std::vector<int> offsets{ 1, 7, 8, 9 };
	for (int offset : offsets)
	{
		if (index + offset <= 63 && abs(m_Grid.GetColFromIdx(index + offset) - m_Grid.GetColFromIdx(index)) <= 1)
		{
			if (m_Board[m_Grid.GetRowFromIdx(index + offset)][m_Grid.GetColFromIdx(index + offset)] >= 0)
				moves.push_back(index + offset);
		}
		if (index - offset >= 0 && abs(m_Grid.GetColFromIdx(index - offset) - m_Grid.GetColFromIdx(index)) <= 1)
		{
			if (m_Board[m_Grid.GetRowFromIdx(index - offset)][m_Grid.GetColFromIdx(index - offset)] >= 0)
				moves.push_back(index - offset);
		}
	}

	if (!m_HasBlackKingMoved)
	{
		if (m_WhiteThreats[m_Grid.GetRowFromIdx(index)][m_Grid.GetColFromIdx(index)] == 0)
		{
			if (!m_HasBlackRightRookMoved)
			{
				if (m_Board[m_Grid.GetRowFromIdx(index + 1)][m_Grid.GetColFromIdx(index + 1)] == 0 &&
					m_Board[m_Grid.GetRowFromIdx(index + 2)][m_Grid.GetColFromIdx(index + 2)] == 0 &&
					m_WhiteThreats[m_Grid.GetRowFromIdx(index + 1)][m_Grid.GetColFromIdx(index + 1)] == 0 &&
					m_WhiteThreats[m_Grid.GetRowFromIdx(index + 2)][m_Grid.GetColFromIdx(index + 2)] == 0)
				{
					moves.push_back(index + 2);
				}
			}
			if (!m_HasBlackLeftRookMoved)
			{
				if (m_Board[m_Grid.GetRowFromIdx(index - 1)][m_Grid.GetColFromIdx(index - 1)] == 0 &&
					m_Board[m_Grid.GetRowFromIdx(index - 2)][m_Grid.GetColFromIdx(index - 2)] == 0 &&
					m_WhiteThreats[m_Grid.GetRowFromIdx(index - 1)][m_Grid.GetColFromIdx(index - 1)] == 0 &&
					m_WhiteThreats[m_Grid.GetRowFromIdx(index - 2)][m_Grid.GetColFromIdx(index - 2)] == 0)
				{
					moves.push_back(index - 2);
				}
			}
		}
	}

	CheckCheck(index, -5, moves);

	return moves;
}

std::vector<int> Game::GetWhiteKingMoves(int index, int board[8][8]) const
{
	std::vector<int> moves;

	std::vector<int> offsets{ 1, 7, 8, 9 };
	for (int offset : offsets)
	{
		if (index + offset <= 63 && abs(m_Grid.GetColFromIdx(index + offset) - m_Grid.GetColFromIdx(index)) <= 1)
		{
			if (board[m_Grid.GetRowFromIdx(index + offset)][m_Grid.GetColFromIdx(index + offset)] <= 0)
				moves.push_back(index + offset);
		}
		if (index - offset >= 0 && abs(m_Grid.GetColFromIdx(index - offset) - m_Grid.GetColFromIdx(index)) <= 1)
		{
			if (board[m_Grid.GetRowFromIdx(index - offset)][m_Grid.GetColFromIdx(index - offset)] <= 0)
				moves.push_back(index - offset);
		}
	}

	//CheckCheck(index, 5, moves);

	return moves;
}

std::vector<int> Game::GetBlackKingMoves(int index, int board[8][8]) const
{
	std::vector<int> moves;

	std::vector<int> offsets{ 1, 7, 8, 9 };
	for (int offset : offsets)
	{
		if (index + offset <= 63 && abs(m_Grid.GetColFromIdx(index + offset) - m_Grid.GetColFromIdx(index)) <= 1)
		{
			if (board[m_Grid.GetRowFromIdx(index + offset)][m_Grid.GetColFromIdx(index + offset)] >= 0)
				moves.push_back(index + offset);
		}
		if (index - offset >= 0 && abs(m_Grid.GetColFromIdx(index - offset) - m_Grid.GetColFromIdx(index)) <= 1)
		{
			if (board[m_Grid.GetRowFromIdx(index - offset)][m_Grid.GetColFromIdx(index - offset)] >= 0)
				moves.push_back(index - offset);
		}
	}

	//CheckCheck(index, -5, moves);

	return moves;
}