/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.
Copyright (C) 2014 Robert Beckebans

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

#include "libBudPCH.hpp"
#pragma hdrstop

//#define DEBUG_EVAL
#define MAX_DEFINEPARMS				128
#define DEFINEHASHSIZE				2048

#define TOKEN_FL_RECURSIVE_DEFINE	1

define_t* budParser::globaldefines;

/*
================
budParser::SetBaseFolder
================
*/
void budParser::SetBaseFolder( const char* path )
{
	budLexer::SetBaseFolder( path );
}

/*
================
budParser::AddGlobalDefine
================
*/
int budParser::AddGlobalDefine( const char* string )
{
	define_t* define;
	
	define = budParser::DefineFromString( string );
	if( !define )
	{
		return false;
	}
	define->next = globaldefines;
	globaldefines = define;
	return true;
}

/*
================
budParser::RemoveGlobalDefine
================
*/
int budParser::RemoveGlobalDefine( const char* name )
{
	define_t* d, *prev;
	
	for( prev = NULL, d = budParser::globaldefines; d; prev = d, d = d->next )
	{
		if( !strcmp( d->name, name ) )
		{
			break;
		}
	}
	if( d )
	{
		if( prev )
		{
			prev->next = d->next;
		}
		else
		{
			budParser::globaldefines = d->next;
		}
		budParser::FreeDefine( d );
		return true;
	}
	return false;
}

/*
================
budParser::RemoveAllGlobalDefines
================
*/
void budParser::RemoveAllGlobalDefines()
{
	define_t* define;
	
	for( define = globaldefines; define; define = globaldefines )
	{
		globaldefines = globaldefines->next;
		budParser::FreeDefine( define );
	}
}


/*
===============================================================================

budParser

===============================================================================
*/

/*
================
budParser::PrintDefine
================
*/
void budParser::PrintDefine( define_t* define )
{
	libBud::common->Printf( "define->name = %s\n", define->name );
	libBud::common->Printf( "define->flags = %d\n", define->flags );
	libBud::common->Printf( "define->builtin = %d\n", define->builtin );
	libBud::common->Printf( "define->numparms = %d\n", define->numparms );
}

/*
================
PC_PrintDefineHashTable
================
* /
static void PC_PrintDefineHashTable(define_t **definehash) {
	int i;
	define_t *d;

	for (i = 0; i < DEFINEHASHSIZE; i++) {
		Log_Write("%4d:", i);
		for (d = definehash[i]; d; d = d->hashnext) {
			Log_Write(" %s", d->name);
		}
		Log_Write("\n");
	}
}
*/

/*
================
PC_NameHash
================
*/
BUD_INLINE int PC_NameHash( const char* name )
{
	int hash, i;
	
	hash = 0;
	for( i = 0; name[i] != '\0'; i++ )
	{
		hash += name[i] * ( 119 + i );
	}
	hash = ( hash ^ ( hash >> 10 ) ^ ( hash >> 20 ) ) & ( DEFINEHASHSIZE - 1 );
	return hash;
}

/*
================
budParser::AddDefineToHash
================
*/
void budParser::AddDefineToHash( define_t* define, define_t** definehash )
{
	int hash;
	
	hash = PC_NameHash( define->name );
	define->hashnext = definehash[hash];
	definehash[hash] = define;
}

/*
================
FindHashedDefine
================
*/
define_t* budParser::FindHashedDefine( define_t** definehash, const char* name )
{
	define_t* d;
	int hash;
	
	hash = PC_NameHash( name );
	for( d = definehash[hash]; d; d = d->hashnext )
	{
		if( !strcmp( d->name, name ) )
		{
			return d;
		}
	}
	return NULL;
}

/*
================
budParser::FindDefine
================
*/
define_t* budParser::FindDefine( define_t* defines, const char* name )
{
	define_t* d;
	
	for( d = defines; d; d = d->next )
	{
		if( !strcmp( d->name, name ) )
		{
			return d;
		}
	}
	return NULL;
}

/*
================
budParser::FindDefineParm
================
*/
int budParser::FindDefineParm( define_t* define, const char* name )
{
	budToken* p;
	int i;
	
	i = 0;
	for( p = define->parms; p; p = p->next )
	{
		if( ( *p ) == name )
		{
			return i;
		}
		i++;
	}
	return -1;
}

/*
================
budParser::CopyDefine
================
*/
define_t* budParser::CopyDefine( define_t* define )
{
	define_t* newdefine;
	budToken* token, *newtoken, *lasttoken;
	
	newdefine = ( define_t* ) Mem_Alloc( sizeof( define_t ) + strlen( define->name ) + 1, TAG_LIBBUD_PARSER );
	//copy the define name
	newdefine->name = ( char* ) newdefine + sizeof( define_t );
	strcpy( newdefine->name, define->name );
	newdefine->flags = define->flags;
	newdefine->builtin = define->builtin;
	newdefine->numparms = define->numparms;
	//the define is not linked
	newdefine->next = NULL;
	newdefine->hashnext = NULL;
	//copy the define tokens
	newdefine->tokens = NULL;
	for( lasttoken = NULL, token = define->tokens; token; token = token->next )
	{
		newtoken = new( TAG_LIBBUD_PARSER ) budToken( token );
		newtoken->next = NULL;
		if( lasttoken ) lasttoken->next = newtoken;
		else newdefine->tokens = newtoken;
		lasttoken = newtoken;
	}
	//copy the define parameters
	newdefine->parms = NULL;
	for( lasttoken = NULL, token = define->parms; token; token = token->next )
	{
		newtoken = new( TAG_LIBBUD_PARSER ) budToken( token );
		newtoken->next = NULL;
		if( lasttoken ) lasttoken->next = newtoken;
		else newdefine->parms = newtoken;
		lasttoken = newtoken;
	}
	return newdefine;
}

/*
================
budParser::FreeDefine
================
*/
void budParser::FreeDefine( define_t* define )
{
	budToken* t, *next;
	
	//free the define parameters
	for( t = define->parms; t; t = next )
	{
		next = t->next;
		delete t;
	}
	//free the define tokens
	for( t = define->tokens; t; t = next )
	{
		next = t->next;
		delete t;
	}
	//free the define
	Mem_Free( define );
}

/*
================
budParser::DefineFromString
================
*/
define_t* budParser::DefineFromString( const char* string )
{
	budParser src;
	define_t* def;
	
	if( !src.LoadMemory( string, strlen( string ), "*defineString" ) )
	{
		return NULL;
	}
	// create a define from the source
	if( !src.Directive_define() )
	{
		src.FreeSource();
		return NULL;
	}
	def = src.CopyFirstDefine();
	src.FreeSource();
	//if the define was created succesfully
	return def;
}

/*
================
budParser::Error
================
*/
void budParser::Error( const char* str, ... ) const
{
	char text[MAX_STRING_CHARS];
	va_list ap;
	
	va_start( ap, str );
	vsprintf( text, str, ap );
	va_end( ap );
	if( budParser::scriptstack )
	{
		budParser::scriptstack->Error( text );
	}
}

/*
================
budParser::Warning
================
*/
void budParser::Warning( const char* str, ... ) const
{
	char text[MAX_STRING_CHARS];
	va_list ap;
	
	va_start( ap, str );
	vsprintf( text, str, ap );
	va_end( ap );
	if( budParser::scriptstack )
	{
		budParser::scriptstack->Warning( text );
	}
}

/*
================
budParser::PushIndent
================
*/
void budParser::PushIndent( int type, int skip )
{
	indent_t* indent;
	
	indent = ( indent_t* ) Mem_Alloc( sizeof( indent_t ), TAG_LIBBUD_PARSER );
	indent->type = type;
	indent->script = budParser::scriptstack;
	indent->skip = ( skip != 0 );
	budParser::skip += indent->skip;
	indent->next = budParser::indentstack;
	budParser::indentstack = indent;
}

/*
================
budParser::PopIndent
================
*/
void budParser::PopIndent( int* type, int* skip )
{
	indent_t* indent;
	
	*type = 0;
	*skip = 0;
	
	indent = budParser::indentstack;
	if( !indent ) return;
	
	// must be an indent from the current script
	if( budParser::indentstack->script != budParser::scriptstack )
	{
		return;
	}
	
	*type = indent->type;
	*skip = indent->skip;
	budParser::indentstack = budParser::indentstack->next;
	budParser::skip -= indent->skip;
	Mem_Free( indent );
}

/*
================
budParser::PushScript
================
*/
void budParser::PushScript( budLexer* script )
{
	budLexer* s;
	
	for( s = budParser::scriptstack; s; s = s->next )
	{
		if( !budStr::Icmp( s->GetFileName(), script->GetFileName() ) )
		{
			budParser::Warning( "'%s' recursively included", script->GetFileName() );
			return;
		}
	}
	//push the script on the script stack
	script->next = budParser::scriptstack;
	budParser::scriptstack = script;
}

/*
================
budParser::ReadSourceToken
================
*/
int budParser::ReadSourceToken( budToken* token )
{
	budToken* t;
	budLexer* script;
	int type, skip, changedScript;
	
	if( !budParser::scriptstack )
	{
		libBud::common->FatalError( "budParser::ReadSourceToken: not loaded" );
		return false;
	}
	changedScript = 0;
	// if there's no token already available
	while( !budParser::tokens )
	{
		// if there's a token to read from the script
		if( budParser::scriptstack->ReadToken( token ) )
		{
			token->linesCrossed += changedScript;
			
			// set the marker based on the start of the token read in
			if( !marker_p )
			{
				marker_p = token->whiteSpaceEnd_p;
			}
			return true;
		}
		// if at the end of the script
		if( budParser::scriptstack->EndOfFile() )
		{
			// remove all indents of the script
			while( budParser::indentstack && budParser::indentstack->script == budParser::scriptstack )
			{
				budParser::Warning( "missing #endif" );
				budParser::PopIndent( &type, &skip );
			}
			changedScript = 1;
		}
		// if this was the initial script
		if( !budParser::scriptstack->next )
		{
			return false;
		}
		// remove the script and return to the previous one
		script = budParser::scriptstack;
		budParser::scriptstack = budParser::scriptstack->next;
		delete script;
	}
	// copy the already available token
	*token = budParser::tokens;
	// remove the token from the source
	t = budParser::tokens;
	assert( budParser::tokens != NULL );
	budParser::tokens = budParser::tokens->next;
	delete t;
	return true;
}

/*
================
budParser::UnreadSourceToken
================
*/
int budParser::UnreadSourceToken( budToken* token )
{
	budToken* t;
	
	t = new( TAG_LIBBUD_PARSER ) budToken( token );
	t->next = budParser::tokens;
	budParser::tokens = t;
	return true;
}

/*
================
budParser::ReadDefineParms
================
*/
int budParser::ReadDefineParms( define_t* define, budToken** parms, int maxparms )
{
	define_t* newdefine;
	budToken token, *t, *last;
	int i, done, lastcomma, numparms, indent;
	
	if( !budParser::ReadSourceToken( &token ) )
	{
		budParser::Error( "define '%s' missing parameters", define->name );
		return false;
	}
	
	if( define->numparms > maxparms )
	{
		budParser::Error( "define with more than %d parameters", maxparms );
		return false;
	}
	
	for( i = 0; i < define->numparms; i++ )
	{
		parms[i] = NULL;
	}
	// if no leading "("
	if( token != "(" )
	{
		budParser::UnreadSourceToken( &token );
		budParser::Error( "define '%s' missing parameters", define->name );
		return false;
	}
	// read the define parameters
	for( done = 0, numparms = 0, indent = 1; !done; )
	{
		if( numparms >= maxparms )
		{
			budParser::Error( "define '%s' with too many parameters", define->name );
			return false;
		}
		parms[numparms] = NULL;
		lastcomma = 1;
		last = NULL;
		while( !done )
		{
		
			if( !budParser::ReadSourceToken( &token ) )
			{
				budParser::Error( "define '%s' incomplete", define->name );
				return false;
			}
			
			if( token == "," )
			{
				if( indent <= 1 )
				{
					if( lastcomma )
					{
						budParser::Warning( "too many comma's" );
					}
					if( numparms >= define->numparms )
					{
						budParser::Warning( "too many define parameters" );
					}
					lastcomma = 1;
					break;
				}
			}
			else if( token == "(" )
			{
				indent++;
			}
			else if( token == ")" )
			{
				indent--;
				if( indent <= 0 )
				{
					if( !parms[define->numparms - 1] )
					{
						budParser::Warning( "too few define parameters" );
					}
					done = 1;
					break;
				}
			}
			else if( token.type == TT_NAME )
			{
				newdefine = FindHashedDefine( budParser::definehash, token.c_str() );
				if( newdefine )
				{
					if( !budParser::ExpandDefineIntoSource( &token, newdefine ) )
					{
						return false;
					}
					continue;
				}
			}
			
			lastcomma = 0;
			
			if( numparms < define->numparms )
			{
			
				t = new( TAG_LIBBUD_PARSER ) budToken( token );
				t->next = NULL;
				if( last ) last->next = t;
				else parms[numparms] = t;
				last = t;
			}
		}
		numparms++;
	}
	return true;
}

/*
================
budParser::StringizeTokens
================
*/
int budParser::StringizeTokens( budToken* tokens, budToken* token )
{
	budToken* t;
	
	token->type = TT_STRING;
	token->whiteSpaceStart_p = NULL;
	token->whiteSpaceEnd_p = NULL;
	( *token ) = "";
	for( t = tokens; t; t = t->next )
	{
		token->Append( t->c_str() );
	}
	return true;
}

/*
================
budParser::MergeTokens
================
*/
int budParser::MergeTokens( budToken* t1, budToken* t2 )
{
	// merging of a name with a name or number
	if( t1->type == TT_NAME && ( t2->type == TT_NAME || ( t2->type == TT_NUMBER && !( t2->subtype & TT_FLOAT ) ) ) )
	{
		t1->Append( t2->c_str() );
		return true;
	}
	// merging of two strings
	if( t1->type == TT_STRING && t2->type == TT_STRING )
	{
		t1->Append( t2->c_str() );
		return true;
	}
	// merging of two numbers
	if( t1->type == TT_NUMBER && t2->type == TT_NUMBER &&
			!( t1->subtype & ( TT_HEX | TT_BINARY ) ) && !( t2->subtype & ( TT_HEX | TT_BINARY ) ) &&
			( !( t1->subtype & TT_FLOAT ) || !( t2->subtype & TT_FLOAT ) ) )
	{
		t1->Append( t2->c_str() );
		return true;
	}
	
	return false;
}

/*
================
budParser::AddBuiltinDefines
================
*/
void budParser::AddBuiltinDefines()
{
	int i;
	define_t* define;
	struct builtin
	{
		const char* string;
		int id;
	} builtin[] =
	{
		{ "__LINE__",	BUILTIN_LINE },
		{ "__FILE__",	BUILTIN_FILE },
		{ "__DATE__",	BUILTIN_DATE },
		{ "__TIME__",	BUILTIN_TIME },
		{ "__STDC__", BUILTIN_STDC },
		{ NULL, 0 }
	};
	
	for( i = 0; builtin[i].string; i++ )
	{
		define = ( define_t* ) Mem_Alloc( sizeof( define_t ) + strlen( builtin[i].string ) + 1, TAG_LIBBUD_PARSER );
		define->name = ( char* ) define + sizeof( define_t );
		strcpy( define->name, builtin[i].string );
		define->flags = DEFINE_FIXED;
		define->builtin = builtin[i].id;
		define->numparms = 0;
		define->parms = NULL;
		define->tokens = NULL;
		// add the define to the source
		AddDefineToHash( define, budParser::definehash );
	}
}

/*
================
budParser::CopyFirstDefine
================
*/
define_t* budParser::CopyFirstDefine()
{
	int i;
	
	for( i = 0; i < DEFINEHASHSIZE; i++ )
	{
		if( budParser::definehash[i] )
		{
			return CopyDefine( budParser::definehash[i] );
		}
	}
	return NULL;
}

static budStr PreProcessorDate()
{
	time_t t = time( NULL );
	char* curtime = ctime( &t );
	if( budStr::Length( curtime ) < 24 )
	{
		return budStr( "*** BAD CURTIME ***" );
	}
	budStr	str = "\"";
	// skip DAY, extract MMM DD
	for( int i = 4 ; i < 10 ; i++ )
	{
		str.Append( curtime[i] );
	}
	// skip time, extract space+YYYY
	for( int i = 19 ; i < 24 ; i++ )
	{
		str.Append( curtime[i] );
	}
	str.Append( "\"" );
	return str;
}

static budStr PreProcessorTime()
{
	time_t t = time( NULL );
	char* curtime = ctime( &t );
	if( budStr::Length( curtime ) < 24 )
	{
		return budStr( "*** BAD CURTIME ***" );
	}
	
	budStr	str = "\"";
	for( int i = 11 ; i < 19 ; i++ )
	{
		str.Append( curtime[i] );
	}
	str.Append( "\"" );
	return str;
}

CONSOLE_COMMAND( TestPreprocessorMacros, "check analyze warning", 0 )
{
	libBud::Printf( "%s : %s\n", __DATE__, PreProcessorDate().c_str() );
	libBud::Printf( "%s : %s\n", __TIME__, PreProcessorTime().c_str() );
}

/*
================
budParser::ExpandBuiltinDefine
================
*/
int budParser::ExpandBuiltinDefine( budToken* deftoken, define_t* define, budToken** firsttoken, budToken** lasttoken )
{
	budToken* token;
	char buf[MAX_STRING_CHARS];
	
	token = new( TAG_LIBBUD_PARSER ) budToken( deftoken );
	switch( define->builtin )
	{
		case BUILTIN_LINE:
		{
			sprintf( buf, "%d", deftoken->line );
			( *token ) = buf;
			token->intvalue = deftoken->line;
			token->floatvalue = deftoken->line;
			token->type = TT_NUMBER;
			token->subtype = TT_DECIMAL | TT_INTEGER | TT_VALUESVALID;
			token->line = deftoken->line;
			token->linesCrossed = deftoken->linesCrossed;
			token->flags = 0;
			*firsttoken = token;
			*lasttoken = token;
			break;
		}
		case BUILTIN_FILE:
		{
			( *token ) = budParser::scriptstack->GetFileName();
			token->type = TT_NAME;
			token->subtype = token->Length();
			token->line = deftoken->line;
			token->linesCrossed = deftoken->linesCrossed;
			token->flags = 0;
			*firsttoken = token;
			*lasttoken = token;
			break;
		}
		case BUILTIN_DATE:
		{
			*token = PreProcessorDate();
			token->type = TT_STRING;
			token->subtype = token->Length();
			token->line = deftoken->line;
			token->linesCrossed = deftoken->linesCrossed;
			token->flags = 0;
			*firsttoken = token;
			*lasttoken = token;
			break;
		}
		case BUILTIN_TIME:
		{
			*token = PreProcessorTime();
			token->type = TT_STRING;
			token->subtype = token->Length();
			token->line = deftoken->line;
			token->linesCrossed = deftoken->linesCrossed;
			token->flags = 0;
			*firsttoken = token;
			*lasttoken = token;
			break;
		}
		case BUILTIN_STDC:
		{
			budParser::Warning( "__STDC__ not supported\n" );
			*firsttoken = NULL;
			*lasttoken = NULL;
			break;
		}
		default:
		{
			*firsttoken = NULL;
			*lasttoken = NULL;
			break;
		}
	}
	return true;
}

/*
================
budParser::ExpandDefine
================
*/
int budParser::ExpandDefine( budToken* deftoken, define_t* define, budToken** firsttoken, budToken** lasttoken )
{
	budToken* parms[MAX_DEFINEPARMS], *dt, *pt, *t;
	budToken* t1, *t2, *first, *last, *nextpt, token;
	int parmnum, i;
	
	// if it is a builtin define
	if( define->builtin )
	{
		return budParser::ExpandBuiltinDefine( deftoken, define, firsttoken, lasttoken );
	}
	// if the define has parameters
	if( define->numparms )
	{
		if( !budParser::ReadDefineParms( define, parms, MAX_DEFINEPARMS ) )
		{
			return false;
		}
#ifdef DEBUG_EVAL
		for( i = 0; i < define->numparms; i++ )
		{
			Log_Write( "define parms %d:", i );
			for( pt = parms[i]; pt; pt = pt->next )
			{
				Log_Write( "%s", pt->c_str() );
			}
		}
#endif //DEBUG_EVAL
	}
	// empty list at first
	first = NULL;
	last = NULL;
	// create a list with tokens of the expanded define
	for( dt = define->tokens; dt; dt = dt->next )
	{
		parmnum = -1;
		// if the token is a name, it could be a define parameter
		if( dt->type == TT_NAME )
		{
			parmnum = FindDefineParm( define, dt->c_str() );
		}
		// if it is a define parameter
		if( parmnum >= 0 )
		{
			for( pt = parms[parmnum]; pt; pt = pt->next )
			{
				t = new( TAG_LIBBUD_PARSER ) budToken( pt );
				//add the token to the list
				t->next = NULL;
				if( last ) last->next = t;
				else first = t;
				last = t;
			}
		}
		else
		{
			// if stringizing operator
			if( ( *dt ) == "#" )
			{
				// the stringizing operator must be followed by a define parameter
				if( dt->next )
				{
					parmnum = FindDefineParm( define, dt->next->c_str() );
				}
				else
				{
					parmnum = -1;
				}
				
				if( parmnum >= 0 )
				{
					// step over the stringizing operator
					dt = dt->next;
					// stringize the define parameter tokens
					if( !budParser::StringizeTokens( parms[parmnum], &token ) )
					{
						budParser::Error( "can't stringize tokens" );
						return false;
					}
					t = new( TAG_LIBBUD_PARSER ) budToken( token );
					t->line = deftoken->line;
				}
				else
				{
					budParser::Warning( "stringizing operator without define parameter" );
					continue;
				}
			}
			else
			{
				t = new( TAG_LIBBUD_PARSER ) budToken( dt );
				t->line = deftoken->line;
			}
			// add the token to the list
			t->next = NULL;
// the token being read from the define list should use the line number of
// the original file, not the header file
			t->line = deftoken->line;
			
			if( last ) last->next = t;
			else first = t;
			last = t;
		}
	}
	// check for the merging operator
	for( t = first; t; )
	{
		if( t->next )
		{
			// if the merging operator
			if( ( *t->next ) == "##" )
			{
				t1 = t;
				t2 = t->next->next;
				if( t2 )
				{
					if( !budParser::MergeTokens( t1, t2 ) )
					{
						budParser::Error( "can't merge '%s' with '%s'", t1->c_str(), t2->c_str() );
						return false;
					}
					delete t1->next;
					t1->next = t2->next;
					if( t2 == last ) last = t1;
					delete t2;
					continue;
				}
			}
		}
		t = t->next;
	}
	// store the first and last token of the list
	*firsttoken = first;
	*lasttoken = last;
	// free all the parameter tokens
	for( i = 0; i < define->numparms; i++ )
	{
		for( pt = parms[i]; pt; pt = nextpt )
		{
			nextpt = pt->next;
			delete pt;
		}
	}
	
	return true;
}

/*
================
budParser::ExpandDefineIntoSource
================
*/
int budParser::ExpandDefineIntoSource( budToken* deftoken, define_t* define )
{
	budToken* firsttoken, *lasttoken;
	
	if( !budParser::ExpandDefine( deftoken, define, &firsttoken, &lasttoken ) )
	{
		return false;
	}
	// if the define is not empty
	if( firsttoken && lasttoken )
	{
		firsttoken->linesCrossed += deftoken->linesCrossed;
		lasttoken->next = budParser::tokens;
		budParser::tokens = firsttoken;
	}
	return true;
}

/*
================
budParser::ReadLine

reads a token from the current line, continues reading on the next
line only if a backslash '\' is found
================
*/
int budParser::ReadLine( budToken* token )
{
	int crossline;
	
	crossline = 0;
	do
	{
		if( !budParser::ReadSourceToken( token ) )
		{
			return false;
		}
		
		if( token->linesCrossed > crossline )
		{
			budParser::UnreadSourceToken( token );
			return false;
		}
		crossline = 1;
	}
	while( ( *token ) == "\\" );
	return true;
}

/*
================
budParser::Directive_include
================
*/
// RB: added token as parameter
int budParser::Directive_include( budToken* token, bool supressWarning )
{
	budLexer* script;
	budStr path;
	
	if( !budParser::ReadSourceToken( token ) )
	{
		budParser::Error( "#include without file name" );
		return false;
	}
	if( token->linesCrossed > 0 )
	{
		budParser::Error( "#include without file name" );
		return false;
	}
	if( token->type == TT_STRING )
	{
		script = new( TAG_LIBBUD_PARSER ) budLexer;
		// try relative to the current file
		path = scriptstack->GetFileName();
		path.StripFilename();
		path += "/";
		path += *token;
		if( !script->LoadFile( path, OSPath ) )
		{
			// try absolute path
			path = *token;
			if( !script->LoadFile( path, OSPath ) )
			{
				// try from the include path
				path = includepath + *token;
				if( !script->LoadFile( path, OSPath ) )
				{
					delete script;
					script = NULL;
				}
			}
		}
	}
	else if( token->type == TT_PUNCTUATION && *token == "<" )
	{
		path = budParser::includepath;
		while( budParser::ReadSourceToken( token ) )
		{
			if( token->linesCrossed > 0 )
			{
				budParser::UnreadSourceToken( token );
				break;
			}
			if( token->type == TT_PUNCTUATION && *token == ">" )
			{
				break;
			}
			path += *token;
		}
		if( *token != ">" )
		{
			budParser::Warning( "#include missing trailing >" );
		}
		if( !path.Length() )
		{
			budParser::Error( "#include without file name between < >" );
			return false;
		}
		if( budParser::flags & LEXFL_NOBASEINCLUDES )
		{
			return true;
		}
		script = new( TAG_LIBBUD_PARSER ) budLexer;
		if( !script->LoadFile( includepath + path, OSPath ) )
		{
			delete script;
			script = NULL;
		}
	}
	else
	{
		budParser::Error( "#include without file name" );
		return false;
	}
	
	if( !script )
	{
		if( !supressWarning )
		{
			budParser::Error( "file '%s' not found", path.c_str() );
		}
		return false;
	}
	script->SetFlags( budParser::flags );
	script->SetPunctuations( budParser::punctuations );
	budParser::PushScript( script );
	return true;
}
// RB end

/*
================
budParser::Directive_undef
================
*/
int budParser::Directive_undef()
{
	budToken token;
	define_t* define, *lastdefine;
	int hash;
	
	//
	if( !budParser::ReadLine( &token ) )
	{
		budParser::Error( "undef without name" );
		return false;
	}
	if( token.type != TT_NAME )
	{
		budParser::UnreadSourceToken( &token );
		budParser::Error( "expected name but found '%s'", token.c_str() );
		return false;
	}
	
	hash = PC_NameHash( token.c_str() );
	for( lastdefine = NULL, define = budParser::definehash[hash]; define; define = define->hashnext )
	{
		if( !strcmp( define->name, token.c_str() ) )
		{
			if( define->flags & DEFINE_FIXED )
			{
				budParser::Warning( "can't undef '%s'", token.c_str() );
			}
			else
			{
				if( lastdefine )
				{
					lastdefine->hashnext = define->hashnext;
				}
				else
				{
					budParser::definehash[hash] = define->hashnext;
				}
				FreeDefine( define );
			}
			break;
		}
		lastdefine = define;
	}
	return true;
}

/*
================
budParser::Directive_define
================
*/
int budParser::Directive_define()
{
	budToken token, *t, *last;
	define_t* define;
	
	if( !budParser::ReadLine( &token ) )
	{
		budParser::Error( "#define without name" );
		return false;
	}
	if( token.type != TT_NAME )
	{
		budParser::UnreadSourceToken( &token );
		budParser::Error( "expected name after #define, found '%s'", token.c_str() );
		return false;
	}
	// check if the define already exists
	define = FindHashedDefine( budParser::definehash, token.c_str() );
	if( define )
	{
		if( define->flags & DEFINE_FIXED )
		{
			budParser::Error( "can't redefine '%s'", token.c_str() );
			return false;
		}
		budParser::Warning( "redefinition of '%s'", token.c_str() );
		// unread the define name before executing the #undef directive
		budParser::UnreadSourceToken( &token );
		if( !budParser::Directive_undef() )
			return false;
		// if the define was not removed (define->flags & DEFINE_FIXED)
		define = FindHashedDefine( budParser::definehash, token.c_str() );
	}
	// allocate define
	define = ( define_t* ) Mem_ClearedAlloc( sizeof( define_t ) + token.Length() + 1, TAG_LIBBUD_PARSER );
	define->name = ( char* ) define + sizeof( define_t );
	strcpy( define->name, token.c_str() );
	// add the define to the source
	AddDefineToHash( define, budParser::definehash );
	// if nothing is defined, just return
	if( !budParser::ReadLine( &token ) )
	{
		return true;
	}
	// if it is a define with parameters
	if( token.WhiteSpaceBeforeToken() == 0 && token == "(" )
	{
		// read the define parameters
		last = NULL;
		if( !budParser::CheckTokenString( ")" ) )
		{
			while( 1 )
			{
				if( !budParser::ReadLine( &token ) )
				{
					budParser::Error( "expected define parameter" );
					return false;
				}
				// if it isn't a name
				if( token.type != TT_NAME )
				{
					budParser::Error( "invalid define parameter" );
					return false;
				}
				
				if( FindDefineParm( define, token.c_str() ) >= 0 )
				{
					budParser::Error( "two the same define parameters" );
					return false;
				}
				// add the define parm
				t = new( TAG_LIBBUD_PARSER ) budToken( token );
				t->ClearTokenWhiteSpace();
				t->next = NULL;
				if( last ) last->next = t;
				else define->parms = t;
				last = t;
				define->numparms++;
				// read next token
				if( !budParser::ReadLine( &token ) )
				{
					budParser::Error( "define parameters not terminated" );
					return false;
				}
				
				if( token == ")" )
				{
					break;
				}
				// then it must be a comma
				if( token != "," )
				{
					budParser::Error( "define not terminated" );
					return false;
				}
			}
		}
		if( !budParser::ReadLine( &token ) )
		{
			return true;
		}
	}
	// read the defined stuff
	last = NULL;
	do
	{
		t = new( TAG_LIBBUD_PARSER ) budToken( token );
		if( t->type == TT_NAME && !strcmp( t->c_str(), define->name ) )
		{
			t->flags |= TOKEN_FL_RECURSIVE_DEFINE;
			budParser::Warning( "recursive define (removed recursion)" );
		}
		t->ClearTokenWhiteSpace();
		t->next = NULL;
		if( last ) last->next = t;
		else define->tokens = t;
		last = t;
	}
	while( budParser::ReadLine( &token ) );
	
	if( last )
	{
		// check for merge operators at the beginning or end
		if( ( *define->tokens ) == "##" || ( *last ) == "##" )
		{
			budParser::Error( "define with misplaced ##" );
			return false;
		}
	}
	return true;
}

/*
================
budParser::AddDefine
================
*/
int budParser::AddDefine( const char* string )
{
	define_t* define;
	
	define = DefineFromString( string );
	if( !define )
	{
		return false;
	}
	AddDefineToHash( define, budParser::definehash );
	return true;
}

/*
================
budParser::AddGlobalDefinesToSource
================
*/
void budParser::AddGlobalDefinesToSource()
{
	define_t* define, *newdefine;
	
	for( define = globaldefines; define; define = define->next )
	{
		newdefine = CopyDefine( define );
		AddDefineToHash( newdefine, budParser::definehash );
	}
}

/*
================
budParser::Directive_if_def
================
*/
int budParser::Directive_if_def( int type )
{
	budToken token;
	define_t* d;
	int skip;
	
	if( !budParser::ReadLine( &token ) )
	{
		budParser::Error( "#ifdef without name" );
		return false;
	}
	if( token.type != TT_NAME )
	{
		budParser::UnreadSourceToken( &token );
		budParser::Error( "expected name after #ifdef, found '%s'", token.c_str() );
		return false;
	}
	d = FindHashedDefine( budParser::definehash, token.c_str() );
	skip = ( type == INDENT_IFDEF ) == ( d == NULL );
	budParser::PushIndent( type, skip );
	return true;
}

/*
================
budParser::Directive_ifdef
================
*/
int budParser::Directive_ifdef()
{
	return budParser::Directive_if_def( INDENT_IFDEF );
}

/*
================
budParser::Directive_ifndef
================
*/
int budParser::Directive_ifndef()
{
	return budParser::Directive_if_def( INDENT_IFNDEF );
}

/*
================
budParser::Directive_else
================
*/
int budParser::Directive_else()
{
	int type, skip;
	
	budParser::PopIndent( &type, &skip );
	if( !type )
	{
		budParser::Error( "misplaced #else" );
		return false;
	}
	if( type == INDENT_ELSE )
	{
		budParser::Error( "#else after #else" );
		return false;
	}
	budParser::PushIndent( INDENT_ELSE, !skip );
	return true;
}

/*
================
budParser::Directive_endif
================
*/
int budParser::Directive_endif()
{
	int type, skip;
	
	budParser::PopIndent( &type, &skip );
	if( !type )
	{
		budParser::Error( "misplaced #endif" );
		return false;
	}
	return true;
}

/*
================
budParser::EvaluateTokens
================
*/
typedef struct operator_s
{
	int op;
	int priority;
	int parentheses;
	struct operator_s* prev, *next;
} operator_t;

typedef struct value_s
{
	signed int intvalue; // DG: use int instead of long for 64bit compatibility
	double floatvalue;
	int parentheses;
	struct value_s* prev, *next;
} value_t;

int PC_OperatorPriority( int op )
{
	switch( op )
	{
		case P_MUL:
			return 15;
		case P_DIV:
			return 15;
		case P_MOD:
			return 15;
		case P_ADD:
			return 14;
		case P_SUB:
			return 14;
			
		case P_LOGIC_AND:
			return 7;
		case P_LOGIC_OR:
			return 6;
		case P_LOGIC_GEQ:
			return 12;
		case P_LOGIC_LEQ:
			return 12;
		case P_LOGIC_EQ:
			return 11;
		case P_LOGIC_UNEQ:
			return 11;
			
		case P_LOGIC_NOT:
			return 16;
		case P_LOGIC_GREATER:
			return 12;
		case P_LOGIC_LESS:
			return 12;
			
		case P_RSHIFT:
			return 13;
		case P_LSHIFT:
			return 13;
			
		case P_BIN_AND:
			return 10;
		case P_BIN_OR:
			return 8;
		case P_BIN_XOR:
			return 9;
		case P_BIN_NOT:
			return 16;
			
		case P_COLON:
			return 5;
		case P_QUESTIONMARK:
			return 5;
	}
	return false;
}

//#define AllocValue()			GetClearedMemory(sizeof(value_t));
//#define FreeValue(val)		FreeMemory(val)
//#define AllocOperator(op)		op = (operator_t *) GetClearedMemory(sizeof(operator_t));
//#define FreeOperator(op)		FreeMemory(op);

#define MAX_VALUES		64
#define MAX_OPERATORS	64

#define AllocValue(val)									\
	if ( numvalues >= MAX_VALUES ) {					\
		budParser::Error( "out of value space\n" );		\
		error = 1;										\
		break;											\
	}													\
	else {												\
		val = &value_heap[numvalues++];					\
	}

#define FreeValue(val)

#define AllocOperator(op)								\
	if ( numoperators >= MAX_OPERATORS ) {				\
		budParser::Error( "out of operator space\n" );	\
		error = 1;										\
		break;											\
	}													\
	else {												\
		op = &operator_heap[numoperators++];			\
	}

#define FreeOperator(op)

int budParser::EvaluateTokens( budToken* tokens, signed int* intvalue, double* floatvalue, int integer )
{
	operator_t* o, *firstoperator, *lastoperator;
	value_t* v, *firstvalue, *lastvalue, *v1, *v2;
	budToken* t;
	int brace = 0;
	int parentheses = 0;
	int error = 0;
	int lastwasvalue = 0;
	int negativevalue = 0;
	int questmarkintvalue = 0;
	double questmarkfloatvalue = 0;
	int gotquestmarkvalue = false;
	int lastoperatortype = 0;
	//
	operator_t operator_heap[MAX_OPERATORS];
	int numoperators = 0;
	value_t value_heap[MAX_VALUES];
	int numvalues = 0;
	
	firstoperator = lastoperator = NULL;
	firstvalue = lastvalue = NULL;
	if( intvalue ) *intvalue = 0;
	if( floatvalue ) *floatvalue = 0;
	for( t = tokens; t; t = t->next )
	{
		switch( t->type )
		{
			case TT_NAME:
			{
				if( lastwasvalue || negativevalue )
				{
					budParser::Error( "syntax error in #if/#elif" );
					error = 1;
					break;
				}
				if( ( *t ) != "defined" )
				{
					budParser::Error( "undefined name '%s' in #if/#elif", t->c_str() );
					error = 1;
					break;
				}
				t = t->next;
				if( ( *t ) == "(" )
				{
					brace = true;
					t = t->next;
				}
				if( !t || t->type != TT_NAME )
				{
					budParser::Error( "defined() without name in #if/#elif" );
					error = 1;
					break;
				}
				//v = (value_t *) GetClearedMemory(sizeof(value_t));
				AllocValue( v );
				if( FindHashedDefine( budParser::definehash, t->c_str() ) )
				{
					v->intvalue = 1;
					v->floatvalue = 1;
				}
				else
				{
					v->intvalue = 0;
					v->floatvalue = 0;
				}
				v->parentheses = parentheses;
				v->next = NULL;
				v->prev = lastvalue;
				if( lastvalue ) lastvalue->next = v;
				else firstvalue = v;
				lastvalue = v;
				if( brace )
				{
					t = t->next;
					if( !t || ( *t ) != ")" )
					{
						budParser::Error( "defined missing ) in #if/#elif" );
						error = 1;
						break;
					}
				}
				brace = false;
				// defined() creates a value
				lastwasvalue = 1;
				break;
			}
			case TT_NUMBER:
			{
				if( lastwasvalue )
				{
					budParser::Error( "syntax error in #if/#elif" );
					error = 1;
					break;
				}
				//v = (value_t *) GetClearedMemory(sizeof(value_t));
				AllocValue( v );
				if( negativevalue )
				{
					v->intvalue = - t->GetIntValue();
					v->floatvalue = - t->GetFloatValue();
				}
				else
				{
					v->intvalue = t->GetIntValue();
					v->floatvalue = t->GetFloatValue();
				}
				v->parentheses = parentheses;
				v->next = NULL;
				v->prev = lastvalue;
				if( lastvalue ) lastvalue->next = v;
				else firstvalue = v;
				lastvalue = v;
				//last token was a value
				lastwasvalue = 1;
				//
				negativevalue = 0;
				break;
			}
			case TT_PUNCTUATION:
			{
				if( negativevalue )
				{
					budParser::Error( "misplaced minus sign in #if/#elif" );
					error = 1;
					break;
				}
				if( t->subtype == P_PARENTHESESOPEN )
				{
					parentheses++;
					break;
				}
				else if( t->subtype == P_PARENTHESESCLOSE )
				{
					parentheses--;
					if( parentheses < 0 )
					{
						budParser::Error( "too many ) in #if/#elsif" );
						error = 1;
					}
					break;
				}
				//check for invalid operators on floating point values
				if( !integer )
				{
					if( t->subtype == P_BIN_NOT || t->subtype == P_MOD ||
							t->subtype == P_RSHIFT || t->subtype == P_LSHIFT ||
							t->subtype == P_BIN_AND || t->subtype == P_BIN_OR ||
							t->subtype == P_BIN_XOR )
					{
						budParser::Error( "illigal operator '%s' on floating point operands\n", t->c_str() );
						error = 1;
						break;
					}
				}
				switch( t->subtype )
				{
					case P_LOGIC_NOT:
					case P_BIN_NOT:
					{
						if( lastwasvalue )
						{
							budParser::Error( "! or ~ after value in #if/#elif" );
							error = 1;
							break;
						}
						break;
					}
					case P_INC:
					case P_DEC:
					{
						budParser::Error( "++ or -- used in #if/#elif" );
						break;
					}
					case P_SUB:
					{
						if( !lastwasvalue )
						{
							negativevalue = 1;
							break;
						}
					}
					
					case P_MUL:
					case P_DIV:
					case P_MOD:
					case P_ADD:
					
					case P_LOGIC_AND:
					case P_LOGIC_OR:
					case P_LOGIC_GEQ:
					case P_LOGIC_LEQ:
					case P_LOGIC_EQ:
					case P_LOGIC_UNEQ:
					
					case P_LOGIC_GREATER:
					case P_LOGIC_LESS:
					
					case P_RSHIFT:
					case P_LSHIFT:
					
					case P_BIN_AND:
					case P_BIN_OR:
					case P_BIN_XOR:
					
					case P_COLON:
					case P_QUESTIONMARK:
					{
						if( !lastwasvalue )
						{
							budParser::Error( "operator '%s' after operator in #if/#elif", t->c_str() );
							error = 1;
							break;
						}
						break;
					}
					default:
					{
						budParser::Error( "invalid operator '%s' in #if/#elif", t->c_str() );
						error = 1;
						break;
					}
				}
				if( !error && !negativevalue )
				{
					//o = (operator_t *) GetClearedMemory(sizeof(operator_t));
					AllocOperator( o );
					o->op = t->subtype;
					o->priority = PC_OperatorPriority( t->subtype );
					o->parentheses = parentheses;
					o->next = NULL;
					o->prev = lastoperator;
					if( lastoperator ) lastoperator->next = o;
					else firstoperator = o;
					lastoperator = o;
					lastwasvalue = 0;
				}
				break;
			}
			default:
			{
				budParser::Error( "unknown '%s' in #if/#elif", t->c_str() );
				error = 1;
				break;
			}
		}
		if( error )
		{
			break;
		}
	}
	if( !error )
	{
		if( !lastwasvalue )
		{
			budParser::Error( "trailing operator in #if/#elif" );
			error = 1;
		}
		else if( parentheses )
		{
			budParser::Error( "too many ( in #if/#elif" );
			error = 1;
		}
	}
	//
	gotquestmarkvalue = false;
	questmarkintvalue = 0;
	questmarkfloatvalue = 0;
	//while there are operators
	while( !error && firstoperator )
	{
		v = firstvalue;
		for( o = firstoperator; o->next; o = o->next )
		{
			//if the current operator is nested deeper in parentheses
			//than the next operator
			if( o->parentheses > o->next->parentheses )
			{
				break;
			}
			//if the current and next operator are nested equally deep in parentheses
			if( o->parentheses == o->next->parentheses )
			{
				//if the priority of the current operator is equal or higher
				//than the priority of the next operator
				if( o->priority >= o->next->priority )
				{
					break;
				}
			}
			//if the arity of the operator isn't equal to 1
			if( o->op != P_LOGIC_NOT && o->op != P_BIN_NOT )
			{
				v = v->next;
			}
			//if there's no value or no next value
			if( !v )
			{
				budParser::Error( "mising values in #if/#elif" );
				error = 1;
				break;
			}
		}
		if( error )
		{
			break;
		}
		v1 = v;
		v2 = v->next;
#ifdef DEBUG_EVAL
		if( integer )
		{
			Log_Write( "operator %s, value1 = %d", budParser::scriptstack->getPunctuationFromId( o->op ), v1->intvalue );
			if( v2 ) Log_Write( "value2 = %d", v2->intvalue );
		}
		else
		{
			Log_Write( "operator %s, value1 = %f", budParser::scriptstack->getPunctuationFromId( o->op ), v1->floatvalue );
			if( v2 ) Log_Write( "value2 = %f", v2->floatvalue );
		}
#endif //DEBUG_EVAL
		switch( o->op )
		{
			case P_LOGIC_NOT:
				v1->intvalue = !v1->intvalue;
				v1->floatvalue = !v1->floatvalue;
				break;
			case P_BIN_NOT:
				v1->intvalue = ~v1->intvalue;
				break;
			case P_MUL:
				v1->intvalue *= v2->intvalue;
				v1->floatvalue *= v2->floatvalue;
				break;
			case P_DIV:
				if( !v2->intvalue || !v2->floatvalue )
				{
					budParser::Error( "divide by zero in #if/#elif\n" );
					error = 1;
					break;
				}
				v1->intvalue /= v2->intvalue;
				v1->floatvalue /= v2->floatvalue;
				break;
			case P_MOD:
				if( !v2->intvalue )
				{
					budParser::Error( "divide by zero in #if/#elif\n" );
					error = 1;
					break;
				}
				v1->intvalue %= v2->intvalue;
				break;
			case P_ADD:
				v1->intvalue += v2->intvalue;
				v1->floatvalue += v2->floatvalue;
				break;
			case P_SUB:
				v1->intvalue -= v2->intvalue;
				v1->floatvalue -= v2->floatvalue;
				break;
			case P_LOGIC_AND:
				v1->intvalue = v1->intvalue && v2->intvalue;
				v1->floatvalue = v1->floatvalue && v2->floatvalue;
				break;
			case P_LOGIC_OR:
				v1->intvalue = v1->intvalue || v2->intvalue;
				v1->floatvalue = v1->floatvalue || v2->floatvalue;
				break;
			case P_LOGIC_GEQ:
				v1->intvalue = v1->intvalue >= v2->intvalue;
				v1->floatvalue = v1->floatvalue >= v2->floatvalue;
				break;
			case P_LOGIC_LEQ:
				v1->intvalue = v1->intvalue <= v2->intvalue;
				v1->floatvalue = v1->floatvalue <= v2->floatvalue;
				break;
			case P_LOGIC_EQ:
				v1->intvalue = v1->intvalue == v2->intvalue;
				v1->floatvalue = v1->floatvalue == v2->floatvalue;
				break;
			case P_LOGIC_UNEQ:
				v1->intvalue = v1->intvalue != v2->intvalue;
				v1->floatvalue = v1->floatvalue != v2->floatvalue;
				break;
			case P_LOGIC_GREATER:
				v1->intvalue = v1->intvalue > v2->intvalue;
				v1->floatvalue = v1->floatvalue > v2->floatvalue;
				break;
			case P_LOGIC_LESS:
				v1->intvalue = v1->intvalue < v2->intvalue;
				v1->floatvalue = v1->floatvalue < v2->floatvalue;
				break;
			case P_RSHIFT:
				v1->intvalue >>= v2->intvalue;
				break;
			case P_LSHIFT:
				v1->intvalue <<= v2->intvalue;
				break;
			case P_BIN_AND:
				v1->intvalue &= v2->intvalue;
				break;
			case P_BIN_OR:
				v1->intvalue |= v2->intvalue;
				break;
			case P_BIN_XOR:
				v1->intvalue ^= v2->intvalue;
				break;
			case P_COLON:
			{
				if( !gotquestmarkvalue )
				{
					budParser::Error( ": without ? in #if/#elif" );
					error = 1;
					break;
				}
				if( integer )
				{
					if( !questmarkintvalue )
						v1->intvalue = v2->intvalue;
				}
				else
				{
					if( !questmarkfloatvalue )
						v1->floatvalue = v2->floatvalue;
				}
				gotquestmarkvalue = false;
				break;
			}
			case P_QUESTIONMARK:
			{
				if( gotquestmarkvalue )
				{
					budParser::Error( "? after ? in #if/#elif" );
					error = 1;
					break;
				}
				questmarkintvalue = v1->intvalue;
				questmarkfloatvalue = v1->floatvalue;
				gotquestmarkvalue = true;
				break;
			}
		}
#ifdef DEBUG_EVAL
		if( integer ) Log_Write( "result value = %d", v1->intvalue );
		else Log_Write( "result value = %f", v1->floatvalue );
#endif //DEBUG_EVAL
		if( error )
			break;
		lastoperatortype = o->op;
		//if not an operator with arity 1
		if( o->op != P_LOGIC_NOT && o->op != P_BIN_NOT )
		{
			//remove the second value if not question mark operator
			if( o->op != P_QUESTIONMARK )
			{
				v = v->next;
			}
			//
			if( v->prev ) v->prev->next = v->next;
			else firstvalue = v->next;
			if( v->next ) v->next->prev = v->prev;
			else lastvalue = v->prev;
			//FreeMemory(v);
			FreeValue( v );
		}
		//remove the operator
		if( o->prev ) o->prev->next = o->next;
		else firstoperator = o->next;
		if( o->next ) o->next->prev = o->prev;
		else lastoperator = o->prev;
		//FreeMemory(o);
		FreeOperator( o );
	}
	if( firstvalue )
	{
		if( intvalue ) *intvalue = firstvalue->intvalue;
		if( floatvalue ) *floatvalue = firstvalue->floatvalue;
	}
	for( o = firstoperator; o; o = lastoperator )
	{
		lastoperator = o->next;
		//FreeMemory(o);
		FreeOperator( o );
	}
	for( v = firstvalue; v; v = lastvalue )
	{
		lastvalue = v->next;
		//FreeMemory(v);
		FreeValue( v );
	}
	if( !error )
	{
		return true;
	}
	if( intvalue )
	{
		*intvalue = 0;
	}
	if( floatvalue )
	{
		*floatvalue = 0;
	}
	return false;
}

/*
================
budParser::Evaluate
================
*/
int budParser::Evaluate( signed int* intvalue, double* floatvalue, int integer )
{
	budToken token, *firsttoken, *lasttoken;
	budToken* t, *nexttoken;
	define_t* define;
	int defined = false;
	
	if( intvalue )
	{
		*intvalue = 0;
	}
	if( floatvalue )
	{
		*floatvalue = 0;
	}
	//
	if( !budParser::ReadLine( &token ) )
	{
		budParser::Error( "no value after #if/#elif" );
		return false;
	}
	firsttoken = NULL;
	lasttoken = NULL;
	do
	{
		//if the token is a name
		if( token.type == TT_NAME )
		{
			if( defined )
			{
				defined = false;
				t = new( TAG_LIBBUD_PARSER ) budToken( token );
				t->next = NULL;
				if( lasttoken ) lasttoken->next = t;
				else firsttoken = t;
				lasttoken = t;
			}
			else if( token == "defined" )
			{
				defined = true;
				t = new( TAG_LIBBUD_PARSER ) budToken( token );
				t->next = NULL;
				if( lasttoken ) lasttoken->next = t;
				else firsttoken = t;
				lasttoken = t;
			}
			else
			{
				//then it must be a define
				define = FindHashedDefine( budParser::definehash, token.c_str() );
				if( !define )
				{
					budParser::Error( "can't Evaluate '%s', not defined", token.c_str() );
					return false;
				}
				if( !budParser::ExpandDefineIntoSource( &token, define ) )
				{
					return false;
				}
			}
		}
		//if the token is a number or a punctuation
		else if( token.type == TT_NUMBER || token.type == TT_PUNCTUATION )
		{
			t = new( TAG_LIBBUD_PARSER ) budToken( token );
			t->next = NULL;
			if( lasttoken ) lasttoken->next = t;
			else firsttoken = t;
			lasttoken = t;
		}
		else
		{
			budParser::Error( "can't Evaluate '%s'", token.c_str() );
			return false;
		}
	}
	while( budParser::ReadLine( &token ) );
	//
	if( !budParser::EvaluateTokens( firsttoken, intvalue, floatvalue, integer ) )
	{
		return false;
	}
	//
#ifdef DEBUG_EVAL
	Log_Write( "eval:" );
#endif //DEBUG_EVAL
	for( t = firsttoken; t; t = nexttoken )
	{
#ifdef DEBUG_EVAL
		Log_Write( " %s", t->c_str() );
#endif //DEBUG_EVAL
		nexttoken = t->next;
		delete t;
	} //end for
#ifdef DEBUG_EVAL
	if( integer ) Log_Write( "eval result: %d", *intvalue );
	else Log_Write( "eval result: %f", *floatvalue );
#endif //DEBUG_EVAL
	//
	return true;
}

/*
================
budParser::DollarEvaluate
================
*/
int budParser::DollarEvaluate( signed int* intvalue, double* floatvalue, int integer )
{
	int indent, defined = false;
	budToken token, *firsttoken, *lasttoken;
	budToken* t, *nexttoken;
	define_t* define;
	
	if( intvalue )
	{
		*intvalue = 0;
	}
	if( floatvalue )
	{
		*floatvalue = 0;
	}
	//
	if( !budParser::ReadSourceToken( &token ) )
	{
		budParser::Error( "no leading ( after $evalint/$evalfloat" );
		return false;
	}
	if( !budParser::ReadSourceToken( &token ) )
	{
		budParser::Error( "nothing to Evaluate" );
		return false;
	}
	indent = 1;
	firsttoken = NULL;
	lasttoken = NULL;
	do
	{
		//if the token is a name
		if( token.type == TT_NAME )
		{
			if( defined )
			{
				defined = false;
				t = new( TAG_LIBBUD_PARSER ) budToken( token );
				t->next = NULL;
				if( lasttoken ) lasttoken->next = t;
				else firsttoken = t;
				lasttoken = t;
			}
			else if( token == "defined" )
			{
				defined = true;
				t = new( TAG_LIBBUD_PARSER ) budToken( token );
				t->next = NULL;
				if( lasttoken ) lasttoken->next = t;
				else firsttoken = t;
				lasttoken = t;
			}
			else
			{
				//then it must be a define
				define = FindHashedDefine( budParser::definehash, token.c_str() );
				if( !define )
				{
					budParser::Warning( "can't Evaluate '%s', not defined", token.c_str() );
					return false;
				}
				if( !budParser::ExpandDefineIntoSource( &token, define ) )
				{
					return false;
				}
			}
		}
		//if the token is a number or a punctuation
		else if( token.type == TT_NUMBER || token.type == TT_PUNCTUATION )
		{
			if( token[0] == '(' ) indent++;
			else if( token[0] == ')' ) indent--;
			if( indent <= 0 )
			{
				break;
			}
			t = new( TAG_LIBBUD_PARSER ) budToken( token );
			t->next = NULL;
			if( lasttoken ) lasttoken->next = t;
			else firsttoken = t;
			lasttoken = t;
		}
		else
		{
			budParser::Error( "can't Evaluate '%s'", token.c_str() );
			return false;
		}
	}
	while( budParser::ReadSourceToken( &token ) );
	//
	if( !budParser::EvaluateTokens( firsttoken, intvalue, floatvalue, integer ) )
	{
		return false;
	}
	//
#ifdef DEBUG_EVAL
	Log_Write( "$eval:" );
#endif //DEBUG_EVAL
	for( t = firsttoken; t; t = nexttoken )
	{
#ifdef DEBUG_EVAL
		Log_Write( " %s", t->c_str() );
#endif //DEBUG_EVAL
		nexttoken = t->next;
		delete t;
	} //end for
#ifdef DEBUG_EVAL
	if( integer ) Log_Write( "$eval result: %d", *intvalue );
	else Log_Write( "$eval result: %f", *floatvalue );
#endif //DEBUG_EVAL
	//
	return true;
}

/*
================
budParser::Directive_elif
================
*/
int budParser::Directive_elif()
{
	signed int value; // DG: use int instead of long for 64bit compatibility
	int type, skip;
	
	budParser::PopIndent( &type, &skip );
	if( !type || type == INDENT_ELSE )
	{
		budParser::Error( "misplaced #elif" );
		return false;
	}
	if( !budParser::Evaluate( &value, NULL, true ) )
	{
		return false;
	}
	skip = ( value == 0 );
	budParser::PushIndent( INDENT_ELIF, skip );
	return true;
}

/*
================
budParser::Directive_if
================
*/
int budParser::Directive_if()
{
	signed int value; // DG: use int instead of long for 64bit compatibility
	int skip;
	
	if( !budParser::Evaluate( &value, NULL, true ) )
	{
		return false;
	}
	skip = ( value == 0 );
	budParser::PushIndent( INDENT_IF, skip );
	return true;
}

/*
================
budParser::Directive_line
================
*/
int budParser::Directive_line()
{
	budToken token;
	
	budParser::Error( "#line directive not supported" );
	while( budParser::ReadLine( &token ) )
	{
	}
	return true;
}

/*
================
budParser::Directive_error
================
*/
int budParser::Directive_error()
{
	budToken token;
	
	if( !budParser::ReadLine( &token ) || token.type != TT_STRING )
	{
		budParser::Error( "#error without string" );
		return false;
	}
	budParser::Error( "#error: %s", token.c_str() );
	return true;
}

/*
================
budParser::Directive_warning
================
*/
int budParser::Directive_warning()
{
	budToken token;
	
	if( !budParser::ReadLine( &token ) || token.type != TT_STRING )
	{
		budParser::Warning( "#warning without string" );
		return false;
	}
	budParser::Warning( "#warning: %s", token.c_str() );
	return true;
}

/*
================
budParser::Directive_pragma
================
*/
int budParser::Directive_pragma()
{
	budToken token;
	
	budParser::Warning( "#pragma directive not supported" );
	while( budParser::ReadLine( &token ) )
	{
	}
	return true;
}

/*
================
budParser::UnreadSignToken
================
*/
void budParser::UnreadSignToken()
{
	budToken token;
	
	token.line = budParser::scriptstack->GetLineNum();
	token.whiteSpaceStart_p = NULL;
	token.whiteSpaceEnd_p = NULL;
	token.linesCrossed = 0;
	token.flags = 0;
	token = "-";
	token.type = TT_PUNCTUATION;
	token.subtype = P_SUB;
	budParser::UnreadSourceToken( &token );
}

/*
================
budParser::Directive_eval
================
*/
int budParser::Directive_eval()
{
	signed int value; // DG: use int instead of long for 64bit compatibility
	budToken token;
	char buf[128];
	
	if( !budParser::Evaluate( &value, NULL, true ) )
	{
		return false;
	}
	
	token.line = budParser::scriptstack->GetLineNum();
	token.whiteSpaceStart_p = NULL;
	token.whiteSpaceEnd_p = NULL;
	token.linesCrossed = 0;
	token.flags = 0;
	sprintf( buf, "%d", abs( value ) );
	token = buf;
	token.type = TT_NUMBER;
	token.subtype = TT_INTEGER | TT_LONG | TT_DECIMAL;
	budParser::UnreadSourceToken( &token );
	if( value < 0 )
	{
		budParser::UnreadSignToken();
	}
	return true;
}

/*
================
budParser::Directive_evalfloat
================
*/
int budParser::Directive_evalfloat()
{
	double value;
	budToken token;
	char buf[128];
	
	if( !budParser::Evaluate( NULL, &value, false ) )
	{
		return false;
	}
	
	token.line = budParser::scriptstack->GetLineNum();
	token.whiteSpaceStart_p = NULL;
	token.whiteSpaceEnd_p = NULL;
	token.linesCrossed = 0;
	token.flags = 0;
	sprintf( buf, "%1.2f", budMath::Fabs( value ) );
	token = buf;
	token.type = TT_NUMBER;
	token.subtype = TT_FLOAT | TT_LONG | TT_DECIMAL;
	budParser::UnreadSourceToken( &token );
	if( value < 0 )
	{
		budParser::UnreadSignToken();
	}
	return true;
}

/*
================
budParser::ReadDirective
================
*/
int budParser::ReadDirective()
{
	budToken token;
	
	//read the directive name
	if( !budParser::ReadSourceToken( &token ) )
	{
		budParser::Error( "found '#' without name" );
		return false;
	}
	//directive name must be on the same line
	if( token.linesCrossed > 0 )
	{
		budParser::UnreadSourceToken( &token );
		budParser::Error( "found '#' at end of line" );
		return false;
	}
	//if if is a name
	if( token.type == TT_NAME )
	{
		if( token == "if" )
		{
			return budParser::Directive_if();
		}
		else if( token == "ifdef" )
		{
			return budParser::Directive_ifdef();
		}
		else if( token == "ifndef" )
		{
			return budParser::Directive_ifndef();
		}
		else if( token == "elif" )
		{
			return budParser::Directive_elif();
		}
		else if( token == "else" )
		{
			return budParser::Directive_else();
		}
		else if( token == "endif" )
		{
			return budParser::Directive_endif();
		}
		else if( budParser::skip > 0 )
		{
			// skip the rest of the line
			while( budParser::ReadLine( &token ) )
			{
			}
			return true;
		}
		else
		{
			if( token == "include" )
			{
				// RB lets override for embedded shaders
				budToken filename;
				return Directive_include( &filename );
				// RB end
			}
			else if( token == "define" )
			{
				return budParser::Directive_define();
			}
			else if( token == "undef" )
			{
				return budParser::Directive_undef();
			}
			else if( token == "line" )
			{
				return budParser::Directive_line();
			}
			else if( token == "error" )
			{
				return budParser::Directive_error();
			}
			else if( token == "warning" )
			{
				return budParser::Directive_warning();
			}
			else if( token == "pragma" )
			{
				return budParser::Directive_pragma();
			}
			else if( token == "eval" )
			{
				return budParser::Directive_eval();
			}
			else if( token == "evalfloat" )
			{
				return budParser::Directive_evalfloat();
			}
		}
	}
	budParser::Error( "unknown precompiler directive '%s'", token.c_str() );
	return false;
}

/*
================
budParser::DollarDirective_evalint
================
*/
int budParser::DollarDirective_evalint()
{
	signed int value; // DG: use int instead of long for 64bit compatibility
	budToken token;
	char buf[128];
	
	if( !budParser::DollarEvaluate( &value, NULL, true ) )
	{
		return false;
	}
	
	token.line = budParser::scriptstack->GetLineNum();
	token.whiteSpaceStart_p = NULL;
	token.whiteSpaceEnd_p = NULL;
	token.linesCrossed = 0;
	token.flags = 0;
	sprintf( buf, "%d", abs( value ) );
	token = buf;
	token.type = TT_NUMBER;
	token.subtype = TT_INTEGER | TT_LONG | TT_DECIMAL | TT_VALUESVALID;
	token.intvalue = abs( value );
	token.floatvalue = abs( value );
	budParser::UnreadSourceToken( &token );
	if( value < 0 )
	{
		budParser::UnreadSignToken();
	}
	return true;
}

/*
================
budParser::DollarDirective_evalfloat
================
*/
int budParser::DollarDirective_evalfloat()
{
	double value;
	budToken token;
	char buf[128];
	
	if( !budParser::DollarEvaluate( NULL, &value, false ) )
	{
		return false;
	}
	
	token.line = budParser::scriptstack->GetLineNum();
	token.whiteSpaceStart_p = NULL;
	token.whiteSpaceEnd_p = NULL;
	token.linesCrossed = 0;
	token.flags = 0;
	sprintf( buf, "%1.2f", fabs( value ) );
	token = buf;
	token.type = TT_NUMBER;
	token.subtype = TT_FLOAT | TT_LONG | TT_DECIMAL | TT_VALUESVALID;
	token.intvalue = ( unsigned int ) fabs( value ); // DG: use int instead of long for 64bit compatibility
	token.floatvalue = fabs( value );
	budParser::UnreadSourceToken( &token );
	if( value < 0 )
	{
		budParser::UnreadSignToken();
	}
	return true;
}

/*
================
budParser::ReadDollarDirective
================
*/
int budParser::ReadDollarDirective()
{
	budToken token;
	
	// read the directive name
	if( !budParser::ReadSourceToken( &token ) )
	{
		budParser::Error( "found '$' without name" );
		return false;
	}
	// directive name must be on the same line
	if( token.linesCrossed > 0 )
	{
		budParser::UnreadSourceToken( &token );
		budParser::Error( "found '$' at end of line" );
		return false;
	}
	// if if is a name
	if( token.type == TT_NAME )
	{
		if( token == "evalint" )
		{
			return budParser::DollarDirective_evalint();
		}
		else if( token == "evalfloat" )
		{
			return budParser::DollarDirective_evalfloat();
		}
	}
	budParser::UnreadSourceToken( &token );
	return false;
}

/*
================
budParser::ReadToken
================
*/
int budParser::ReadToken( budToken* token )
{
	define_t* define;
	
	while( 1 )
	{
		if( !budParser::ReadSourceToken( token ) )
		{
			return false;
		}
		// check for precompiler directives
		if( token->type == TT_PUNCTUATION && ( *token )[0] == '#' && ( *token )[1] == '\0' )
		{
			// read the precompiler directive
			if( !budParser::ReadDirective() )
			{
				return false;
			}
			continue;
		}
		// if skipping source because of conditional compilation
		if( budParser::skip )
		{
			continue;
		}
		// recursively concatenate strings that are behind each other still resolving defines
		if( token->type == TT_STRING && !( budParser::scriptstack->GetFlags() & LEXFL_NOSTRINGCONCAT ) )
		{
			budToken newtoken;
			if( budParser::ReadToken( &newtoken ) )
			{
				if( newtoken.type == TT_STRING )
				{
					token->Append( newtoken.c_str() );
				}
				else
				{
					budParser::UnreadSourceToken( &newtoken );
				}
			}
		}
		//
		if( !( budParser::scriptstack->GetFlags() & LEXFL_NODOLLARPRECOMPILE ) )
		{
			// check for special precompiler directives
			if( token->type == TT_PUNCTUATION && ( *token )[0] == '$' && ( *token )[1] == '\0' )
			{
				// read the precompiler directive
				if( budParser::ReadDollarDirective() )
				{
					continue;
				}
			}
		}
		// if the token is a name
		if( token->type == TT_NAME && !( token->flags & TOKEN_FL_RECURSIVE_DEFINE ) )
		{
			// check if the name is a define macro
			define = FindHashedDefine( budParser::definehash, token->c_str() );
			// if it is a define macro
			if( define )
			{
				// expand the defined macro
				if( !budParser::ExpandDefineIntoSource( token, define ) )
				{
					return false;
				}
				continue;
			}
		}
		// found a token
		return true;
	}
}

/*
================
budParser::ExpectTokenString
================
*/
int budParser::ExpectTokenString( const char* string )
{
	budToken token;
	
	if( !budParser::ReadToken( &token ) )
	{
		budParser::Error( "couldn't find expected '%s'", string );
		return false;
	}
	
	if( token != string )
	{
		budParser::Error( "expected '%s' but found '%s'", string, token.c_str() );
		return false;
	}
	return true;
}

/*
================
budParser::ExpectTokenType
================
*/
int budParser::ExpectTokenType( int type, int subtype, budToken* token )
{
	budStr str;
	
	if( !budParser::ReadToken( token ) )
	{
		budParser::Error( "couldn't read expected token" );
		return 0;
	}
	
	if( token->type != type )
	{
		switch( type )
		{
			case TT_STRING:
				str = "string";
				break;
			case TT_LITERAL:
				str = "literal";
				break;
			case TT_NUMBER:
				str = "number";
				break;
			case TT_NAME:
				str = "name";
				break;
			case TT_PUNCTUATION:
				str = "punctuation";
				break;
			default:
				str = "unknown type";
				break;
		}
		budParser::Error( "expected a %s but found '%s'", str.c_str(), token->c_str() );
		return 0;
	}
	if( token->type == TT_NUMBER )
	{
		if( ( token->subtype & subtype ) != subtype )
		{
			str.Clear();
			if( subtype & TT_DECIMAL ) str = "decimal ";
			if( subtype & TT_HEX ) str = "hex ";
			if( subtype & TT_OCTAL ) str = "octal ";
			if( subtype & TT_BINARY ) str = "binary ";
			if( subtype & TT_UNSIGNED ) str += "unsigned ";
			if( subtype & TT_LONG ) str += "long ";
			if( subtype & TT_FLOAT ) str += "float ";
			if( subtype & TT_INTEGER ) str += "integer ";
			str.StripTrailing( ' ' );
			budParser::Error( "expected %s but found '%s'", str.c_str(), token->c_str() );
			return 0;
		}
	}
	else if( token->type == TT_PUNCTUATION )
	{
		if( subtype < 0 )
		{
			budParser::Error( "BUG: wrong punctuation subtype" );
			return 0;
		}
		if( token->subtype != subtype )
		{
			budParser::Error( "expected '%s' but found '%s'", scriptstack->GetPunctuationFromId( subtype ), token->c_str() );
			return 0;
		}
	}
	return 1;
}

/*
================
budParser::ExpectAnyToken
================
*/
int budParser::ExpectAnyToken( budToken* token )
{
	if( !budParser::ReadToken( token ) )
	{
		budParser::Error( "couldn't read expected token" );
		return false;
	}
	else
	{
		return true;
	}
}

/*
================
budParser::CheckTokenString
================
*/
int budParser::CheckTokenString( const char* string )
{
	budToken tok;
	
	if( !ReadToken( &tok ) )
	{
		return false;
	}
	//if the token is available
	if( tok == string )
	{
		return true;
	}
	
	UnreadSourceToken( &tok );
	return false;
}

/*
================
budParser::CheckTokenType
================
*/
int budParser::CheckTokenType( int type, int subtype, budToken* token )
{
	budToken tok;
	
	if( !ReadToken( &tok ) )
	{
		return false;
	}
	//if the type matches
	if( tok.type == type && ( tok.subtype & subtype ) == subtype )
	{
		*token = tok;
		return true;
	}
	
	UnreadSourceToken( &tok );
	return false;
}

/*
================
budParser::PeekTokenString
================
*/
int budParser::PeekTokenString( const char* string )
{
	budToken tok;
	
	if( !ReadToken( &tok ) )
	{
		return false;
	}
	
	UnreadSourceToken( &tok );
	
	// if the token is available
	if( tok == string )
	{
		return true;
	}
	return false;
}

/*
================
budParser::PeekTokenType
================
*/
int budParser::PeekTokenType( int type, int subtype, budToken* token )
{
	budToken tok;
	
	if( !ReadToken( &tok ) )
	{
		return false;
	}
	
	UnreadSourceToken( &tok );
	
	// if the type matches
	if( tok.type == type && ( tok.subtype & subtype ) == subtype )
	{
		*token = tok;
		return true;
	}
	return false;
}

/*
================
budParser::SkipUntilString
================
*/
int budParser::SkipUntilString( const char* string )
{
	budToken token;
	
	while( budParser::ReadToken( &token ) )
	{
		if( token == string )
		{
			return true;
		}
	}
	return false;
}

/*
================
budParser::SkipRestOfLine
================
*/
int budParser::SkipRestOfLine()
{
	budToken token;
	
	while( budParser::ReadToken( &token ) )
	{
		if( token.linesCrossed )
		{
			budParser::UnreadSourceToken( &token );
			return true;
		}
	}
	return false;
}

/*
=================
budParser::SkipBracedSection

Skips until a matching close brace is found.
Internal brace depths are properly skipped.
=================
*/
int budParser::SkipBracedSection( bool parseFirstBrace )
{
	budToken token;
	int depth;
	
	depth = parseFirstBrace ? 0 : 1;
	do
	{
		if( !ReadToken( &token ) )
		{
			return false;
		}
		if( token.type == TT_PUNCTUATION )
		{
			if( token == "{" )
			{
				depth++;
			}
			else if( token == "}" )
			{
				depth--;
			}
		}
	}
	while( depth );
	return true;
}

/*
=================
budParser::ParseBracedSectionExact

The next token should be an open brace.
Parses until a matching close brace is found.
Maintains the exact formating of the braced section

  FIXME: what about precompilation ?
=================
*/
const char* budParser::ParseBracedSectionExact( budStr& out, int tabs )
{
	return scriptstack->ParseBracedSectionExact( out, tabs );
}


/*
========================
budParser::ParseBracedSection

The next token should be an open brace. Parses until a matching close brace is found. Internal
brace depths are properly skipped.
========================
*/
const char* budParser::ParseBracedSection( budStr& out, int tabs, bool parseFirstBrace, char intro, char outro )
{
	budToken token;
	int i, depth;
	bool doTabs;
	
	char temp[ 2 ] = { 0, 0 };
	*temp = intro;
	
	out.Empty();
	if( parseFirstBrace )
	{
		if( !ExpectTokenString( temp ) )
		{
			return out.c_str();
		}
		out = temp;
	}
	depth = 1;
	doTabs = ( tabs >= 0 );
	do
	{
		if( !ReadToken( &token ) )
		{
			Error( "missing closing brace" );
			return out.c_str();
		}
		
		// if the token is on a new line
		for( i = 0; i < token.linesCrossed; i++ )
		{
			out += "\r\n";
		}
		
		if( doTabs && token.linesCrossed )
		{
			i = tabs;
			if( token[ 0 ] == outro && i > 0 )
			{
				i--;
			}
			while( i-- > 0 )
			{
				out += "\t";
			}
		}
		if( token.type == TT_STRING )
		{
			out += "\"" + token + "\"";
		}
		else if( token.type == TT_LITERAL )
		{
			out += "\'" + token + "\'";
		}
		else
		{
			if( token[ 0 ] == intro )
			{
				depth++;
				if( doTabs )
				{
					tabs++;
				}
			}
			else if( token[ 0 ] == outro )
			{
				depth--;
				if( doTabs )
				{
					tabs--;
				}
			}
			out += token;
		}
		out += " ";
	}
	while( depth );
	
	return out.c_str();
}

/*
=================
budParser::ParseRestOfLine

  parse the rest of the line
=================
*/
const char* budParser::ParseRestOfLine( budStr& out )
{
	budToken token;
	
	out.Empty();
	while( budParser::ReadToken( &token ) )
	{
		if( token.linesCrossed )
		{
			budParser::UnreadSourceToken( &token );
			break;
		}
		if( out.Length() )
		{
			out += " ";
		}
		out += token;
	}
	return out.c_str();
}

/*
================
budParser::UnreadToken
================
*/
void budParser::UnreadToken( budToken* token )
{
	budParser::UnreadSourceToken( token );
}

/*
================
budParser::ReadTokenOnLine
================
*/
int budParser::ReadTokenOnLine( budToken* token )
{
	budToken tok;
	
	if( !budParser::ReadToken( &tok ) )
	{
		return false;
	}
	// if no lines were crossed before this token
	if( !tok.linesCrossed )
	{
		*token = tok;
		return true;
	}
	//
	budParser::UnreadSourceToken( &tok );
	return false;
}

/*
================
budParser::ParseInt
================
*/
int budParser::ParseInt()
{
	budToken token;
	
	if( !budParser::ReadToken( &token ) )
	{
		budParser::Error( "couldn't read expected integer" );
		return 0;
	}
	if( token.type == TT_PUNCTUATION && token == "-" )
	{
		budParser::ExpectTokenType( TT_NUMBER, TT_INTEGER, &token );
		return -( ( signed int ) token.GetIntValue() );
	}
	else if( token.type != TT_NUMBER || token.subtype == TT_FLOAT )
	{
		budParser::Error( "expected integer value, found '%s'", token.c_str() );
	}
	return token.GetIntValue();
}

/*
================
budParser::ParseBool
================
*/
bool budParser::ParseBool()
{
	budToken token;
	
	if( !budParser::ExpectTokenType( TT_NUMBER, 0, &token ) )
	{
		budParser::Error( "couldn't read expected boolean" );
		return false;
	}
	return ( token.GetIntValue() != 0 );
}

/*
================
budParser::ParseFloat
================
*/
float budParser::ParseFloat()
{
	budToken token;
	
	if( !budParser::ReadToken( &token ) )
	{
		budParser::Error( "couldn't read expected floating point number" );
		return 0.0f;
	}
	if( token.type == TT_PUNCTUATION && token == "-" )
	{
		budParser::ExpectTokenType( TT_NUMBER, 0, &token );
		return -token.GetFloatValue();
	}
	else if( token.type != TT_NUMBER )
	{
		budParser::Error( "expected float value, found '%s'", token.c_str() );
	}
	return token.GetFloatValue();
}

/*
================
budParser::Parse1DMatrix
================
*/
int budParser::Parse1DMatrix( int x, float* m )
{
	int i;
	
	if( !budParser::ExpectTokenString( "(" ) )
	{
		return false;
	}
	
	for( i = 0; i < x; i++ )
	{
		m[i] = budParser::ParseFloat();
	}
	
	if( !budParser::ExpectTokenString( ")" ) )
	{
		return false;
	}
	return true;
}

/*
================
budParser::Parse2DMatrix
================
*/
int budParser::Parse2DMatrix( int y, int x, float* m )
{
	int i;
	
	if( !budParser::ExpectTokenString( "(" ) )
	{
		return false;
	}
	
	for( i = 0; i < y; i++ )
	{
		if( !budParser::Parse1DMatrix( x, m + i * x ) )
		{
			return false;
		}
	}
	
	if( !budParser::ExpectTokenString( ")" ) )
	{
		return false;
	}
	return true;
}

/*
================
budParser::Parse3DMatrix
================
*/
int budParser::Parse3DMatrix( int z, int y, int x, float* m )
{
	int i;
	
	if( !budParser::ExpectTokenString( "(" ) )
	{
		return false;
	}
	
	for( i = 0 ; i < z; i++ )
	{
		if( !budParser::Parse2DMatrix( y, x, m + i * x * y ) )
		{
			return false;
		}
	}
	
	if( !budParser::ExpectTokenString( ")" ) )
	{
		return false;
	}
	return true;
}

/*
================
budParser::GetLastWhiteSpace
================
*/
int budParser::GetLastWhiteSpace( budStr& whiteSpace ) const
{
	if( scriptstack )
	{
		scriptstack->GetLastWhiteSpace( whiteSpace );
	}
	else
	{
		whiteSpace.Clear();
	}
	return whiteSpace.Length();
}

/*
================
budParser::SetMarker
================
*/
void budParser::SetMarker()
{
	marker_p = NULL;
}

/*
================
budParser::GetStringFromMarker

  FIXME: this is very bad code, the script isn't even garrenteed to still be around
================
*/
void budParser::GetStringFromMarker( budStr& out, bool clean )
{
	char*	p;
	char	save;
	
	if( marker_p == NULL )
	{
		marker_p = scriptstack->buffer;
	}
	
	if( tokens )
	{
		p = ( char* )tokens->whiteSpaceStart_p;
	}
	else
	{
		p = ( char* )scriptstack->script_p;
	}
	
	// Set the end character to NULL to give us a complete string
	save = *p;
	*p = 0;
	
	// If cleaning then reparse
	if( clean )
	{
		budParser temp( marker_p, strlen( marker_p ), "temp", flags );
		budToken token;
		while( temp.ReadToken( &token ) )
		{
			out += token;
		}
	}
	else
	{
		out = marker_p;
	}
	
	// restore the character we set to NULL
	*p = save;
}

/*
================
budParser::SetIncludePath
================
*/
void budParser::SetIncludePath( const char* path )
{
	budParser::includepath = path;
	// add trailing path seperator
	if( budParser::includepath[budParser::includepath.Length() - 1] != '\\' &&
			budParser::includepath[budParser::includepath.Length() - 1] != '/' )
	{
		budParser::includepath += PATHSEPARATOR_STR;
	}
}

/*
================
budParser::SetPunctuations
================
*/
void budParser::SetPunctuations( const punctuation_t* p )
{
	budParser::punctuations = p;
}

/*
================
budParser::SetFlags
================
*/
void budParser::SetFlags( int flags )
{
	budLexer* s;
	
	budParser::flags = flags;
	for( s = budParser::scriptstack; s; s = s->next )
	{
		s->SetFlags( flags );
	}
}

/*
================
budParser::GetFlags
================
*/
int budParser::GetFlags() const
{
	return budParser::flags;
}

/*
================
budParser::LoadFile
================
*/
int budParser::LoadFile( const char* filename, bool OSPath )
{
	budLexer* script;
	
	if( budParser::loaded )
	{
		libBud::common->FatalError( "budParser::loadFile: another source already loaded" );
		return false;
	}
	script = new( TAG_LIBBUD_PARSER ) budLexer( filename, 0, OSPath );
	if( !script->IsLoaded() )
	{
		delete script;
		return false;
	}
	script->SetFlags( budParser::flags );
	script->SetPunctuations( budParser::punctuations );
	script->next = NULL;
	budParser::OSPath = OSPath;
	budParser::filename = filename;
	budParser::scriptstack = script;
	budParser::tokens = NULL;
	budParser::indentstack = NULL;
	budParser::skip = 0;
	budParser::loaded = true;
	
	if( !budParser::definehash )
	{
		budParser::defines = NULL;
		budParser::definehash = ( define_t** ) Mem_ClearedAlloc( DEFINEHASHSIZE * sizeof( define_t* ), TAG_LIBBUD_PARSER );
		budParser::AddGlobalDefinesToSource();
	}
	return true;
}

/*
================
budParser::LoadMemory
================
*/
int budParser::LoadMemory( const char* ptr, int length, const char* name )
{
	budLexer* script;
	
	if( budParser::loaded )
	{
		libBud::common->FatalError( "budParser::loadMemory: another source already loaded" );
		return false;
	}
	script = new( TAG_LIBBUD_PARSER ) budLexer( ptr, length, name );
	if( !script->IsLoaded() )
	{
		delete script;
		return false;
	}
	script->SetFlags( budParser::flags );
	script->SetPunctuations( budParser::punctuations );
	script->next = NULL;
	budParser::filename = name;
	budParser::scriptstack = script;
	budParser::tokens = NULL;
	budParser::indentstack = NULL;
	budParser::skip = 0;
	budParser::loaded = true;
	
	if( !budParser::definehash )
	{
		budParser::defines = NULL;
		budParser::definehash = ( define_t** ) Mem_ClearedAlloc( DEFINEHASHSIZE * sizeof( define_t* ), TAG_LIBBUD_PARSER );
		budParser::AddGlobalDefinesToSource();
	}
	return true;
}

/*
================
budParser::FreeSource
================
*/
void budParser::FreeSource( bool keepDefines )
{
	budLexer* script;
	budToken* token;
	define_t* define;
	indent_t* indent;
	int i;
	
	// free all the scripts
	while( scriptstack )
	{
		script = scriptstack;
		scriptstack = scriptstack->next;
		delete script;
	}
	// free all the tokens
	while( tokens )
	{
		token = tokens;
		tokens = tokens->next;
		delete token;
	}
	// free all indents
	while( indentstack )
	{
		indent = indentstack;
		indentstack = indentstack->next;
		Mem_Free( indent );
	}
	if( !keepDefines )
	{
		// free hash table
		if( definehash )
		{
			// free defines
			for( i = 0; i < DEFINEHASHSIZE; i++ )
			{
				while( definehash[i] )
				{
					define = definehash[i];
					definehash[i] = definehash[i]->hashnext;
					FreeDefine( define );
				}
			}
			defines = NULL;
			Mem_Free( budParser::definehash );
			definehash = NULL;
		}
	}
	loaded = false;
}

/*
================
budParser::GetPunctuationFromId
================
*/
const char* budParser::GetPunctuationFromId( int id )
{
	int i;
	
	if( !budParser::punctuations )
	{
		budLexer lex;
		return lex.GetPunctuationFromId( id );
	}
	
	for( i = 0; budParser::punctuations[i].p; i++ )
	{
		if( budParser::punctuations[i].n == id )
		{
			return budParser::punctuations[i].p;
		}
	}
	return "unknown punctuation";
}

/*
================
budParser::GetPunctuationId
================
*/
int budParser::GetPunctuationId( const char* p )
{
	int i;
	
	if( !budParser::punctuations )
	{
		budLexer lex;
		return lex.GetPunctuationId( p );
	}
	
	for( i = 0; budParser::punctuations[i].p; i++ )
	{
		if( !strcmp( budParser::punctuations[i].p, p ) )
		{
			return budParser::punctuations[i].n;
		}
	}
	return 0;
}

/*
================
budParser::budParser
================
*/
budParser::budParser()
{
	this->loaded = false;
	this->OSPath = false;
	this->punctuations = 0;
	this->flags = 0;
	this->scriptstack = NULL;
	this->indentstack = NULL;
	this->definehash = NULL;
	this->defines = NULL;
	this->tokens = NULL;
	this->marker_p = NULL;
}

/*
================
budParser::budParser
================
*/
budParser::budParser( int flags )
{
	this->loaded = false;
	this->OSPath = false;
	this->punctuations = 0;
	this->flags = flags;
	this->scriptstack = NULL;
	this->indentstack = NULL;
	this->definehash = NULL;
	this->defines = NULL;
	this->tokens = NULL;
	this->marker_p = NULL;
}

/*
================
budParser::budParser
================
*/
budParser::budParser( const char* filename, int flags, bool OSPath )
{
	this->loaded = false;
	this->OSPath = true;
	this->punctuations = 0;
	this->flags = flags;
	this->scriptstack = NULL;
	this->indentstack = NULL;
	this->definehash = NULL;
	this->defines = NULL;
	this->tokens = NULL;
	this->marker_p = NULL;
	LoadFile( filename, OSPath );
}

/*
================
budParser::budParser
================
*/
budParser::budParser( const char* ptr, int length, const char* name, int flags )
{
	this->loaded = false;
	this->OSPath = false;
	this->punctuations = 0;
	this->flags = flags;
	this->scriptstack = NULL;
	this->indentstack = NULL;
	this->definehash = NULL;
	this->defines = NULL;
	this->tokens = NULL;
	this->marker_p = NULL;
	LoadMemory( ptr, length, name );
}

/*
================
budParser::~budParser
================
*/
budParser::~budParser()
{
	budParser::FreeSource( false );
}

/*
========================
budParser::EndOfFile
========================
*/
bool budParser::EndOfFile()
{
	if( scriptstack != NULL )
	{
		return ( bool ) scriptstack->EndOfFile();
	}
	return true;
}

