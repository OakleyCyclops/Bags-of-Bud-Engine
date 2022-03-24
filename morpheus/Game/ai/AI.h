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

#ifndef __AI_H__
#define __AI_H__

/*
===============================================================================

	budAI

===============================================================================
*/

const float	SQUARE_ROOT_OF_2			= 1.414213562f;
const float	AI_TURN_PREDICTION			= 0.2f;
const float	AI_TURN_SCALE				= 60.0f;
const float	AI_SEEK_PREDICTION			= 0.3f;
const float	AI_FLY_DAMPENING			= 0.15f;
const float	AI_HEARING_RANGE			= 2048.0f;
const int	DEFAULT_FLY_OFFSET			= 68;

#define ATTACK_IGNORE			0
#define ATTACK_ON_DAMAGE		1
#define ATTACK_ON_ACTIVATE		2
#define ATTACK_ON_SIGHT			4

typedef struct ballistics_s
{
	float				angle;		// angle in degrees in the range [-180, 180]
	float				time;		// time it takes before the projectile arrives
} ballistics_t;

extern int Ballistics( const budVec3& start, const budVec3& end, float speed, float gravity, ballistics_t bal[2] );

// defined in script/ai_base.script.  please keep them up to date.
typedef enum
{
	MOVETYPE_DEAD,
	MOVETYPE_ANIM,
	MOVETYPE_SLIDE,
	MOVETYPE_FLY,
	MOVETYPE_STATIC,
	NUM_MOVETYPES
} moveType_t;

typedef enum
{
	MOVE_NONE,
	MOVE_FACE_ENEMY,
	MOVE_FACE_ENTITY,
	
	// commands < NUM_NONMOVING_COMMANDS don't cause a change in position
	NUM_NONMOVING_COMMANDS,
	
	MOVE_TO_ENEMY = NUM_NONMOVING_COMMANDS,
	MOVE_TO_ENEMYHEIGHT,
	MOVE_TO_ENTITY,
	MOVE_OUT_OF_RANGE,
	MOVE_TO_ATTACK_POSITION,
	MOVE_TO_COVER,
	MOVE_TO_POSITION,
	MOVE_TO_POSITION_DIRECT,
	MOVE_SLIDE_TO_POSITION,
	MOVE_WANDER,
	NUM_MOVE_COMMANDS
} moveCommand_t;

typedef enum
{
	TALK_NEVER,
	TALK_DEAD,
	TALK_OK,
	TALK_BUSY,
	NUM_TALK_STATES
} talkState_t;

//
// status results from move commands
// make sure to change script/doom_defs.script if you add any, or change their order
//
typedef enum
{
	MOVE_STATUS_DONE,
	MOVE_STATUS_MOVING,
	MOVE_STATUS_WAITING,
	MOVE_STATUS_DEST_NOT_FOUND,
	MOVE_STATUS_DEST_UNREACHABLE,
	MOVE_STATUS_BLOCKED_BY_WALL,
	MOVE_STATUS_BLOCKED_BY_OBJECT,
	MOVE_STATUS_BLOCKED_BY_ENEMY,
	MOVE_STATUS_BLOCKED_BY_MONSTER
} moveStatus_t;

#define	DI_NODIR	-1

// obstacle avoidance
typedef struct obstaclePath_s
{
	budVec3				seekPos;					// seek position avoiding obstacles
	idEntity* 			firstObstacle;				// if != NULL the first obstacle along the path
	budVec3				startPosOutsideObstacles;	// start position outside obstacles
	idEntity* 			startPosObstacle;			// if != NULL the obstacle containing the start position
	budVec3				seekPosOutsideObstacles;	// seek position outside obstacles
	idEntity* 			seekPosObstacle;			// if != NULL the obstacle containing the seek position
} obstaclePath_t;

// path prediction
typedef enum
{
	SE_BLOCKED			= BIT( 0 ),
	SE_ENTER_LEDGE_AREA	= BIT( 1 ),
	SE_ENTER_OBSTACLE	= BIT( 2 ),
	SE_FALL				= BIT( 3 ),
	SE_LAND				= BIT( 4 )
} stopEvent_t;

typedef struct predictedPath_s
{
	budVec3				endPos;						// final position
	budVec3				endVelocity;				// velocity at end position
	budVec3				endNormal;					// normal of blocking surface
	int					endTime;					// time predicted
	int					endEvent;					// event that stopped the prediction
	const idEntity* 	blockingEntity;				// entity that blocks the movement
} predictedPath_t;

//
// events
//
extern const idEventDef AI_BeginAttack;
extern const idEventDef AI_EndAttack;
extern const idEventDef AI_MuzzleFlash;
extern const idEventDef AI_CreateMissile;
extern const idEventDef AI_AttackMissile;
extern const idEventDef AI_FireMissileAtTarget;
extern const idEventDef AI_LaunchProjectile;
extern const idEventDef AI_TriggerFX;
extern const idEventDef AI_StartEmitter;
extern const idEventDef AI_StopEmitter;
extern const idEventDef AI_AttackMelee;
extern const idEventDef AI_DirectDamage;
extern const idEventDef AI_JumpFrame;
extern const idEventDef AI_EnableClip;
extern const idEventDef AI_DisableClip;
extern const idEventDef AI_EnableGravity;
extern const idEventDef AI_DisableGravity;
extern const idEventDef AI_TriggerParticles;
extern const idEventDef AI_RandomPath;

class idPathCorner;

typedef struct particleEmitter_s
{
	particleEmitter_s()
	{
		particle = NULL;
		time = 0;
		joint = INVALID_JOINT;
	};
	const budDeclParticle* particle;
	int					time;
	jointHandle_t		joint;
} particleEmitter_t;

typedef struct funcEmitter_s
{
	char				name[64];
	idFuncEmitter*		particle;
	jointHandle_t		joint;
} funcEmitter_t;

class idMoveState
{
public:
	idMoveState();
	
	void					Save( idSaveGame* savefile ) const;
	void					Restore( idRestoreGame* savefile );
	
	moveType_t				moveType;
	moveCommand_t			moveCommand;
	moveStatus_t			moveStatus;
	budVec3					moveDest;
	budVec3					moveDir;			// used for wandering and slide moves
	idEntityPtr<idEntity>	goalEntity;
	budVec3					goalEntityOrigin;	// move to entity uses this to avoid checking the floor position every frame
	int						toAreaNum;
	int						startTime;
	int						duration;
	float					speed;				// only used by flying creatures
	float					range;
	float					wanderYaw;
	int						nextWanderTime;
	int						blockTime;
	idEntityPtr<idEntity>	obstacle;
	budVec3					lastMoveOrigin;
	int						lastMoveTime;
	int						anim;
};

class budAASFindCover : public budAASCallback
{
public:
	budAASFindCover( const budVec3& hideFromPos );
	~budAASFindCover();
	
	virtual bool		TestArea( const budAAS* aas, int areaNum );
	
private:
	pvsHandle_t			hidePVS;
	int					PVSAreas[ idEntity::MAX_PVS_AREAS ];
};

class budAASFindAreaOutOfRange : public budAASCallback
{
public:
	budAASFindAreaOutOfRange( const budVec3& targetPos, float maxDist );
	
	virtual bool		TestArea( const budAAS* aas, int areaNum );
	
private:
	budVec3				targetPos;
	float				maxDistSqr;
};

class budAASFindAttackPosition : public budAASCallback
{
public:
	budAASFindAttackPosition( const budAI* self, const budMat3& gravityAxis, idEntity* target, const budVec3& targetPos, const budVec3& fireOffset );
	~budAASFindAttackPosition();
	
	virtual bool		TestArea( const budAAS* aas, int areaNum );
	
private:
	const budAI*			self;
	idEntity*			target;
	budBounds			excludeBounds;
	budVec3				targetPos;
	budVec3				fireOffset;
	budMat3				gravityAxis;
	pvsHandle_t			targetPVS;
	int					PVSAreas[ idEntity::MAX_PVS_AREAS ];
};

class budAI : public budActor
{
public:
	CLASS_PROTOTYPE( budAI );
	
	budAI();
	~budAI();
	
	void					Save( idSaveGame* savefile ) const;
	void					Restore( idRestoreGame* savefile );
	
	void					Spawn();
	void					HeardSound( idEntity* ent, const char* action );
	budActor*					GetEnemy() const;
	void					TalkTo( budActor* actor );
	talkState_t				GetTalkState() const;
	
	bool					GetAimDir( const budVec3& firePos, idEntity* aimAtEnt, const idEntity* ignore, budVec3& aimDir ) const;
	
	void					TouchedByFlashlight( budActor* flashlight_owner );
	
	// Outputs a list of all monsters to the console.
	static void				List_f( const budCmdArgs& args );
	
	// Finds a path around dynamic obstacles.
	static bool				FindPathAroundObstacles( const idPhysics* physics, const budAAS* aas, const idEntity* ignore, const budVec3& startPos, const budVec3& seekPos, obstaclePath_t& path );
	// Frees any nodes used for the dynamic obstacle avoidance.
	static void				FreeObstacleAvoidanceNodes();
	// Predicts movement, returns true if a stop event was triggered.
	static bool				PredictPath( const idEntity* ent, const budAAS* aas, const budVec3& start, const budVec3& velocity, int totalTime, int frameTime, int stopEvent, predictedPath_t& path );
	// Return true if the trajectory of the clip model is collision free.
	static bool				TestTrajectory( const budVec3& start, const budVec3& end, float zVel, float gravity, float time, float max_height, const budClipModel* clip, int clipmask, const idEntity* ignore, const idEntity* targetEntity, int drawtime );
	// Finds the best collision free trajectory for a clip model.
	static bool				PredictTrajectory( const budVec3& firePos, const budVec3& target, float projectileSpeed, const budVec3& projGravity, const budClipModel* clip, int clipmask, float max_height, const idEntity* ignore, const idEntity* targetEntity, int drawtime, budVec3& aimDir );
	
	virtual void			Gib( const budVec3& dir, const char* damageDefName );
	
protected:
	// navigation
	budAAS* 					aas;
	int						travelFlags;
	
	idMoveState				move;
	idMoveState				savedMove;
	
	float					kickForce;
	bool					ignore_obstacles;
	float					blockedRadius;
	int						blockedMoveTime;
	int						blockedAttackTime;
	
	// turning
	float					ideal_yaw;
	float					current_yaw;
	float					turnRate;
	float					turnVel;
	float					anim_turn_yaw;
	float					anim_turn_amount;
	float					anim_turn_angles;
	
	// physics
	idPhysics_Monster		physicsObj;
	
	// flying
	jointHandle_t			flyTiltJoint;
	float					fly_speed;
	float					fly_bob_strength;
	float					fly_bob_vert;
	float					fly_bob_horz;
	int						fly_offset;					// prefered offset from player's view
	float					fly_seek_scale;
	float					fly_roll_scale;
	float					fly_roll_max;
	float					fly_roll;
	float					fly_pitch_scale;
	float					fly_pitch_max;
	float					fly_pitch;
	
	bool					allowMove;					// disables any animation movement
	bool					allowHiddenMovement;		// allows character to still move around while hidden
	bool					disableGravity;				// disables gravity and allows vertical movement by the animation
	bool					af_push_moveables;			// allow the articulated figure to push moveable objects
	
	// weapon/attack vars
	bool					lastHitCheckResult;
	int						lastHitCheckTime;
	int						lastAttackTime;
	float					melee_range;
	float					projectile_height_to_distance_ratio;	// calculates the maximum height a projectile can be thrown
	budList<budVec3, TAG_AI>			missileLaunchOffset;
	
	const idDict* 			projectileDef;
	mutable budClipModel*		projectileClipModel;
	float					projectileRadius;
	float					projectileSpeed;
	budVec3					projectileVelocity;
	budVec3					projectileGravity;
	idEntityPtr<idProjectile> projectile;
	budStr					attack;
	budVec3					homingMissileGoal;
	
	// chatter/talking
	const idSoundShader*		chat_snd;
	int						chat_min;
	int						chat_max;
	int						chat_time;
	talkState_t				talk_state;
	idEntityPtr<budActor>	talkTarget;
	
	// cinematics
	int						num_cinematics;
	int						current_cinematic;
	
	bool					allowJointMod;
	idEntityPtr<idEntity>	focusEntity;
	budVec3					currentFocusPos;
	int						focusTime;
	int						alignHeadTime;
	int						forceAlignHeadTime;
	budAngles				eyeAng;
	budAngles				lookAng;
	budAngles				destLookAng;
	budAngles				lookMin;
	budAngles				lookMax;
	budList<jointHandle_t, TAG_AI>	lookJoints;
	budList<budAngles, TAG_AI>		lookJointAngles;
	float					eyeVerticalOffset;
	float					eyeHorizontalOffset;
	float					eyeFocusRate;
	float					headFocusRate;
	int						focusAlignTime;
	
	// special fx
	bool					restartParticles;			// should smoke emissions restart
	bool					useBoneAxis;				// use the bone vs the model axis
	budList<particleEmitter_t, TAG_AI> particles;				// particle data
	
	renderLight_t			worldMuzzleFlash;			// positioned on world weapon bone
	int						worldMuzzleFlashHandle;
	jointHandle_t			flashJointWorld;
	int						muzzleFlashEnd;
	int						flashTime;
	
	// joint controllers
	budAngles				eyeMin;
	budAngles				eyeMax;
	jointHandle_t			focusJoint;
	jointHandle_t			orientationJoint;
	
	// enemy variables
	idEntityPtr<budActor>	enemy;
	budVec3					lastVisibleEnemyPos;
	budVec3					lastVisibleEnemyEyeOffset;
	budVec3					lastVisibleReachableEnemyPos;
	budVec3					lastReachableEnemyPos;
	bool					wakeOnFlashlight;
	
	bool					spawnClearMoveables;
	
	budHashTable<funcEmitter_t> funcEmitters;
	
	idEntityPtr<idHarvestable>	harvestEnt;
	
	// script variables
	idScriptBool			AI_TALK;
	idScriptBool			AI_DAMAGE;
	idScriptBool			AI_PAIN;
	idScriptFloat			AI_SPECIAL_DAMAGE;
	idScriptBool			AI_DEAD;
	idScriptBool			AI_ENEMY_VISIBLE;
	idScriptBool			AI_ENEMY_IN_FOV;
	idScriptBool			AI_ENEMY_DEAD;
	idScriptBool			AI_MOVE_DONE;
	idScriptBool			AI_ONGROUND;
	idScriptBool			AI_ACTIVATED;
	idScriptBool			AI_FORWARD;
	idScriptBool			AI_JUMP;
	idScriptBool			AI_ENEMY_REACHABLE;
	idScriptBool			AI_BLOCKED;
	idScriptBool			AI_OBSTACLE_IN_PATH;
	idScriptBool			AI_DEST_UNREACHABLE;
	idScriptBool			AI_HIT_ENEMY;
	idScriptBool			AI_PUSHED;
	
	//
	// ai/ai.cpp
	//
	void					SetAAS();
	virtual	void			DormantBegin();	// called when entity becomes dormant
	virtual	void			DormantEnd();		// called when entity wakes from being dormant
	void					Think();
	void					Activate( idEntity* activator );
public:
	int						ReactionTo( const idEntity* ent );
protected:
	bool					CheckForEnemy();
	void					EnemyDead();
	virtual bool			CanPlayChatterSounds() const;
	void					SetChatSound();
	void					PlayChatter();
	virtual void			Hide();
	virtual void			Show();
	budVec3					FirstVisiblePointOnPath( const budVec3 origin, const budVec3& target, int travelFlags ) const;
	void					CalculateAttackOffsets();
	void					PlayCinematic();
	
	// movement
	virtual void			ApplyImpulse( idEntity* ent, int id, const budVec3& point, const budVec3& impulse );
	void					GetMoveDelta( const budMat3& oldaxis, const budMat3& axis, budVec3& delta );
	void					CheckObstacleAvoidance( const budVec3& goalPos, budVec3& newPos );
	void					DeadMove();
	void					AnimMove();
	void					SlideMove();
	void					AdjustFlyingAngles();
	void					AddFlyBob( budVec3& vel );
	void					AdjustFlyHeight( budVec3& vel, const budVec3& goalPos );
	void					FlySeekGoal( budVec3& vel, budVec3& goalPos );
	void					AdjustFlySpeed( budVec3& vel );
	void					FlyTurn();
	void					FlyMove();
	void					StaticMove();
	
	// damage
	virtual bool			Pain( idEntity* inflictor, idEntity* attacker, int damage, const budVec3& dir, int location );
	virtual void			Killed( idEntity* inflictor, idEntity* attacker, int damage, const budVec3& dir, int location );
	
	// navigation
	void					KickObstacles( const budVec3& dir, float force, idEntity* alwaysKick );
	bool					ReachedPos( const budVec3& pos, const moveCommand_t moveCommand ) const;
	float					TravelDistance( const budVec3& start, const budVec3& end ) const;
	int						PointReachableAreaNum( const budVec3& pos, const float boundsScale = 2.0f ) const;
	bool					PathToGoal( aasPath_t& path, int areaNum, const budVec3& origin, int goalAreaNum, const budVec3& goalOrigin ) const;
	void					DrawRoute() const;
	bool					GetMovePos( budVec3& seekPos );
	bool					MoveDone() const;
	bool					EntityCanSeePos( budActor* actor, const budVec3& actorOrigin, const budVec3& pos );
	void					BlockedFailSafe();
	
	// movement control
	void					StopMove( moveStatus_t status );
	bool					FaceEnemy();
	bool					FaceEntity( idEntity* ent );
	bool					DirectMoveToPosition( const budVec3& pos );
	bool					MoveToEnemyHeight();
	bool					MoveOutOfRange( idEntity* entity, float range );
	bool					MoveToAttackPosition( idEntity* ent, int attack_anim );
	bool					MoveToEnemy();
	bool					MoveToEntity( idEntity* ent );
	bool					MoveToPosition( const budVec3& pos );
	bool					MoveToCover( idEntity* entity, const budVec3& pos );
	bool					SlideToPosition( const budVec3& pos, float time );
	bool					WanderAround();
	bool					StepDirection( float dir );
	bool					NewWanderDir( const budVec3& dest );
	
	// effects
	const budDeclParticle*	SpawnParticlesOnJoint( particleEmitter_t& pe, const char* particleName, const char* jointName );
	void					SpawnParticles( const char* keyName );
	bool					ParticlesActive();
	
	// turning
	bool					FacingIdeal();
	void					Turn();
	bool					TurnToward( float yaw );
	bool					TurnToward( const budVec3& pos );
	
	// enemy management
	void					ClearEnemy();
	bool					EnemyPositionValid() const;
	void					SetEnemyPosition();
	void					UpdateEnemyPosition();
	void					SetEnemy( budActor* newEnemy );
	
	// attacks
	void					CreateProjectileClipModel() const;
	idProjectile*			CreateProjectile( const budVec3& pos, const budVec3& dir );
	void					RemoveProjectile();
	idProjectile*			LaunchProjectile( const char* jointname, idEntity* target, bool clampToAttackCone );
	virtual void			DamageFeedback( idEntity* victim, idEntity* inflictor, int& damage );
	void					DirectDamage( const char* meleeDefName, idEntity* ent );
	bool					TestMelee() const;
	bool					AttackMelee( const char* meleeDefName );
	void					BeginAttack( const char* name );
	void					EndAttack();
	void					PushWithAF();
	
	// special effects
	void					GetMuzzle( const char* jointname, budVec3& muzzle, budMat3& axis );
	void					InitMuzzleFlash();
	void					TriggerWeaponEffects( const budVec3& muzzle );
	void					UpdateMuzzleFlash();
	virtual bool			UpdateAnimationControllers();
	void					UpdateParticles();
	void					TriggerParticles( const char* jointName );
	
	void					TriggerFX( const char* joint, const char* fx );
	idEntity*				StartEmitter( const char* name, const char* joint, const char* particle );
	idEntity*				GetEmitter( const char* name );
	void					StopEmitter( const char* name );
	
	// AI script state management
	void					LinkScriptVariables();
	void					UpdateAIScript();
	
	//
	// ai/ai_events.cpp
	//
	void					Event_Activate( idEntity* activator );
	void					Event_Touch( idEntity* other, trace_t* trace );
	void					Event_FindEnemy( int useFOV );
	void					Event_FindEnemyAI( int useFOV );
	void					Event_FindEnemyInCombatNodes();
	void					Event_ClosestReachableEnemyOfEntity( idEntity* team_mate );
	void					Event_HeardSound( int ignore_team );
	void					Event_SetEnemy( idEntity* ent );
	void					Event_ClearEnemy();
	void					Event_MuzzleFlash( const char* jointname );
	void					Event_CreateMissile( const char* jointname );
	void					Event_AttackMissile( const char* jointname );
	void					Event_FireMissileAtTarget( const char* jointname, const char* targetname );
	void					Event_LaunchMissile( const budVec3& muzzle, const budAngles& ang );
	void					Event_LaunchHomingMissile();
	void					Event_SetHomingMissileGoal();
	void					Event_LaunchProjectile( const char* entityDefName );
	void					Event_AttackMelee( const char* meleeDefName );
	void					Event_DirectDamage( idEntity* damageTarget, const char* damageDefName );
	void					Event_RadiusDamageFromJoint( const char* jointname, const char* damageDefName );
	void					Event_BeginAttack( const char* name );
	void					Event_EndAttack();
	void					Event_MeleeAttackToJoint( const char* jointname, const char* meleeDefName );
	void					Event_RandomPath();
	void					Event_CanBecomeSolid();
	void					Event_BecomeSolid();
	void					Event_BecomeNonSolid();
	void					Event_BecomeRagdoll();
	void					Event_StopRagdoll();
	void					Event_SetHealth( float newHealth );
	void					Event_GetHealth();
	void					Event_AllowDamage();
	void					Event_IgnoreDamage();
	void					Event_GetCurrentYaw();
	void					Event_TurnTo( float angle );
	void					Event_TurnToPos( const budVec3& pos );
	void					Event_TurnToEntity( idEntity* ent );
	void					Event_MoveStatus();
	void					Event_StopMove();
	void					Event_MoveToCover();
	void					Event_MoveToEnemy();
	void					Event_MoveToEnemyHeight();
	void					Event_MoveOutOfRange( idEntity* entity, float range );
	void					Event_MoveToAttackPosition( idEntity* entity, const char* attack_anim );
	void					Event_MoveToEntity( idEntity* ent );
	void					Event_MoveToPosition( const budVec3& pos );
	void					Event_SlideTo( const budVec3& pos, float time );
	void					Event_Wander();
	void					Event_FacingIdeal();
	void					Event_FaceEnemy();
	void					Event_FaceEntity( idEntity* ent );
	void					Event_WaitAction( const char* waitForState );
	void					Event_GetCombatNode();
	void					Event_EnemyInCombatCone( idEntity* ent, int use_current_enemy_location );
	void					Event_WaitMove();
	void					Event_GetJumpVelocity( const budVec3& pos, float speed, float max_height );
	void					Event_EntityInAttackCone( idEntity* ent );
	void					Event_CanSeeEntity( idEntity* ent );
	void					Event_SetTalkTarget( idEntity* target );
	void					Event_GetTalkTarget();
	void					Event_SetTalkState( int state );
	void					Event_EnemyRange();
	void					Event_EnemyRange2D();
	void					Event_GetEnemy();
	void					Event_GetEnemyPos();
	void					Event_GetEnemyEyePos();
	void					Event_PredictEnemyPos( float time );
	void					Event_CanHitEnemy();
	void					Event_CanHitEnemyFromAnim( const char* animname );
	void					Event_CanHitEnemyFromJoint( const char* jointname );
	void					Event_EnemyPositionValid();
	void					Event_ChargeAttack( const char* damageDef );
	void					Event_TestChargeAttack();
	void					Event_TestAnimMoveTowardEnemy( const char* animname );
	void					Event_TestAnimMove( const char* animname );
	void					Event_TestMoveToPosition( const budVec3& position );
	void					Event_TestMeleeAttack();
	void					Event_TestAnimAttack( const char* animname );
	void					Event_Burn();
	void					Event_PreBurn();
	void					Event_ClearBurn();
	void					Event_SetSmokeVisibility( int num, int on );
	void					Event_NumSmokeEmitters();
	void					Event_StopThinking();
	void					Event_GetTurnDelta();
	void					Event_GetMoveType();
	void					Event_SetMoveType( int moveType );
	void					Event_SaveMove();
	void					Event_RestoreMove();
	void					Event_AllowMovement( float flag );
	void					Event_JumpFrame();
	void					Event_EnableClip();
	void					Event_DisableClip();
	void					Event_EnableGravity();
	void					Event_DisableGravity();
	void					Event_EnableAFPush();
	void					Event_DisableAFPush();
	void					Event_SetFlySpeed( float speed );
	void					Event_SetFlyOffset( int offset );
	void					Event_ClearFlyOffset();
	void					Event_GetClosestHiddenTarget( const char* type );
	void					Event_GetRandomTarget( const char* type );
	void					Event_TravelDistanceToPoint( const budVec3& pos );
	void					Event_TravelDistanceToEntity( idEntity* ent );
	void					Event_TravelDistanceBetweenPoints( const budVec3& source, const budVec3& dest );
	void					Event_TravelDistanceBetweenEntities( idEntity* source, idEntity* dest );
	void					Event_LookAtEntity( idEntity* ent, float duration );
	void					Event_LookAtEnemy( float duration );
	void					Event_SetJointMod( int allowJointMod );
	void					Event_ThrowMoveable();
	void					Event_ThrowAF();
	void					Event_SetAngles( budAngles const& ang );
	void					Event_GetAngles();
	void					Event_GetTrajectoryToPlayer();
	void					Event_RealKill();
	void					Event_Kill();
	void					Event_WakeOnFlashlight( int enable );
	void					Event_LocateEnemy();
	void					Event_KickObstacles( idEntity* kickEnt, float force );
	void					Event_GetObstacle();
	void					Event_PushPointIntoAAS( const budVec3& pos );
	void					Event_GetTurnRate();
	void					Event_SetTurnRate( float rate );
	void					Event_AnimTurn( float angles );
	void					Event_AllowHiddenMovement( int enable );
	void					Event_TriggerParticles( const char* jointName );
	void					Event_FindActorsInBounds( const budVec3& mins, const budVec3& maxs );
	void 					Event_CanReachPosition( const budVec3& pos );
	void 					Event_CanReachEntity( idEntity* ent );
	void					Event_CanReachEnemy();
	void					Event_GetReachableEntityPosition( idEntity* ent );
	void					Event_MoveToPositionDirect( const budVec3& pos );
	void					Event_AvoidObstacles( int ignore );
	void					Event_TriggerFX( const char* joint, const char* fx );
	
	void					Event_StartEmitter( const char* name, const char* joint, const char* particle );
	void					Event_GetEmitter( const char* name );
	void					Event_StopEmitter( const char* name );
};

class idCombatNode : public idEntity
{
public:
	CLASS_PROTOTYPE( idCombatNode );
	
	idCombatNode();
	
	void				Save( idSaveGame* savefile ) const;
	void				Restore( idRestoreGame* savefile );
	
	void				Spawn();
	bool				IsDisabled() const;
	bool				EntityInView( budActor* actor, const budVec3& pos );
	static void			DrawDebugInfo();
	
private:
	float				min_dist;
	float				max_dist;
	float				cone_dist;
	float				min_height;
	float				max_height;
	budVec3				cone_left;
	budVec3				cone_right;
	budVec3				offset;
	bool				disabled;
	
	void				Event_Activate( idEntity* activator );
	void				Event_MarkUsed();
};

#endif /* !__AI_H__ */
