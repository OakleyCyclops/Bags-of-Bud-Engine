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

#ifndef __GAME_ITEM_H__
#define __GAME_ITEM_H__


/*
===============================================================================

  Items the player can pick up or use.

===============================================================================
*/

/*
================================================
These flags are passed to the Give functions
to set their behavior. We need to be able to
separate the feedback from the actual
state modification so that we can hide lag
on MP clients.

For the previous behavior of functions which
take a giveFlags parameter (this is usually
desired on the server too) pass
ITEM_GIVE_FEEDBACK | ITEM_GIVE_UPDATE_STATE.
================================================
*/
enum itemGiveFlags_t
{
	ITEM_GIVE_FEEDBACK			= BIT( 0 ),
	ITEM_GIVE_UPDATE_STATE		= BIT( 1 ),
	ITEM_GIVE_FROM_WEAPON		= BIT( 2 ),			// indicates this was given via a weapon's launchPowerup (for bloodstone powerups)
};

class idItem : public idEntity
{
public:
	CLASS_PROTOTYPE( idItem );
	
	idItem();
	virtual					~idItem();
	
	void					Save( idSaveGame* savefile ) const;
	void					Restore( idRestoreGame* savefile );
	
	void					Spawn();
	void					GetAttributes( Dict& attributes ) const;
	virtual bool			GiveToPlayer( budPlayer* player, unsigned int giveFlags );
	virtual bool			Pickup( budPlayer* player );
	virtual void			Think();
	virtual void			Present();
	
	enum
	{
		EVENT_PICKUP = idEntity::EVENT_MAXEVENTS,
		EVENT_RESPAWN,
		EVENT_RESPAWNFX,
		EVENT_TAKEFLAG,
		EVENT_DROPFLAG,
		EVENT_FLAGRETURN,
		EVENT_FLAGCAPTURE,
		EVENT_MAXEVENTS
	};
	
	void					ClientThink( const int curTime, const float fraction, const bool predict );
	virtual void			ClientPredictionThink();
	virtual bool			ClientReceiveEvent( int event, int time, const budBitMsg& msg );
	
	// networking
	virtual void			WriteToSnapshot( budBitMsg& msg ) const;
	virtual void			ReadFromSnapshot( const budBitMsg& msg );
	
protected:
	int						GetPredictPickupMilliseconds() const
	{
		return clientPredictPickupMilliseconds;
	}
	
private:
	Vector3					orgOrigin;
	bool					spin;
	bool					pulse;
	bool					canPickUp;
	
	// for item pulse effect
	int						itemShellHandle;
	const budMaterial* 		shellMaterial;
	
	// used to update the item pulse effect
	mutable bool			inView;
	mutable int				inViewTime;
	mutable int				lastCycle;
	mutable int				lastRenderViewTime;
	
	// used for prediction in mp
	int						clientPredictPickupMilliseconds;
	
	bool					UpdateRenderEntity( renderEntity_s* renderEntity, const renderView_t* renderView ) const;
	static bool				ModelCallback( renderEntity_s* renderEntity, const renderView_t* renderView );
	
	void					Event_DropToFloor();
	void					Event_Touch( idEntity* other, trace_t* trace );
	void					Event_Trigger( idEntity* activator );
	void					Event_Respawn();
	void					Event_RespawnFx();
};

class idItemPowerup : public idItem
{
public:
	CLASS_PROTOTYPE( idItemPowerup );
	
	idItemPowerup();
	
	void					Save( idSaveGame* savefile ) const;
	void					Restore( idRestoreGame* savefile );
	
	void					Spawn();
	virtual bool			GiveToPlayer( budPlayer* player, unsigned int giveFlags );
	
private:
	int						time;
	int						type;
};

class idObjective : public idItem
{
public:
	CLASS_PROTOTYPE( idObjective );
	
	idObjective();
	
	void					Save( idSaveGame* savefile ) const;
	void					Restore( idRestoreGame* savefile );
	
	void					Spawn();
	
private:
	Vector3					playerPos;
	const budMaterial* 		screenshot;
	
	void					Event_Trigger( idEntity* activator );
	void					Event_HideObjective( idEntity* e );
	void					Event_GetPlayerPos();
};

class idVideoCDItem : public idItem
{
public:
	CLASS_PROTOTYPE( idVideoCDItem );
	
	virtual bool			GiveToPlayer( budPlayer* player, unsigned int giveFlags );
};

class idPDAItem : public idItem
{
public:
	CLASS_PROTOTYPE( idPDAItem );
	
	virtual bool			GiveToPlayer( budPlayer* player, unsigned int giveFlags );
};

class idMoveableItem : public idItem
{
public:
	CLASS_PROTOTYPE( idMoveableItem );
	
	idMoveableItem();
	virtual					~idMoveableItem();
	
	void					Save( idSaveGame* savefile ) const;
	void					Restore( idRestoreGame* savefile );
	
	void					Spawn();
	virtual void			Think();
	void					ClientThink( const int curTime, const float fraction, const bool predict );
	virtual bool			Collide( const trace_t& collision, const Vector3& velocity );
	virtual bool			Pickup( budPlayer* player );
	
	static void				DropItems( budAnimatedEntity* ent, const char* type, List<idEntity*>* list );
	static idEntity*			DropItem( const char* classname, const Vector3& origin, const Matrix3& axis, const Vector3& velocity, int activateDelay, int removeDelay );
	
	virtual void			WriteToSnapshot( budBitMsg& msg ) const;
	virtual void			ReadFromSnapshot( const budBitMsg& msg );
	
protected:
	idPhysics_RigidBody		physicsObj;
	budClipModel* 			trigger;
	const budDeclParticle* 	smoke;
	int						smokeTime;
	
	int						nextSoundTime;
	bool					repeatSmoke;	// never stop updating the particles
	
	void					Gib( const Vector3& dir, const char* damageDefName );
	
	void					Event_DropToFloor();
	void					Event_Gib( const char* damageDefName );
};

class idItemTeam : public idMoveableItem
{
public:
	CLASS_PROTOTYPE( idItemTeam );
	
	idItemTeam();
	virtual					~idItemTeam();
	
	void                    Spawn();
	virtual bool			Pickup( budPlayer* player );
	virtual bool			ClientReceiveEvent( int event, int time, const budBitMsg& msg );
	virtual void			Think();
	
	void					Drop( bool death = false );	// was the drop caused by death of carrier?
	void					Return( budPlayer* player = NULL );
	void					Capture();
	
	virtual void			FreeLightDef();
	virtual void			Present();
	
	// networking
	virtual void			WriteToSnapshot( budBitMsg& msg ) const;
	virtual void			ReadFromSnapshot( const budBitMsg& msg );
	
public:
	int                     team;
	// TODO : turn this into a state :
	bool					carried;			// is it beeing carried by a player?
	bool					dropped;			// was it dropped?
	
private:
	Vector3					returnOrigin;
	Matrix3					returnAxis;
	int						lastDrop;
	
	const budDeclSkin* 		skinDefault;
	const budDeclSkin* 		skinCarried;
	
	const function_t* 		scriptTaken;
	const function_t* 		scriptDropped;
	const function_t* 		scriptReturned;
	const function_t* 		scriptCaptured;
	
	renderLight_t           itemGlow;           // Used by flags when they are picked up
	int                     itemGlowHandle;
	
	int						lastNuggetDrop;
	const char* 			nuggetName;
	
private:

	void					Event_TakeFlag( budPlayer* player );
	void					Event_DropFlag( bool death );
	void					Event_FlagReturn( budPlayer* player = NULL );
	void					Event_FlagCapture();
	
	void					PrivateReturn();
	function_t* 			LoadScript( const char* script );
	
	void					SpawnNugget( Vector3 pos );
	void                    UpdateGuis();
};

class idMoveablePDAItem : public idMoveableItem
{
public:
	CLASS_PROTOTYPE( idMoveablePDAItem );
	
	virtual bool			GiveToPlayer( budPlayer* player, unsigned int giveFlags );
};

/*
===============================================================================

  Item removers.

===============================================================================
*/

class idItemRemover : public idEntity
{
public:
	CLASS_PROTOTYPE( idItemRemover );
	
	void					Spawn();
	void					RemoveItem( budPlayer* player );
	
private:
	void					Event_Trigger( idEntity* activator );
};

class idObjectiveComplete : public idItemRemover
{
public:
	CLASS_PROTOTYPE( idObjectiveComplete );
	
	idObjectiveComplete();
	
	void					Save( idSaveGame* savefile ) const;
	void					Restore( idRestoreGame* savefile );
	
	void					Spawn();
	
private:
	Vector3					playerPos;
	
	void					Event_Trigger( idEntity* activator );
	void					Event_HideObjective( idEntity* e );
	void					Event_GetPlayerPos();
};

#endif /* !__GAME_ITEM_H__ */
