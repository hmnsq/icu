// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  messageimpl.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2011apr04
*   created by: Markus W. Scherer
*/

#నిర్వచితం_కాకపోతే __MESSAGEIMPL_H__
#నిర్వచించు __MESSAGEIMPL_H__

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_FORMATTING

#అన్తర్భావ "unicode/messagepattern.h"

U_NAMESPACE_BEGIN

/**
 * Helper functions for use of MessagePattern.
 * In Java, these are package-private methods in MessagePattern itself.
 * In C++, they are declared here and implemented in messagepattern.cpp.
 */
class U_COMMON_API సందేశం_Impl {
బహిరంగ:
    /**
     * @return true if getApostropheMode()==UMSGPAT_APOS_DOUBLE_REQUIRED
     */
    స్థిర UBool jdk_Apos_బహుళకం(స్థిరరాశి సందేశం_నమూనా &సందేశం_నమూనా_2) {
        తిరిగి సందేశం_నమూనా_2.get_Apostrophe_బహుళకం()==UMSGPAT_APOS_DOUBLE_REQUIRED;
    }

    /**
     * Appends the s[start, limit[ substring to sb, but with only half of the apostrophes
     * according to JDK pattern behavior.
     */
    స్థిర శూన్య appendReducedApostrophes(స్థిరరాశి UnicodeString &s, int32_t ప్రారంభం, int32_t హద్దు,
                                         UnicodeString &sb);

    /**
     * Appends the sub-message to the result string.
     * Omits SKIP_SYNTAX and appends whole arguments using appendReducedApostrophes().
     */
    స్థిర UnicodeString& append_Sub_సందేశం_Without_Skip_Syntax(స్థిరరాశి సందేశం_నమూనా& సందేశం_నమూనా_2,
                                                            int32_t సందేశం_ప్రారంభం,
                                                            UnicodeString& ఫలితం U_LIFETIME_BOUND);

అంతరంగ:
    సందేశం_Impl() = delete;  // no constructor: all static methods
};

U_NAMESPACE_చివర

#అయితే_ముగింపు  // !UCONFIG_NO_FORMATTING

#అయితే_ముగింపు  // __MESSAGEIMPL_H__
