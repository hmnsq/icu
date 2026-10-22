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

#अगर_अघोषित __MESSAGEIMPL_H__
#घोषणा __MESSAGEIMPL_H__

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_FORMATTING

#समावेश "unicode/messagepattern.h"

U_NAMESPACE_BEGIN

/**
 * Helper functions for use of MessagePattern.
 * In Java, these are package-private methods in MessagePattern itself.
 * In C++, they are declared here and implemented in messagepattern.cpp.
 */
class U_COMMON_API MessageImpl {
खुला:
    /**
     * @return true if getApostropheMode()==UMSGPAT_APOS_DOUBLE_REQUIRED
     */
    अटल UBool jdkAposMode(स्थिर MessagePattern &संदेश_Pattern) {
        वापस संदेश_Pattern.getApostropheMode()==UMSGPAT_APOS_DOUBLE_REQUIRED;
    }

    /**
     * Appends the s[start, limit[ substring to sb, but with only half of the apostrophes
     * according to JDK pattern behavior.
     */
    अटल व्योम appendReducedApostrophes(स्थिर UnicodeString &s, int32_t start, int32_t limit,
                                         UnicodeString &sb);

    /**
     * Appends the sub-message to the result string.
     * Omits SKIP_SYNTAX and appends whole arguments using appendReducedApostrophes().
     */
    अटल UnicodeString& appendSubMessageWithoutSkipSyntax(स्थिर MessagePattern& संदेश_Pattern,
                                                            int32_t संदेश_Start,
                                                            UnicodeString& परिणाम U_LIFETIME_BOUND);

निजी:
    MessageImpl() = delete;  // no constructor: all static methods
};

U_NAMESPACE_END

#पूर्ण_अगर  // !UCONFIG_NO_FORMATTING

#पूर्ण_अगर  // __MESSAGEIMPL_H__
