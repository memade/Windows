/****************************************************************************
** Copyright (c) 2001-2014
**
** This file is part of the QuickFIX FIX Engine
**
** This file may be distributed under the terms of the quickfixengine.org
** license as defined by quickfixengine.org and appearing in the file
** LICENSE included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.quickfixengine.org/LICENSE for licensing information.
**
** Contact ask@quickfixengine.org if any conditions of this licensing are
** not clear to you.
**
****************************************************************************/

#ifdef _MSC_VER
#include "stdafx.h"
#else
#include "config.h"
#endif

#include "SessionSettings.h"
#include "Settings.h"
#include "Values.h"
#include <fstream>

namespace FIX
{
SessionSettings::SessionSettings( std::istream& stream, bool resolveEnvVars )
throw( ConfigError )
: m_resolveEnvVars( resolveEnvVars )
{
  stream >> *this;
}

SessionSettings::SessionSettings( const std::string& file, bool resolveEnvVars )
throw( ConfigError )
: m_resolveEnvVars( resolveEnvVars )
{
  std::ifstream fstream( file.c_str() );
  if ( !fstream.is_open() )
    throw ConfigError( ( "File " + file + " not found" ).c_str() );
  fstream >> *this;
}

std::istream& operator>>( std::istream& stream, SessionSettings& s )
throw( ConfigError )
{
  Settings settings(s.m_resolveEnvVars);
  stream >> settings;

  Settings::Sections section;

  section = settings.get( "DEFAULT" );
  Dictionary def;
  if ( section.size() )
    def = section[ 0 ];
  s.set( def );

  section = settings.get( "SESSION" );
  Settings::Sections::size_type session;
  Dictionary dict;

  for ( session = 0; session < section.size(); ++session )
  {
    dict = section[ session ];
    dict.merge( def );

    dict.FIX40Cache(s.FIX40Cache());
    dict.FIX41Cache(s.FIX41Cache());
    dict.FIX42Cache(s.FIX42Cache());
    dict.FIX43Cache(s.FIX43Cache());
    dict.FIX44Cache(s.FIX44Cache());
    dict.FIX50Cache(s.FIX50Cache());
    dict.FIX50SP1Cache(s.FIX50SP1Cache());
    dict.FIX50SP2Cache(s.FIX50SP2Cache());
    dict.FIXT11Cache(s.FIXT11Cache());

    BeginString beginString
    ( dict.getString( BEGINSTRING ) );
    SenderCompID senderCompID
    ( dict.getString( SENDERCOMPID ) );
    TargetCompID targetCompID
    ( dict.getString( TARGETCOMPID ) );

    std::string sessionQualifier;
    if( dict.has( SESSION_QUALIFIER ) )
      sessionQualifier = dict.getString( SESSION_QUALIFIER );
    SessionID sessionID( beginString, senderCompID, targetCompID, sessionQualifier );
    s.set( sessionID, dict );
  }
  return stream;
}

std::ostream& operator<<( std::ostream& stream, const SessionSettings& s )
{
  const Dictionary& defaults = s.m_defaults;
  if( defaults.size() )
  {
    stream << "[DEFAULT]" << std::endl;
    Dictionary::iterator i;
    for( i = defaults.begin(); i != defaults.end(); ++i )
      stream << i->first << "=" << i->second << std::endl;
    stream << std::endl;
  }

  std::set<SessionID> sessions = s.getSessions();
  std::set<SessionID>::iterator i;
  for( i = sessions.begin(); i != sessions.end(); ++i )
  {
    stream << "[SESSION]" << std::endl;
    const Dictionary& section = s.get( *i );
    if( !section.size() ) continue;

    Dictionary::iterator i;
    for( i = section.begin(); i != section.end(); ++i )
    {
      if( defaults.has(i->first) && defaults.getString(i->first) == i->second )
        continue;
      stream << i->first << "=" << i->second << std::endl;
    }
    stream << std::endl;
  }

  return stream;
}

const bool SessionSettings::has( const SessionID& sessionID ) const
{
  return m_settings.find( sessionID ) != m_settings.end();
}

const Dictionary& SessionSettings::get( const SessionID& sessionID ) const
throw( ConfigError )
{
  Dictionaries::const_iterator i;
  i = m_settings.find( sessionID );
  if ( i == m_settings.end() ) throw ConfigError( "Session not found" );
  return i->second;
}

void SessionSettings::set( const SessionID& sessionID,
                           Dictionary settings )
throw( ConfigError )
{
  if( has(sessionID) )
    throw ConfigError( "Duplicate Session " + sessionID.toString() );

  settings.setString( BEGINSTRING, sessionID.getBeginString() );
  settings.setString( SENDERCOMPID, sessionID.getSenderCompID() );
  settings.setString( TARGETCOMPID, sessionID.getTargetCompID() );

  settings.merge( m_defaults );
  validate( settings );
  m_settings[ sessionID ] = settings;
}

SessionSettings::SessionSettings(const std::string& data, const std::string& path, bool resolveEnvVars) throw(ConfigError)
    : m_resolveEnvVars(resolveEnvVars)
{
	std::stringstream gangplank;
	std::iostream io(gangplank.rdbuf());
	io.write(data.data(), data.size());
	io >> *this;

	if (m_defaults.has(DATA_DICTIONARY))
	{
		m_defaults.setString(DATA_DICTIONARY, path + "/" + m_defaults.getString(DATA_DICTIONARY));
	}
	if (m_defaults.has(FILE_STORE_PATH))
	{
		m_defaults.setString(FILE_STORE_PATH, path + "/" + m_defaults.getString(FILE_STORE_PATH));
	}
	if (m_defaults.has(FILE_LOG_PATH))
	{
		m_defaults.setString(FILE_LOG_PATH, path + "/" + m_defaults.getString(FILE_LOG_PATH));
	}
    if (m_defaults.has(FILE_LOG_BACKUP_PATH))
    {
        m_defaults.setString(FILE_LOG_BACKUP_PATH, path + "/" + m_defaults.getString(FILE_LOG_BACKUP_PATH));
    }
    if (m_defaults.has(TRANSPORT_DATA_DICTIONARY))
    {
        m_defaults.setString(TRANSPORT_DATA_DICTIONARY, path + "/" + m_defaults.getString(TRANSPORT_DATA_DICTIONARY));
    }
    if (m_defaults.has(APP_DATA_DICTIONARY))
    {
        m_defaults.setString(APP_DATA_DICTIONARY, path + "/" + m_defaults.getString(APP_DATA_DICTIONARY));
    }
	for (auto i = m_settings.begin(); i != m_settings.end(); ++i)
	{
		if (i->second.has(DATA_DICTIONARY))
		{
			i->second.setString(DATA_DICTIONARY, path + "/" + i->second.getString(DATA_DICTIONARY));
		}
		if (i->second.has(FILE_STORE_PATH))
		{
			i->second.setString(FILE_STORE_PATH, path + "/" + i->second.getString(FILE_STORE_PATH));
		}
		if (i->second.has(FILE_LOG_PATH))
		{
			i->second.setString(FILE_LOG_PATH, path + "/" + i->second.getString(FILE_LOG_PATH));
		}
        if (i->second.has(FILE_LOG_BACKUP_PATH))
        {
            i->second.setString(FILE_LOG_BACKUP_PATH, path + "/" + i->second.getString(FILE_LOG_BACKUP_PATH));
        }
        if (i->second.has(TRANSPORT_DATA_DICTIONARY))
        {
            i->second.setString(TRANSPORT_DATA_DICTIONARY, path + "/" + i->second.getString(TRANSPORT_DATA_DICTIONARY));
        }
        if (i->second.has(APP_DATA_DICTIONARY))
        {
            i->second.setString(APP_DATA_DICTIONARY, path + "/" + i->second.getString(APP_DATA_DICTIONARY));
        }
	}
}
void SessionSettings::set( const Dictionary& defaults ) throw( ConfigError ) 
{ 
  m_defaults = defaults;
  Dictionaries::iterator i = m_settings.begin();
  for( i = m_settings.begin(); i != m_settings.end(); ++i )
    i->second.merge( defaults );
}

std::set < SessionID > SessionSettings::getSessions() const
{
  std::set < SessionID > result;
  Dictionaries::const_iterator i;
  for ( i = m_settings.begin(); i != m_settings.end(); ++i )
    result.insert( i->first );
  return result;
}

void SessionSettings::validate( const Dictionary& dictionary ) const
throw( ConfigError )
{
  std::string beginString = dictionary.getString( BEGINSTRING );
  if( beginString != BeginString_FIX40 &&
      beginString != BeginString_FIX41 &&
      beginString != BeginString_FIX42 &&
      beginString != BeginString_FIX43 &&
      beginString != BeginString_FIX44 &&
      beginString != BeginString_FIXT11 )
  {
    throw ConfigError( std::string(BEGINSTRING) + " must be FIX.4.0 to FIX.4.4 or FIXT.1.1" );
  }

  std::string connectionType = dictionary.getString( CONNECTION_TYPE );
  if( connectionType != "initiator" &&
      connectionType != "acceptor" )
  {
    throw ConfigError( std::string(CONNECTION_TYPE) + " must be 'initiator' or 'acceptor'" );
  }
}
}
