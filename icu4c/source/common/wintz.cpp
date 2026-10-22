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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API

#అన్తర్భావ "wintz.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"

#అన్తర్భావ "unicode/ures.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "uresimp.h"

#నిర్వచితం_కాకపోతే WIN32_LEAN_AND_సగటు
#   define WIN32_LEAN_AND_సగటు
#అయితే_ముగింపు
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX
#అన్తర్భావ <windows.h>

U_NAMESPACE_BEGIN

// Note these constants and the struct are only used when dealing with the fallback path for RDP sessions.

// This is the location of the time zones in the registry on Vista+ systems.
// See: https://docs.microsoft.com/windows/win32/api/timezoneapi/ns-timezoneapi-dynamic_time_zone_information
#నిర్వచించు WINDOWS_TIMEZONES_REG_కీలకం_మార్గం L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones"

// Max length for a registry key is 255. +1 for null.
// See: https://docs.microsoft.com/windows/win32/sysinfo/registry-element-size-limits
#నిర్వచించు WINDOWS_MAX_REG_కీలకం_నిడివి 256

#ఒకవేళ U_PLATFORM_HAS_WINUWP_API == 0

// This is the layout of the TZI binary value in the registry.
// See: https://docs.microsoft.com/windows/win32/api/timezoneapi/ns-timezoneapi-time_zone_information
రకనిర్వచనం నిర్మాణం _REG_TZI_FORMAT {
    LONG Bias;
    LONG StandardBias;
    LONG DaylightBias;
    SYSTEMTIME StandardDate;
    SYSTEMTIME DaylightDate;
} REG_TZI_FORMAT;

#అయితే_ముగింపు // U_PLATFORM_HAS_WINUWP_API

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
U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
uprv_detect_Windows_సమయం_Zone()
{
    // We first try to obtain the time zone directly by using the TimeZoneKeyName field of the DYNAMIC_TIME_ZONE_INFORMATION struct.
    DYNAMIC_సమయం_ZONE_INFORMATION dynamicTZI;
    uprv_memset(&dynamicTZI, 0, పరిమాణం(dynamicTZI));
    SYSTEMTIME system_సమయం_All_Zero;
    uprv_memset(&system_సమయం_All_Zero, 0, పరిమాణం(system_సమయం_All_Zero));

    ఒకవేళ (Get_Dynamic_సమయం_Zone_Information(&dynamicTZI) == సమయం_ZONE_ID_INVALID) {
        తిరిగి శూన్యసూచిక;
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
    ఒకవేళ (dynamicTZI.Dynamic_Daylight_సమయం_Disabled != 0 &&
        uprv_memcmp(&dynamicTZI.StandardDate, &dynamicTZI.DaylightDate, పరిమాణం(dynamicTZI.StandardDate)) == 0 &&
        ((dynamicTZI.సమయం_Zone_కీలకం_Name[0] != L'\0' && uprv_memcmp(&dynamicTZI.StandardDate, &system_సమయం_All_Zero, పరిమాణం(system_సమయం_All_Zero)) == 0) ||
         (dynamicTZI.సమయం_Zone_కీలకం_Name[0] == L'\0' && uprv_memcmp(&dynamicTZI.StandardDate, &system_సమయం_All_Zero, పరిమాణం(system_సమయం_All_Zero)) != 0)))
    {
        LONG utc_స్థానభ్రంశం_Mins = dynamicTZI.Bias;
        ఒకవేళ (utc_స్థానభ్రంశం_Mins == 0) {
            తిరిగి uprv_strdup("Etc/UTC");
        }

        // No way to support when DST is turned off and the offset in minutes is not a multiple of 60.
        ఒకవేళ (utc_స్థానభ్రంశం_Mins % 60 == 0) {
            అక్షరం gmt_స్థానభ్రంశం_Tz[11] = {}; // "Etc/GMT+dd" is 11-char long with a terminal null.
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
            పూర్ణాంకం ret = snprintf(gmt_స్థానభ్రంశం_Tz, పరిమాణం(gmt_స్థానభ్రంశం_Tz), "Etc/GMT%+ld", utc_స్థానభ్రంశం_Mins / 60);
            ఒకవేళ (ret > 0 && ret < UPRV_LENGTHOF(gmt_స్థానభ్రంశం_Tz)) {
                తిరిగి uprv_strdup(gmt_స్థానభ్రంశం_Tz);
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
    WCHAR timezone_Sub_కీలకం_Name[WINDOWS_MAX_REG_కీలకం_నిడివి];
    WCHAR *windows_సమయం_Zone_Name = dynamicTZI.సమయం_Zone_కీలకం_Name;

    ఒకవేళ (dynamicTZI.సమయం_Zone_కీలకం_Name[0] == 0) {

// We can't use the registry APIs in the UWP version.
#ఒకవేళ U_PLATFORM_HAS_WINUWP_API == 1
        (శూన్య)timezone_Sub_కీలకం_Name; // suppress unused variable warnings.
        తిరిగి శూన్యసూచిక;
#లేకపోతే
        // Open the path to the time zones in the Windows registry.
        LONG ret;
        HKEY h_కీలకం_All_సమయం_Zones = శూన్యసూచిక;
        ret = Reg_Open_కీలకం_Ex_W(HKEY_LOCAL_MACHINE, WINDOWS_TIMEZONES_REG_కీలకం_మార్గం, 0, కీలకం_READ,
                            reinterpret_cast<PHKEY>(&h_కీలకం_All_సమయం_Zones));
        
        ఒకవేళ (ret != ERROR_SUCCESS) {
            // If we can't open the key, then we can't do much, so fail.
            తిరిగి శూన్యసూచిక;
        }

        // Read the number of subkeys under the time zone registry path.
        DWORD అంకె_సమయం_Zone_Sub_కీలకాలు;
        ret = Reg_Query_Info_కీలకం_W(h_కీలకం_All_సమయం_Zones, శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక, &అంకె_సమయం_Zone_Sub_కీలకాలు,
                               శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక);
        
        ఒకవేళ (ret != ERROR_SUCCESS) {
            Reg_Close_కీలకం(h_కీలకం_All_సమయం_Zones);
            తిరిగి శూన్యసూచిక;
        }

        // Examine each of the subkeys to try and find a match for the localized standard name ("Std").
        //
        // Note: The name of the time zone subkey itself is not localized, but the "Std" name is localized. This means
        // that we could fail to find a match if the RDP client and RDP server are using different languages, but unfortunately
        // there isn't much we can do about it.
        HKEY h_కీలకం_సమయం_Zone_Sub_కీలకం = శూన్యసూచిక;
        ULONG registry_విలువ_Type;
        WCHAR registryStandardName[WINDOWS_MAX_REG_కీలకం_నిడివి];

        కొరకు (DWORD i = 0; i < అంకె_సమయం_Zone_Sub_కీలకాలు; i++) {
            // Note: RegEnumKeyExW wants the size of the buffer in characters.
            DWORD size = UPRV_LENGTHOF(timezone_Sub_కీలకం_Name);
            ret = Reg_Enum_కీలకం_Ex_W(h_కీలకం_All_సమయం_Zones, i, timezone_Sub_కీలకం_Name, &size, శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక);

            ఒకవేళ (ret != ERROR_SUCCESS) {
                Reg_Close_కీలకం(h_కీలకం_All_సమయం_Zones);
                తిరిగి శూన్యసూచిక;
            }
            
            ret = Reg_Open_కీలకం_Ex_W(h_కీలకం_All_సమయం_Zones, timezone_Sub_కీలకం_Name, 0, కీలకం_READ,
                                reinterpret_cast<PHKEY>(&h_కీలకం_సమయం_Zone_Sub_కీలకం));
            
            ఒకవేళ (ret != ERROR_SUCCESS) {
                Reg_Close_కీలకం(h_కీలకం_All_సమయం_Zones);
                తిరిగి శూన్యసూచిక;
            }

            // Note: RegQueryValueExW wants the size of the buffer in bytes.
            size = పరిమాణం(registryStandardName);
            ret = Reg_Query_విలువ_Ex_W(h_కీలకం_సమయం_Zone_Sub_కీలకం, L"Std", శూన్యసూచిక, &registry_విలువ_Type,
                                   reinterpret_cast<LPBYTE>(registryStandardName), &size);
            
            ఒకవేళ (ret != ERROR_SUCCESS || registry_విలువ_Type != REG_SZ) {
                Reg_Close_కీలకం(h_కీలకం_సమయం_Zone_Sub_కీలకం);
                Reg_Close_కీలకం(h_కీలకం_All_సమయం_Zones);
                తిరిగి శూన్యసూచిక;
            }

            // Note: wcscmp does an ordinal (byte) comparison.
            ఒకవేళ (wcscmp(reinterpret_cast<WCHAR *>(registryStandardName), dynamicTZI.StandardName) == 0) {
                // Since we are comparing the *localized* time zone name, it's possible that some languages might use
                // the same string for more than one time zone. Thus we need to examine the TZI data in the registry to
                // compare the GMT offset (the bias), and the DST transition dates, to ensure it's the same time zone
                // as the currently reported one.
                REG_TZI_FORMAT registry_Tzi_విలువ;
                uprv_memset(&registry_Tzi_విలువ, 0, పరిమాణం(registry_Tzi_విలువ));

                // Note: RegQueryValueExW wants the size of the buffer in bytes.
                DWORD timezone_Tzi_విలువ_కొలత = పరిమాణం(registry_Tzi_విలువ);
                ret = Reg_Query_విలువ_Ex_W(h_కీలకం_సమయం_Zone_Sub_కీలకం, L"TZI", శూన్యసూచిక, &registry_విలువ_Type,
                                     reinterpret_cast<LPBYTE>(&registry_Tzi_విలువ), &timezone_Tzi_విలువ_కొలత);

                ఒకవేళ (ret == ERROR_SUCCESS) {
                    ఒకవేళ ((dynamicTZI.Bias == registry_Tzi_విలువ.Bias) &&
                        (memcmp((స్థిరరాశి శూన్య *)&dynamicTZI.StandardDate, (స్థిరరాశి శూన్య *)&registry_Tzi_విలువ.StandardDate, పరిమాణం(SYSTEMTIME)) == 0) &&
                        (memcmp((స్థిరరాశి శూన్య *)&dynamicTZI.DaylightDate, (స్థిరరాశి శూన్య *)&registry_Tzi_విలువ.DaylightDate, పరిమాణం(SYSTEMTIME)) == 0))
                    {
                        // We found a matching time zone.
                        windows_సమయం_Zone_Name = timezone_Sub_కీలకం_Name;
                        ఆపు;
                    }
                }
            }
            Reg_Close_కీలకం(h_కీలకం_సమయం_Zone_Sub_కీలకం);
            h_కీలకం_సమయం_Zone_Sub_కీలకం = శూన్యసూచిక;
        }

        ఒకవేళ (h_కీలకం_సమయం_Zone_Sub_కీలకం != శూన్యసూచిక) {
            Reg_Close_కీలకం(h_కీలకం_సమయం_Zone_Sub_కీలకం);
        }
        ఒకవేళ (h_కీలకం_All_సమయం_Zones != శూన్యసూచిక) {
            Reg_Close_కీలకం(h_కీలకం_All_సమయం_Zones);
        }
#అయితే_ముగింపు // U_PLATFORM_HAS_WINUWP_API
    }

    CharString winTZ;
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    winTZ.appendInvariantChars(UnicodeString(నిజం, windows_సమయం_Zone_Name, -1), స్థితి_2);

    // Map Windows Timezone name (non-localized) to ICU timezone ID (~ Olson timezone id).
    దొంతర_UResource_Bundle winTZBundle;
    ures_open_ప్రత్యక్ష_Fill_లోపల(winTZBundle.getAlias(), శూన్యసూచిక, "windowsZones", &స్థితి_2);
    ures_get_By_కీలకం(winTZBundle.getAlias(), "mapTimezones", winTZBundle.getAlias(), &స్థితి_2);
    ures_get_By_కీలకం(winTZBundle.getAlias(), winTZ.దత్తాంశం(), winTZBundle.getAlias(), &స్థితి_2);

    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    
    // Note: Since the ISO 3166 country/region codes are all invariant ASCII chars, we can
    // directly downcast from wchar_t to do the conversion.
    // We could call the A version of the GetGeoInfo API, but that would be slightly slower than calling the W API,
    // as the A version of the API will end up calling MultiByteToWideChar anyways internally.
    wchar_t region_సంకేతం_W[3] = {};
    అక్షరం region_సంకేతం[3] = {}; // 2 letter ISO 3166 country/region code made entirely of invariant chars.
    పూర్ణాంకం geoId = GetUserGeoID(GEOCLASS_NATION);
    పూర్ణాంకం region_సంకేతం_Len = GetGeoInfoW(geoId, GEO_ISO2, region_సంకేతం_W, UPRV_LENGTHOF(region_సంకేతం_W), 0);

    స్థిరరాశి char16_t *icuTZ16 = శూన్యసూచిక;
    int32_t tzListLen = 0;

    ఒకవేళ (region_సంకేతం_Len != 0) {
        కొరకు (పూర్ణాంకం i = 0; i < UPRV_LENGTHOF(region_సంకేతం_W); i++) {
            region_సంకేతం[i] = static_cast<అక్షరం>(region_సంకేతం_W[i]);
        }
        icuTZ16 = ures_get_String_By_కీలకం(winTZBundle.getAlias(), region_సంకేతం, &tzListLen, &స్థితి_2);
    }
    ఒకవేళ (region_సంకేతం_Len == 0 || U_FAILURE(స్థితి_2)) {
        // fallback to default "001" (world)
        స్థితి_2 = U_ZERO_ERROR;
        icuTZ16 = ures_get_String_By_కీలకం(winTZBundle.getAlias(), "001", &tzListLen, &స్థితి_2);
    }

    // Note: We want the first entry in the string returned by ures_getStringByKey.
    // However this string can be a space delimited list of timezones:
    //  Ex: "America/New_York America/Detroit America/Indiana/Petersburg ..."
    // We need to stop at the first space, so we pass tzLen (instead of tzListLen) to appendInvariantChars below.
    int32_t tzLen = 0;
    ఒకవేళ (tzListLen > 0) {
        అయితే (!(icuTZ16[tzLen] == u'\0' || icuTZ16[tzLen] == u' ')) {
            tzLen++;
        }
    }

    // Note: cloneData returns nullptr if the status is a failure, so this
    // will return nullptr if the above look-up fails.
    CharString icuTZStr;
    తిరిగి icuTZStr.appendInvariantChars(icuTZ16, tzLen, స్థితి_2).clone_దత్తాంశం(స్థితి_2);
}

U_NAMESPACE_చివర
#అయితే_ముగింపు /* U_PLATFORM_USES_ONLY_WIN32_API  */
