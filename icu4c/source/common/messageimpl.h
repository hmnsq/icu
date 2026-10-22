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

#ifndef __MESSAGEIMPL_H__
#define __MESSAGEIMPL_H__

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_FORMATTING

#شامل "unicode/messagepattern.h"

U_NAMESPACE_BEGIN

/**
 * Helper functions for use of MessagePattern.
 * In Java, these are package-private methods in MessagePattern itself.
 * In C++, they are declared here and implemented in messagepattern.cpp.
 */
class U_COMMON_API MessageImpl {
عمومی:
    /**
     * @return true if getApostropheMode()==UMSGPAT_APOS_DOUBLE_REQUIRED
     */
    ایستا UBool jdkAposMode(ثابت MessagePattern &پیام_Pattern) {
        بازگشت پیام_Pattern.getApostropheMode()==UMSGPAT_APOS_DOUBLE_REQUIRED;
    }

    /**
     * Appends the s[start, limit[ substring to sb, but with only half of the apostrophes
     * according to JDK pattern behavior.
     */
    ایستا تهی appendReducedApostrophes(ثابت UnicodeString &s, int32_t start, int32_t limit,
                                         UnicodeString &sb);

    /**
     * Appends the sub-message to the result string.
     * Omits SKIP_SYNTAX and appends whole arguments using appendReducedApostrophes().
     */
    ایستا UnicodeString& appendSubMessageWithoutSkipSyntax(ثابت MessagePattern& پیام_Pattern,
                                                            int32_t پیام_Start,
                                                            UnicodeString& result U_LIFETIME_BOUND);

خصوصی:
    MessageImpl() = delete;  // no constructor: all static methods
};

U_NAMESPACE_END

#endif  // !UCONFIG_NO_FORMATTING

#endif  // __MESSAGEIMPL_H__
