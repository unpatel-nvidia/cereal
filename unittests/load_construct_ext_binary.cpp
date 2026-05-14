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
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "load_construct_ext_binary.hpp"

// Issue #839: External load_and_construct does not work with Binary archive
// These tests verify that external LoadAndConstruct specializations are
// correctly invoked for all archive types, including BinaryInputArchive.

TEST_SUITE_BEGIN("load_construct_ext_binary");

TEST_CASE("binary_external_load_and_construct")
{
  test_external_load_and_construct_binary<cereal::BinaryInputArchive, cereal::BinaryOutputArchive>();
}

TEST_CASE("portable_binary_external_load_and_construct")
{
  test_external_load_and_construct_binary<cereal::PortableBinaryInputArchive, cereal::PortableBinaryOutputArchive>();
}

TEST_CASE("xml_external_load_and_construct")
{
  test_external_load_and_construct_binary<cereal::XMLInputArchive, cereal::XMLOutputArchive>();
}

TEST_CASE("json_external_load_and_construct")
{
  test_external_load_and_construct_binary<cereal::JSONInputArchive, cereal::JSONOutputArchive>();
}

TEST_SUITE_END();
