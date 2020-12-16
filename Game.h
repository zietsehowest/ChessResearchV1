#pragma once
#include <vector>
#include <map>
#include "Texture.h"
#include "Grid.h"
class Game
{
public:
	explicit Game( const Window& window );
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game( Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	~Game();

	void Update( float elapsedSec );
	void Draw( ) const;
	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e );
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e );
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e );
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e );
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e );

private:
	// DATA MEMBERS
	const Window m_Window;
	// FUNCTIONS
	void Initialize( );
	void Cleanup( );
	void ClearBackground( ) const;
	void DrawPieces() const;
	void InitializeTextures();
	int convert2DTo1D(int row, int column) const {return  (row * 8) + column; };

	Grid m_Grid;
	int m_Board[8][8] = {
		1,2,3,4,5,3,2,1,
		6,6,6,6,6,6,6,6,
		 6, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 -6, -6, -6, -6, -6, -6, -6, -6,
		 -1, -2, -3, -4, -5, -3, -2, -1};
	float size = m_Window.width/8;
	Vector2f offset{ 0.f,0.f };
	std::map<int, Texture> m_Textures;
	void PrintBoard();
};