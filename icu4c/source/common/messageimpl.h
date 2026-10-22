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

#جيڪڏهن_وصف_نه_ٿيل __MESSAGEIMPL_H__
#وصف_ڪريو __MESSAGEIMPL_H__

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_FORMATTING

#ضمن "unicode/messagepattern.h"

U_NAMESPACE_BEGIN

/**
 * Helper functions for use of MessagePattern.
 * In Java, these are package-private methods in MessagePattern itself.
 * In C++, they are declared here and implemented in messagepattern.cpp.
 */
class U_COMMON_API پيغام_Impl {
عام:
    /**
     * @return true if getApostropheMode()==UMSGPAT_APOS_DOUBLE_REQUIRED
     */
    ساكن UBool jdk_Apos_گهڻائي(ثابت پيغام_نمونو &نياپو_نمونو) {
        أرجع نياپو_نمونو.get_Apostrophe_گهڻائي()==UMSGPAT_APOS_DOUBLE_REQUIRED;
    }

    /**
     * Appends the s[start, limit[ substring to sb, but with only half of the apostrophes
     * according to JDK pattern behavior.
     */
    ساكن فراغ appendReducedApostrophes(ثابت UnicodeString &s, int32_t شروعات, int32_t حد_بندي,
                                         UnicodeString &sb);

    /**
     * Appends the sub-message to the result string.
     * Omits SKIP_SYNTAX and appends whole arguments using appendReducedApostrophes().
     */
    ساكن UnicodeString& append_Sub_پيغام_Without_Skip_Syntax(ثابت پيغام_نمونو& نياپو_نمونو,
                                                            int32_t نياپو_شروعات,
                                                            UnicodeString& نتيجو U_LIFETIME_BOUND);

خاص:
    پيغام_Impl() = delete;  // no constructor: all static methods
};

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو  // !UCONFIG_NO_FORMATTING

#جيڪڏهن_پورو  // __MESSAGEIMPL_H__
