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

#ifndef __SAVEGAME_H__
#define __SAVEGAME_H__

/*

Save game related helper classes.

*/

class idSaveGame
{
public:
	idSaveGame( budFile* savefile, budFile* stringFile, int inVersion );
	~idSaveGame();
	
	void					Close();
	
	void					WriteDecls();
	
	void					AddObject( const idClass* obj );
	void					Resize( const int count )
	{
		objects.Resize( count );
	}
	void					WriteObjectList();
	
	void					Write( const void* buffer, int len );
	void					WriteInt( const int value );
	void					WriteJoint( const jointHandle_t value );
	void					WriteShort( const short value );
	void					WriteByte( const byte value );
	void					WriteSignedChar( const signed char value );
	void					WriteFloat( const float value );
	void					WriteBool( const bool value );
	void					WriteString( const char* string );
	void					WriteVec2( const Vector2& vec );
	void					WriteVec3( const Vector3& vec );
	void					WriteVec4( const Vector4& vec );
	void					WriteVec6( const Vector6& vec );
	void					WriteWinding( const idWinding& winding );
	void					WriteBounds( const budBounds& bounds );
	void					WriteMat3( const Matrix3& mat );
	void					WriteAngles( const Angles& angles );
	void					WriteObject( const idClass* obj );
	void					WriteStaticObject( const idClass& obj );
	void					WriteDict( const Dict* dict );
	void					WriteMaterial( const budMaterial* material );
	void					WriteSkin( const budDeclSkin* skin );
	void					WriteParticle( const budDeclParticle* particle );
	void					WriteFX( const budDeclFX* fx );
	void					WriteSoundShader( const idSoundShader* shader );
	void					WriteModelDef( const class budDeclModelDef* modelDef );
	void					WriteModel( const budRenderModel* model );
	void					WriteUserInterface( const budUserInterface* ui, bool unique );
	void					WriteRenderEntity( const renderEntity_t& renderEntity );
	void					WriteRenderLight( const renderLight_t& renderLight );
	void					WriteRefSound( const refSound_t& refSound );
	void					WriteRenderView( const renderView_t& view );
	void					WriteUsercmd( const usercmd_t& usercmd );
	void					WriteContactInfo( const contactInfo_t& contactInfo );
	void					WriteTrace( const trace_t& trace );
	void					WriteTraceModel( const budTraceModel& trace );
	void					WriteClipModel( const class budClipModel* clipModel );
	void					WriteSoundCommands();
	
	void					WriteBuildNumber( const int value );
	
	int						GetBuildNumber() const
	{
		return version;
	}
	
	int						GetCurrentSaveSize() const
	{
		return file->Length();
	}
	
private:
	budFile* 				file;
	budFile* 				stringFile;
	budCompressor* 			compressor;
	
	List<const idClass*>	objects;
	int						version;
	
	void					CallSave_r( const idTypeInfo* cls, const idClass* obj );
	
	struct stringTableIndex_s
	{
		String		string;
		int			offset;
	};
	
	budHashIndex						stringHash;
	List< stringTableIndex_s >	stringTable;
	int								curStringTableOffset;
	
};

class idRestoreGame
{
public:
	idRestoreGame( budFile* savefile, budFile* stringTableFile, int saveVersion );
	~idRestoreGame();
	
	void					ReadDecls();
	
	void					CreateObjects();
	void					RestoreObjects();
	void					DeleteObjects();
	
	void					Error( VERIFY_FORMAT_STRING const char* fmt, ... );
	
	void					Read( void* buffer, int len );
	void					ReadInt( int& value );
	void					ReadJoint( jointHandle_t& value );
	void					ReadShort( short& value );
	void					ReadByte( byte& value );
	void					ReadSignedChar( signed char& value );
	void					ReadFloat( float& value );
	void					ReadBool( bool& value );
	void					ReadString( String& string );
	void					ReadVec2( Vector2& vec );
	void					ReadVec3( Vector3& vec );
	void					ReadVec4( Vector4& vec );
	void					ReadVec6( Vector6& vec );
	void					ReadWinding( idWinding& winding );
	void					ReadBounds( budBounds& bounds );
	void					ReadMat3( Matrix3& mat );
	void					ReadAngles( Angles& angles );
	void					ReadObject( idClass*& obj );
	void					ReadStaticObject( idClass& obj );
	void					ReadDict( Dict* dict );
	void					ReadMaterial( const budMaterial*& material );
	void					ReadSkin( const budDeclSkin*& skin );
	void					ReadParticle( const budDeclParticle*& particle );
	void					ReadFX( const budDeclFX*& fx );
	void					ReadSoundShader( const idSoundShader*& shader );
	void					ReadModelDef( const budDeclModelDef*& modelDef );
	void					ReadModel( budRenderModel*& model );
	void					ReadUserInterface( budUserInterface*& ui );
	void					ReadRenderEntity( renderEntity_t& renderEntity );
	void					ReadRenderLight( renderLight_t& renderLight );
	void					ReadRefSound( refSound_t& refSound );
	void					ReadRenderView( renderView_t& view );
	void					ReadUsercmd( usercmd_t& usercmd );
	void					ReadContactInfo( contactInfo_t& contactInfo );
	void					ReadTrace( trace_t& trace );
	void					ReadTraceModel( budTraceModel& trace );
	void					ReadClipModel( budClipModel*& clipModel );
	void					ReadSoundCommands();
	
	//						Used to retrieve the saved game buildNumber from within class Restore methods
	int						GetBuildNumber() const
	{
		return version;
	}
	
private:
	budFile* 		file;
	budFile* 		stringFile;
	List<idClass*, TAG_SAVEGAMES>		objects;
	int						version;
	int						stringTableOffset;
	
	void					CallRestore_r( const idTypeInfo* cls, idClass* obj );
};

#endif /* !__SAVEGAME_H__*/
