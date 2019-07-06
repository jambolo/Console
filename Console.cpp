/** @file *//********************************************************************************************************

                                                     Console.cpp

						                    Copyright 2006, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Libraries/Console/Console.cpp#5 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "Console.h"

#include <cstring>
#include <sstream>

using namespace std;


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Console::Console()
	: m_HistorySize( 20 )
{
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Console::~Console()
{
	// Deallocate all data in the history

	for ( History::iterator psLine = m_History.begin(); psLine != m_History.end(); ++psLine )
	{
		delete *psLine;
	}

	// Deallocate all variable data

	for ( VariableMap::iterator pV = m_Variables.begin(); pV != m_Variables.end(); ++pV )
	{
		delete pV->second;
	}
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

Console & Console::Instance()
{
	if ( Singleton<Console>::Instance() == 0 )
	{
		new Console;
	}

	return *static_cast< Console * >( Singleton<Console>::Instance() );
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void Console::RemoveVariable( char const * sName )
{
	VariableMap::iterator	pV	= m_Variables.find( sName );

	if ( pV != m_Variables.end() )
	{
		delete pV->second;
		m_Variables.erase( pV );
	}
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void Console::Input( char c )
{
	// If the inputted character is printable, then add it to the command line.

	if ( isprint( c ) )
	{
		m_CommandLine += c;
	}

	// Otherwise, if the inputted character is backspace, then delete the last character on the line

	else if ( c == 0x08 )
	{
		if ( m_CommandLine.size() > 0 )
		{
			m_CommandLine.erase( m_CommandLine.size() - 1 );
		}
	}
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void Console::ClearCommandLine()
{
	m_CommandLine.erase();
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void Console::SetHistorySize( int size )
{
	while ( (int)m_History.size() > size )
	{
		delete m_History.front();
		m_History.pop_front();
	}

	m_HistorySize = size;
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void Console::Execute()
{
	if ( m_CommandLine.size() == 0 )
	{
		return;
	}

	// Save the command in the history

	AddToHistory( string( "> " ) + m_CommandLine );

	// Parse the command

	size_t			command_start;
	size_t			command_end;
	size_t			parameter_start;

	command_start = m_CommandLine.find_first_not_of( ' ', 0 );
	if ( command_start != string::npos )
	{
		string		command;
		string		parameters;

		command_end = m_CommandLine.find_first_of( ' ', command_start+1 );
		if ( command_end != string::npos )
		{
			command = m_CommandLine.substr( command_start, command_end-command_start );

			parameter_start = m_CommandLine.find_first_not_of( ' ', command_end+1 );
			if ( parameter_start != string::npos )
			{
				parameters = m_CommandLine.substr( parameter_start );
			}
		}
		else
		{
			command = m_CommandLine.substr( command_start );
		}

		if ( command == "set" )
		{
			DoSetCommand( parameters );
		}
		else if ( command == "show" )
		{
			DoShowCommand( parameters );
		}
		else if ( command == "dump" )
		{
			DoDumpCommand( parameters );
		}
	}
	else
	{
		AddToHistory( "Invalid command" );
		goto done;
	}

done:

	// Erase the command line

	m_CommandLine.erase();
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

bool Console::DoSetCommand( string const & parameters )
{
	size_t	start;
	size_t	end;

	end = parameters.find_first_of( ' ' );
	if ( end == string::npos )
	{
		AddToHistory( "Invalid command" );
		return false;
	}

	string variable = parameters.substr( 0, end );

	start = parameters.find_first_not_of( ' ', end+1 );
	if ( start == string::npos )
	{
		AddToHistory( "Invalid command" );
		return false;
	}

	string value = parameters.substr( start );

	VariableMap::iterator pV	= m_Variables.find( variable );

	if ( pV == m_Variables.end() )
	{
		AddToHistory( "Unknown variable" );
		return false;
	}

	pV->second->FromString( value );

	return true;
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

bool Console::DoShowCommand( string const & parameters )
{
	VariableMap::iterator pV	= m_Variables.find( parameters );

	if ( pV == m_Variables.end() )
	{
		AddToHistory( "Unknown variable" );
		return false;
	}

	ostringstream	buffer;
	buffer << pV->first << " = " << pV->second->ToString();
	AddToHistory( buffer.str() );

	return true;
}


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

#pragma warning( push )
#pragma warning( disable : 4100 )	// 'identifier' : unreferenced formal parameter

bool Console::DoDumpCommand( string const & parameters )
{
	for ( VariableMap::iterator pV = m_Variables.begin(); pV != m_Variables.end(); ++pV )
	{
		ostringstream	buffer;
		buffer << pV->first << " = " << pV->second->ToString();
		AddToHistory( buffer.str() );
	}

	return true;
}

#pragma warning( pop )

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void Console::AddToHistory( string const & line )
{
	// Make a copy of the string

	char * pLine	= new char[ line.size() + 1 ];

	strcpy( pLine, line.c_str() );

	// Make room for the new line if needed

	while ( m_History.size() >= (size_t)m_HistorySize )
	{
		delete m_History.front();
		m_History.pop_front();
	}

	// Add the line

	m_History.push_back( pLine );
}
