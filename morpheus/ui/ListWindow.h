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
#ifndef __LISTWINDOW_H
#define __LISTWINDOW_H

class idSliderWindow;

enum
{
	TAB_TYPE_TEXT = 0,
	TAB_TYPE_ICON = 1
};

struct idTabRect
{
	int x;
	int w;
	int align;
	int valign;
	int	type;
	budVec2 iconSize;
	float iconVOffset;
};

class budListWindow : public idWindow
{
public:
	budListWindow( budUserInterfaceLocal* gui );
	
	virtual const char*	HandleEvent( const sysEvent_t* event, bool* updateVisuals );
	virtual void		PostParse();
	virtual void		Draw( int time, float x, float y );
	virtual void		Activate( bool activate, budStr& act );
	virtual void		HandleBuddyUpdate( idWindow* buddy );
	virtual void		StateChanged( bool redraw = false );
	virtual size_t		Allocated()
	{
		return idWindow::Allocated();
	};
	virtual idWinVar*	GetWinVarByName( const char* _name, bool winLookup = false, drawWin_t** owner = NULL );
	
	void				UpdateList();
	
private:
	virtual bool		ParseInternalVar( const char* name, budTokenParser* src );
	void				CommonInit();
	void				InitScroller( bool horizontal );
	void				SetCurrentSel( int sel );
	void				AddCurrentSel( int sel );
	int					GetCurrentSel();
	bool				IsSelected( int index );
	void				ClearSelection( int sel );
	
	budList<idTabRect, TAG_OLD_UI>	tabInfo;
	int					top;
	float				sizeBias;
	bool				horizontal;
	budStr				tabStopStr;
	budStr				tabAlignStr;
	budStr				tabVAlignStr;
	budStr				tabTypeStr;
	budStr				tabIconSizeStr;
	budStr				tabIconVOffsetStr;
	budHashTable<const budMaterial*> iconMaterials;
	bool				multipleSel;
	
	budStrList			listItems;
	idSliderWindow*		scroller;
	budList<int, TAG_OLD_UI>			currentSel;
	budStr				listName;
	
	int					clickTime;
	
	int					typedTime;
	budStr				typed;
};

#endif // __LISTWINDOW_H
