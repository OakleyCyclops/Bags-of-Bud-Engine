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

extern budCVar	developer;

extern budCVar	g_cinematic;
extern budCVar	g_cinematicMaxSkipTime;

extern budCVar	g_monsters;
extern budCVar	g_decals;
extern budCVar	g_knockback;
extern budCVar	g_skill;
extern budCVar	g_gravity;
extern budCVar	g_skipFX;
extern budCVar	g_bloodEffects;
extern budCVar	g_projectileLights;
extern budCVar	g_muzzleFlash;

extern budCVar	g_disasm;
extern budCVar	g_debugBounds;
extern budCVar	g_debugAnim;
extern budCVar	g_debugMove;
extern budCVar	g_debugDamage;
extern budCVar	g_debugWeapon;
extern budCVar	g_debugScript;
extern budCVar	g_debugMover;
extern budCVar	g_debugTriggers;
extern budCVar	g_debugCinematic;
extern budCVar	g_stopTime;
extern budCVar	g_armorProtection;
extern budCVar	g_armorProtectionMP;
extern budCVar	g_damageScale;
extern budCVar	g_useDynamicProtection;
extern budCVar	g_healthTakeTime;
extern budCVar	g_healthTakeAmt;
extern budCVar	g_healthTakeLimit;

extern budCVar	g_showPVS;
extern budCVar	g_showTargets;
extern budCVar	g_showTriggers;
extern budCVar	g_showCollisionWorld;
extern budCVar	g_showCollisionModels;
extern budCVar	g_showCollisionTraces;
extern budCVar	g_maxShowDistance;
extern budCVar	g_showEntityInfo;
extern budCVar	g_showviewpos;
extern budCVar	g_showcamerainfo;
extern budCVar	g_showTestModelFrame;
extern budCVar	g_showActiveEntities;
extern budCVar	g_showEnemies;

extern budCVar	g_frametime;
extern budCVar	g_timeentities;

extern budCVar	ai_debugScript;
extern budCVar	ai_debugMove;
extern budCVar	ai_debugTrajectory;
extern budCVar	ai_testPredictPath;
extern budCVar	ai_showCombatNodes;
extern budCVar	ai_showPaths;
extern budCVar	ai_showObstacleAvoidance;
extern budCVar	ai_blockedFailSafe;
extern budCVar	ai_showHealth;

extern budCVar	g_dvTime;
extern budCVar	g_dvAmplitude;
extern budCVar	g_dvFrequency;

extern budCVar	g_kickTime;
extern budCVar	g_kickAmplitude;
extern budCVar	g_blobTime;
extern budCVar	g_blobSize;

extern budCVar	g_testHealthVision;
extern budCVar	g_editEntityMode;
extern budCVar	g_dragEntity;
extern budCVar	g_dragDamping;
extern budCVar	g_dragShowSelection;
extern budCVar	g_dropItemRotation;

extern budCVar	g_vehicleVelocity;
extern budCVar	g_vehicleForce;
extern budCVar	g_vehicleSuspensionUp;
extern budCVar	g_vehicleSuspensionDown;
extern budCVar	g_vehicleSuspensionKCompress;
extern budCVar	g_vehicleSuspensionDamping;
extern budCVar	g_vehicleTireFriction;
extern budCVar	g_vehicleDebug;
extern budCVar	g_debugShockwave;
extern budCVar	g_enablePortalSky;

extern budCVar	ik_enable;
extern budCVar	ik_debug;

extern budCVar	af_useLinearTime;
extern budCVar	af_useImpulseFriction;
extern budCVar	af_useJointImpulseFriction;
extern budCVar	af_useSymmetry;
extern budCVar	af_skipSelfCollision;
extern budCVar	af_skipLimits;
extern budCVar	af_skipFriction;
extern budCVar	af_forceFriction;
extern budCVar	af_maxLinearVelocity;
extern budCVar	af_maxAngularVelocity;
extern budCVar	af_timeScale;
extern budCVar	af_jointFrictionScale;
extern budCVar	af_contactFrictionScale;
extern budCVar	af_highlightBody;
extern budCVar	af_highlightConstraint;
extern budCVar	af_showTimings;
extern budCVar	af_showConstraints;
extern budCVar	af_showConstraintNames;
extern budCVar	af_showConstrainedBodies;
extern budCVar	af_showPrimaryOnly;
extern budCVar	af_showTrees;
extern budCVar	af_showLimits;
extern budCVar	af_showBodies;
extern budCVar	af_showBodyNames;
extern budCVar	af_showMass;
extern budCVar	af_showTotalMass;
extern budCVar	af_showInertia;
extern budCVar	af_showVelocity;
extern budCVar	af_showActive;
extern budCVar	af_testSolid;

extern budCVar	rb_showTimings;
extern budCVar	rb_showBodies;
extern budCVar	rb_showMass;
extern budCVar	rb_showInertia;
extern budCVar	rb_showVelocity;
extern budCVar	rb_showActive;

extern budCVar	pm_jumpheight;
extern budCVar	pm_stepsize;
extern budCVar	pm_crouchspeed;
extern budCVar	pm_walkspeed;
extern budCVar	pm_runspeed;
extern budCVar	pm_noclipspeed;
extern budCVar	pm_spectatespeed;
extern budCVar	pm_spectatebbox;
extern budCVar	pm_usecylinder;
extern budCVar	pm_minviewpitch;
extern budCVar	pm_maxviewpitch;
extern budCVar	pm_stamina;
extern budCVar	pm_staminathreshold;
extern budCVar	pm_staminarate;
extern budCVar	pm_crouchheight;
extern budCVar	pm_crouchviewheight;
extern budCVar	pm_normalheight;
extern budCVar	pm_normalviewheight;
extern budCVar	pm_deadheight;
extern budCVar	pm_deadviewheight;
extern budCVar	pm_crouchrate;
extern budCVar	pm_bboxwidth;
extern budCVar	pm_crouchbob;
extern budCVar	pm_walkbob;
extern budCVar	pm_runbob;
extern budCVar	pm_runpitch;
extern budCVar	pm_runroll;
extern budCVar	pm_bobup;
extern budCVar	pm_bobpitch;
extern budCVar	pm_bobroll;
extern budCVar	pm_thirdPersonRange;
extern budCVar	pm_thirdPersonHeight;
extern budCVar	pm_thirdPersonAngle;
extern budCVar	pm_thirdPersonClip;
extern budCVar	pm_thirdPerson;
extern budCVar	pm_thirdPersonDeath;
extern budCVar	pm_modelView;
extern budCVar	pm_airMsec;

extern budCVar	g_showPlayerShadow;
extern budCVar	g_showHud;
extern budCVar	g_showProjectilePct;
extern budCVar	g_showBrass;
extern budCVar	g_gun_x;
extern budCVar	g_gun_y;
extern budCVar	g_gun_z;
extern budCVar	g_gunScale;
extern budCVar	g_viewNodalX;
extern budCVar	g_viewNodalZ;
extern budCVar	g_fov;
extern budCVar	g_testDeath;
extern budCVar	g_skipViewEffects;
extern budCVar   g_mpWeaponAngleScale;

extern budCVar	g_testParticle;
extern budCVar	g_testParticleName;

extern budCVar	g_testPostProcess;

extern budCVar	g_testModelRotate;
extern budCVar	g_testModelAnimate;
extern budCVar	g_testModelBlend;
extern budCVar	g_flushSave;

extern budCVar	g_enableSlowmo;
extern budCVar	g_slowmoStepRate;
extern budCVar	g_testFullscreenFX;
extern budCVar	g_testHelltimeFX;
extern budCVar	g_testMultiplayerFX;
extern budCVar	g_moveableDamageScale;
extern budCVar	g_testBloomIntensity;
extern budCVar	g_testBloomNumPasses;

extern budCVar	g_grabberHoldSeconds;
extern budCVar	g_grabberEnableShake;
extern budCVar	g_grabberRandomMotion;
extern budCVar	g_grabberHardStop;
extern budCVar	g_grabberDamping;

extern budCVar	g_xp_bind_run_once;

extern budCVar	aas_test;
extern budCVar	aas_showAreas;
extern budCVar	aas_showPath;
extern budCVar	aas_showFlyPath;
extern budCVar	aas_showWallEdges;
extern budCVar	aas_showHideArea;
extern budCVar	aas_pullPlayer;
extern budCVar	aas_randomPullPlayer;
extern budCVar	aas_goalArea;
extern budCVar	aas_showPushIntoArea;

extern budCVar	net_clientPredictGUI;

extern budCVar	si_timeLimit;
extern budCVar	si_fragLimit;
extern budCVar	si_spectators;

extern budCVar si_flagDropTimeLimit;
extern budCVar si_midnight;

extern budCVar g_flagAttachJoint;
extern budCVar g_flagAttachOffsetX;
extern budCVar g_flagAttachOffsetY;
extern budCVar g_flagAttachOffsetZ;
extern budCVar g_flagAttachAngleX;
extern budCVar g_flagAttachAngleY;
extern budCVar g_flagAttachAngleZ;

extern budCVar g_CTFArrows;

extern budCVar	net_clientSelfSmoothing;

#endif /* !__SYS_CVAR_H__ */
