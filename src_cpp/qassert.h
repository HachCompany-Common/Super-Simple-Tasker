/*===========================================================================
* (HA)rdware Real-Time Operating System (HARTOS) for ARM Cortex-M
*
* Copyright (C) 2006-2023 Quantum Leaps, <state-machine.com>
*
* SPDX-License-Identifier: MIT
*
* See the accompanying LICENSE file or visit <opensource.org/licenses/MIT>.
===========================================================================*/
#ifndef QP_INC_QASSERT_H_
#define QP_INC_QASSERT_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*$declare${DbC} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${DbC::Q_NORETURN} .......................................................*/
#ifndef Q_NORETURN
/*! No-return function specifier for the Q_onAssert() callback function.
*
* The `Q_NORETURN` macro is supposed to be defined in the QP/C port
* (file `qep_port.h`). If such definition is NOT porvided, the default
* definition assumes only `void` type returned from Q_onAssert().
*
* @tr{PQA01_4}
*/
#define Q_NORETURN void
#endif /* ndef Q_NORETURN */

/*${DbC::Q_DEFINE_THIS_FILE} ...............................................*/
/*! Define the file name (with `__FILE__`) for assertions in this file
*
* @details
* Macro to be placed at the top of each C/C++ module to define the
* single instance of the file name string to be used in reporting
* assertions in this module.
*
* @note
* The file name string literal is defined by means of the standard
* preprocessor macro `__FILE__`. However, please note that, depending
* on the compiler, the `__FILE__` macro might contain the whole path name
* to the file, which might be inconvenient to log assertions.
*
* @attention
* This macro should **not** be terminated by a semicolon.
*
* @sa Q_DEFINE_THIS_MODULE()
*/
#define Q_DEFINE_THIS_FILE Q_DEFINE_THIS_MODULE(__FILE__)

/*${DbC::Q_DEFINE_THIS_MODULE} .............................................*/
#ifndef Q_NASSERT
/*! Define the user-specified module name for assertions in this file.
*
* @details
* Macro to be placed at the top of each C/C++ module to define the
* single instance of the module name string to be used in reporting
* assertions in this module. This macro takes the user-supplied parameter
* `name_` instead of `__FILE__` to precisely control the name of the
* module.
*
* @param[in] name_ string constant representing the module name
*
* @note
* This macro should **not** be terminated by a semicolon.
*/
#define Q_DEFINE_THIS_MODULE(name_) \
    static char const Q_this_module_[] = name_;
#endif /* ndef Q_NASSERT */

/*${DbC::Q_DEFINE_THIS_MODULE} .............................................*/
#ifdef Q_NASSERT
/*! inactive version of Q_DEFINE_THIS_MODULE() */
#define Q_DEFINE_THIS_MODULE(name_)
#endif /* def Q_NASSERT */

/*${DbC::Q_ASSERT_ID} ......................................................*/
#ifndef Q_NASSERT
/*! General purpose assertion with user-specified ID number.
*
* @details
* Makes sure the `test_` parameter is TRUE. Calls the Q_onAssert()
* callback if the `test_` evaluates to FALSE. This assertion takes the
* user-supplied parameter `id_` to identify the location of this
* assertion within the file. This avoids the volatility of using line
* numbers, which change whenever a line of code is added or removed
* upstream from the assertion.
*
* @param[in] id_   ID number (unique within the module) of the assertion
* @param[in] expr_ Boolean expression to check
*
* @note
* The `test_` expression is **not** evaluated if assertions are
* disabled with the ::Q_NASSERT switch.
*/
#define Q_ASSERT_ID(id_, expr_) ((expr_) \
    ? ((void)0) : Q_onAssert(&Q_this_module_[0], (id_)))
#endif /* ndef Q_NASSERT */

/*${DbC::Q_ASSERT_ID} ......................................................*/
#ifdef Q_NASSERT
/*! inactive version of Q_ASSERT_ID() */
#define Q_ASSERT_ID(id_, expr_) ((void)0)
#endif /* def Q_NASSERT */

/*${DbC::Q_ERROR_ID} .......................................................*/
#ifndef Q_NASSERT
/*! Assertion with user-specified ID for a wrong path through the code
*
* @details
* Calls the Q_onAssert() callback if ever executed. This assertion
* takes the user-supplied parameter `id_` to identify the location of
* this assertion within the file. This avoids the volatility of using
* line numbers, which change whenever a line of code is added or removed
* upstream from the assertion.
*
* @param[in] id_   ID number (unique within the module) of the assertion
*
* @note
* Does noting if assertions are disabled with the ::Q_NASSERT switch.
*/
#define Q_ERROR_ID(id_) Q_onAssert(&Q_this_module_[0], (id_))
#endif /* ndef Q_NASSERT */

/*${DbC::Q_ERROR_ID} .......................................................*/
#ifdef Q_NASSERT
/*! inactive version of Q_ERROR_ID() */
#define Q_ERROR_ID(id_) ((void)0)
#endif /* def Q_NASSERT */

/*${DbC::Q_ALLEGE_ID} ......................................................*/
#ifndef Q_NASSERT
/*! General purpose assertion with user-specified ID number that
* **always** evaluates the `expr_` expression.
*
* @details
* Like the Q_ASSERT_ID() macro, except it **always** evaluates the
* `expr_` expression even when assertions are disabled with the
* ::Q_NASSERT macro. However, when the ::Q_NASSERT macro is defined, the
* Q_onAssert() callback is **not** called, even if `expr_` evaluates
* to FALSE.
*
* @param[in] id_   ID number (unique within the module) of the assertion
* @param[in] expr_ Boolean expression to check
*/
#define Q_ALLEGE_ID(id_, expr_) Q_ASSERT_ID((id_), (expr_))
#endif /* ndef Q_NASSERT */

/*${DbC::Q_ALLEGE_ID} ......................................................*/
#ifdef Q_NASSERT
/*! inactive version of Q_ALLEGE_ID()
*
* @attention
* The expression `expr_` **is** executed.
*/
#define Q_ALLEGE_ID(id_, expr_) ((void)(expr_))
#endif /* def Q_NASSERT */

/*${DbC::Q_REQUIRE_ID} .....................................................*/
/*! Assertion for checking preconditions.
*
* @details
* This macro is equivalent to Q_ASSERT_ID(), except the name provides a better
* documentation of the intention of this assertion.
*
* @param[in] id_   ID number (unique within the module) of the assertion
* @param[in] expr_ Boolean expression
*/
#define Q_REQUIRE_ID(id_, expr_) Q_ASSERT_ID((id_), (expr_))

/*${DbC::Q_REQUIRE} ........................................................*/
/*! Assertion for checking preconditions (based on __LINE__).
*
* @details
* Equivalent to Q_ASSERT(), except the name provides a better documentation
* of the intention of this assertion.
*
* @param[in] expr_ Boolean expression
*/
#define Q_REQUIRE(expr_) Q_REQUIRE_ID(__LINE__, (expr_))

/*${DbC::Q_ENSURE_ID} ......................................................*/
/*! Assertion for checking postconditions.
*
* @details
* This macro is equivalent to Q_ASSERT_ID(), except the name provides a better
* documentation of the intention of this assertion.
*
* @param[in] id_   ID number (unique within the module) of the assertion
* @param[in] expr_ Boolean expression
*/
#define Q_ENSURE_ID(id_, expr_) Q_ASSERT_ID((id_), (expr_))

/*${DbC::Q_ENSURE} .........................................................*/
/*! Assertion for checking postconditions.
*
* @details
* Equivalent to Q_ASSERT(), except the name provides a better documentation
* of the intention of this assertion.
*
* @param[in] expr_ Boolean expression
*/
#define Q_ENSURE(expr_) Q_ENSURE_ID(__LINE__, (expr_))

/*${DbC::Q_INVARIANT_ID} ...................................................*/
/*! Assertion for checking invariants.
*
* @details
* Equivalent to Q_ASSERT(), except the name provides a better documentation
* of the intention of this assertion.
*
* @param[in] id_   ID number (unique within the module) of the assertion
* @param[in] expr_ Boolean expression
*/
#define Q_INVARIANT_ID(id_, expr_) Q_ASSERT_ID((id_), (expr_))

/*${DbC::Q_INVARIANT} ......................................................*/
/*! Assertion for checking invariants.
*
* @details
* Equivalent to Q_ASSERT(), except the name provides a better documentation
* of the intention of this assertion.
*
* @param[in] expr_ Boolean expression
*/
#define Q_INVARIANT(expr_) Q_INVARIANT_ID(__LINE__, (expr_))

/*${DbC::Q_ASSERT} .........................................................*/
/*! Assertion for a wrong path through the code (based on __LINE__)
*
* @details
* Calls the Q_onAssert() callback if ever executed.
*
* @note
* This macro identifies the problem location with the line number,
* which might change as the code is modified.
*
* @sa Q_ASSERT_ID()
*/
#define Q_ASSERT(expr_) Q_ASSERT_ID(__LINE__, (expr_))

/*${DbC::Q_ERROR} ..........................................................*/
/*! Assertion for a wrong path through the code
*
* @details
* Calls the Q_onAssert() callback if ever executed.
*
* @note
* This macro identifies the problem location with the line number,
* which might change as the code is modified.
*
* @sa Q_ERROR_ID()
*/
#define Q_ERROR() Q_ERROR_ID(__LINE__)

/*${DbC::Q_ALLEGE} .........................................................*/
/*! General purpose assertion with user-specified ID number that
* **always** evaluates the `expr_` expression.
*
* @details
* Like the Q_ASSERT_ID() macro, except it **always** evaluates the
* `expr_` expression even when assertions are disabled with the
* ::Q_NASSERT macro. However, when the ::Q_NASSERT macro is defined, the
* Q_onAssert() callback is **not** called, even if `expr_` evaluates
* to FALSE.
*
* @param[in] expr_ Boolean expression to check
*
* @sa Q_ALLEGE_ID()
*/
#define Q_ALLEGE(expr_) Q_ALLEGE_ID(__LINE__, (expr_))

/*${DbC::Q_ASSERT_STATIC} ..................................................*/
/*! Static (compile-time) assertion.
*
* @details
* This type of assertion deliberately causes a compile-time error when
* the `expr_` Boolean expression evaluates to FALSE. The macro exploits
* the fact that in C/C++ a dimension of an array cannot be negative.
* The compile-time assertion has no runtime side effects.
*
* @param[in] expr_ Compile-time Boolean expression
*
* @note
* The static assertion macro is provided for backwards compatibility with
* older C standards. Newer C11 supports `_Static_assert()`, which should
* be used instead of Q_ASSERT_STATIC().
*/
#define Q_ASSERT_STATIC(expr_) extern char Q_static_assert_[(expr_) ? 1 : -1]

/*${DbC::Q_ASSERT_COMPILE} .................................................*/
/*! Static (compile-time) assertion.
*
* @deprecated
* Use Q_ASSERT_STATIC() or better yet `_Static_assert()` instead.
*/
#define Q_ASSERT_COMPILE(expr_) Q_ASSERT_STATIC(expr_)

/*${DbC::int_t} ............................................................*/
#ifndef QP_VERSION
/*! typedef for assertions-ids and line numbers in assertions.
*
* @details
* This typedef specifies integer type for exclusive use in assertions.
* Use of this type, rather than plain 'int', is in compliance
* with the MISRA-C 2012 Dir 4.6 (adv).
*/
typedef int int_t;
#endif /* ndef QP_VERSION */

/*${DbC::Q_onAssert} .......................................................*/
/*! Callback function invoked in case of an assertion failure.
*
* @details
* This is an application-specific callback function needs to be defined in
* the application to perform the clean system shutdown and perhaps a reset.
* The Q_onAssert() function is the last line of defense after the
* system failure and its implementation shouild be very **carefully**
* designed and **tested** under various fault conditions, including but
* not limited to: stack overflow, stack corruption, or calling Q_onAssert()
* from an interrupt.

* @param[in] module name of the file/module in which the assertion failed
*                   (constant, zero-terminated C string)
* @param[in] location location of the assertion within the module. This could
*                   be a line number or a user-specified ID-number.
*
* @returns
* This callback function should **not return** (see ::Q_NORETURN),
* as continuation after an assertion failure does not make sense.
*
* @note
* It is typically a **bad idea** to implement Q_onAssert() as an endless
* loop that ties up the CPU. During debuggin, Q_onAssert() is an ideal
* place to put a breakpoint.
*
* @tr{PQA01_4}
*
* Called by the following: Q_ASSERT_ID(), Q_ERROR_ID(), Q_REQUIRE_ID(),
* Q_ENSURE_ID(), Q_INVARIANT_ID() and Q_ALLEGE_ID() as well as:
* Q_ASSERT(), Q_ERROR(), Q_REQUIRE(), Q_ENSURE(), Q_INVARIANT(),
* and Q_ALLEGE().
*/
Q_NORETURN Q_onAssert(
    char const * module,
    int_t location);

/*${DbC::Q_DIM} ............................................................*/
#ifndef QP_VERSION
/*! Helper macro to calculate static dimension of a 1-dim `array_`
*
* @param array_ 1-dimensional array
* @returns the length of the array (number of elements it can hold)
*/
#define Q_DIM(array_) (sizeof(array_) / sizeof((array_)[0U]))
#endif /* ndef QP_VERSION */
/*$enddecl${DbC} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

#ifdef __cplusplus
    }
#endif

#endif /* QP_INC_QASSERT_H_ */
