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

#pragma hdrstop
#include "PCH.hpp"


#include "../Game_local.h"

#if defined( _DEBUG )
	#define	BUILD_DEBUG	"-debug"
#else
	#define	BUILD_DEBUG "-release"
#endif

/*
All game cvars should be defined here.
*/

struct gameVersion_s {
	gameVersion_s() { sprintf( string, "%s.%d%s %s %s %s", ENGINE_VERSION, BUILD_NUMBER, BUILD_DEBUG, BUILD_STRING, __DATE__, __TIME__ ); }
	char	string[256];
} gameVersion;


// noset vars
budCVar gamename(					"gamename",					GAME_VERSION,	CVAR_GAME | CVAR_ROM, "" );
budCVar gamedate(					"gamedate",					__DATE__,		CVAR_GAME | CVAR_ROM, "" );

budCVar si_map(						"si_map",					"-1",			CVAR_GAME | CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_INTEGER, "default map choice for profile" );
budCVar si_mode(						"si_mode",					"-1",			CVAR_GAME | CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_INTEGER, "default mode choice for profile", -1, GAME_COUNT - 1 );
budCVar si_fragLimit(				"si_fragLimit",				"10",			CVAR_GAME | CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_INTEGER, "frag limit", 1, MP_PLAYER_MAXFRAGS );
budCVar si_timeLimit(				"si_timeLimit",				"10",			CVAR_GAME | CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_INTEGER, "time limit in minutes", 0, 60 );
budCVar si_teamDamage(				"si_teamDamage",			"0",			CVAR_GAME | CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_BOOL, "enable team damage" );
budCVar si_spectators(				"si_spectators",			"1",			CVAR_GAME | CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_BOOL, "allow spectators or require all clients to play" );

//budCVar si_pointLimit(				"si_pointlimit",			"8",			CVAR_GAME | CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_INTEGER, "team points limit to win in CTF" );
budCVar si_flagDropTimeLimit(		"si_flagDropTimeLimit",		"30",			CVAR_GAME | CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_INTEGER, "seconds before a dropped CTF flag is returned" );
budCVar si_midnight(                 "si_midnight",              "0",            CVAR_GAME | CVAR_INTEGER | CVAR_SERVERINFO, "Start the game up in midnight CTF (completely dark)" );

// change anytime vars
budCVar developer(					"developer",				"0",			CVAR_GAME | CVAR_BOOL, "" );

budCVar g_cinematic(					"g_cinematic",				"1",			CVAR_GAME | CVAR_BOOL, "skips updating entities that aren't marked 'cinematic' '1' during cinematics" );
budCVar g_cinematicMaxSkipTime(		"g_cinematicMaxSkipTime",	"600",			CVAR_GAME | CVAR_FLOAT, "# of seconds to allow game to run when skipping cinematic.  prevents lock-up when cinematic doesn't end.", 0, 3600 );

budCVar g_muzzleFlash(				"g_muzzleFlash",			"1",			CVAR_GAME | CVAR_ARCHIVE | CVAR_BOOL, "show muzzle flashes" );
budCVar g_projectileLights(			"g_projectileLights",		"1",			CVAR_GAME | CVAR_ARCHIVE | CVAR_BOOL, "show dynamic lights on projectiles" );
budCVar g_bloodEffects(				"g_bloodEffects",			"1",			CVAR_GAME | CVAR_ARCHIVE | CVAR_BOOL, "show blood splats, sprays and gibs" );
budCVar g_monsters(					"g_monsters",				"1",			CVAR_GAME | CVAR_BOOL, "" );
budCVar g_decals(					"g_decals",					"1",			CVAR_GAME | CVAR_ARCHIVE | CVAR_BOOL, "show decals such as bullet holes" );
budCVar g_knockback(					"g_knockback",				"1000",			CVAR_GAME | CVAR_INTEGER, "" );
budCVar g_skill(						"g_skill",					"1",			CVAR_GAME | CVAR_INTEGER, "" );
budCVar g_nightmare(					"g_nightmare",				"0",			CVAR_GAME | CVAR_ARCHIVE | CVAR_BOOL, "if nightmare mode is allowed" );
budCVar g_roeNightmare(				"g_roeNightmare",			"0",			CVAR_GAME | CVAR_ARCHIVE | CVAR_BOOL, "if nightmare mode is allowed for roe" );
budCVar g_leNightmare(				"g_leNightmare",			"0",			CVAR_GAME | CVAR_ARCHIVE | CVAR_BOOL, "if nightmare mode is allowed for le" );
budCVar g_gravity(					"g_gravity",		DEFAULT_GRAVITY_STRING, CVAR_GAME | CVAR_FLOAT, "" );
budCVar g_skipFX(					"g_skipFX",					"0",			CVAR_GAME | CVAR_BOOL, "" );

budCVar g_disasm(					"g_disasm",					"0",			CVAR_GAME | CVAR_BOOL, "disassemble script into base/script/disasm.txt on the local drive when script is compiled" );
budCVar g_debugBounds(				"g_debugBounds",			"0",			CVAR_GAME | CVAR_BOOL, "checks for models with bounds > 2048" );
budCVar g_debugAnim(					"g_debugAnim",				"-1",			CVAR_GAME | CVAR_INTEGER, "displays information on which animations are playing on the specified entity number.  set to -1 to disable." );
budCVar g_debugMove(					"g_debugMove",				"0",			CVAR_GAME | CVAR_BOOL, "" );
budCVar g_debugDamage(				"g_debugDamage",			"0",			CVAR_GAME | CVAR_BOOL, "" );
budCVar g_debugWeapon(				"g_debugWeapon",			"0",			CVAR_GAME | CVAR_BOOL, "" );
budCVar g_debugScript(				"g_debugScript",			"0",			CVAR_GAME | CVAR_BOOL, "" );
budCVar g_debugMover(				"g_debugMover",				"0",			CVAR_GAME | CVAR_BOOL, "" );
budCVar g_debugTriggers(				"g_debugTriggers",			"0",			CVAR_GAME | CVAR_BOOL, "" );
budCVar g_debugCinematic(			"g_debugCinematic",			"0",			CVAR_GAME | CVAR_BOOL, "" );
budCVar g_stopTime(					"g_stopTime",				"0",			CVAR_GAME | CVAR_BOOL, "" );
budCVar g_damageScale(				"g_damageScale",			"1",			CVAR_GAME | CVAR_FLOAT | CVAR_ARCHIVE, "scale final damage on player by this factor" );
budCVar g_armorProtection(			"g_armorProtection",		"0.3",			CVAR_GAME | CVAR_FLOAT | CVAR_ARCHIVE, "armor takes this percentage of damage" );
budCVar g_armorProtectionMP(			"g_armorProtectionMP",		"0.6",			CVAR_GAME | CVAR_FLOAT | CVAR_ARCHIVE, "armor takes this percentage of damage in mp" );
budCVar g_useDynamicProtection(		"g_useDynamicProtection",	"1",			CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "scale damage and armor dynamically to keep the player alive more often" );
budCVar g_healthTakeTime(			"g_healthTakeTime",			"5",			CVAR_GAME | CVAR_INTEGER | CVAR_ARCHIVE, "how often to take health in nightmare mode" );
budCVar g_healthTakeAmt(				"g_healthTakeAmt",			"5",			CVAR_GAME | CVAR_INTEGER | CVAR_ARCHIVE, "how much health to take in nightmare mode" );
budCVar g_healthTakeLimit(			"g_healthTakeLimit",		"25",			CVAR_GAME | CVAR_INTEGER | CVAR_ARCHIVE, "how low can health get taken in nightmare mode" );



budCVar g_showPVS(					"g_showPVS",				"0",			CVAR_GAME | CVAR_INTEGER, "", 0, 2 );
budCVar g_showTargets(				"g_showTargets",			"0",			CVAR_GAME | CVAR_BOOL, "draws entities and thier targets.  hidden entities are drawn grey." );
budCVar g_showTriggers(				"g_showTriggers",			"0",			CVAR_GAME | CVAR_BOOL, "draws trigger entities (orange) and thier targets (green).  disabled triggers are drawn grey." );
budCVar g_showCollisionWorld(		"g_showCollisionWorld",		"0",			CVAR_GAME | CVAR_BOOL, "" );
budCVar g_showCollisionModels(		"g_showCollisionModels",	"0",			CVAR_GAME | CVAR_BOOL, "" );
budCVar g_showCollisionTraces(		"g_showCollisionTraces",	"0",			CVAR_GAME | CVAR_BOOL, "" );
budCVar g_maxShowDistance(			"g_maxShowDistance",		"128",			CVAR_GAME | CVAR_FLOAT, "" );
budCVar g_showEntityInfo(			"g_showEntityInfo",			"0",			CVAR_GAME | CVAR_BOOL, "" );
budCVar g_showviewpos(				"g_showviewpos",			"0",			CVAR_GAME | CVAR_BOOL, "" );
budCVar g_showcamerainfo(			"g_showcamerainfo",			"0",			CVAR_GAME | CVAR_ARCHIVE, "displays the current frame # for the camera when playing cinematics" );
budCVar g_showTestModelFrame(		"g_showTestModelFrame",		"0",			CVAR_GAME | CVAR_BOOL, "displays the current animation and frame # for testmodels" );
budCVar g_showActiveEntities(		"g_showActiveEntities",		"0",			CVAR_GAME | CVAR_BOOL, "draws boxes around thinking entities.  dormant entities (outside of pvs) are drawn yellow.  non-dormant are green." );
budCVar g_showEnemies(				"g_showEnemies",			"0",			CVAR_GAME | CVAR_BOOL, "draws boxes around monsters that have targeted the the player" );

budCVar g_frametime(					"g_frametime",				"0",			CVAR_GAME | CVAR_BOOL, "displays timing information for each game frame" );
budCVar g_timeentities(				"g_timeEntities",			"0",			CVAR_GAME | CVAR_FLOAT, "when non-zero, shows entities whose think functions exceeded the # of milliseconds specified" );

budCVar g_debugShockwave(			"g_debugShockwave",			"0",			CVAR_GAME | CVAR_BOOL, "Debug the shockwave" );

budCVar g_enableSlowmo(				"g_enableSlowmo",			"0",			CVAR_GAME | CVAR_BOOL, "for testing purposes only" );
budCVar g_slowmoStepRate(			"g_slowmoStepRate",			"0.02",			CVAR_GAME | CVAR_FLOAT, "" );

budCVar g_enablePortalSky(			"g_enablePortalSky",		"1",			CVAR_GAME | CVAR_BOOL, "enables the portal sky" );
budCVar g_testFullscreenFX(			"g_testFullscreenFX",		"-1",			CVAR_GAME | CVAR_INTEGER, "index will activate specific fx, -2 is for all on, -1 is off" );
budCVar g_testHelltimeFX(			"g_testHelltimeFX",			"-1",			CVAR_GAME | CVAR_INTEGER, "set to 0, 1, 2 to test helltime, -1 is off" );
budCVar g_testMultiplayerFX(			"g_testMultiplayerFX",		"-1",			CVAR_GAME | CVAR_INTEGER, "set to 0, 1, 2 to test multiplayer, -1 is off" );

budCVar g_moveableDamageScale(		"g_moveableDamageScale",	"0.1",			CVAR_GAME | CVAR_FLOAT, "scales damage wrt mass of object in multiplayer" );

budCVar g_testBloomIntensity(		"g_testBloomIntensity",		"-0.01",		CVAR_GAME | CVAR_FLOAT, "" );
budCVar g_testBloomNumPasses(		"g_testBloomNumPasses",		"30",			CVAR_GAME | CVAR_INTEGER, "" );

budCVar ai_debugScript(				"ai_debugScript",			"-1",			CVAR_GAME | CVAR_INTEGER, "displays script calls for the specified monster entity number" );
budCVar ai_debugMove(				"ai_debugMove",				"0",			CVAR_GAME | CVAR_BOOL, "draws movement information for monsters" );
budCVar ai_debugTrajectory(			"ai_debugTrajectory",		"0",			CVAR_GAME | CVAR_BOOL, "draws trajectory tests for monsters" );
budCVar ai_testPredictPath(			"ai_testPredictPath",		"0",			CVAR_GAME | CVAR_BOOL, "" );
budCVar ai_showCombatNodes(			"ai_showCombatNodes",		"0",			CVAR_GAME | CVAR_BOOL, "draws attack cones for monsters" );
budCVar ai_showPaths(				"ai_showPaths",				"0",			CVAR_GAME | CVAR_BOOL, "draws path_* entities" );
budCVar ai_showObstacleAvoidance(	"ai_showObstacleAvoidance",	"0",			CVAR_GAME | CVAR_INTEGER, "draws obstacle avoidance information for monsters.  if 2, draws obstacles for player, as well", 0, 2, budCmdSystem::ArgCompletion_Integer<0,2> );
budCVar ai_blockedFailSafe(			"ai_blockedFailSafe",		"1",			CVAR_GAME | CVAR_BOOL, "enable blocked fail safe handling" );

budCVar ai_showHealth(				"ai_showHealth",			"0",			CVAR_GAME | CVAR_BOOL, "Draws the AI's health above its head" );

budCVar g_dvTime(					"g_dvTime",					"1",			CVAR_GAME | CVAR_FLOAT, "" );
budCVar g_dvAmplitude(				"g_dvAmplitude",			"0.001",		CVAR_GAME | CVAR_FLOAT, "" );
budCVar g_dvFrequency(				"g_dvFrequency",			"0.5",			CVAR_GAME | CVAR_FLOAT, "" );

budCVar g_kickTime(					"g_kickTime",				"1",			CVAR_GAME | CVAR_FLOAT, "" );
budCVar g_kickAmplitude(				"g_kickAmplitude",			"0.0001",		CVAR_GAME | CVAR_FLOAT, "" );
budCVar g_blobTime(					"g_blobTime",				"1",			CVAR_GAME | CVAR_FLOAT, "" );
budCVar g_blobSize(					"g_blobSize",				"1",			CVAR_GAME | CVAR_FLOAT, "" );

budCVar g_testHealthVision(			"g_testHealthVision",		"0",			CVAR_GAME | CVAR_FLOAT, "" );
budCVar g_editEntityMode(			"g_editEntityMode",			"0",			CVAR_GAME | CVAR_INTEGER,	"0 = off\n"
																											"1 = lights\n"
																											"2 = sounds\n"
																											"3 = articulated figures\n"
																											"4 = particle systems\n"
																											"5 = monsters\n"
																											"6 = entity names\n"
																											"7 = entity models", 0, 7, budCmdSystem::ArgCompletion_Integer<0,7> );
budCVar g_dragEntity(				"g_dragEntity",				"0",			CVAR_GAME | CVAR_BOOL, "allows dragging physics objects around by placing the crosshair over them and holding the fire button" );
budCVar g_dragDamping(				"g_dragDamping",			"0.5",			CVAR_GAME | CVAR_FLOAT, "" );
budCVar g_dragShowSelection(			"g_dragShowSelection",		"0",			CVAR_GAME | CVAR_BOOL, "" );
budCVar g_dropItemRotation(			"g_dropItemRotation",		"",				CVAR_GAME, "" );

// Note: These cvars do not necessarily need to be in the shipping game. 
budCVar g_flagAttachJoint( "g_flagAttachJoint", "Chest", CVAR_GAME | CVAR_CHEAT, "player joint to attach CTF flag to" );
budCVar g_flagAttachOffsetX( "g_flagAttachOffsetX", "8", CVAR_GAME | CVAR_CHEAT, "X offset of CTF flag when carried" );
budCVar g_flagAttachOffsetY( "g_flagAttachOffsetY", "4", CVAR_GAME | CVAR_CHEAT, "Y offset of CTF flag when carried" );
budCVar g_flagAttachOffsetZ( "g_flagAttachOffsetZ", "-12", CVAR_GAME | CVAR_CHEAT, "Z offset of CTF flag when carried" );
budCVar g_flagAttachAngleX( "g_flagAttachAngleX", "90", CVAR_GAME | CVAR_CHEAT, "X angle of CTF flag when carried" );
budCVar g_flagAttachAngleY( "g_flagAttachAngleY", "25", CVAR_GAME | CVAR_CHEAT, "Y angle of CTF flag when carried" );
budCVar g_flagAttachAngleZ( "g_flagAttachAngleZ", "-90", CVAR_GAME | CVAR_CHEAT, "Z angle of CTF flag when carried" );


budCVar g_vehicleVelocity(			"g_vehicleVelocity",		"1000",			CVAR_GAME | CVAR_FLOAT, "" );
budCVar g_vehicleForce(				"g_vehicleForce",			"50000",		CVAR_GAME | CVAR_FLOAT, "" );
budCVar g_vehicleSuspensionUp(		"g_vehicleSuspensionUp",	"32",			CVAR_GAME | CVAR_FLOAT, "" );
budCVar g_vehicleSuspensionDown(		"g_vehicleSuspensionDown",	"20",			CVAR_GAME | CVAR_FLOAT, "" );
budCVar g_vehicleSuspensionKCompress("g_vehicleSuspensionKCompress","200",		CVAR_GAME | CVAR_FLOAT, "" );
budCVar g_vehicleSuspensionDamping(	"g_vehicleSuspensionDamping","400",			CVAR_GAME | CVAR_FLOAT, "" );
budCVar g_vehicleTireFriction(		"g_vehicleTireFriction",	"0.8",			CVAR_GAME | CVAR_FLOAT, "" );
budCVar g_vehicleDebug(				"g_vehicleDebug",			"0",			CVAR_GAME | CVAR_BOOL, "" );

budCVar ik_enable(					"ik_enable",				"1",			CVAR_GAME | CVAR_BOOL, "enable IK" );
budCVar ik_debug(					"ik_debug",					"0",			CVAR_GAME | CVAR_BOOL, "show IK debug lines" );

budCVar af_useLinearTime(			"af_useLinearTime",			"1",			CVAR_GAME | CVAR_BOOL, "use linear time algorithm for tree-like structures" );
budCVar af_useImpulseFriction(		"af_useImpulseFriction",	"0",			CVAR_GAME | CVAR_BOOL, "use impulse based contact friction" );
budCVar af_useJointImpulseFriction(	"af_useJointImpulseFriction","0",			CVAR_GAME | CVAR_BOOL, "use impulse based joint friction" );
budCVar af_useSymmetry(				"af_useSymmetry",			"1",			CVAR_GAME | CVAR_BOOL, "use constraint matrix symmetry" );
budCVar af_skipSelfCollision(		"af_skipSelfCollision",		"0",			CVAR_GAME | CVAR_BOOL, "skip self collision detection" );
budCVar af_skipLimits(				"af_skipLimits",			"0",			CVAR_GAME | CVAR_BOOL, "skip joint limits" );
budCVar af_skipFriction(				"af_skipFriction",			"0",			CVAR_GAME | CVAR_BOOL, "skip friction" );
budCVar af_forceFriction(			"af_forceFriction",			"-1",			CVAR_GAME | CVAR_FLOAT, "force the given friction value" );
budCVar af_maxLinearVelocity(		"af_maxLinearVelocity",		"128",			CVAR_GAME | CVAR_FLOAT, "maximum linear velocity" );
budCVar af_maxAngularVelocity(		"af_maxAngularVelocity",	"1.57",			CVAR_GAME | CVAR_FLOAT, "maximum angular velocity" );
budCVar af_timeScale(				"af_timeScale",				"1",			CVAR_GAME | CVAR_FLOAT, "scales the time" );
budCVar af_jointFrictionScale(		"af_jointFrictionScale",	"0",			CVAR_GAME | CVAR_FLOAT, "scales the joint friction" );
budCVar af_contactFrictionScale(		"af_contactFrictionScale",	"0",			CVAR_GAME | CVAR_FLOAT, "scales the contact friction" );
budCVar af_highlightBody(			"af_highlightBody",			"",				CVAR_GAME, "name of the body to highlight" );
budCVar af_highlightConstraint(		"af_highlightConstraint",	"",				CVAR_GAME, "name of the constraint to highlight" );
budCVar af_showTimings(				"af_showTimings",			"0",			CVAR_GAME | CVAR_BOOL, "show articulated figure cpu usage" );
budCVar af_showConstraints(			"af_showConstraints",		"0",			CVAR_GAME | CVAR_BOOL, "show constraints" );
budCVar af_showConstraintNames(		"af_showConstraintNames",	"0",			CVAR_GAME | CVAR_BOOL, "show constraint names" );
budCVar af_showConstrainedBodies(	"af_showConstrainedBodies",	"0",			CVAR_GAME | CVAR_BOOL, "show the two bodies contrained by the highlighted constraint" );
budCVar af_showPrimaryOnly(			"af_showPrimaryOnly",		"0",			CVAR_GAME | CVAR_BOOL, "show primary constraints only" );
budCVar af_showTrees(				"af_showTrees",				"0",			CVAR_GAME | CVAR_BOOL, "show tree-like structures" );
budCVar af_showLimits(				"af_showLimits",			"0",			CVAR_GAME | CVAR_BOOL, "show joint limits" );
budCVar af_showBodies(				"af_showBodies",			"0",			CVAR_GAME | CVAR_BOOL, "show bodies" );
budCVar af_showBodyNames(			"af_showBodyNames",			"0",			CVAR_GAME | CVAR_BOOL, "show body names" );
budCVar af_showMass(					"af_showMass",				"0",			CVAR_GAME | CVAR_BOOL, "show the mass of each body" );
budCVar af_showTotalMass(			"af_showTotalMass",			"0",			CVAR_GAME | CVAR_BOOL, "show the total mass of each articulated figure" );
budCVar af_showInertia(				"af_showInertia",			"0",			CVAR_GAME | CVAR_BOOL, "show the inertia tensor of each body" );
budCVar af_showVelocity(				"af_showVelocity",			"0",			CVAR_GAME | CVAR_BOOL, "show the velocity of each body" );
budCVar af_showActive(				"af_showActive",			"0",			CVAR_GAME | CVAR_BOOL, "show tree-like structures of articulated figures not at rest" );
budCVar af_testSolid(				"af_testSolid",				"1",			CVAR_GAME | CVAR_BOOL, "test for bodies initially stuck in solid" );

budCVar rb_showTimings(				"rb_showTimings",			"0",			CVAR_GAME | CVAR_BOOL, "show rigid body cpu usage" );
budCVar rb_showBodies(				"rb_showBodies",			"0",			CVAR_GAME | CVAR_BOOL, "show rigid bodies" );
budCVar rb_showMass(					"rb_showMass",				"0",			CVAR_GAME | CVAR_BOOL, "show the mass of each rigid body" );
budCVar rb_showInertia(				"rb_showInertia",			"0",			CVAR_GAME | CVAR_BOOL, "show the inertia tensor of each rigid body" );
budCVar rb_showVelocity(				"rb_showVelocity",			"0",			CVAR_GAME | CVAR_BOOL, "show the velocity of each rigid body" );
budCVar rb_showActive(				"rb_showActive",			"0",			CVAR_GAME | CVAR_BOOL, "show rigid bodies that are not at rest" );

// The default values for player movement cvars are set in def/player.def
budCVar pm_jumpheight(				"pm_jumpheight",			"48",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "approximate hieght the player can jump" );
budCVar pm_stepsize(					"pm_stepsize",				"16",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "maximum height the player can step up without jumping" );
budCVar pm_crouchspeed(				"pm_crouchspeed",			"80",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "speed the player can move while crouched" );
budCVar pm_walkspeed(				"pm_walkspeed",				"140",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "speed the player can move while walking" );
budCVar pm_runspeed(					"pm_runspeed",				"220",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "speed the player can move while running" );
budCVar pm_noclipspeed(				"pm_noclipspeed",			"200",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "speed the player can move while in noclip" );
budCVar pm_spectatespeed(			"pm_spectatespeed",			"450",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "speed the player can move while spectating" );
budCVar pm_spectatebbox(				"pm_spectatebbox",			"32",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "size of the spectator bounding box" );
budCVar pm_usecylinder(				"pm_usecylinder",			"0",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_BOOL, "use a cylinder approximation instead of a bounding box for player collision detection" );
budCVar pm_minviewpitch(				"pm_minviewpitch",			"-89",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "amount player's view can look up (negative values are up)" );
budCVar pm_maxviewpitch(				"pm_maxviewpitch",			"89",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "amount player's view can look down" );
budCVar pm_stamina(					"pm_stamina",				"24",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "length of time player can run" );
budCVar pm_staminathreshold(			"pm_staminathreshold",		"45",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "when stamina drops below this value, player gradually slows to a walk" );
budCVar pm_staminarate(				"pm_staminarate",			"0.75",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "rate that player regains stamina. divide pm_stamina by this value to determine how long it takes to fully recharge." );
budCVar pm_crouchheight(				"pm_crouchheight",			"38",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "height of player's bounding box while crouched" );
budCVar pm_crouchviewheight(			"pm_crouchviewheight",		"32",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "height of player's view while crouched" );
budCVar pm_normalheight(				"pm_normalheight",			"74",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "height of player's bounding box while standing" );
budCVar pm_normalviewheight(			"pm_normalviewheight",		"68",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "height of player's view while standing" );
budCVar pm_deadheight(				"pm_deadheight",			"20",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "height of player's bounding box while dead" );
budCVar pm_deadviewheight(			"pm_deadviewheight",		"10",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "height of player's view while dead" );
budCVar pm_crouchrate(				"pm_crouchrate",			"0.87",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "time it takes for player's view to change from standing to crouching" );
budCVar pm_bboxwidth(				"pm_bboxwidth",				"32",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "x/y size of player's bounding box" );
budCVar pm_crouchbob(				"pm_crouchbob",				"0.5",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "bob much faster when crouched" );
budCVar pm_walkbob(					"pm_walkbob",				"0.3",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "bob slowly when walking" );
budCVar pm_runbob(					"pm_runbob",				"0.4",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "bob faster when running" );
budCVar pm_runpitch(					"pm_runpitch",				"0.002",		CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "" );
budCVar pm_runroll(					"pm_runroll",				"0.005",		CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "" );
budCVar pm_bobup(					"pm_bobup",					"0.005",		CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "" );
budCVar pm_bobpitch(					"pm_bobpitch",				"0.002",		CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "" );
budCVar pm_bobroll(					"pm_bobroll",				"0.002",		CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "" );
budCVar pm_thirdPersonRange(			"pm_thirdPersonRange",		"80",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "camera distance from player in 3rd person" );
budCVar pm_thirdPersonHeight(		"pm_thirdPersonHeight",		"0",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "height of camera from normal view height in 3rd person" );
budCVar pm_thirdPersonAngle(			"pm_thirdPersonAngle",		"0",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_FLOAT, "direction of camera from player in 3rd person in degrees (0 = behind player, 180 = in front)" );
budCVar pm_thirdPersonClip(			"pm_thirdPersonClip",		"1",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_BOOL, "clip third person view into world space" );
budCVar pm_thirdPerson(				"pm_thirdPerson",			"0",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_BOOL, "enables third person view" );
budCVar pm_thirdPersonDeath(			"pm_thirdPersonDeath",		"0",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_BOOL, "enables third person view when player dies" );
budCVar pm_modelView(				"pm_modelView",				"0",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_INTEGER, "draws camera from POV of player model (1 = always, 2 = when dead)", 0, 2, budCmdSystem::ArgCompletion_Integer<0,2> );
budCVar pm_airMsec(					"pm_air",					"30000",		CVAR_GAME | CVAR_NETWORKSYNC | CVAR_INTEGER, "how long in milliseconds the player can go without air before he starts taking damage" );

budCVar g_showPlayerShadow(			"g_showPlayerShadow",		"0",			CVAR_GAME | CVAR_ARCHIVE | CVAR_BOOL, "enables shadow of player model" );
budCVar g_showHud(					"g_showHud",				"1",			CVAR_GAME | CVAR_ARCHIVE | CVAR_BOOL, "" );
budCVar g_showProjectilePct(			"g_showProjectilePct",		"0",			CVAR_GAME | CVAR_ARCHIVE | CVAR_BOOL, "enables display of player hit percentage" );
budCVar g_showBrass(					"g_showBrass",				"1",			CVAR_GAME | CVAR_ARCHIVE | CVAR_BOOL, "enables ejected shells from weapon" );
budCVar g_gun_x(						"g_gunX",					"3",			CVAR_GAME | CVAR_ARCHIVE | CVAR_FLOAT, "" );
budCVar g_gun_y(						"g_gunY",					"0",			CVAR_GAME | CVAR_ARCHIVE | CVAR_FLOAT, "" );
budCVar g_gun_z(						"g_gunZ",					"0",			CVAR_GAME | CVAR_ARCHIVE | CVAR_FLOAT, "" );
budCVar g_gunScale(					"g_gunScale",				"1",			CVAR_GAME | CVAR_ARCHIVE | CVAR_FLOAT, "" );
budCVar g_viewNodalX(				"g_viewNodalX",				"3",			CVAR_GAME | CVAR_FLOAT, "" );
budCVar g_viewNodalZ(				"g_viewNodalZ",				"6",			CVAR_GAME | CVAR_FLOAT, "" );
// RB: fixed missing CVAR_ARCHIVE
budCVar g_fov(						"g_fov",					"80",			CVAR_GAME | CVAR_ARCHIVE | CVAR_INTEGER | CVAR_NOCHEAT, "" );
// RB end
budCVar g_skipViewEffects(			"g_skipViewEffects",		"0",			CVAR_GAME | CVAR_BOOL, "skip damage and other view effects" );
budCVar g_mpWeaponAngleScale(		"g_mpWeaponAngleScale",		"0",			CVAR_GAME | CVAR_FLOAT, "Control the weapon sway in MP" );

budCVar g_testParticle(				"g_testParticle",			"0",			CVAR_GAME | CVAR_INTEGER, "test particle visualation, set by the particle editor" );
budCVar g_testParticleName(			"g_testParticleName",		"",				CVAR_GAME, "name of the particle being tested by the particle editor" );
budCVar g_testModelRotate(			"g_testModelRotate",		"0",			CVAR_GAME, "test model rotation speed" );
budCVar g_testPostProcess(			"g_testPostProcess",		"",				CVAR_GAME, "name of material to draw over screen" );
budCVar g_testModelAnimate(			"g_testModelAnimate",		"0",			CVAR_GAME | CVAR_INTEGER, "test model animation,\n"
																							"0 = cycle anim with origin reset\n"
																							"1 = cycle anim with fixed origin\n"
																							"2 = cycle anim with continuous origin\n"
																							"3 = frame by frame with continuous origin\n"
																							"4 = play anim once", 0, 4, budCmdSystem::ArgCompletion_Integer<0,4> );
budCVar g_testModelBlend(			"g_testModelBlend",			"0",			CVAR_GAME | CVAR_INTEGER, "number of frames to blend" );
budCVar g_testDeath(					"g_testDeath",				"0",			CVAR_GAME | CVAR_BOOL, "" );
budCVar g_flushSave(					"g_flushSave",				"0",			CVAR_GAME | CVAR_BOOL, "1 = don't buffer file writing for save games." );

budCVar aas_test(					"aas_test",					"0",			CVAR_GAME | CVAR_INTEGER, "" );
budCVar aas_showAreas(				"aas_showAreas",			"0",			CVAR_GAME | CVAR_BOOL, "" );
budCVar aas_showPath(				"aas_showPath",				"0",			CVAR_GAME | CVAR_INTEGER, "" );
budCVar aas_showFlyPath(				"aas_showFlyPath",			"0",			CVAR_GAME | CVAR_INTEGER, "" );
budCVar aas_showWallEdges(			"aas_showWallEdges",		"0",			CVAR_GAME | CVAR_BOOL, "" );
budCVar aas_showHideArea(			"aas_showHideArea",			"0",			CVAR_GAME | CVAR_INTEGER, "" );
budCVar aas_pullPlayer(				"aas_pullPlayer",			"0",			CVAR_GAME | CVAR_INTEGER, "" );
budCVar aas_randomPullPlayer(		"aas_randomPullPlayer",		"0",			CVAR_GAME | CVAR_BOOL, "" );
budCVar aas_goalArea(				"aas_goalArea",				"0",			CVAR_GAME | CVAR_INTEGER, "" );
budCVar aas_showPushIntoArea(		"aas_showPushIntoArea",		"0",			CVAR_GAME | CVAR_BOOL, "" );

budCVar g_countDown(					"g_countDown",				"15",			CVAR_GAME | CVAR_INTEGER | CVAR_ARCHIVE, "pregame countdown in seconds", 4, 3600 );
budCVar g_gameReviewPause(			"g_gameReviewPause",		"10",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_INTEGER | CVAR_ARCHIVE, "scores review time in seconds (at end game)", 2, 3600 );
budCVar g_CTFArrows(					"g_CTFArrows",				"1",			CVAR_GAME | CVAR_NETWORKSYNC | CVAR_BOOL, "draw arrows over teammates in CTF" );

budCVar net_clientPredictGUI(		"net_clientPredictGUI",		"1",			CVAR_GAME | CVAR_BOOL, "test guis in networking without prediction" );

budCVar g_grabberHoldSeconds(		"g_grabberHoldSeconds",		"3",			CVAR_GAME | CVAR_FLOAT | CVAR_CHEAT, "number of seconds to hold object" );
budCVar g_grabberEnableShake(		"g_grabberEnableShake",		"1",			CVAR_GAME | CVAR_BOOL | CVAR_CHEAT, "enable the grabber shake" );
budCVar g_grabberRandomMotion(		"g_grabberRandomMotion",	"1",			CVAR_GAME | CVAR_BOOL | CVAR_CHEAT, "enable random motion on the grabbed object" );
budCVar g_grabberHardStop(			"g_grabberHardStop",		"1",			CVAR_GAME | CVAR_BOOL | CVAR_CHEAT, "hard stops object if too fast" );
budCVar g_grabberDamping(			"g_grabberDamping",			"0.5",			CVAR_GAME | CVAR_FLOAT | CVAR_CHEAT, "damping of grabber" );

budCVar g_xp_bind_run_once( "g_xp_bind_run_once", "0", CVAR_GAME | CVAR_BOOL | CVAR_ARCHIVE, "Rebind all controls once for D3XP." );
