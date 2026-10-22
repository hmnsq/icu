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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __MESSAGEIMPL_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __MESSAGEIMPL_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_FORMATTING

#ಅನ್ತರ್ಭಾವ "unicode/messagepattern.h"

U_NAMESPACE_BEGIN

/**
 * Helper functions for use of MessagePattern.
 * In Java, these are package-private methods in MessagePattern itself.
 * In C++, they are declared here and implemented in messagepattern.cpp.
 */
class U_COMMON_API ಸಂದೇಶ_Impl {
ಸಾರ್ವಜನಿಕ:
    /**
     * @return true if getApostropheMode()==UMSGPAT_APOS_DOUBLE_REQUIRED
     */
    ಸ್ಥಿರ UBool jdk_Apos_ಬಹುಲಕ(ಸ್ಥಿರಾಂಕ ಸಂದೇಶ_ನಮೂನೆ &ಸಂದೇಶ_ನಮೂನೆ_2) {
        ಹಿಂತಿರುಗಿ ಸಂದೇಶ_ನಮೂನೆ_2.get_Apostrophe_ಬಹುಲಕ()==UMSGPAT_APOS_DOUBLE_REQUIRED;
    }

    /**
     * Appends the s[start, limit[ substring to sb, but with only half of the apostrophes
     * according to JDK pattern behavior.
     */
    ಸ್ಥಿರ ಶೂನ್ಯ appendReducedApostrophes(ಸ್ಥಿರಾಂಕ UnicodeString &s, int32_t ಆರಂಭ, int32_t ಎಲ್ಲೆ,
                                         UnicodeString &sb);

    /**
     * Appends the sub-message to the result string.
     * Omits SKIP_SYNTAX and appends whole arguments using appendReducedApostrophes().
     */
    ಸ್ಥಿರ UnicodeString& append_Sub_ಸಂದೇಶ_Without_Skip_Syntax(ಸ್ಥಿರಾಂಕ ಸಂದೇಶ_ನಮೂನೆ& ಸಂದೇಶ_ನಮೂನೆ_2,
                                                            int32_t ಸಂದೇಶ_ಆರಂಭ,
                                                            UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND);

ಖಾಸಗಿ:
    ಸಂದೇಶ_Impl() = delete;  // no constructor: all static methods
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ  // !UCONFIG_NO_FORMATTING

#ಆದರೆ_ಅಂತ್ಯ  // __MESSAGEIMPL_H__
