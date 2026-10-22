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

#ضمن "unicode/utypes.h"

#إذا U_PLATFORM_استعمال_ڪري_ONLY_WIN32_API

#ضمن "wintz.h"
#ضمن "charstr.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"

#ضمن "unicode/ures.h"
#ضمن "unicode/unistr.h"
#ضمن "uresimp.h"

#جيڪڏهن_وصف_نه_ٿيل WIN32_LEAN_AND_سراسري
#   define WIN32_LEAN_AND_سراسري
#جيڪڏهن_پورو
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX
#ضمن <windows.h>

U_NAMESPACE_BEGIN

// Note these constants and the struct are only used when dealing with the fallback path for RDP sessions.

// This is the location of the time zones in the registry on Vista+ systems.
// See: https://docs.microsoft.com/windows/win32/api/timezoneapi/ns-timezoneapi-dynamic_time_zone_information
#وصف_ڪريو WINDOWS_TIMEZONES_REG_ڪنجي_رستو L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones"

// Max length for a registry key is 255. +1 for null.
// See: https://docs.microsoft.com/windows/win32/sysinfo/registry-element-size-limits
#وصف_ڪريو WINDOWS_MAX_REG_ڪنجي_ڊيگهائي 256

#إذا U_PLATFORM_HAS_WINUWP_API == 0

// This is the layout of the TZI binary value in the registry.
// See: https://docs.microsoft.com/windows/win32/api/timezoneapi/ns-timezoneapi-time_zone_information
تعريفنوع بنية _REG_TZI_FORMAT {
    LONG Bias;
    LONG StandardBias;
    LONG DaylightBias;
    SYSTEMTIME StandardDate;
    SYSTEMTIME DaylightDate;
} REG_TZI_FORMAT;

#جيڪڏهن_پورو // U_PLATFORM_HAS_WINUWP_API

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
U_CAPI ثابت حرف* U_EXPORT2
uprv_detect_Windows_وقت_Zone()
{
    // We first try to obtain the time zone directly by using the TimeZoneKeyName field of the DYNAMIC_TIME_ZONE_INFORMATION struct.
    DYNAMIC_وقت_ZONE_INFORMATION dynamicTZI;
    uprv_memset(&dynamicTZI, 0, حجم(dynamicTZI));
    SYSTEMTIME system_وقت_All_Zero;
    uprv_memset(&system_وقت_All_Zero, 0, حجم(system_وقت_All_Zero));

    إذا (Get_Dynamic_وقت_Zone_Information(&dynamicTZI) == وقت_ZONE_ID_INVALID) {
        أرجع خالي_اشارو;
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
    إذا (dynamicTZI.Dynamic_Daylight_وقت_Disabled != 0 &&
        uprv_memcmp(&dynamicTZI.StandardDate, &dynamicTZI.DaylightDate, حجم(dynamicTZI.StandardDate)) == 0 &&
        ((dynamicTZI.وقت_Zone_ڪنجي_Name[0] != L'\0' && uprv_memcmp(&dynamicTZI.StandardDate, &system_وقت_All_Zero, حجم(system_وقت_All_Zero)) == 0) ||
         (dynamicTZI.وقت_Zone_ڪنجي_Name[0] == L'\0' && uprv_memcmp(&dynamicTZI.StandardDate, &system_وقت_All_Zero, حجم(system_وقت_All_Zero)) != 0)))
    {
        LONG utc_وٿي_Mins = dynamicTZI.Bias;
        إذا (utc_وٿي_Mins == 0) {
            أرجع uprv_strdup("Etc/UTC");
        }

        // No way to support when DST is turned off and the offset in minutes is not a multiple of 60.
        إذا (utc_وٿي_Mins % 60 == 0) {
            حرف gmt_وٿي_Tz[11] = {}; // "Etc/GMT+dd" is 11-char long with a terminal null.
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
            صحيح ret = snprintf(gmt_وٿي_Tz, حجم(gmt_وٿي_Tz), "Etc/GMT%+ld", utc_وٿي_Mins / 60);
            إذا (ret > 0 && ret < UPRV_LENGTHOF(gmt_وٿي_Tz)) {
                أرجع uprv_strdup(gmt_وٿي_Tz);
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
    WCHAR timezone_Sub_ڪنجي_Name[WINDOWS_MAX_REG_ڪنجي_ڊيگهائي];
    WCHAR *windows_وقت_Zone_Name = dynamicTZI.وقت_Zone_ڪنجي_Name;

    إذا (dynamicTZI.وقت_Zone_ڪنجي_Name[0] == 0) {

// We can't use the registry APIs in the UWP version.
#إذا U_PLATFORM_HAS_WINUWP_API == 1
        (فراغ)timezone_Sub_ڪنجي_Name; // suppress unused variable warnings.
        أرجع خالي_اشارو;
#وإلا
        // Open the path to the time zones in the Windows registry.
        LONG ret;
        HKEY h_ڪنجي_All_وقت_Zones = خالي_اشارو;
        ret = Reg_Open_ڪنجي_Ex_W(HKEY_LOCAL_MACHINE, WINDOWS_TIMEZONES_REG_ڪنجي_رستو, 0, ڪنجي_READ,
                            reinterpret_cast<PHKEY>(&h_ڪنجي_All_وقت_Zones));
        
        إذا (ret != ERROR_SUCCESS) {
            // If we can't open the key, then we can't do much, so fail.
            أرجع خالي_اشارو;
        }

        // Read the number of subkeys under the time zone registry path.
        DWORD انگ_وقت_Zone_Sub_ڪنجيون;
        ret = Reg_Query_Info_ڪنجي_W(h_ڪنجي_All_وقت_Zones, خالي_اشارو, خالي_اشارو, خالي_اشارو, &انگ_وقت_Zone_Sub_ڪنجيون,
                               خالي_اشارو, خالي_اشارو, خالي_اشارو, خالي_اشارو, خالي_اشارو, خالي_اشارو, خالي_اشارو);
        
        إذا (ret != ERROR_SUCCESS) {
            Reg_Close_ڪنجي(h_ڪنجي_All_وقت_Zones);
            أرجع خالي_اشارو;
        }

        // Examine each of the subkeys to try and find a match for the localized standard name ("Std").
        //
        // Note: The name of the time zone subkey itself is not localized, but the "Std" name is localized. This means
        // that we could fail to find a match if the RDP client and RDP server are using different languages, but unfortunately
        // there isn't much we can do about it.
        HKEY h_ڪنجي_وقت_Zone_Sub_ڪنجي = خالي_اشارو;
        ULONG registry_قدر_Type;
        WCHAR registryStandardName[WINDOWS_MAX_REG_ڪنجي_ڊيگهائي];

        لكل (DWORD i = 0; i < انگ_وقت_Zone_Sub_ڪنجيون; i++) {
            // Note: RegEnumKeyExW wants the size of the buffer in characters.
            DWORD size = UPRV_LENGTHOF(timezone_Sub_ڪنجي_Name);
            ret = Reg_Enum_ڪنجي_Ex_W(h_ڪنجي_All_وقت_Zones, i, timezone_Sub_ڪنجي_Name, &size, خالي_اشارو, خالي_اشارو, خالي_اشارو, خالي_اشارو);

            إذا (ret != ERROR_SUCCESS) {
                Reg_Close_ڪنجي(h_ڪنجي_All_وقت_Zones);
                أرجع خالي_اشارو;
            }
            
            ret = Reg_Open_ڪنجي_Ex_W(h_ڪنجي_All_وقت_Zones, timezone_Sub_ڪنجي_Name, 0, ڪنجي_READ,
                                reinterpret_cast<PHKEY>(&h_ڪنجي_وقت_Zone_Sub_ڪنجي));
            
            إذا (ret != ERROR_SUCCESS) {
                Reg_Close_ڪنجي(h_ڪنجي_All_وقت_Zones);
                أرجع خالي_اشارو;
            }

            // Note: RegQueryValueExW wants the size of the buffer in bytes.
            size = حجم(registryStandardName);
            ret = Reg_Query_قدر_Ex_W(h_ڪنجي_وقت_Zone_Sub_ڪنجي, L"Std", خالي_اشارو, &registry_قدر_Type,
                                   reinterpret_cast<LPBYTE>(registryStandardName), &size);
            
            إذا (ret != ERROR_SUCCESS || registry_قدر_Type != REG_SZ) {
                Reg_Close_ڪنجي(h_ڪنجي_وقت_Zone_Sub_ڪنجي);
                Reg_Close_ڪنجي(h_ڪنجي_All_وقت_Zones);
                أرجع خالي_اشارو;
            }

            // Note: wcscmp does an ordinal (byte) comparison.
            إذا (wcscmp(reinterpret_cast<WCHAR *>(registryStandardName), dynamicTZI.StandardName) == 0) {
                // Since we are comparing the *localized* time zone name, it's possible that some languages might use
                // the same string for more than one time zone. Thus we need to examine the TZI data in the registry to
                // compare the GMT offset (the bias), and the DST transition dates, to ensure it's the same time zone
                // as the currently reported one.
                REG_TZI_FORMAT registry_Tzi_قدر;
                uprv_memset(&registry_Tzi_قدر, 0, حجم(registry_Tzi_قدر));

                // Note: RegQueryValueExW wants the size of the buffer in bytes.
                DWORD timezone_Tzi_قدر_سائيز = حجم(registry_Tzi_قدر);
                ret = Reg_Query_قدر_Ex_W(h_ڪنجي_وقت_Zone_Sub_ڪنجي, L"TZI", خالي_اشارو, &registry_قدر_Type,
                                     reinterpret_cast<LPBYTE>(&registry_Tzi_قدر), &timezone_Tzi_قدر_سائيز);

                إذا (ret == ERROR_SUCCESS) {
                    إذا ((dynamicTZI.Bias == registry_Tzi_قدر.Bias) &&
                        (memcmp((ثابت فراغ *)&dynamicTZI.StandardDate, (ثابت فراغ *)&registry_Tzi_قدر.StandardDate, حجم(SYSTEMTIME)) == 0) &&
                        (memcmp((ثابت فراغ *)&dynamicTZI.DaylightDate, (ثابت فراغ *)&registry_Tzi_قدر.DaylightDate, حجم(SYSTEMTIME)) == 0))
                    {
                        // We found a matching time zone.
                        windows_وقت_Zone_Name = timezone_Sub_ڪنجي_Name;
                        اكسر;
                    }
                }
            }
            Reg_Close_ڪنجي(h_ڪنجي_وقت_Zone_Sub_ڪنجي);
            h_ڪنجي_وقت_Zone_Sub_ڪنجي = خالي_اشارو;
        }

        إذا (h_ڪنجي_وقت_Zone_Sub_ڪنجي != خالي_اشارو) {
            Reg_Close_ڪنجي(h_ڪنجي_وقت_Zone_Sub_ڪنجي);
        }
        إذا (h_ڪنجي_All_وقت_Zones != خالي_اشارو) {
            Reg_Close_ڪنجي(h_ڪنجي_All_وقت_Zones);
        }
#جيڪڏهن_پورو // U_PLATFORM_HAS_WINUWP_API
    }

    CharString winTZ;
    UError_ڪوڊ حالت = U_ZERO_ERROR;
    winTZ.appendInvariantChars(UnicodeString(سچ, windows_وقت_Zone_Name, -1), حالت);

    // Map Windows Timezone name (non-localized) to ICU timezone ID (~ Olson timezone id).
    ڍير_UResource_Bundle winTZBundle;
    ures_open_سڌو_Fill_اندريون(winTZBundle.getAlias(), خالي_اشارو, "windowsZones", &حالت);
    ures_get_By_ڪنجي(winTZBundle.getAlias(), "mapTimezones", winTZBundle.getAlias(), &حالت);
    ures_get_By_ڪنجي(winTZBundle.getAlias(), winTZ.معلومات(), winTZBundle.getAlias(), &حالت);

    إذا (U_FAILURE(حالت)) {
        أرجع خالي_اشارو;
    }
    
    // Note: Since the ISO 3166 country/region codes are all invariant ASCII chars, we can
    // directly downcast from wchar_t to do the conversion.
    // We could call the A version of the GetGeoInfo API, but that would be slightly slower than calling the W API,
    // as the A version of the API will end up calling MultiByteToWideChar anyways internally.
    wchar_t region_ڪوڊ_W[3] = {};
    حرف region_ڪوڊ[3] = {}; // 2 letter ISO 3166 country/region code made entirely of invariant chars.
    صحيح geoId = GetUserGeoID(GEOCLASS_NATION);
    صحيح region_ڪوڊ_Len = GetGeoInfoW(geoId, GEO_ISO2, region_ڪوڊ_W, UPRV_LENGTHOF(region_ڪوڊ_W), 0);

    ثابت char16_t *icuTZ16 = خالي_اشارو;
    int32_t tzListLen = 0;

    إذا (region_ڪوڊ_Len != 0) {
        لكل (صحيح i = 0; i < UPRV_LENGTHOF(region_ڪوڊ_W); i++) {
            region_ڪوڊ[i] = static_cast<حرف>(region_ڪوڊ_W[i]);
        }
        icuTZ16 = ures_get_String_By_ڪنجي(winTZBundle.getAlias(), region_ڪوڊ, &tzListLen, &حالت);
    }
    إذا (region_ڪوڊ_Len == 0 || U_FAILURE(حالت)) {
        // fallback to default "001" (world)
        حالت = U_ZERO_ERROR;
        icuTZ16 = ures_get_String_By_ڪنجي(winTZBundle.getAlias(), "001", &tzListLen, &حالت);
    }

    // Note: We want the first entry in the string returned by ures_getStringByKey.
    // However this string can be a space delimited list of timezones:
    //  Ex: "America/New_York America/Detroit America/Indiana/Petersburg ..."
    // We need to stop at the first space, so we pass tzLen (instead of tzListLen) to appendInvariantChars below.
    int32_t tzLen = 0;
    إذا (tzListLen > 0) {
        طالما (!(icuTZ16[tzLen] == u'\0' || icuTZ16[tzLen] == u' ')) {
            tzLen++;
        }
    }

    // Note: cloneData returns nullptr if the status is a failure, so this
    // will return nullptr if the above look-up fails.
    CharString icuTZStr;
    أرجع icuTZStr.appendInvariantChars(icuTZ16, tzLen, حالت).clone_معلومات(حالت);
}

U_NAMESPACE_پڇاڙي
#جيڪڏهن_پورو /* U_PLATFORM_USES_ONLY_WIN32_API  */
