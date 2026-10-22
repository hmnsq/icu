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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ U_PLATFORM_ਵਰਤਦਾ_ONLY_WIN32_API

#ਅਨ੍ਤਰ੍ਭਾਵ "wintz.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ures.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uresimp.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32_LEAN_AND_ਔਸਤ
#   define WIN32_LEAN_AND_ਔਸਤ
#ਜੇ_ਅੰਤ
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX
#ਅਨ੍ਤਰ੍ਭਾਵ <windows.h>

U_NAMESPACE_BEGIN

// Note these constants and the struct are only used when dealing with the fallback path for RDP sessions.

// This is the location of the time zones in the registry on Vista+ systems.
// See: https://docs.microsoft.com/windows/win32/api/timezoneapi/ns-timezoneapi-dynamic_time_zone_information
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WINDOWS_TIMEZONES_REG_ਕੁੰਜੀ_ਰਸਤਾ L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones"

// Max length for a registry key is 255. +1 for null.
// See: https://docs.microsoft.com/windows/win32/sysinfo/registry-element-size-limits
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WINDOWS_MAX_REG_ਕੁੰਜੀ_ਲੰਮਾਈ 256

#ਜੇ U_PLATFORM_HAS_WINUWP_API == 0

// This is the layout of the TZI binary value in the registry.
// See: https://docs.microsoft.com/windows/win32/api/timezoneapi/ns-timezoneapi-time_zone_information
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ _REG_TZI_FORMAT {
    LONG Bias;
    LONG StandardBias;
    LONG DaylightBias;
    SYSTEMTIME StandardDate;
    SYSTEMTIME DaylightDate;
} REG_TZI_FORMAT;

#ਜੇ_ਅੰਤ // U_PLATFORM_HAS_WINUWP_API

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
U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
uprv_detect_Windows_ਸਮਾਂ_Zone()
{
    // We first try to obtain the time zone directly by using the TimeZoneKeyName field of the DYNAMIC_TIME_ZONE_INFORMATION struct.
    DYNAMIC_ਸਮਾਂ_ZONE_INFORMATION dynamicTZI;
    uprv_memset(&dynamicTZI, 0, ਆਕਾਰ(dynamicTZI));
    SYSTEMTIME system_ਸਮਾਂ_All_Zero;
    uprv_memset(&system_ਸਮਾਂ_All_Zero, 0, ਆਕਾਰ(system_ਸਮਾਂ_All_Zero));

    ਜੇ (Get_Dynamic_ਸਮਾਂ_Zone_Information(&dynamicTZI) == ਸਮਾਂ_ZONE_ID_INVALID) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
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
    ਜੇ (dynamicTZI.Dynamic_Daylight_ਸਮਾਂ_Disabled != 0 &&
        uprv_memcmp(&dynamicTZI.StandardDate, &dynamicTZI.DaylightDate, ਆਕਾਰ(dynamicTZI.StandardDate)) == 0 &&
        ((dynamicTZI.ਸਮਾਂ_Zone_ਕੁੰਜੀ_Name[0] != L'\0' && uprv_memcmp(&dynamicTZI.StandardDate, &system_ਸਮਾਂ_All_Zero, ਆਕਾਰ(system_ਸਮਾਂ_All_Zero)) == 0) ||
         (dynamicTZI.ਸਮਾਂ_Zone_ਕੁੰਜੀ_Name[0] == L'\0' && uprv_memcmp(&dynamicTZI.StandardDate, &system_ਸਮਾਂ_All_Zero, ਆਕਾਰ(system_ਸਮਾਂ_All_Zero)) != 0)))
    {
        LONG utc_ਵਿੱਥ_Mins = dynamicTZI.Bias;
        ਜੇ (utc_ਵਿੱਥ_Mins == 0) {
            ਵਾਪਸ uprv_strdup("Etc/UTC");
        }

        // No way to support when DST is turned off and the offset in minutes is not a multiple of 60.
        ਜੇ (utc_ਵਿੱਥ_Mins % 60 == 0) {
            ਅੱਖਰ gmt_ਵਿੱਥ_Tz[11] = {}; // "Etc/GMT+dd" is 11-char long with a terminal null.
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
            ਪੂਰਨ ret = snprintf(gmt_ਵਿੱਥ_Tz, ਆਕਾਰ(gmt_ਵਿੱਥ_Tz), "Etc/GMT%+ld", utc_ਵਿੱਥ_Mins / 60);
            ਜੇ (ret > 0 && ret < UPRV_LENGTHOF(gmt_ਵਿੱਥ_Tz)) {
                ਵਾਪਸ uprv_strdup(gmt_ਵਿੱਥ_Tz);
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
    WCHAR timezone_Sub_ਕੁੰਜੀ_Name[WINDOWS_MAX_REG_ਕੁੰਜੀ_ਲੰਮਾਈ];
    WCHAR *windows_ਸਮਾਂ_Zone_Name = dynamicTZI.ਸਮਾਂ_Zone_ਕੁੰਜੀ_Name;

    ਜੇ (dynamicTZI.ਸਮਾਂ_Zone_ਕੁੰਜੀ_Name[0] == 0) {

// We can't use the registry APIs in the UWP version.
#ਜੇ U_PLATFORM_HAS_WINUWP_API == 1
        (ਖਾਲੀ)timezone_Sub_ਕੁੰਜੀ_Name; // suppress unused variable warnings.
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
#ਨਹੀਂਤਾਂ
        // Open the path to the time zones in the Windows registry.
        LONG ret;
        HKEY h_ਕੁੰਜੀ_All_ਸਮਾਂ_Zones = ਖਾਲੀ_ਸੂਚਕ;
        ret = Reg_Open_ਕੁੰਜੀ_Ex_W(HKEY_LOCAL_MACHINE, WINDOWS_TIMEZONES_REG_ਕੁੰਜੀ_ਰਸਤਾ, 0, ਕੁੰਜੀ_READ,
                            reinterpret_cast<PHKEY>(&h_ਕੁੰਜੀ_All_ਸਮਾਂ_Zones));
        
        ਜੇ (ret != ERROR_SUCCESS) {
            // If we can't open the key, then we can't do much, so fail.
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }

        // Read the number of subkeys under the time zone registry path.
        DWORD ਅੰਕ_ਸਮਾਂ_Zone_Sub_ਕੁੰਜੀਆਂ;
        ret = Reg_Query_Info_ਕੁੰਜੀ_W(h_ਕੁੰਜੀ_All_ਸਮਾਂ_Zones, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, &ਅੰਕ_ਸਮਾਂ_Zone_Sub_ਕੁੰਜੀਆਂ,
                               ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ);
        
        ਜੇ (ret != ERROR_SUCCESS) {
            Reg_Close_ਕੁੰਜੀ(h_ਕੁੰਜੀ_All_ਸਮਾਂ_Zones);
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }

        // Examine each of the subkeys to try and find a match for the localized standard name ("Std").
        //
        // Note: The name of the time zone subkey itself is not localized, but the "Std" name is localized. This means
        // that we could fail to find a match if the RDP client and RDP server are using different languages, but unfortunately
        // there isn't much we can do about it.
        HKEY h_ਕੁੰਜੀ_ਸਮਾਂ_Zone_Sub_ਕੁੰਜੀ = ਖਾਲੀ_ਸੂਚਕ;
        ULONG registry_ਮੁੱਲ_Type;
        WCHAR registryStandardName[WINDOWS_MAX_REG_ਕੁੰਜੀ_ਲੰਮਾਈ];

        ਲਈ (DWORD i = 0; i < ਅੰਕ_ਸਮਾਂ_Zone_Sub_ਕੁੰਜੀਆਂ; i++) {
            // Note: RegEnumKeyExW wants the size of the buffer in characters.
            DWORD size = UPRV_LENGTHOF(timezone_Sub_ਕੁੰਜੀ_Name);
            ret = Reg_Enum_ਕੁੰਜੀ_Ex_W(h_ਕੁੰਜੀ_All_ਸਮਾਂ_Zones, i, timezone_Sub_ਕੁੰਜੀ_Name, &size, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ);

            ਜੇ (ret != ERROR_SUCCESS) {
                Reg_Close_ਕੁੰਜੀ(h_ਕੁੰਜੀ_All_ਸਮਾਂ_Zones);
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
            
            ret = Reg_Open_ਕੁੰਜੀ_Ex_W(h_ਕੁੰਜੀ_All_ਸਮਾਂ_Zones, timezone_Sub_ਕੁੰਜੀ_Name, 0, ਕੁੰਜੀ_READ,
                                reinterpret_cast<PHKEY>(&h_ਕੁੰਜੀ_ਸਮਾਂ_Zone_Sub_ਕੁੰਜੀ));
            
            ਜੇ (ret != ERROR_SUCCESS) {
                Reg_Close_ਕੁੰਜੀ(h_ਕੁੰਜੀ_All_ਸਮਾਂ_Zones);
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }

            // Note: RegQueryValueExW wants the size of the buffer in bytes.
            size = ਆਕਾਰ(registryStandardName);
            ret = Reg_Query_ਮੁੱਲ_Ex_W(h_ਕੁੰਜੀ_ਸਮਾਂ_Zone_Sub_ਕੁੰਜੀ, L"Std", ਖਾਲੀ_ਸੂਚਕ, &registry_ਮੁੱਲ_Type,
                                   reinterpret_cast<LPBYTE>(registryStandardName), &size);
            
            ਜੇ (ret != ERROR_SUCCESS || registry_ਮੁੱਲ_Type != REG_SZ) {
                Reg_Close_ਕੁੰਜੀ(h_ਕੁੰਜੀ_ਸਮਾਂ_Zone_Sub_ਕੁੰਜੀ);
                Reg_Close_ਕੁੰਜੀ(h_ਕੁੰਜੀ_All_ਸਮਾਂ_Zones);
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }

            // Note: wcscmp does an ordinal (byte) comparison.
            ਜੇ (wcscmp(reinterpret_cast<WCHAR *>(registryStandardName), dynamicTZI.StandardName) == 0) {
                // Since we are comparing the *localized* time zone name, it's possible that some languages might use
                // the same string for more than one time zone. Thus we need to examine the TZI data in the registry to
                // compare the GMT offset (the bias), and the DST transition dates, to ensure it's the same time zone
                // as the currently reported one.
                REG_TZI_FORMAT registry_Tzi_ਮੁੱਲ;
                uprv_memset(&registry_Tzi_ਮੁੱਲ, 0, ਆਕਾਰ(registry_Tzi_ਮੁੱਲ));

                // Note: RegQueryValueExW wants the size of the buffer in bytes.
                DWORD timezone_Tzi_ਮੁੱਲ_ਮਾਪ = ਆਕਾਰ(registry_Tzi_ਮੁੱਲ);
                ret = Reg_Query_ਮੁੱਲ_Ex_W(h_ਕੁੰਜੀ_ਸਮਾਂ_Zone_Sub_ਕੁੰਜੀ, L"TZI", ਖਾਲੀ_ਸੂਚਕ, &registry_ਮੁੱਲ_Type,
                                     reinterpret_cast<LPBYTE>(&registry_Tzi_ਮੁੱਲ), &timezone_Tzi_ਮੁੱਲ_ਮਾਪ);

                ਜੇ (ret == ERROR_SUCCESS) {
                    ਜੇ ((dynamicTZI.Bias == registry_Tzi_ਮੁੱਲ.Bias) &&
                        (memcmp((ਅਚਲ ਖਾਲੀ *)&dynamicTZI.StandardDate, (ਅਚਲ ਖਾਲੀ *)&registry_Tzi_ਮੁੱਲ.StandardDate, ਆਕਾਰ(SYSTEMTIME)) == 0) &&
                        (memcmp((ਅਚਲ ਖਾਲੀ *)&dynamicTZI.DaylightDate, (ਅਚਲ ਖਾਲੀ *)&registry_Tzi_ਮੁੱਲ.DaylightDate, ਆਕਾਰ(SYSTEMTIME)) == 0))
                    {
                        // We found a matching time zone.
                        windows_ਸਮਾਂ_Zone_Name = timezone_Sub_ਕੁੰਜੀ_Name;
                        ਰੋਕੋ;
                    }
                }
            }
            Reg_Close_ਕੁੰਜੀ(h_ਕੁੰਜੀ_ਸਮਾਂ_Zone_Sub_ਕੁੰਜੀ);
            h_ਕੁੰਜੀ_ਸਮਾਂ_Zone_Sub_ਕੁੰਜੀ = ਖਾਲੀ_ਸੂਚਕ;
        }

        ਜੇ (h_ਕੁੰਜੀ_ਸਮਾਂ_Zone_Sub_ਕੁੰਜੀ != ਖਾਲੀ_ਸੂਚਕ) {
            Reg_Close_ਕੁੰਜੀ(h_ਕੁੰਜੀ_ਸਮਾਂ_Zone_Sub_ਕੁੰਜੀ);
        }
        ਜੇ (h_ਕੁੰਜੀ_All_ਸਮਾਂ_Zones != ਖਾਲੀ_ਸੂਚਕ) {
            Reg_Close_ਕੁੰਜੀ(h_ਕੁੰਜੀ_All_ਸਮਾਂ_Zones);
        }
#ਜੇ_ਅੰਤ // U_PLATFORM_HAS_WINUWP_API
    }

    CharString winTZ;
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    winTZ.appendInvariantChars(UnicodeString(ਸੱਚ, windows_ਸਮਾਂ_Zone_Name, -1), ਸਥਿਤੀ);

    // Map Windows Timezone name (non-localized) to ICU timezone ID (~ Olson timezone id).
    ਢੇਰ_UResource_Bundle winTZBundle;
    ures_open_ਸਿੱਧਾ_Fill_ਵਿੱਚ(winTZBundle.getAlias(), ਖਾਲੀ_ਸੂਚਕ, "windowsZones", &ਸਥਿਤੀ);
    ures_get_By_ਕੁੰਜੀ(winTZBundle.getAlias(), "mapTimezones", winTZBundle.getAlias(), &ਸਥਿਤੀ);
    ures_get_By_ਕੁੰਜੀ(winTZBundle.getAlias(), winTZ.ਅੰਕੜੇ(), winTZBundle.getAlias(), &ਸਥਿਤੀ);

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    
    // Note: Since the ISO 3166 country/region codes are all invariant ASCII chars, we can
    // directly downcast from wchar_t to do the conversion.
    // We could call the A version of the GetGeoInfo API, but that would be slightly slower than calling the W API,
    // as the A version of the API will end up calling MultiByteToWideChar anyways internally.
    wchar_t region_ਕੋਡ_W[3] = {};
    ਅੱਖਰ region_ਕੋਡ[3] = {}; // 2 letter ISO 3166 country/region code made entirely of invariant chars.
    ਪੂਰਨ geoId = GetUserGeoID(GEOCLASS_NATION);
    ਪੂਰਨ region_ਕੋਡ_Len = GetGeoInfoW(geoId, GEO_ISO2, region_ਕੋਡ_W, UPRV_LENGTHOF(region_ਕੋਡ_W), 0);

    ਅਚਲ char16_t *icuTZ16 = ਖਾਲੀ_ਸੂਚਕ;
    int32_t tzListLen = 0;

    ਜੇ (region_ਕੋਡ_Len != 0) {
        ਲਈ (ਪੂਰਨ i = 0; i < UPRV_LENGTHOF(region_ਕੋਡ_W); i++) {
            region_ਕੋਡ[i] = static_cast<ਅੱਖਰ>(region_ਕੋਡ_W[i]);
        }
        icuTZ16 = ures_get_String_By_ਕੁੰਜੀ(winTZBundle.getAlias(), region_ਕੋਡ, &tzListLen, &ਸਥਿਤੀ);
    }
    ਜੇ (region_ਕੋਡ_Len == 0 || U_FAILURE(ਸਥਿਤੀ)) {
        // fallback to default "001" (world)
        ਸਥਿਤੀ = U_ZERO_ERROR;
        icuTZ16 = ures_get_String_By_ਕੁੰਜੀ(winTZBundle.getAlias(), "001", &tzListLen, &ਸਥਿਤੀ);
    }

    // Note: We want the first entry in the string returned by ures_getStringByKey.
    // However this string can be a space delimited list of timezones:
    //  Ex: "America/New_York America/Detroit America/Indiana/Petersburg ..."
    // We need to stop at the first space, so we pass tzLen (instead of tzListLen) to appendInvariantChars below.
    int32_t tzLen = 0;
    ਜੇ (tzListLen > 0) {
        ਜਦੋਂ (!(icuTZ16[tzLen] == u'\0' || icuTZ16[tzLen] == u' ')) {
            tzLen++;
        }
    }

    // Note: cloneData returns nullptr if the status is a failure, so this
    // will return nullptr if the above look-up fails.
    CharString icuTZStr;
    ਵਾਪਸ icuTZStr.appendInvariantChars(icuTZ16, tzLen, ਸਥਿਤੀ).clone_ਅੰਕੜੇ(ਸਥਿਤੀ);
}

U_NAMESPACE_ਸਿਰਾ
#ਜੇ_ਅੰਤ /* U_PLATFORM_USES_ONLY_WIN32_API  */
