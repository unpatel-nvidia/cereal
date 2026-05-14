/*
  Copyright (c) 2014, Randolph Voorhies, Shane Grant
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of the copyright holder nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CEREAL_TEST_LOAD_CONSTRUCT_EXT_BINARY_H_
#define CEREAL_TEST_LOAD_CONSTRUCT_EXT_BINARY_H_
#include "common.hpp"

// Issue #839: External load_and_construct does not work with Binary archive
//
// This test verifies that external LoadAndConstruct specializations work
// correctly with BinaryInputArchive, just as they do with XML, JSON, and
// PortableBinary archives.

// A type with no default constructor and only an external save function.
// Loading is exclusively through external LoadAndConstruct.
struct ExtLacSaveOnly
{
  ExtLacSaveOnly( int xx, double yy ) : x( xx ), y( yy ) {}

  int x;
  double y;

  bool operator==( ExtLacSaveOnly const & other ) const
  { return x == other.x && std::abs(y - other.y) < 1e-10; }
};

std::ostream& operator<<(std::ostream& os, ExtLacSaveOnly const & s)
{
  os << "[" << s.x << ", " << s.y << "]";
  return os;
}

template <class Archive>
void save( Archive & ar, ExtLacSaveOnly const & obj )
{
  ar( obj.x, obj.y );
}

namespace cereal
{
  template <>
  struct LoadAndConstruct<ExtLacSaveOnly>
  {
    template <class Archive>
    static void load_and_construct( Archive & ar, cereal::construct<ExtLacSaveOnly> & construct )
    {
      int xx;
      double yy;
      ar( xx, yy );
      construct( xx, yy );
    }
  };
}

// A type with no default constructor, member serialize, and external LoadAndConstruct.
// This tests the case where the type has a serialize function but the external
// LoadAndConstruct should still be used when loading through pointers.
struct ExtLacWithSerialize
{
  ExtLacWithSerialize( int xx ) : x( xx ) {}

  int x;

  template <class Archive>
  void serialize( Archive & ar )
  { ar( x ); }

  bool operator==( ExtLacWithSerialize const & other ) const
  { return x == other.x; }

private:
  ExtLacWithSerialize() : x( 0 ) {}
  friend class cereal::access;
};

std::ostream& operator<<(std::ostream& os, ExtLacWithSerialize const & s)
{
  os << "[" << s.x << "]";
  return os;
}

namespace cereal
{
  template <>
  struct LoadAndConstruct<ExtLacWithSerialize>
  {
    template <class Archive>
    static void load_and_construct( Archive & ar, cereal::construct<ExtLacWithSerialize> & construct )
    {
      int xx;
      ar( xx );
      construct( xx );
    }
  };
}

// A type with no default constructor, external save/LoadAndConstruct, and
// multiple data members to verify correct binary stream positioning.
struct ExtLacMultiField
{
  ExtLacMultiField( int a, float b, std::string c ) : alpha( a ), beta( b ), gamma( std::move(c) ) {}

  int alpha;
  float beta;
  std::string gamma;

  bool operator==( ExtLacMultiField const & other ) const
  { return alpha == other.alpha && std::abs(beta - other.beta) < 1e-5f && gamma == other.gamma; }
};

std::ostream& operator<<(std::ostream& os, ExtLacMultiField const & s)
{
  os << "[" << s.alpha << ", " << s.beta << ", " << s.gamma << "]";
  return os;
}

template <class Archive>
void save( Archive & ar, ExtLacMultiField const & obj )
{
  ar( obj.alpha, obj.beta, obj.gamma );
}

namespace cereal
{
  template <>
  struct LoadAndConstruct<ExtLacMultiField>
  {
    template <class Archive>
    static void load_and_construct( Archive & ar, cereal::construct<ExtLacMultiField> & construct )
    {
      int a;
      float b;
      std::string c;
      ar( a, b, c );
      construct( a, b, std::move(c) );
    }
  };
}

template <class IArchive, class OArchive>
void test_external_load_and_construct_binary()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(int ii=0; ii<100; ++ii)
  {
    auto o_shared1 = std::make_shared<ExtLacSaveOnly>( random_value<int>(gen), random_value<double>(gen) );
    std::unique_ptr<ExtLacSaveOnly> o_unique1( new ExtLacSaveOnly( random_value<int>(gen), random_value<double>(gen) ) );
    auto o_shared2 = std::make_shared<ExtLacWithSerialize>( random_value<int>(gen) );
    std::unique_ptr<ExtLacWithSerialize> o_unique2( new ExtLacWithSerialize( random_value<int>(gen) ) );
    auto o_shared3 = std::make_shared<ExtLacMultiField>( random_value<int>(gen), random_value<float>(gen), "test_str" );
    std::unique_ptr<ExtLacMultiField> o_unique3( new ExtLacMultiField( random_value<int>(gen), random_value<float>(gen), "hello" ) );

    std::ostringstream os;
    {
      OArchive oar(os);
      oar( o_shared1, o_unique1, o_shared2, o_unique2, o_shared3, o_unique3 );
    }

    decltype(o_shared1) i_shared1;
    decltype(o_unique1) i_unique1;
    decltype(o_shared2) i_shared2;
    decltype(o_unique2) i_unique2;
    decltype(o_shared3) i_shared3;
    decltype(o_unique3) i_unique3;

    std::istringstream is(os.str());
    {
      IArchive iar(is);
      iar( i_shared1, i_unique1, i_shared2, i_unique2, i_shared3, i_unique3 );
    }

    CHECK_EQ( *o_shared1, *i_shared1 );
    CHECK_EQ( *o_unique1, *i_unique1 );
    CHECK_EQ( *o_shared2, *i_shared2 );
    CHECK_EQ( *o_unique2, *i_unique2 );
    CHECK_EQ( *o_shared3, *i_shared3 );
    CHECK_EQ( *o_unique3, *i_unique3 );
  }
}

#endif // CEREAL_TEST_LOAD_CONSTRUCT_EXT_BINARY_H_
