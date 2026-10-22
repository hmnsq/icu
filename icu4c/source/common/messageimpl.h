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

#अन्तर्भाव "unicode/utypes.h"

#जइ !UCONFIG_NO_FORMATTING

#अन्तर्भाव "unicode/messagepattern.h"

U_NAMESPACE_BEGIN

/**
 * Helper functions for use of MessagePattern.
 * In Java, these are package-private methods in MessagePattern itself.
 * In C++, they are declared here and implemented in messagepattern.cpp.
 */
class U_COMMON_API MessageImpl {
सार्वजनिक:
    /**
     * @return true if getApostropheMode()==UMSGPAT_APOS_DOUBLE_REQUIRED
     */
    स्थिर UBool jdkAposMode(ध्रुव MessagePattern &सन्देशः_Pattern) {
        प्रत्यागम सन्देशः_Pattern.getApostropheMode()==UMSGPAT_APOS_DOUBLE_REQUIRED;
    }

    /**
     * Appends the s[start, limit[ substring to sb, but with only half of the apostrophes
     * according to JDK pattern behavior.
     */
    स्थिर शून्य appendReducedApostrophes(ध्रुव UnicodeString &s, int32_t start, int32_t limit,
                                         UnicodeString &sb);

    /**
     * Appends the sub-message to the result string.
     * Omits SKIP_SYNTAX and appends whole arguments using appendReducedApostrophes().
     */
    स्थिर UnicodeString& appendSubMessageWithoutSkipSyntax(ध्रुव MessagePattern& सन्देशः_Pattern,
                                                            int32_t सन्देशः_Start,
                                                            UnicodeString& result U_LIFETIME_BOUND);

गुप्त:
    MessageImpl() = delete;  // no constructor: all static methods
};

U_NAMESPACE_END

#endif  // !UCONFIG_NO_FORMATTING

#endif  // __MESSAGEIMPL_H__
