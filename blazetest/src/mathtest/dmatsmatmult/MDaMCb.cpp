//=================================================================================================
/*!
//  \file src/mathtest/dmatsmatmult/MDaMCb.cpp
//  \brief Source file for the MDaMCb dense matrix/sparse matrix multiplication math test
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
#include <blaze/math/CompressedMatrix.h>
#include <blaze/math/DynamicMatrix.h>
#include <blazetest/mathtest/Creator.h>
#include <blazetest/mathtest/DMatSMatMult.h>
#include <blazetest/system/MathTest.h>


//=================================================================================================
//
//  MAIN FUNCTION
//
//=================================================================================================

//*************************************************************************************************
int main()
{
   std::cout << "   Running 'MDaMCb'..." << std::endl;

   using blazetest::mathtest::TypeA;
   using blazetest::mathtest::TypeB;

   try
   {
      // Matrix type definitions
      typedef blaze::DynamicMatrix<TypeA>     MDa;
      typedef blaze::CompressedMatrix<TypeB>  MCb;

      // Creator type definitions
      typedef blazetest::Creator<MDa>  CMDa;
      typedef blazetest::Creator<MCb>  CMCb;

      // Running tests with small matrices
      for( size_t i=0UL; i<=6UL; ++i ) {
         for( size_t j=0UL; j<=6UL; ++j ) {
            for( size_t k=0UL; k<=6UL; ++k ) {
               for( size_t l=0UL; l<=i*k; ++l ) {
                  RUN_DMATSMATMULT_TEST( CMDa( j, i ), CMCb( i, k, l ) );
               }
            }
         }
      }

      // Running tests with large matrices
      RUN_DMATSMATMULT_TEST( CMDa(  31UL,  67UL ), CMCb(  67UL,  31UL,  7UL ) );
      RUN_DMATSMATMULT_TEST( CMDa(  31UL,  67UL ), CMCb(  67UL, 127UL, 13UL ) );
      RUN_DMATSMATMULT_TEST( CMDa(  67UL,  67UL ), CMCb(  67UL,  67UL,  7UL ) );
      RUN_DMATSMATMULT_TEST( CMDa( 127UL,  67UL ), CMCb(  67UL,  31UL,  7UL ) );
      RUN_DMATSMATMULT_TEST( CMDa( 127UL,  67UL ), CMCb(  67UL, 127UL, 13UL ) );
      RUN_DMATSMATMULT_TEST( CMDa(  32UL,  64UL ), CMCb(  64UL,  32UL,  8UL ) );
      RUN_DMATSMATMULT_TEST( CMDa(  32UL,  64UL ), CMCb(  64UL, 128UL, 16UL ) );
      RUN_DMATSMATMULT_TEST( CMDa(  64UL,  64UL ), CMCb(  64UL,  64UL,  8UL ) );
      RUN_DMATSMATMULT_TEST( CMDa( 128UL,  64UL ), CMCb(  64UL,  32UL,  8UL ) );
      RUN_DMATSMATMULT_TEST( CMDa( 128UL,  64UL ), CMCb(  64UL, 128UL, 16UL ) );
   }
   catch( std::exception& ex ) {
      std::cerr << "\n\n ERROR DETECTED during dense matrix/sparse matrix multiplication:\n"
                << ex.what() << "\n";
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}
//*************************************************************************************************
