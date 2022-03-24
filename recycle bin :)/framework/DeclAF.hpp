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

#ifndef __DECLAF_H__
#define __DECLAF_H__

/*
===============================================================================

	Articulated Figure

===============================================================================
*/

class budDeclAF;

typedef enum
{
	DECLAF_CONSTRAINT_INVALID,
	DECLAF_CONSTRAINT_FIXED,
	DECLAF_CONSTRAINT_BALLANDSOCKETJOINT,
	DECLAF_CONSTRAINT_UNIVERSALJOINT,
	DECLAF_CONSTRAINT_HINGE,
	DECLAF_CONSTRAINT_SLIDER,
	DECLAF_CONSTRAINT_SPRING
} declAFConstraintType_t;

typedef enum
{
	DECLAF_JOINTMOD_AXIS,
	DECLAF_JOINTMOD_ORIGIN,
	DECLAF_JOINTMOD_BOTH
} declAFJointMod_t;

typedef bool ( *getJointTransform_t )( void* model, const budJointMat* frame, const char* jointName, budVec3& origin, budMat3& axis );

class budAFVector
{
public:
	enum
	{
		VEC_COORDS = 0,
		VEC_JOINT,
		VEC_BONECENTER,
		VEC_BONEDIR
	}						type;
	budStr					joint1;
	budStr					joint2;
	
public:
	budAFVector();
	
	bool					Parse( budLexer& src );
	bool					Finish( const char* fileName, const getJointTransform_t GetJointTransform, const budJointMat* frame, void* model ) const;
	bool					Write( budFile* f ) const;
	const char* 			ToString( budStr& str, const int precision = 8 );
	const budVec3& 			ToVec3() const
	{
		return vec;
	}
	budVec3& 				ToVec3()
	{
		return vec;
	}
	
private:
	mutable budVec3			vec;
	bool					negate;
};

class budDeclAF_Body
{
public:
	budStr					name;
	budStr					jointName;
	declAFJointMod_t		jointMod;
	int						modelType;
	budAFVector				v1, v2;
	int						numSides;
	float					width;
	float					density;
	budAFVector				origin;
	budAngles				angles;
	int						contents;
	int						clipMask;
	bool					selfCollision;
	budMat3					inertiaScale;
	float					linearFriction;
	float					angularFriction;
	float					contactFriction;
	budStr					containedJoints;
	budAFVector				frictionDirection;
	budAFVector				contactMotorDirection;
public:
	void					SetDefault( const budDeclAF* file );
};

class budDeclAF_Constraint
{
public:
	budStr					name;
	budStr					body1;
	budStr					body2;
	declAFConstraintType_t	type;
	float					friction;
	float					stretch;
	float					compress;
	float					damping;
	float					restLength;
	float					minLength;
	float					maxLength;
	budAFVector				anchor;
	budAFVector				anchor2;
	budAFVector				shaft[2];
	budAFVector				axis;
	enum
	{
		LIMIT_NONE = -1,
		LIMIT_CONE,
		LIMIT_PYRAMID
	}						limit;
	budAFVector				limitAxis;
	float					limitAngles[3];
	
public:
	void					SetDefault( const budDeclAF* file );
};

class budDeclAF : public budDecl
{
	friend class budAFFileManager;
public:
	budDeclAF();
	virtual					~budDeclAF();
	
	virtual size_t			Size() const;
	virtual const char* 	DefaultDefinition() const;
	virtual bool			Parse( const char* text, const int textLength, bool allowBinaryVersion );
	virtual void			FreeData();
	
	virtual void			Finish( const getJointTransform_t GetJointTransform, const budJointMat* frame, void* model ) const;
	
	bool					Save();
	
	void					NewBody( const char* name );
	void					RenameBody( const char* oldName, const char* newName );
	void					DeleteBody( const char* name );
	
	void					NewConstraint( const char* name );
	void					RenameConstraint( const char* oldName, const char* newName );
	void					DeleteConstraint( const char* name );
	
	static int				ContentsFromString( const char* str );
	static const char* 		ContentsToString( const int contents, budStr& str );
	
	static declAFJointMod_t	JointModFromString( const char* str );
	static const char* 		JointModToString( declAFJointMod_t jointMod );
	
public:
	bool					modified;
	budStr					model;
	budStr					skin;
	float					defaultLinearFriction;
	float					defaultAngularFriction;
	float					defaultContactFriction;
	float					defaultConstraintFriction;
	float					totalMass;
	budVec2					suspendVelocity;
	budVec2					suspendAcceleration;
	float					noMoveTime;
	float					noMoveTranslation;
	float					noMoveRotation;
	float					minMoveTime;
	float					maxMoveTime;
	int						contents;
	int						clipMask;
	bool					selfCollision;
	budList<budDeclAF_Body*, TAG_libBud_LIST_PHYSICS>			bodies;
	budList<budDeclAF_Constraint*, TAG_libBud_LIST_PHYSICS>	constraints;
	
private:
	bool					ParseContents( budLexer& src, int& c ) const;
	bool					ParseBody( budLexer& src );
	bool					ParseFixed( budLexer& src );
	bool					ParseBallAndSocketJoint( budLexer& src );
	bool					ParseUniversalJoint( budLexer& src );
	bool					ParseHinge( budLexer& src );
	bool					ParseSlider( budLexer& src );
	bool					ParseSpring( budLexer& src );
	bool					ParseSettings( budLexer& src );
	
	bool					WriteBody( budFile* f, const budDeclAF_Body& body ) const;
	bool					WriteFixed( budFile* f, const budDeclAF_Constraint& c ) const;
	bool					WriteBallAndSocketJoint( budFile* f, const budDeclAF_Constraint& c ) const;
	bool					WriteUniversalJoint( budFile* f, const budDeclAF_Constraint& c ) const;
	bool					WriteHinge( budFile* f, const budDeclAF_Constraint& c ) const;
	bool					WriteSlider( budFile* f, const budDeclAF_Constraint& c ) const;
	bool					WriteSpring( budFile* f, const budDeclAF_Constraint& c ) const;
	bool					WriteConstraint( budFile* f, const budDeclAF_Constraint& c ) const;
	bool					WriteSettings( budFile* f ) const;
	
	bool					RebuildTextSource();
};

#endif /* !__DECLAF_H__ */
