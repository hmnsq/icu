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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ U_PLATFORM_ಬಳಸುತ್ತದೆ_ONLY_WIN32_API

#ಅನ್ತರ್ಭಾವ "wintz.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"

#ಅನ್ತರ್ಭಾವ "unicode/ures.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32_LEAN_AND_ಸರಾಸರಿ
#   define WIN32_LEAN_AND_ಸರಾಸರಿ
#ಆದರೆ_ಅಂತ್ಯ
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX
#ಅನ್ತರ್ಭಾವ <windows.h>

U_NAMESPACE_BEGIN

// Note these constants and the struct are only used when dealing with the fallback path for RDP sessions.

// This is the location of the time zones in the registry on Vista+ systems.
// See: https://docs.microsoft.com/windows/win32/api/timezoneapi/ns-timezoneapi-dynamic_time_zone_information
#ವ್ಯಾಖ್ಯಾನಿಸು WINDOWS_TIMEZONES_REG_ಕೀಲಿ_ಮಾರ್ಗ L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones"

// Max length for a registry key is 255. +1 for null.
// See: https://docs.microsoft.com/windows/win32/sysinfo/registry-element-size-limits
#ವ್ಯಾಖ್ಯಾನಿಸು WINDOWS_MAX_REG_ಕೀಲಿ_ಉದ್ದಳತೆ 256

#ಆದರೆ U_PLATFORM_HAS_WINUWP_API == 0

// This is the layout of the TZI binary value in the registry.
// See: https://docs.microsoft.com/windows/win32/api/timezoneapi/ns-timezoneapi-time_zone_information
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ _REG_TZI_FORMAT {
    LONG Bias;
    LONG StandardBias;
    LONG DaylightBias;
    SYSTEMTIME StandardDate;
    SYSTEMTIME DaylightDate;
} REG_TZI_FORMAT;

#ಆದರೆ_ಅಂತ್ಯ // U_PLATFORM_HAS_WINUWP_API

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
U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
uprv_detect_Windows_ಸಮಯ_Zone()
{
    // We first try to obtain the time zone directly by using the TimeZoneKeyName field of the DYNAMIC_TIME_ZONE_INFORMATION struct.
    DYNAMIC_ಸಮಯ_ZONE_INFORMATION dynamicTZI;
    uprv_memset(&dynamicTZI, 0, ಗಾತ್ರ(dynamicTZI));
    SYSTEMTIME system_ಸಮಯ_All_Zero;
    uprv_memset(&system_ಸಮಯ_All_Zero, 0, ಗಾತ್ರ(system_ಸಮಯ_All_Zero));

    ಆದರೆ (Get_Dynamic_ಸಮಯ_Zone_Information(&dynamicTZI) == ಸಮಯ_ZONE_ID_INVALID) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
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
    ಆದರೆ (dynamicTZI.Dynamic_Daylight_ಸಮಯ_Disabled != 0 &&
        uprv_memcmp(&dynamicTZI.StandardDate, &dynamicTZI.DaylightDate, ಗಾತ್ರ(dynamicTZI.StandardDate)) == 0 &&
        ((dynamicTZI.ಸಮಯ_Zone_ಕೀಲಿ_Name[0] != L'\0' && uprv_memcmp(&dynamicTZI.StandardDate, &system_ಸಮಯ_All_Zero, ಗಾತ್ರ(system_ಸಮಯ_All_Zero)) == 0) ||
         (dynamicTZI.ಸಮಯ_Zone_ಕೀಲಿ_Name[0] == L'\0' && uprv_memcmp(&dynamicTZI.StandardDate, &system_ಸಮಯ_All_Zero, ಗಾತ್ರ(system_ಸಮಯ_All_Zero)) != 0)))
    {
        LONG utc_ಸ್ಥಾನಾಂತರ_Mins = dynamicTZI.Bias;
        ಆದರೆ (utc_ಸ್ಥಾನಾಂತರ_Mins == 0) {
            ಹಿಂತಿರುಗಿ uprv_strdup("Etc/UTC");
        }

        // No way to support when DST is turned off and the offset in minutes is not a multiple of 60.
        ಆದರೆ (utc_ಸ್ಥಾನಾಂತರ_Mins % 60 == 0) {
            ಅಕ್ಷರ gmt_ಸ್ಥಾನಾಂತರ_Tz[11] = {}; // "Etc/GMT+dd" is 11-char long with a terminal null.
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
            ಪೂರ್ಣಾಂಕ ret = snprintf(gmt_ಸ್ಥಾನಾಂತರ_Tz, ಗಾತ್ರ(gmt_ಸ್ಥಾನಾಂತರ_Tz), "Etc/GMT%+ld", utc_ಸ್ಥಾನಾಂತರ_Mins / 60);
            ಆದರೆ (ret > 0 && ret < UPRV_LENGTHOF(gmt_ಸ್ಥಾನಾಂತರ_Tz)) {
                ಹಿಂತಿರುಗಿ uprv_strdup(gmt_ಸ್ಥಾನಾಂತರ_Tz);
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
    WCHAR timezone_Sub_ಕೀಲಿ_Name[WINDOWS_MAX_REG_ಕೀಲಿ_ಉದ್ದಳತೆ];
    WCHAR *windows_ಸಮಯ_Zone_Name = dynamicTZI.ಸಮಯ_Zone_ಕೀಲಿ_Name;

    ಆದರೆ (dynamicTZI.ಸಮಯ_Zone_ಕೀಲಿ_Name[0] == 0) {

// We can't use the registry APIs in the UWP version.
#ಆದರೆ U_PLATFORM_HAS_WINUWP_API == 1
        (ಶೂನ್ಯ)timezone_Sub_ಕೀಲಿ_Name; // suppress unused variable warnings.
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
#ಇಲ್ಲದಿದ್ದರೆ
        // Open the path to the time zones in the Windows registry.
        LONG ret;
        HKEY h_ಕೀಲಿ_All_ಸಮಯ_Zones = ಶೂನ್ಯಸೂಚಕ;
        ret = Reg_Open_ಕೀಲಿ_Ex_W(HKEY_LOCAL_MACHINE, WINDOWS_TIMEZONES_REG_ಕೀಲಿ_ಮಾರ್ಗ, 0, ಕೀಲಿ_READ,
                            reinterpret_cast<PHKEY>(&h_ಕೀಲಿ_All_ಸಮಯ_Zones));
        
        ಆದರೆ (ret != ERROR_SUCCESS) {
            // If we can't open the key, then we can't do much, so fail.
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }

        // Read the number of subkeys under the time zone registry path.
        DWORD ಅಂಕೆ_ಸಮಯ_Zone_Sub_ಕೀಲಿಗಳು;
        ret = Reg_Query_Info_ಕೀಲಿ_W(h_ಕೀಲಿ_All_ಸಮಯ_Zones, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, &ಅಂಕೆ_ಸಮಯ_Zone_Sub_ಕೀಲಿಗಳು,
                               ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ);
        
        ಆದರೆ (ret != ERROR_SUCCESS) {
            Reg_Close_ಕೀಲಿ(h_ಕೀಲಿ_All_ಸಮಯ_Zones);
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }

        // Examine each of the subkeys to try and find a match for the localized standard name ("Std").
        //
        // Note: The name of the time zone subkey itself is not localized, but the "Std" name is localized. This means
        // that we could fail to find a match if the RDP client and RDP server are using different languages, but unfortunately
        // there isn't much we can do about it.
        HKEY h_ಕೀಲಿ_ಸಮಯ_Zone_Sub_ಕೀಲಿ = ಶೂನ್ಯಸೂಚಕ;
        ULONG registry_ಮೌಲ್ಯ_Type;
        WCHAR registryStandardName[WINDOWS_MAX_REG_ಕೀಲಿ_ಉದ್ದಳತೆ];

        ಗಾಗಿ (DWORD i = 0; i < ಅಂಕೆ_ಸಮಯ_Zone_Sub_ಕೀಲಿಗಳು; i++) {
            // Note: RegEnumKeyExW wants the size of the buffer in characters.
            DWORD size = UPRV_LENGTHOF(timezone_Sub_ಕೀಲಿ_Name);
            ret = Reg_Enum_ಕೀಲಿ_Ex_W(h_ಕೀಲಿ_All_ಸಮಯ_Zones, i, timezone_Sub_ಕೀಲಿ_Name, &size, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ);

            ಆದರೆ (ret != ERROR_SUCCESS) {
                Reg_Close_ಕೀಲಿ(h_ಕೀಲಿ_All_ಸಮಯ_Zones);
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
            
            ret = Reg_Open_ಕೀಲಿ_Ex_W(h_ಕೀಲಿ_All_ಸಮಯ_Zones, timezone_Sub_ಕೀಲಿ_Name, 0, ಕೀಲಿ_READ,
                                reinterpret_cast<PHKEY>(&h_ಕೀಲಿ_ಸಮಯ_Zone_Sub_ಕೀಲಿ));
            
            ಆದರೆ (ret != ERROR_SUCCESS) {
                Reg_Close_ಕೀಲಿ(h_ಕೀಲಿ_All_ಸಮಯ_Zones);
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }

            // Note: RegQueryValueExW wants the size of the buffer in bytes.
            size = ಗಾತ್ರ(registryStandardName);
            ret = Reg_Query_ಮೌಲ್ಯ_Ex_W(h_ಕೀಲಿ_ಸಮಯ_Zone_Sub_ಕೀಲಿ, L"Std", ಶೂನ್ಯಸೂಚಕ, &registry_ಮೌಲ್ಯ_Type,
                                   reinterpret_cast<LPBYTE>(registryStandardName), &size);
            
            ಆದರೆ (ret != ERROR_SUCCESS || registry_ಮೌಲ್ಯ_Type != REG_SZ) {
                Reg_Close_ಕೀಲಿ(h_ಕೀಲಿ_ಸಮಯ_Zone_Sub_ಕೀಲಿ);
                Reg_Close_ಕೀಲಿ(h_ಕೀಲಿ_All_ಸಮಯ_Zones);
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }

            // Note: wcscmp does an ordinal (byte) comparison.
            ಆದರೆ (wcscmp(reinterpret_cast<WCHAR *>(registryStandardName), dynamicTZI.StandardName) == 0) {
                // Since we are comparing the *localized* time zone name, it's possible that some languages might use
                // the same string for more than one time zone. Thus we need to examine the TZI data in the registry to
                // compare the GMT offset (the bias), and the DST transition dates, to ensure it's the same time zone
                // as the currently reported one.
                REG_TZI_FORMAT registry_Tzi_ಮೌಲ್ಯ;
                uprv_memset(&registry_Tzi_ಮೌಲ್ಯ, 0, ಗಾತ್ರ(registry_Tzi_ಮೌಲ್ಯ));

                // Note: RegQueryValueExW wants the size of the buffer in bytes.
                DWORD timezone_Tzi_ಮೌಲ್ಯ_ಗಾತ್ರ = ಗಾತ್ರ(registry_Tzi_ಮೌಲ್ಯ);
                ret = Reg_Query_ಮೌಲ್ಯ_Ex_W(h_ಕೀಲಿ_ಸಮಯ_Zone_Sub_ಕೀಲಿ, L"TZI", ಶೂನ್ಯಸೂಚಕ, &registry_ಮೌಲ್ಯ_Type,
                                     reinterpret_cast<LPBYTE>(&registry_Tzi_ಮೌಲ್ಯ), &timezone_Tzi_ಮೌಲ್ಯ_ಗಾತ್ರ);

                ಆದರೆ (ret == ERROR_SUCCESS) {
                    ಆದರೆ ((dynamicTZI.Bias == registry_Tzi_ಮೌಲ್ಯ.Bias) &&
                        (memcmp((ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)&dynamicTZI.StandardDate, (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)&registry_Tzi_ಮೌಲ್ಯ.StandardDate, ಗಾತ್ರ(SYSTEMTIME)) == 0) &&
                        (memcmp((ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)&dynamicTZI.DaylightDate, (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)&registry_Tzi_ಮೌಲ್ಯ.DaylightDate, ಗಾತ್ರ(SYSTEMTIME)) == 0))
                    {
                        // We found a matching time zone.
                        windows_ಸಮಯ_Zone_Name = timezone_Sub_ಕೀಲಿ_Name;
                        ನಿಲ್ಲಿಸು;
                    }
                }
            }
            Reg_Close_ಕೀಲಿ(h_ಕೀಲಿ_ಸಮಯ_Zone_Sub_ಕೀಲಿ);
            h_ಕೀಲಿ_ಸಮಯ_Zone_Sub_ಕೀಲಿ = ಶೂನ್ಯಸೂಚಕ;
        }

        ಆದರೆ (h_ಕೀಲಿ_ಸಮಯ_Zone_Sub_ಕೀಲಿ != ಶೂನ್ಯಸೂಚಕ) {
            Reg_Close_ಕೀಲಿ(h_ಕೀಲಿ_ಸಮಯ_Zone_Sub_ಕೀಲಿ);
        }
        ಆದರೆ (h_ಕೀಲಿ_All_ಸಮಯ_Zones != ಶೂನ್ಯಸೂಚಕ) {
            Reg_Close_ಕೀಲಿ(h_ಕೀಲಿ_All_ಸಮಯ_Zones);
        }
#ಆದರೆ_ಅಂತ್ಯ // U_PLATFORM_HAS_WINUWP_API
    }

    CharString winTZ;
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    winTZ.appendInvariantChars(UnicodeString(ನಿಜ, windows_ಸಮಯ_Zone_Name, -1), ಸ್ಥಿತಿ_2);

    // Map Windows Timezone name (non-localized) to ICU timezone ID (~ Olson timezone id).
    ರಾಶಿ_UResource_Bundle winTZBundle;
    ures_open_ನೇರ_Fill_ಒಳ(winTZBundle.getAlias(), ಶೂನ್ಯಸೂಚಕ, "windowsZones", &ಸ್ಥಿತಿ_2);
    ures_get_By_ಕೀಲಿ(winTZBundle.getAlias(), "mapTimezones", winTZBundle.getAlias(), &ಸ್ಥಿತಿ_2);
    ures_get_By_ಕೀಲಿ(winTZBundle.getAlias(), winTZ.ದತ್ತಾಂಶ(), winTZBundle.getAlias(), &ಸ್ಥಿತಿ_2);

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    
    // Note: Since the ISO 3166 country/region codes are all invariant ASCII chars, we can
    // directly downcast from wchar_t to do the conversion.
    // We could call the A version of the GetGeoInfo API, but that would be slightly slower than calling the W API,
    // as the A version of the API will end up calling MultiByteToWideChar anyways internally.
    wchar_t region_ಸಂಕೇತ_W[3] = {};
    ಅಕ್ಷರ region_ಸಂಕೇತ[3] = {}; // 2 letter ISO 3166 country/region code made entirely of invariant chars.
    ಪೂರ್ಣಾಂಕ geoId = GetUserGeoID(GEOCLASS_NATION);
    ಪೂರ್ಣಾಂಕ region_ಸಂಕೇತ_Len = GetGeoInfoW(geoId, GEO_ISO2, region_ಸಂಕೇತ_W, UPRV_LENGTHOF(region_ಸಂಕೇತ_W), 0);

    ಸ್ಥಿರಾಂಕ char16_t *icuTZ16 = ಶೂನ್ಯಸೂಚಕ;
    int32_t tzListLen = 0;

    ಆದರೆ (region_ಸಂಕೇತ_Len != 0) {
        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < UPRV_LENGTHOF(region_ಸಂಕೇತ_W); i++) {
            region_ಸಂಕೇತ[i] = static_cast<ಅಕ್ಷರ>(region_ಸಂಕೇತ_W[i]);
        }
        icuTZ16 = ures_get_String_By_ಕೀಲಿ(winTZBundle.getAlias(), region_ಸಂಕೇತ, &tzListLen, &ಸ್ಥಿತಿ_2);
    }
    ಆದರೆ (region_ಸಂಕೇತ_Len == 0 || U_FAILURE(ಸ್ಥಿತಿ_2)) {
        // fallback to default "001" (world)
        ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
        icuTZ16 = ures_get_String_By_ಕೀಲಿ(winTZBundle.getAlias(), "001", &tzListLen, &ಸ್ಥಿತಿ_2);
    }

    // Note: We want the first entry in the string returned by ures_getStringByKey.
    // However this string can be a space delimited list of timezones:
    //  Ex: "America/New_York America/Detroit America/Indiana/Petersburg ..."
    // We need to stop at the first space, so we pass tzLen (instead of tzListLen) to appendInvariantChars below.
    int32_t tzLen = 0;
    ಆದರೆ (tzListLen > 0) {
        ಆಗ (!(icuTZ16[tzLen] == u'\0' || icuTZ16[tzLen] == u' ')) {
            tzLen++;
        }
    }

    // Note: cloneData returns nullptr if the status is a failure, so this
    // will return nullptr if the above look-up fails.
    CharString icuTZStr;
    ಹಿಂತಿರುಗಿ icuTZStr.appendInvariantChars(icuTZ16, tzLen, ಸ್ಥಿತಿ_2).clone_ದತ್ತಾಂಶ(ಸ್ಥಿತಿ_2);
}

U_NAMESPACE_ಕೊನೆ
#ಆದರೆ_ಅಂತ್ಯ /* U_PLATFORM_USES_ONLY_WIN32_API  */
