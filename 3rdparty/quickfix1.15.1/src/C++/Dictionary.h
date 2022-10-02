/* -*- C++ -*- */

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

#ifndef FIX_DICTIONARY_H
#define FIX_DICTIONARY_H

#ifdef _MSC_VER
#pragma warning( disable : 4503 4355 4786 4290 )
#endif

#include <map>
#include <string>
#include "Exceptions.h"

namespace FIX
{
/// For storage and retrieval of key/value pairs.
class Dictionary
{
public:
  Dictionary( const std::string& name ) : m_name( name ) {}
  Dictionary() {}
  virtual ~Dictionary() {}

  typedef std::map < std::string, std::string > Data;
  typedef Data::const_iterator iterator;
  typedef iterator const_iterator;

  /// Get the name of the dictionary.
  std::string getName() const { return m_name; }
  /// Return the number of key/value pairs.
  size_t size() const { return m_data.size(); }

  /// Get a value as a string.
  std::string getString( const std::string&, bool capitalize = false ) const
  throw( ConfigError, FieldConvertError );
  /// Get a value as a int.
  int getInt( const std::string& ) const
  throw( ConfigError, FieldConvertError );
  /// Get a value as a double.
  double getDouble( const std::string& ) const
  throw( ConfigError, FieldConvertError );
  /// Get a value as a bool
  bool getBool( const std::string& ) const
  throw( ConfigError, FieldConvertError );
  /// Get a value as a day of week
  int getDay( const std::string& ) const
  throw( ConfigError, FieldConvertError );

  /// Set a value from a string.
  void setString( const std::string&, const std::string& );
  /// Set a value from a int.
  void setInt( const std::string&, int );
  /// Set a value from a double.
  void setDouble( const std::string&, double );
  /// Set a value from a bool
  void setBool( const std::string&, bool );
  /// Set a value from a day
  void setDay( const std::string&, int );

  /// Check if the dictionary contains a value for key.
  bool has( const std::string& ) const;
  /// Merge two dictionaries.
  void merge( const Dictionary& );

  void FIX40Cache(const std::string& cache) { m_FIX40Cache = cache; }
  const std::string& FIX40Cache() const { return m_FIX40Cache; }
  void FIX41Cache(const std::string& cache) { m_FIX41Cache = cache; }
  const std::string& FIX41Cache() const { return m_FIX41Cache; }
  void FIX42Cache(const std::string& cache) { m_FIX42Cache = cache; }
  const std::string& FIX42Cache() const { return m_FIX42Cache; }
  void FIX43Cache(const std::string& cache) { m_FIX43Cache = cache; }
  const std::string& FIX43Cache() const { return m_FIX43Cache; }
  void FIX44Cache(const std::string& cache) { m_FIX44Cache = cache; }
  const std::string& FIX44Cache() const { return m_FIX44Cache; }
  void FIX50Cache(const std::string& cache) { m_FIX50Cache = cache; }
  const std::string& FIX50Cache() const { return m_FIX50Cache; }
  void FIX50SP1Cache(const std::string& cache) { m_FIX50SP1Cache = cache; }
  const std::string& FIX50SP1Cache() const { return m_FIX50SP1Cache; }
  void FIX50SP2Cache(const std::string& cache) { m_FIX50SP2Cache = cache; }
  const std::string& FIX50SP2Cache() const { return m_FIX50SP2Cache; }
  void FIXT11Cache(const std::string& cache) { m_FIXT11Cache = cache; }
  const std::string& FIXT11Cache() const { return m_FIXT11Cache; }

  iterator begin() const { return m_data.begin(); }
  iterator end() const { return m_data.end(); }
private:
  Data m_data;
  std::string m_name;
  
  std::string m_FIX40Cache;
  std::string m_FIX41Cache;
  std::string m_FIX42Cache;
  std::string m_FIX43Cache;
  std::string m_FIX44Cache;
  std::string m_FIX50Cache;
  std::string m_FIX50SP1Cache;
  std::string m_FIX50SP2Cache;
  std::string m_FIXT11Cache;
};
/*! @} */
}

#endif //FIX_DICTIONARY_H
