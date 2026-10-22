// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1998-2005, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*
* File schriter.h
*
* Modification History:
*
*   Date        Name        Description
*  05/05/99     stephen     Cleaned up.
******************************************************************************
*/

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SCHRITER_H
#ವ್ಯಾಖ್ಯಾನಿಸು SCHRITER_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ U_SHOW_CPLUSPLUS_API

#ಅನ್ತರ್ಭಾವ "unicode/chariter.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchriter.h"

/**
 * \file 
 * \brief C++ API: String Character Iterator
 */
 
U_NAMESPACE_BEGIN
/**
 * A concrete subclass of CharacterIterator that iterates over the
 * characters (code units or code points) in a UnicodeString.
 * It's possible not only to create an
 * iterator that iterates over an entire UnicodeString, but also to
 * create one that iterates over only a subrange of a UnicodeString
 * (iterators over different subranges of the same UnicodeString don't
 * compare equal).
 * @see CharacterIterator
 * @see ForwardCharacterIterator
 * @stable ICU 2.0
 */
class U_COMMON_API StringCharacterIterator : ಸಾರ್ವಜನಿಕ UCharCharacterIterator {
ಸಾರ್ವಜನಿಕ:
  /**
   * Create an iterator over the UnicodeString referred to by "textStr".
   * The UnicodeString object is copied.
   * The iteration range is the whole string, and the starting position is 0.
   * @param textStr The unicode string used to create an iterator
   * @stable ICU 2.0
   */
  StringCharacterIterator(ಸ್ಥಿರಾಂಕ UnicodeString& ಪಠ್ಯ_Str);

  /**
   * Create an iterator over the UnicodeString referred to by "textStr".
   * The iteration range is the whole string, and the starting
   * position is specified by "textPos".  If "textPos" is outside the valid
   * iteration range, the behavior of this object is undefined.
   * @param textStr The unicode string used to create an iterator
   * @param textPos The starting position of the iteration
   * @stable ICU 2.0
   */
  StringCharacterIterator(ಸ್ಥಿರಾಂಕ UnicodeString&    ಪಠ್ಯ_Str,
              int32_t              ಪಠ್ಯ_Pos);

  /**
   * Create an iterator over the UnicodeString referred to by "textStr".
   * The UnicodeString object is copied.
   * The iteration range begins with the code unit specified by
   * "textBegin" and ends with the code unit BEFORE the code unit specified
   * by "textEnd".  The starting position is specified by "textPos".  If
   * "textBegin" and "textEnd" don't form a valid range on "text" (i.e.,
   * textBegin >= textEnd or either is negative or greater than text.size()),
   * or "textPos" is outside the range defined by "textBegin" and "textEnd",
   * the behavior of this iterator is undefined.
   * @param textStr    The unicode string used to create the StringCharacterIterator
   * @param textBegin  The begin position of the iteration range
   * @param textEnd    The end position of the iteration range
   * @param textPos    The starting position of the iteration
   * @stable ICU 2.0
   */
  StringCharacterIterator(ಸ್ಥಿರಾಂಕ UnicodeString&    ಪಠ್ಯ_Str,
              int32_t              ಪಠ್ಯ_Begin,
              int32_t              ಪಠ್ಯ_ಕೊನೆ,
              int32_t              ಪಠ್ಯ_Pos);

  /**
   * Copy constructor.  The new iterator iterates over the same range
   * of the same string as "that", and its initial position is the
   * same as "that"'s current position.
   * The UnicodeString object in "that" is copied.
   * @param that The StringCharacterIterator to be copied
   * @stable ICU 2.0
   */
  StringCharacterIterator(ಸ್ಥಿರಾಂಕ StringCharacterIterator&  that);

  /**
   * Destructor.
   * @stable ICU 2.0
   */
  ವಾಸ್ತವಿಕ ~StringCharacterIterator();

  /**
   * Assignment operator.  *this is altered to iterate over the same
   * range of the same string as "that", and refers to the same
   * character within that string as "that" does.
   * @param that The object to be copied.
   * @return the newly created object.
   * @stable ICU 2.0
   */
  StringCharacterIterator& ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ StringCharacterIterator& that) U_LIFETIME_BOUND;

  /**
   * Returns true if the iterators iterate over the same range of the
   * same string and are pointing at the same character.
   * @param that The ForwardCharacterIterator to be compared for equality
   * @return true if the iterators iterate over the same range of the
   * same string and are pointing at the same character.
   * @stable ICU 2.0
   */
  ವಾಸ್ತವಿಕ bool           ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ ForwardCharacterIterator& that) ಸ್ಥಿರಾಂಕ override;

  /**
   * Returns a new StringCharacterIterator referring to the same
   * character in the same range of the same string as this one.  The
   * caller must delete the new iterator.
   * @return the newly cloned object.
   * @stable ICU 2.0
   */
  ವಾಸ್ತವಿಕ StringCharacterIterator* clone() ಸ್ಥಿರಾಂಕ override;

  /**
   * Sets the iterator to iterate over the provided string.
   * @param newText The string to be iterated over
   * @stable ICU 2.0
   */
  ಶೂನ್ಯ ಗಣ_ಪಠ್ಯ(ಸ್ಥಿರಾಂಕ UnicodeString& new_ಪಠ್ಯ);

  /**
   * Copies the UnicodeString under iteration into the UnicodeString
   * referred to by "result".  Even if this iterator iterates across
   * only a part of this string, the whole string is copied.
   * @param result Receives a copy of the text under iteration.
   * @stable ICU 2.0
   */
  ವಾಸ್ತವಿಕ ಶೂನ್ಯ            get_ಪಠ್ಯ(UnicodeString& ಫಲಿತಾಂಶ) override;

  /**
   * Return a class ID for this object (not really public)
   * @return a class ID for this object.
   * @stable ICU 2.0
   */
  ವಾಸ್ತವಿಕ UClassID getDynamicClassID() ಸ್ಥಿರಾಂಕ override;

  /**
   * Return a class ID for this class (not really public)
   * @return a class ID for this class
   * @stable ICU 2.0
   */
  ಸ್ಥಿರ UClassID U_EXPORT2 getStaticClassID();

ಸಂರಕ್ಷಿತ:
  /**
   * Default constructor, iteration over empty string.
   * @stable ICU 2.0
   */
  StringCharacterIterator();

  /**
   * Copy of the iterated string object.
   * @stable ICU 2.0
   */
  UnicodeString            ಪಠ್ಯ;

};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* U_SHOW_CPLUSPLUS_API */

#ಆದರೆ_ಅಂತ್ಯ
