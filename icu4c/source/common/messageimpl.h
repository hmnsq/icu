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

#اگر_تعریف_نشده __MESSAGEIMPL_H__
#تعریف_کن __MESSAGEIMPL_H__

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_FORMATTING

#ضمن "unicode/messagepattern.h"

U_NAMESPACE_BEGIN

/**
 * Helper functions for use of MessagePattern.
 * In Java, these are package-private methods in MessagePattern itself.
 * In C++, they are declared here and implemented in messagepattern.cpp.
 */
class U_COMMON_API پیام_Impl {
عام:
    /**
     * @return true if getApostropheMode()==UMSGPAT_APOS_DOUBLE_REQUIRED
     */
    ساكن UBool jdk_Apos_نما(ثابت پیام_الگو &پیام_الگو_2) {
        أرجع پیام_الگو_2.get_Apostrophe_نما()==UMSGPAT_APOS_DOUBLE_REQUIRED;
    }

    /**
     * Appends the s[start, limit[ substring to sb, but with only half of the apostrophes
     * according to JDK pattern behavior.
     */
    ساكن فراغ appendReducedApostrophes(ثابت UnicodeString &s, int32_t شروع, int32_t حد,
                                         UnicodeString &sb);

    /**
     * Appends the sub-message to the result string.
     * Omits SKIP_SYNTAX and appends whole arguments using appendReducedApostrophes().
     */
    ساكن UnicodeString& append_Sub_پیام_Without_Skip_Syntax(ثابت پیام_الگو& پیام_الگو_2,
                                                            int32_t پیام_شروع,
                                                            UnicodeString& نتیجه U_LIFETIME_BOUND);

خاص:
    پیام_Impl() = delete;  // no constructor: all static methods
};

U_NAMESPACE_انتها

#پایان_اگر  // !UCONFIG_NO_FORMATTING

#پایان_اگر  // __MESSAGEIMPL_H__
