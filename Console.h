/** @file *//********************************************************************************************************

                                                      Console.h

						                    Copyright 2004, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/Libraries/Console/Console.h#5 $

	$NoKeywords: $

 ********************************************************************************************************************/

#pragma once

#include <sstream>
#include <map>
#include <string>
#include <deque>
#include <cstdlib>
#include <Misc/Singleton.h>


/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

class Console : Singleton<Console>
{
private:

	class IConvertibleToFromString
	{
	public:
		virtual void		FromString( std::string const & value )	= 0;
		virtual std::string	ToString() const						= 0;
	};

	template < typename T > class Variable : public IConvertibleToFromString
	{
	public:

		Variable( T * pValue ) : m_pValue( pValue )
		{
		}

		void FromString( std::string const & value )
		{
			std::istringstream	buffer( value );
			buffer >> *m_pValue;
		}

		std::string ToString() const
		{
			std::ostringstream	buffer;
			buffer << *m_pValue;
			return buffer.str();
		}

	private:
		T *	m_pValue;
	};

public:

	typedef std::deque<char *> History;
    
	//! Returns a reference to the single instance of Console
	static Console & Console::Instance();

	//! Adds a variable whose value can be viewed and changed through the console
	//
	//! @param	sName	name of the variable
	//! @param	pValue	location of the variable
	template < typename T >
	void AddVariable( char const * sName, T * pValue )
	{
		Variable<T> *	pVariable	= new Variable<T>( pValue );
		m_Variables.insert( VariableMap::value_type( sName, pVariable ) );
	}

	//! Removes a variable
	void RemoveVariable( char const * sName );

	//! Accept a character input
	void Input( char c );

	//! Clears the contents of the command line.
	void ClearCommandLine();

	//! Returns the console output history
	History const & GetHistory() const						{ return m_History; }

	//! Sets the number of lines in the history
	void SetHistorySize( int size );

	//! Returns the current command line
	std::string GetCommandLine() const						{ return std::string( "> " ) + m_CommandLine; }

	//! Executes the command in the command line
	void Execute();

private:

	// Prevent external construction and deletion
	Console();
	virtual ~Console();
	Console( Console const & );
	Console & operator=( Console & );

	void Console::AddToHistory( std::string const & line );

	bool DoSetCommand( std::string const & parameters );
	bool DoShowCommand( std::string const & parameters );
	bool DoDumpCommand( std::string const & parameters );

	typedef std::map<std::string, IConvertibleToFromString *>	VariableMap;

	VariableMap		m_Variables;
	History			m_History;
	int				m_HistorySize;
	std::string		m_CommandLine;
};

