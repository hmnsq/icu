// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1997-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#اگر_غیر_تعریف_شدہ FILTEREDBRK_H
#تعریف_کرو FILTEREDBRK_H

#شامل "unicode/utypes.h"

#اگر U_SHOW_CPLUSPLUS_API

#شامل "unicode/brkiter.h"

#اگر !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

U_NAMESPACE_BEGIN

/**
 * \file
 * \brief C++ API: FilteredBreakIteratorBuilder
 */

/**
 * The BreakIteratorFilter is used to modify the behavior of a BreakIterator
 *  by constructing a new BreakIterator which suppresses certain segment boundaries.
 *  See  http://www.unicode.org/reports/tr35/tr35-general.html#Segmentation_Exceptions .
 *  For example, a typical English Sentence Break Iterator would break on the space
 *  in the string "Mr. Smith" (resulting in two segments),
 *  but with "Mr." as an exception, a filtered break iterator
 *  would consider the string "Mr. Smith" to be a single segment.
 *
 * @stable ICU 56
 */
class U_COMMON_API FilteredBreakIteratorBuilder : عوامی UObject {
 عوامی:
  /**
   *  destructor.
   * @stable ICU 56
   */
  مجازی ~FilteredBreakIteratorBuilder();

  /**
   * Construct a FilteredBreakIteratorBuilder based on rules in a locale.
   * The rules are taken from CLDR exception data for the locale,
   *  see http://www.unicode.org/reports/tr35/tr35-general.html#Segmentation_Exceptions
   *  This is the equivalent of calling createInstance(UErrorCode&)
   *    and then repeatedly calling addNoBreakAfter(...) with the contents
   *    of the CLDR exception data.
   * @param where the locale.
   * @param status The error code.
   * @return the new builder
   * @stable ICU 56
   */
  ساکن FilteredBreakIteratorBuilder *createInstance(ثابت Locale& where, UError_رمز& حالت_2);

#اگر_غیر_تعریف_شدہ U_HIDE_DEPRECATED_API
  /**
   * This function has been deprecated in favor of createEmptyInstance, which has
   * identical behavior.
   * @param status The error code.
   * @return the new builder
   * @deprecated ICU 60 use createEmptyInstance instead
   * @see createEmptyInstance()
   */
  ساکن FilteredBreakIteratorBuilder *createInstance(UError_رمز &حالت_2);
#اگر_ختم  /* U_HIDE_DEPRECATED_API */

  /**
   * Construct an empty FilteredBreakIteratorBuilder.
   * In this state, it will not suppress any segment boundaries.
   * @param status The error code.
   * @return the new builder
   * @stable ICU 60
   */
  ساکن FilteredBreakIteratorBuilder *create_خالی_خانے_Instance(UError_رمز &حالت_2);

  /**
   * Suppress a certain string from being the end of a segment.
   * For example, suppressing "Mr.", then segments ending in "Mr." will not be returned
   * by the iterator.
   * @param string the string to suppress, such as "Mr."
   * @param status error code
   * @return returns true if the string was not present and now added,
   * false if the call was a no-op because the string was already being suppressed.
   * @stable ICU 56
   */
  مجازی UBool suppressBreakAfter(ثابت UnicodeString& string, UError_رمز& حالت_2) = 0;

  /**
   * Stop suppressing a certain string from being the end of the segment.
   * This function does not create any new segment boundaries, but only serves to un-do
   * the effect of earlier calls to suppressBreakAfter, or to un-do the effect of
   * locale data which may be suppressing certain strings.
   * @param string the exception to remove
   * @param status error code
   * @return returns true if the string was present and now removed,
   * false if the call was a no-op because the string was not being suppressed.
   * @stable ICU 56
   */
  مجازی UBool unsuppressBreakAfter(ثابت UnicodeString& string, UError_رمز& حالت_2) = 0;

#اگر_غیر_تعریف_شدہ U_FORCE_HIDE_DEPRECATED_API
  /**
   * This function has been deprecated in favor of wrapIteratorWithFilter()
   * The behavior is identical.
   * @param adoptBreakIterator the break iterator to adopt
   * @param status error code
   * @return the new BreakIterator, owned by the caller.
   * @deprecated ICU 60 use wrapIteratorWithFilter() instead
   * @see wrapBreakIteratorWithFilter()
   */
  مجازی BreakIterator *build(BreakIterator* adoptBreakIterator, UError_رمز& حالت_2) = 0;
#اگر_ختم  // U_FORCE_HIDE_DEPRECATED_API

  /**
   * Wrap (adopt) an existing break iterator in a new filtered instance.
   * The resulting BreakIterator is owned by the caller.
   * The BreakIteratorFilter may be destroyed before the BreakIterator is destroyed.
   * Note that the adoptBreakIterator is adopted by the new BreakIterator
   * and should no longer be used by the caller.
   * The FilteredBreakIteratorBuilder may be reused.
   * This function is an alias for build()
   * @param adoptBreakIterator the break iterator to adopt
   * @param status error code
   * @return the new BreakIterator, owned by the caller.
   * @stable ICU 60
   */
  اندرونی BreakIterator *wrapIteratorWithFilter(BreakIterator* adoptBreakIterator, UError_رمز& حالت_2) {
    واپس build(adoptBreakIterator, حالت_2);
  }

 محفوظ:
  /**
   * For subclass use
   * @stable ICU 56
   */
  FilteredBreakIteratorBuilder();
};


U_NAMESPACE_انتہا

#اگر_ختم // #if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

#اگر_ختم /* U_SHOW_CPLUSPLUS_API */

#اگر_ختم // #ifndef FILTEREDBRK_H
