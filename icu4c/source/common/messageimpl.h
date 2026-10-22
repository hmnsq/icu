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

#که_نه_تعریف_شوی __MESSAGEIMPL_H__
#تعریف_کړه __MESSAGEIMPL_H__

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_FORMATTING

#ضمن "unicode/messagepattern.h"

U_NAMESPACE_BEGIN

/**
 * Helper functions for use of MessagePattern.
 * In Java, these are package-private methods in MessagePattern itself.
 * In C++, they are declared here and implemented in messagepattern.cpp.
 */
class U_COMMON_API پیغام_Impl {
عام:
    /**
     * @return true if getApostropheMode()==UMSGPAT_APOS_DOUBLE_REQUIRED
     */
    ساكن UBool jdk_Apos_ډېرپېښ(ثابت پیغام_نمونه &پیغام_نمونه_2) {
        أرجع پیغام_نمونه_2.get_Apostrophe_ډېرپېښ()==UMSGPAT_APOS_DOUBLE_REQUIRED;
    }

    /**
     * Appends the s[start, limit[ substring to sb, but with only half of the apostrophes
     * according to JDK pattern behavior.
     */
    ساكن فراغ appendReducedApostrophes(ثابت UnicodeString &s, int32_t پیل, int32_t حد,
                                         UnicodeString &sb);

    /**
     * Appends the sub-message to the result string.
     * Omits SKIP_SYNTAX and appends whole arguments using appendReducedApostrophes().
     */
    ساكن UnicodeString& append_Sub_پیغام_Without_Skip_Syntax(ثابت پیغام_نمونه& پیغام_نمونه_2,
                                                            int32_t پیغام_پیل,
                                                            UnicodeString& پایله U_LIFETIME_BOUND);

خاص:
    پیغام_Impl() = delete;  // no constructor: all static methods
};

U_NAMESPACE_پای_ټکی

#د_که_پای  // !UCONFIG_NO_FORMATTING

#د_که_پای  // __MESSAGEIMPL_H__
