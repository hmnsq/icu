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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __MESSAGEIMPL_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __MESSAGEIMPL_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_FORMATTING

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/messagepattern.h"

U_NAMESPACE_BEGIN

/**
 * Helper functions for use of MessagePattern.
 * In Java, these are package-private methods in MessagePattern itself.
 * In C++, they are declared here and implemented in messagepattern.cpp.
 */
class U_COMMON_API ਸੁਨੇਹਾ_Impl {
ਜਨਤਕ:
    /**
     * @return true if getApostropheMode()==UMSGPAT_APOS_DOUBLE_REQUIRED
     */
    ਸਥਿਰ UBool jdk_Apos_ਬਹੁਲਕ(ਅਚਲ ਸੁਨੇਹਾ_ਨਮੂਨਾ &ਸੁਨੇਹਾ_ਨਮੂਨਾ_2) {
        ਵਾਪਸ ਸੁਨੇਹਾ_ਨਮੂਨਾ_2.get_Apostrophe_ਬਹੁਲਕ()==UMSGPAT_APOS_DOUBLE_REQUIRED;
    }

    /**
     * Appends the s[start, limit[ substring to sb, but with only half of the apostrophes
     * according to JDK pattern behavior.
     */
    ਸਥਿਰ ਖਾਲੀ appendReducedApostrophes(ਅਚਲ UnicodeString &s, int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ,
                                         UnicodeString &sb);

    /**
     * Appends the sub-message to the result string.
     * Omits SKIP_SYNTAX and appends whole arguments using appendReducedApostrophes().
     */
    ਸਥਿਰ UnicodeString& append_Sub_ਸੁਨੇਹਾ_Without_Skip_Syntax(ਅਚਲ ਸੁਨੇਹਾ_ਨਮੂਨਾ& ਸੁਨੇਹਾ_ਨਮੂਨਾ_2,
                                                            int32_t ਸੁਨੇਹਾ_ਸ਼ੁਰੂ,
                                                            UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND);

ਨਿਜੀ:
    ਸੁਨੇਹਾ_Impl() = delete;  // no constructor: all static methods
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ  // !UCONFIG_NO_FORMATTING

#ਜੇ_ਅੰਤ  // __MESSAGEIMPL_H__
