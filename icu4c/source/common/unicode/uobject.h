// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2002-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  uobject.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002jun26
*   created by: Markus W. Scherer
*/

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __UOBJECT_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __UOBJECT_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ U_SHOW_CPLUSPLUS_API

#ಅನ್ತರ್ಭಾವ "unicode/platform.h"

/**
 * \file
 * \brief C++ API: Common ICU base class UObject.
 */

/**
 * \def U_NO_THROW
 *         Since ICU 64, use noexcept instead.
 *
 *         Previously, define this to define the throw() specification so
 *                 certain functions do not throw any exceptions
 *
 *         UMemory operator new methods should have the throw() specification 
 *         appended to them, so that the compiler adds the additional nullptr check 
 *         before calling constructors. Without, if <code>operator new</code> returns nullptr the
 *         constructor is still called, and if the constructor references member 
 *         data, (which it typically does), the result is a segmentation violation.
 *
 * @stable ICU 4.2. Since ICU 64, Use noexcept instead. See ICU-20422.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_NO_THROW
#ವ್ಯಾಖ್ಯಾನಿಸು U_NO_THROW noexcept
#ಆದರೆ_ಅಂತ್ಯ

/*===========================================================================*/
/* UClassID-based RTTI */
/*===========================================================================*/

/**
 * UClassID is used to identify classes without using the compiler's RTTI.
 * This was used before C++ compilers consistently supported RTTI.
 * ICU 4.6 requires compiler RTTI to be turned on.
 *
 * Each class hierarchy which needs
 * to implement polymorphic clone() or operator==() defines two methods,
 * described in detail below.  UClassID values can be compared using
 * operator==(). Nothing else should be done with them.
 *
 * \par
 * In class hierarchies that implement "poor man's RTTI",
 * each concrete subclass implements getDynamicClassID() in the same way:
 *
 * \code
 *      class Derived {
 *      public:
 *          virtual UClassID getDynamicClassID() const
 *            { return Derived::getStaticClassID(); }
 *      }
 * \endcode
 *
 * Each concrete class implements getStaticClassID() as well, which allows
 * clients to test for a specific type.
 *
 * \code
 *      class Derived {
 *      public:
 *          static UClassID U_EXPORT2 getStaticClassID();
 *      private:
 *          static char fgClassID;
 *      }
 *
 *      // In Derived.cpp:
 *      UClassID Derived::getStaticClassID()
 *        { return (UClassID)&Derived::fgClassID; }
 *      char Derived::fgClassID = 0; // Value is irrelevant
 * \endcode
 * @stable ICU 2.0
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ* UClassID;

U_NAMESPACE_BEGIN

/**
 * UMemory is the common ICU base class.
 * All other ICU C++ classes are derived from UMemory (starting with ICU 2.4).
 *
 * This is primarily to make it possible and simple to override the
 * C++ memory management by adding new/delete operators to this base class.
 *
 * To override ALL ICU memory management, including that from plain C code,
 * replace the allocation functions declared in cmemory.h
 *
 * UMemory does not contain any virtual functions.
 * Common "boilerplate" functions are defined in UObject.
 *
 * @stable ICU 2.4
 */
class U_COMMON_API UMemory {
ಸಾರ್ವಜನಿಕ:

/* test versions for debugging shaper heap memory problems */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SHAPER_ಸ್ಮರಣೆ_DEBUG  
    ಸ್ಥಿರ ಶೂನ್ಯ * NewArray(ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_3, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2);
    ಸ್ಥಿರ ಶೂನ್ಯ * GrowArray(ಶೂನ್ಯ * array, ಪೂರ್ಣಾಂಕ new_ಗಾತ್ರ );
    ಸ್ಥಿರ ಶೂನ್ಯ   FreeArray(ಶೂನ್ಯ * array );
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ U_OVERRIDE_CXX_ಹಂಚಿಕೆ
    /**
     * Override for ICU4C C++ memory management.
     * simple, non-class types are allocated using the macros in common/cmemory.h
     * (uprv_malloc(), uprv_free(), uprv_realloc());
     * they or something else could be used here to implement C++ new/delete
     * for ICU4C C++ classes
     * @stable ICU 2.4
     */
    ಸ್ಥಿರ ಶೂನ್ಯ * U_EXPORT2 ಕ್ರಿಯಾಕಾರಕ new(size_t ಗಾತ್ರ_3) noexcept;

    /**
     * Override for ICU4C C++ memory management.
     * See new().
     * @stable ICU 2.4
     */
    ಸ್ಥಿರ ಶೂನ್ಯ * U_EXPORT2 ಕ್ರಿಯಾಕಾರಕ new[](size_t ಗಾತ್ರ_3) noexcept;

    /**
     * Override for ICU4C C++ memory management.
     * simple, non-class types are allocated using the macros in common/cmemory.h
     * (uprv_malloc(), uprv_free(), uprv_realloc());
     * they or something else could be used here to implement C++ new/delete
     * for ICU4C C++ classes
     * @stable ICU 2.4
     */
    ಸ್ಥಿರ ಶೂನ್ಯ U_EXPORT2 ಕ್ರಿಯಾಕಾರಕ delete(ಶೂನ್ಯ *p) noexcept;

    /**
     * Override for ICU4C C++ memory management.
     * See delete().
     * @stable ICU 2.4
     */
    ಸ್ಥಿರ ಶೂನ್ಯ U_EXPORT2 ಕ್ರಿಯಾಕಾರಕ delete[](ಶೂನ್ಯ *p) noexcept;

    /**
     * Override for ICU4C C++ memory management for STL.
     * See new().
     * @stable ICU 2.6
     */
    ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ * U_EXPORT2 ಕ್ರಿಯಾಕಾರಕ new(size_t, ಶೂನ್ಯ *ptr) noexcept { ಹಿಂತಿರುಗಿ ptr; }

    /**
     * Override for ICU4C C++ memory management for STL.
     * See delete().
     * @stable ICU 2.6
     */
    ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ U_EXPORT2 ಕ್ರಿಯಾಕಾರಕ delete(ಶೂನ್ಯ *, ಶೂನ್ಯ *) noexcept {}

#ಆದರೆ U_HAVE_DEBUG_LOCATION_NEW
    /**
      * This method overrides the MFC debug version of the operator new
      * 
      * @param size   The requested memory size
      * @param file   The file where the allocation was requested
      * @param line   The line where the allocation was requested 
      */ 
    ಸ್ಥಿರ ಶೂನ್ಯ * U_EXPORT2 ಕ್ರಿಯಾಕಾರಕ new(size_t ಗಾತ್ರ_3, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* file, ಪೂರ್ಣಾಂಕ ಸಾಲು) noexcept;
    /**
      * This method provides a matching delete for the MFC debug new
      * 
      * @param p      The pointer to the allocated memory
      * @param file   The file where the allocation was requested
      * @param line   The line where the allocation was requested 
      */ 
    ಸ್ಥಿರ ಶೂನ್ಯ U_EXPORT2 ಕ್ರಿಯಾಕಾರಕ delete(ಶೂನ್ಯ* p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* file, ಪೂರ್ಣಾಂಕ ಸಾಲು) noexcept;
#ಆದರೆ_ಅಂತ್ಯ /* U_HAVE_DEBUG_LOCATION_NEW */
#ಆದರೆ_ಅಂತ್ಯ /* U_OVERRIDE_CXX_ALLOCATION */

    /*
     * Assignment operator not declared. The compiler will provide one
     * which does nothing since this class does not contain any data members.
     * API/code coverage may show the assignment operator as present and
     * untested - ignore.
     * Subclasses need this assignment operator if they use compiler-provided
     * assignment operators of their own. An alternative to not declaring one
     * here would be to declare and empty-implement a protected or public one.
    UMemory &UMemory::operator=(const UMemory &);
     */
};

/**
 * UObject is the common ICU "boilerplate" class.
 * UObject inherits UMemory (starting with ICU 2.4),
 * and all other public ICU C++ classes
 * are derived from UObject (starting with ICU 2.2).
 *
 * UObject contains common virtual functions, in particular a virtual destructor.
 *
 * The clone() function is not available in UObject because it is not
 * implemented by all ICU classes.
 * Many ICU services provide a clone() function for their class trees,
 * defined on the service's C++ base class
 * (which itself is a subclass of UObject).
 *
 * @stable ICU 2.2
 */
class U_COMMON_API UObject : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    /**
     * Destructor.
     *
     * @stable ICU 2.2
     */
    ವಾಸ್ತವಿಕ ~UObject();

    /**
     * ICU4C "poor man's RTTI", returns a UClassID for the actual ICU class.
     * The base class implementation returns a dummy value.
     *
     * Use compiler RTTI rather than ICU's "poor man's RTTI".
     * Since ICU 4.6, new ICU C++ class hierarchies do not implement "poor man's RTTI".
     *
     * @stable ICU 2.2
     */
    ವಾಸ್ತವಿಕ UClassID getDynamicClassID() ಸ್ಥಿರಾಂಕ;

ಸಂರಕ್ಷಿತ:
    // the following functions are protected to prevent instantiation and
    // direct use of UObject itself

    // default constructor
    // inline UObject() {}

    // copy constructor
    // inline UObject(const UObject &other) {}

#ಆದರೆ 0
    // TODO Sometime in the future. Implement operator==().
    // (This comment inserted in 2.2)
    // some or all of the following "boilerplate" functions may be made public
    // in a future ICU4C release when all subclasses implement them

    // assignment operator
    // (not virtual, see "Taligent's Guide to Designing Programs" pp.73..74)
    // commented out because the implementation is the same as a compiler's default
    // UObject &operator=(const UObject &other) { return *this; }

    // comparison operators
    ವಾಸ್ತವಿಕ ಸಾಲೊಳಗೆ bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ UObject &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ this==&other; }
    ಸಾಲೊಳಗೆ bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ UObject &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ !ಕ್ರಿಯಾಕಾರಕ==(other); }

    // clone() commented out from the base class:
    // some compilers do not support co-variant return types
    // (i.e., subclasses would have to return UObject * as well, instead of SubClass *)
    // see also UObject class documentation.
    // virtual UObject *clone() const;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Assignment operator not declared. The compiler will provide one
     * which does nothing since this class does not contain any data members.
     * API/code coverage may show the assignment operator as present and
     * untested - ignore.
     * Subclasses need this assignment operator if they use compiler-provided
     * assignment operators of their own. An alternative to not declaring one
     * here would be to declare and empty-implement a protected or public one.
    UObject &UObject::operator=(const UObject &);
     */
};

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_HIDE_ಆಂತರಿಕ_API
/**
 * This is a simple macro to add ICU RTTI to an ICU object implementation.
 * This does not go into the header. This should only be used in *.cpp files.
 *
 * @param myClass The name of the class that needs RTTI defined.
 * @internal
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UOBJECT_DEFINE_RTTI_IMPLEMENTATION(myClass) \
    UClassID U_EXPORT2 myClass::getStaticClassID() { \
        ಸ್ಥಿರ ಅಕ್ಷರ classID = 0; \
        ಹಿಂತಿರುಗಿ (UClassID)&classID; \
    } \
    UClassID myClass::getDynamicClassID() ಸ್ಥಿರಾಂಕ \
    { ಹಿಂತಿರುಗಿ myClass::getStaticClassID(); }


/**
 * This macro adds ICU RTTI to an ICU abstract class implementation.
 * This macro should be invoked in *.cpp files.  The corresponding
 * header should declare getStaticClassID.
 *
 * @param myClass The name of the class that needs RTTI defined.
 * @internal
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UOBJECT_DEFINE_ABSTRACT_RTTI_IMPLEMENTATION(myClass) \
    UClassID U_EXPORT2 myClass::getStaticClassID() { \
        ಸ್ಥಿರ ಅಕ್ಷರ classID = 0; \
        ಹಿಂತಿರುಗಿ (UClassID)&classID; \
    }

#ಆದರೆ_ಅಂತ್ಯ  /* U_HIDE_INTERNAL_API */

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* U_SHOW_CPLUSPLUS_API */

#ಆದರೆ_ಅಂತ್ಯ
