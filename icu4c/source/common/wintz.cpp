// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 2005-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*
* File WINTZ.CPP
*
********************************************************************************
*/

#অন্তর্ভাব "unicode/utypes.h"

#যদি U_PLATFORM_USES_ONLY_WIN32_API

#অন্তর্ভাব "wintz.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"

#অন্তর্ভাব "unicode/ures.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "uresimp.h"

#ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#endif
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX
#অন্তর্ভাব <windows.h>

U_NAMESPACE_BEGIN

// Note these constants and the struct are only used when dealing with the fallback path for RDP sessions.

// This is the location of the time zones in the registry on Vista+ systems.
// See: https://docs.microsoft.com/windows/win32/api/timezoneapi/ns-timezoneapi-dynamic_time_zone_information
#define WINDOWS_TIMEZONES_REG_চাবি_PATH L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones"

// Max length for a registry key is 255. +1 for null.
// See: https://docs.microsoft.com/windows/win32/sysinfo/registry-element-size-limits
#define WINDOWS_MAX_REG_চাবি_দৈর্ঘ্য 256

#যদি U_PLATFORM_HAS_WINUWP_API == 0

// This is the layout of the TZI binary value in the registry.
// See: https://docs.microsoft.com/windows/win32/api/timezoneapi/ns-timezoneapi-time_zone_information
প্রকারলেখ কাঠামো _REG_TZI_FORMAT {
    LONG Bias;
    LONG StandardBias;
    LONG DaylightBias;
    SYSTEMTIME StandardDate;
    SYSTEMTIME DaylightDate;
} REG_TZI_FORMAT;

#endif // U_PLATFORM_HAS_WINUWP_API

/**
* This is main Windows time zone detection function.
* 
* It returns the Windows time zone converted to an ICU time zone as a heap-allocated buffer, or nullptr upon failure.
*
* We use the Win32 API GetDynamicTimeZoneInformation (which is available since Vista) to get the current time zone info,
* as this API returns a non-localized time zone name which can be then mapped to an ICU time zone.
* 
* However, in some RDP/terminal services situations, this struct isn't always fully complete, and the TimeZoneKeyName
* field of the struct might be nullptr. This can happen with some 3rd party RDP clients, and also when using older versions
* of the RDP protocol, which don't send the newer TimeZoneKeyNamei information and only send the StandardName and DaylightName.
* 
* Since these 3rd party clients and older RDP clients only send the pre-Vista time zone information to the server, this means that we 
* need to fallback on using the pre-Vista methods to determine the time zone. This unfortunately requires examining the registry directly
* in order to try and determine the current time zone.
* 
* Note that this can however still fail in some cases though if the client and server are using different languages, as the StandardName
* that is sent by client is localized in the client's language. However, we must compare this to the names that are on the server, which
* are localized in registry using the server's language. Despite that, this is the best we can do.
* 
* Note: This fallback method won't work for the UWP version though, as we can't use the registry APIs in UWP.
* 
* Once we have the current Windows time zone, then we can then map it to an ICU time zone ID (~ Olsen ID).
*/
U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
uprv_detectWindowsTimeZone()
{
    // We first try to obtain the time zone directly by using the TimeZoneKeyName field of the DYNAMIC_TIME_ZONE_INFORMATION struct.
    DYNAMIC_TIME_ZONE_INFORMATION dynamicTZI;
    uprv_memset(&dynamicTZI, 0, আকার(dynamicTZI));
    SYSTEMTIME systemTimeAllZero;
    uprv_memset(&systemTimeAllZero, 0, আকার(systemTimeAllZero));

    যদি (GetDynamicTimeZoneInformation(&dynamicTZI) == TIME_ZONE_ID_INVALID) {
        ফেরত nullptr;
    }

    // If the DST setting has been turned off in the Control Panel, then return "Etc/GMT<offset>".
    //
    // Note: This logic is based on how the Control Panel itself determines if DST is 'off' on Windows.
    // The code is somewhat convoluted; in a sort of pseudo-code it looks like this:
    // 
    //   IF (GetDynamicTimeZoneInformation != TIME_ZONE_ID_INVALID) && (DynamicDaylightTimeDisabled != 0) &&
    //      (StandardDate == DaylightDate) &&
    //      (
    //       (TimeZoneKeyName != Empty && StandardDate == 0) ||
    //       (TimeZoneKeyName == Empty && StandardDate != 0)
    //      )
    //   THEN
    //     DST setting is "Disabled".
    //
    যদি (dynamicTZI.DynamicDaylightTimeDisabled != 0 &&
        uprv_memcmp(&dynamicTZI.StandardDate, &dynamicTZI.DaylightDate, আকার(dynamicTZI.StandardDate)) == 0 &&
        ((dynamicTZI.Time_Zone_চাবি_Name[0] != L'\0' && uprv_memcmp(&dynamicTZI.StandardDate, &systemTimeAllZero, আকার(systemTimeAllZero)) == 0) ||
         (dynamicTZI.Time_Zone_চাবি_Name[0] == L'\0' && uprv_memcmp(&dynamicTZI.StandardDate, &systemTimeAllZero, আকার(systemTimeAllZero)) != 0)))
    {
        LONG utcOffsetMins = dynamicTZI.Bias;
        যদি (utcOffsetMins == 0) {
            ফেরত uprv_strdup("Etc/UTC");
        }

        // No way to support when DST is turned off and the offset in minutes is not a multiple of 60.
        যদি (utcOffsetMins % 60 == 0) {
            অক্ষর gmtOffsetTz[11] = {}; // "Etc/GMT+dd" is 11-char long with a terminal null.
            // Important note on the sign convention for zones:
            //
            // From https://en.wikipedia.org/wiki/Tz_database#Area
            //   "In order to conform with the POSIX style, those zone names beginning with "Etc/GMT" have their sign reversed
            //   from the standard ISO 8601 convention. In the "Etc" area, zones west of GMT have a positive sign and those
            //   east have a negative sign in their name (e.g "Etc/GMT-14" is 14 hours ahead of GMT)."
            //
            // Regarding the POSIX style, from https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
            //   "The offset specifies the time value you must add to the local time to get a Coordinated Universal Time value."
            //
            // However, the Bias value in DYNAMIC_TIME_ZONE_INFORMATION *already* follows the POSIX convention.
            // 
            // From https://docs.microsoft.com/en-us/windows/win32/api/timezoneapi/ns-timezoneapi-dynamic_time_zone_information
            //   "The bias is the difference, in minutes, between Coordinated Universal Time (UTC) and
            //   local time. All translations between UTC and local time are based on the following formula:
            //      UTC = local time + bias"
            //
            // For example, a time zone that is 3 hours ahead of UTC (UTC+03:00) would have a Bias value of -180, and the
            // corresponding time zone ID would be "Etc/GMT-3". (So there is no need to negate utcOffsetMins below.)
            পূর্ণসংখ্যা ret = snprintf(gmtOffsetTz, আকার(gmtOffsetTz), "Etc/GMT%+ld", utcOffsetMins / 60);
            যদি (ret > 0 && ret < UPRV_LENGTHOF(gmtOffsetTz)) {
                ফেরত uprv_strdup(gmtOffsetTz);
            }
        }
    }

    // If DST is NOT disabled, but the TimeZoneKeyName field of the struct is nullptr, then we may be dealing with a
    // RDP/terminal services session where the 'Time Zone Redirection' feature is enabled. However, either the RDP
    // client sent the server incomplete info (some 3rd party RDP clients only send the StandardName and  DaylightName,
    // but do not send the important TimeZoneKeyName), or if the RDP server has not appropriately populated the struct correctly.
    //
    // In this case we unfortunately have no choice but to fallback to using the pre-Vista method of determining the
    // time zone, which requires examining the registry directly.
    //
    // Note that this can however still fail though if the client and server are using different languages, as the StandardName
    // that is sent by client is *localized* in the client's language. However, we must compare this to the names that are
    // on the server, which are *localized* in registry using the server's language.
    //
    // One other note is that this fallback method doesn't work for the UWP version, as we can't use the registry APIs.

    // windowsTimeZoneName will point at timezoneSubKeyName if we had to fallback to using the registry, and we found a match.
    WCHAR timezone_Sub_চাবি_Name[WINDOWS_MAX_REG_চাবি_দৈর্ঘ্য];
    WCHAR *windowsTimeZoneName = dynamicTZI.Time_Zone_চাবি_Name;

    যদি (dynamicTZI.Time_Zone_চাবি_Name[0] == 0) {

// We can't use the registry APIs in the UWP version.
#যদি U_PLATFORM_HAS_WINUWP_API == 1
        (শূন্য)timezone_Sub_চাবি_Name; // suppress unused variable warnings.
        ফেরত nullptr;
#অন্যথা
        // Open the path to the time zones in the Windows registry.
        LONG ret;
        HKEY h_চাবি_All_Time_Zones = nullptr;
        ret = Reg_Open_চাবি_Ex_W(HKEY_LOCAL_MACHINE, WINDOWS_TIMEZONES_REG_চাবি_PATH, 0, চাবি_READ,
                            reinterpret_cast<PHKEY>(&h_চাবি_All_Time_Zones));
        
        যদি (ret != ERROR_SUCCESS) {
            // If we can't open the key, then we can't do much, so fail.
            ফেরত nullptr;
        }

        // Read the number of subkeys under the time zone registry path.
        DWORD অঙ্ক_Time_Zone_Sub_চাবিগুলি;
        ret = Reg_Query_Info_চাবি_W(h_চাবি_All_Time_Zones, nullptr, nullptr, nullptr, &অঙ্ক_Time_Zone_Sub_চাবিগুলি,
                               nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
        
        যদি (ret != ERROR_SUCCESS) {
            Reg_Close_চাবি(h_চাবি_All_Time_Zones);
            ফেরত nullptr;
        }

        // Examine each of the subkeys to try and find a match for the localized standard name ("Std").
        //
        // Note: The name of the time zone subkey itself is not localized, but the "Std" name is localized. This means
        // that we could fail to find a match if the RDP client and RDP server are using different languages, but unfortunately
        // there isn't much we can do about it.
        HKEY h_চাবি_Time_Zone_Sub_চাবি = nullptr;
        ULONG registry_মান_Type;
        WCHAR registryStandardName[WINDOWS_MAX_REG_চাবি_দৈর্ঘ্য];

        জন্য (DWORD i = 0; i < অঙ্ক_Time_Zone_Sub_চাবিগুলি; i++) {
            // Note: RegEnumKeyExW wants the size of the buffer in characters.
            DWORD size = UPRV_LENGTHOF(timezone_Sub_চাবি_Name);
            ret = Reg_Enum_চাবি_Ex_W(h_চাবি_All_Time_Zones, i, timezone_Sub_চাবি_Name, &size, nullptr, nullptr, nullptr, nullptr);

            যদি (ret != ERROR_SUCCESS) {
                Reg_Close_চাবি(h_চাবি_All_Time_Zones);
                ফেরত nullptr;
            }
            
            ret = Reg_Open_চাবি_Ex_W(h_চাবি_All_Time_Zones, timezone_Sub_চাবি_Name, 0, চাবি_READ,
                                reinterpret_cast<PHKEY>(&h_চাবি_Time_Zone_Sub_চাবি));
            
            যদি (ret != ERROR_SUCCESS) {
                Reg_Close_চাবি(h_চাবি_All_Time_Zones);
                ফেরত nullptr;
            }

            // Note: RegQueryValueExW wants the size of the buffer in bytes.
            size = আকার(registryStandardName);
            ret = Reg_Query_মান_Ex_W(h_চাবি_Time_Zone_Sub_চাবি, L"Std", nullptr, &registry_মান_Type,
                                   reinterpret_cast<LPBYTE>(registryStandardName), &size);
            
            যদি (ret != ERROR_SUCCESS || registry_মান_Type != REG_SZ) {
                Reg_Close_চাবি(h_চাবি_Time_Zone_Sub_চাবি);
                Reg_Close_চাবি(h_চাবি_All_Time_Zones);
                ফেরত nullptr;
            }

            // Note: wcscmp does an ordinal (byte) comparison.
            যদি (wcscmp(reinterpret_cast<WCHAR *>(registryStandardName), dynamicTZI.StandardName) == 0) {
                // Since we are comparing the *localized* time zone name, it's possible that some languages might use
                // the same string for more than one time zone. Thus we need to examine the TZI data in the registry to
                // compare the GMT offset (the bias), and the DST transition dates, to ensure it's the same time zone
                // as the currently reported one.
                REG_TZI_FORMAT registry_Tzi_মান;
                uprv_memset(&registry_Tzi_মান, 0, আকার(registry_Tzi_মান));

                // Note: RegQueryValueExW wants the size of the buffer in bytes.
                DWORD timezone_Tzi_মান_আকার = আকার(registry_Tzi_মান);
                ret = Reg_Query_মান_Ex_W(h_চাবি_Time_Zone_Sub_চাবি, L"TZI", nullptr, &registry_মান_Type,
                                     reinterpret_cast<LPBYTE>(&registry_Tzi_মান), &timezone_Tzi_মান_আকার);

                যদি (ret == ERROR_SUCCESS) {
                    যদি ((dynamicTZI.Bias == registry_Tzi_মান.Bias) &&
                        (memcmp((ধ্রুবক শূন্য *)&dynamicTZI.StandardDate, (ধ্রুবক শূন্য *)&registry_Tzi_মান.StandardDate, আকার(SYSTEMTIME)) == 0) &&
                        (memcmp((ধ্রুবক শূন্য *)&dynamicTZI.DaylightDate, (ধ্রুবক শূন্য *)&registry_Tzi_মান.DaylightDate, আকার(SYSTEMTIME)) == 0))
                    {
                        // We found a matching time zone.
                        windowsTimeZoneName = timezone_Sub_চাবি_Name;
                        বিরতি;
                    }
                }
            }
            Reg_Close_চাবি(h_চাবি_Time_Zone_Sub_চাবি);
            h_চাবি_Time_Zone_Sub_চাবি = nullptr;
        }

        যদি (h_চাবি_Time_Zone_Sub_চাবি != nullptr) {
            Reg_Close_চাবি(h_চাবি_Time_Zone_Sub_চাবি);
        }
        যদি (h_চাবি_All_Time_Zones != nullptr) {
            Reg_Close_চাবি(h_চাবি_All_Time_Zones);
        }
#endif // U_PLATFORM_HAS_WINUWP_API
    }

    CharString winTZ;
    UErrorCode status = U_ZERO_ERROR;
    winTZ.appendInvariantChars(UnicodeString(সত্য, windowsTimeZoneName, -1), status);

    // Map Windows Timezone name (non-localized) to ICU timezone ID (~ Olson timezone id).
    স্তূপ_UResource_Bundle winTZBundle;
    ures_openDirectFillIn(winTZBundle.getAlias(), nullptr, "windowsZones", &status);
    ures_get_By_চাবি(winTZBundle.getAlias(), "mapTimezones", winTZBundle.getAlias(), &status);
    ures_get_By_চাবি(winTZBundle.getAlias(), winTZ.তথ্য(), winTZBundle.getAlias(), &status);

    যদি (U_FAILURE(status)) {
        ফেরত nullptr;
    }
    
    // Note: Since the ISO 3166 country/region codes are all invariant ASCII chars, we can
    // directly downcast from wchar_t to do the conversion.
    // We could call the A version of the GetGeoInfo API, but that would be slightly slower than calling the W API,
    // as the A version of the API will end up calling MultiByteToWideChar anyways internally.
    wchar_t regionCodeW[3] = {};
    অক্ষর regionCode[3] = {}; // 2 letter ISO 3166 country/region code made entirely of invariant chars.
    পূর্ণসংখ্যা geoId = GetUserGeoID(GEOCLASS_NATION);
    পূর্ণসংখ্যা regionCodeLen = GetGeoInfoW(geoId, GEO_ISO2, regionCodeW, UPRV_LENGTHOF(regionCodeW), 0);

    ধ্রুবক char16_t *icuTZ16 = nullptr;
    int32_t tzListLen = 0;

    যদি (regionCodeLen != 0) {
        জন্য (পূর্ণসংখ্যা i = 0; i < UPRV_LENGTHOF(regionCodeW); i++) {
            regionCode[i] = static_cast<অক্ষর>(regionCodeW[i]);
        }
        icuTZ16 = ures_get_String_By_চাবি(winTZBundle.getAlias(), regionCode, &tzListLen, &status);
    }
    যদি (regionCodeLen == 0 || U_FAILURE(status)) {
        // fallback to default "001" (world)
        status = U_ZERO_ERROR;
        icuTZ16 = ures_get_String_By_চাবি(winTZBundle.getAlias(), "001", &tzListLen, &status);
    }

    // Note: We want the first entry in the string returned by ures_getStringByKey.
    // However this string can be a space delimited list of timezones:
    //  Ex: "America/New_York America/Detroit America/Indiana/Petersburg ..."
    // We need to stop at the first space, so we pass tzLen (instead of tzListLen) to appendInvariantChars below.
    int32_t tzLen = 0;
    যদি (tzListLen > 0) {
        যখন (!(icuTZ16[tzLen] == u'\0' || icuTZ16[tzLen] == u' ')) {
            tzLen++;
        }
    }

    // Note: cloneData returns nullptr if the status is a failure, so this
    // will return nullptr if the above look-up fails.
    CharString icuTZStr;
    ফেরত icuTZStr.appendInvariantChars(icuTZ16, tzLen, status).clone_তথ্য(status);
}

U_NAMESPACE_END
#endif /* U_PLATFORM_USES_ONLY_WIN32_API  */
