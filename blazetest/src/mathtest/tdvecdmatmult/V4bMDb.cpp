//=================================================================================================
/*!
//  \file src/mathtest/tdvecdmatmult/V4bMDb.cpp
//  \brief Source file for the V4bMDb dense vector/dense matrix multiplication math test
//
//  Copyright (C) 2011 Klaus Iglberger - All Rights Reserved
//
//  This file is part of the Blaze library. This library is free software; you can redistribute
//  it and/or modify it under the terms of the GNU General Public License as published by the
//  Free Software Foundation; either version 3, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along with a special
//  exception for linking and compiling against the Blaze library, the so-called "runtime
//  exception"; see the file COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <cstdlib>
#include <iostream>
#include <blaze/math/DynamicMatrix.h>
#include <blaze/math/StaticVector.h>
#include <blazetest/mathtest/Creator.h>
#include <blazetest/mathtest/TDVecDMatMult.h>
#include <blazetest/system/MathTest.h>


//=================================================================================================
//
//  MAIN FUNCTION
//
//=================================================================================================

//*************************************************************************************************
int main()
{
   std::cout << "   Running 'V4bMDb'..." << std::endl;

   using blazetest::mathtest::TypeB;

   try
   {
      // Matrix type definitions
      typedef blaze::StaticVector<TypeB,4UL>  V4b;
      typedef blaze::DynamicMatrix<TypeB>     MDb;

      // Creator type definitions
      typedef blazetest::Creator<V4b>  CV4b;
      typedef blazetest::Creator<MDb>  CMDb;

      // Running the tests
      for( size_t i=0UL; i<=6UL; ++i ) {
         RUN_TDVECDMATMULT_TEST( CV4b(), CMDb( 4UL, i ) );
      }
   }
   catch( std::exception& ex ) {
      std::cerr << "\n\n ERROR DETECTED during dense vector/dense matrix multiplication:\n"
                << ex.what() << "\n";
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}
//*************************************************************************************************
