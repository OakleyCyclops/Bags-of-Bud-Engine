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
#ifndef __SWF_SHAPEPARSER_H__
#define __SWF_SHAPEPARSER_H__

/*
================================================
This class handles parsing and triangulating a shape
================================================
*/
class budSWFShapeParser
{
public:
	budSWFShapeParser() { }
	void Parse( budSWFBitStream& bitstream, budSWFShape& shape, int recordType );
	void ParseMorph( budSWFBitStream& bitstream, budSWFShape& shape );
	void ParseFont( budSWFBitStream& bitstream, budSWFFontGlyph& shape );
	
private:
	bool extendedCount;
	bool rgba;
	bool morph;
	bool lineStyle2;
	
	struct swfSPEdge_t
	{
		uint16 v0;
		uint16 v1;
		uint16 cp;	// control point if this is a curve, 0xFFFF otherwise
	};
	struct swfSPMorphEdge_t
	{
		swfSPEdge_t start;
		swfSPEdge_t end;
	};
	struct swfSPLineLoop_t
	{
		bool hole;
		budList< uint16, TAG_SWF > vindex1;
		budList< uint16, TAG_SWF > vindex2;
	};
	struct swfSPDrawFill_t
	{
		swfFillStyle_t style;
		budList< swfSPMorphEdge_t, TAG_SWF > edges;
		budList< swfSPLineLoop_t, TAG_SWF > loops;
	};
	struct swfSPDrawLine_t
	{
		swfLineStyle_t style;
		budList< swfSPMorphEdge_t, TAG_SWF > edges;
	};
	budList< budVec2, TAG_SWF > verts;
	budList< swfSPDrawFill_t, TAG_SWF > fillDraws;
	budList< swfSPDrawLine_t, TAG_SWF > lineDraws;
	
	
private:
	void ParseShapes( budSWFBitStream& bitstream1, budSWFBitStream* bitstream2, bool swap );
	void ReadFillStyle( budSWFBitStream& bitstream );
	void ParseEdge( budSWFBitStream& bitstream, int32& penX, int32& penY, swfSPEdge_t& edge );
	void MakeLoops();
	void TriangulateSoup( budSWFShape& shape );
	void TriangulateSoup( budSWFFontGlyph& shape );
	int FindEarVert( const swfSPLineLoop_t& loop );
	void AddUniqueVert( budSWFShapeDrawFill& drawFill, const budVec2& start, const budVec2& end );
	
};

#endif // !__SWF_SHAPEPARSER_H__
