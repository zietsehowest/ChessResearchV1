#pragma once
#include "Room.h"
#include "Entity.h"
#include "Camera.h"
#include "BulletManager.h"
#include "EntityManager.h"
#include "LevelManager.h"
#include "ItemManager.h"
#include "Camera.h"
#include "Player.h"
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
	void PrintInstructions() const;
	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e );
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e );
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e );
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e );
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e );

private:
	// DATA MEMBERS
	bool m_MapToggle;
	const Window m_Window;
	const Texture* m_BodyTexture;
	const Texture* m_HeadTexture;
	Camera m_Camera;
	BulletManager* m_BulletManager;
	EntityManager* m_EntityManager;
	LevelManager* m_LevelManager;
	ItemManager* m_pItemManager;
	// FUNCTIONS
	void Initialize( );
	void Cleanup( );
	void ClearBackground( ) const;
	void GenerateMap() const;
};