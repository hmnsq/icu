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

#إذا U_PLATFORM_استفاده_میکند_ONLY_WIN32_API

#ضمن "wintz.h"
#ضمن "charstr.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"

#ضمن "unicode/ures.h"
#ضمن "unicode/unistr.h"
#ضمن "uresimp.h"

#اگر_تعریف_نشده WIN32_LEAN_AND_میانگین
#   define WIN32_LEAN_AND_میانگین
#پایان_اگر
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
#تعریف_کن WINDOWS_TIMEZONES_REG_کلید_مسیر L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones"

// Max length for a registry key is 255. +1 for null.
// See: https://docs.microsoft.com/windows/win32/sysinfo/registry-element-size-limits
#تعریف_کن WINDOWS_MAX_REG_کلید_درازی 256

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

#پایان_اگر // U_PLATFORM_HAS_WINUWP_API

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
uprv_detect_Windows_زمان_Zone()
{
    // We first try to obtain the time zone directly by using the TimeZoneKeyName field of the DYNAMIC_TIME_ZONE_INFORMATION struct.
    DYNAMIC_زمان_ZONE_INFORMATION dynamicTZI;
    uprv_memset(&dynamicTZI, 0, حجم(dynamicTZI));
    SYSTEMTIME system_زمان_All_Zero;
    uprv_memset(&system_زمان_All_Zero, 0, حجم(system_زمان_All_Zero));

    إذا (Get_Dynamic_زمان_Zone_Information(&dynamicTZI) == زمان_ZONE_ID_INVALID) {
        أرجع اشاره_تهی;
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
    إذا (dynamicTZI.Dynamic_Daylight_زمان_Disabled != 0 &&
        uprv_memcmp(&dynamicTZI.StandardDate, &dynamicTZI.DaylightDate, حجم(dynamicTZI.StandardDate)) == 0 &&
        ((dynamicTZI.زمان_Zone_کلید_Name[0] != L'\0' && uprv_memcmp(&dynamicTZI.StandardDate, &system_زمان_All_Zero, حجم(system_زمان_All_Zero)) == 0) ||
         (dynamicTZI.زمان_Zone_کلید_Name[0] == L'\0' && uprv_memcmp(&dynamicTZI.StandardDate, &system_زمان_All_Zero, حجم(system_زمان_All_Zero)) != 0)))
    {
        LONG utc_جابه_جایی_Mins = dynamicTZI.Bias;
        إذا (utc_جابه_جایی_Mins == 0) {
            أرجع uprv_strdup("Etc/UTC");
        }

        // No way to support when DST is turned off and the offset in minutes is not a multiple of 60.
        إذا (utc_جابه_جایی_Mins % 60 == 0) {
            حرف gmt_جابه_جایی_Tz[11] = {}; // "Etc/GMT+dd" is 11-char long with a terminal null.
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
            صحيح ret = snprintf(gmt_جابه_جایی_Tz, حجم(gmt_جابه_جایی_Tz), "Etc/GMT%+ld", utc_جابه_جایی_Mins / 60);
            إذا (ret > 0 && ret < UPRV_LENGTHOF(gmt_جابه_جایی_Tz)) {
                أرجع uprv_strdup(gmt_جابه_جایی_Tz);
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
    WCHAR timezone_Sub_کلید_Name[WINDOWS_MAX_REG_کلید_درازی];
    WCHAR *windows_زمان_Zone_Name = dynamicTZI.زمان_Zone_کلید_Name;

    إذا (dynamicTZI.زمان_Zone_کلید_Name[0] == 0) {

// We can't use the registry APIs in the UWP version.
#إذا U_PLATFORM_HAS_WINUWP_API == 1
        (فراغ)timezone_Sub_کلید_Name; // suppress unused variable warnings.
        أرجع اشاره_تهی;
#وإلا
        // Open the path to the time zones in the Windows registry.
        LONG ret;
        HKEY h_کلید_All_زمان_Zones = اشاره_تهی;
        ret = Reg_Open_کلید_Ex_W(HKEY_LOCAL_MACHINE, WINDOWS_TIMEZONES_REG_کلید_مسیر, 0, کلید_READ,
                            reinterpret_cast<PHKEY>(&h_کلید_All_زمان_Zones));
        
        إذا (ret != ERROR_SUCCESS) {
            // If we can't open the key, then we can't do much, so fail.
            أرجع اشاره_تهی;
        }

        // Read the number of subkeys under the time zone registry path.
        DWORD رقم_زمان_Zone_Sub_کلیدها;
        ret = Reg_Query_Info_کلید_W(h_کلید_All_زمان_Zones, اشاره_تهی, اشاره_تهی, اشاره_تهی, &رقم_زمان_Zone_Sub_کلیدها,
                               اشاره_تهی, اشاره_تهی, اشاره_تهی, اشاره_تهی, اشاره_تهی, اشاره_تهی, اشاره_تهی);
        
        إذا (ret != ERROR_SUCCESS) {
            Reg_Close_کلید(h_کلید_All_زمان_Zones);
            أرجع اشاره_تهی;
        }

        // Examine each of the subkeys to try and find a match for the localized standard name ("Std").
        //
        // Note: The name of the time zone subkey itself is not localized, but the "Std" name is localized. This means
        // that we could fail to find a match if the RDP client and RDP server are using different languages, but unfortunately
        // there isn't much we can do about it.
        HKEY h_کلید_زمان_Zone_Sub_کلید = اشاره_تهی;
        ULONG registry_مقدار_Type;
        WCHAR registryStandardName[WINDOWS_MAX_REG_کلید_درازی];

        لكل (DWORD i = 0; i < رقم_زمان_Zone_Sub_کلیدها; i++) {
            // Note: RegEnumKeyExW wants the size of the buffer in characters.
            DWORD size = UPRV_LENGTHOF(timezone_Sub_کلید_Name);
            ret = Reg_Enum_کلید_Ex_W(h_کلید_All_زمان_Zones, i, timezone_Sub_کلید_Name, &size, اشاره_تهی, اشاره_تهی, اشاره_تهی, اشاره_تهی);

            إذا (ret != ERROR_SUCCESS) {
                Reg_Close_کلید(h_کلید_All_زمان_Zones);
                أرجع اشاره_تهی;
            }
            
            ret = Reg_Open_کلید_Ex_W(h_کلید_All_زمان_Zones, timezone_Sub_کلید_Name, 0, کلید_READ,
                                reinterpret_cast<PHKEY>(&h_کلید_زمان_Zone_Sub_کلید));
            
            إذا (ret != ERROR_SUCCESS) {
                Reg_Close_کلید(h_کلید_All_زمان_Zones);
                أرجع اشاره_تهی;
            }

            // Note: RegQueryValueExW wants the size of the buffer in bytes.
            size = حجم(registryStandardName);
            ret = Reg_Query_مقدار_Ex_W(h_کلید_زمان_Zone_Sub_کلید, L"Std", اشاره_تهی, &registry_مقدار_Type,
                                   reinterpret_cast<LPBYTE>(registryStandardName), &size);
            
            إذا (ret != ERROR_SUCCESS || registry_مقدار_Type != REG_SZ) {
                Reg_Close_کلید(h_کلید_زمان_Zone_Sub_کلید);
                Reg_Close_کلید(h_کلید_All_زمان_Zones);
                أرجع اشاره_تهی;
            }

            // Note: wcscmp does an ordinal (byte) comparison.
            إذا (wcscmp(reinterpret_cast<WCHAR *>(registryStandardName), dynamicTZI.StandardName) == 0) {
                // Since we are comparing the *localized* time zone name, it's possible that some languages might use
                // the same string for more than one time zone. Thus we need to examine the TZI data in the registry to
                // compare the GMT offset (the bias), and the DST transition dates, to ensure it's the same time zone
                // as the currently reported one.
                REG_TZI_FORMAT registry_Tzi_مقدار;
                uprv_memset(&registry_Tzi_مقدار, 0, حجم(registry_Tzi_مقدار));

                // Note: RegQueryValueExW wants the size of the buffer in bytes.
                DWORD timezone_Tzi_مقدار_اندازه = حجم(registry_Tzi_مقدار);
                ret = Reg_Query_مقدار_Ex_W(h_کلید_زمان_Zone_Sub_کلید, L"TZI", اشاره_تهی, &registry_مقدار_Type,
                                     reinterpret_cast<LPBYTE>(&registry_Tzi_مقدار), &timezone_Tzi_مقدار_اندازه);

                إذا (ret == ERROR_SUCCESS) {
                    إذا ((dynamicTZI.Bias == registry_Tzi_مقدار.Bias) &&
                        (memcmp((ثابت فراغ *)&dynamicTZI.StandardDate, (ثابت فراغ *)&registry_Tzi_مقدار.StandardDate, حجم(SYSTEMTIME)) == 0) &&
                        (memcmp((ثابت فراغ *)&dynamicTZI.DaylightDate, (ثابت فراغ *)&registry_Tzi_مقدار.DaylightDate, حجم(SYSTEMTIME)) == 0))
                    {
                        // We found a matching time zone.
                        windows_زمان_Zone_Name = timezone_Sub_کلید_Name;
                        اكسر;
                    }
                }
            }
            Reg_Close_کلید(h_کلید_زمان_Zone_Sub_کلید);
            h_کلید_زمان_Zone_Sub_کلید = اشاره_تهی;
        }

        إذا (h_کلید_زمان_Zone_Sub_کلید != اشاره_تهی) {
            Reg_Close_کلید(h_کلید_زمان_Zone_Sub_کلید);
        }
        إذا (h_کلید_All_زمان_Zones != اشاره_تهی) {
            Reg_Close_کلید(h_کلید_All_زمان_Zones);
        }
#پایان_اگر // U_PLATFORM_HAS_WINUWP_API
    }

    CharString winTZ;
    UError_کد وضعیت = U_ZERO_ERROR;
    winTZ.appendInvariantChars(UnicodeString(درست, windows_زمان_Zone_Name, -1), وضعیت);

    // Map Windows Timezone name (non-localized) to ICU timezone ID (~ Olson timezone id).
    پشته_UResource_Bundle winTZBundle;
    ures_open_مستقیم_Fill_درون(winTZBundle.getAlias(), اشاره_تهی, "windowsZones", &وضعیت);
    ures_get_By_کلید(winTZBundle.getAlias(), "mapTimezones", winTZBundle.getAlias(), &وضعیت);
    ures_get_By_کلید(winTZBundle.getAlias(), winTZ.داده(), winTZBundle.getAlias(), &وضعیت);

    إذا (U_FAILURE(وضعیت)) {
        أرجع اشاره_تهی;
    }
    
    // Note: Since the ISO 3166 country/region codes are all invariant ASCII chars, we can
    // directly downcast from wchar_t to do the conversion.
    // We could call the A version of the GetGeoInfo API, but that would be slightly slower than calling the W API,
    // as the A version of the API will end up calling MultiByteToWideChar anyways internally.
    wchar_t region_کد_W[3] = {};
    حرف region_کد[3] = {}; // 2 letter ISO 3166 country/region code made entirely of invariant chars.
    صحيح geoId = GetUserGeoID(GEOCLASS_NATION);
    صحيح region_کد_Len = GetGeoInfoW(geoId, GEO_ISO2, region_کد_W, UPRV_LENGTHOF(region_کد_W), 0);

    ثابت char16_t *icuTZ16 = اشاره_تهی;
    int32_t tzListLen = 0;

    إذا (region_کد_Len != 0) {
        لكل (صحيح i = 0; i < UPRV_LENGTHOF(region_کد_W); i++) {
            region_کد[i] = static_cast<حرف>(region_کد_W[i]);
        }
        icuTZ16 = ures_get_String_By_کلید(winTZBundle.getAlias(), region_کد, &tzListLen, &وضعیت);
    }
    إذا (region_کد_Len == 0 || U_FAILURE(وضعیت)) {
        // fallback to default "001" (world)
        وضعیت = U_ZERO_ERROR;
        icuTZ16 = ures_get_String_By_کلید(winTZBundle.getAlias(), "001", &tzListLen, &وضعیت);
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
    أرجع icuTZStr.appendInvariantChars(icuTZ16, tzLen, وضعیت).clone_داده(وضعیت);
}

U_NAMESPACE_انتها
#پایان_اگر /* U_PLATFORM_USES_ONLY_WIN32_API  */
