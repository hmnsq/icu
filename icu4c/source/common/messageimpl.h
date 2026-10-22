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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_FORMATTING

#ಅನ್ತರ್ಭಾವ "unicode/messagepattern.h"

U_NAMESPACE_BEGIN

/**
 * Helper functions for use of MessagePattern.
 * In Java, these are package-private methods in MessagePattern itself.
 * In C++, they are declared here and implemented in messagepattern.cpp.
 */
class U_COMMON_API MessageImpl {
ಸಾರ್ವಜನಿಕ:
    /**
     * @return true if getApostropheMode()==UMSGPAT_APOS_DOUBLE_REQUIRED
     */
    ಸ್ಥಿರ UBool jdkAposMode(ಧ್ರುವ MessagePattern &ಸಂದೇಶ_Pattern) {
        ಪ್ರತ್ಯಾಗಮ ಸಂದೇಶ_Pattern.getApostropheMode()==UMSGPAT_APOS_DOUBLE_REQUIRED;
    }

    /**
     * Appends the s[start, limit[ substring to sb, but with only half of the apostrophes
     * according to JDK pattern behavior.
     */
    ಸ್ಥಿರ ಶೂನ್ಯ appendReducedApostrophes(ಧ್ರುವ UnicodeString &s, int32_t start, int32_t limit,
                                         UnicodeString &sb);

    /**
     * Appends the sub-message to the result string.
     * Omits SKIP_SYNTAX and appends whole arguments using appendReducedApostrophes().
     */
    ಸ್ಥಿರ UnicodeString& appendSubMessageWithoutSkipSyntax(ಧ್ರುವ MessagePattern& ಸಂದೇಶ_Pattern,
                                                            int32_t ಸಂದೇಶ_Start,
                                                            UnicodeString& result U_LIFETIME_BOUND);

ಗುಪ್ತ:
    MessageImpl() = delete;  // no constructor: all static methods
};

U_NAMESPACE_END

#endif  // !UCONFIG_NO_FORMATTING

#endif  // __MESSAGEIMPL_H__
