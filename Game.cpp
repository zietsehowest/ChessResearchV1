#include "pch.h"
#include "Game.h"
Game::Game(const Window& window)
	: m_Grid{ 8,8, window.width, window.height }
{
	Initialize( );
}
Game::~Game( )
{
	Cleanup( );
}
void Game::InitializeTextures()
{
	for (int i = -6; i < 7; i++)
	{
		if (i != 0)
		{
			m_Textures.emplace(i,Texture("Resources/" + std::to_string(i) + ".png"));
		}
	}
}

void Game::Initialize( )
{
	InitializeTextures();
	PrintBoard();
}
void Game::Cleanup( )
{

}
void Game::Update( float elapsedSec )
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
void Game::Draw( ) const
{
	ClearBackground( );
	m_Grid.DrawGrid(true);
	DrawPieces();
}
void Game::DrawPieces() const
{
	for (int i{ 0 }; i < 8; i++)
	{
		for (int j{ 0 }; j < 8; j++)
		{
			if (m_Board[i][j] != 0)
				m_Textures.at(m_Board[i][j]).Draw(Rectf{m_Grid.GetPosFromIdx(convert2DTo1D(i,j)).x,m_Grid.GetPosFromIdx(convert2DTo1D(i,j)).y,size* 2.7f,size* 2.7f }, Rectf{ 0,0,68,68 });
		}
	}
	utils::SetColor(Color4f{ 1.f,0.f,0.f,0.3f });
	utils::FillRect(Rectf{ m_Grid.GetPosFromIdx(convert2DTo1D(0,0)).x, m_Grid.GetPosFromIdx(convert2DTo1D(0,0)).y,size * 2.8f,size * 2.8f });
}
void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{
	//std::cout << "KEYDOWN event: " << e.keysym.sym << std::endl;
}

void Game::ProcessKeyUpEvent( const SDL_KeyboardEvent& e )
{
	
}

void Game::ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e )
{
	//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;
}

void Game::ProcessMouseDownEvent( const SDL_MouseButtonEvent& e )
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

void Game::ProcessMouseUpEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONUP event: ";
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

void Game::ClearBackground( ) const
{
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}
