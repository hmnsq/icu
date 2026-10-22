// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 1997-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  loclikely.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010feb25
*   created by: Markus W. Scherer
*
*   Code for miscellaneous locale-related resource bundle data access,
*   separated out from other .cpp files
*   that then do not depend on resource bundle code and this data.
*/

#شامل "unicode/utypes.h"
#شامل "unicode/putil.h"
#شامل "unicode/uloc.h"
#شامل "unicode/ures.h"
#شامل "charstr.h"
#شامل "cstring.h"
#شامل "ulocimp.h"
#شامل "uresimp.h"

/*
 * Lookup a resource bundle table item with fallback on the table level.
 * Regular resource bundle lookups perform fallback to parent locale bundles
 * and eventually the root bundle, but only for top-level items.
 * This function takes the name of a top-level table and of an item in that table
 * and performs a lookup of both, falling back until a bundle contains a table
 * with this item.
 *
 * Note: Only the opening of entire bundles falls back through the default locale
 * before root. Once a bundle is open, item lookups do not go through the
 * default locale because that would result in a mix of languages that is
 * unpredictable to the programmer and most likely useless.
 */
U_CAPI ثابت char16_t * U_EXPORT2
uloc_get_جدول_String_With_Fallback(ثابت حرف *راستہ, ثابت حرف *locale,
                              ثابت حرف *جدول_کلید, ثابت حرف *sub_جدول_کلید,
                              ثابت حرف *شے_کلید,
                              int32_t *p_لمبائی,
                              UError_رمز *p_Error_رمز)
{
    اگر (U_FAILURE(*p_Error_رمز)) { واپس خالی_اشاریہ; }
/*    char localeBuffer[ULOC_FULLNAME_CAPACITY*4];*/
    ثابت char16_t *شے=خالی_اشاریہ;
    UError_رمز error_رمز;

    /*
     * open the bundle for the current locale
     * this falls back through the locale's chain to root
     */
    error_رمز=U_ZERO_ERROR;
    icu::Local_UResource_Bundle_اشارہ_گر rb(ures_open(راستہ, locale, &error_رمز));

    اگر(U_FAILURE(error_رمز)) {
        /* total failure, not even root could be opened */
        *p_Error_رمز=error_رمز;
        واپس خالی_اشاریہ;
    } ورنہ اگر(error_رمز==U_USING_DEFAULT_WARNING ||
                (error_رمز==U_USING_FALLBACK_WARNING && *p_Error_رمز!=U_USING_DEFAULT_WARNING)
    ) {
        /* set the "strongest" error code (success->fallback->default->failure) */
        *p_Error_رمز=error_رمز;
    }

    برائے(;;){
        icu::ڈھیر_UResource_Bundle جدول;
        icu::ڈھیر_UResource_Bundle sub_جدول;
        ures_get_By_کلید_With_Fallback(rb.getAlias(), جدول_کلید, جدول.getAlias(), &error_رمز);

        اگر (sub_جدول_کلید != خالی_اشاریہ) {
            /*
            ures_getByKeyWithFallback(table.getAlias(), subTableKey, subTable.getAlias(), &errorCode);
            item = ures_getStringByKeyWithFallback(subTable.getAlias(), itemKey, pLength, &errorCode);
            if(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
            }
            
            break;*/
            
            ures_get_By_کلید_With_Fallback(جدول.getAlias(), sub_جدول_کلید, جدول.getAlias(), &error_رمز);
        }
        اگر(U_SUCCESS(error_رمز)){
            شے = ures_get_String_By_کلید_With_Fallback(جدول.getAlias(), شے_کلید, p_لمبائی, &error_رمز);
            اگر(U_FAILURE(error_رمز)){
                ثابت حرف* replacement = خالی_اشاریہ;
                *p_Error_رمز = error_رمز; /*save the errorCode*/
                error_رمز = U_ZERO_ERROR;
                /* may be a deprecated code */
                اگر(uprv_strcmp(جدول_کلید, "Countries")==0){
                    replacement =  uloc_getCurrentCountryID(شے_کلید);
                }ورنہ اگر(uprv_strcmp(جدول_کلید, "Languages")==0){
                    replacement =  uloc_getCurrentLanguageID(شے_کلید);
                }
                /*pointer comparison is ok since uloc_getCurrentCountryID & uloc_getCurrentLanguageID return the key itself is replacement is not found*/
                اگر(replacement!=خالی_اشاریہ && شے_کلید != replacement){
                    شے = ures_get_String_By_کلید_With_Fallback(جدول.getAlias(), replacement, p_لمبائی, &error_رمز);
                    اگر(U_SUCCESS(error_رمز)){
                        *p_Error_رمز = error_رمز;
                        توڑو;
                    }
                }
            }ورنہ{
                توڑو;
            }
        }
        
        اگر(U_FAILURE(error_رمز)){    

            /* still can't figure out ?.. try the fallback mechanism */
            int32_t len = 0;
            ثابت char16_t* fallbackLocale =  خالی_اشاریہ;
            *p_Error_رمز = error_رمز;
            error_رمز = U_ZERO_ERROR;

            fallbackLocale = ures_get_String_By_کلید_With_Fallback(جدول.getAlias(), "Fallback", &len, &error_رمز);
            اگر(U_FAILURE(error_رمز)){
               *p_Error_رمز = error_رمز;
                توڑو;
            }

            icu::CharString explicitFallbackName;
            explicitFallbackName.appendInvariantChars(fallbackLocale, len, error_رمز);

            /* guard against recursive fallback */
            اگر (explicitFallbackName == locale) {
                *p_Error_رمز = U_اندرونی_پروگرام_ERROR;
                توڑو;
            }
            rb.adoptInstead(ures_open(راستہ, explicitFallbackName.ڈیٹا(), &error_رمز));
            اگر(U_FAILURE(error_رمز)){
                *p_Error_رمز = error_رمز;
                توڑو;
            }
            /* succeeded in opening the fallback bundle .. continue and try to fetch the item */
        }ورنہ{
            توڑو;
        }
    }

    واپس شے;
}

نامخلا {

ULayoutType
_uloc_getOrientationHelper(ثابت حرف* localeId,
                           ثابت حرف* کلید,
                           UError_رمز& حالت_2)
{
    ULayoutType نتیجہ = ULOC_LAYOUT_UNKNOWN;

    اگر (U_FAILURE(حالت_2)) { واپس نتیجہ; }

    اگر (localeId == خالی_اشاریہ) {
        localeId = uloc_getDefault();
    }
    icu::CharString locale_بفر = ulocimp_canonicalize(localeId, حالت_2);

    اگر (U_FAILURE(حالت_2)) { واپس نتیجہ; }

    int32_t length = 0;
    ثابت char16_t* ثابت value =
        uloc_get_جدول_String_With_Fallback(
            خالی_اشاریہ,
            locale_بفر.ڈیٹا(),
            "layout",
            خالی_اشاریہ,
            کلید,
            &length,
            &حالت_2);

    اگر (U_FAILURE(حالت_2)) { واپس نتیجہ; }

    اگر (length != 0) {
        انتخاب(value[0])
        {
        حالت 0x0062: /* 'b' */
            نتیجہ = ULOC_LAYOUT_BTT;
            توڑو;
        حالت 0x006C: /* 'l' */
            نتیجہ = ULOC_LAYOUT_LTR;
            توڑو;
        حالت 0x0072: /* 'r' */
            نتیجہ = ULOC_LAYOUT_RTL;
            توڑو;
        حالت 0x0074: /* 't' */
            نتیجہ = ULOC_LAYOUT_TTB;
            توڑو;
        طےشدہ:
            حالت_2 = U_اندرونی_پروگرام_ERROR;
            توڑو;
        }
    }

    واپس نتیجہ;
}

}  // namespace

U_CAPI ULayoutType U_EXPORT2
uloc_getCharacterOrientation(ثابت حرف* localeId,
                             UError_رمز *حالت_2)
{
    واپس _uloc_getOrientationHelper(localeId, "characters", *حالت_2);
}

/**
 * Get the layout line orientation for the specified locale.
 * 
 * @param localeID locale name
 * @param status Error status
 * @return an enum indicating the layout orientation for lines.
 */
U_CAPI ULayoutType U_EXPORT2
uloc_get_سطر_Orientation(ثابت حرف* localeId,
                        UError_رمز *حالت_2)
{
    واپس _uloc_getOrientationHelper(localeId, "lines", *حالت_2);
}
