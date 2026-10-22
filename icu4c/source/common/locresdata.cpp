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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/putil.h"
#অন্তর্ভাব "unicode/uloc.h"
#অন্তর্ভাব "unicode/ures.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "ulocimp.h"
#অন্তর্ভাব "uresimp.h"

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
U_CAPI ধ্রুবক char16_t * U_EXPORT2
uloc_getTableStringWithFallback(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *locale,
                              ধ্রুবক অক্ষর *table_চাবি, ধ্রুবক অক্ষর *sub_Table_চাবি,
                              ধ্রুবক অক্ষর *item_চাবি,
                              int32_t *p_দৈর্ঘ্য,
                              UErrorCode *pErrorCode)
{
    যদি (U_FAILURE(*pErrorCode)) { ফেরত nullptr; }
/*    char localeBuffer[ULOC_FULLNAME_CAPACITY*4];*/
    ধ্রুবক char16_t *item=nullptr;
    UErrorCode errorCode;

    /*
     * open the bundle for the current locale
     * this falls back through the locale's chain to root
     */
    errorCode=U_ZERO_ERROR;
    icu::LocalUResourceBundlePointer rb(ures_open(path, locale, &errorCode));

    যদি(U_FAILURE(errorCode)) {
        /* total failure, not even root could be opened */
        *pErrorCode=errorCode;
        ফেরত nullptr;
    } অন্যথা যদি(errorCode==U_USING_DEFAULT_WARNING ||
                (errorCode==U_USING_FALLBACK_WARNING && *pErrorCode!=U_USING_DEFAULT_WARNING)
    ) {
        /* set the "strongest" error code (success->fallback->default->failure) */
        *pErrorCode=errorCode;
    }

    জন্য(;;){
        icu::স্তূপ_UResource_Bundle table;
        icu::স্তূপ_UResource_Bundle subTable;
        ures_get_By_চাবি_With_Fallback(rb.getAlias(), table_চাবি, table.getAlias(), &errorCode);

        যদি (sub_Table_চাবি != nullptr) {
            /*
            ures_getByKeyWithFallback(table.getAlias(), subTableKey, subTable.getAlias(), &errorCode);
            item = ures_getStringByKeyWithFallback(subTable.getAlias(), itemKey, pLength, &errorCode);
            if(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
            }
            
            break;*/
            
            ures_get_By_চাবি_With_Fallback(table.getAlias(), sub_Table_চাবি, table.getAlias(), &errorCode);
        }
        যদি(U_SUCCESS(errorCode)){
            item = ures_get_String_By_চাবি_With_Fallback(table.getAlias(), item_চাবি, p_দৈর্ঘ্য, &errorCode);
            যদি(U_FAILURE(errorCode)){
                ধ্রুবক অক্ষর* replacement = nullptr;
                *pErrorCode = errorCode; /*save the errorCode*/
                errorCode = U_ZERO_ERROR;
                /* may be a deprecated code */
                যদি(uprv_strcmp(table_চাবি, "Countries")==0){
                    replacement =  uloc_getCurrentCountryID(item_চাবি);
                }অন্যথা যদি(uprv_strcmp(table_চাবি, "Languages")==0){
                    replacement =  uloc_getCurrentLanguageID(item_চাবি);
                }
                /*pointer comparison is ok since uloc_getCurrentCountryID & uloc_getCurrentLanguageID return the key itself is replacement is not found*/
                যদি(replacement!=nullptr && item_চাবি != replacement){
                    item = ures_get_String_By_চাবি_With_Fallback(table.getAlias(), replacement, p_দৈর্ঘ্য, &errorCode);
                    যদি(U_SUCCESS(errorCode)){
                        *pErrorCode = errorCode;
                        বিরতি;
                    }
                }
            }অন্যথা{
                বিরতি;
            }
        }
        
        যদি(U_FAILURE(errorCode)){    

            /* still can't figure out ?.. try the fallback mechanism */
            int32_t len = 0;
            ধ্রুবক char16_t* fallbackLocale =  nullptr;
            *pErrorCode = errorCode;
            errorCode = U_ZERO_ERROR;

            fallbackLocale = ures_get_String_By_চাবি_With_Fallback(table.getAlias(), "Fallback", &len, &errorCode);
            যদি(U_FAILURE(errorCode)){
               *pErrorCode = errorCode;
                বিরতি;
            }

            icu::CharString explicitFallbackName;
            explicitFallbackName.appendInvariantChars(fallbackLocale, len, errorCode);

            /* guard against recursive fallback */
            যদি (explicitFallbackName == locale) {
                *pErrorCode = U_INTERNAL_PROGRAM_ERROR;
                বিরতি;
            }
            rb.adoptInstead(ures_open(path, explicitFallbackName.তথ্য(), &errorCode));
            যদি(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
                বিরতি;
            }
            /* succeeded in opening the fallback bundle .. continue and try to fetch the item */
        }অন্যথা{
            বিরতি;
        }
    }

    ফেরত item;
}

নামাকাশ {

ULayoutType
_uloc_getOrientationHelper(ধ্রুবক অক্ষর* localeId,
                           ধ্রুবক অক্ষর* চাবি,
                           UErrorCode& status)
{
    ULayoutType ফলাফল = ULOC_LAYOUT_UNKNOWN;

    যদি (U_FAILURE(status)) { ফেরত ফলাফল; }

    যদি (localeId == nullptr) {
        localeId = uloc_getDefault();
    }
    icu::CharString locale_বাফার = ulocimp_canonicalize(localeId, status);

    যদি (U_FAILURE(status)) { ফেরত ফলাফল; }

    int32_t length = 0;
    ধ্রুবক char16_t* ধ্রুবক value =
        uloc_getTableStringWithFallback(
            nullptr,
            locale_বাফার.তথ্য(),
            "layout",
            nullptr,
            চাবি,
            &length,
            &status);

    যদি (U_FAILURE(status)) { ফেরত ফলাফল; }

    যদি (length != 0) {
        নির্বাচন(value[0])
        {
        ক্ষেত্র 0x0062: /* 'b' */
            ফলাফল = ULOC_LAYOUT_BTT;
            বিরতি;
        ক্ষেত্র 0x006C: /* 'l' */
            ফলাফল = ULOC_LAYOUT_LTR;
            বিরতি;
        ক্ষেত্র 0x0072: /* 'r' */
            ফলাফল = ULOC_LAYOUT_RTL;
            বিরতি;
        ক্ষেত্র 0x0074: /* 't' */
            ফলাফল = ULOC_LAYOUT_TTB;
            বিরতি;
        পূর্বনির্ধারিত:
            status = U_INTERNAL_PROGRAM_ERROR;
            বিরতি;
        }
    }

    ফেরত ফলাফল;
}

}  // namespace

U_CAPI ULayoutType U_EXPORT2
uloc_getCharacterOrientation(ধ্রুবক অক্ষর* localeId,
                             UErrorCode *status)
{
    ফেরত _uloc_getOrientationHelper(localeId, "characters", *status);
}

/**
 * Get the layout line orientation for the specified locale.
 * 
 * @param localeID locale name
 * @param status Error status
 * @return an enum indicating the layout orientation for lines.
 */
U_CAPI ULayoutType U_EXPORT2
uloc_get_সারি_Orientation(ধ্রুবক অক্ষর* localeId,
                        UErrorCode *status)
{
    ফেরত _uloc_getOrientationHelper(localeId, "lines", *status);
}
