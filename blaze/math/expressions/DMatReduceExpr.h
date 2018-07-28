//=================================================================================================
/*!
//  \file blaze/math/expressions/DMatReduceExpr.h
//  \brief Header file for the dense matrix reduce expression
//
//  Copyright (C) 2012-2018 Klaus Iglberger - All Rights Reserved
//
//  This file is part of the Blaze library. You can redistribute it and/or modify it under
//  the terms of the New (Revised) BSD License. Redistribution and use in source and binary
//  forms, with or without modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list
//     of conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//  3. Neither the names of the Blaze development group nor the names of its contributors
//     may be used to endorse or promote products derived from this software without specific
//     prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
//  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
//  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
//  SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
//  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
//  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
*/
//=================================================================================================

#ifndef _BLAZE_MATH_EXPRESSIONS_DMATREDUCEEXPR_H_
#define _BLAZE_MATH_EXPRESSIONS_DMATREDUCEEXPR_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <iterator>
#include <blaze/math/Aliases.h>
#include <blaze/math/constraints/ColumnMajorMatrix.h>
#include <blaze/math/constraints/ColumnVector.h>
#include <blaze/math/constraints/DenseMatrix.h>
#include <blaze/math/constraints/DenseVector.h>
#include <blaze/math/constraints/RequiresEvaluation.h>
#include <blaze/math/constraints/RowMajorMatrix.h>
#include <blaze/math/constraints/RowVector.h>
#include <blaze/math/Exception.h>
#include <blaze/math/expressions/Computation.h>
#include <blaze/math/expressions/DenseMatrix.h>
#include <blaze/math/expressions/DenseVector.h>
#include <blaze/math/expressions/Forward.h>
#include <blaze/math/expressions/MatReduceExpr.h>
#include <blaze/math/functors/Add.h>
#include <blaze/math/functors/Mult.h>
#include <blaze/math/shims/Serial.h>
#include <blaze/math/SIMD.h>
#include <blaze/math/traits/ReduceTrait.h>
#include <blaze/math/typetraits/IsExpression.h>
#include <blaze/math/typetraits/RequiresEvaluation.h>
#include <blaze/math/views/Check.h>
#include <blaze/system/Thresholds.h>
#include <blaze/util/Assert.h>
#include <blaze/util/DisableIf.h>
#include <blaze/util/EnableIf.h>
#include <blaze/util/FunctionTrace.h>
#include <blaze/util/mpl/If.h>
#include <blaze/util/StaticAssert.h>
#include <blaze/util/Template.h>
#include <blaze/util/Types.h>
#include <blaze/util/typetraits/HasMember.h>
#include <blaze/util/typetraits/IsSame.h>
#include <blaze/util/typetraits/RemoveReference.h>


namespace blaze {

//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Base template for row-major dense matrix reduction operations.
// \ingroup dense_vector_expression
//
// The DMatReduceExpr class represents the compile time expression for partial reduction operations
// of row-major dense matrices.
*/
template< size_t RF      // Reduction flag
        , typename MT    // Type of the dense matrix
        , typename OP >  // Type of the reduction operation
class DMatReduceExpr
{};
//*************************************************************************************************




//=================================================================================================
//
//  CLASS TEMPLATE SPECIALIZATION FOR COLUMN-WISE REDUCTION OPERATIONS OF ROW-MAJOR MATRICES
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Expression object for column-wise row-major dense matrix reduction operations.
// \ingroup dense_vector_expression
//
// This specialization of the DMatReduceExpr class template represents the compile time expression
// for column-wise reduction operations of row-major dense matrices.
*/
template< typename MT    // Type of the dense matrix
        , typename OP >  // Type of the reduction operation
class DMatReduceExpr<0UL,MT,OP>
   : public MatReduceExpr<0UL, DenseVector< DMatReduceExpr<0UL,MT,OP>, true > >
   , private Computation
{
 private:
   //**Type definitions****************************************************************************
   using RT = ResultType_t<MT>;     //!< Result type of the dense matrix expression.
   using ET = ElementType_t<MT>;    //!< Element type of the dense matrix expression.
   using CT = CompositeType_t<MT>;  //!< Composite type of the dense matrix expression.
   //**********************************************************************************************

   //**Parallel evaluation strategy****************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper variable template for the explicit application of the SFINAE principle.
   /*! This variable template is a helper for the selection of the parallel evaluation strategy.
       In case the dense matrix operand is not SMP assignable and requires an intermediate
       evaluation, the variable is set to 1 and the expression specific evaluation strategy is
       selected. Otherwise the variable is set to 0 and the default strategy is chosen. */
   template< typename VT >
   static constexpr bool UseSMPAssign_v = ( !MT::smpAssignable && RequiresEvaluation_v<MT> );
   /*! \endcond */
   //**********************************************************************************************

 public:
   //**Type definitions****************************************************************************
   using This          = DMatReduceExpr<0UL,MT,OP>;    //!< Type of this DMatReduceExpr instance.
   using ResultType    = ReduceTrait_t<RT,OP,0UL>;     //!< Result type for expression template evaluations.
   using TransposeType = TransposeType_t<ResultType>;  //!< Transpose type for expression template evaluations.
   using ElementType   = ElementType_t<ResultType>;    //!< Resulting element type.
   using SIMDType      = SIMDTrait_t<ElementType>;     //!< Resulting SIMD element type.
   using ReturnType    = const ElementType;            //!< Return type for expression template evaluations.
   using CompositeType = const ResultType;             //!< Data type for composite expression templates.

   //! Composite type of the left-hand side dense matrix expression.
   using Operand = If_t< IsExpression_v<MT>, const MT, const MT& >;

   //! Data type of the custom unary operation.
   using Operation = OP;
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation switch for the expression template evaluation strategy.
   static constexpr bool simdEnabled = false;

   //! Compilation switch for the expression template assignment strategy.
   static constexpr bool smpAssignable = MT::smpAssignable;
   //**********************************************************************************************

   //**Constructor*********************************************************************************
   /*!\brief Constructor for the DMatReduceExpr class.
   //
   // \param dm The matrix operand of the reduction expression.
   */
   explicit inline DMatReduceExpr( const MT& dm, OP op ) noexcept
      : dm_( dm )  // Dense matrix of the reduction expression
      , op_( op )  // The reduction operation
   {}
   //**********************************************************************************************

   //**Subscript operator**************************************************************************
   /*!\brief Subscript operator for the direct access to the vector elements.
   //
   // \param index Access index. The index has to be in the range \f$[0..N-1]\f$.
   // \return The resulting value.
   */
   inline ReturnType operator[]( size_t index ) const {
      BLAZE_INTERNAL_ASSERT( index < dm_.columns(), "Invalid vector access index" );
      return reduce( column( dm_, index, unchecked ), op_ );
   }
   //**********************************************************************************************

   //**At function*********************************************************************************
   /*!\brief Checked access to the vector elements.
   //
   // \param index Access index. The index has to be in the range \f$[0..N-1]\f$.
   // \return The resulting value.
   // \exception std::out_of_range Invalid vector access index.
   */
   inline ReturnType at( size_t index ) const {
      if( index >= dm_.columns() ) {
         BLAZE_THROW_OUT_OF_RANGE( "Invalid vector access index" );
      }
      return (*this)[index];
   }
   //**********************************************************************************************

   //**Size function*******************************************************************************
   /*!\brief Returns the current size/dimension of the vector.
   //
   // \return The size of the vector.
   */
   inline size_t size() const noexcept {
      return dm_.columns();
   }
   //**********************************************************************************************

   //**Operand access******************************************************************************
   /*!\brief Returns the dense matrix operand.
   //
   // \return The dense matrix operand.
   */
   inline Operand operand() const noexcept {
      return dm_;
   }
   //**********************************************************************************************

   //**Operation access****************************************************************************
   /*!\brief Returns a copy of the reduction operation.
   //
   // \return A copy of the reduction operation.
   */
   inline Operation operation() const {
      return op_;
   }
   //**********************************************************************************************

   //**********************************************************************************************
   /*!\brief Returns whether the expression can alias with the given address \a alias.
   //
   // \param alias The alias to be checked.
   // \return \a true in case an aliasing effect is possible, \a false if not.
   */
   template< typename T >
   inline bool canAlias( const T* alias ) const noexcept {
      return ( dm_.isAliased( alias ) );
   }
   //**********************************************************************************************

   //**********************************************************************************************
   /*!\brief Returns whether the expression is aliased with the given address \a alias.
   //
   // \param alias The alias to be checked.
   // \return \a true in case the given alias is contained in this expression, \a false if not.
   */
   template< typename T >
   inline bool isAliased( const T* alias ) const noexcept {
      return ( dm_.isAliased( alias ) );
   }
   //**********************************************************************************************

   //**********************************************************************************************
   /*!\brief Returns whether the operands of the expression are properly aligned in memory.
   //
   // \return \a true in case the operands are aligned, \a false if not.
   */
   inline bool isAligned() const noexcept {
      return false;
   }
   //**********************************************************************************************

   //**********************************************************************************************
   /*!\brief Returns whether the expression can be used in SMP assignments.
   //
   // \return \a true in case the expression can be used in SMP assignments, \a false if not.
   */
   inline bool canSMPAssign() const noexcept {
      return dm_.canSMPAssign() || ( size() > SMP_DMATDVECMULT_THRESHOLD );  // TODO: rethink
   }
   //**********************************************************************************************

 private:
   //**Member variables****************************************************************************
   Operand   dm_;  //!< Dense matrix of the reduction expression.
   Operation op_;  //!< The reduction operation.
   //**********************************************************************************************

   //**Assignment to dense vectors*****************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Assignment of a column-wise row-major dense matrix reduction operation to a dense vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side reduction expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized assignment of a column-wise row-major
   // dense matrix reduction expression to a dense vector.
   */
   template< typename VT1 >  // Type of the target dense vector
   friend inline void assign( DenseVector<VT1,true>& lhs, const DMatReduceExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const size_t M( rhs.dm_.rows() );

      if( M == 0UL ) {
         reset( ~lhs );
         return;
      }

      CT tmp( serial( rhs.dm_ ) );

      assign( ~lhs, row( tmp, 0UL, unchecked ) );
      for( size_t i=1UL; i<M; ++i ) {
         assign( ~lhs, map( ~lhs, row( tmp, i, unchecked ), rhs.op_ ) );
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Assignment to sparse vectors****************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Assignment of a column-wise row-major dense matrix reduction operation to a dense vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side reduction expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized assignment of a column-wise row-major
   // dense matrix reduction expression to a dense vector.
   */
   template< typename VT1 >  // Type of the target dense vector
   friend inline void assign( SparseVector<VT1,true>& lhs, const DMatReduceExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_ROW_VECTOR_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( ResultType );

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const ResultType tmp( serial( rhs ) );
      assign( ~lhs, tmp );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Addition assignment to dense vectors********************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Addition assignment of a column-wise row-major dense matrix reduction operation to a
   //        dense vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side reduction expression to be added.
   // \return void
   //
   // This function implements the performance optimized addition assignment of a column-wise
   // row-major dense matrix reduction expression to a dense vector.
   */
   template< typename VT1 >  // Type of the target dense vector
   friend inline void addAssign( DenseVector<VT1,true>& lhs, const DMatReduceExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      if( rhs.dm_.rows() == 0UL ) {
         return;
      }
      else if( IsSame_v<OP,Add> ) {
         CT tmp( serial( rhs.dm_ ) );
         const size_t M( tmp.rows() );
         for( size_t i=0UL; i<M; ++i ) {
            addAssign( (~lhs), row( tmp, i, unchecked ) );
         }
      }
      else {
         const ResultType tmp( rhs );
         addAssign( ~lhs, tmp );
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Addition assignment to sparse vectors*******************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Addition assignment of a column-wise row-major dense matrix reduction operation to a
   //        dense vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side reduction expression to be added.
   // \return void
   //
   // This function implements the performance optimized addition assignment of a column-wise
   // row-major dense matrix reduction expression to a dense vector.
   */
   template< typename VT1 >  // Type of the target dense vector
   friend inline void addAssign( SparseVector<VT1,true>& lhs, const DMatReduceExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_ROW_VECTOR_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( ResultType );

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const ResultType tmp( serial( rhs ) );
      addAssign( ~lhs, tmp );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Subtraction assignment to dense vectors*****************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Subtraction assignment of a column-wise row-major dense matrix reduction operation
   //        to a dense vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side reduction expression to be subtracted.
   // \return void
   //
   // This function implements the performance optimized subtraction assignment of a column-wise
   // row-major dense matrix reduction expression to a dense vector.
   */
   template< typename VT1 >  // Type of the target dense vector
   friend inline void subAssign( DenseVector<VT1,true>& lhs, const DMatReduceExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      if( rhs.dm_.rows() == 0UL ) {
         return;
      }
      else if( IsSame_v<OP,Add> ) {
         CT tmp( serial( rhs.dm_ ) );
         const size_t M( tmp.rows() );
         for( size_t i=0UL; i<M; ++i ) {
            subAssign( (~lhs), row( tmp, i, unchecked ) );
         }
      }
      else {
         const ResultType tmp( rhs );
         subAssign( ~lhs, tmp );
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Subtraction assignment to sparse vectors****************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Subtraction assignment of a column-wise row-major dense matrix reduction operation
   //        to a dense vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side reduction expression to be subtracted.
   // \return void
   //
   // This function implements the performance optimized subtraction assignment of a column-wise
   // row-major dense matrix reduction expression to a dense vector.
   */
   template< typename VT1 >  // Type of the target dense vector
   friend inline void subAssign( SparseVector<VT1,true>& lhs, const DMatReduceExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_ROW_VECTOR_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( ResultType );

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const ResultType tmp( serial( rhs ) );
      subAssign( ~lhs, tmp );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Multiplication assignment to dense vectors**************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Multiplication assignment of a column-wise row-major dense matrix reduction operation
   //        to a dense vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side reduction expression to be multiplied.
   // \return void
   //
   // This function implements the performance optimized multiplication assignment of a column-wise
   // row-major dense matrix reduction expression to a dense vector.
   */
   template< typename VT1 >  // Type of the target dense vector
   friend inline void multAssign( DenseVector<VT1,true>& lhs, const DMatReduceExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      if( rhs.dm_.rows() == 0UL ) {
         reset( ~lhs );
      }
      else if( IsSame_v<OP,Mult> ) {
         CT tmp( serial( rhs.dm_ ) );
         const size_t M( tmp.rows() );
         for( size_t i=0UL; i<M; ++i ) {
            multAssign( (~lhs), row( tmp, i, unchecked ) );
         }
      }
      else {
         const ResultType tmp( rhs );
         multAssign( ~lhs, tmp );
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Multiplication assignment to sparse vectors*************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Multiplication assignment of a column-wise row-major dense matrix reduction operation
   //        to a dense vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side dense vector.
   // \param rhs The right-hand side reduction expression to be multiplied.
   // \return void
   //
   // This function implements the performance optimized multiplication assignment of a column-wise
   // row-major dense matrix reduction expression to a dense vector.
   */
   template< typename VT1 >  // Type of the target dense vector
   friend inline void multAssign( SparseVector<VT1,true>& lhs, const DMatReduceExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_ROW_VECTOR_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( ResultType );

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const ResultType tmp( serial( rhs ) );
      multAssign( ~lhs, tmp );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Division assignment to vectors**************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Division assignment of a column-wise row-major dense matrix reduction operation
   //        to a vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side vector.
   // \param rhs The right-hand side reduction expression divisor.
   // \return void
   //
   // This function implements the performance optimized division assignment of a column-wise
   // row-major dense matrix reduction expression to a vector.
   */
   template< typename VT1 >  // Type of the target vector
   friend inline void divAssign( Vector<VT1,true>& lhs, const DMatReduceExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_CONSTRAINT_MUST_BE_DENSE_VECTOR_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_ROW_VECTOR_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( ResultType );

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const ResultType tmp( serial( rhs ) );
      divAssign( ~lhs, tmp );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**SMP assignment to vectors*******************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief SMP assignment of a column-wise row-major dense matrix reduction operation to a vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side vector.
   // \param rhs The right-hand side reduction expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized SMP assignment of a column-wise row-major
   // dense matrix reduction expression to a vector. Due to the explicit application of the SFINAE
   // principle, this function can only be selected by the compiler in case the expression specific
   // parallel evaluation strategy is selected.
   */
   template< typename VT1 >  // Type of the target vector
   friend inline auto smpAssign( Vector<VT1,true>& lhs, const DMatReduceExpr& rhs )
      -> EnableIf_t< UseSMPAssign_v<VT1> >
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const RT tmp( rhs.dm_ );  // Evaluation of the dense matrix operand
      smpAssign( ~lhs, reduce<0UL>( tmp, rhs.op_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**SMP addition assignment to vectors**********************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief SMP addition assignment of a column-wise row-major dense matrix reduction operation
   //        to a vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side vector.
   // \param rhs The right-hand side reduction expression to be added.
   // \return void
   //
   // This function implements the performance optimized SMP addition assignment of a column-wise
   // row-major dense matrix reduction expression to a vector. Due to the explicit application
   // of the SFINAE principle, this function can only be selected by the compiler in case the
   // expression specific parallel evaluation strategy is selected.
   */
   template< typename VT1 >  // Type of the target vector
   friend inline auto smpAddAssign( Vector<VT1,true>& lhs, const DMatReduceExpr& rhs )
      -> EnableIf_t< UseSMPAssign_v<VT1> >
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const RT tmp( rhs.dm_ );  // Evaluation of the dense matrix operand
      smpAddAssign( ~lhs, reduce<0UL>( tmp, rhs.op_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**SMP subtraction assignment to vectors*******************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief SMP subtraction assignment of a column-wise row-major dense matrix reduction operation
   //        to a vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side vector.
   // \param rhs The right-hand side reduction expression to be subtracted.
   // \return void
   //
   // This function implements the performance optimized SMP subtraction assignment of a column-wise
   // row-major dense matrix reduction expression to a vector. Due to the explicit application
   // of the SFINAE principle, this function can only be selected by the compiler in case the
   // expression specific parallel evaluation strategy is selected.
   */
   template< typename VT1 >  // Type of the target vector
   friend inline auto smpSubAssign( Vector<VT1,true>& lhs, const DMatReduceExpr& rhs )
      -> EnableIf_t< UseSMPAssign_v<VT1> >
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const RT tmp( rhs.dm_ );  // Evaluation of the dense matrix operand
      smpSubAssign( ~lhs, reduce<0UL>( tmp, rhs.op_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**SMP multiplication assignment to vectors****************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief SMP multiplication assignment of a column-wise row-major dense matrix reduction operation
   //        to a vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side vector.
   // \param rhs The right-hand side reduction expression to be multiplied.
   // \return void
   //
   // This function implements the performance optimized SMP multiplication assignment of a
   // column-wise row-major dense matrix reduction expression to a vector. Due to the explicit
   // application of the SFINAE principle, this function can only be selected by the compiler
   // in case the expression specific parallel evaluation strategy is selected.
   */
   template< typename VT1 >  // Type of the target vector
   friend inline auto smpMultAssign( Vector<VT1,true>& lhs, const DMatReduceExpr& rhs )
      -> EnableIf_t< UseSMPAssign_v<VT1> >
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const RT tmp( rhs.dm_ );  // Evaluation of the dense matrix operand
      smpMultAssign( ~lhs, reduce<0UL>( tmp, rhs.op_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**SMP division assignment to vectors**********************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief SMP division assignment of a column-wise row-major dense matrix reduction operation
   //        to a vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side vector.
   // \param rhs The right-hand side reduction expression divisor.
   // \return void
   //
   // This function implements the performance optimized SMP division assignment of a column-wise
   // row-major dense matrix reduction expression to a vector. Due to the explicit application
   // of the SFINAE principle, this function can only be selected by the compiler in case the
   // expression specific parallel evaluation strategy is selected.
   */
   template< typename VT1 >  // Type of the target vector
   friend inline auto smpDivAssign( Vector<VT1,true>& lhs, const DMatReduceExpr& rhs )
      -> EnableIf_t< UseSMPAssign_v<VT1> >
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const RT tmp( rhs.dm_ );  // Evaluation of the dense matrix operand
      smpDivAssign( ~lhs, reduce<0UL>( tmp, rhs.op_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( MT );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT );
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************




//=================================================================================================
//
//  CLASS TEMPLATE SPECIALIZATION FOR ROW-WISE REDUCTION OPERATIONS OF ROW-MAJOR MATRICES
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Expression object for row-wise row-major dense matrix reduction operations.
// \ingroup dense_vector_expression
//
// This specialization of the DMatReduceExpr class template represents the compile time expression
// for row-wise reduction operations of row-major dense matrices.
*/
template< typename MT    // Type of the dense matrix
        , typename OP >  // Type of the reduction operation
class DMatReduceExpr<1UL,MT,OP>
   : public MatReduceExpr<1UL, DenseVector< DMatReduceExpr<1UL,MT,OP>, false > >
   , private Computation
{
 private:
   //**Type definitions****************************************************************************
   using RT = ResultType_t<MT>;   //!< Result type of the dense matrix expression.
   using ET = ElementType_t<MT>;  //!< Element type of the dense matrix expression.
   //**********************************************************************************************

   //**Serial evaluation strategy******************************************************************
   //! Compilation switch for the serial evaluation strategy of the reduction expression.
   /*! The \a useAssign compile time constant expression represents a compilation switch for
       the serial evaluation strategy of the reduction expression. In case the dense matrix
       operand requires an intermediate evaluation, \a useAssign will be set to 1 and the
       reduction expression will be evaluated via the \a assign function family. Otherwise
       \a useAssign will be set to 0 and the expression will be evaluated via the subscript
       operator. */
   static constexpr bool useAssign = RequiresEvaluation_v<MT>;

   /*! \cond BLAZE_INTERNAL */
   //! Helper variable template for the explicit application of the SFINAE principle.
   template< typename VT >
   static constexpr bool UseAssign_v = useAssign;
   /*! \endcond */
   //**********************************************************************************************

   //**Parallel evaluation strategy****************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper variable template for the explicit application of the SFINAE principle.
   /*! This variable template is a helper for the selection of the parallel evaluation strategy.
       In case the dense matrix operand is not SMP assignable and requires an intermediate
       evaluation, the variable is set to 1 and the expression specific evaluation strategy
       is selected. Otherwise the variable is set to 0 and the default strategy is chosen. */
   template< typename VT >
   static constexpr bool UseSMPAssign_v = ( !MT::smpAssignable && useAssign );
   /*! \endcond */
   //**********************************************************************************************

 public:
   //**Type definitions****************************************************************************
   using This          = DMatReduceExpr<1UL,MT,OP>;    //!< Type of this DMatReduceExpr instance.
   using ResultType    = ReduceTrait_t<RT,OP,1UL>;     //!< Result type for expression template evaluations.
   using TransposeType = TransposeType_t<ResultType>;  //!< Transpose type for expression template evaluations.
   using ElementType   = ElementType_t<ResultType>;    //!< Resulting element type.
   using SIMDType      = SIMDTrait_t<ElementType>;     //!< Resulting SIMD element type.
   using ReturnType    = const ElementType;            //!< Return type for expression template evaluations.

   //! Data type for composite expression templates.
   using CompositeType = If_t< useAssign, const ResultType, const DMatReduceExpr& >;

   //! Composite type of the left-hand side dense matrix expression.
   using Operand = If_t< IsExpression_v<MT>, const MT, const MT& >;

   //! Data type of the custom unary operation.
   using Operation = OP;
   //**********************************************************************************************

   //**ConstIterator class definition**************************************************************
   /*!\brief Iterator over the elements of the dense vector.
   */
   class ConstIterator
   {
    public:
      //**Type definitions*************************************************************************
      using IteratorCategory = std::random_access_iterator_tag;  //!< The iterator category.
      using ValueType        = ElementType;                      //!< Type of the underlying elements.
      using PointerType      = ElementType*;                     //!< Pointer return type.
      using ReferenceType    = ElementType&;                     //!< Reference return type.
      using DifferenceType   = ptrdiff_t;                        //!< Difference between two iterators.

      // STL iterator requirements
      using iterator_category = IteratorCategory;  //!< The iterator category.
      using value_type        = ValueType;         //!< Type of the underlying elements.
      using pointer           = PointerType;       //!< Pointer return type.
      using reference         = ReferenceType;     //!< Reference return type.
      using difference_type   = DifferenceType;    //!< Difference between two iterators.
      //*******************************************************************************************

      //**Constructor******************************************************************************
      /*!\brief Constructor for the ConstIterator class.
      //
      // \param dm The dense matrix operand of the reduction expression.
      // \param index Index to the initial matrix row.
      // \param op The reduction operation.
      */
      explicit inline ConstIterator( Operand dm, size_t index, OP op )
         : dm_   ( dm    )  // Dense matrix of the reduction expression
         , index_( index )  // Index to the current matrix row
         , op_   ( op    )  // The reduction operation
      {}
      //*******************************************************************************************

      //**Addition assignment operator*************************************************************
      /*!\brief Addition assignment operator.
      //
      // \param inc The increment of the iterator.
      // \return The incremented iterator.
      */
      inline ConstIterator& operator+=( size_t inc ) {
         index_ += inc;
         return *this;
      }
      //*******************************************************************************************

      //**Subtraction assignment operator**********************************************************
      /*!\brief Subtraction assignment operator.
      //
      // \param dec The decrement of the iterator.
      // \return The decremented iterator.
      */
      inline ConstIterator& operator-=( size_t dec ) {
         index_ -= dec;
         return *this;
      }
      //*******************************************************************************************

      //**Prefix increment operator****************************************************************
      /*!\brief Pre-increment operator.
      //
      // \return Reference to the incremented iterator.
      */
      inline ConstIterator& operator++() {
         ++index_;
         return *this;
      }
      //*******************************************************************************************

      //**Postfix increment operator***************************************************************
      /*!\brief Post-increment operator.
      //
      // \return The previous position of the iterator.
      */
      inline const ConstIterator operator++( int ) {
         return ConstIterator( index_++ );
      }
      //*******************************************************************************************

      //**Prefix decrement operator****************************************************************
      /*!\brief Pre-decrement operator.
      //
      // \return Reference to the decremented iterator.
      */
      inline ConstIterator& operator--() {
         --index_;
         return *this;
      }
      //*******************************************************************************************

      //**Postfix decrement operator***************************************************************
      /*!\brief Post-decrement operator.
      //
      // \return The previous position of the iterator.
      */
      inline const ConstIterator operator--( int ) {
         return ConstIterator( index_-- );
      }
      //*******************************************************************************************

      //**Element access operator******************************************************************
      /*!\brief Direct access to the element at the current iterator position.
      //
      // \return The resulting value.
      */
      inline ReturnType operator*() const {
         return reduce( row( dm_, index_, unchecked ), op_ );
      }
      //*******************************************************************************************

      //**Equality operator************************************************************************
      /*!\brief Equality comparison between two ConstIterator objects.
      //
      // \param rhs The right-hand side iterator.
      // \return \a true if the iterators refer to the same element, \a false if not.
      */
      inline bool operator==( const ConstIterator& rhs ) const {
         return index_ == rhs.index_;
      }
      //*******************************************************************************************

      //**Inequality operator**********************************************************************
      /*!\brief Inequality comparison between two ConstIterator objects.
      //
      // \param rhs The right-hand side iterator.
      // \return \a true if the iterators don't refer to the same element, \a false if they do.
      */
      inline bool operator!=( const ConstIterator& rhs ) const {
         return index_ != rhs.index_;
      }
      //*******************************************************************************************

      //**Less-than operator***********************************************************************
      /*!\brief Less-than comparison between two ConstIterator objects.
      //
      // \param rhs The right-hand side iterator.
      // \return \a true if the left-hand side iterator is smaller, \a false if not.
      */
      inline bool operator<( const ConstIterator& rhs ) const {
         return index_ < rhs.index_;
      }
      //*******************************************************************************************

      //**Greater-than operator********************************************************************
      /*!\brief Greater-than comparison between two ConstIterator objects.
      //
      // \param rhs The right-hand side iterator.
      // \return \a true if the left-hand side iterator is greater, \a false if not.
      */
      inline bool operator>( const ConstIterator& rhs ) const {
         return index_ > rhs.index_;
      }
      //*******************************************************************************************

      //**Less-or-equal-than operator**************************************************************
      /*!\brief Less-than comparison between two ConstIterator objects.
      //
      // \param rhs The right-hand side iterator.
      // \return \a true if the left-hand side iterator is smaller or equal, \a false if not.
      */
      inline bool operator<=( const ConstIterator& rhs ) const {
         return index_ <= rhs.index_;
      }
      //*******************************************************************************************

      //**Greater-or-equal-than operator***********************************************************
      /*!\brief Greater-than comparison between two ConstIterator objects.
      //
      // \param rhs The right-hand side iterator.
      // \return \a true if the left-hand side iterator is greater or equal, \a false if not.
      */
      inline bool operator>=( const ConstIterator& rhs ) const {
         return index_ >= rhs.index_;
      }
      //*******************************************************************************************

      //**Subtraction operator*********************************************************************
      /*!\brief Calculating the number of elements between two iterators.
      //
      // \param rhs The right-hand side iterator.
      // \return The number of elements between the two iterators.
      */
      inline DifferenceType operator-( const ConstIterator& rhs ) const {
         return index_ - rhs.index_;
      }
      //*******************************************************************************************

      //**Addition operator************************************************************************
      /*!\brief Addition between a ConstIterator and an integral value.
      //
      // \param it The iterator to be incremented.
      // \param inc The number of elements the iterator is incremented.
      // \return The incremented iterator.
      */
      friend inline const ConstIterator operator+( const ConstIterator& it, size_t inc ) {
         return ConstIterator( it.index_ + inc );
      }
      //*******************************************************************************************

      //**Addition operator************************************************************************
      /*!\brief Addition between an integral value and a ConstIterator.
      //
      // \param inc The number of elements the iterator is incremented.
      // \param it The iterator to be incremented.
      // \return The incremented iterator.
      */
      friend inline const ConstIterator operator+( size_t inc, const ConstIterator& it ) {
         return ConstIterator( it.index_ + inc );
      }
      //*******************************************************************************************

      //**Subtraction operator*********************************************************************
      /*!\brief Subtraction between a ConstIterator and an integral value.
      //
      // \param it The iterator to be decremented.
      // \param dec The number of elements the iterator is decremented.
      // \return The decremented iterator.
      */
      friend inline const ConstIterator operator-( const ConstIterator& it, size_t dec ) {
         return ConstIterator( it.index_ - dec );
      }
      //*******************************************************************************************

    private:
      //**Member variables*************************************************************************
      Operand dm_;     //!< Dense matrix of the reduction expression.
      size_t  index_;  //!< Index to the current matrix row.
      OP      op_;     //!< The reduction operation.
      //*******************************************************************************************
   };
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation switch for the expression template evaluation strategy.
   static constexpr bool simdEnabled = false;

   //! Compilation switch for the expression template assignment strategy.
   static constexpr bool smpAssignable = MT::smpAssignable;
   //**********************************************************************************************

   //**Constructor*********************************************************************************
   /*!\brief Constructor for the DMatReduceExpr class.
   //
   // \param dm The matrix operand of the reduction expression.
   */
   explicit inline DMatReduceExpr( const MT& dm, OP op ) noexcept
      : dm_( dm )  // Dense matrix of the reduction expression
      , op_( op )  // The reduction operation
   {}
   //**********************************************************************************************

   //**Subscript operator**************************************************************************
   /*!\brief Subscript operator for the direct access to the vector elements.
   //
   // \param index Access index. The index has to be in the range \f$[0..N-1]\f$.
   // \return The resulting value.
   */
   inline ReturnType operator[]( size_t index ) const {
      BLAZE_INTERNAL_ASSERT( index < dm_.rows(), "Invalid vector access index" );
      return reduce( row( dm_, index, unchecked ), op_ );
   }
   //**********************************************************************************************

   //**At function*********************************************************************************
   /*!\brief Checked access to the vector elements.
   //
   // \param index Access index. The index has to be in the range \f$[0..N-1]\f$.
   // \return The resulting value.
   // \exception std::out_of_range Invalid vector access index.
   */
   inline ReturnType at( size_t index ) const {
      if( index >= dm_.rows() ) {
         BLAZE_THROW_OUT_OF_RANGE( "Invalid vector access index" );
      }
      return (*this)[index];
   }
   //**********************************************************************************************

   //**Begin function******************************************************************************
   /*!\brief Returns an iterator to the first non-zero element of the dense vector.
   //
   // \return Iterator to the first non-zero element of the dense vector.
   */
   inline ConstIterator begin() const {
      return ConstIterator( dm_, 0UL, op_ );
   }
   //**********************************************************************************************

   //**End function********************************************************************************
   /*!\brief Returns an iterator just past the last non-zero element of the dense vector.
   //
   // \return Iterator just past the last non-zero element of the dense vector.
   */
   inline ConstIterator end() const {
      return ConstIterator( dm_, size(), op_ );
   }
   //**********************************************************************************************

   //**Size function*******************************************************************************
   /*!\brief Returns the current size/dimension of the vector.
   //
   // \return The size of the vector.
   */
   inline size_t size() const noexcept {
      return dm_.rows();
   }
   //**********************************************************************************************

   //**Operand access******************************************************************************
   /*!\brief Returns the dense matrix operand.
   //
   // \return The dense matrix operand.
   */
   inline Operand operand() const noexcept {
      return dm_;
   }
   //**********************************************************************************************

   //**Operation access****************************************************************************
   /*!\brief Returns a copy of the reduction operation.
   //
   // \return A copy of the reduction operation.
   */
   inline Operation operation() const {
      return op_;
   }
   //**********************************************************************************************

   //**********************************************************************************************
   /*!\brief Returns whether the expression can alias with the given address \a alias.
   //
   // \param alias The alias to be checked.
   // \return \a true in case an aliasing effect is possible, \a false if not.
   */
   template< typename T >
   inline bool canAlias( const T* alias ) const noexcept {
      return ( dm_.isAliased( alias ) );
   }
   //**********************************************************************************************

   //**********************************************************************************************
   /*!\brief Returns whether the expression is aliased with the given address \a alias.
   //
   // \param alias The alias to be checked.
   // \return \a true in case the given alias is contained in this expression, \a false if not.
   */
   template< typename T >
   inline bool isAliased( const T* alias ) const noexcept {
      return ( dm_.isAliased( alias ) );
   }
   //**********************************************************************************************

   //**********************************************************************************************
   /*!\brief Returns whether the operands of the expression are properly aligned in memory.
   //
   // \return \a true in case the operands are aligned, \a false if not.
   */
   inline bool isAligned() const noexcept {
      return false;
   }
   //**********************************************************************************************

   //**********************************************************************************************
   /*!\brief Returns whether the expression can be used in SMP assignments.
   //
   // \return \a true in case the expression can be used in SMP assignments, \a false if not.
   */
   inline bool canSMPAssign() const noexcept {
      return dm_.canSMPAssign() || ( size() > SMP_DMATDVECMULT_THRESHOLD );  // TODO: rethink
   }
   //**********************************************************************************************

 private:
   //**Member variables****************************************************************************
   Operand   dm_;  //!< Dense matrix of the reduction expression.
   Operation op_;  //!< The reduction operation.
   //**********************************************************************************************

   //**Assignment to vectors***********************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Assignment of a row-wise row-major dense matrix reduction operation to a vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side vector.
   // \param rhs The right-hand side reduction expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized assignment of a row-wise row-major
   // dense matrix reduction expression to a vector. Due to the explicit application of the SFINAE
   // principle, this function can only be selected by the compiler in case the expression specific
   // parallel evaluation strategy is selected.
   */
   template< typename VT1 >  // Type of the target vector
   friend inline auto assign( Vector<VT1,false>& lhs, const DMatReduceExpr& rhs )
      -> EnableIf_t< UseAssign_v<VT1> >
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const RT tmp( serial( rhs.dm_ ) );  // Evaluation of the dense matrix operand
      assign( ~lhs, reduce<1UL>( tmp, rhs.op_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Addition assignment to vectors**************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Addition assignment of a row-wise row-major dense matrix reduction operation to a vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side vector.
   // \param rhs The right-hand side reduction expression to be added.
   // \return void
   //
   // This function implements the performance optimized addition assignment of a row-wise
   // row-major dense matrix reduction expression to a vector. Due to the explicit application
   // of the SFINAE principle, this function can only be selected by the compiler in case the
   // expression specific parallel evaluation strategy is selected.
   */
   template< typename VT1 >  // Type of the target vector
   friend inline auto addAssign( Vector<VT1,false>& lhs, const DMatReduceExpr& rhs )
      -> EnableIf_t< UseAssign_v<VT1> >
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const RT tmp( serial( rhs.dm_ ) );  // Evaluation of the dense matrix operand
      addAssign( ~lhs, reduce<1UL>( tmp, rhs.op_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Subtraction assignment to vectors***********************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Subtraction assignment of a row-wise row-major dense matrix reduction operation
   //        to a vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side vector.
   // \param rhs The right-hand side reduction expression to be subtracted.
   // \return void
   //
   // This function implements the performance optimized subtraction assignment of a row-wise
   // row-major dense matrix reduction expression to a vector. Due to the explicit application
   // of the SFINAE principle, this function can only be selected by the compiler in case the
   // expression specific parallel evaluation strategy is selected.
   */
   template< typename VT1 >  // Type of the target vector
   friend inline auto subAssign( Vector<VT1,false>& lhs, const DMatReduceExpr& rhs )
      -> EnableIf_t< UseAssign_v<VT1> >
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const RT tmp( serial( rhs.dm_ ) );  // Evaluation of the dense matrix operand
      subAssign( ~lhs, reduce<1UL>( tmp, rhs.op_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Multiplication assignment to vectors********************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Multiplication assignment of a row-wise row-major dense matrix reduction operation
   //        to a vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side vector.
   // \param rhs The right-hand side reduction expression to be multiplied.
   // \return void
   //
   // This function implements the performance optimized multiplication assignment of a row-wise
   // row-major dense matrix reduction expression to a vector. Due to the explicit application
   // of the SFINAE principle, this function can only be selected by the compiler in case the
   // expression specific parallel evaluation strategy is selected.
   */
   template< typename VT1 >  // Type of the target vector
   friend inline auto multAssign( Vector<VT1,false>& lhs, const DMatReduceExpr& rhs )
      -> EnableIf_t< UseAssign_v<VT1> >
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const RT tmp( serial( rhs.dm_ ) );  // Evaluation of the dense matrix operand
      multAssign( ~lhs, reduce<1UL>( tmp, rhs.op_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Division assignment to vectors**************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Division assignment of a row-wise row-major dense matrix reduction operation to a vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side vector.
   // \param rhs The right-hand side reduction expression divisor.
   // \return void
   //
   // This function implements the performance optimized division assignment of a row-wise
   // row-major dense matrix reduction expression to a vector. Due to the explicit application
   // of the SFINAE principle, this function can only be selected by the compiler in case the
   // expression specific parallel evaluation strategy is selected.
   */
   template< typename VT1 >  // Type of the target vector
   friend inline auto divAssign( Vector<VT1,false>& lhs, const DMatReduceExpr& rhs )
      -> EnableIf_t< UseAssign_v<VT1> >
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const RT tmp( serial( rhs.dm_ ) );  // Evaluation of the dense matrix operand
      divAssign( ~lhs, reduce<1UL>( tmp, rhs.op_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**SMP assignment to vectors*******************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief SMP assignment of a row-wise row-major dense matrix reduction operation to a vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side vector.
   // \param rhs The right-hand side reduction expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized SMP assignment of a row-wise row-major
   // dense matrix reduction expression to a vector. Due to the explicit application of the SFINAE
   // principle, this function can only be selected by the compiler in case the expression specific
   // parallel evaluation strategy is selected.
   */
   template< typename VT1 >  // Type of the target vector
   friend inline auto smpAssign( Vector<VT1,false>& lhs, const DMatReduceExpr& rhs )
      -> EnableIf_t< UseSMPAssign_v<VT1> >
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const RT tmp( rhs.dm_ );  // Evaluation of the dense matrix operand
      smpAssign( ~lhs, reduce<1UL>( tmp, rhs.op_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**SMP addition assignment to vectors**********************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief SMP addition assignment of a row-wise row-major dense matrix reduction operation
   //        to a vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side vector.
   // \param rhs The right-hand side reduction expression to be added.
   // \return void
   //
   // This function implements the performance optimized SMP addition assignment of a row-wise
   // row-major dense matrix reduction expression to a vector. Due to the explicit application
   // of the SFINAE principle, this function can only be selected by the compiler in case the
   // expression specific parallel evaluation strategy is selected.
   */
   template< typename VT1 >  // Type of the target vector
   friend inline auto smpAddAssign( Vector<VT1,false>& lhs, const DMatReduceExpr& rhs )
      -> EnableIf_t< UseSMPAssign_v<VT1> >
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const RT tmp( rhs.dm_ );  // Evaluation of the dense matrix operand
      smpAddAssign( ~lhs, reduce<1UL>( tmp, rhs.op_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**SMP subtraction assignment to vectors*******************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief SMP subtraction assignment of a row-wise row-major dense matrix reduction operation
   //        to a vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side vector.
   // \param rhs The right-hand side reduction expression to be subtracted.
   // \return void
   //
   // This function implements the performance optimized SMP subtraction assignment of a row-wise
   // row-major dense matrix reduction expression to a vector. Due to the explicit application
   // of the SFINAE principle, this function can only be selected by the compiler in case the
   // expression specific parallel evaluation strategy is selected.
   */
   template< typename VT1 >  // Type of the target vector
   friend inline auto smpSubAssign( Vector<VT1,false>& lhs, const DMatReduceExpr& rhs )
      -> EnableIf_t< UseSMPAssign_v<VT1> >
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const RT tmp( rhs.dm_ );  // Evaluation of the dense matrix operand
      smpSubAssign( ~lhs, reduce<1UL>( tmp, rhs.op_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**SMP multiplication assignment to vectors****************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief SMP multiplication assignment of a row-wise row-major dense matrix reduction operation
   //        to a vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side vector.
   // \param rhs The right-hand side reduction expression to be multiplied.
   // \return void
   //
   // This function implements the performance optimized SMP multiplication assignment of a
   // row-wise row-major dense matrix reduction expression to a vector. Due to the explicit
   // application of the SFINAE principle, this function can only be selected by the compiler
   // in case the expression specific parallel evaluation strategy is selected.
   */
   template< typename VT1 >  // Type of the target vector
   friend inline auto smpMultAssign( Vector<VT1,false>& lhs, const DMatReduceExpr& rhs )
      -> EnableIf_t< UseSMPAssign_v<VT1> >
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const RT tmp( rhs.dm_ );  // Evaluation of the dense matrix operand
      smpMultAssign( ~lhs, reduce<1UL>( tmp, rhs.op_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**SMP division assignment to vectors**********************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief SMP division assignment of a row-wise row-major dense matrix reduction operation
   //        to a vector.
   // \ingroup dense_vector
   //
   // \param lhs The target left-hand side vector.
   // \param rhs The right-hand side reduction expression divisor.
   // \return void
   //
   // This function implements the performance optimized SMP division assignment of a row-wise
   // row-major dense matrix reduction expression to a vector. Due to the explicit application
   // of the SFINAE principle, this function can only be selected by the compiler in case the
   // expression specific parallel evaluation strategy is selected.
   */
   template< typename VT1 >  // Type of the target vector
   friend inline auto smpDivAssign( Vector<VT1,false>& lhs, const DMatReduceExpr& rhs )
      -> EnableIf_t< UseSMPAssign_v<VT1> >
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).size() == rhs.size(), "Invalid vector sizes" );

      const RT tmp( rhs.dm_ );  // Evaluation of the dense matrix operand
      smpDivAssign( ~lhs, reduce<1UL>( tmp, rhs.op_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( MT );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT );
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************




//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Auxiliary helper struct for the dense matrix reduction operation.
// \ingroup dense_matrix
*/
template< typename MT    // Type of the dense matrix
        , typename OP >  // Type of the reduction operation
struct DMatReduceExprHelper
{
   //**Type definitions****************************************************************************
   //! Composite type of the dense matrix expression.
   using CT = RemoveReference_t< CompositeType_t<MT> >;

   //! Element type of the dense matrix expression.
   using ET = ElementType_t<CT>;

   //! Definition of the HasSIMDEnabled type trait.
   BLAZE_CREATE_HAS_DATA_OR_FUNCTION_MEMBER_TYPE_TRAIT( HasSIMDEnabled, simdEnabled );

   //! Definition of the HasLoad type trait.
   BLAZE_CREATE_HAS_DATA_OR_FUNCTION_MEMBER_TYPE_TRAIT( HasLoad, load );
   //**********************************************************************************************

   //**SIMD support detection**********************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the detection of the SIMD capabilities of the given custom operation.
   struct UseSIMDEnabledFlag {
      static constexpr bool test( bool (*fnc)() ) { return fnc(); }
      static constexpr bool test( bool b ) { return b; }
      static constexpr bool value = test( OP::BLAZE_TEMPLATE simdEnabled<ET,ET> );
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   static constexpr bool value =
      ( CT::simdEnabled &&
        If_t< HasSIMDEnabled_v<OP>, UseSIMDEnabledFlag, HasLoad<OP> >::value );
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  GLOBAL FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default backend implementation of the reduction of a row-major dense matrix.
// \ingroup dense_matrix
//
// \param dm The given row-major dense matrix for the reduction computation.
// \param op The reduction operation.
// \return The result of the reduction operation.
//
// This function implements the performance optimized reduction operation for a row-major dense
// matrix. Due to the explicit application of the SFINAE principle, this function can only be
// selected by the compiler in case vectorization cannot be applied.
*/
template< typename MT    // Type of the dense matrix
        , typename OP >  // Type of the reduction operation
inline auto dmatreduce( const DenseMatrix<MT,false>& dm, OP op )
   -> DisableIf_t< DMatReduceExprHelper<MT,OP>::value, ElementType_t<MT> >
{
   using CT = CompositeType_t<MT>;
   using ET = ElementType_t<MT>;

   const size_t M( (~dm).rows()    );
   const size_t N( (~dm).columns() );

   if( M == 0UL || N == 0UL ) return ET{};
   if( M == 1UL && N == 1UL ) return (~dm)(0UL,0UL);

   CT tmp( ~dm );

   BLAZE_INTERNAL_ASSERT( tmp.rows()    == M, "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( tmp.columns() == N, "Invalid number of columns" );

   ET redux0{};

   {
      redux0 = tmp(0UL,0UL);

      for( size_t j=1UL; j<N; ++j ) {
         redux0 = op( redux0, tmp(0UL,j) );
      }
   }

   size_t i( 1UL );

   for( ; (i+2UL) <= M; i+=2UL )
   {
      ET redux1( tmp(i    ,0UL) );
      ET redux2( tmp(i+1UL,0UL) );

      for( size_t j=1UL; j<N; ++j ) {
         redux1 = op( redux1, tmp(i    ,j) );
         redux2 = op( redux2, tmp(i+1UL,j) );
      }

      redux1 = op( redux1, redux2 );
      redux0 = op( redux0, redux1 );
   }

   if( i < M )
   {
      ET redux1( tmp(i,0UL) );

      for( size_t j=1UL; j<N; ++j ) {
         redux1 = op( redux1, tmp(i,j) );
      }

      redux0 = op( redux0, redux1 );
   }

   return redux0;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief SIMD optimized backend implementation of the reduction of a row-major dense matrix.
// \ingroup dense_matrix
//
// \param dm The given row-major dense matrix for the reduction computation.
// \param op The reduction operation.
// \return The result of the reduction operation.
//
// This function implements the performance optimized reduction operation for a row-major dense
// matrix. Due to the explicit application of the SFINAE principle, this function can only be
// selected by the compiler in case vectorization can be applied.
*/
template< typename MT    // Type of the dense matrix
        , typename OP >  // Type of the reduction operation
inline auto dmatreduce( const DenseMatrix<MT,false>& dm, OP op )
   -> EnableIf_t< DMatReduceExprHelper<MT,OP>::value, ElementType_t<MT> >
{
   using CT = CompositeType_t<MT>;
   using ET = ElementType_t<MT>;

   const size_t M( (~dm).rows()    );
   const size_t N( (~dm).columns() );

   if( M == 0UL || N == 0UL ) return ET{};

   CT tmp( ~dm );

   BLAZE_INTERNAL_ASSERT( tmp.rows()    == M, "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( tmp.columns() == N, "Invalid number of columns" );

   constexpr size_t SIMDSIZE = SIMDTrait<ET>::size;

   ET redux{};

   if( N >= SIMDSIZE )
   {
      const size_t jpos( N & size_t(-SIMDSIZE) );
      BLAZE_INTERNAL_ASSERT( ( N - ( N % SIMDSIZE ) ) == jpos, "Invalid end calculation" );

      SIMDTrait_t<ET> xmm1;

      {
         xmm1 = tmp.load(0UL,0UL);
         size_t j( SIMDSIZE );

         for( ; j<jpos; j+=SIMDSIZE ) {
            xmm1 = op( xmm1, tmp.load(0UL,j) );
         }
         for( ; j<N; ++j ) {
            xmm1[0] = op( xmm1[0], tmp(0UL,j) );
         }
      }

      size_t i( 1UL );

      for( ; (i+4UL) <= M; i+=4UL )
      {
         xmm1 = op( xmm1, tmp.load(i,0UL) );
         SIMDTrait_t<ET> xmm2( tmp.load(i+1UL,0UL) );
         SIMDTrait_t<ET> xmm3( tmp.load(i+2UL,0UL) );
         SIMDTrait_t<ET> xmm4( tmp.load(i+3UL,0UL) );
         size_t j( SIMDSIZE );

         for( ; j<jpos; j+=SIMDSIZE ) {
            xmm1 = op( xmm1, tmp.load(i    ,j) );
            xmm2 = op( xmm2, tmp.load(i+1UL,j) );
            xmm3 = op( xmm3, tmp.load(i+2UL,j) );
            xmm4 = op( xmm4, tmp.load(i+3UL,j) );
         }
         for( ; j<N; ++j ) {
            xmm1[0] = op( xmm1[0], tmp(i    ,j) );
            xmm2[0] = op( xmm2[0], tmp(i+1UL,j) );
            xmm3[0] = op( xmm3[0], tmp(i+2UL,j) );
            xmm4[0] = op( xmm4[0], tmp(i+3UL,j) );
         }

         xmm1 = op( xmm1, xmm2 );
         xmm3 = op( xmm3, xmm4 );
         xmm1 = op( xmm1, xmm3 );
      }

      if( i+2UL <= M )
      {
         xmm1 = op( xmm1, tmp.load(i,0UL) );
         SIMDTrait_t<ET> xmm2( tmp.load(i+1UL,0UL) );
         size_t j( SIMDSIZE );

         for( ; j<jpos; j+=SIMDSIZE ) {
            xmm1 = op( xmm1, tmp.load(i    ,j) );
            xmm2 = op( xmm2, tmp.load(i+1UL,j) );
         }
         for( ; j<N; ++j ) {
            xmm1[0] = op( xmm1[0], tmp(i    ,j) );
            xmm2[0] = op( xmm2[0], tmp(i+1UL,j) );
         }

         xmm1 = op( xmm1, xmm2 );

         i += 2UL;
      }

      if( i < M )
      {
         xmm1 = op( xmm1, tmp.load(i,0UL) );
         size_t j( SIMDSIZE );

         for( ; j<jpos; j+=SIMDSIZE ) {
            xmm1 = op( xmm1, tmp.load(i,j) );
         }
         for( ; j<N; ++j ) {
            xmm1[0] = op( xmm1[0], tmp(i,j) );
         }
      }

      if( IsSame_v<OP,Mult> ) {
         redux = prod( xmm1 );
      }
      else {
         redux = xmm1[0UL];
         for( size_t k=1UL; k<SIMDSIZE; ++k ) {
            redux = op( redux, xmm1[k] );
         }
      }
   }
   else
   {
      {
         redux = tmp(0UL,0UL);
         for( size_t j=1UL; j<N; ++j ) {
            redux = op( redux, tmp(0UL,j) );
         }
      }
      for( size_t i=1UL; i<M; ++i ) {
         for( size_t j=0UL; j<N; ++j ) {
            redux = op( redux, tmp(i,j) );
         }
      }
   }

   return redux;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief SIMD optimized backend implementation of the summation of a row-major dense matrix.
// \ingroup dense_matrix
//
// \param dm The given row-major dense matrix for the summation.
// \return The result of the summation.
//
// This function implements the performance optimized summation for a row-major dense matrix.
// Due to the explicit application of the SFINAE principle, this function can only be selected
// by the compiler in case vectorization can be applied.
*/
template< typename MT >  // Type of the dense matrix
inline auto dmatreduce( const DenseMatrix<MT,false>& dm, Add /*op*/ )
   -> EnableIf_t< DMatReduceExprHelper<MT,Add>::value, ElementType_t<MT> >
{
   using CT = CompositeType_t<MT>;
   using ET = ElementType_t<MT>;

   const size_t M( (~dm).rows()    );
   const size_t N( (~dm).columns() );

   if( M == 0UL || N == 0UL ) return ET{};

   CT tmp( ~dm );

   BLAZE_INTERNAL_ASSERT( tmp.rows()    == M, "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( tmp.columns() == N, "Invalid number of columns" );

   constexpr bool remainder( !usePadding || !IsPadded_v< RemoveReference_t<CT> > );
   constexpr size_t SIMDSIZE = SIMDTrait<ET>::size;

   ET redux{};

   if( !remainder || N >= SIMDSIZE )
   {
      const size_t jpos( ( remainder )?( N & size_t(-SIMDSIZE) ):( N ) );
      BLAZE_INTERNAL_ASSERT( !remainder || ( N - ( N % SIMDSIZE ) ) == jpos, "Invalid end calculation" );

      SIMDTrait_t<ET> xmm1;
      size_t i( 0UL );

      for( ; (i+4UL) <= M; i+=4UL )
      {
         xmm1 += tmp.load(i,0UL);
         SIMDTrait_t<ET> xmm2( tmp.load(i+1UL,0UL) );
         SIMDTrait_t<ET> xmm3( tmp.load(i+2UL,0UL) );
         SIMDTrait_t<ET> xmm4( tmp.load(i+3UL,0UL) );
         size_t j( SIMDSIZE );

         for( ; j<jpos; j+=SIMDSIZE ) {
            xmm1 += tmp.load(i    ,j);
            xmm2 += tmp.load(i+1UL,j);
            xmm3 += tmp.load(i+2UL,j);
            xmm4 += tmp.load(i+3UL,j);
         }
         for( ; remainder && j<N; ++j ) {
            redux += tmp(i    ,j);
            redux += tmp(i+1UL,j);
            redux += tmp(i+2UL,j);
            redux += tmp(i+3UL,j);
         }

         xmm1 += xmm2;
         xmm3 += xmm4;
         xmm1 += xmm3;
      }

      if( i+2UL <= M )
      {
         xmm1 += tmp.load(i,0UL);
         SIMDTrait_t<ET> xmm2( tmp.load(i+1UL,0UL) );
         size_t j( SIMDSIZE );

         for( ; j<jpos; j+=SIMDSIZE ) {
            xmm1 += tmp.load(i    ,j);
            xmm2 += tmp.load(i+1UL,j);
         }
         for( ; remainder && j<N; ++j ) {
            redux += tmp(i    ,j);
            redux += tmp(i+1UL,j);
         }

         xmm1 += xmm2;

         i += 2UL;
      }

      if( i < M )
      {
         xmm1 += tmp.load(i,0UL);
         size_t j( SIMDSIZE );

         for( ; j<jpos; j+=SIMDSIZE ) {
            xmm1 += tmp.load(i,j);
         }
         for( ; remainder && j<N; ++j ) {
            redux += tmp(i,j);
         }
      }

      redux += sum( xmm1 );
   }
   else
   {
      for( size_t i=0UL; i<M; ++i ) {
         for( size_t j=0UL; j<N; ++j ) {
            redux += tmp(i,j);
         }
      }
   }

   return redux;

}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default backend implementation of the reduction of a column-major dense matrix.
// \ingroup dense_matrix
//
// \param dm The given column-major dense matrix for the reduction computation.
// \param op The reduction operation.
// \return The result of the reduction operation.
//
// This function implements the performance optimized reduction operation for a column-major
// dense matrix. Due to the explicit application of the SFINAE principle, this function can
// only be selected by the compiler in case vectorization cannot be applied.
*/
template< typename MT    // Type of the dense matrix
        , typename OP >  // Type of the reduction operation
inline ElementType_t<MT> dmatreduce( const DenseMatrix<MT,true>& dm, OP op )
{
   return dmatreduce( trans( ~dm ), op );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Performs a custom reduction operation on the given dense matrix.
// \ingroup dense_matrix
//
// \param dm The given dense matrix for the reduction computation.
// \param op The reduction operation.
// \return The result of the reduction operation.
//
// This function reduces the given dense matrix \a dm by means of the given reduction operation
// \a op:

   \code
   blaze::DynamicMatrix<double> a;
   // ... Resizing and initialization
   const double sum = reduce( a, Add() );
   \endcode

// Please note that the evaluation order of the reduction operation is unspecified. Thus the
// behavior is non-deterministic if \a op is not associative or not commutative. Also, the
// operation is undefined if the given reduction operation modifies the values.
*/
template< typename MT    // Type of the dense matrix
        , bool SO        // Storage order
        , typename OP >  // Type of the reduction operation
inline decltype(auto) reduce( const DenseMatrix<MT,SO>& dm, OP op )
{
   BLAZE_FUNCTION_TRACE;

   return dmatreduce( ~dm, op );
}
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Backend implementation for custom reduction operations on row-major dense matrices.
// \ingroup dense_matrix
//
// \param dm The given row-major dense matrix for the reduction computation.
// \param op The reduction operation.
// \return The result of the reduction operation.
*/
template< size_t RF      // Reduction flag
        , typename MT    // Type of the dense matrix
        , typename OP >  // Type of the reduction operation
inline const DMatReduceExpr<RF,MT,OP> reduce_backend( const DenseMatrix<MT,false>& dm, OP op )
{
   return DMatReduceExpr<RF,MT,OP>( ~dm, op );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Backend implementation for custom reduction operations on column-major dense matrices.
// \ingroup dense_matrix
//
// \param dm The given column-major dense matrix for the reduction computation.
// \param op The reduction operation.
// \return The result of the reduction operation.
*/
template< size_t RF      // Reduction flag
        , typename MT    // Type of the dense matrix
        , typename OP >  // Type of the reduction operation
inline decltype(auto) reduce_backend( const DenseMatrix<MT,true>& dm, OP op )
{
   return trans( reduce<1UL-RF>( trans( ~dm ), op ) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Performs a custom reduction operation on the given dense matrix.
// \ingroup dense_matrix
//
// \param dm The given dense matrix for the reduction computation.
// \param op The reduction operation.
// \return The result of the reduction operation.
//
// This function reduces the rows or columns of the given dense matrix \a dm by means of the
// given reduction operation \a op. In case the reduction flag \a RF is set to 0, the elements
// of the matrix are reduced along the 0th dimension of the matrix (i.e. column-wise) and the
// result is a row vector. In case \a RF is set to 1, the elements of the matrix are reduced
// along the 1st dimension of the matrix (i.e. row-wise) and the result is a column vector:

   \code
   blaze::DynamicMatrix<double> A;
   // ... Resizing and initialization
   const blaze::DynamicVector<double,rowVector> colsum( reduce<0UL>( A, Add() ) );
   \endcode

   \code
   blaze::DynamicMatrix<double> A;
   // ... Resizing and initialization
   const blaze::DynamicVector<double,columnVector> rowsum( reduce<1UL>( A, Add() ) );
   \endcode

// Please note that the evaluation order of the reduction operation is unspecified. Thus the
// behavior is non-deterministic if \a op is not associative or not commutative. Also, the
// operation is undefined if the given reduction operation modifies the values.
*/
template< size_t RF      // Reduction flag
        , typename MT    // Type of the dense matrix
        , bool SO        // Storage order
        , typename OP >  // Type of the reduction operation
inline decltype(auto) reduce( const DenseMatrix<MT,SO>& dm, OP op )
{
   BLAZE_FUNCTION_TRACE;

   BLAZE_STATIC_ASSERT_MSG( RF < 2UL, "Invalid reduction flag" );

   return reduce_backend<RF>( ~dm, op );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Reduces the given dense matrix by means of addition.
// \ingroup dense_matrix
//
// \param dm The given dense matrix for the reduction operation.
// \return The result of the reduction operation.
//
// This function reduces the given dense matrix \a dm by means of addition:

   \code
   blaze::DynamicMatrix<int> a{ { 1, 2 }, { 3, 4 } };
   // ... Resizing and initialization
   const int sum = sum( a );  // Results in 10
   \endcode

// Please note that the evaluation order of the reduction operation is unspecified.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline decltype(auto) sum( const DenseMatrix<MT,SO>& dm )
{
   BLAZE_FUNCTION_TRACE;

   return reduce( ~dm, Add() );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Reduces the given dense matrix by means of addition.
// \ingroup dense_matrix
//
// \param dm The given dense matrix for the reduction operation.
// \return The result of the reduction operation.
//
// This function reduces the rows or columns of the given dense matrix \a dm by means of
// addition. In case the reduction flag \a RF is set to 0, the elements of the matrix are
// reduced along the 0th dimension of the matrix (i.e. column-wise) and the result is a row
// vector. In case \a RF is set to 1, the elements of the matrix are reduced along the 1st
// dimension of the matrix (i.e. row-wise) and the result is a column vector:

   \code
   blaze::DynamicMatrix<int> A{ { 1, 0, 2 }, { 1, 3, 4 } };
   // ... Resizing and initialization
   const blaze::DynamicVector<int,rowVector> colsum = sum<0UL>( A );  // Results in { 2, 3, 6 }
   \endcode

   \code
   blaze::DynamicMatrix<int> A{ { 1, 0, 2 }, { 1, 3, 4 } };
   // ... Resizing and initialization
   const blaze::DynamicVector<int,columnVector> rowsum = sum<1UL>( A );  // Results in { 3, 8 }
   \endcode

// Please note that the evaluation order of the reduction operation is unspecified.
*/
template< size_t RF    // Reduction flag
        , typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline decltype(auto) sum( const DenseMatrix<MT,SO>& dm )
{
   BLAZE_FUNCTION_TRACE;

   return reduce<RF>( ~dm, Add() );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Reduces the given dense matrix by means of multiplication.
// \ingroup dense_matrix
//
// \param dm The given dense matrix for the reduction operation.
// \return The result of the reduction operation.
//
// This function reduces the given dense matrix \a dm by means of multiplication:

   \code
   blaze::DynamicMatrix<int> a{ { 1, 2 }, { 3, 4 } };
   // ... Resizing and initialization
   const int prod = prod( a );  // Results in 24
   \endcode

// Please note that the evaluation order of the reduction operation is unspecified.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline decltype(auto) prod( const DenseMatrix<MT,SO>& dm )
{
   BLAZE_FUNCTION_TRACE;

   return reduce( ~dm, Mult() );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Reduces the given dense matrix by means of multiplication.
// \ingroup dense_matrix
//
// \param dm The given dense matrix for the reduction operation.
// \return The result of the reduction operation.
//
// This function reduces the rows or columns of the given dense matrix \a dm by means of
// multiplication. In case the reduction flag \a RF is set to 0, the elements of the matrix
// are reduced along the 0th dimension of the matrix (i.e. column-wise) and the result is a
// row vector. In case \a RF is set to 1, the elements of the matrix are reduced along the
// 1st dimension of the matrix (i.e. row-wise) and the result is a column vector:

   \code
   blaze::DynamicMatrix<int> A{ { 1, 0, 2 }, { 1, 3, 4 } };
   // ... Resizing and initialization
   const blaze::DynamicVector<int,rowVector> colprod = prod<0UL>( A );  // Results in { 1, 0, 8 }
   \endcode

   \code
   blaze::DynamicMatrix<int> A{ { 1, 0, 2 }, { 1, 3, 4 } };
   // ... Resizing and initialization
   const blaze::DynamicVector<int,columnVector> rowprod = prod<1UL>( A );  // Results in { 0, 12 }
   \endcode

// Please note that the evaluation order of the reduction operation is unspecified.
*/
template< size_t RF    // Reduction flag
        , typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline decltype(auto) prod( const DenseMatrix<MT,SO>& dm )
{
   BLAZE_FUNCTION_TRACE;

   return reduce<RF>( ~dm, Mult() );
}
//*************************************************************************************************

} // namespace blaze

#endif
