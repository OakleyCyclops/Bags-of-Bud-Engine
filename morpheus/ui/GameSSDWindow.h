/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.

This file is part of the Doom 3 BFG Edition GPL Source Code ("Doom 3 BFG Edition Source Code").

Doom 3 BFG Edition Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 BFG Edition Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 BFG Edition Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 BFG Edition Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 BFG Edition Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/
#ifndef __GAME_SSD_WINDOW_H__
#define __GAME_SSD_WINDOW_H__

class budGameSSDWindow;

class SSDCrossHair
{

public:
	enum
	{
		CROSSHAIR_STANDARD = 0,
		CROSSHAIR_SUPER,
		CROSSHAIR_COUNT
	};
	const budMaterial*	crosshairMaterial[CROSSHAIR_COUNT];
	int					currentCrosshair;
	float				crosshairWidth, crosshairHeight;
	
public:
	SSDCrossHair();
	~SSDCrossHair();
	
	virtual void	WriteToSaveGame( budFile* savefile );
	virtual void	ReadFromSaveGame( budFile* savefile );
	
	void		InitCrosshairs();
	void		Draw( const Vector2& cursor );
};

enum
{
	SSD_ENTITY_BASE = 0,
	SSD_ENTITY_ASTEROID,
	SSD_ENTITY_ASTRONAUT,
	SSD_ENTITY_EXPLOSION,
	SSD_ENTITY_POINTS,
	SSD_ENTITY_PROJECTILE,
	SSD_ENTITY_POWERUP
};

class SSDEntity
{

public:
	//SSDEntity Information
	int					type;
	int					id;
	String				materialName;
	const budMaterial*	material;
	Vector3				position;
	Vector2				size;
	float				radius;
	float				hitRadius;
	float				rotation;
	
	Vector4				matColor;
	
	String				text;
	float				textScale;
	Vector4				foreColor;
	
	budGameSSDWindow*	game;
	int					currentTime;
	int					lastUpdate;
	int					elapsed;
	
	bool				destroyed;
	bool				noHit;
	bool				noPlayerDamage;
	
	bool				inUse;
	
	
public:
	SSDEntity();
	virtual				~SSDEntity();
	
	virtual void		WriteToSaveGame( budFile* savefile );
	virtual void		ReadFromSaveGame( budFile* savefile,  budGameSSDWindow* _game );
	
	void				EntityInit();
	
	void				SetGame( budGameSSDWindow* _game );
	void				SetMaterial( const char* _name );
	void				SetPosition( const Vector3& _position );
	void				SetSize( const Vector2& _size );
	void				SetRadius( float _radius, float _hitFactor = 1.0f );
	void				SetRotation( float _rotation );
	
	void				Update();
	bool				HitTest( const Vector2& pt );
	
	
	virtual void		EntityUpdate() {};
	virtual void		Draw();
	virtual void		DestroyEntity();
	
	virtual void		OnHit( int key ) {};
	virtual void		OnStrikePlayer() {};
	
	budBounds			WorldToScreen( const budBounds worldBounds );
	Vector3				WorldToScreen( const Vector3& worldPos );
	
	Vector3				ScreenToWorld( const Vector3& screenPos );
	
};

/*
*****************************************************************************
* SSDMover
****************************************************************************
*/

class SSDMover : public SSDEntity
{

public:
	Vector3				speed;
	float				rotationSpeed;
	
public:
	SSDMover();
	virtual				~SSDMover();
	
	virtual void	WriteToSaveGame( budFile* savefile );
	virtual void	ReadFromSaveGame( budFile* savefile,  budGameSSDWindow* _game );
	
	void				MoverInit( const Vector3& _speed, float _rotationSpeed );
	
	virtual void		EntityUpdate();
	
	
};

/*
*****************************************************************************
* SSDAsteroid
****************************************************************************
*/

#define MAX_ASTEROIDS 64

class SSDAsteroid : public SSDMover
{

public:

	int					health;
	
public:
	SSDAsteroid();
	~SSDAsteroid();
	
	virtual void	WriteToSaveGame( budFile* savefile );
	virtual void	ReadFromSaveGame( budFile* savefile,  budGameSSDWindow* _game );
	
	void				Init( budGameSSDWindow* _game, const Vector3& startPosition, const Vector2& _size, float _speed, float rotate, int _health );
	
	virtual void		EntityUpdate();
	static SSDAsteroid*	GetNewAsteroid( budGameSSDWindow* _game, const Vector3& startPosition, const Vector2& _size, float _speed, float rotate, int _health );
	static SSDAsteroid*	GetSpecificAsteroid( int id );
	static void			WriteAsteroids( budFile* savefile );
	static void			ReadAsteroids( budFile* savefile, budGameSSDWindow* _game );
	
	
	
protected:
	static SSDAsteroid	asteroidPool[MAX_ASTEROIDS];
	
};

/*
*****************************************************************************
* SSDAstronaut
****************************************************************************
*/
#define MAX_ASTRONAUT 8

class SSDAstronaut : public SSDMover
{

public:

	int					health;
	
public:
	SSDAstronaut();
	~SSDAstronaut();
	
	virtual void	WriteToSaveGame( budFile* savefile );
	virtual void	ReadFromSaveGame( budFile* savefile,  budGameSSDWindow* _game );
	
	void					Init( budGameSSDWindow* _game, const Vector3& startPosition, float _speed, float rotate, int _health );
	
	static SSDAstronaut*	GetNewAstronaut( budGameSSDWindow* _game, const Vector3& startPosition, float _speed, float rotate, int _health );
	static SSDAstronaut*	GetSpecificAstronaut( int id );
	static void				WriteAstronauts( budFile* savefile );
	static void				ReadAstronauts( budFile* savefile, budGameSSDWindow* _game );
	
protected:
	static SSDAstronaut	astronautPool[MAX_ASTRONAUT];
	
};

/*
*****************************************************************************
* SSDExplosion
****************************************************************************
*/
#define MAX_EXPLOSIONS 64

class SSDExplosion : public SSDEntity
{

public:
	Vector2	finalSize;
	int		length;
	int		beginTime;
	int		endTime;
	int		explosionType;
	
	//The entity that is exploding
	SSDEntity*			buddy;
	bool				killBuddy;
	bool				followBuddy;
	
	enum
	{
		EXPLOSION_NORMAL = 0,
		EXPLOSION_TELEPORT = 1
	};
	
public:
	SSDExplosion();
	~SSDExplosion();
	
	virtual void	WriteToSaveGame( budFile* savefile );
	virtual void	ReadFromSaveGame( budFile* savefile,  budGameSSDWindow* _game );
	
	void				Init( budGameSSDWindow* _game, const Vector3& _position, const Vector2& _size, int _length, int _type, SSDEntity* _buddy, bool _killBuddy = true, bool _followBuddy = true );
	
	virtual void		EntityUpdate();
	static SSDExplosion*	GetNewExplosion( budGameSSDWindow* _game, const Vector3& _position, const Vector2& _size, int _length, int _type, SSDEntity* _buddy, bool _killBuddy = true, bool _followBuddy = true );
	static SSDExplosion*	GetSpecificExplosion( int id );
	static void				WriteExplosions( budFile* savefile );
	static void				ReadExplosions( budFile* savefile, budGameSSDWindow* _game );
	
protected:
	static SSDExplosion	explosionPool[MAX_EXPLOSIONS];
};

#define MAX_POINTS 16

class SSDPoints : public SSDEntity
{

	int		length;
	int		distance;
	int		beginTime;
	int		endTime;
	
	Vector3	beginPosition;
	Vector3	endPosition;
	
	Vector4	beginColor;
	Vector4	endColor;
	
	
public:
	SSDPoints();
	~SSDPoints();
	
	virtual void	WriteToSaveGame( budFile* savefile );
	virtual void	ReadFromSaveGame( budFile* savefile,  budGameSSDWindow* _game );
	
	void				Init( budGameSSDWindow* _game, SSDEntity* _ent, int _points, int _length, int _distance, const Vector4& color );
	virtual void		EntityUpdate();
	
	static SSDPoints*	GetNewPoints( budGameSSDWindow* _game, SSDEntity* _ent, int _points, int _length, int _distance, const Vector4& color );
	static SSDPoints*	GetSpecificPoints( int id );
	static void			WritePoints( budFile* savefile );
	static void			ReadPoints( budFile* savefile, budGameSSDWindow* _game );
	
protected:
	static SSDPoints	pointsPool[MAX_POINTS];
};

#define MAX_PROJECTILES 64

class SSDProjectile : public SSDEntity
{

	Vector3	dir;
	Vector3	speed;
	int		beginTime;
	int		endTime;
	
	Vector3	endPosition;
	
public:
	SSDProjectile();
	~SSDProjectile();
	
	virtual void	WriteToSaveGame( budFile* savefile );
	virtual void	ReadFromSaveGame( budFile* savefile,  budGameSSDWindow* _game );
	
	void				Init( budGameSSDWindow* _game, const Vector3& _beginPosition, const Vector3& _endPosition, float _speed, float _size );
	virtual void		EntityUpdate();
	
	static SSDProjectile* GetNewProjectile( budGameSSDWindow* _game, const Vector3& _beginPosition, const Vector3& _endPosition, float _speed, float _size );
	static SSDProjectile* GetSpecificProjectile( int id );
	static void				WriteProjectiles( budFile* savefile );
	static void				ReadProjectiles( budFile* savefile, budGameSSDWindow* _game );
	
protected:
	static SSDProjectile	projectilePool[MAX_PROJECTILES];
};


#define MAX_POWERUPS 64

/**
* Powerups work in two phases:
*	1.) Closed container hurls at you
*		If you shoot the container it open
*	3.) If an opened powerup hits the player he aquires the powerup
* Powerup Types:
*	Health - Give a specific amount of health
*	Super Blaster - Increases the power of the blaster (lasts a specific amount of time)
*	Asteroid Nuke - Destroys all asteroids on screen as soon as it is aquired
*	Rescue Powerup - Rescues all astronauts as soon as it is acquited
*	Bonus Points - Gives some bonus points when acquired
*/
class SSDPowerup : public SSDMover
{

	enum
	{
		POWERUP_STATE_CLOSED = 0,
		POWERUP_STATE_OPEN
	};
	
	enum
	{
		POWERUP_TYPE_HEALTH = 0,
		POWERUP_TYPE_SUPER_BLASTER,
		POWERUP_TYPE_ASTEROID_NUKE,
		POWERUP_TYPE_RESCUE_ALL,
		POWERUP_TYPE_BONUS_POINTS,
		POWERUP_TYPE_DAMAGE,
		POWERUP_TYPE_MAX
	};
	
	int powerupState;
	int powerupType;
	
	
public:


public:
	SSDPowerup();
	virtual ~SSDPowerup();
	
	virtual void	WriteToSaveGame( budFile* savefile );
	virtual void	ReadFromSaveGame( budFile* savefile,  budGameSSDWindow* _game );
	
	virtual void		OnHit( int key );
	virtual void		OnStrikePlayer();
	
	void	OnOpenPowerup();
	void	OnActivatePowerup();
	
	
	
	void	Init( budGameSSDWindow* _game, float _speed, float _rotation );
	
	static SSDPowerup* GetNewPowerup( budGameSSDWindow* _game, float _speed, float _rotation );
	static SSDPowerup* GetSpecificPowerup( int id );
	static void			WritePowerups( budFile* savefile );
	static void			ReadPowerups( budFile* savefile, budGameSSDWindow* _game );
	
protected:
	static SSDPowerup	powerupPool[MAX_POWERUPS];
	
};


typedef struct
{
	float	spawnBuffer;
	int		needToWin;
} SSDLevelData_t;

typedef struct
{
	float	speedMin, speedMax;
	float	sizeMin, sizeMax;
	float	rotateMin, rotateMax;
	int		spawnMin, spawnMax;
	int		asteroidHealth;
	int		asteroidPoints;
	int		asteroidDamage;
} SSDAsteroidData_t;

typedef struct
{
	float	speedMin, speedMax;
	float	rotateMin, rotateMax;
	int		spawnMin, spawnMax;
	int		health;
	int		points;
	int		penalty;
} SSDAstronautData_t;

typedef struct
{
	float	speedMin, speedMax;
	float	rotateMin, rotateMax;
	int		spawnMin, spawnMax;
} SSDPowerupData_t;

typedef struct
{
	float	speed;
	int		damage;
	int		size;
} SSDWeaponData_t;

/**
* SSDLevelStats_t
*	Data that is used for each level. This data is reset
*	each new level.
*/
typedef struct
{
	int					shotCount;
	int					hitCount;
	int					destroyedAsteroids;
	int					nextAsteroidSpawnTime;
	
	int					killedAstronauts;
	int					savedAstronauts;
	
	//Astronaut Level Data
	int					nextAstronautSpawnTime;
	
	//Powerup Level Data
	int					nextPowerupSpawnTime;
	
	SSDEntity*			targetEnt;
} SSDLevelStats_t;

/**
* SSDGameStats_t
*	Data that is used for the game that is currently running. Memset this
*	to completely reset the game
*/
typedef struct
{
	bool				gameRunning;
	
	int					score;
	int					prebonusscore;
	
	int					health;
	
	int					currentWeapon;
	int					currentLevel;
	int					nextLevel;
	
	SSDLevelStats_t		levelStats;
} SSDGameStats_t;


class budGameSSDWindow : public idWindow
{
public:
	budGameSSDWindow( budUserInterfaceLocal* gui );
	~budGameSSDWindow();
	
	virtual void	WriteToSaveGame( budFile* savefile );
	virtual void	ReadFromSaveGame( budFile* savefile );
	
	virtual const char*	HandleEvent( const sysEvent_t* event, bool* updateVisuals );
	virtual idWinVar*	GetWinVarByName( const char* _name, bool winLookup = false, drawWin_t** owner = NULL );
	
	
	virtual void		Draw( int time, float x, float y );
	
	void				AddHealth( int health );
	void				AddScore( SSDEntity* ent, int points );
	void				AddDamage( int damage );
	
	void				OnNuke();
	void				OnRescueAll();
	void				OnSuperBlaster();
	
	SSDEntity*			GetSpecificEntity( int type, int id );
	
	void				PlaySound( const char* sound );
	
	
	
	
	static idRandom		random;
	int					ssdTime;
	
private:

	//Initialization
	virtual bool		ParseInternalVar( const char* name, budTokenParser* src );
	void				ParseLevelData( int level, const String& levelDataString );
	void				ParseAsteroidData( int level, const String& asteroidDataString );
	void				ParseWeaponData( int weapon, const String& weaponDataString );
	void				ParseAstronautData( int level, const String& astronautDataString );
	void				ParsePowerupData( int level, const String& powerupDataString );
	
	void				CommonInit();
	void				ResetGameStats();
	void				ResetLevelStats();
	void				ResetEntities();
	
	//Game Running Methods
	void				StartGame();
	void				StopGame();
	void				GameOver();
	
	//Starting the Game
	void				BeginLevel( int level );
	void				ContinueGame();
	
	//Stopping the Game
	void				LevelComplete();
	void				GameComplete();
	
	
	
	void				UpdateGame();
	void				CheckForHits();
	void				ZOrderEntities();
	
	void				SpawnAsteroid();
	
	void				FireWeapon( int key );
	SSDEntity*			EntityHitTest( const Vector2& pt );
	
	void				HitAsteroid( SSDAsteroid* asteroid, int key );
	void				AsteroStringuckPlayer( SSDAsteroid* asteroid );
	
	
	
	
	
	void				RefreshGuiData();
	
	Vector2				GetCursorWorld();
	
	//Astronaut Methods
	void				SpawnAstronaut();
	void				HitAstronaut( SSDAstronaut* astronaut, int key );
	void				AstronautStruckPlayer( SSDAstronaut* astronaut );
	
	//Powerup Methods
	void				SpawnPowerup();
	
	
	void				StartSuperBlaster();
	void				StopSuperBlaster();
	
	//void				FreeSoundEmitter( bool immediate );
	
	
	
	
public:

	//WinVars used to call functions from the guis
	idWinBool					beginLevel;
	idWinBool					resetGame;
	idWinBool					continueGame;
	idWinBool					refreshGuiData;
	
	SSDCrossHair				crosshair;
	budBounds					screenBounds;
	
	//Level Data
	int							levelCount;
	List<SSDLevelData_t>		levelData;
	List<SSDAsteroidData_t>	asteroidData;
	List<SSDAstronautData_t>	astronautData;
	List<SSDPowerupData_t>	powerupData;
	
	
	//Weapon Data
	int							weaponCount;
	List<SSDWeaponData_t>		weaponData;
	
	int							superBlasterTimeout;
	
	//All current game data is stored in this structure (except the entity list)
	SSDGameStats_t				gameStats;
	List<SSDEntity*>			entities;
	
	int							currentSound;
	
};

#endif //__GAME_SSD_WINDOW_H__
