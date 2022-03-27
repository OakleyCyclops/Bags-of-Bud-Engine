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

#pragma hdrstop
#include "PCH.hpp"

#include "Model_local.h"
#include "RenderCommon.h"	// just for R_FreeWorldInteractions and R_CreateWorldInteractions

budCVar binaryLoadRenderModels( "binaryLoadRenderModels", "1", 0, "enable binary load/write of render models" );
budCVar preload_MapModels( "preload_MapModels", "1", CVAR_SYSTEM | CVAR_BOOL, "preload models during begin or end levelload" );

// RB begin
budCVar postLoadExportModels( "postLoadExportModels", "0", CVAR_BOOL | CVAR_RENDERER, "export models after loading to OBJ model format" );
// RB end

class budRenderModelManagerLocal : public budRenderModelManager
{
public:
	budRenderModelManagerLocal();
	virtual					~budRenderModelManagerLocal() {}
	
	virtual void			Init();
	virtual void			Shutdown();
	virtual budRenderModel* 	AllocModel();
	virtual void			FreeModel( budRenderModel* model );
	virtual budRenderModel* 	FindModel( const char* modelName );
	virtual budRenderModel* 	CheckModel( const char* modelName );
	virtual budRenderModel* 	DefaultModel();
	virtual void			AddModel( budRenderModel* model );
	virtual void			RemoveModel( budRenderModel* model );
	virtual void			ReloadModels( bool forceAll = false );
	virtual void			FreeModelVertexCaches();
	virtual void			WritePrecacheCommands( budFile* file );
	virtual void			BeginLevelLoad();
	virtual void			EndLevelLoad();
	virtual void			Preload( const idPreloadManifest& manifest );
	
	virtual	void			PrintMemInfo( MemInfo_t* mi );
	
private:
	budList<budRenderModel*, TAG_MODEL>	models;
	budHashIndex				hash;
	budRenderModel* 			defaultModel;
	budRenderModel* 			beamModel;
	budRenderModel* 			spriteModel;
	bool					insideLevelLoad;		// don't actually load now
	
	budRenderModel* 			GetModel( const char* modelName, bool createIfNotFound );
	
	static void				PrintModel_f( const budCmdArgs& args );
	static void				ListModels_f( const budCmdArgs& args );
	static void				ReloadModels_f( const budCmdArgs& args );
	static void				TouchModel_f( const budCmdArgs& args );
};


budRenderModelManagerLocal	localModelManager;
budRenderModelManager* 		renderModelManager = &localModelManager;

/*
==============
budRenderModelManagerLocal::budRenderModelManagerLocal
==============
*/
budRenderModelManagerLocal::budRenderModelManagerLocal()
{
	defaultModel = NULL;
	beamModel = NULL;
	spriteModel = NULL;
	insideLevelLoad = false;
}

/*
==============
budRenderModelManagerLocal::PrintModel_f
==============
*/
void budRenderModelManagerLocal::PrintModel_f( const budCmdArgs& args )
{
	budRenderModel*	model;
	
	if( args.Argc() != 2 )
	{
		common->Printf( "usage: printModel <modelName>\n" );
		return;
	}
	
	model = renderModelManager->CheckModel( args.Argv( 1 ) );
	if( !model )
	{
		common->Printf( "model \"%s\" not found\n", args.Argv( 1 ) );
		return;
	}
	
	model->Print();
}

/*
==============
budRenderModelManagerLocal::ListModels_f
==============
*/
void budRenderModelManagerLocal::ListModels_f( const budCmdArgs& args )
{
	int		totalMem = 0;
	int		inUse = 0;
	
	common->Printf( " mem   srf verts tris\n" );
	common->Printf( " ---   --- ----- ----\n" );
	
	for( int i = 0; i < localModelManager.models.Num(); i++ )
	{
		budRenderModel*	model = localModelManager.models[i];
		
		if( !model->IsLoaded() )
		{
			continue;
		}
		model->List();
		totalMem += model->Memory();
		inUse++;
	}
	
	common->Printf( " ---   --- ----- ----\n" );
	common->Printf( " mem   srf verts tris\n" );
	
	common->Printf( "%i loaded models\n", inUse );
	common->Printf( "total memory: %4.1fM\n", ( float )totalMem / ( 1024 * 1024 ) );
}

/*
==============
budRenderModelManagerLocal::ReloadModels_f
==============
*/
void budRenderModelManagerLocal::ReloadModels_f( const budCmdArgs& args )
{
	if( budStr::Icmp( args.Argv( 1 ), "all" ) == 0 )
	{
		localModelManager.ReloadModels( true );
	}
	else
	{
		localModelManager.ReloadModels( false );
	}
}

/*
==============
budRenderModelManagerLocal::TouchModel_f

Precache a specific model
==============
*/
void budRenderModelManagerLocal::TouchModel_f( const budCmdArgs& args )
{
	const char*	model = args.Argv( 1 );
	
	if( !model[0] )
	{
		common->Printf( "usage: touchModel <modelName>\n" );
		return;
	}
	
	common->Printf( "touchModel %s\n", model );
	const bool captureToImage = false;
	budRenderModel* m = renderModelManager->CheckModel( model );
	if( !m )
	{
		common->Printf( "...not found\n" );
	}
}

/*
=================
budRenderModelManagerLocal::WritePrecacheCommands
=================
*/
void budRenderModelManagerLocal::WritePrecacheCommands( budFile* f )
{
	for( int i = 0; i < models.Num(); i++ )
	{
		budRenderModel*	model = models[i];
		
		if( !model )
		{
			continue;
		}
		if( !model->IsReloadable() )
		{
			continue;
		}
		
		char	str[1024];
		sprintf( str, "touchModel %s\n", model->Name() );
		common->Printf( "%s", str );
		f->Printf( "%s", str );
	}
}

/*
=================
budRenderModelManagerLocal::Init
=================
*/
void budRenderModelManagerLocal::Init()
{
	cmdSystem->AddCommand( "listModels", ListModels_f, CMD_FL_RENDERER, "lists all models" );
	cmdSystem->AddCommand( "printModel", PrintModel_f, CMD_FL_RENDERER, "prints model info", budCmdSystem::ArgCompletion_ModelName );
	cmdSystem->AddCommand( "reloadModels", ReloadModels_f, CMD_FL_RENDERER | CMD_FL_CHEAT, "reloads models" );
	cmdSystem->AddCommand( "touchModel", TouchModel_f, CMD_FL_RENDERER, "touches a model", budCmdSystem::ArgCompletion_ModelName );
	
	insideLevelLoad = false;
	
	// create a default model
	budRenderModelStatic* model = new( TAG_MODEL ) budRenderModelStatic;
	model->InitEmpty( "_DEFAULT" );
	model->MakeDefaultModel();
	model->SetLevelLoadReferenced( true );
	defaultModel = model;
	AddModel( model );
	
	// create the beam model
	budRenderModelStatic* beam = new( TAG_MODEL ) budRenderModelBeam;
	beam->InitEmpty( "_BEAM" );
	beam->SetLevelLoadReferenced( true );
	beamModel = beam;
	AddModel( beam );
	
	budRenderModelStatic* sprite = new( TAG_MODEL ) budRenderModelSprite;
	sprite->InitEmpty( "_SPRITE" );
	sprite->SetLevelLoadReferenced( true );
	spriteModel = sprite;
	AddModel( sprite );
}

/*
=================
budRenderModelManagerLocal::Shutdown
=================
*/
void budRenderModelManagerLocal::Shutdown()
{
	models.DeleteContents( true );
	hash.Free();
}

/*
=================
budRenderModelManagerLocal::GetModel
=================
*/
budRenderModel* budRenderModelManagerLocal::GetModel( const char* _modelName, bool createIfNotFound )
{

	if( !_modelName || !_modelName[0] )
	{
		return NULL;
	}
	
	budStrStatic< MAX_OSPATH > canonical = _modelName;
	canonical.ToLower();
	
	budStrStatic< MAX_OSPATH > extension;
	canonical.ExtractFileExtension( extension );
	
	// see if it is already present
	int key = hash.GenerateKey( canonical, false );
	for( int i = hash.First( key ); i != -1; i = hash.Next( i ) )
	{
		budRenderModel* model = models[i];
		
		if( canonical.Icmp( model->Name() ) == 0 )
		{
			if( !model->IsLoaded() )
			{
				// reload it if it was purged
				budStr generatedFileName = "generated/rendermodels/";
				generatedFileName.AppendPath( canonical );
				generatedFileName.SetFileExtension( va( "b%s", extension.c_str() ) );
				
				// Get the timestamp on the original file, if it's newer than what is stored in binary model, regenerate it
				ID_TIME_T sourceTimeStamp = fileSystem->GetTimestamp( canonical );
				
				if( model->SupportsBinaryModel() && binaryLoadRenderModels.GetBool() )
				{
					budFileLocal file( fileSystem->OpenFileReadMemory( generatedFileName ) );
					model->PurgeModel();
					if( !model->LoadBinaryModel( file, sourceTimeStamp ) )
					{
						model->LoadModel();
					}
				}
				else
				{
					model->LoadModel();
				}
			}
			else if( insideLevelLoad && !model->IsLevelLoadReferenced() )
			{
				// we are reusing a model already in memory, but
				// touch all the materials to make sure they stay
				// in memory as well
				model->TouchData();
			}
			model->SetLevelLoadReferenced( true );
			return model;
		}
	}
	
	// see if we can load it
	
	// determine which subclass of budRenderModel to initialize
	
	budRenderModel* model = NULL;
	
	// RB: added dae
	if( ( extension.Icmp( "dae" ) == 0 ) || ( extension.Icmp( "ase" ) == 0 ) || ( extension.Icmp( "lwo" ) == 0 ) || ( extension.Icmp( "flt" ) == 0 ) || ( extension.Icmp( "ma" ) == 0 ) )
	{
		model = new( TAG_MODEL ) budRenderModelStatic;
	}
	else if( extension.Icmp( MD5_MESH_EXT ) == 0 )
	{
		model = new( TAG_MODEL ) budRenderModelMD5;
	}
	else if( extension.Icmp( "md3" ) == 0 )
	{
		model = new( TAG_MODEL ) budRenderModelMD3;
	}
	else if( extension.Icmp( "prt" ) == 0 )
	{
		model = new( TAG_MODEL ) budRenderModelPrt;
	}
	else if( extension.Icmp( "liquid" ) == 0 )
	{
		model = new( TAG_MODEL ) budRenderModelLiquid;
	}
	
	budStrStatic< MAX_OSPATH > generatedFileName;
	
	if( model != NULL )
	{
	
		generatedFileName = "generated/rendermodels/";
		generatedFileName.AppendPath( canonical );
		generatedFileName.SetFileExtension( va( "b%s", extension.c_str() ) );
		
		// Get the timestamp on the original file, if it's newer than what is stored in binary model, regenerate it
		ID_TIME_T sourceTimeStamp = fileSystem->GetTimestamp( canonical );
		
		budFileLocal file( fileSystem->OpenFileReadMemory( generatedFileName ) );
		
		if( !model->SupportsBinaryModel() || !binaryLoadRenderModels.GetBool() )
		{
			model->InitFromFile( canonical );
		}
		else
		{
			if( !model->LoadBinaryModel( file, sourceTimeStamp ) )
			{
				model->InitFromFile( canonical );
				
				// RB: default models shouldn't be cached as binary models
				if( !model->IsDefaultModel() )
				{
					budFileLocal outputFile( fileSystem->OpenFileWrite( generatedFileName, "fs_basepath" ) );
					libBud::Printf( "Writing %s\n", generatedFileName.c_str() );
					model->WriteBinaryModel( outputFile );
				}
				// RB end
			} /* else {
				libBud::Printf( "loaded binary model %s from file %s\n", model->Name(), generatedFileName.c_str() );
			} */
		}
	}
	
	// Not one of the known formats
	if( model == NULL )
	{
	
		if( extension.Length() )
		{
			common->Warning( "unknown model type '%s'", canonical.c_str() );
		}
		
		if( !createIfNotFound )
		{
			return NULL;
		}
		
		budRenderModelStatic*	smodel = new( TAG_MODEL ) budRenderModelStatic;
		smodel->InitEmpty( canonical );
		smodel->MakeDefaultModel();
		
		model = smodel;
	}
	
	if( cvarSystem->GetCVarBool( "fs_buildresources" ) )
	{
		fileSystem->AddModelPreload( canonical );
	}
	model->SetLevelLoadReferenced( true );
	
	if( !createIfNotFound && model->IsDefaultModel() )
	{
		delete model;
		model = NULL;
		
		return NULL;
	}
	
	if( cvarSystem->GetCVarBool( "fs_buildgame" ) )
	{
		fileSystem->AddModelPreload( model->Name() );
	}
	
	// RB begin
	if( postLoadExportModels.GetBool() && ( model != defaultModel && model != beamModel && model != spriteModel ) )
	{
		budStrStatic< MAX_OSPATH > exportedFileName;
		
		exportedFileName = "exported/rendermodels/";
		exportedFileName.AppendPath( canonical );
		exportedFileName.SetFileExtension( ".obj" );
		
		ID_TIME_T sourceTimeStamp = fileSystem->GetTimestamp( canonical );
		ID_TIME_T timeStamp = fileSystem->GetTimestamp( exportedFileName );
		
		// TODO only update if generated has changed
		
		//if( timeStamp == FILE_NOT_FOUND_TIMESTAMP )
		{
			budFileLocal objFile( fileSystem->OpenFileWrite( exportedFileName, "fs_basepath" ) );
			libBud::Printf( "Writing %s\n", exportedFileName.c_str() );
			
			exportedFileName.SetFileExtension( ".mtl" );
			budFileLocal mtlFile( fileSystem->OpenFileWrite( exportedFileName, "fs_basepath" ) );
			
			model->ExportOBJ( objFile, mtlFile );
		}
	}
	// RB end
	
	AddModel( model );
	
	return model;
}

/*
=================
budRenderModelManagerLocal::AllocModel
=================
*/
budRenderModel* budRenderModelManagerLocal::AllocModel()
{
	return new( TAG_MODEL ) budRenderModelStatic();
}

/*
=================
budRenderModelManagerLocal::FreeModel
=================
*/
void budRenderModelManagerLocal::FreeModel( budRenderModel* model )
{
	if( !model )
	{
		return;
	}
	if( !dynamic_cast<budRenderModelStatic*>( model ) )
	{
		common->Error( "budRenderModelManager::FreeModel: model '%s' is not a static model", model->Name() );
		return;
	}
	if( model == defaultModel )
	{
		common->Error( "budRenderModelManager::FreeModel: can't free the default model" );
		return;
	}
	if( model == beamModel )
	{
		common->Error( "budRenderModelManager::FreeModel: can't free the beam model" );
		return;
	}
	if( model == spriteModel )
	{
		common->Error( "budRenderModelManager::FreeModel: can't free the sprite model" );
		return;
	}
	
	R_CheckForEntityDefsUsingModel( model );
	
	delete model;
}

/*
=================
budRenderModelManagerLocal::FindModel
=================
*/
budRenderModel* budRenderModelManagerLocal::FindModel( const char* modelName )
{
	return GetModel( modelName, true );
}

/*
=================
budRenderModelManagerLocal::CheckModel
=================
*/
budRenderModel* budRenderModelManagerLocal::CheckModel( const char* modelName )
{
	return GetModel( modelName, false );
}

/*
=================
budRenderModelManagerLocal::DefaultModel
=================
*/
budRenderModel* budRenderModelManagerLocal::DefaultModel()
{
	return defaultModel;
}

/*
=================
budRenderModelManagerLocal::AddModel
=================
*/
void budRenderModelManagerLocal::AddModel( budRenderModel* model )
{
	hash.Add( hash.GenerateKey( model->Name(), false ), models.Append( model ) );
}

/*
=================
budRenderModelManagerLocal::RemoveModel
=================
*/
void budRenderModelManagerLocal::RemoveModel( budRenderModel* model )
{
	int index = models.FindIndex( model );
	if( index != -1 )
	{
		hash.RemoveIndex( hash.GenerateKey( model->Name(), false ), index );
		models.RemoveIndex( index );
	}
}

/*
=================
budRenderModelManagerLocal::ReloadModels
=================
*/
void budRenderModelManagerLocal::ReloadModels( bool forceAll )
{
	if( forceAll )
	{
		common->Printf( "Reloading all model files...\n" );
	}
	else
	{
		common->Printf( "Checking for changed model files...\n" );
	}
	
	R_FreeDerivedData();
	
	// skip the default model at index 0
	for( int i = 1; i < models.Num(); i++ )
	{
		budRenderModel*	model = models[i];
		
		// we may want to allow world model reloading in the future, but we don't now
		if( !model->IsReloadable() )
		{
			continue;
		}
		
		if( !forceAll )
		{
			// check timestamp
			ID_TIME_T current;
			
			fileSystem->ReadFile( model->Name(), NULL, &current );
			if( current <= model->Timestamp() )
			{
				continue;
			}
		}
		
		common->DPrintf( "reloading %s.\n", model->Name() );
		
		model->LoadModel();
	}
	
	// we must force the world to regenerate, because models may
	// have changed size, making their references invalid
	R_ReCreateWorldReferences();
}

/*
=================
budRenderModelManagerLocal::FreeModelVertexCaches
=================
*/
void budRenderModelManagerLocal::FreeModelVertexCaches()
{
	for( int i = 0; i < models.Num(); i++ )
	{
		budRenderModel* model = models[i];
		model->FreeVertexCache();
	}
}

/*
=================
budRenderModelManagerLocal::BeginLevelLoad
=================
*/
void budRenderModelManagerLocal::BeginLevelLoad()
{
	insideLevelLoad = true;
	
	for( int i = 0; i < models.Num(); i++ )
	{
		budRenderModel* model = models[i];
		
		// always reload all models
		if( model->IsReloadable() )
		{
			R_CheckForEntityDefsUsingModel( model );
			model->PurgeModel();
		}
		
		model->SetLevelLoadReferenced( false );
	}
	
	vertexCache.FreeStaticData();
}

/*
=================
budRenderModelManagerLocal::Preload
=================
*/
void budRenderModelManagerLocal::Preload( const idPreloadManifest& manifest )
{
	if( preload_MapModels.GetBool() )
	{
		// preload this levels images
		int	start = Sys_Milliseconds();
		int numLoaded = 0;
		budList< preloadSort_t > preloadSort;
		preloadSort.Resize( manifest.NumResources() );
		for( int i = 0; i < manifest.NumResources(); i++ )
		{
			const preloadEntry_s& p = manifest.GetPreloadByIndex( i );
			idResourceCacheEntry rc;
			budStrStatic< MAX_OSPATH > filename;
			if( p.resType == PRELOAD_MODEL )
			{
				filename = "generated/rendermodels/";
				filename += p.resourceName;
				budStrStatic< 16 > ext;
				filename.ExtractFileExtension( ext );
				filename.SetFileExtension( va( "b%s", ext.c_str() ) );
			}
			if( p.resType == PRELOAD_PARTICLE )
			{
				filename = "generated/particles/";
				filename += p.resourceName;
				filename += ".bprt";
			}
			if( !filename.IsEmpty() )
			{
				if( fileSystem->GetResourceCacheEntry( filename, rc ) )
				{
					preloadSort_t ps = {};
					ps.idx = i;
					ps.ofs = rc.offset;
					preloadSort.Append( ps );
				}
			}
		}
		
		preloadSort.SortWithTemplate( budSort_Preload() );
		
		for( int i = 0; i < preloadSort.Num(); i++ )
		{
			const preloadSort_t& ps = preloadSort[ i ];
			const preloadEntry_s& p = manifest.GetPreloadByIndex( ps.idx );
			if( p.resType == PRELOAD_MODEL )
			{
				budRenderModel* model = FindModel( p.resourceName );
				if( model != NULL )
				{
					model->SetLevelLoadReferenced( true );
				}
			}
			else if( p.resType == PRELOAD_PARTICLE )
			{
				declManager->FindType( DECL_PARTICLE, p.resourceName );
			}
			numLoaded++;
		}
		
		int	end = Sys_Milliseconds();
		common->Printf( "%05d models preloaded ( or were already loaded ) in %5.1f seconds\n", numLoaded, ( end - start ) * 0.001 );
		common->Printf( "----------------------------------------\n" );
	}
}



/*
=================
budRenderModelManagerLocal::EndLevelLoad
=================
*/
void budRenderModelManagerLocal::EndLevelLoad()
{
	common->Printf( "----- budRenderModelManagerLocal::EndLevelLoad -----\n" );
	
	int start = Sys_Milliseconds();
	
	insideLevelLoad = false;
	int	purgeCount = 0;
	int	keepCount = 0;
	int	loadCount = 0;
	
	// purge any models not touched
	for( int i = 0; i < models.Num(); i++ )
	{
		budRenderModel* model = models[i];
		
		if( !model->IsLevelLoadReferenced() && model->IsLoaded() && model->IsReloadable() )
		{
		
//			common->Printf( "purging %s\n", model->Name() );

			purgeCount++;
			
			R_CheckForEntityDefsUsingModel( model );
			
			model->PurgeModel();
			
		}
		else
		{
		
//			common->Printf( "keeping %s\n", model->Name() );

			keepCount++;
		}
		
		common->UpdateLevelLoadPacifier();
	}
	
	// load any new ones
	for( int i = 0; i < models.Num(); i++ )
	{
		common->UpdateLevelLoadPacifier();
		
		
		budRenderModel* model = models[i];
		
		if( model->IsLevelLoadReferenced() && !model->IsLoaded() && model->IsReloadable() )
		{
			loadCount++;
			model->LoadModel();
		}
	}
	
	// create static vertex/index buffers for all models
	for( int i = 0; i < models.Num(); i++ )
	{
		common->UpdateLevelLoadPacifier();
		
		
		budRenderModel* model = models[i];
		if( model->IsLoaded() )
		{
			for( int j = 0; j < model->NumSurfaces(); j++ )
			{
				R_CreateStaticBuffersForTri( *( model->Surface( j )->geometry ) );
			}
		}
	}
	
	
	// _D3XP added this
	int	end = Sys_Milliseconds();
	common->Printf( "%5i models purged from previous level, ", purgeCount );
	common->Printf( "%5i models kept.\n", keepCount );
	if( loadCount )
	{
		common->Printf( "%5i new models loaded in %5.1f seconds\n", loadCount, ( end - start ) * 0.001 );
	}
	common->Printf( "---------------------------------------------------\n" );
}

/*
=================
budRenderModelManagerLocal::PrintMemInfo
=================
*/
void budRenderModelManagerLocal::PrintMemInfo( MemInfo_t* mi )
{
	int i, j, totalMem = 0;
	int* sortIndex;
	budFile* f;
	
	f = fileSystem->OpenFileWrite( mi->filebase + "_models.txt" );
	if( !f )
	{
		return;
	}
	
	// sort first
	sortIndex = new( TAG_MODEL ) int[ localModelManager.models.Num()];
	
	for( i = 0; i <  localModelManager.models.Num(); i++ )
	{
		sortIndex[i] = i;
	}
	
	for( i = 0; i <  localModelManager.models.Num() - 1; i++ )
	{
		for( j = i + 1; j <  localModelManager.models.Num(); j++ )
		{
			if( localModelManager.models[sortIndex[i]]->Memory() <  localModelManager.models[sortIndex[j]]->Memory() )
			{
				int temp = sortIndex[i];
				sortIndex[i] = sortIndex[j];
				sortIndex[j] = temp;
			}
		}
	}
	
	// print next
	for( int i = 0; i < localModelManager.models.Num(); i++ )
	{
		budRenderModel*	model = localModelManager.models[sortIndex[i]];
		int mem;
		
		if( !model->IsLoaded() )
		{
			continue;
		}
		
		mem = model->Memory();
		totalMem += mem;
		f->Printf( "%s %s\n", budStr::FormatNumber( mem ).c_str(), model->Name() );
	}
	
	delete [] sortIndex;
	mi->modelAssetsTotal = totalMem;
	
	f->Printf( "\nTotal model bytes allocated: %s\n", budStr::FormatNumber( totalMem ).c_str() );
	fileSystem->CloseFile( f );
}
