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

#ifndef __SYS_CVAR_H__
#define __SYS_CVAR_H__

extern CVar	developer;

extern CVar	g_cinematic;
extern CVar	g_cinematicMaxSkipTime;

extern CVar	g_monsters;
extern CVar	g_decals;
extern CVar	g_knockback;
extern CVar	g_skill;
extern CVar	g_gravity;
extern CVar	g_skipFX;
extern CVar	g_bloodEffects;
extern CVar	g_projectileLights;
extern CVar	g_muzzleFlash;

extern CVar	g_disasm;
extern CVar	g_debugBounds;
extern CVar	g_debugAnim;
extern CVar	g_debugMove;
extern CVar	g_debugDamage;
extern CVar	g_debugWeapon;
extern CVar	g_debugScript;
extern CVar	g_debugMover;
extern CVar	g_debugTriggers;
extern CVar	g_debugCinematic;
extern CVar	g_stopTime;
extern CVar	g_armorProtection;
extern CVar	g_armorProtectionMP;
extern CVar	g_damageScale;
extern CVar	g_useDynamicProtection;
extern CVar	g_healthTakeTime;
extern CVar	g_healthTakeAmt;
extern CVar	g_healthTakeLimit;

extern CVar	g_showPVS;
extern CVar	g_showTargets;
extern CVar	g_showTriggers;
extern CVar	g_showCollisionWorld;
extern CVar	g_showCollisionModels;
extern CVar	g_showCollisionTraces;
extern CVar	g_maxShowDistance;
extern CVar	g_showEntityInfo;
extern CVar	g_showviewpos;
extern CVar	g_showcamerainfo;
extern CVar	g_showTestModelFrame;
extern CVar	g_showActiveEntities;
extern CVar	g_showEnemies;

extern CVar	g_frametime;
extern CVar	g_timeentities;

extern CVar	ai_debugScript;
extern CVar	ai_debugMove;
extern CVar	ai_debugTrajectory;
extern CVar	ai_testPredictPath;
extern CVar	ai_showCombatNodes;
extern CVar	ai_showPaths;
extern CVar	ai_showObstacleAvoidance;
extern CVar	ai_blockedFailSafe;
extern CVar	ai_showHealth;

extern CVar	g_dvTime;
extern CVar	g_dvAmplitude;
extern CVar	g_dvFrequency;

extern CVar	g_kickTime;
extern CVar	g_kickAmplitude;
extern CVar	g_blobTime;
extern CVar	g_blobSize;

extern CVar	g_testHealthVision;
extern CVar	g_editEntityMode;
extern CVar	g_dragEntity;
extern CVar	g_dragDamping;
extern CVar	g_dragShowSelection;
extern CVar	g_dropItemRotation;

extern CVar	g_vehicleVelocity;
extern CVar	g_vehicleForce;
extern CVar	g_vehicleSuspensionUp;
extern CVar	g_vehicleSuspensionDown;
extern CVar	g_vehicleSuspensionKCompress;
extern CVar	g_vehicleSuspensionDamping;
extern CVar	g_vehicleTireFriction;
extern CVar	g_vehicleDebug;
extern CVar	g_debugShockwave;
extern CVar	g_enablePortalSky;

extern CVar	ik_enable;
extern CVar	ik_debug;

extern CVar	af_useLinearTime;
extern CVar	af_useImpulseFriction;
extern CVar	af_useJointImpulseFriction;
extern CVar	af_useSymmetry;
extern CVar	af_skipSelfCollision;
extern CVar	af_skipLimits;
extern CVar	af_skipFriction;
extern CVar	af_forceFriction;
extern CVar	af_maxLinearVelocity;
extern CVar	af_maxAngularVelocity;
extern CVar	af_timeScale;
extern CVar	af_jointFrictionScale;
extern CVar	af_contactFrictionScale;
extern CVar	af_highlightBody;
extern CVar	af_highlightConstraint;
extern CVar	af_showTimings;
extern CVar	af_showConstraints;
extern CVar	af_showConstraintNames;
extern CVar	af_showConstrainedBodies;
extern CVar	af_showPrimaryOnly;
extern CVar	af_showTrees;
extern CVar	af_showLimits;
extern CVar	af_showBodies;
extern CVar	af_showBodyNames;
extern CVar	af_showMass;
extern CVar	af_showTotalMass;
extern CVar	af_showInertia;
extern CVar	af_showVelocity;
extern CVar	af_showActive;
extern CVar	af_testSolid;

extern CVar	rb_showTimings;
extern CVar	rb_showBodies;
extern CVar	rb_showMass;
extern CVar	rb_showInertia;
extern CVar	rb_showVelocity;
extern CVar	rb_showActive;

extern CVar	pm_jumpheight;
extern CVar	pm_stepsize;
extern CVar	pm_crouchspeed;
extern CVar	pm_walkspeed;
extern CVar	pm_runspeed;
extern CVar	pm_noclipspeed;
extern CVar	pm_spectatespeed;
extern CVar	pm_spectatebbox;
extern CVar	pm_usecylinder;
extern CVar	pm_minviewpitch;
extern CVar	pm_maxviewpitch;
extern CVar	pm_stamina;
extern CVar	pm_staminathreshold;
extern CVar	pm_staminarate;
extern CVar	pm_crouchheight;
extern CVar	pm_crouchviewheight;
extern CVar	pm_normalheight;
extern CVar	pm_normalviewheight;
extern CVar	pm_deadheight;
extern CVar	pm_deadviewheight;
extern CVar	pm_crouchrate;
extern CVar	pm_bboxwidth;
extern CVar	pm_crouchbob;
extern CVar	pm_walkbob;
extern CVar	pm_runbob;
extern CVar	pm_runpitch;
extern CVar	pm_runroll;
extern CVar	pm_bobup;
extern CVar	pm_bobpitch;
extern CVar	pm_bobroll;
extern CVar	pm_thirdPersonRange;
extern CVar	pm_thirdPersonHeight;
extern CVar	pm_thirdPersonAngle;
extern CVar	pm_thirdPersonClip;
extern CVar	pm_thirdPerson;
extern CVar	pm_thirdPersonDeath;
extern CVar	pm_modelView;
extern CVar	pm_airMsec;

extern CVar	g_showPlayerShadow;
extern CVar	g_showHud;
extern CVar	g_showProjectilePct;
extern CVar	g_showBrass;
extern CVar	g_gun_x;
extern CVar	g_gun_y;
extern CVar	g_gun_z;
extern CVar	g_gunScale;
extern CVar	g_viewNodalX;
extern CVar	g_viewNodalZ;
extern CVar	g_fov;
extern CVar	g_testDeath;
extern CVar	g_skipViewEffects;
extern CVar   g_mpWeaponAngleScale;

extern CVar	g_testParticle;
extern CVar	g_testParticleName;

extern CVar	g_testPostProcess;

extern CVar	g_testModelRotate;
extern CVar	g_testModelAnimate;
extern CVar	g_testModelBlend;
extern CVar	g_flushSave;

extern CVar	g_enableSlowmo;
extern CVar	g_slowmoStepRate;
extern CVar	g_testFullscreenFX;
extern CVar	g_testHelltimeFX;
extern CVar	g_testMultiplayerFX;
extern CVar	g_moveableDamageScale;
extern CVar	g_testBloomIntensity;
extern CVar	g_testBloomNumPasses;

extern CVar	g_grabberHoldSeconds;
extern CVar	g_grabberEnableShake;
extern CVar	g_grabberRandomMotion;
extern CVar	g_grabberHardStop;
extern CVar	g_grabberDamping;

extern CVar	g_xp_bind_run_once;

extern CVar	aas_test;
extern CVar	aas_showAreas;
extern CVar	aas_showPath;
extern CVar	aas_showFlyPath;
extern CVar	aas_showWallEdges;
extern CVar	aas_showHideArea;
extern CVar	aas_pullPlayer;
extern CVar	aas_randomPullPlayer;
extern CVar	aas_goalArea;
extern CVar	aas_showPushIntoArea;

extern CVar	net_clientPredictGUI;

extern CVar	si_timeLimit;
extern CVar	si_fragLimit;
extern CVar	si_spectators;

extern CVar si_flagDropTimeLimit;
extern CVar si_midnight;

extern CVar g_flagAttachJoint;
extern CVar g_flagAttachOffsetX;
extern CVar g_flagAttachOffsetY;
extern CVar g_flagAttachOffsetZ;
extern CVar g_flagAttachAngleX;
extern CVar g_flagAttachAngleY;
extern CVar g_flagAttachAngleZ;

extern CVar g_CTFArrows;

extern CVar	net_clientSelfSmoothing;

#endif /* !__SYS_CVAR_H__ */
