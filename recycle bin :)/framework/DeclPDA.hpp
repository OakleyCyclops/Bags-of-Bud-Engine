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

#ifndef __DECLPDA_H__
#define __DECLPDA_H__

/*
===============================================================================

	budDeclPDA

===============================================================================
*/


class budDeclEmail : public budDecl
{
public:
	budDeclEmail() {}
	
	virtual size_t			Size() const;
	virtual const char* 	DefaultDefinition() const;
	virtual bool			Parse( const char* text, const int textLength, bool allowBinaryVersion );
	virtual void			FreeData();
	virtual void			Print() const;
	virtual void			List() const;
	
	const char* 			GetFrom() const
	{
		return from;
	}
	const char* 			GetBody() const
	{
		return text;
	}
	const char* 			GetSubject() const
	{
		return subject;
	}
	const char* 			GetDate() const
	{
		return date;
	}
	const char* 			GetTo() const
	{
		return to;
	}
	
private:
	budStr					text;
	budStr					subject;
	budStr					date;
	budStr					to;
	budStr					from;
};


class budDeclVideo : public budDecl
{
public:
	budDeclVideo() : preview( NULL ), video( NULL ), audio( NULL ) {};
	
	virtual size_t			Size() const;
	virtual const char* 	DefaultDefinition() const;
	virtual bool			Parse( const char* text, const int textLength, bool allowBinaryVersion );
	virtual void			FreeData();
	virtual void			Print() const;
	virtual void			List() const;
	
	const budMaterial* 		GetRoq() const
	{
		return video;
	}
	const idSoundShader* 	GetWave() const
	{
		return audio;
	}
	const char* 			GetVideoName() const
	{
		return videoName;
	}
	const char* 			GetInfo() const
	{
		return info;
	}
	const budMaterial* 		GetPreview() const
	{
		return preview;
	}
	
private:
	const budMaterial* 		preview;
	const budMaterial* 		video;
	budStr					videoName;
	budStr					info;
	const idSoundShader* 	audio;
};


class budDeclAudio : public budDecl
{
public:
	budDeclAudio() : audio( NULL ) {};
	
	virtual size_t			Size() const;
	virtual const char* 	DefaultDefinition() const;
	virtual bool			Parse( const char* text, const int textLength, bool allowBinaryVersion );
	virtual void			FreeData();
	virtual void			Print() const;
	virtual void			List() const;
	
	const char* 			GetAudioName() const
	{
		return audioName;
	}
	const idSoundShader* 	GetWave() const
	{
		return audio;
	}
	const char* 			GetInfo() const
	{
		return info;
	}
	
private:
	const idSoundShader* 	audio;
	budStr					audioName;
	budStr					info;
};

class budDeclPDA : public budDecl
{
public:
	budDeclPDA()
	{
		originalEmails = originalVideos = 0;
	};
	
	virtual size_t			Size() const;
	virtual const char* 	DefaultDefinition() const;
	virtual bool			Parse( const char* text, const int textLength, bool allowBinaryVersion );
	virtual void			FreeData();
	virtual void			Print() const;
	virtual void			List() const;
	
	virtual void			AddVideo( const budDeclVideo* video, bool unique = true ) const
	{
		if( unique )
		{
			videos.AddUnique( video );
		}
		else
		{
			videos.Append( video );
		}
	}
	virtual void			AddAudio( const budDeclAudio* audio, bool unique = true ) const
	{
		if( unique )
		{
			audios.AddUnique( audio );
		}
		else
		{
			audios.Append( audio );
		}
	}
	virtual void			AddEmail( const budDeclEmail* email, bool unique = true ) const
	{
		if( unique )
		{
			emails.AddUnique( email );
		}
		else
		{
			emails.Append( email );
		}
	}
	virtual void			RemoveAddedEmailsAndVideos() const;
	
	virtual const int		GetNumVideos() const
	{
		return videos.Num();
	}
	virtual const int		GetNumAudios() const
	{
		return audios.Num();
	}
	virtual const int		GetNumEmails() const
	{
		return emails.Num();
	}
	virtual const budDeclVideo* GetVideoByIndex( int index ) const
	{
		return ( index < 0 || index > videos.Num() ? NULL : videos[index] );
	}
	virtual const budDeclAudio* GetAudioByIndex( int index ) const
	{
		return ( index < 0 || index > audios.Num() ? NULL : audios[index] );
	}
	virtual const budDeclEmail* GetEmailByIndex( int index ) const
	{
		return ( index < 0 || index > emails.Num() ? NULL : emails[index] );
	}
	
	virtual void			SetSecurity( const char* sec ) const;
	
	const char* 			GetPdaName() const
	{
		return pdaName;
	}
	const char* 			GetSecurity() const
	{
		return security;
	}
	const char* 			GetFullName() const
	{
		return fullName;
	}
	const char* 			GetIcon() const
	{
		return icon;
	}
	const char* 			GetPost() const
	{
		return post;
	}
	const char* 			GetID() const
	{
		return id;
	}
	const char* 			GetTitle() const
	{
		return title;
	}
	
private:
	mutable budList<const budDeclVideo*>	videos;
	mutable budList<const budDeclAudio*>	audios;
	mutable budList<const budDeclEmail*>	emails;
	budStr					pdaName;
	budStr					fullName;
	budStr					icon;
	budStr					id;
	budStr					post;
	budStr					title;
	mutable budStr			security;
	mutable	int				originalEmails;
	mutable int				originalVideos;
};

#endif /* !__DECLPDA_H__ */
