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

#اگر_غیر_تعریف_شدہ __MESSAGEIMPL_H__
#تعریف_کرو __MESSAGEIMPL_H__

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_FORMATTING

#شامل "unicode/messagepattern.h"

U_NAMESPACE_BEGIN

/**
 * Helper functions for use of MessagePattern.
 * In Java, these are package-private methods in MessagePattern itself.
 * In C++, they are declared here and implemented in messagepattern.cpp.
 */
class U_COMMON_API پیغام_Impl {
عوامی:
    /**
     * @return true if getApostropheMode()==UMSGPAT_APOS_DOUBLE_REQUIRED
     */
    ساکن UBool jdk_Apos_منوال(ثابت پیغام_نمونہ &پیغام_نمونہ_2) {
        واپس پیغام_نمونہ_2.get_Apostrophe_منوال()==UMSGPAT_APOS_DOUBLE_REQUIRED;
    }

    /**
     * Appends the s[start, limit[ substring to sb, but with only half of the apostrophes
     * according to JDK pattern behavior.
     */
    ساکن خالی appendReducedApostrophes(ثابت UnicodeString &s, int32_t آغاز, int32_t حد_بندی,
                                         UnicodeString &sb);

    /**
     * Appends the sub-message to the result string.
     * Omits SKIP_SYNTAX and appends whole arguments using appendReducedApostrophes().
     */
    ساکن UnicodeString& append_Sub_پیغام_Without_Skip_Syntax(ثابت پیغام_نمونہ& پیغام_نمونہ_2,
                                                            int32_t پیغام_آغاز,
                                                            UnicodeString& نتیجہ U_LIFETIME_BOUND);

نجی:
    پیغام_Impl() = delete;  // no constructor: all static methods
};

U_NAMESPACE_انتہا

#اگر_ختم  // !UCONFIG_NO_FORMATTING

#اگر_ختم  // __MESSAGEIMPL_H__
