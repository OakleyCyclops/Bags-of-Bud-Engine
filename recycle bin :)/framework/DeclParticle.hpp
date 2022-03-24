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

#ifndef __DECLPARTICLE_H__
#define __DECLPARTICLE_H__

/*
===============================================================================

	budDeclParticle

===============================================================================
*/

static const int MAX_PARTICLE_STAGES	= 32;

class idParticleParm
{
public:
	idParticleParm()
	{
		table = NULL;
		from = to = 0.0f;
	}
	
	const budDeclTable* 		table;
	float					from;
	float					to;
	
	float					Eval( float frac, idRandom& rand ) const;
	float					Integrate( float frac, idRandom& rand ) const;
};


typedef enum
{
	PDIST_RECT,				// ( sizeX sizeY sizeZ )
	PDIST_CYLINDER,			// ( sizeX sizeY sizeZ )
	PDIST_SPHERE			// ( sizeX sizeY sizeZ ringFraction )
	// a ringFraction of zero allows the entire sphere, 0.9 would only
	// allow the outer 10% of the sphere
} prtDistribution_t;

typedef enum
{
	PDIR_CONE,				// parm0 is the solid cone angle
	PDIR_OUTWARD			// direction is relative to offset from origin, parm0 is an upward bias
} prtDirection_t;

typedef enum
{
	PPATH_STANDARD,
	PPATH_HELIX,			// ( sizeX sizeY sizeZ radialSpeed climbSpeed )
	PPATH_FLIES,
	PPATH_ORBIT,
	PPATH_DRIP
} prtCustomPth_t;

typedef enum
{
	POR_VIEW,
	POR_AIMED,				// angle and aspect are disregarded
	POR_X,
	POR_Y,
	POR_Z
} prtOrientation_t;

typedef struct renderEntity_s renderEntity_t;
typedef struct renderView_s renderView_t;

typedef struct
{
	const renderEntity_t* 	renderEnt;			// for shaderParms, etc
	const renderView_t* 	renderView;
	int						index;				// particle number in the system
	float					frac;				// 0.0 to 1.0
	idRandom				random;
	budVec3					origin;				// dynamic smoke particles can have individual origins and axis
	budMat3					axis;
	
	
	float					age;				// in seconds, calculated as fraction * stage->particleLife
	idRandom				originalRandom;		// needed so aimed particles can reset the random for another origin calculation
	float					animationFrameFrac;	// set by ParticleTexCoords, used to make the cross faded version
} particleGen_t;


//
// single particle stage
//
class idParticleStage
{
public:
	idParticleStage();
	~idParticleStage() {}
	
	void					Default();
	int						NumQuadsPerParticle() const;	// includes trails and cross faded animations
	// returns the number of verts created, which will range from 0 to 4*NumQuadsPerParticle()
	int						CreateParticle( particleGen_t* g, budDrawVert* verts ) const;
	
	void					ParticleOrigin( particleGen_t* g, budVec3& origin ) const;
	int						ParticleVerts( particleGen_t* g, const budVec3 origin, budDrawVert* verts ) const;
	void					ParticleTexCoords( particleGen_t* g, budDrawVert* verts ) const;
	void					ParticleColors( particleGen_t* g, budDrawVert* verts ) const;
	
	const char* 			GetCustomPathName();
	const char* 			GetCustomPathDesc();
	int						NumCustomPathParms();
	void					SetCustomPathType( const char* p );
	void					operator=( const idParticleStage& src );
	
	
	//------------------------------
	
	const budMaterial* 		material;
	
	int						totalParticles;		// total number of particles, although some may be invisible at a given time
	float					cycles;				// allows things to oneShot ( 1 cycle ) or run for a set number of cycles
	// on a per stage basis
	
	int						cycleMsec;			// ( particleLife + deadTime ) in msec
	
	float					spawnBunching;		// 0.0 = all come out at first instant, 1.0 = evenly spaced over cycle time
	float					particleLife;		// total seconds of life for each particle
	float					timeOffset;			// time offset from system start for the first particle to spawn
	float					deadTime;			// time after particleLife before respawning
	
	//-------------------------------	// standard path parms
	
	prtDistribution_t		distributionType;
	float					distributionParms[4];
	
	prtDirection_t			directionType;
	float					directionParms[4];
	
	idParticleParm			speed;
	float					gravity;				// can be negative to float up
	bool					worldGravity;			// apply gravity in world space
	bool					randomDistribution;		// randomly orient the quad on emission ( defaults to true )
	bool					entityColor;			// force color from render entity ( fadeColor is still valid )
	
	//------------------------------	// custom path will completely replace the standard path calculations
	
	prtCustomPth_t			customPathType;		// use custom C code routines for determining the origin
	float					customPathParms[8];
	
	//--------------------------------
	
	budVec3					offset;				// offset from origin to spawn all particles, also applies to customPath
	
	int						animationFrames;	// if > 1, subdivide the texture S axis into frames and crossfade
	float					animationRate;		// frames per second
	
	float					initialAngle;		// in degrees, random angle is used if zero ( default )
	idParticleParm			rotationSpeed;		// half the particles will have negative rotation speeds
	
	prtOrientation_t		orientation;	// view, aimed, or axis fixed
	float					orientationParms[4];
	
	idParticleParm			size;
	idParticleParm			aspect;				// greater than 1 makes the T axis longer
	
	budVec4					color;
	budVec4					fadeColor;			// either 0 0 0 0 for additive, or 1 1 1 0 for blended materials
	float					fadeInFraction;		// in 0.0 to 1.0 range
	float					fadeOutFraction;	// in 0.0 to 1.0 range
	float					fadeIndexFraction;	// in 0.0 to 1.0 range, causes later index smokes to be more faded
	
	bool					hidden;				// for editor use
	//-----------------------------------
	
	float					boundsExpansion;	// user tweak to fix poorly calculated bounds
	
	budBounds				bounds;				// derived
};


//
// group of particle stages
//
class budDeclParticle : public budDecl
{
public:

	virtual size_t			Size() const;
	virtual const char* 	DefaultDefinition() const;
	virtual bool			Parse( const char* text, const int textLength, bool allowBinaryVersion );
	virtual void			FreeData();
	
	bool					Save( const char* fileName = NULL );
	
	// Loaded instead of re-parsing, written if MD5 hash different
	bool					LoadBinary( budFile* file, unsigned int checksum );
	void					WriteBinary( budFile* file, unsigned int checksum );
	
	budList<idParticleStage*, TAG_libBud_LIST_DECL>stages;
	budBounds				bounds;
	float					depthHack;
	
private:
	bool					RebuildTextSource();
	void					GetStageBounds( idParticleStage* stage );
	idParticleStage* 		ParseParticleStage( budLexer& src );
	void					ParseParms( budLexer& src, float* parms, int maxParms );
	void					ParseParametric( budLexer& src, idParticleParm* parm );
	void					WriteStage( budFile* f, idParticleStage* stage );
	void					WriteParticleParm( budFile* f, idParticleParm* parm, const char* name );
};

#endif /* !__DECLPARTICLE_H__ */
