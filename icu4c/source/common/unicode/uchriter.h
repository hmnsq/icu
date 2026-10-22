// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1998-2005, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#اگر_تعریف_نشده UCHRITER_H
#تعریف_کن UCHRITER_H

#ضمن "unicode/utypes.h"

#إذا U_SHOW_CPLUSPLUS_API

#ضمن "unicode/chariter.h"

/**
 * \file 
 * \brief C++ API: char16_t Character Iterator
 */
 
U_NAMESPACE_BEGIN

/**
 * A concrete subclass of CharacterIterator that iterates over the
 * characters (code units or code points) in a char16_t array.
 * It's possible not only to create an
 * iterator that iterates over an entire char16_t array, but also to
 * create one that iterates over only a subrange of a char16_t array
 * (iterators over different subranges of the same char16_t array don't
 * compare equal).
 * @see CharacterIterator
 * @see ForwardCharacterIterator
 * @stable ICU 2.0
 */
class U_COMMON_API UCharCharacterIterator : عام CharacterIterator {
عام:
  /**
   * Create an iterator over the char16_t array referred to by "textPtr".
   * The iteration range is 0 to <code>length-1</code>.
   * text is only aliased, not adopted (the
   * destructor will not delete it).
   * @param textPtr The char16_t array to be iterated over
   * @param length The length of the char16_t array
   * @stable ICU 2.0
   */
  UCharCharacterIterator(ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND, int32_t درازی);

  /**
   * Create an iterator over the char16_t array referred to by "textPtr".
   * The iteration range is 0 to <code>length-1</code>.
   * text is only aliased, not adopted (the
   * destructor will not delete it).
   * The starting
   * position is specified by "position". If "position" is outside the valid
   * iteration range, the behavior of this object is undefined.
   * @param textPtr The char16_t array to be iterated over
   * @param length The length of the char16_t array
   * @param position The starting position of the iteration
   * @stable ICU 2.0
   */
  UCharCharacterIterator(ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND, int32_t درازی,
                         int32_t position);

  /**
   * Create an iterator over the char16_t array referred to by "textPtr".
   * The iteration range is 0 to <code>end-1</code>.
   * text is only aliased, not adopted (the
   * destructor will not delete it).
   * The starting
   * position is specified by "position". If begin and end do not
   * form a valid iteration range or "position" is outside the valid
   * iteration range, the behavior of this object is undefined.
   * @param textPtr The char16_t array to be iterated over
   * @param length The length of the char16_t array
   * @param textBegin  The begin position of the iteration range
   * @param textEnd    The end position of the iteration range
   * @param position    The starting position of the iteration
   * @stable ICU 2.0
   */
  UCharCharacterIterator(ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND, int32_t درازی,
                         int32_t متن_Begin,
                         int32_t متن_انتها,
                         int32_t position);

  /**
   * Copy constructor.  The new iterator iterates over the same range
   * of the same string as "that", and its initial position is the
   * same as "that"'s current position.
   * @param that The UCharCharacterIterator to be copied
   * @stable ICU 2.0
   */
  UCharCharacterIterator(ثابت UCharCharacterIterator&  that);

  /**
   * Destructor.
   * @stable ICU 2.0
   */
  مجازی ~UCharCharacterIterator();

  /**
   * Assignment operator.  *this is altered to iterate over the sane
   * range of the same string as "that", and refers to the same
   * character within that string as "that" does.
   * @param that The object to be copied
   * @return the newly created object
   * @stable ICU 2.0
   */
  UCharCharacterIterator& عملگر=(ثابت UCharCharacterIterator& that) U_LIFETIME_BOUND;

  /**
   * Returns true if the iterators iterate over the same range of the
   * same string and are pointing at the same character.
   * @param that The ForwardCharacterIterator used to be compared for equality
   * @return true if the iterators iterate over the same range of the
   * same string and are pointing at the same character.
   * @stable ICU 2.0
   */
  مجازی bool           عملگر==(ثابت ForwardCharacterIterator& that) ثابت override;

  /**
   * Generates a hash code for this iterator.
   * @return the hash code.
   * @stable ICU 2.0
   */
  مجازی int32_t hash_کد() ثابت override;

  /**
   * Returns a new UCharCharacterIterator referring to the same
   * character in the same range of the same string as this one.  The
   * caller must delete the new iterator.
   * @return the CharacterIterator newly created
   * @stable ICU 2.0
   */
  مجازی UCharCharacterIterator* clone() ثابت override;

  /**
   * Sets the iterator to refer to the first code unit in its
   * iteration range, and returns that code unit.
   * This can be used to begin an iteration with next().
   * @return the first code unit in its iteration range.
   * @stable ICU 2.0
   */
  مجازی char16_t اولی() override;

  /**
   * Sets the iterator to refer to the first code unit in its
   * iteration range, returns that code unit, and moves the position
   * to the second code unit. This is an alternative to setToStart()
   * for forward iteration with nextPostInc().
   * @return the first code unit in its iteration range
   * @stable ICU 2.0
   */
  مجازی char16_t اولی_Post_Inc() override;

  /**
   * Sets the iterator to refer to the first code point in its
   * iteration range, and returns that code unit,
   * This can be used to begin an iteration with next32().
   * Note that an iteration with next32PostInc(), beginning with,
   * e.g., setToStart() or firstPostInc(), is more efficient.
   * @return the first code point in its iteration range
   * @stable ICU 2.0
   */
  مجازی UChar32 first32() override;

  /**
   * Sets the iterator to refer to the first code point in its
   * iteration range, returns that code point, and moves the position
   * to the second code point. This is an alternative to setToStart()
   * for forward iteration with next32PostInc().
   * @return the first code point in its iteration range.
   * @stable ICU 2.0
   */
  مجازی UChar32 first32PostInc() override;

  /**
   * Sets the iterator to refer to the last code unit in its
   * iteration range, and returns that code unit.
   * This can be used to begin an iteration with previous().
   * @return the last code unit in its iteration range.
   * @stable ICU 2.0
   */
  مجازی char16_t آخرین() override;

  /**
   * Sets the iterator to refer to the last code point in its
   * iteration range, and returns that code unit.
   * This can be used to begin an iteration with previous32().
   * @return the last code point in its iteration range.
   * @stable ICU 2.0
   */
  مجازی UChar32 last32() override;

  /**
   * Sets the iterator to refer to the "position"-th code unit
   * in the text-storage object the iterator refers to, and
   * returns that code unit.
   * @param position the position within the text-storage object
   * @return the code unit
   * @stable ICU 2.0
   */
  مجازی char16_t         مجموعه_شاخص(int32_t position) override;

  /**
   * Sets the iterator to refer to the beginning of the code point
   * that contains the "position"-th code unit
   * in the text-storage object the iterator refers to, and
   * returns that code point.
   * The current position is adjusted to the beginning of the code point
   * (its first code unit).
   * @param position the position within the text-storage object
   * @return the code unit
   * @stable ICU 2.0
   */
  مجازی UChar32       مجموعه_Index32(int32_t position) override;

  /**
   * Returns the code unit the iterator currently refers to.
   * @return the code unit the iterator currently refers to.
   * @stable ICU 2.0
   */
  مجازی char16_t current() ثابت override;

  /**
   * Returns the code point the iterator currently refers to.
   * @return the code point the iterator currently refers to.
   * @stable ICU 2.0
   */
  مجازی UChar32 current32() ثابت override;

  /**
   * Advances to the next code unit in the iteration range (toward
   * endIndex()), and returns that code unit.  If there are no more
   * code units to return, returns DONE.
   * @return the next code unit in the iteration range.
   * @stable ICU 2.0
   */
  مجازی char16_t بعدی() override;

  /**
   * Gets the current code unit for returning and advances to the next code unit
   * in the iteration range
   * (toward endIndex()).  If there are
   * no more code units to return, returns DONE.
   * @return the current code unit.
   * @stable ICU 2.0
   */
  مجازی char16_t بعدی_Post_Inc() override;

  /**
   * Advances to the next code point in the iteration range (toward
   * endIndex()), and returns that code point.  If there are no more
   * code points to return, returns DONE.
   * Note that iteration with "pre-increment" semantics is less
   * efficient than iteration with "post-increment" semantics
   * that is provided by next32PostInc().
   * @return the next code point in the iteration range.
   * @stable ICU 2.0
   */
  مجازی UChar32 next32() override;

  /**
   * Gets the current code point for returning and advances to the next code point
   * in the iteration range
   * (toward endIndex()).  If there are
   * no more code points to return, returns DONE.
   * @return the current point.
   * @stable ICU 2.0
   */
  مجازی UChar32 next32PostInc() override;

  /**
   * Returns false if there are no more code units or code points
   * at or after the current position in the iteration range.
   * This is used with nextPostInc() or next32PostInc() in forward
   * iteration.
   * @return false if there are no more code units or code points
   * at or after the current position in the iteration range.
   * @stable ICU 2.0
   */
  مجازی UBool        has_بعدی() override;

  /**
   * Advances to the previous code unit in the iteration range (toward
   * startIndex()), and returns that code unit.  If there are no more
   * code units to return, returns DONE.
   * @return the previous code unit in the iteration range.
   * @stable ICU 2.0
   */
  مجازی char16_t قبلی() override;

  /**
   * Advances to the previous code point in the iteration range (toward
   * startIndex()), and returns that code point.  If there are no more
   * code points to return, returns DONE.
   * @return the previous code point in the iteration range.
   * @stable ICU 2.0
   */
  مجازی UChar32 previous32() override;

  /**
   * Returns false if there are no more code units or code points
   * before the current position in the iteration range.
   * This is used with previous() or previous32() in backward
   * iteration.
   * @return false if there are no more code units or code points
   * before the current position in the iteration range.
   * @stable ICU 2.0
   */
  مجازی UBool        has_قبلی() override;

  /**
   * Moves the current position relative to the start or end of the
   * iteration range, or relative to the current position itself.
   * The movement is expressed in numbers of code units forward
   * or backward by specifying a positive or negative delta.
   * @param delta the position relative to origin. A positive delta means forward;
   * a negative delta means backward.
   * @param origin Origin enumeration {kStart, kCurrent, kEnd}
   * @return the new position
   * @stable ICU 2.0
   */
  مجازی int32_t      move(int32_t delta, EOrigin origin) override;

  /**
   * Moves the current position relative to the start or end of the
   * iteration range, or relative to the current position itself.
   * The movement is expressed in numbers of code points forward
   * or backward by specifying a positive or negative delta.
   * @param delta the position relative to origin. A positive delta means forward;
   * a negative delta means backward.
   * @param origin Origin enumeration {kStart, kCurrent, kEnd}
   * @return the new position
   * @stable ICU 2.0
   */
#اگر_تعریف_شده move32
   // One of the system headers right now is sometimes defining a conflicting macro we don't use
#لغو_تعریف move32
#پایان_اگر
  مجازی int32_t      move32(int32_t delta, EOrigin origin) override;

  /**
   * Sets the iterator to iterate over a new range of text
   * @stable ICU 2.0
   */
  فراغ مجموعه_متن(ConstChar16Ptr new_متن, int32_t new_متن_درازی);

  /**
   * Copies the char16_t array under iteration into the UnicodeString
   * referred to by "result".  Even if this iterator iterates across
   * only a part of this string, the whole string is copied.
   * @param result Receives a copy of the text under iteration.
   * @stable ICU 2.0
   */
  مجازی فراغ            get_متن(UnicodeString& نتیجه) override;

  /**
   * Return a class ID for this class (not really public)
   * @return a class ID for this class
   * @stable ICU 2.0
   */
  ساكن UClassID U_EXPORT2 getStaticClassID();

  /**
   * Return a class ID for this object (not really public)
   * @return a class ID for this object.
   * @stable ICU 2.0
   */
  مجازی UClassID getDynamicClassID() ثابت override;

محمي:
  /**
   * Protected constructor
   * @stable ICU 2.0
   */
  UCharCharacterIterator();
  /**
   * Protected member text
   * @stable ICU 2.0
   */
  ثابت char16_t*            متن;

};

U_NAMESPACE_انتها

#پایان_اگر /* U_SHOW_CPLUSPLUS_API */

#پایان_اگر
