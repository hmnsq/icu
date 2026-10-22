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

#यदि_अपरिभाषितम् __MESSAGEIMPL_H__
#परिभाषय __MESSAGEIMPL_H__

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_FORMATTING

#अन्तर्भाव "unicode/messagepattern.h"

U_NAMESPACE_BEGIN

/**
 * Helper functions for use of MessagePattern.
 * In Java, these are package-private methods in MessagePattern itself.
 * In C++, they are declared here and implemented in messagepattern.cpp.
 */
class U_COMMON_API सन्देशः_Impl {
सार्वजनिक:
    /**
     * @return true if getApostropheMode()==UMSGPAT_APOS_DOUBLE_REQUIRED
     */
    स्थिर UBool jdk_Apos_बहुलकम्(ध्रुव सन्देशः_आदर्शः &सन्देशः_आदर्शः_2) {
        प्रत्यागम सन्देशः_आदर्शः_2.get_Apostrophe_बहुलकम्()==UMSGPAT_APOS_DOUBLE_REQUIRED;
    }

    /**
     * Appends the s[start, limit[ substring to sb, but with only half of the apostrophes
     * according to JDK pattern behavior.
     */
    स्थिर शून्य appendReducedApostrophes(ध्रुव UnicodeString &s, int32_t start, int32_t सीमा,
                                         UnicodeString &sb);

    /**
     * Appends the sub-message to the result string.
     * Omits SKIP_SYNTAX and appends whole arguments using appendReducedApostrophes().
     */
    स्थिर UnicodeString& append_Sub_सन्देशः_Without_Skip_Syntax(ध्रुव सन्देशः_आदर्शः& सन्देशः_आदर्शः_2,
                                                            int32_t सन्देशः_Start,
                                                            UnicodeString& परिणाम U_LIFETIME_BOUND);

गुप्त:
    सन्देशः_Impl() = delete;  // no constructor: all static methods
};

U_NAMESPACE_अन्तः

#यदि_अन्तः  // !UCONFIG_NO_FORMATTING

#यदि_अन्तः  // __MESSAGEIMPL_H__
