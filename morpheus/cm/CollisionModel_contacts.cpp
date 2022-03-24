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

/*
===============================================================================

	Trace model vs. polygonal model collision detection.

===============================================================================
*/

#pragma hdrstop
#include "PCH.hpp"


#include "CollisionModel_local.h"

/*
===============================================================================

Retrieving contacts

===============================================================================
*/

/*
==================
budCollisionModelManagerLocal::Contacts
==================
*/
int budCollisionModelManagerLocal::Contacts( contactInfo_t* contacts, const int maxContacts, const budVec3& start, const budVec6& dir, const float depth,
		const budTraceModel* trm, const budMat3& trmAxis, int contentMask,
		cmHandle_t model, const budVec3& origin, const budMat3& modelAxis )
{
	trace_t results;
	budVec3 end;
	
	// same as Translation but instead of storing the first collision we store all collisions as contacts
	budCollisionModelManagerLocal::getContacts = true;
	budCollisionModelManagerLocal::contacts = contacts;
	budCollisionModelManagerLocal::maxContacts = maxContacts;
	budCollisionModelManagerLocal::numContacts = 0;
	end = start + dir.SubVec3( 0 ) * depth;
	budCollisionModelManagerLocal::Translation( &results, start, end, trm, trmAxis, contentMask, model, origin, modelAxis );
	if( dir.SubVec3( 1 ).LengthSqr() != 0.0f )
	{
		// FIXME: rotational contacts
	}
	budCollisionModelManagerLocal::getContacts = false;
	budCollisionModelManagerLocal::maxContacts = 0;
	
	return budCollisionModelManagerLocal::numContacts;
}
