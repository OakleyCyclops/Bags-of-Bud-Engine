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

#ifndef __MATH_SIMD_H__
#define __MATH_SIMD_H__

/*
===============================================================================

	Single Instruction Multiple Data (SIMD)

	For optimal use data should be aligned on a 16 byte boundary.
	All idSIMDProcessor routines are thread safe.

===============================================================================
*/

class idSIMD
{
public:
	static void			Init();
	static void			InitProcessor( const char* module, bool forceGeneric );
	static void			Shutdown();
	static void			Test_f( const class budCmdArgs& args );
};


/*
===============================================================================

	virtual base class for different SIMD processors

===============================================================================
*/

// RB begin
#ifdef _WIN32
#define VPCALL __fastcall
#else
#define VPCALL
#endif
// RB end

class budVec2;
class budVec3;
class budVec4;
class budVec5;
class budVec6;
class budVecX;
class budMat2;
class budMat3;
class budMat4;
class budMat5;
class budMat6;
class budMatX;
class budPlane;
class budDrawVert;
class budJointQuat;
class budJointMat;
struct dominantTri_t;

class idSIMDProcessor
{
public:
	idSIMDProcessor()
	{
		cpuid = CPUID_NONE;
	}
	
	cpuid_t							cpuid;
	
	virtual const char* VPCALL		GetName() const = 0;
	
	virtual	void VPCALL MinMax( float& min,			float& max,				const float* src,		const int count ) = 0;
	virtual	void VPCALL MinMax( budVec2& min,		budVec2& max,			const budVec2* src,		const int count ) = 0;
	virtual	void VPCALL MinMax( budVec3& min,		budVec3& max,			const budVec3* src,		const int count ) = 0;
	virtual	void VPCALL MinMax( budVec3& min,		budVec3& max,			const budDrawVert* src,	const int count ) = 0;
	virtual	void VPCALL MinMax( budVec3& min,		budVec3& max,			const budDrawVert* src,	const triIndex_t* indexes,		const int count ) = 0;
	
	virtual void VPCALL Memcpy( void* dst,			const void* src,		const int count ) = 0;
	virtual void VPCALL Memset( void* dst,			const int val,			const int count ) = 0;
	
	// animation
	virtual void VPCALL BlendJoints( budJointQuat* joints, const budJointQuat* blendJoints, const float lerp, const int* index, const int numJoints ) = 0;
	virtual void VPCALL BlendJointsFast( budJointQuat* joints, const budJointQuat* blendJoints, const float lerp, const int* index, const int numJoints ) = 0;
	virtual void VPCALL ConvertJointQuatsToJointMats( budJointMat* jointMats, const budJointQuat* jointQuats, const int numJoints ) = 0;
	virtual void VPCALL ConvertJointMatsToJointQuats( budJointQuat* jointQuats, const budJointMat* jointMats, const int numJoints ) = 0;
	virtual void VPCALL TransformJoints( budJointMat* jointMats, const int* parents, const int firstJoint, const int lastJoint ) = 0;
	virtual void VPCALL UntransformJoints( budJointMat* jointMats, const int* parents, const int firstJoint, const int lastJoint ) = 0;
};

// pointer to SIMD processor
extern idSIMDProcessor* SIMDProcessor;

#endif /* !__MATH_SIMD_H__ */
