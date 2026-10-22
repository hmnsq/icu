// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1997-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#నిర్వచితం_కాకపోతే FILTEREDBRK_H
#నిర్వచించు FILTEREDBRK_H

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ U_SHOW_CPLUSPLUS_API

#అన్తర్భావ "unicode/brkiter.h"

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

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
class U_COMMON_API FilteredBreakIteratorBuilder : బహిరంగ UObject {
 బహిరంగ:
  /**
   *  destructor.
   * @stable ICU 56
   */
  మాయా ~FilteredBreakIteratorBuilder();

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
  స్థిర FilteredBreakIteratorBuilder *createInstance(స్థిరరాశి Locale& where, UError_సంకేతం& స్థితి_2);

#నిర్వచితం_కాకపోతే U_HIDE_DEPRECATED_API
  /**
   * This function has been deprecated in favor of createEmptyInstance, which has
   * identical behavior.
   * @param status The error code.
   * @return the new builder
   * @deprecated ICU 60 use createEmptyInstance instead
   * @see createEmptyInstance()
   */
  స్థిర FilteredBreakIteratorBuilder *createInstance(UError_సంకేతం &స్థితి_2);
#అయితే_ముగింపు  /* U_HIDE_DEPRECATED_API */

  /**
   * Construct an empty FilteredBreakIteratorBuilder.
   * In this state, it will not suppress any segment boundaries.
   * @param status The error code.
   * @return the new builder
   * @stable ICU 60
   */
  స్థిర FilteredBreakIteratorBuilder *create_ఖాళీలు_Instance(UError_సంకేతం &స్థితి_2);

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
  మాయా UBool suppressBreakAfter(స్థిరరాశి UnicodeString& string, UError_సంకేతం& స్థితి_2) = 0;

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
  మాయా UBool unsuppressBreakAfter(స్థిరరాశి UnicodeString& string, UError_సంకేతం& స్థితి_2) = 0;

#నిర్వచితం_కాకపోతే U_FORCE_HIDE_DEPRECATED_API
  /**
   * This function has been deprecated in favor of wrapIteratorWithFilter()
   * The behavior is identical.
   * @param adoptBreakIterator the break iterator to adopt
   * @param status error code
   * @return the new BreakIterator, owned by the caller.
   * @deprecated ICU 60 use wrapIteratorWithFilter() instead
   * @see wrapBreakIteratorWithFilter()
   */
  మాయా BreakIterator *build(BreakIterator* adoptBreakIterator, UError_సంకేతం& స్థితి_2) = 0;
#అయితే_ముగింపు  // U_FORCE_HIDE_DEPRECATED_API

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
  అంతఃవరుస BreakIterator *wrapIteratorWithFilter(BreakIterator* adoptBreakIterator, UError_సంకేతం& స్థితి_2) {
    తిరిగి build(adoptBreakIterator, స్థితి_2);
  }

 రక్షిత:
  /**
   * For subclass use
   * @stable ICU 56
   */
  FilteredBreakIteratorBuilder();
};


U_NAMESPACE_చివర

#అయితే_ముగింపు // #if !UCONFIG_NO_BREAK_ITERATION && !UCONFIG_NO_FILTERED_BREAK_ITERATION

#అయితే_ముగింపు /* U_SHOW_CPLUSPLUS_API */

#అయితే_ముగింపు // #ifndef FILTEREDBRK_H
