// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1997-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*
* File ULOC.CPP
*
* Modification History:
*
*   Date        Name        Description
*   04/01/97    aliu        Creation.
*   08/21/98    stephen     JDK 1.2 sync
*   12/08/98    rtg         New Locale implementation and C API
*   03/15/99    damiba      overhaul.
*   04/06/99    stephen     changed setDefault() to realloc and copy
*   06/14/99    stephen     Changed calls to ures_open for new params
*   07/21/99    stephen     Modified setDefault() to propagate to C++
*   05/14/04    alan        7 years later: refactored, cleaned up, fixed bugs,
*                           brought canonicalization code into line with spec
*****************************************************************************/

/*
   POSIX's locale format, from putil.c: [no spaces]

     ll [ _CC ] [ . MM ] [ @ VV]

     l = lang, C = ctry, M = charmap, V = variant
*/

#समावेश <algorithm>
#समावेश <optional>
#समावेश <string_view>

#समावेश "unicode/bytestream.h"
#समावेश "unicode/errorcode.h"
#समावेश "unicode/stringpiece.h"
#समावेश "unicode/utypes.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/uloc.h"

#समावेश "bytesinkutil.h"
#समावेश "putilimp.h"
#समावेश "ustr_imp.h"
#समावेश "ulocimp.h"
#समावेश "umutex.h"
#समावेश "cstring.h"
#समावेश "cmemory.h"
#समावेश "locmap.h"
#समावेश "uarrsort.h"
#समावेश "uenumimp.h"
#समावेश "uassert.h"
#समावेश "charstr.h"

U_NAMESPACE_USE

/* ### Declarations **************************************************/

/* Locale stuff from locid.cpp */
U_CFUNC व्योम locale_set_default(स्थिर अक्षर *id);
U_CFUNC स्थिर अक्षर *locale_get_default();

namespace {

/* ### Data tables **************************************************/

/**
 * Table of language codes, both 2- and 3-letter, with preference
 * given to 2-letter codes where possible.  Includes 3-letter codes
 * that lack a 2-letter equivalent.
 *
 * This list must be in sorted order.  This list is returned directly
 * to the user by some API.
 *
 * This list must be kept in sync with LANGUAGES_3, with corresponding
 * entries matched.
 *
 * This table should be terminated with a nullptr entry, followed by a
 * second list, and another nullptr entry.  The first list is visible to
 * user code when this array is returned by API.  The second list
 * contains codes we support, but do not expose through user API.
 *
 * Notes
 *
 * Tables updated per http://lcweb.loc.gov/standards/iso639-2/ to
 * include the revisions up to 2001/7/27 *CWB*
 *
 * The 3 character codes are the terminology codes like RFC 3066.  This
 * is compatible with prior ICU codes
 *
 * "in" "iw" "ji" "jw" & "sh" have been withdrawn but are still in the
 * table but now at the end of the table because 3 character codes are
 * duplicates.  This avoids bad searches going from 3 to 2 character
 * codes.
 *
 * The range qaa-qtz is reserved for local use
 */
/* Generated using org.unicode.cldr.icu.GenerateISO639LanguageTables */
/* ISO639 table version is 20150505 */
/* Subsequent hand addition of selected languages */
constexpr स्थिर अक्षर* LANGUAGES[] = {
    "aa", "ab", "ace", "ach", "ada", "ady", "ae", "aeb", "af", "afh", "agq", "ain", "ak", "akk",
    "akz", "ale", "aln", "alt", "am", "an", "ang", "anp", "ar", "arc", "arn", "aro", "arp",
    "arq", "ars", "arw", "ary", "arz", "as", "asa", "ase", "ast", "av", "avk", "awa", "ay",
    "az", "ba", "bal", "ban", "bar", "bas", "bax", "bbc", "bbj", "be", "bej", "bem", "bew",
    "bez", "bfd", "bfq", "bg", "bgc", "bgn", "bho", "bi", "bik", "bin", "bjn", "bkm", "bla",
    "blo", "bm", "bn", "bo", "bpy", "bqi", "br", "bra", "brh", "brx", "bs", "bss", "bua", "bug",
    "bum", "byn", "byv", "ca", "cad", "car", "cay", "cch", "ccp", "ce", "ceb", "cgg", "ch",
    "chb", "chg", "chk", "chm", "chn", "cho", "chp", "chr", "chy", "ckb", "co", "cop", "cps",
    "cr", "crh", "cs", "csb", "csw", "cu", "cv", "cy", "da", "dak", "dar", "dav", "day", "de",
    "del", "den", "dgr", "din", "dje", "doi", "dra", "dsb", "dtp", "dua", "dum", "dv", "dyo",
    "dyu", "dz", "dzg", "ebu", "ee", "efi", "egl", "egy", "eka", "el", "elx", "en", "enm", "eo",
    "es", "esu", "et", "eu", "ewo", "ext", "fa", "fan", "fat", "ff", "fi", "fil", "fit", "fiu",
    "fj", "fo", "fon", "fr", "frc", "frm", "fro", "frp", "frr", "frs", "fur", "fy", "ga", "gaa",
    "gag", "gan", "gay", "gba", "gbz", "gd", "gem", "gez", "gil", "gl", "glk", "gmh", "gn",
    "goh", "gom", "gon", "gor", "got", "grb", "grc", "gsw", "gu", "guc", "gur", "guz", "gv",
    "gwi", "ha", "hai", "hak", "haw", "he", "hi", "hif", "hil", "him", "hit", "hmn", "ho", "hr",
    "hsb", "hsn", "ht", "hu", "hup", "hy", "hz", "ia", "iba", "ibb", "id", "ie", "ig", "ii",
    "ijo", "ik", "ilo", "inc", "ine", "inh", "io", "ira", "iro", "is", "it", "iu", "izh", "ja",
    "jam", "jbo", "jgo", "jmc", "jpr", "jrb", "jut", "jv", "ka", "kaa", "kab", "kac", "kaj",
    "kam", "kar", "kaw", "kbd", "kbl", "kcg", "kde", "kea", "ken", "kfo", "kg", "kgp", "kha",
    "khi", "kho", "khq", "khw", "ki", "kiu", "kj", "kk", "kkj", "kl", "kln", "km", "kmb", "kn",
    "ko", "koi", "kok", "kos", "kpe", "kr", "krc", "kri", "krj", "krl", "kro", "kru", "ks",
    "ksb", "ksf", "ksh", "ku", "kum", "kut", "kv", "kw", "kxv", "ky", "la", "lad", "lag", "lah",
    "lam", "lb", "lez", "lfn", "lg", "li", "lij", "liv", "lkt", "lmo", "ln", "lo", "lol", "loz",
    "lrc", "lt", "ltg", "lu", "lua", "lui", "lun", "luo", "lus", "luy", "lv", "lzh", "lzz",
    "mad", "maf", "mag", "mai", "mak", "man", "map", "mas", "mde", "mdf", "mdh", "mdr", "men",
    "mer", "mfe", "mg", "mga", "mgh", "mgo", "mh", "mi", "mic", "min", "mis", "mk", "mkh", "ml",
    "mn", "mnc", "mni", "mno", "moh", "mos", "mr", "mrj", "ms", "mt", "mua", "mul", "mun",
    "mus", "mwl", "mwr", "mwv", "my", "mye", "myn", "myv", "mzn", "na", "nah", "nai", "nan",
    "nap", "naq", "nb", "nd", "nds", "ne", "new", "ng", "nia", "nic", "niu", "njo", "nl", "nmg",
    "nn", "nnh", "no", "nog", "non", "nov", "nqo", "nr", "nso", "nub", "nus", "nv", "nwc", "ny",
    "nym", "nyn", "nyo", "nzi", "oc", "oj", "om", "or", "os", "osa", "ota", "oto", "pa", "paa",
    "pag", "pal", "pam", "pap", "pau", "pcd", "pcm", "pdc", "pdt", "peo", "pfl", "phi", "phn",
    "pi", "pl", "pms", "pnt", "pon", "pra", "prg", "pro", "ps", "pt", "qu", "quc", "qug", "raj",
    "rap", "rar", "rgn", "rif", "rm", "rn", "ro", "roa", "rof", "rom", "rtm", "ru", "rue",
    "rug", "rup", "rw", "rwk", "sa", "sad", "sah", "sai", "sal", "sam", "saq", "sas", "sat",
    "saz", "sba", "sbp", "sc", "scn", "sco", "sd", "sdc", "sdh", "se", "see", "seh", "sei",
    "sel", "sem", "ses", "sg", "sga", "sgn", "sgs", "shi", "shn", "shu", "si", "sid", "sio",
    "sit", "sk", "sl", "sla", "sli", "sly", "sm", "sma", "smi", "smj", "smn", "sms", "sn",
    "snk", "so", "sog", "son", "sq", "sr", "srn", "srr", "ss", "ssa", "ssy", "st", "stq", "su",
    "suk", "sus", "sux", "sv", "sw", "swb", "syc", "syr", "szl", "ta", "tai", "tcy", "te",
    "tem", "teo", "ter", "tet", "tg", "th", "ti", "tig", "tiv", "tk", "tkl", "tkr", "tlh",
    "tli", "tly", "tmh", "tn", "to", "tog", "tok", "tpi", "tr", "tru", "trv", "ts", "tsd",
    "tsi", "tt", "ttt", "tum", "tup", "tut", "tvl", "tw", "twq", "ty", "tyv", "tzm", "udm",
    "ug", "uga", "uk", "umb", "und", "ur", "uz", "vai", "ve", "vec", "vep", "vi", "vls", "vmf",
    "vmw", "vo", "vot", "vro", "vun", "wa", "wae", "wak", "wal", "war", "was", "wbp", "wen",
    "wo", "wuu", "xal", "xh", "xmf", "xnr", "xog", "yao", "yap", "yav", "ybb", "yi", "yo",
    "ypk", "yrl", "yue", "za", "zap", "zbl", "zea", "zen", "zgh", "zh", "znd", "zu", "zun",
    "zxx", "zza",
nullptr,
    "in",  "iw",  "ji",  "jw",  "mo",  "sh",  "swc", "tl",  /* obsolete language codes */
nullptr
};

constexpr स्थिर अक्षर* DEPRECATED_LANGUAGES[]={
    "in", "iw", "ji", "jw", "mo", nullptr, nullptr
};
constexpr स्थिर अक्षर* REPLACEMENT_LANGUAGES[]={
    "id", "he", "yi", "jv", "ro", nullptr, nullptr
};

/**
 * Table of 3-letter language codes.
 *
 * This is a lookup table used to convert 3-letter language codes to
 * their 2-letter equivalent, where possible.  It must be kept in sync
 * with LANGUAGES.  For all valid i, LANGUAGES[i] must refer to the
 * same language as LANGUAGES_3[i].  The commented-out lines are
 * copied from LANGUAGES to make eyeballing this baby easier.
 *
 * Where a 3-letter language code has no 2-letter equivalent, the
 * 3-letter code occupies both LANGUAGES[i] and LANGUAGES_3[i].
 *
 * This table should be terminated with a nullptr entry, followed by a
 * second list, and another nullptr entry.  The two lists correspond to
 * the two lists in LANGUAGES.
 */
/* Generated using org.unicode.cldr.icu.GenerateISO639LanguageTables */
/* ISO639 table version is 20150505 */
/* Subsequent hand addition of selected languages */
constexpr स्थिर अक्षर* LANGUAGES_3[] = {
    "aar", "abk", "ace", "ach", "ada", "ady", "ave", "aeb", "afr", "afh", "agq", "ain", "aka",
    "akk", "akz", "ale", "aln", "alt", "amh", "arg", "ang", "anp", "ara", "arc", "arn", "aro",
    "arp", "arq", "ars", "arw", "ary", "arz", "asm", "asa", "ase", "ast", "ava", "avk", "awa",
    "aym", "aze", "bak", "bal", "ban", "bar", "bas", "bax", "bbc", "bbj", "bel", "bej", "bem",
    "bew", "bez", "bfd", "bfq", "bul", "bgc", "bgn", "bho", "bis", "bik", "bin", "bjn", "bkm",
    "bla", "blo", "bam", "ben", "bod", "bpy", "bqi", "bre", "bra", "brh", "brx", "bos", "bss",
    "bua", "bug", "bum", "byn", "byv", "cat", "cad", "car", "cay", "cch", "ccp", "che", "ceb",
    "cgg", "cha", "chb", "chg", "chk", "chm", "chn", "cho", "chp", "chr", "chy", "ckb", "cos",
    "cop", "cps", "cre", "crh", "ces", "csb", "csw", "chu", "chv", "cym", "dan", "dak", "dar",
    "dav", "day", "deu", "del", "den", "dgr", "din", "dje", "doi", "dra", "dsb", "dtp", "dua",
    "dum", "div", "dyo", "dyu", "dzo", "dzg", "ebu", "ewe", "efi", "egl", "egy", "eka", "ell",
    "elx", "eng", "enm", "epo", "spa", "esu", "est", "eus", "ewo", "ext", "fas", "fan", "fat",
    "ful", "fin", "fil", "fit", "fiu", "fij", "fao", "fon", "fra", "frc", "frm", "fro", "frp",
    "frr", "frs", "fur", "fry", "gle", "gaa", "gag", "gan", "gay", "gba", "gbz", "gla", "gem",
    "gez", "gil", "glg", "glk", "gmh", "grn", "goh", "gom", "gon", "gor", "got", "grb", "grc",
    "gsw", "guj", "guc", "gur", "guz", "glv", "gwi", "hau", "hai", "hak", "haw", "heb", "hin",
    "hif", "hil", "him", "hit", "hmn", "hmo", "hrv", "hsb", "hsn", "hat", "hun", "hup", "hye",
    "her", "ina", "iba", "ibb", "ind", "ile", "ibo", "iii", "ijo", "ipk", "ilo", "inc", "ine",
    "inh", "ido", "ira", "iro", "isl", "ita", "iku", "izh", "jpn", "jam", "jbo", "jgo", "jmc",
    "jpr", "jrb", "jut", "jav", "kat", "kaa", "kab", "kac", "kaj", "kam", "kar", "kaw", "kbd",
    "kbl", "kcg", "kde", "kea", "ken", "kfo", "kon", "kgp", "kha", "khi", "kho", "khq", "khw",
    "kik", "kiu", "kua", "kaz", "kkj", "kal", "kln", "khm", "kmb", "kan", "kor", "koi", "kok",
    "kos", "kpe", "kau", "krc", "kri", "krj", "krl", "kro", "kru", "kas", "ksb", "ksf", "ksh",
    "kur", "kum", "kut", "kom", "cor", "kxv", "kir", "lat", "lad", "lag", "lah", "lam", "ltz",
    "lez", "lfn", "lug", "lim", "lij", "liv", "lkt", "lmo", "lin", "lao", "lol", "loz", "lrc",
    "lit", "ltg", "lub", "lua", "lui", "lun", "luo", "lus", "luy", "lav", "lzh", "lzz", "mad",
    "maf", "mag", "mai", "mak", "man", "map", "mas", "mde", "mdf", "mdh", "mdr", "men", "mer",
    "mfe", "mlg", "mga", "mgh", "mgo", "mah", "mri", "mic", "min", "mis", "mkd", "mkh", "mal",
    "mon", "mnc", "mni", "mno", "moh", "mos", "mar", "mrj", "msa", "mlt", "mua", "mul", "mun",
    "mus", "mwl", "mwr", "mwv", "mya", "mye", "myn", "myv", "mzn", "nau", "nah", "nai", "nan",
    "nap", "naq", "nob", "nde", "nds", "nep", "new", "ndo", "nia", "nic", "niu", "njo", "nld",
    "nmg", "nno", "nnh", "nor", "nog", "non", "nov", "nqo", "nbl", "nso", "nub", "nus", "nav",
    "nwc", "nya", "nym", "nyn", "nyo", "nzi", "oci", "oji", "orm", "ori", "oss", "osa", "ota",
    "oto", "pan", "paa", "pag", "pal", "pam", "pap", "pau", "pcd", "pcm", "pdc", "pdt", "peo",
    "pfl", "phi", "phn", "pli", "pol", "pms", "pnt", "pon", "pra", "prg", "pro", "pus", "por",
    "que", "quc", "qug", "raj", "rap", "rar", "rgn", "rif", "roh", "run", "ron", "roa", "rof",
    "rom", "rtm", "rus", "rue", "rug", "rup", "kin", "rwk", "san", "sad", "sah", "sai", "sal",
    "sam", "saq", "sas", "sat", "saz", "sba", "sbp", "srd", "scn", "sco", "snd", "sdc", "sdh",
    "sme", "see", "seh", "sei", "sel", "sem", "ses", "sag", "sga", "sgn", "sgs", "shi", "shn",
    "shu", "sin", "sid", "sio", "sit", "slk", "slv", "sla", "sli", "sly", "smo", "sma", "smi",
    "smj", "smn", "sms", "sna", "snk", "som", "sog", "son", "sqi", "srp", "srn", "srr", "ssw",
    "ssa", "ssy", "sot", "stq", "sun", "suk", "sus", "sux", "swe", "swa", "swb", "syc", "syr",
    "szl", "tam", "tai", "tcy", "tel", "tem", "teo", "ter", "tet", "tgk", "tha", "tir", "tig",
    "tiv", "tuk", "tkl", "tkr", "tlh", "tli", "tly", "tmh", "tsn", "ton", "tog", "tok", "tpi",
    "tur", "tru", "trv", "tso", "tsd", "tsi", "tat", "ttt", "tum", "tup", "tut", "tvl", "twi",
    "twq", "tah", "tyv", "tzm", "udm", "uig", "uga", "ukr", "umb", "und", "urd", "uzb", "vai",
    "ven", "vec", "vep", "vie", "vls", "vmf", "vmw", "vol", "vot", "vro", "vun", "wln", "wae",
    "wak", "wal", "war", "was", "wbp", "wen", "wol", "wuu", "xal", "xho", "xmf", "xnr", "xog",
    "yao", "yap", "yav", "ybb", "yid", "yor", "ypk", "yrl", "yue", "zha", "zap", "zbl", "zea",
    "zen", "zgh", "zho", "znd", "zul", "zun", "zxx", "zza",
nullptr,
/*  "in",  "iw",  "ji",  "jw",  "mo",  "sh",  "swc", "tl",  */
    "ind", "heb", "yid", "jaw", "mol", "srp", "swc", "tgl",
nullptr
};

/**
 * Table of 2-letter country codes.
 *
 * This list must be in sorted order.  This list is returned directly
 * to the user by some API.
 *
 * This list must be kept in sync with COUNTRIES_3, with corresponding
 * entries matched.
 *
 * This table should be terminated with a nullptr entry, followed by a
 * second list, and another nullptr entry.  The first list is visible to
 * user code when this array is returned by API.  The second list
 * contains codes we support, but do not expose through user API.
 *
 * Notes:
 *
 * ZR(ZAR) is now CD(COD) and FX(FXX) is PS(PSE) as per
 * http://www.evertype.com/standards/iso3166/iso3166-1-en.html added
 * new codes keeping the old ones for compatibility updated to include
 * 1999/12/03 revisions *CWB*
 *
 * RO(ROM) is now RO(ROU) according to
 * http://www.iso.org/iso/en/prods-services/iso3166ma/03updates-on-iso-3166/nlv3e-rou.html
 */
constexpr स्थिर अक्षर* COUNTRIES[] = {
    "AD",  "AE",  "AF",  "AG",  "AI",  "AL",  "AM",
    "AO",  "AQ",  "AR",  "AS",  "AT",  "AU",  "AW",  "AX",  "AZ",
    "BA",  "BB",  "BD",  "BE",  "BF",  "BG",  "BH",  "BI",
    "BJ",  "BL",  "BM",  "BN",  "BO",  "BQ",  "BR",  "BS",  "BT",  "BV",
    "BW",  "BY",  "BZ",  "CA",  "CC",  "CD",  "CF",  "CG",
    "CH",  "CI",  "CK",  "CL",  "CM",  "CN",  "CO",  "CQ",  "CR",
    "CU",  "CV",  "CW",  "CX",  "CY",  "CZ",  "DE",  "DG",  "DJ",  "DK",
    "DM",  "DO",  "DZ",  "EA",  "EC",  "EE",  "EG",  "EH",  "ER",
    "ES",  "ET",  "FI",  "FJ",  "FK",  "FM",  "FO",  "FR",
    "GA",  "GB",  "GD",  "GE",  "GF",  "GG",  "GH",  "GI",  "GL",
    "GM",  "GN",  "GP",  "GQ",  "GR",  "GS",  "GT",  "GU",
    "GW",  "GY",  "HK",  "HM",  "HN",  "HR",  "HT",  "HU",
    "IC",  "ID",  "IE",  "IL",  "IM",  "IN",  "IO",  "IQ",  "IR",  "IS",
    "IT",  "JE",  "JM",  "JO",  "JP",  "KE",  "KG",  "KH",  "KI",
    "KM",  "KN",  "KP",  "KR",  "KW",  "KY",  "KZ",  "LA",
    "LB",  "LC",  "LI",  "LK",  "LR",  "LS",  "LT",  "LU",
    "LV",  "LY",  "MA",  "MC",  "MD",  "ME",  "MF",  "MG",  "MH",  "MK",
    "ML",  "MM",  "MN",  "MO",  "MP",  "MQ",  "MR",  "MS",
    "MT",  "MU",  "MV",  "MW",  "MX",  "MY",  "MZ",  "NA",
    "NC",  "NE",  "NF",  "NG",  "NI",  "NL",  "NO",  "NP",
    "NR",  "NU",  "NZ",  "OM",  "PA",  "PE",  "PF",  "PG",
    "PH",  "PK",  "PL",  "PM",  "PN",  "PR",  "PS",  "PT",
    "PW",  "PY",  "QA",  "RE",  "RO",  "RS",  "RU",  "RW",  "SA",
    "SB",  "SC",  "SD",  "SE",  "SG",  "SH",  "SI",  "SJ",
    "SK",  "SL",  "SM",  "SN",  "SO",  "SR",  "SS",  "ST",  "SV",
    "SX",  "SY",  "SZ",  "TC",  "TD",  "TF",  "TG",  "TH",  "TJ",
    "TK",  "TL",  "TM",  "TN",  "TO",  "TR",  "TT",  "TV",
    "TW",  "TZ",  "UA",  "UG",  "UM",  "US",  "UY",  "UZ",
    "VA",  "VC",  "VE",  "VG",  "VI",  "VN",  "VU",  "WF",
    "WS",  "XK",  "YE",  "YT",  "ZA",  "ZM",  "ZW",
nullptr,
    "AN",  "BU", "CS", "FX", "RO", "SU", "TP", "YD", "YU", "ZR",   /* obsolete country codes */
nullptr
};

constexpr स्थिर अक्षर* DEPRECATED_COUNTRIES[] = {
    "AN", "BU", "CS", "DD", "DY", "FX", "HV", "NH", "RH", "SU", "TP", "UK", "VD", "YD", "YU", "ZR", nullptr, nullptr /* deprecated country list */
};
constexpr स्थिर अक्षर* REPLACEMENT_COUNTRIES[] = {
/*  "AN", "BU", "CS", "DD", "DY", "FX", "HV", "NH", "RH", "SU", "TP", "UK", "VD", "YD", "YU", "ZR" */
    "CW", "MM", "RS", "DE", "BJ", "FR", "BF", "VU", "ZW", "RU", "TL", "GB", "VN", "YE", "RS", "CD", nullptr, nullptr  /* replacement country codes */
};

/**
 * Table of 3-letter country codes.
 *
 * This is a lookup table used to convert 3-letter country codes to
 * their 2-letter equivalent.  It must be kept in sync with COUNTRIES.
 * For all valid i, COUNTRIES[i] must refer to the same country as
 * COUNTRIES_3[i].  The commented-out lines are copied from COUNTRIES
 * to make eyeballing this baby easier.
 *
 * This table should be terminated with a nullptr entry, followed by a
 * second list, and another nullptr entry.  The two lists correspond to
 * the two lists in COUNTRIES.
 */
constexpr स्थिर अक्षर* COUNTRIES_3[] = {
/*  "AD",  "AE",  "AF",  "AG",  "AI",  "AL",  "AM",      */
    "AND", "ARE", "AFG", "ATG", "AIA", "ALB", "ARM",
/*  "AO",  "AQ",  "AR",  "AS",  "AT",  "AU",  "AW",  "AX",  "AZ",     */
    "AGO", "ATA", "ARG", "ASM", "AUT", "AUS", "ABW", "ALA", "AZE",
/*  "BA",  "BB",  "BD",  "BE",  "BF",  "BG",  "BH",  "BI",     */
    "BIH", "BRB", "BGD", "BEL", "BFA", "BGR", "BHR", "BDI",
/*  "BJ",  "BL",  "BM",  "BN",  "BO",  "BQ",  "BR",  "BS",  "BT",  "BV",     */
    "BEN", "BLM", "BMU", "BRN", "BOL", "BES", "BRA", "BHS", "BTN", "BVT",
/*  "BW",  "BY",  "BZ",  "CA",  "CC",  "CD",  "CF",  "CG",     */
    "BWA", "BLR", "BLZ", "CAN", "CCK", "COD", "CAF", "COG",
/*  "CH",  "CI",  "CK",  "CL",  "CM",  "CN",  "CO",  "CQ",  "CR",     */
    "CHE", "CIV", "COK", "CHL", "CMR", "CHN", "COL", "CRQ", "CRI",
/*  "CU",  "CV",  "CW",  "CX",  "CY",  "CZ",  "DE",  "DG",  "DJ",  "DK",     */
    "CUB", "CPV", "CUW", "CXR", "CYP", "CZE", "DEU", "DGA", "DJI", "DNK",
/*  "DM",  "DO",  "DZ",  "EA",  "EC",  "EE",  "EG",  "EH",  "ER",     */
    "DMA", "DOM", "DZA", "XEA", "ECU", "EST", "EGY", "ESH", "ERI",
/*  "ES",  "ET",  "FI",  "FJ",  "FK",  "FM",  "FO",  "FR",     */
    "ESP", "ETH", "FIN", "FJI", "FLK", "FSM", "FRO", "FRA",
/*  "GA",  "GB",  "GD",  "GE",  "GF",  "GG",  "GH",  "GI",  "GL",     */
    "GAB", "GBR", "GRD", "GEO", "GUF", "GGY", "GHA", "GIB", "GRL",
/*  "GM",  "GN",  "GP",  "GQ",  "GR",  "GS",  "GT",  "GU",     */
    "GMB", "GIN", "GLP", "GNQ", "GRC", "SGS", "GTM", "GUM",
/*  "GW",  "GY",  "HK",  "HM",  "HN",  "HR",  "HT",  "HU",     */
    "GNB", "GUY", "HKG", "HMD", "HND", "HRV", "HTI", "HUN",
/*  "IC",  "ID",  "IE",  "IL",  "IM",  "IN",  "IO",  "IQ",  "IR",  "IS" */
    "XIC", "IDN", "IRL", "ISR", "IMN", "IND", "IOT", "IRQ", "IRN", "ISL",
/*  "IT",  "JE",  "JM",  "JO",  "JP",  "KE",  "KG",  "KH",  "KI",     */
    "ITA", "JEY", "JAM", "JOR", "JPN", "KEN", "KGZ", "KHM", "KIR",
/*  "KM",  "KN",  "KP",  "KR",  "KW",  "KY",  "KZ",  "LA",     */
    "COM", "KNA", "PRK", "KOR", "KWT", "CYM", "KAZ", "LAO",
/*  "LB",  "LC",  "LI",  "LK",  "LR",  "LS",  "LT",  "LU",     */
    "LBN", "LCA", "LIE", "LKA", "LBR", "LSO", "LTU", "LUX",
/*  "LV",  "LY",  "MA",  "MC",  "MD",  "ME",  "MF",  "MG",  "MH",  "MK",     */
    "LVA", "LBY", "MAR", "MCO", "MDA", "MNE", "MAF", "MDG", "MHL", "MKD",
/*  "ML",  "MM",  "MN",  "MO",  "MP",  "MQ",  "MR",  "MS",     */
    "MLI", "MMR", "MNG", "MAC", "MNP", "MTQ", "MRT", "MSR",
/*  "MT",  "MU",  "MV",  "MW",  "MX",  "MY",  "MZ",  "NA",     */
    "MLT", "MUS", "MDV", "MWI", "MEX", "MYS", "MOZ", "NAM",
/*  "NC",  "NE",  "NF",  "NG",  "NI",  "NL",  "NO",  "NP",     */
    "NCL", "NER", "NFK", "NGA", "NIC", "NLD", "NOR", "NPL",
/*  "NR",  "NU",  "NZ",  "OM",  "PA",  "PE",  "PF",  "PG",     */
    "NRU", "NIU", "NZL", "OMN", "PAN", "PER", "PYF", "PNG",
/*  "PH",  "PK",  "PL",  "PM",  "PN",  "PR",  "PS",  "PT",     */
    "PHL", "PAK", "POL", "SPM", "PCN", "PRI", "PSE", "PRT",
/*  "PW",  "PY",  "QA",  "RE",  "RO",  "RS",  "RU",  "RW",  "SA",     */
    "PLW", "PRY", "QAT", "REU", "ROU", "SRB", "RUS", "RWA", "SAU",
/*  "SB",  "SC",  "SD",  "SE",  "SG",  "SH",  "SI",  "SJ",     */
    "SLB", "SYC", "SDN", "SWE", "SGP", "SHN", "SVN", "SJM",
/*  "SK",  "SL",  "SM",  "SN",  "SO",  "SR",  "SS",  "ST",  "SV",     */
    "SVK", "SLE", "SMR", "SEN", "SOM", "SUR", "SSD", "STP", "SLV",
/*  "SX",  "SY",  "SZ",  "TC",  "TD",  "TF",  "TG",  "TH",  "TJ",     */
    "SXM", "SYR", "SWZ", "TCA", "TCD", "ATF", "TGO", "THA", "TJK",
/*  "TK",  "TL",  "TM",  "TN",  "TO",  "TR",  "TT",  "TV",     */
    "TKL", "TLS", "TKM", "TUN", "TON", "TUR", "TTO", "TUV",
/*  "TW",  "TZ",  "UA",  "UG",  "UM",  "US",  "UY",  "UZ",     */
    "TWN", "TZA", "UKR", "UGA", "UMI", "USA", "URY", "UZB",
/*  "VA",  "VC",  "VE",  "VG",  "VI",  "VN",  "VU",  "WF",     */
    "VAT", "VCT", "VEN", "VGB", "VIR", "VNM", "VUT", "WLF",
/*  "WS",  "XK",  "YE",  "YT",  "ZA",  "ZM",  "ZW",          */
    "WSM", "XKK", "YEM", "MYT", "ZAF", "ZMB", "ZWE",
nullptr,
/*  "AN",  "BU",  "CS",  "FX",  "RO", "SU",  "TP",  "YD",  "YU",  "ZR" */
    "ANT", "BUR", "SCG", "FXX", "ROM", "SUN", "TMP", "YMD", "YUG", "ZAR",
nullptr
};

प्रकार काष्ठा CanonicalizationMap {
    स्थिर अक्षर *id;          /* input ID */
    स्थिर अक्षर *canonicalID; /* canonicalized output ID */
} CanonicalizationMap;

/**
 * A map to canonicalize locale IDs.  This handles a variety of
 * different semantic kinds of transformations.
 */
constexpr CanonicalizationMap CANONICALIZE_MAP[] = {
    { "art__LOJBAN",    "jbo" }, /* registered name */
    { "hy__AREVELA",    "hy" }, /* Registered IANA variant */
    { "hy__AREVMDA",    "hyw" }, /* Registered IANA variant */
    { "zh__GUOYU",      "zh" }, /* registered name */
    { "zh__HAKKA",      "hak" }, /* registered name */
    { "zh__XIANG",      "hsn" }, /* registered name */
    // subtags with 3 chars won't be treated as variants.
    { "zh_GAN",         "gan" }, /* registered name */
    { "zh_MIN_NAN",     "nan" }, /* registered name */
    { "zh_WUU",         "wuu" }, /* registered name */
    { "zh_YUE",         "yue" }, /* registered name */
};

/* ### BCP47 Conversion *******************************************/
/* Gets the size of the shortest subtag in the given localeID. */
int32_t get_Shortest_Subtag_लंबाई(std::string_view localeID) {
    int32_t localeIDLength = static_cast<int32_t>(localeID.length());
    int32_t length = localeIDLength;
    int32_t tmp_लंबाई = 0;
    int32_t i;
    bool reset = true;

    क्रम (i = 0; i < localeIDLength; i++) {
        अगर (localeID[i] != '_' && localeID[i] != '-') {
            अगर (reset) {
                tmp_लंबाई = 0;
                reset = false;
            }
            tmp_लंबाई++;
        } अन्यथा {
            अगर (tmp_लंबाई != 0 && tmp_लंबाई < length) {
                length = tmp_लंबाई;
            }
            reset = true;
        }
    }

    वापस length;
}
/* Test if the locale id has BCP47 u extension and does not have '@' */
अंतरभूत bool _hasBCP47Extension(std::string_view id) {
    वापस id.find('@') == std::string_view::npos && get_Shortest_Subtag_लंबाई(id) == 1;
}

/* ### Keywords **************************************************/
अंतरभूत bool UPRV_ISDIGIT(अक्षर c) { वापस c >= '0' && c <= '9'; }
अंतरभूत bool UPRV_ISALPHANUM(अक्षर c) { वापस uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }
/* Punctuation/symbols allowed in legacy key values */
अंतरभूत bool UPRV_OK_मूल्य_PUNCTUATION(अक्षर c) { वापस c == '_' || c == '-' || c == '+' || c == '/'; }

}  // namespace

#घोषणा ULOC_KEYWORD_बफर_LEN 25
#घोषणा ULOC_MAX_NO_KEYWORDS 25

U_CAPI स्थिर अक्षर* U_EXPORT2
locale_getKeywordsStart(std::string_view localeID U_LIFETIME_BOUND) {
    अगर (माप_प्रकार pos = localeID.find('@'); pos != std::string_view::npos) {
        वापस localeID.तथ्य() + pos;
    }
#अगर (U_CHARSET_FAMILY == U_EBCDIC_FAMILY)
    अन्यथा {
        /* We do this because the @ sign is variant, and the @ sign used on one
        EBCDIC machine won't be compiled the same way on other EBCDIC based
        machines. */
        अटल स्थिर uint8_t ebcdicSigns[] = { 0x7C, 0x44, 0x66, 0x80, 0xAC, 0xAE, 0xAF, 0xB5, 0xEC, 0xEF, 0x00 };
        स्थिर uint8_t *charToFind = ebcdicSigns;
        जबतक(*charToFind) {
            अगर (माप_प्रकार pos = localeID.find(*charToFind); pos != std::string_view::npos) {
                वापस localeID.तथ्य() + pos;
            }
            charToFind++;
        }
    }
#पूर्ण_अगर
    वापस nullptr;
}

namespace {

/**
 * @param keywordName incoming name to be canonicalized
 * @param status return status (keyword too long)
 * @return the keyword name
 */
CharString locale_canonKeywordName(std::string_view keywordName, UErrorCode& status)
{
  अगर (U_FAILURE(status)) { वापस {}; }
  CharString परिणाम;

  क्रम (अक्षर c : keywordName) {
    अगर (!UPRV_ISALPHANUM(c)) {
      status = U_ILLEGAL_ARGUMENT_ERROR; /* malformed keyword name */
      वापस {};
    }
    परिणाम.append(uprv_tolower(c), status);
  }
  अगर (परिणाम.isEmpty()) {
    status = U_ILLEGAL_ARGUMENT_ERROR; /* empty keyword name */
    वापस {};
  }

  वापस परिणाम;
}

प्रकार काष्ठा {
    अक्षर keyword[ULOC_KEYWORD_बफर_LEN];
    int32_t keywordLen;
    स्थिर अक्षर *मूल्य_Start;
    int32_t मूल्य_Len;
} KeywordStruct;

int32_t U_CALLCONV
compareKeywordStructs(स्थिर व्योम * /*context*/, स्थिर व्योम *बाँए, स्थिर व्योम *दाँए) {
    स्थिर अक्षर* बायाँ_String = static_cast<स्थिर KeywordStruct*>(बाँए)->keyword;
    स्थिर अक्षर* दायाँ_String = static_cast<स्थिर KeywordStruct*>(दाँए)->keyword;
    वापस uprv_strcmp(बायाँ_String, दायाँ_String);
}

}  // namespace

U_EXPORT CharString
ulocimp_getKeywords(std::string_view localeID,
                    अक्षर पूर्व,
                    bool मूल्यावली_Too,
                    UErrorCode& status)
{
    वापस ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getKeywords(localeID,
                                पूर्व,
                                sink,
                                मूल्यावली_Too,
                                status);
        },
        status);
}

U_EXPORT व्योम
ulocimp_getKeywords(std::string_view localeID,
                    अक्षर पूर्व,
                    ByteSink& sink,
                    bool मूल्यावली_Too,
                    UErrorCode& status)
{
    अगर (U_FAILURE(status)) { वापस; }

    KeywordStruct keywordList[ULOC_MAX_NO_KEYWORDS];

    int32_t maxKeywords = ULOC_MAX_NO_KEYWORDS;
    int32_t अंक_Keywords = 0;
    माप_प्रकार equalSign = std::string_view::npos;
    माप_प्रकार semicolon = std::string_view::npos;
    int32_t i = 0, j, n;

    अगर(पूर्व == '@') { /* start of keyword definition */
        /* we will grab pairs, trim spaces, lowercase keywords, sort and return */
        करो {
            bool duplicate = false;
            /* skip leading spaces */
            जबतक (!localeID.empty() && localeID.front() == ' ') {
                localeID.remove_prefix(1);
            }
            अगर (localeID.empty()) { /* handle trailing "; " */
                अवरोध;
            }
            अगर(अंक_Keywords == maxKeywords) {
                status = U_INTERNAL_PROGRAM_ERROR;
                वापस;
            }
            equalSign = localeID.find('=');
            semicolon = localeID.find(';');
            /* lack of '=' [foo@currency] is illegal */
            /* ';' before '=' [foo@currency;collation=pinyin] is illegal */
            अगर (equalSign == std::string_view::npos ||
                (semicolon != std::string_view::npos && semicolon < equalSign)) {
                status = U_INVALID_FORMAT_ERROR;
                वापस;
            }
            /* zero-length keyword is an error. */
            अगर (equalSign == 0) {
                status = U_INVALID_FORMAT_ERROR;
                वापस;
            }
            /* need to normalize both keyword and keyword name */
            अगर (equalSign >= ULOC_KEYWORD_बफर_LEN) {
                /* keyword name too long for internal buffer */
                status = U_INTERNAL_PROGRAM_ERROR;
                वापस;
            }
            क्रम (i = 0, n = 0; static_cast<माप_प्रकार>(i) < equalSign; ++i) {
                अगर (localeID[i] != ' ') {
                    keywordList[अंक_Keywords].keyword[n++] = uprv_tolower(localeID[i]);
                }
            }

            keywordList[अंक_Keywords].keyword[n] = 0;
            keywordList[अंक_Keywords].keywordLen = n;
            /* now grab the value part. First we skip the '=' */
            equalSign++;
            /* then we leading spaces */
            जबतक (equalSign < localeID.length() && localeID[equalSign] == ' ') {
                equalSign++;
            }

            /* Premature end or zero-length value */
            अगर (equalSign == localeID.length() || equalSign == semicolon) {
                status = U_INVALID_FORMAT_ERROR;
                वापस;
            }

            keywordList[अंक_Keywords].मूल्य_Start = localeID.तथ्य() + equalSign;

            std::string_view value = localeID;
            अगर (semicolon != std::string_view::npos) {
                value.remove_suffix(value.length() - semicolon);
                localeID.remove_prefix(semicolon + 1);
            } अन्यथा {
                localeID = {};
            }
            value.remove_prefix(equalSign);
            अगर (माप_प्रकार last = value.find_last_not_of(' '); last != std::string_view::npos) {
                value.remove_suffix(value.length() - last - 1);
            }
            keywordList[अंक_Keywords].मूल्य_Len = static_cast<int32_t>(value.length());

            /* If this is a duplicate keyword, then ignore it */
            क्रम (j=0; j<अंक_Keywords; ++j) {
                अगर (uprv_strcmp(keywordList[j].keyword, keywordList[अंक_Keywords].keyword) == 0) {
                    duplicate = true;
                    अवरोध;
                }
            }
            अगर (!duplicate) {
                ++अंक_Keywords;
            }
        } जबतक (!localeID.empty());

        /* now we have a list of keywords */
        /* we need to sort it */
        uprv_sortArray(keywordList, अंक_Keywords, माप(KeywordStruct), compareKeywordStructs, nullptr, false, &status);

        /* Now construct the keyword part */
        क्रम(i = 0; i < अंक_Keywords; i++) {
            sink.Append(keywordList[i].keyword, keywordList[i].keywordLen);
            अगर(मूल्यावली_Too) {
                sink.Append("=", 1);
                sink.Append(keywordList[i].मूल्य_Start, keywordList[i].मूल्य_Len);
                अगर(i < अंक_Keywords - 1) {
                    sink.Append(";", 1);
                }
            } अन्यथा {
                sink.Append("\0", 1);
            }
        }
    }
}

U_CAPI int32_t U_EXPORT2
uloc_get_Keyword_मूल्य(स्थिर अक्षर* localeID,
                     स्थिर अक्षर* keywordName,
                     अक्षर* बफर, int32_t बफर_Capacity,
                     UErrorCode* status)
{
    अगर (U_FAILURE(*status)) { वापस 0; }
    अगर (keywordName == nullptr || *keywordName == '\0') {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }
    वापस ByteSinkUtil::viaByteSinkToTerminatedChars(
        बफर, बफर_Capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_get_Keyword_मूल्य(localeID, keywordName, sink, status);
        },
        *status);
}

U_EXPORT CharString
ulocimp_get_Keyword_मूल्य(स्थिर अक्षर* localeID,
                        std::string_view keywordName,
                        UErrorCode& status)
{
    वापस ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_get_Keyword_मूल्य(localeID, keywordName, sink, status);
        },
        status);
}

U_EXPORT व्योम
ulocimp_get_Keyword_मूल्य(स्थिर अक्षर* localeID,
                        std::string_view keywordName,
                        icu::ByteSink& sink,
                        UErrorCode& status)
{
    अगर (U_FAILURE(status)) { वापस; }

    अगर (localeID == nullptr || keywordName.empty()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    स्थिर अक्षर* start_खोज_Here = nullptr;
    स्थिर अक्षर* अग्र_Separator = nullptr;

    CharString अस्थायी_बफर;
    स्थिर अक्षर* tmpLocaleID;

    CharString canonKeywordName = locale_canonKeywordName(keywordName, status);
    अगर (U_FAILURE(status)) {
      वापस;
    }

    अगर (localeID != nullptr && _hasBCP47Extension(localeID)) {
        अस्थायी_बफर = ulocimp_forLanguageTag(localeID, -1, nullptr, status);
        tmpLocaleID = U_SUCCESS(status) && !अस्थायी_बफर.isEmpty() ? अस्थायी_बफर.तथ्य() : localeID;
    } अन्यथा {
        tmpLocaleID=localeID;
    }

    start_खोज_Here = locale_getKeywordsStart(tmpLocaleID);
    अगर(start_खोज_Here == nullptr) {
        /* no keywords, return at once */
        वापस;
    }

    /* find the first keyword */
    जबतक(start_खोज_Here) {
        स्थिर अक्षर* कुंजी_मूल्य_पुच्छ;

        start_खोज_Here++; /* skip @ or ; */
        अग्र_Separator = uprv_strchr(start_खोज_Here, '=');
        अगर(!अग्र_Separator) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* key must have =value */
            वापस;
        }
        /* strip leading & trailing spaces (TC decided to tolerate these) */
        जबतक(*start_खोज_Here == ' ') {
            start_खोज_Here++;
        }
        कुंजी_मूल्य_पुच्छ = अग्र_Separator;
        जबतक (कुंजी_मूल्य_पुच्छ > start_खोज_Here && *(कुंजी_मूल्य_पुच्छ-1) == ' ') {
            कुंजी_मूल्य_पुच्छ--;
        }
        /* now keyValueTail points to first char after the keyName */
        /* copy & normalize keyName from locale */
        अगर (start_खोज_Here == कुंजी_मूल्य_पुच्छ) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* empty keyword name in passed-in locale */
            वापस;
        }
        CharString localeKeywordName;
        जबतक (start_खोज_Here < कुंजी_मूल्य_पुच्छ) {
          अगर (!UPRV_ISALPHANUM(*start_खोज_Here)) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* malformed keyword name */
            वापस;
          }
          localeKeywordName.append(uprv_tolower(*start_खोज_Here++), status);
        }
        अगर (U_FAILURE(status)) {
            वापस;
        }

        start_खोज_Here = uprv_strchr(अग्र_Separator, ';');

        अगर (canonKeywordName == localeKeywordName) {
             /* current entry matches the keyword. */
           अग्र_Separator++; /* skip '=' */
            /* First strip leading & trailing spaces (TC decided to tolerate these) */
            जबतक(*अग्र_Separator == ' ') {
              अग्र_Separator++;
            }
            कुंजी_मूल्य_पुच्छ = (start_खोज_Here)? start_खोज_Here: अग्र_Separator + uprv_strlen(अग्र_Separator);
            जबतक(कुंजी_मूल्य_पुच्छ > अग्र_Separator && *(कुंजी_मूल्य_पुच्छ-1) == ' ') {
              कुंजी_मूल्य_पुच्छ--;
            }
            /* Now copy the value, but check well-formedness */
            अगर (अग्र_Separator == कुंजी_मूल्य_पुच्छ) {
              status = U_ILLEGAL_ARGUMENT_ERROR; /* empty key value name in passed-in locale */
              वापस;
            }
            जबतक (अग्र_Separator < कुंजी_मूल्य_पुच्छ) {
              अगर (!UPRV_ISALPHANUM(*अग्र_Separator) && !UPRV_OK_मूल्य_PUNCTUATION(*अग्र_Separator)) {
                status = U_ILLEGAL_ARGUMENT_ERROR; /* malformed key value */
                वापस;
              }
              /* Should we lowercase value to return here? Tests expect as-is. */
              sink.Append(अग्र_Separator++, 1);
            }
            वापस;
        }
    }
}

U_CAPI int32_t U_EXPORT2
uloc_set_Keyword_मूल्य(स्थिर अक्षर* keywordName,
                     स्थिर अक्षर* keyword_मूल्य,
                     अक्षर* बफर, int32_t बफर_Capacity,
                     UErrorCode* status)
{
    अगर (U_FAILURE(*status)) { वापस 0; }

    अगर (keywordName == nullptr || *keywordName == 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    अगर (बफर_Capacity <= 1) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    int32_t bufLen = (int32_t)uprv_strlen(बफर);
    अगर(बफर_Capacity<bufLen) {
        /* The capacity is less than the length?! Is this NUL terminated? */
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    अक्षर* keywords = const_cast<अक्षर*>(
        locale_getKeywordsStart({बफर, static_cast<std::string_view::size_type>(bufLen)}));
    int32_t baseLen = keywords == nullptr ? bufLen : keywords - बफर;
    // Remove -1 from the capacity so that this function can guarantee NUL termination.
    CheckedArrayByteSink sink(keywords == nullptr ? बफर + bufLen : keywords,
                              बफर_Capacity - baseLen - 1);
    int32_t reslen = ulocimp_set_Keyword_मूल्य(
        keywords == nullptr ? std::string_view() : keywords,
        keywordName,
        keyword_मूल्य == nullptr ? std::string_view() : keyword_मूल्य,
        sink,
        *status);

    अगर (U_FAILURE(*status)) {
        वापस *status == U_बफर_OVERFLOW_ERROR ? reslen + baseLen : 0;
    }

    // See the documentation for this function, it's guaranteed to never
    // overflow the buffer but instead abort with BUFFER_OVERFLOW_ERROR.
    // In this case, nothing has been written to the sink, so it cannot have Overflowed().
    U_ASSERT(!sink.Overflowed());
    U_ASSERT(reslen >= 0);
    वापस u_terminateChars(बफर, बफर_Capacity, reslen + baseLen, status);
}

U_EXPORT व्योम
ulocimp_set_Keyword_मूल्य(std::string_view keywordName,
                        std::string_view keyword_मूल्य,
                        CharString& localeID,
                        UErrorCode& status)
{
    अगर (U_FAILURE(status)) { वापस; }
    std::string_view keywords;
    अगर (स्थिर अक्षर* start = locale_getKeywordsStart(localeID.toStringPiece()); start != nullptr) {
        // This is safe because CharString::truncate() doesn't actually erase any
        // data, but simply sets the position for where new data will be written.
        int32_t size = start - localeID.तथ्य();
        keywords = localeID.toStringPiece();
        keywords.remove_prefix(size);
        localeID.truncate(size);
    }
    CharStringByteSink sink(&localeID);
    ulocimp_set_Keyword_मूल्य(keywords, keywordName, keyword_मूल्य, sink, status);
}

U_EXPORT int32_t
ulocimp_set_Keyword_मूल्य(std::string_view keywords,
                        std::string_view keywordName,
                        std::string_view keyword_मूल्य,
                        ByteSink& sink,
                        UErrorCode& status)
{
    अगर (U_FAILURE(status)) { वापस 0; }

    /* TODO: sorting. removal. */
    int32_t needLen = 0;
    int32_t rc;
    CharString updated_कुंजियाँ_And_मूल्यावली;
    bool handled_निवेश_कुंजी_And_मूल्य = false;
    अक्षर कुंजी_मूल्य_Prefix = '@';

    अगर (status == U_STRING_NOT_TERMINATED_WARNING) {
        status = U_ZERO_ERROR;
    }
    अगर (keywordName.empty()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }
    CharString canonKeywordName = locale_canonKeywordName(keywordName, status);
    अगर (U_FAILURE(status)) {
        वापस 0;
    }

    CharString canon_Keyword_मूल्य;
    क्रम (अक्षर c : keyword_मूल्य) {
        अगर (!UPRV_ISALPHANUM(c) && !UPRV_OK_मूल्य_PUNCTUATION(c)) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* malformed key value */
            वापस 0;
        }
        /* Should we force lowercase in value to set? */
        canon_Keyword_मूल्य.append(c, status);
    }
    अगर (U_FAILURE(status)) {
        वापस 0;
    }

    अगर (keywords.size() <= 1) {
        अगर (canon_Keyword_मूल्य.isEmpty()) { /* no keywords = nothing to remove */
            U_ASSERT(status != U_STRING_NOT_TERMINATED_WARNING);
            वापस 0;
        }

        needLen = 1 + canonKeywordName.length() + 1 + canon_Keyword_मूल्य.length();
        int32_t capacity = 0;
        अक्षर* बफर = sink.Get_Append_बफर(
                needLen, needLen, nullptr, needLen, &capacity);
        अगर (capacity < needLen || बफर == nullptr) {
            status = U_बफर_OVERFLOW_ERROR;
            वापस needLen; /* no change */
        }
        अक्षर* it = बफर;

        *it++ = '@';
        uprv_memcpy(it, canonKeywordName.तथ्य(), canonKeywordName.length());
        it += canonKeywordName.length();
        *it++ = '=';
        uprv_memcpy(it, canon_Keyword_मूल्य.तथ्य(), canon_Keyword_मूल्य.length());
        sink.Append(बफर, needLen);
        U_ASSERT(status != U_STRING_NOT_TERMINATED_WARNING);
        वापस needLen;
    } /* end shortcut - no @ */

    /* search for keyword */
    क्रम (माप_प्रकार keywordStart = 0; keywordStart != std::string_view::npos;) {
        keywordStart++; /* skip @ or ; */
        माप_प्रकार अग्र_Equalsign = keywords.find('=', keywordStart);
        अगर (अग्र_Equalsign == std::string_view::npos) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* key must have =value */
            वापस 0;
        }
        /* strip leading & trailing spaces (TC decided to tolerate these) */
        जबतक (keywordStart < keywords.size() && keywords[keywordStart] == ' ') {
            keywordStart++;
        }
        माप_प्रकार कुंजी_मूल्य_पुच्छ = अग्र_Equalsign;
        जबतक (कुंजी_मूल्य_पुच्छ > keywordStart && keywords[कुंजी_मूल्य_पुच्छ - 1] == ' ') {
            कुंजी_मूल्य_पुच्छ--;
        }
        /* now keyValueTail points to first char after the keyName */
        /* copy & normalize keyName from locale */
        अगर (keywordStart == कुंजी_मूल्य_पुच्छ) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* empty keyword name in passed-in locale */
            वापस 0;
        }
        CharString localeKeywordName;
        जबतक (keywordStart < कुंजी_मूल्य_पुच्छ) {
            अगर (!UPRV_ISALPHANUM(keywords[keywordStart])) {
                status = U_ILLEGAL_ARGUMENT_ERROR; /* malformed keyword name */
                वापस 0;
            }
            localeKeywordName.append(uprv_tolower(keywords[keywordStart++]), status);
        }
        अगर (U_FAILURE(status)) {
            वापस 0;
        }

        माप_प्रकार अग्र_Separator = keywords.find(';', अग्र_Equalsign);

        /* start processing the value part */
        अग्र_Equalsign++; /* skip '=' */
        /* First strip leading & trailing spaces (TC decided to tolerate these) */
        जबतक (अग्र_Equalsign < keywords.size() && keywords[अग्र_Equalsign] == ' ') {
            अग्र_Equalsign++;
        }
        कुंजी_मूल्य_पुच्छ = अग्र_Separator == std::string_view::npos ? keywords.size() : अग्र_Separator;
        जबतक (कुंजी_मूल्य_पुच्छ > अग्र_Equalsign && keywords[कुंजी_मूल्य_पुच्छ - 1] == ' ') {
            कुंजी_मूल्य_पुच्छ--;
        }
        अगर (अग्र_Equalsign == कुंजी_मूल्य_पुच्छ) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* empty key value in passed-in locale */
            वापस 0;
        }

        rc = uprv_strcmp(canonKeywordName.तथ्य(), localeKeywordName.तथ्य());
        अगर(rc == 0) {
            /* Current entry matches the input keyword. Update the entry */
            अगर (!canon_Keyword_मूल्य.isEmpty()) { /* updating a value */
                updated_कुंजियाँ_And_मूल्यावली.append(कुंजी_मूल्य_Prefix, status);
                कुंजी_मूल्य_Prefix = ';'; /* for any subsequent key-value pair */
                updated_कुंजियाँ_And_मूल्यावली.append(canonKeywordName, status);
                updated_कुंजियाँ_And_मूल्यावली.append('=', status);
                updated_कुंजियाँ_And_मूल्यावली.append(canon_Keyword_मूल्य, status);
            } /* else removing this entry, don't emit anything */
            handled_निवेश_कुंजी_And_मूल्य = true;
        } अन्यथा {
           /* input keyword sorts earlier than current entry, add before current entry */
            अगर (rc < 0 && !canon_Keyword_मूल्य.isEmpty() && !handled_निवेश_कुंजी_And_मूल्य) {
                /* insert new entry at this location */
                updated_कुंजियाँ_And_मूल्यावली.append(कुंजी_मूल्य_Prefix, status);
                कुंजी_मूल्य_Prefix = ';'; /* for any subsequent key-value pair */
                updated_कुंजियाँ_And_मूल्यावली.append(canonKeywordName, status);
                updated_कुंजियाँ_And_मूल्यावली.append('=', status);
                updated_कुंजियाँ_And_मूल्यावली.append(canon_Keyword_मूल्य, status);
                handled_निवेश_कुंजी_And_मूल्य = true;
            }
            /* copy the current entry */
            updated_कुंजियाँ_And_मूल्यावली.append(कुंजी_मूल्य_Prefix, status);
            कुंजी_मूल्य_Prefix = ';'; /* for any subsequent key-value pair */
            updated_कुंजियाँ_And_मूल्यावली.append(localeKeywordName, status);
            updated_कुंजियाँ_And_मूल्यावली.append('=', status);
            updated_कुंजियाँ_And_मूल्यावली.append(keywords.तथ्य() + अग्र_Equalsign,
                                        static_cast<int32_t>(कुंजी_मूल्य_पुच्छ - अग्र_Equalsign), status);
        }
        अगर (अग्र_Separator == std::string_view::npos && !canon_Keyword_मूल्य.isEmpty() && !handled_निवेश_कुंजी_And_मूल्य) {
            /* append new entry at the end, it sorts later than existing entries */
            updated_कुंजियाँ_And_मूल्यावली.append(कुंजी_मूल्य_Prefix, status);
            /* skip keyValuePrefix update, no subsequent key-value pair */
            updated_कुंजियाँ_And_मूल्यावली.append(canonKeywordName, status);
            updated_कुंजियाँ_And_मूल्यावली.append('=', status);
            updated_कुंजियाँ_And_मूल्यावली.append(canon_Keyword_मूल्य, status);
            handled_निवेश_कुंजी_And_मूल्य = true;
        }
        keywordStart = अग्र_Separator;
    } /* end loop searching */

    /* Any error from updatedKeysAndValues.append above would be internal and not due to
     * problems with the passed-in locale. So if we did encounter problems with the
     * passed-in locale above, those errors took precedence and overrode any error
     * status from updatedKeysAndValues.append, and also caused a return of 0. If there
     * are errors here they are from updatedKeysAndValues.append; they do cause an
     * error return but the passed-in locale is unmodified and the original bufLen is
     * returned.
     */
    अगर (!handled_निवेश_कुंजी_And_मूल्य || U_FAILURE(status)) {
        /* if input key/value specified removal of a keyword not present in locale, or
         * there was an error in CharString.append, leave original locale alone. */
        U_ASSERT(status != U_STRING_NOT_TERMINATED_WARNING);
        // The sink is expected to be a buffer which already contains the full
        // locale string, so when it isn't going to be modified there's no need
        // to actually write any data to it, as the data is already there. Only
        // the first character needs to be overwritten (changing '\0' to '@').
        needLen = static_cast<int32_t>(keywords.size());
        int32_t capacity = 0;
        अक्षर* बफर = sink.Get_Append_बफर(
                needLen, needLen, nullptr, needLen, &capacity);
        अगर (capacity < needLen || बफर == nullptr) {
            status = U_बफर_OVERFLOW_ERROR;
        } अन्यथा {
            *बफर = '@';
            sink.Append(बफर, needLen);
        }
        वापस needLen;
    }

    needLen = updated_कुंजियाँ_And_मूल्यावली.length();
    // Check to see can we fit the updatedKeysAndValues, if not, return
    // U_BUFFER_OVERFLOW_ERROR without copy updatedKeysAndValues into it.
    // We do this because this API function does not behave like most others:
    // It promises never to set a U_STRING_NOT_TERMINATED_WARNING.
    // When the contents fits but without the terminating NUL, in this case we need to not change
    // the buffer contents and return with a buffer overflow error.
    अगर (needLen > 0) {
        int32_t capacity = 0;
        अक्षर* बफर = sink.Get_Append_बफर(
                needLen, needLen, nullptr, needLen, &capacity);
        अगर (capacity < needLen || बफर == nullptr) {
            status = U_बफर_OVERFLOW_ERROR;
            वापस needLen;
        }
        uprv_memcpy(बफर, updated_कुंजियाँ_And_मूल्यावली.तथ्य(), needLen);
        sink.Append(बफर, needLen);
    }
    U_ASSERT(status != U_STRING_NOT_TERMINATED_WARNING);
    वापस needLen;
}

/* ### ID parsing implementation **************************************************/

namespace {

अंतरभूत bool _isPrefixLetter(अक्षर a) { वापस a == 'x' || a == 'X' || a == 'i' || a == 'I'; }

/*returns true if one of the special prefixes is here (s=string)
  'x-' or 'i-' */
अंतरभूत bool _isIDPrefix(std::string_view s) {
    वापस s.size() >= 2 && _isPrefixLetter(s[0]) && _isIDSeparator(s[1]);
}

/* Dot terminates it because of POSIX form  where dot precedes the codepage
 * except for variant
 */
अंतरभूत bool _isTerminator(अक्षर a) { वापस a == '.' || a == '@'; }

अंतरभूत bool _isBCP47Extension(std::string_view p) {
    वापस p.size() >= 3 &&
           p[0] == '-' &&
           (p[1] == 't' || p[1] == 'T' ||
            p[1] == 'u' || p[1] == 'U' ||
            p[1] == 'x' || p[1] == 'X') &&
           p[2] == '-';
}

/**
 * Lookup 'key' in the array 'list'.  The array 'list' should contain
 * a nullptr entry, followed by more entries, and a second nullptr entry.
 *
 * The 'list' param should be LANGUAGES, LANGUAGES_3, COUNTRIES, or
 * COUNTRIES_3.
 */
std::optional<int16_t> find_सूचकांक_2(स्थिर अक्षर* स्थिर* list, स्थिर अक्षर* कुंजी)
{
    स्थिर अक्षर* स्थिर* anchor = list;
    int32_t pass = 0;

    /* Make two passes through two nullptr-terminated arrays at 'list' */
    जबतक (pass++ < 2) {
        जबतक (*list) {
            अगर (uprv_strcmp(कुंजी, *list) == 0) {
                वापस static_cast<int16_t>(list - anchor);
            }
            list++;
        }
        ++list;     /* skip final nullptr *CWB*/
    }
    वापस std::nullopt;
}

}  // namespace

U_CFUNC स्थिर अक्षर*
uloc_getCurrentCountryID(स्थिर अक्षर* oldID U_LIFETIME_BOUND) {
    std::optional<int16_t> offset = find_सूचकांक_2(DEPRECATED_COUNTRIES, oldID);
    वापस offset.has_value() ? REPLACEMENT_COUNTRIES[*offset] : oldID;
}
U_CFUNC स्थिर अक्षर*
uloc_getCurrentLanguageID(स्थिर अक्षर* oldID U_LIFETIME_BOUND) {
    std::optional<int16_t> offset = find_सूचकांक_2(DEPRECATED_LANGUAGES, oldID);
    वापस offset.has_value() ? REPLACEMENT_LANGUAGES[*offset] : oldID;
}

namespace {

/*
 * the internal functions _getLanguage(), _getScript(), _getRegion(), _getVariant()
 * avoid duplicating code to handle the earlier locale ID pieces
 * in the functions for the later ones by
 * setting the *pEnd pointer to where they stopped parsing
 *
 * TODO try to use this in Locale
 */

माप_प्रकार _getLanguage(std::string_view localeID, ByteSink* sink, UErrorCode& status) {
    माप_प्रकार skip = 0;
    अगर (localeID.size() == 4 && uprv_strnicmp(localeID.तथ्य(), "root", 4) == 0) {
        skip = 4;
        localeID.remove_prefix(skip);
    } अन्यथा अगर (localeID.size() >= 3 && uprv_strnicmp(localeID.तथ्य(), "und", 3) == 0 &&
               (localeID.size() == 3 ||
                localeID[3] == '-' ||
                localeID[3] == '_' ||
                localeID[3] == '@')) {
        skip = 3;
        localeID.remove_prefix(skip);
    }

    constexpr int32_t MAXLEN = ULOC_LANG_CAPACITY - 1;  // Minus NUL.

    /* if it starts with i- or x- then copy that prefix */
    माप_प्रकार len = _isIDPrefix(localeID) ? 2 : 0;
    जबतक (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len])) {
        अगर (len == MAXLEN) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            वापस 0;
        }
        len++;
    }

    अगर (sink == nullptr || len == 0) { वापस skip + len; }

    int32_t minCapacity = uprv_max(static_cast<int32_t>(len), 4);  // Minimum 3 letters plus NUL.
    अक्षर scratch[MAXLEN];
    int32_t capacity = 0;
    अक्षर* बफर = sink->Get_Append_बफर(
            minCapacity, minCapacity, scratch, UPRV_LENGTHOF(scratch), &capacity);

    क्रम (माप_प्रकार i = 0; i < len; ++i) {
        बफर[i] = uprv_tolower(localeID[i]);
    }
    अगर (localeID.size() >= 2 && _isIDSeparator(localeID[1])) {
        बफर[1] = '-';
    }

    अगर (len == 3) {
        /* convert 3 character code to 2 character code if possible *CWB*/
        U_ASSERT(capacity >= 4);
        बफर[3] = '\0';
        std::optional<int16_t> offset = find_सूचकांक_2(LANGUAGES_3, बफर);
        अगर (offset.has_value()) {
            स्थिर अक्षर* स्थिर alias = LANGUAGES[*offset];
            sink->Append(alias, static_cast<int32_t>(uprv_strlen(alias)));
            वापस skip + len;
        }
    }

    sink->Append(बफर, static_cast<int32_t>(len));
    वापस skip + len;
}

माप_प्रकार _getScript(std::string_view localeID, ByteSink* sink) {
    constexpr int32_t लंबाई_3 = 4;

    माप_प्रकार len = 0;
    जबतक (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len]) &&
            uprv_isASCIILetter(localeID[len])) {
        अगर (len == लंबाई_3) { वापस 0; }
        len++;
    }
    अगर (len != लंबाई_3) { वापस 0; }

    अगर (sink == nullptr) { वापस len; }

    अक्षर scratch[लंबाई_3];
    int32_t capacity = 0;
    अक्षर* बफर = sink->Get_Append_बफर(
            लंबाई_3, लंबाई_3, scratch, UPRV_LENGTHOF(scratch), &capacity);

    बफर[0] = uprv_toupper(localeID[0]);
    क्रम (int32_t i = 1; i < लंबाई_3; ++i) {
        बफर[i] = uprv_tolower(localeID[i]);
    }

    sink->Append(बफर, लंबाई_3);
    वापस len;
}

माप_प्रकार _getRegion(std::string_view localeID, ByteSink* sink) {
    constexpr int32_t MINLEN = 2;
    constexpr int32_t MAXLEN = ULOC_COUNTRY_CAPACITY - 1;  // Minus NUL.

    माप_प्रकार len = 0;
    जबतक (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len])) {
        अगर (len == MAXLEN) { वापस 0; }
        len++;
    }
    अगर (len < MINLEN) { वापस 0; }

    अगर (sink == nullptr) { वापस len; }

    अक्षर scratch[ULOC_COUNTRY_CAPACITY];
    int32_t capacity = 0;
    अक्षर* बफर = sink->Get_Append_बफर(
            ULOC_COUNTRY_CAPACITY,
            ULOC_COUNTRY_CAPACITY,
            scratch,
            UPRV_LENGTHOF(scratch),
            &capacity);

    क्रम (माप_प्रकार i = 0; i < len; ++i) {
        बफर[i] = uprv_toupper(localeID[i]);
    }

    अगर (len == 3) {
        /* convert 3 character code to 2 character code if possible *CWB*/
        U_ASSERT(capacity >= 4);
        बफर[3] = '\0';
        std::optional<int16_t> offset = find_सूचकांक_2(COUNTRIES_3, बफर);
        अगर (offset.has_value()) {
            स्थिर अक्षर* स्थिर alias = COUNTRIES[*offset];
            sink->Append(alias, static_cast<int32_t>(uprv_strlen(alias)));
            वापस len;
        }
    }

    sink->Append(बफर, static_cast<int32_t>(len));
    वापस len;
}

/**
 * @param needSeparator if true, then add leading '_' if any variants
 * are added to 'variant'
 */
माप_प्रकार
_getVariant(std::string_view localeID,
            अक्षर पूर्व,
            ByteSink* sink,
            bool needSeparator,
            UErrorCode& status) {
    अगर (U_FAILURE(status) || localeID.empty()) वापस 0;

    // Reasonable upper limit for variants
    // There are no strict limitation of the syntax of variant in the legacy
    // locale format. If the locale is constructed from unicode_locale_id
    // as defined in UTS35, then we know each unicode_variant_subtag
    // could have max length of 8 ((alphanum{5,8} | digit alphanum{3})
    // 179 would allow 20 unicode_variant_subtag with sep in the
    // unicode_locale_id
    // 8*20 + 1*(20-1) = 179
    constexpr int32_t MAX_VARIANTS_लंबाई = 179;

    /* get one or more variant tags and separate them with '_' */
    माप_प्रकार सूचकांक = 0;
    अगर (_isIDSeparator(पूर्व)) {
        /* get a variant string after a '-' or '_' */
        क्रम (std::string_view sub = localeID;;) {
            माप_प्रकार next = sub.find_first_of(".@_-");
            // For historical reasons, a trailing separator is included in the variant.
            bool finished = next == std::string_view::npos || next + 1 == sub.length();
            माप_प्रकार limit = finished ? sub.length() : next;
            सूचकांक += limit;
            अगर (सूचकांक > MAX_VARIANTS_लंबाई) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
                वापस 0;
            }

            अगर (sink != nullptr) {
                अगर (needSeparator) {
                    sink->Append("_", 1);
                } अन्यथा {
                    needSeparator = true;
                }

                int32_t length = static_cast<int32_t>(limit);
                int32_t minCapacity = uprv_min(length, MAX_VARIANTS_लंबाई);
                अक्षर scratch[MAX_VARIANTS_लंबाई];
                int32_t capacity = 0;
                अक्षर* बफर = sink->Get_Append_बफर(
                        minCapacity, minCapacity, scratch, UPRV_LENGTHOF(scratch), &capacity);

                क्रम (माप_प्रकार i = 0; i < limit; ++i) {
                    बफर[i] = uprv_toupper(sub[i]);
                }
                sink->Append(बफर, length);
            }

            अगर (finished) { वापस सूचकांक; }
            sub.remove_prefix(next);
            अगर (_isTerminator(sub.front()) || _isBCP47Extension(sub)) { वापस सूचकांक; }
            sub.remove_prefix(1);
            सूचकांक++;
        }
    }

    माप_प्रकार skip = 0;
    /* if there is no variant tag after a '-' or '_' then look for '@' */
    अगर (पूर्व == '@') {
        /* keep localeID */
    } अन्यथा अगर (स्थिर अक्षर* p = locale_getKeywordsStart(localeID); p != nullptr) {
        skip = 1 + p - localeID.तथ्य(); /* point after the '@' */
        localeID.remove_prefix(skip);
    } अन्यथा {
        वापस 0;
    }
    क्रम (; सूचकांक < localeID.size() && !_isTerminator(localeID[सूचकांक]); सूचकांक++) {
        अगर (सूचकांक >= MAX_VARIANTS_लंबाई) { // same as length > MAX_VARIANTS_LENGTH
            status = U_ILLEGAL_ARGUMENT_ERROR;
            वापस 0;
        }
        अगर (needSeparator) {
            अगर (sink != nullptr) {
                sink->Append("_", 1);
            }
            needSeparator = false;
        }
        अगर (sink != nullptr) {
            अक्षर c = uprv_toupper(localeID[सूचकांक]);
            अगर (c == '-' || c == ',') c = '_';
            sink->Append(&c, 1);
        }
    }
    वापस skip + सूचकांक;
}

}  // namespace

U_EXPORT CharString
ulocimp_getLanguage(std::string_view localeID, UErrorCode& status) {
    वापस ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getSubtags(
                    localeID,
                    &sink,
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr,
                    status);
        },
        status);
}

U_EXPORT CharString
ulocimp_getScript(std::string_view localeID, UErrorCode& status) {
    वापस ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getSubtags(
                    localeID,
                    nullptr,
                    &sink,
                    nullptr,
                    nullptr,
                    nullptr,
                    status);
        },
        status);
}

U_EXPORT CharString
ulocimp_getRegion(std::string_view localeID, UErrorCode& status) {
    वापस ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getSubtags(
                    localeID,
                    nullptr,
                    nullptr,
                    &sink,
                    nullptr,
                    nullptr,
                    status);
        },
        status);
}

U_EXPORT CharString
ulocimp_getVariant(std::string_view localeID, UErrorCode& status) {
    वापस ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getSubtags(
                    localeID,
                    nullptr,
                    nullptr,
                    nullptr,
                    &sink,
                    nullptr,
                    status);
        },
        status);
}

U_EXPORT व्योम
ulocimp_getSubtags(
        std::string_view localeID,
        CharString* language,
        CharString* script,
        CharString* region,
        CharString* variant,
        स्थिर अक्षर** pEnd,
        UErrorCode& status) {
    अगर (U_FAILURE(status)) { वापस; }

    std::optional<CharStringByteSink> languageSink;
    std::optional<CharStringByteSink> scriptSink;
    std::optional<CharStringByteSink> regionSink;
    std::optional<CharStringByteSink> variantSink;

    अगर (language != nullptr) { languageSink.emplace(language); }
    अगर (script != nullptr) { scriptSink.emplace(script); }
    अगर (region != nullptr) { regionSink.emplace(region); }
    अगर (variant != nullptr) { variantSink.emplace(variant); }

    ulocimp_getSubtags(
            localeID,
            languageSink.has_value() ? &*languageSink : nullptr,
            scriptSink.has_value() ? &*scriptSink : nullptr,
            regionSink.has_value() ? &*regionSink : nullptr,
            variantSink.has_value() ? &*variantSink : nullptr,
            pEnd,
            status);
}

U_EXPORT व्योम
ulocimp_getSubtags(
        std::string_view localeID,
        ByteSink* language,
        ByteSink* script,
        ByteSink* region,
        ByteSink* variant,
        स्थिर अक्षर** pEnd,
        UErrorCode& status) {
    अगर (U_FAILURE(status)) { वापस; }

    अगर (pEnd != nullptr) {
        *pEnd = localeID.तथ्य();
    } अन्यथा अगर (language == nullptr &&
               script == nullptr &&
               region == nullptr &&
               variant == nullptr) {
        वापस;
    }

    अगर (localeID.empty()) { वापस; }

    bool hasRegion = false;

    {
        माप_प्रकार len = _getLanguage(localeID, language, status);
        अगर (U_FAILURE(status)) { वापस; }
        अगर (len > 0) {
            localeID.remove_prefix(len);
        }
    }

    अगर (pEnd != nullptr) {
        *pEnd = localeID.तथ्य();
    } अन्यथा अगर (script == nullptr &&
               region == nullptr &&
               variant == nullptr) {
        वापस;
    }

    अगर (localeID.empty()) { वापस; }

    अगर (_isIDSeparator(localeID.front())) {
        std::string_view sub = localeID;
        sub.remove_prefix(1);
        माप_प्रकार len = _getScript(sub, script);
        अगर (len > 0) {
            localeID.remove_prefix(len + 1);
            अगर (pEnd != nullptr) { *pEnd = localeID.तथ्य(); }
        }
    }

    अगर ((region == nullptr && variant == nullptr && pEnd == nullptr) || localeID.empty()) { वापस; }

    अगर (_isIDSeparator(localeID.front())) {
        std::string_view sub = localeID;
        sub.remove_prefix(1);
        माप_प्रकार len = _getRegion(sub, region);
        अगर (len > 0) {
            hasRegion = true;
            localeID.remove_prefix(len + 1);
            अगर (pEnd != nullptr) { *pEnd = localeID.तथ्य(); }
        }
    }

    अगर ((variant == nullptr && pEnd == nullptr) || localeID.empty()) { वापस; }

    bool hasVariant = false;

    अगर (_isIDSeparator(localeID.front()) && !_isBCP47Extension(localeID)) {
        std::string_view sub = localeID;
        /* If there was no country ID, skip a possible extra IDSeparator */
        माप_प्रकार skip = !hasRegion && localeID.size() > 1 && _isIDSeparator(localeID[1]) ? 2 : 1;
        sub.remove_prefix(skip);
        माप_प्रकार len = _getVariant(sub, localeID[0], variant, false, status);
        अगर (U_FAILURE(status)) { वापस; }
        अगर (len > 0) {
            hasVariant = true;
            localeID.remove_prefix(skip + len);
            अगर (pEnd != nullptr) { *pEnd = localeID.तथ्य(); }
        }
    }

    अगर ((variant == nullptr && pEnd == nullptr) || localeID.empty()) { वापस; }

    अगर (_isBCP47Extension(localeID)) {
        localeID.remove_prefix(2);
        constexpr अक्षर vaposix[] = "-va-posix";
        constexpr माप_प्रकार length = माप vaposix - 1;
        क्रम (माप_प्रकार next;; localeID.remove_prefix(next)) {
            next = localeID.find('-', 1);
            अगर (next == std::string_view::npos) { अवरोध; }
            next = localeID.find('-', next + 1);
            bool finished = next == std::string_view::npos;
            std::string_view sub = localeID;
            अगर (!finished) { sub.remove_suffix(sub.length() - next); }

            अगर (sub.length() == length && uprv_strnicmp(sub.तथ्य(), vaposix, length) == 0) {
                अगर (variant != nullptr) {
                    अगर (hasVariant) { variant->Append("_", 1); }
                    constexpr अक्षर posix[] = "POSIX";
                    variant->Append(posix, माप posix - 1);
                }
                अगर (pEnd != nullptr) { *pEnd = localeID.तथ्य() + length; }
            }

            अगर (finished) { अवरोध; }
        }
    }
}

/* Keyword enumeration */

प्रकार काष्ठा UKeywordsContext {
    अक्षर* keywords;
    अक्षर* current;
} UKeywordsContext;

U_CDECL_BEGIN

अटल व्योम U_CALLCONV
uloc_kw_closeKeywords(UEnumeration *enumerator) {
    uprv_free(((UKeywordsContext *)enumerator->context)->keywords);
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

अटल int32_t U_CALLCONV
uloc_kw_गिनती_Keywords(UEnumeration *en, UErrorCode * /*status*/) {
    अक्षर *kw = ((UKeywordsContext *)en->context)->keywords;
    int32_t परिणाम = 0;
    जबतक(*kw) {
        परिणाम++;
        kw += uprv_strlen(kw)+1;
    }
    वापस परिणाम;
}

अटल स्थिर अक्षर * U_CALLCONV
uloc_kw_अग्र_Keyword(UEnumeration* en,
                    int32_t* परिणाम_लंबाई,
                    UErrorCode* /*status*/) {
    स्थिर अक्षर* परिणाम = ((UKeywordsContext *)en->context)->current;
    int32_t len = 0;
    अगर(*परिणाम) {
        len = (int32_t)uprv_strlen(((UKeywordsContext *)en->context)->current);
        ((UKeywordsContext *)en->context)->current += len+1;
    } अन्यथा {
        परिणाम = nullptr;
    }
    अगर (परिणाम_लंबाई) {
        *परिणाम_लंबाई = len;
    }
    वापस परिणाम;
}

अटल व्योम U_CALLCONV
uloc_kw_resetKeywords(UEnumeration* en,
                      UErrorCode* /*status*/) {
    ((UKeywordsContext *)en->context)->current = ((UKeywordsContext *)en->context)->keywords;
}

U_CDECL_END


अटल स्थिर UEnumeration gKeywordsEnum = {
    nullptr,
    nullptr,
    uloc_kw_closeKeywords,
    uloc_kw_गिनती_Keywords,
    uenum_unextDefault,
    uloc_kw_अग्र_Keyword,
    uloc_kw_resetKeywords
};

U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywordList(स्थिर अक्षर *keywordList, int32_t keyword_List_आकार, UErrorCode* status)
{
    अगर (U_FAILURE(*status)) { वापस nullptr; }

    LocalMemory<UKeywordsContext> myContext;
    LocalMemory<UEnumeration> परिणाम;

    myContext.adoptInstead(static_cast<UKeywordsContext *>(uprv_malloc(माप(UKeywordsContext))));
    परिणाम.adoptInstead(static_cast<UEnumeration *>(uprv_malloc(माप(UEnumeration))));
    अगर (myContext.isNull() || परिणाम.isNull()) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
    uprv_memcpy(परिणाम.getAlias(), &gKeywordsEnum, माप(UEnumeration));
    myContext->keywords = static_cast<अक्षर *>(uprv_malloc(keyword_List_आकार+1));
    अगर (myContext->keywords == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
    uprv_memcpy(myContext->keywords, keywordList, keyword_List_आकार);
    myContext->keywords[keyword_List_आकार] = 0;
    myContext->current = myContext->keywords;
    परिणाम->context = myContext.orphan();
    वापस परिणाम.orphan();
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywords(स्थिर अक्षर* localeID,
                        UErrorCode* status)
{
    अगर(status==nullptr || U_FAILURE(*status)) {
        वापस nullptr;
    }

    CharString अस्थायी_बफर;
    स्थिर अक्षर* tmpLocaleID;

    अगर (localeID != nullptr && _hasBCP47Extension(localeID)) {
        अस्थायी_बफर = ulocimp_forLanguageTag(localeID, -1, nullptr, *status);
        tmpLocaleID = U_SUCCESS(*status) && !अस्थायी_बफर.isEmpty() ? अस्थायी_बफर.तथ्य() : localeID;
    } अन्यथा {
        अगर (localeID==nullptr) {
            localeID=uloc_getDefault();
        }
        tmpLocaleID=localeID;
    }

    ulocimp_getSubtags(
            tmpLocaleID,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            &tmpLocaleID,
            *status);
    अगर (U_FAILURE(*status)) {
        वापस nullptr;
    }

    /* keywords are located after '@' */
    अगर((tmpLocaleID = locale_getKeywordsStart(tmpLocaleID)) != nullptr) {
        CharString keywords = ulocimp_getKeywords(tmpLocaleID + 1, '@', false, *status);
        अगर (U_FAILURE(*status)) {
            वापस nullptr;
        }
        वापस uloc_openKeywordList(keywords.तथ्य(), keywords.length(), status);
    }
    वापस nullptr;
}


/* bit-flags for 'options' parameter of _canonicalize */
#घोषणा _ULOC_STRIP_KEYWORDS 0x2
#घोषणा _ULOC_CANONICALIZE   0x1

namespace {

अंतरभूत bool OPTION_SET(uint32_t options, uint32_t mask) { वापस (options & mask) != 0; }

constexpr अक्षर i_default[] = {'i', '-', 'd', 'e', 'f', 'a', 'u', 'l', 't'};
constexpr int32_t I_DEFAULT_लंबाई = UPRV_LENGTHOF(i_default);

/**
 * Canonicalize the given localeID, to level 1 or to level 2,
 * depending on the options.  To specify level 1, pass in options=0.
 * To specify level 2, pass in options=_ULOC_CANONICALIZE.
 *
 * This is the code underlying uloc_getName and uloc_canonicalize.
 */
व्योम
_canonicalize(std::string_view localeID,
              ByteSink& sink,
              uint32_t options,
              UErrorCode& err) {
    अगर (U_FAILURE(err)) {
        वापस;
    }

    int32_t j, field_गिनती=0;
    CharString अस्थायी_बफर;  // if localeID has a BCP47 extension, tmpLocaleID points to this
    CharString localeIDWithHyphens;  // if localeID has a BPC47 extension and have _, tmpLocaleID points to this
    std::string_view origLocaleID;
    std::string_view tmpLocaleID;
    माप_प्रकार keywordAssign = std::string_view::npos;
    माप_प्रकार separatorIndicator = std::string_view::npos;

    अगर (_hasBCP47Extension(localeID)) {
        std::string_view localeIDPtr = localeID;

        // convert all underbars to hyphens, unless the "BCP47 extension" comes at the beginning of the string
        अगर (localeID.size() >= 2 && localeID.find('_') != std::string_view::npos && localeID[1] != '-' && localeID[1] != '_') {
            localeIDWithHyphens.append(localeID, err);
            अगर (U_SUCCESS(err)) {
                क्रम (अक्षर* p = localeIDWithHyphens.तथ्य(); *p != '\0'; ++p) {
                    अगर (*p == '_') {
                        *p = '-';
                    }
                }
                localeIDPtr = localeIDWithHyphens.toStringPiece();
            }
        }

        अस्थायी_बफर = ulocimp_forLanguageTag(localeIDPtr.तथ्य(), static_cast<int32_t>(localeIDPtr.size()), nullptr, err);
        tmpLocaleID = U_SUCCESS(err) && !अस्थायी_बफर.isEmpty() ? static_cast<std::string_view>(अस्थायी_बफर.toStringPiece()) : localeIDPtr;
    } अन्यथा {
        tmpLocaleID=localeID;
    }

    origLocaleID=tmpLocaleID;

    /* get all pieces, one after another, and separate with '_' */
    CharString tag;
    CharString script;
    CharString country;
    CharString variant;
    स्थिर अक्षर* end = nullptr;
    ulocimp_getSubtags(
            tmpLocaleID,
            &tag,
            &script,
            &country,
            &variant,
            &end,
            err);
    अगर (U_FAILURE(err)) {
        वापस;
    }
    U_ASSERT(end != nullptr);
    अगर (end > tmpLocaleID.तथ्य()) {
        tmpLocaleID.remove_prefix(end - tmpLocaleID.तथ्य());
    }

    अगर (tag.length() == I_DEFAULT_लंबाई && origLocaleID.length() >= I_DEFAULT_लंबाई &&
            uprv_strncmp(origLocaleID.तथ्य(), i_default, I_DEFAULT_लंबाई) == 0) {
        tag.सफा();
        tag.append(uloc_getDefault(), err);
    } अन्यथा {
        अगर (!script.isEmpty()) {
            ++field_गिनती;
            tag.append('_', err);
            tag.append(script, err);
        }
        अगर (!country.isEmpty()) {
            ++field_गिनती;
            tag.append('_', err);
            tag.append(country, err);
        }
        अगर (!variant.isEmpty()) {
            ++field_गिनती;
            अगर (country.isEmpty()) {
                tag.append('_', err);
            }
            tag.append('_', err);
            tag.append(variant, err);
        }
    }

    /* Copy POSIX-style charset specifier, if any [mr.utf8] */
    अगर (!OPTION_SET(options, _ULOC_CANONICALIZE) && !tmpLocaleID.empty() && tmpLocaleID.front() == '.') {
        tag.append('.', err);
        tmpLocaleID.remove_prefix(1);
        माप_प्रकार length;
        अगर (माप_प्रकार atPos = tmpLocaleID.find('@'); atPos != std::string_view::npos) {
            length = atPos;
        } अन्यथा {
            length = tmpLocaleID.length();
        }
        // The longest charset name we found in IANA charset registry
        // https://www.iana.org/assignments/character-sets/ is
        // "Extended_UNIX_Code_Packed_Format_for_Japanese" in length 45.
        // we therefore restrict the length here to be 64 which is a power of 2
        // number that is longer than 45.
        constexpr माप_प्रकार k_Max_Charset_लंबाई = 64;
        अगर (length > k_Max_Charset_लंबाई) {
           err = U_ILLEGAL_ARGUMENT_ERROR; /* malformed keyword name */
           वापस;
        }
        अगर (length > 0) {
            tag.append(tmpLocaleID.तथ्य(), static_cast<int32_t>(length), err);
            tmpLocaleID.remove_prefix(length);
        }
    }

    /* Scan ahead to next '@' and determine if it is followed by '=' and/or ';'
       After this, tmpLocaleID either starts at '@' or is empty. */
    अगर (स्थिर अक्षर* start = locale_getKeywordsStart(tmpLocaleID); start != nullptr) {
        अगर (start > tmpLocaleID.तथ्य()) {
            tmpLocaleID.remove_prefix(start - tmpLocaleID.तथ्य());
        }
        keywordAssign = tmpLocaleID.find('=');
        separatorIndicator = tmpLocaleID.find(';');
    } अन्यथा {
        tmpLocaleID = {};
    }

    /* Copy POSIX-style variant, if any [mr@FOO] */
    अगर (!OPTION_SET(options, _ULOC_CANONICALIZE) &&
        !tmpLocaleID.empty() && keywordAssign == std::string_view::npos) {
        tag.append(tmpLocaleID, err);
        tmpLocaleID = {};
    }

    अगर (OPTION_SET(options, _ULOC_CANONICALIZE)) {
        /* Handle @FOO variant if @ is present and not followed by = */
        अगर (!tmpLocaleID.empty() && keywordAssign == std::string_view::npos) {
            /* Add missing '_' if needed */
            अगर (field_गिनती < 2 || (field_गिनती < 3 && !script.isEmpty())) {
                करो {
                    tag.append('_', err);
                    ++field_गिनती;
                } जबतक(field_गिनती<2);
            }

            CharStringByteSink s(&tag);
            std::string_view sub = tmpLocaleID;
            sub.remove_prefix(1);
            _getVariant(sub, '@', &s, !variant.isEmpty(), err);
            अगर (U_FAILURE(err)) { वापस; }
        }

        /* Look up the ID in the canonicalization map */
        क्रम (j=0; j<UPRV_LENGTHOF(CANONICALIZE_MAP); j++) {
            StringPiece id(CANONICALIZE_MAP[j].id);
            अगर (tag == id) {
                अगर (id.empty() && !tmpLocaleID.empty()) {
                    अवरोध; /* Don't remap "" if keywords present */
                }
                tag.सफा();
                tag.append(CANONICALIZE_MAP[j].canonicalID, err);
                अवरोध;
            }
        }
    }

    sink.Append(tag.तथ्य(), tag.length());

    अगर (!OPTION_SET(options, _ULOC_STRIP_KEYWORDS)) {
        अगर (!tmpLocaleID.empty() && keywordAssign != std::string_view::npos &&
            (separatorIndicator == std::string_view::npos || separatorIndicator > keywordAssign)) {
            sink.Append("@", 1);
            ++field_गिनती;
            tmpLocaleID.remove_prefix(1);
            ulocimp_getKeywords(tmpLocaleID, '@', sink, true, err);
        }
    }
}

}  // namespace

/* ### ID parsing API **************************************************/

U_CAPI int32_t  U_EXPORT2
uloc_getParent(स्थिर अक्षर*    localeID,
               अक्षर* parent,
               int32_t parentCapacity,
               UErrorCode* err)
{
    वापस ByteSinkUtil::viaByteSinkToTerminatedChars(
        parent, parentCapacity,
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getParent(localeID, sink, status);
        },
        *err);
}

U_EXPORT CharString
ulocimp_getParent(स्थिर अक्षर* localeID,
                  UErrorCode& err)
{
    वापस ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getParent(localeID, sink, status);
        },
        err);
}

U_EXPORT व्योम
ulocimp_getParent(स्थिर अक्षर* localeID,
                  icu::ByteSink& sink,
                  UErrorCode& err)
{
    अगर (U_FAILURE(err)) { वापस; }

    स्थिर अक्षर *lastUnderscore;
    int32_t i;

    अगर (localeID == nullptr)
        localeID = uloc_getDefault();

    lastUnderscore=uprv_strrchr(localeID, '_');
    अगर(lastUnderscore!=nullptr) {
        i = static_cast<int32_t>(lastUnderscore - localeID);
    } अन्यथा {
        i=0;
    }

    अगर (i > 0) {
        अगर (uprv_strnicmp(localeID, "und_", 4) == 0) {
            localeID += 3;
            i -= 3;
        }
        sink.Append(localeID, i);
    }
}

U_CAPI int32_t U_EXPORT2
uloc_getLanguage(स्थिर अक्षर*    localeID,
         अक्षर* language,
         int32_t languageCapacity,
         UErrorCode* err)
{
    अगर (localeID == nullptr) {
        localeID = uloc_getDefault();
    }

    /* uloc_getLanguage will return a 2 character iso-639 code if one exists. *CWB*/
    वापस ByteSinkUtil::viaByteSinkToTerminatedChars(
        language, languageCapacity,
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getSubtags(
                    localeID,
                    &sink,
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr,
                    status);
        },
        *err);
}

U_CAPI int32_t U_EXPORT2
uloc_getScript(स्थिर अक्षर*    localeID,
         अक्षर* script,
         int32_t scriptCapacity,
         UErrorCode* err)
{
    अगर (localeID == nullptr) {
        localeID = uloc_getDefault();
    }

    वापस ByteSinkUtil::viaByteSinkToTerminatedChars(
        script, scriptCapacity,
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getSubtags(
                    localeID,
                    nullptr,
                    &sink,
                    nullptr,
                    nullptr,
                    nullptr,
                    status);
        },
        *err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getCountry(स्थिर अक्षर* localeID,
            अक्षर* country,
            int32_t countryCapacity,
            UErrorCode* err)
{
    अगर (localeID == nullptr) {
        localeID = uloc_getDefault();
    }

    वापस ByteSinkUtil::viaByteSinkToTerminatedChars(
        country, countryCapacity,
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getSubtags(
                    localeID,
                    nullptr,
                    nullptr,
                    &sink,
                    nullptr,
                    nullptr,
                    status);
        },
        *err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getVariant(स्थिर अक्षर* localeID,
                अक्षर* variant,
                int32_t variantCapacity,
                UErrorCode* err)
{
    अगर (localeID == nullptr) {
        localeID = uloc_getDefault();
    }

    वापस ByteSinkUtil::viaByteSinkToTerminatedChars(
        variant, variantCapacity,
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getSubtags(
                    localeID,
                    nullptr,
                    nullptr,
                    nullptr,
                    &sink,
                    nullptr,
                    status);
        },
        *err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getName(स्थिर अक्षर* localeID,
             अक्षर* name,
             int32_t nameCapacity,
             UErrorCode* err)
{
    अगर (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    वापस ByteSinkUtil::viaByteSinkToTerminatedChars(
        name, nameCapacity,
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getName(localeID, sink, status);
        },
        *err);
}

U_EXPORT CharString
ulocimp_getName(std::string_view localeID,
                UErrorCode& err)
{
    वापस ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getName(localeID, sink, status);
        },
        err);
}

U_EXPORT व्योम
ulocimp_getName(std::string_view localeID,
                ByteSink& sink,
                UErrorCode& err)
{
    _canonicalize(localeID, sink, 0, err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getBaseName(स्थिर अक्षर* localeID,
                 अक्षर* name,
                 int32_t nameCapacity,
                 UErrorCode* err)
{
    अगर (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    वापस ByteSinkUtil::viaByteSinkToTerminatedChars(
        name, nameCapacity,
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getBaseName(localeID, sink, status);
        },
        *err);
}

U_EXPORT CharString
ulocimp_getBaseName(std::string_view localeID,
                    UErrorCode& err)
{
    वापस ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getBaseName(localeID, sink, status);
        },
        err);
}

U_EXPORT व्योम
ulocimp_getBaseName(std::string_view localeID,
                    ByteSink& sink,
                    UErrorCode& err)
{
    _canonicalize(localeID, sink, _ULOC_STRIP_KEYWORDS, err);
}

U_CAPI int32_t  U_EXPORT2
uloc_canonicalize(स्थिर अक्षर* localeID,
                  अक्षर* name,
                  int32_t nameCapacity,
                  UErrorCode* err)
{
    अगर (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    वापस ByteSinkUtil::viaByteSinkToTerminatedChars(
        name, nameCapacity,
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_canonicalize(localeID, sink, status);
        },
        *err);
}

U_EXPORT CharString
ulocimp_canonicalize(std::string_view localeID,
                     UErrorCode& err)
{
    वापस ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_canonicalize(localeID, sink, status);
        },
        err);
}

U_EXPORT व्योम
ulocimp_canonicalize(std::string_view localeID,
                     ByteSink& sink,
                     UErrorCode& err)
{
    _canonicalize(localeID, sink, _ULOC_CANONICALIZE, err);
}

U_CAPI स्थिर अक्षर*  U_EXPORT2
uloc_getISO3Language(स्थिर अक्षर* localeID)
{
    UErrorCode err = U_ZERO_ERROR;

    अगर (localeID == nullptr)
    {
        localeID = uloc_getDefault();
    }
    CharString lang = ulocimp_getLanguage(localeID, err);
    अगर (U_FAILURE(err))
        वापस "";
    std::optional<int16_t> offset = find_सूचकांक_2(LANGUAGES, lang.तथ्य());
    वापस offset.has_value() ? LANGUAGES_3[*offset] : "";
}

U_CAPI स्थिर अक्षर*  U_EXPORT2
uloc_getISO3Country(स्थिर अक्षर* localeID)
{
    UErrorCode err = U_ZERO_ERROR;

    अगर (localeID == nullptr)
    {
        localeID = uloc_getDefault();
    }
    CharString cntry = ulocimp_getRegion(localeID, err);
    अगर (U_FAILURE(err))
        वापस "";
    std::optional<int16_t> offset = find_सूचकांक_2(COUNTRIES, cntry.तथ्य());
    वापस offset.has_value() ? COUNTRIES_3[*offset] : "";
}

U_CAPI uint32_t  U_EXPORT2
uloc_getLCID(स्थिर अक्षर* localeID)
{
    UErrorCode status = U_ZERO_ERROR;
    uint32_t   lcid = 0;

    /* Check for incomplete id. */
    अगर (!localeID || uprv_strlen(localeID) < 2) {
        वापस 0;
    }

    // First, attempt Windows platform lookup if available, but fall
    // through to catch any special cases (ICU vs Windows name differences).
    lcid = uprv_convertToLCIDPlatform(localeID, &status);
    अगर (U_FAILURE(status)) {
        वापस 0;
    }
    अगर (lcid > 0) {
        // Windows found an LCID, return that
        वापस lcid;
    }

    CharString langID = ulocimp_getLanguage(localeID, status);
    अगर (U_FAILURE(status)) {
        वापस 0;
    }

    अगर (uprv_strchr(localeID, '@')) {
        // uprv_convertToLCID does not support keywords other than collation.
        // Remove all keywords except collation.
        CharString collVal = ulocimp_get_Keyword_मूल्य(localeID, "collation", status);
        अगर (U_SUCCESS(status) && !collVal.isEmpty()) {
            CharString tmpLocaleID = ulocimp_getBaseName(localeID, status);
            ulocimp_set_Keyword_मूल्य("collation", collVal.toStringPiece(), tmpLocaleID, status);
            अगर (U_SUCCESS(status)) {
                वापस uprv_convertToLCID(langID.तथ्य(), tmpLocaleID.तथ्य(), &status);
            }
        }

        // fall through - all keywords are simply ignored
        status = U_ZERO_ERROR;
    }

    वापस uprv_convertToLCID(langID.तथ्य(), localeID, &status);
}

U_CAPI int32_t U_EXPORT2
uloc_getLocaleForLCID(uint32_t hostid, अक्षर *locale, int32_t localeCapacity,
                UErrorCode *status)
{
    वापस uprv_convertToPosix(hostid, locale, localeCapacity, status);
}

/* ### Default locale **************************************************/

U_CAPI स्थिर अक्षर*  U_EXPORT2
uloc_getDefault()
{
    वापस locale_get_default();
}

U_CAPI व्योम  U_EXPORT2
uloc_setDefault(स्थिर अक्षर*   newDefaultLocale,
             UErrorCode* err)
{
    अगर (U_FAILURE(*err))
        वापस;
    /* the error code isn't currently used for anything by this function*/

    /* propagate change to C++ */
    locale_set_default(newDefaultLocale);
}

/**
 * Returns a list of all 2-letter language codes defined in ISO 639.  This is a pointer
 * to an array of pointers to arrays of char.  All of these pointers are owned
 * by ICU-- do not delete them, and do not write through them.  The array is
 * terminated with a null pointer.
 */
U_CAPI स्थिर अक्षर* स्थिर*  U_EXPORT2
uloc_getISOLanguages()
{
    वापस LANGUAGES;
}

/**
 * Returns a list of all 2-letter country codes defined in ISO 639.  This is a
 * pointer to an array of pointers to arrays of char.  All of these pointers are
 * owned by ICU-- do not delete them, and do not write through them.  The array is
 * terminated with a null pointer.
 */
U_CAPI स्थिर अक्षर* स्थिर*  U_EXPORT2
uloc_getISOCountries()
{
    वापस COUNTRIES;
}

U_CAPI स्थिर अक्षर* U_EXPORT2
uloc_to_Unicode_Locale_कुंजी(स्थिर अक्षर* keyword U_LIFETIME_BOUND)
{
    अगर (keyword == nullptr || *keyword == '\0') { वापस nullptr; }
    std::optional<std::string_view> परिणाम = ulocimp_to_Bcp_कुंजी_With_Fallback(keyword);
    वापस परिणाम.has_value() ? परिणाम->तथ्य() : nullptr;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_to_Bcp_कुंजी_With_Fallback(std::string_view keyword U_LIFETIME_BOUND)
{
    std::optional<std::string_view> bcp_कुंजी = ulocimp_to_Bcp_कुंजी(keyword);
    अगर (!bcp_कुंजी.has_value() &&
        ultag_is_Unicode_Locale_कुंजी(keyword.तथ्य(), static_cast<int32_t>(keyword.size()))) {
        // unknown keyword, but syntax is fine..
        वापस keyword;
    }
    वापस bcp_कुंजी;
}

U_CAPI स्थिर अक्षर* U_EXPORT2
uloc_toUnicodeLocaleType(स्थिर अक्षर* keyword, स्थिर अक्षर* value U_LIFETIME_BOUND)
{
    अगर (keyword == nullptr || *keyword == '\0' ||
        value == nullptr || *value == '\0') { वापस nullptr; }
    std::optional<std::string_view> परिणाम = ulocimp_toBcpTypeWithFallback(keyword, value);
    वापस परिणाम.has_value() ? परिणाम->तथ्य() : nullptr;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpTypeWithFallback(std::string_view keyword, std::string_view value U_LIFETIME_BOUND)
{
    std::optional<std::string_view> bcpType = ulocimp_toBcpType(keyword, value);
    अगर (!bcpType.has_value() &&
        ultag_isUnicodeLocaleType(value.तथ्य(), static_cast<int32_t>(value.size()))) {
        // unknown keyword, but syntax is fine..
        वापस value;
    }
    वापस bcpType;
}

namespace {

bool
is_Well_Formed_Legacy_कुंजी(std::string_view कुंजी)
{
    वापस std::all_of(कुंजी.begin(), कुंजी.end(), UPRV_ISALPHANUM);
}

bool
isWellFormedLegacyType(std::string_view legacyType)
{
    int32_t alpha_अंक_Len = 0;
    क्रम (अक्षर c : legacyType) {
        अगर (c == '_' || c == '/' || c == '-') {
            अगर (alpha_अंक_Len == 0) {
                वापस false;
            }
            alpha_अंक_Len = 0;
        } अन्यथा अगर (UPRV_ISALPHANUM(c)) {
            alpha_अंक_Len++;
        } अन्यथा {
            वापस false;
        }
    }
    वापस alpha_अंक_Len != 0;
}

}  // namespace

U_CAPI स्थिर अक्षर* U_EXPORT2
uloc_to_Legacy_कुंजी(स्थिर अक्षर* keyword U_LIFETIME_BOUND)
{
    अगर (keyword == nullptr || *keyword == '\0') { वापस nullptr; }
    std::optional<std::string_view> परिणाम = ulocimp_to_Legacy_कुंजी_With_Fallback(keyword);
    वापस परिणाम.has_value() ? परिणाम->तथ्य() : nullptr;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_to_Legacy_कुंजी_With_Fallback(std::string_view keyword U_LIFETIME_BOUND)
{
    std::optional<std::string_view> legacy_कुंजी_2 = ulocimp_to_Legacy_कुंजी(keyword);
    अगर (!legacy_कुंजी_2.has_value() && is_Well_Formed_Legacy_कुंजी(keyword)) {
        // Checks if the specified locale key is well-formed with the legacy locale syntax.
        //
        // Note:
        //  LDML/CLDR provides some definition of keyword syntax in
        //  * http://www.unicode.org/reports/tr35/#Unicode_locale_identifier and
        //  * http://www.unicode.org/reports/tr35/#Old_Locale_Extension_Syntax
        //  Keys can only consist of [0-9a-zA-Z].
        वापस keyword;
    }
    वापस legacy_कुंजी_2;
}

U_CAPI स्थिर अक्षर* U_EXPORT2
uloc_toLegacyType(स्थिर अक्षर* keyword, स्थिर अक्षर* value U_LIFETIME_BOUND)
{
    अगर (keyword == nullptr || *keyword == '\0' ||
        value == nullptr || *value == '\0') { वापस nullptr; }
    std::optional<std::string_view> परिणाम = ulocimp_toLegacyTypeWithFallback(keyword, value);
    वापस परिणाम.has_value() ? परिणाम->तथ्य() : nullptr;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyTypeWithFallback(std::string_view keyword, std::string_view value U_LIFETIME_BOUND)
{
    std::optional<std::string_view> legacyType = ulocimp_toLegacyType(keyword, value);
    अगर (!legacyType.has_value() && isWellFormedLegacyType(value)) {
        // Checks if the specified locale type is well-formed with the legacy locale syntax.
        //
        // Note:
        //  LDML/CLDR provides some definition of keyword syntax in
        //  * http://www.unicode.org/reports/tr35/#Unicode_locale_identifier and
        //  * http://www.unicode.org/reports/tr35/#Old_Locale_Extension_Syntax
        //  Values (types) can only consist of [0-9a-zA-Z], plus for legacy values
        //  we allow [/_-+] in the middle (e.g. "Etc/GMT+1", "Asia/Tel_Aviv")
        वापस value;
    }
    वापस legacyType;
}

/*eof*/
