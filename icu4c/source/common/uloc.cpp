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

#ضمن <algorithm>
#ضمن <optional>
#ضمن <string_view>

#ضمن "unicode/bytestream.h"
#ضمن "unicode/errorcode.h"
#ضمن "unicode/stringpiece.h"
#ضمن "unicode/utypes.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/uloc.h"

#ضمن "bytesinkutil.h"
#ضمن "putilimp.h"
#ضمن "ustr_imp.h"
#ضمن "ulocimp.h"
#ضمن "umutex.h"
#ضمن "cstring.h"
#ضمن "cmemory.h"
#ضمن "locmap.h"
#ضمن "uarrsort.h"
#ضمن "uenumimp.h"
#ضمن "uassert.h"
#ضمن "charstr.h"

U_NAMESPACE_USE

/* ### Declarations **************************************************/

/* Locale stuff from locid.cpp */
U_CFUNC فراغ locale_set_default(ثابت حرف *id);
U_CFUNC ثابت حرف *locale_get_default();

نطاق {

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
constexpr ثابت حرف* LANGUAGES[] = {
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

constexpr ثابت حرف* DEPRECATED_LANGUAGES[]={
    "in", "iw", "ji", "jw", "mo", nullptr, nullptr
};
constexpr ثابت حرف* REPLACEMENT_LANGUAGES[]={
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
constexpr ثابت حرف* LANGUAGES_3[] = {
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
constexpr ثابت حرف* COUNTRIES[] = {
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

constexpr ثابت حرف* DEPRECATED_COUNTRIES[] = {
    "AN", "BU", "CS", "DD", "DY", "FX", "HV", "NH", "RH", "SU", "TP", "UK", "VD", "YD", "YU", "ZR", nullptr, nullptr /* deprecated country list */
};
constexpr ثابت حرف* REPLACEMENT_COUNTRIES[] = {
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
constexpr ثابت حرف* COUNTRIES_3[] = {
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

تعريفنوع بنية CanonicalizationMap {
    ثابت حرف *id;          /* input ID */
    ثابت حرف *canonicalID; /* canonicalized output ID */
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
int32_t getShortestSubtagLength(std::string_view localeID) {
    int32_t localeIDLength = static_cast<int32_t>(localeID.length());
    int32_t length = localeIDLength;
    int32_t tmpLength = 0;
    int32_t i;
    منطقي reset = 𐡒𐡔𐡈;

    لكل (i = 0; i < localeIDLength; i++) {
        𐡄𐡍 (localeID[i] != '_' && localeID[i] != '-') {
            𐡄𐡍 (reset) {
                tmpLength = 0;
                reset = خطأ;
            }
            tmpLength++;
        } وإلا {
            𐡄𐡍 (tmpLength != 0 && tmpLength < length) {
                length = tmpLength;
            }
            reset = 𐡒𐡔𐡈;
        }
    }

    أرجع length;
}
/* Test if the locale id has BCP47 u extension and does not have '@' */
مضمن منطقي _hasBCP47Extension(std::string_view id) {
    أرجع id.find('@') == std::string_view::npos && getShortestSubtagLength(id) == 1;
}

/* ### Keywords **************************************************/
مضمن منطقي UPRV_ISDIGIT(حرف c) { أرجع c >= '0' && c <= '9'; }
مضمن منطقي UPRV_ISALPHANUM(حرف c) { أرجع uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }
/* Punctuation/symbols allowed in legacy key values */
مضمن منطقي UPRV_OK_VALUE_PUNCTUATION(حرف c) { أرجع c == '_' || c == '-' || c == '+' || c == '/'; }

}  // namespace

#define ULOC_KEYWORD_BUFFER_LEN 25
#define ULOC_MAX_NO_KEYWORDS 25

U_CAPI ثابت حرف* U_EXPORT2
locale_getKeywordsStart(std::string_view localeID U_LIFETIME_BOUND) {
    𐡄𐡍 (size_t pos = localeID.find('@'); pos != std::string_view::npos) {
        أرجع localeID.data() + pos;
    }
#𐡄𐡍 (U_CHARSET_FAMILY == U_EBCDIC_FAMILY)
    وإلا {
        /* We do this because the @ sign is variant, and the @ sign used on one
        EBCDIC machine won't be compiled the same way on other EBCDIC based
        machines. */
        ساكن ثابت uint8_t ebcdicSigns[] = { 0x7C, 0x44, 0x66, 0x80, 0xAC, 0xAE, 0xAF, 0xB5, 0xEC, 0xEF, 0x00 };
        ثابت uint8_t *charToFind = ebcdicSigns;
        طالما(*charToFind) {
            𐡄𐡍 (size_t pos = localeID.find(*charToFind); pos != std::string_view::npos) {
                أرجع localeID.data() + pos;
            }
            charToFind++;
        }
    }
#endif
    أرجع nullptr;
}

نطاق {

/**
 * @param keywordName incoming name to be canonicalized
 * @param status return status (keyword too long)
 * @return the keyword name
 */
CharString locale_canonKeywordName(std::string_view keywordName, UErrorCode& status)
{
  𐡄𐡍 (U_FAILURE(status)) { أرجع {}; }
  CharString result;

  لكل (حرف c : keywordName) {
    𐡄𐡍 (!UPRV_ISALPHANUM(c)) {
      status = U_ILLEGAL_ARGUMENT_ERROR; /* malformed keyword name */
      أرجع {};
    }
    result.append(uprv_tolower(c), status);
  }
  𐡄𐡍 (result.isEmpty()) {
    status = U_ILLEGAL_ARGUMENT_ERROR; /* empty keyword name */
    أرجع {};
  }

  أرجع result;
}

تعريفنوع بنية {
    حرف keyword[ULOC_KEYWORD_BUFFER_LEN];
    int32_t keywordLen;
    ثابت حرف *valueStart;
    int32_t valueLen;
} KeywordStruct;

int32_t U_CALLCONV
compareKeywordStructs(ثابت فراغ * /*context*/, ثابت فراغ *left, ثابت فراغ *right) {
    ثابت حرف* leftString = static_cast<ثابت KeywordStruct*>(left)->keyword;
    ثابت حرف* rightString = static_cast<ثابت KeywordStruct*>(right)->keyword;
    أرجع uprv_strcmp(leftString, rightString);
}

}  // namespace

U_EXPORT CharString
ulocimp_getKeywords(std::string_view localeID,
                    حرف prev,
                    منطقي valuesToo,
                    UErrorCode& status)
{
    أرجع ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getKeywords(localeID,
                                prev,
                                sink,
                                valuesToo,
                                status);
        },
        status);
}

U_EXPORT فراغ
ulocimp_getKeywords(std::string_view localeID,
                    حرف prev,
                    ByteSink& sink,
                    منطقي valuesToo,
                    UErrorCode& status)
{
    𐡄𐡍 (U_FAILURE(status)) { أرجع; }

    KeywordStruct keywordList[ULOC_MAX_NO_KEYWORDS];

    int32_t maxKeywords = ULOC_MAX_NO_KEYWORDS;
    int32_t numKeywords = 0;
    size_t equalSign = std::string_view::npos;
    size_t semicolon = std::string_view::npos;
    int32_t i = 0, j, n;

    𐡄𐡍(prev == '@') { /* start of keyword definition */
        /* we will grab pairs, trim spaces, lowercase keywords, sort and return */
        𐡏𐡁𐡃 {
            منطقي duplicate = خطأ;
            /* skip leading spaces */
            طالما (!localeID.empty() && localeID.front() == ' ') {
                localeID.remove_prefix(1);
            }
            𐡄𐡍 (localeID.empty()) { /* handle trailing "; " */
                اكسر;
            }
            𐡄𐡍(numKeywords == maxKeywords) {
                status = U_INTERNAL_PROGRAM_ERROR;
                أرجع;
            }
            equalSign = localeID.find('=');
            semicolon = localeID.find(';');
            /* lack of '=' [foo@currency] is illegal */
            /* ';' before '=' [foo@currency;collation=pinyin] is illegal */
            𐡄𐡍 (equalSign == std::string_view::npos ||
                (semicolon != std::string_view::npos && semicolon < equalSign)) {
                status = U_INVALID_FORMAT_ERROR;
                أرجع;
            }
            /* zero-length keyword is an error. */
            𐡄𐡍 (equalSign == 0) {
                status = U_INVALID_FORMAT_ERROR;
                أرجع;
            }
            /* need to normalize both keyword and keyword name */
            𐡄𐡍 (equalSign >= ULOC_KEYWORD_BUFFER_LEN) {
                /* keyword name too long for internal buffer */
                status = U_INTERNAL_PROGRAM_ERROR;
                أرجع;
            }
            لكل (i = 0, n = 0; static_cast<size_t>(i) < equalSign; ++i) {
                𐡄𐡍 (localeID[i] != ' ') {
                    keywordList[numKeywords].keyword[n++] = uprv_tolower(localeID[i]);
                }
            }

            keywordList[numKeywords].keyword[n] = 0;
            keywordList[numKeywords].keywordLen = n;
            /* now grab the value part. First we skip the '=' */
            equalSign++;
            /* then we leading spaces */
            طالما (equalSign < localeID.length() && localeID[equalSign] == ' ') {
                equalSign++;
            }

            /* Premature end or zero-length value */
            𐡄𐡍 (equalSign == localeID.length() || equalSign == semicolon) {
                status = U_INVALID_FORMAT_ERROR;
                أرجع;
            }

            keywordList[numKeywords].valueStart = localeID.data() + equalSign;

            std::string_view value = localeID;
            𐡄𐡍 (semicolon != std::string_view::npos) {
                value.remove_suffix(value.length() - semicolon);
                localeID.remove_prefix(semicolon + 1);
            } وإلا {
                localeID = {};
            }
            value.remove_prefix(equalSign);
            𐡄𐡍 (size_t last = value.find_last_not_of(' '); last != std::string_view::npos) {
                value.remove_suffix(value.length() - last - 1);
            }
            keywordList[numKeywords].valueLen = static_cast<int32_t>(value.length());

            /* If this is a duplicate keyword, then ignore it */
            لكل (j=0; j<numKeywords; ++j) {
                𐡄𐡍 (uprv_strcmp(keywordList[j].keyword, keywordList[numKeywords].keyword) == 0) {
                    duplicate = 𐡒𐡔𐡈;
                    اكسر;
                }
            }
            𐡄𐡍 (!duplicate) {
                ++numKeywords;
            }
        } طالما (!localeID.empty());

        /* now we have a list of keywords */
        /* we need to sort it */
        uprv_sortArray(keywordList, numKeywords, حجم(KeywordStruct), compareKeywordStructs, nullptr, خطأ, &status);

        /* Now construct the keyword part */
        لكل(i = 0; i < numKeywords; i++) {
            sink.Append(keywordList[i].keyword, keywordList[i].keywordLen);
            𐡄𐡍(valuesToo) {
                sink.Append("=", 1);
                sink.Append(keywordList[i].valueStart, keywordList[i].valueLen);
                𐡄𐡍(i < numKeywords - 1) {
                    sink.Append(";", 1);
                }
            } وإلا {
                sink.Append("\0", 1);
            }
        }
    }
}

U_CAPI int32_t U_EXPORT2
uloc_getKeywordValue(ثابت حرف* localeID,
                     ثابت حرف* keywordName,
                     حرف* buffer, int32_t bufferCapacity,
                     UErrorCode* status)
{
    𐡄𐡍 (U_FAILURE(*status)) { أرجع 0; }
    𐡄𐡍 (keywordName == nullptr || *keywordName == '\0') {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }
    أرجع ByteSinkUtil::viaByteSinkToTerminatedChars(
        buffer, bufferCapacity,
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getKeywordValue(localeID, keywordName, sink, status);
        },
        *status);
}

U_EXPORT CharString
ulocimp_getKeywordValue(ثابت حرف* localeID,
                        std::string_view keywordName,
                        UErrorCode& status)
{
    أرجع ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getKeywordValue(localeID, keywordName, sink, status);
        },
        status);
}

U_EXPORT فراغ
ulocimp_getKeywordValue(ثابت حرف* localeID,
                        std::string_view keywordName,
                        icu::ByteSink& sink,
                        UErrorCode& status)
{
    𐡄𐡍 (U_FAILURE(status)) { أرجع; }

    𐡄𐡍 (localeID == nullptr || keywordName.empty()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }

    ثابت حرف* startSearchHere = nullptr;
    ثابت حرف* nextSeparator = nullptr;

    CharString tempBuffer;
    ثابت حرف* tmpLocaleID;

    CharString canonKeywordName = locale_canonKeywordName(keywordName, status);
    𐡄𐡍 (U_FAILURE(status)) {
      أرجع;
    }

    𐡄𐡍 (localeID != nullptr && _hasBCP47Extension(localeID)) {
        tempBuffer = ulocimp_forLanguageTag(localeID, -1, nullptr, status);
        tmpLocaleID = U_SUCCESS(status) && !tempBuffer.isEmpty() ? tempBuffer.data() : localeID;
    } وإلا {
        tmpLocaleID=localeID;
    }

    startSearchHere = locale_getKeywordsStart(tmpLocaleID);
    𐡄𐡍(startSearchHere == nullptr) {
        /* no keywords, return at once */
        أرجع;
    }

    /* find the first keyword */
    طالما(startSearchHere) {
        ثابت حرف* keyValueTail;

        startSearchHere++; /* skip @ or ; */
        nextSeparator = uprv_strchr(startSearchHere, '=');
        𐡄𐡍(!nextSeparator) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* key must have =value */
            أرجع;
        }
        /* strip leading & trailing spaces (TC decided to tolerate these) */
        طالما(*startSearchHere == ' ') {
            startSearchHere++;
        }
        keyValueTail = nextSeparator;
        طالما (keyValueTail > startSearchHere && *(keyValueTail-1) == ' ') {
            keyValueTail--;
        }
        /* now keyValueTail points to first char after the keyName */
        /* copy & normalize keyName from locale */
        𐡄𐡍 (startSearchHere == keyValueTail) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* empty keyword name in passed-in locale */
            أرجع;
        }
        CharString localeKeywordName;
        طالما (startSearchHere < keyValueTail) {
          𐡄𐡍 (!UPRV_ISALPHANUM(*startSearchHere)) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* malformed keyword name */
            أرجع;
          }
          localeKeywordName.append(uprv_tolower(*startSearchHere++), status);
        }
        𐡄𐡍 (U_FAILURE(status)) {
            أرجع;
        }

        startSearchHere = uprv_strchr(nextSeparator, ';');

        𐡄𐡍 (canonKeywordName == localeKeywordName) {
             /* current entry matches the keyword. */
           nextSeparator++; /* skip '=' */
            /* First strip leading & trailing spaces (TC decided to tolerate these) */
            طالما(*nextSeparator == ' ') {
              nextSeparator++;
            }
            keyValueTail = (startSearchHere)? startSearchHere: nextSeparator + uprv_strlen(nextSeparator);
            طالما(keyValueTail > nextSeparator && *(keyValueTail-1) == ' ') {
              keyValueTail--;
            }
            /* Now copy the value, but check well-formedness */
            𐡄𐡍 (nextSeparator == keyValueTail) {
              status = U_ILLEGAL_ARGUMENT_ERROR; /* empty key value name in passed-in locale */
              أرجع;
            }
            طالما (nextSeparator < keyValueTail) {
              𐡄𐡍 (!UPRV_ISALPHANUM(*nextSeparator) && !UPRV_OK_VALUE_PUNCTUATION(*nextSeparator)) {
                status = U_ILLEGAL_ARGUMENT_ERROR; /* malformed key value */
                أرجع;
              }
              /* Should we lowercase value to return here? Tests expect as-is. */
              sink.Append(nextSeparator++, 1);
            }
            أرجع;
        }
    }
}

U_CAPI int32_t U_EXPORT2
uloc_setKeywordValue(ثابت حرف* keywordName,
                     ثابت حرف* keywordValue,
                     حرف* buffer, int32_t bufferCapacity,
                     UErrorCode* status)
{
    𐡄𐡍 (U_FAILURE(*status)) { أرجع 0; }

    𐡄𐡍 (keywordName == nullptr || *keywordName == 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    𐡄𐡍 (bufferCapacity <= 1) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    int32_t bufLen = (int32_t)uprv_strlen(buffer);
    𐡄𐡍(bufferCapacity<bufLen) {
        /* The capacity is less than the length?! Is this NUL terminated? */
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    حرف* keywords = const_cast<حرف*>(
        locale_getKeywordsStart({buffer, static_cast<std::string_view::size_type>(bufLen)}));
    int32_t baseLen = keywords == nullptr ? bufLen : keywords - buffer;
    // Remove -1 from the capacity so that this function can guarantee NUL termination.
    CheckedArrayByteSink sink(keywords == nullptr ? buffer + bufLen : keywords,
                              bufferCapacity - baseLen - 1);
    int32_t reslen = ulocimp_setKeywordValue(
        keywords == nullptr ? std::string_view() : keywords,
        keywordName,
        keywordValue == nullptr ? std::string_view() : keywordValue,
        sink,
        *status);

    𐡄𐡍 (U_FAILURE(*status)) {
        أرجع *status == U_BUFFER_OVERFLOW_ERROR ? reslen + baseLen : 0;
    }

    // See the documentation for this function, it's guaranteed to never
    // overflow the buffer but instead abort with BUFFER_OVERFLOW_ERROR.
    // In this case, nothing has been written to the sink, so it cannot have Overflowed().
    U_ASSERT(!sink.Overflowed());
    U_ASSERT(reslen >= 0);
    أرجع u_terminateChars(buffer, bufferCapacity, reslen + baseLen, status);
}

U_EXPORT فراغ
ulocimp_setKeywordValue(std::string_view keywordName,
                        std::string_view keywordValue,
                        CharString& localeID,
                        UErrorCode& status)
{
    𐡄𐡍 (U_FAILURE(status)) { أرجع; }
    std::string_view keywords;
    𐡄𐡍 (ثابت حرف* start = locale_getKeywordsStart(localeID.toStringPiece()); start != nullptr) {
        // This is safe because CharString::truncate() doesn't actually erase any
        // data, but simply sets the position for where new data will be written.
        int32_t size = start - localeID.data();
        keywords = localeID.toStringPiece();
        keywords.remove_prefix(size);
        localeID.truncate(size);
    }
    CharStringByteSink sink(&localeID);
    ulocimp_setKeywordValue(keywords, keywordName, keywordValue, sink, status);
}

U_EXPORT int32_t
ulocimp_setKeywordValue(std::string_view keywords,
                        std::string_view keywordName,
                        std::string_view keywordValue,
                        ByteSink& sink,
                        UErrorCode& status)
{
    𐡄𐡍 (U_FAILURE(status)) { أرجع 0; }

    /* TODO: sorting. removal. */
    int32_t needLen = 0;
    int32_t rc;
    CharString updatedKeysAndValues;
    منطقي handledInputKeyAndValue = خطأ;
    حرف keyValuePrefix = '@';

    𐡄𐡍 (status == U_STRING_NOT_TERMINATED_WARNING) {
        status = U_ZERO_ERROR;
    }
    𐡄𐡍 (keywordName.empty()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }
    CharString canonKeywordName = locale_canonKeywordName(keywordName, status);
    𐡄𐡍 (U_FAILURE(status)) {
        أرجع 0;
    }

    CharString canonKeywordValue;
    لكل (حرف c : keywordValue) {
        𐡄𐡍 (!UPRV_ISALPHANUM(c) && !UPRV_OK_VALUE_PUNCTUATION(c)) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* malformed key value */
            أرجع 0;
        }
        /* Should we force lowercase in value to set? */
        canonKeywordValue.append(c, status);
    }
    𐡄𐡍 (U_FAILURE(status)) {
        أرجع 0;
    }

    𐡄𐡍 (keywords.size() <= 1) {
        𐡄𐡍 (canonKeywordValue.isEmpty()) { /* no keywords = nothing to remove */
            U_ASSERT(status != U_STRING_NOT_TERMINATED_WARNING);
            أرجع 0;
        }

        needLen = 1 + canonKeywordName.length() + 1 + canonKeywordValue.length();
        int32_t capacity = 0;
        حرف* buffer = sink.GetAppendBuffer(
                needLen, needLen, nullptr, needLen, &capacity);
        𐡄𐡍 (capacity < needLen || buffer == nullptr) {
            status = U_BUFFER_OVERFLOW_ERROR;
            أرجع needLen; /* no change */
        }
        حرف* it = buffer;

        *it++ = '@';
        uprv_memcpy(it, canonKeywordName.data(), canonKeywordName.length());
        it += canonKeywordName.length();
        *it++ = '=';
        uprv_memcpy(it, canonKeywordValue.data(), canonKeywordValue.length());
        sink.Append(buffer, needLen);
        U_ASSERT(status != U_STRING_NOT_TERMINATED_WARNING);
        أرجع needLen;
    } /* end shortcut - no @ */

    /* search for keyword */
    لكل (size_t keywordStart = 0; keywordStart != std::string_view::npos;) {
        keywordStart++; /* skip @ or ; */
        size_t nextEqualsign = keywords.find('=', keywordStart);
        𐡄𐡍 (nextEqualsign == std::string_view::npos) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* key must have =value */
            أرجع 0;
        }
        /* strip leading & trailing spaces (TC decided to tolerate these) */
        طالما (keywordStart < keywords.size() && keywords[keywordStart] == ' ') {
            keywordStart++;
        }
        size_t keyValueTail = nextEqualsign;
        طالما (keyValueTail > keywordStart && keywords[keyValueTail - 1] == ' ') {
            keyValueTail--;
        }
        /* now keyValueTail points to first char after the keyName */
        /* copy & normalize keyName from locale */
        𐡄𐡍 (keywordStart == keyValueTail) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* empty keyword name in passed-in locale */
            أرجع 0;
        }
        CharString localeKeywordName;
        طالما (keywordStart < keyValueTail) {
            𐡄𐡍 (!UPRV_ISALPHANUM(keywords[keywordStart])) {
                status = U_ILLEGAL_ARGUMENT_ERROR; /* malformed keyword name */
                أرجع 0;
            }
            localeKeywordName.append(uprv_tolower(keywords[keywordStart++]), status);
        }
        𐡄𐡍 (U_FAILURE(status)) {
            أرجع 0;
        }

        size_t nextSeparator = keywords.find(';', nextEqualsign);

        /* start processing the value part */
        nextEqualsign++; /* skip '=' */
        /* First strip leading & trailing spaces (TC decided to tolerate these) */
        طالما (nextEqualsign < keywords.size() && keywords[nextEqualsign] == ' ') {
            nextEqualsign++;
        }
        keyValueTail = nextSeparator == std::string_view::npos ? keywords.size() : nextSeparator;
        طالما (keyValueTail > nextEqualsign && keywords[keyValueTail - 1] == ' ') {
            keyValueTail--;
        }
        𐡄𐡍 (nextEqualsign == keyValueTail) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* empty key value in passed-in locale */
            أرجع 0;
        }

        rc = uprv_strcmp(canonKeywordName.data(), localeKeywordName.data());
        𐡄𐡍(rc == 0) {
            /* Current entry matches the input keyword. Update the entry */
            𐡄𐡍 (!canonKeywordValue.isEmpty()) { /* updating a value */
                updatedKeysAndValues.append(keyValuePrefix, status);
                keyValuePrefix = ';'; /* for any subsequent key-value pair */
                updatedKeysAndValues.append(canonKeywordName, status);
                updatedKeysAndValues.append('=', status);
                updatedKeysAndValues.append(canonKeywordValue, status);
            } /* else removing this entry, don't emit anything */
            handledInputKeyAndValue = 𐡒𐡔𐡈;
        } وإلا {
           /* input keyword sorts earlier than current entry, add before current entry */
            𐡄𐡍 (rc < 0 && !canonKeywordValue.isEmpty() && !handledInputKeyAndValue) {
                /* insert new entry at this location */
                updatedKeysAndValues.append(keyValuePrefix, status);
                keyValuePrefix = ';'; /* for any subsequent key-value pair */
                updatedKeysAndValues.append(canonKeywordName, status);
                updatedKeysAndValues.append('=', status);
                updatedKeysAndValues.append(canonKeywordValue, status);
                handledInputKeyAndValue = 𐡒𐡔𐡈;
            }
            /* copy the current entry */
            updatedKeysAndValues.append(keyValuePrefix, status);
            keyValuePrefix = ';'; /* for any subsequent key-value pair */
            updatedKeysAndValues.append(localeKeywordName, status);
            updatedKeysAndValues.append('=', status);
            updatedKeysAndValues.append(keywords.data() + nextEqualsign,
                                        static_cast<int32_t>(keyValueTail - nextEqualsign), status);
        }
        𐡄𐡍 (nextSeparator == std::string_view::npos && !canonKeywordValue.isEmpty() && !handledInputKeyAndValue) {
            /* append new entry at the end, it sorts later than existing entries */
            updatedKeysAndValues.append(keyValuePrefix, status);
            /* skip keyValuePrefix update, no subsequent key-value pair */
            updatedKeysAndValues.append(canonKeywordName, status);
            updatedKeysAndValues.append('=', status);
            updatedKeysAndValues.append(canonKeywordValue, status);
            handledInputKeyAndValue = 𐡒𐡔𐡈;
        }
        keywordStart = nextSeparator;
    } /* end loop searching */

    /* Any error from updatedKeysAndValues.append above would be internal and not due to
     * problems with the passed-in locale. So if we did encounter problems with the
     * passed-in locale above, those errors took precedence and overrode any error
     * status from updatedKeysAndValues.append, and also caused a return of 0. If there
     * are errors here they are from updatedKeysAndValues.append; they do cause an
     * error return but the passed-in locale is unmodified and the original bufLen is
     * returned.
     */
    𐡄𐡍 (!handledInputKeyAndValue || U_FAILURE(status)) {
        /* if input key/value specified removal of a keyword not present in locale, or
         * there was an error in CharString.append, leave original locale alone. */
        U_ASSERT(status != U_STRING_NOT_TERMINATED_WARNING);
        // The sink is expected to be a buffer which already contains the full
        // locale string, so when it isn't going to be modified there's no need
        // to actually write any data to it, as the data is already there. Only
        // the first character needs to be overwritten (changing '\0' to '@').
        needLen = static_cast<int32_t>(keywords.size());
        int32_t capacity = 0;
        حرف* buffer = sink.GetAppendBuffer(
                needLen, needLen, nullptr, needLen, &capacity);
        𐡄𐡍 (capacity < needLen || buffer == nullptr) {
            status = U_BUFFER_OVERFLOW_ERROR;
        } وإلا {
            *buffer = '@';
            sink.Append(buffer, needLen);
        }
        أرجع needLen;
    }

    needLen = updatedKeysAndValues.length();
    // Check to see can we fit the updatedKeysAndValues, if not, return
    // U_BUFFER_OVERFLOW_ERROR without copy updatedKeysAndValues into it.
    // We do this because this API function does not behave like most others:
    // It promises never to set a U_STRING_NOT_TERMINATED_WARNING.
    // When the contents fits but without the terminating NUL, in this case we need to not change
    // the buffer contents and return with a buffer overflow error.
    𐡄𐡍 (needLen > 0) {
        int32_t capacity = 0;
        حرف* buffer = sink.GetAppendBuffer(
                needLen, needLen, nullptr, needLen, &capacity);
        𐡄𐡍 (capacity < needLen || buffer == nullptr) {
            status = U_BUFFER_OVERFLOW_ERROR;
            أرجع needLen;
        }
        uprv_memcpy(buffer, updatedKeysAndValues.data(), needLen);
        sink.Append(buffer, needLen);
    }
    U_ASSERT(status != U_STRING_NOT_TERMINATED_WARNING);
    أرجع needLen;
}

/* ### ID parsing implementation **************************************************/

نطاق {

مضمن منطقي _isPrefixLetter(حرف a) { أرجع a == 'x' || a == 'X' || a == 'i' || a == 'I'; }

/*returns true if one of the special prefixes is here (s=string)
  'x-' or 'i-' */
مضمن منطقي _isIDPrefix(std::string_view s) {
    أرجع s.size() >= 2 && _isPrefixLetter(s[0]) && _isIDSeparator(s[1]);
}

/* Dot terminates it because of POSIX form  where dot precedes the codepage
 * except for variant
 */
مضمن منطقي _isTerminator(حرف a) { أرجع a == '.' || a == '@'; }

مضمن منطقي _isBCP47Extension(std::string_view p) {
    أرجع p.size() >= 3 &&
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
std::optional<int16_t> _findIndex(ثابت حرف* ثابت* list, ثابت حرف* key)
{
    ثابت حرف* ثابت* anchor = list;
    int32_t pass = 0;

    /* Make two passes through two nullptr-terminated arrays at 'list' */
    طالما (pass++ < 2) {
        طالما (*list) {
            𐡄𐡍 (uprv_strcmp(key, *list) == 0) {
                أرجع static_cast<int16_t>(list - anchor);
            }
            list++;
        }
        ++list;     /* skip final nullptr *CWB*/
    }
    أرجع std::nullopt;
}

}  // namespace

U_CFUNC ثابت حرف*
uloc_getCurrentCountryID(ثابت حرف* oldID U_LIFETIME_BOUND) {
    std::optional<int16_t> offset = _findIndex(DEPRECATED_COUNTRIES, oldID);
    أرجع offset.has_value() ? REPLACEMENT_COUNTRIES[*offset] : oldID;
}
U_CFUNC ثابت حرف*
uloc_getCurrentLanguageID(ثابت حرف* oldID U_LIFETIME_BOUND) {
    std::optional<int16_t> offset = _findIndex(DEPRECATED_LANGUAGES, oldID);
    أرجع offset.has_value() ? REPLACEMENT_LANGUAGES[*offset] : oldID;
}

نطاق {

/*
 * the internal functions _getLanguage(), _getScript(), _getRegion(), _getVariant()
 * avoid duplicating code to handle the earlier locale ID pieces
 * in the functions for the later ones by
 * setting the *pEnd pointer to where they stopped parsing
 *
 * TODO try to use this in Locale
 */

size_t _getLanguage(std::string_view localeID, ByteSink* sink, UErrorCode& status) {
    size_t skip = 0;
    𐡄𐡍 (localeID.size() == 4 && uprv_strnicmp(localeID.data(), "root", 4) == 0) {
        skip = 4;
        localeID.remove_prefix(skip);
    } وإلا 𐡄𐡍 (localeID.size() >= 3 && uprv_strnicmp(localeID.data(), "und", 3) == 0 &&
               (localeID.size() == 3 ||
                localeID[3] == '-' ||
                localeID[3] == '_' ||
                localeID[3] == '@')) {
        skip = 3;
        localeID.remove_prefix(skip);
    }

    constexpr int32_t MAXLEN = ULOC_LANG_CAPACITY - 1;  // Minus NUL.

    /* if it starts with i- or x- then copy that prefix */
    size_t len = _isIDPrefix(localeID) ? 2 : 0;
    طالما (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len])) {
        𐡄𐡍 (len == MAXLEN) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            أرجع 0;
        }
        len++;
    }

    𐡄𐡍 (sink == nullptr || len == 0) { أرجع skip + len; }

    int32_t minCapacity = uprv_max(static_cast<int32_t>(len), 4);  // Minimum 3 letters plus NUL.
    حرف scratch[MAXLEN];
    int32_t capacity = 0;
    حرف* buffer = sink->GetAppendBuffer(
            minCapacity, minCapacity, scratch, UPRV_LENGTHOF(scratch), &capacity);

    لكل (size_t i = 0; i < len; ++i) {
        buffer[i] = uprv_tolower(localeID[i]);
    }
    𐡄𐡍 (localeID.size() >= 2 && _isIDSeparator(localeID[1])) {
        buffer[1] = '-';
    }

    𐡄𐡍 (len == 3) {
        /* convert 3 character code to 2 character code if possible *CWB*/
        U_ASSERT(capacity >= 4);
        buffer[3] = '\0';
        std::optional<int16_t> offset = _findIndex(LANGUAGES_3, buffer);
        𐡄𐡍 (offset.has_value()) {
            ثابت حرف* ثابت alias = LANGUAGES[*offset];
            sink->Append(alias, static_cast<int32_t>(uprv_strlen(alias)));
            أرجع skip + len;
        }
    }

    sink->Append(buffer, static_cast<int32_t>(len));
    أرجع skip + len;
}

size_t _getScript(std::string_view localeID, ByteSink* sink) {
    constexpr int32_t LENGTH = 4;

    size_t len = 0;
    طالما (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len]) &&
            uprv_isASCIILetter(localeID[len])) {
        𐡄𐡍 (len == LENGTH) { أرجع 0; }
        len++;
    }
    𐡄𐡍 (len != LENGTH) { أرجع 0; }

    𐡄𐡍 (sink == nullptr) { أرجع len; }

    حرف scratch[LENGTH];
    int32_t capacity = 0;
    حرف* buffer = sink->GetAppendBuffer(
            LENGTH, LENGTH, scratch, UPRV_LENGTHOF(scratch), &capacity);

    buffer[0] = uprv_toupper(localeID[0]);
    لكل (int32_t i = 1; i < LENGTH; ++i) {
        buffer[i] = uprv_tolower(localeID[i]);
    }

    sink->Append(buffer, LENGTH);
    أرجع len;
}

size_t _getRegion(std::string_view localeID, ByteSink* sink) {
    constexpr int32_t MINLEN = 2;
    constexpr int32_t MAXLEN = ULOC_COUNTRY_CAPACITY - 1;  // Minus NUL.

    size_t len = 0;
    طالما (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len])) {
        𐡄𐡍 (len == MAXLEN) { أرجع 0; }
        len++;
    }
    𐡄𐡍 (len < MINLEN) { أرجع 0; }

    𐡄𐡍 (sink == nullptr) { أرجع len; }

    حرف scratch[ULOC_COUNTRY_CAPACITY];
    int32_t capacity = 0;
    حرف* buffer = sink->GetAppendBuffer(
            ULOC_COUNTRY_CAPACITY,
            ULOC_COUNTRY_CAPACITY,
            scratch,
            UPRV_LENGTHOF(scratch),
            &capacity);

    لكل (size_t i = 0; i < len; ++i) {
        buffer[i] = uprv_toupper(localeID[i]);
    }

    𐡄𐡍 (len == 3) {
        /* convert 3 character code to 2 character code if possible *CWB*/
        U_ASSERT(capacity >= 4);
        buffer[3] = '\0';
        std::optional<int16_t> offset = _findIndex(COUNTRIES_3, buffer);
        𐡄𐡍 (offset.has_value()) {
            ثابت حرف* ثابت alias = COUNTRIES[*offset];
            sink->Append(alias, static_cast<int32_t>(uprv_strlen(alias)));
            أرجع len;
        }
    }

    sink->Append(buffer, static_cast<int32_t>(len));
    أرجع len;
}

/**
 * @param needSeparator if true, then add leading '_' if any variants
 * are added to 'variant'
 */
size_t
_getVariant(std::string_view localeID,
            حرف prev,
            ByteSink* sink,
            منطقي needSeparator,
            UErrorCode& status) {
    𐡄𐡍 (U_FAILURE(status) || localeID.empty()) أرجع 0;

    // Reasonable upper limit for variants
    // There are no strict limitation of the syntax of variant in the legacy
    // locale format. If the locale is constructed from unicode_locale_id
    // as defined in UTS35, then we know each unicode_variant_subtag
    // could have max length of 8 ((alphanum{5,8} | digit alphanum{3})
    // 179 would allow 20 unicode_variant_subtag with sep in the
    // unicode_locale_id
    // 8*20 + 1*(20-1) = 179
    constexpr int32_t MAX_VARIANTS_LENGTH = 179;

    /* get one or more variant tags and separate them with '_' */
    size_t index = 0;
    𐡄𐡍 (_isIDSeparator(prev)) {
        /* get a variant string after a '-' or '_' */
        لكل (std::string_view sub = localeID;;) {
            size_t next = sub.find_first_of(".@_-");
            // For historical reasons, a trailing separator is included in the variant.
            منطقي finished = next == std::string_view::npos || next + 1 == sub.length();
            size_t limit = finished ? sub.length() : next;
            index += limit;
            𐡄𐡍 (index > MAX_VARIANTS_LENGTH) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
                أرجع 0;
            }

            𐡄𐡍 (sink != nullptr) {
                𐡄𐡍 (needSeparator) {
                    sink->Append("_", 1);
                } وإلا {
                    needSeparator = 𐡒𐡔𐡈;
                }

                int32_t length = static_cast<int32_t>(limit);
                int32_t minCapacity = uprv_min(length, MAX_VARIANTS_LENGTH);
                حرف scratch[MAX_VARIANTS_LENGTH];
                int32_t capacity = 0;
                حرف* buffer = sink->GetAppendBuffer(
                        minCapacity, minCapacity, scratch, UPRV_LENGTHOF(scratch), &capacity);

                لكل (size_t i = 0; i < limit; ++i) {
                    buffer[i] = uprv_toupper(sub[i]);
                }
                sink->Append(buffer, length);
            }

            𐡄𐡍 (finished) { أرجع index; }
            sub.remove_prefix(next);
            𐡄𐡍 (_isTerminator(sub.front()) || _isBCP47Extension(sub)) { أرجع index; }
            sub.remove_prefix(1);
            index++;
        }
    }

    size_t skip = 0;
    /* if there is no variant tag after a '-' or '_' then look for '@' */
    𐡄𐡍 (prev == '@') {
        /* keep localeID */
    } وإلا 𐡄𐡍 (ثابت حرف* p = locale_getKeywordsStart(localeID); p != nullptr) {
        skip = 1 + p - localeID.data(); /* point after the '@' */
        localeID.remove_prefix(skip);
    } وإلا {
        أرجع 0;
    }
    لكل (; index < localeID.size() && !_isTerminator(localeID[index]); index++) {
        𐡄𐡍 (index >= MAX_VARIANTS_LENGTH) { // same as length > MAX_VARIANTS_LENGTH
            status = U_ILLEGAL_ARGUMENT_ERROR;
            أرجع 0;
        }
        𐡄𐡍 (needSeparator) {
            𐡄𐡍 (sink != nullptr) {
                sink->Append("_", 1);
            }
            needSeparator = خطأ;
        }
        𐡄𐡍 (sink != nullptr) {
            حرف c = uprv_toupper(localeID[index]);
            𐡄𐡍 (c == '-' || c == ',') c = '_';
            sink->Append(&c, 1);
        }
    }
    أرجع skip + index;
}

}  // namespace

U_EXPORT CharString
ulocimp_getLanguage(std::string_view localeID, UErrorCode& status) {
    أرجع ByteSinkUtil::viaByteSinkToCharString(
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
    أرجع ByteSinkUtil::viaByteSinkToCharString(
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
    أرجع ByteSinkUtil::viaByteSinkToCharString(
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
    أرجع ByteSinkUtil::viaByteSinkToCharString(
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

U_EXPORT فراغ
ulocimp_getSubtags(
        std::string_view localeID,
        CharString* language,
        CharString* script,
        CharString* region,
        CharString* variant,
        ثابت حرف** pEnd,
        UErrorCode& status) {
    𐡄𐡍 (U_FAILURE(status)) { أرجع; }

    std::optional<CharStringByteSink> languageSink;
    std::optional<CharStringByteSink> scriptSink;
    std::optional<CharStringByteSink> regionSink;
    std::optional<CharStringByteSink> variantSink;

    𐡄𐡍 (language != nullptr) { languageSink.emplace(language); }
    𐡄𐡍 (script != nullptr) { scriptSink.emplace(script); }
    𐡄𐡍 (region != nullptr) { regionSink.emplace(region); }
    𐡄𐡍 (variant != nullptr) { variantSink.emplace(variant); }

    ulocimp_getSubtags(
            localeID,
            languageSink.has_value() ? &*languageSink : nullptr,
            scriptSink.has_value() ? &*scriptSink : nullptr,
            regionSink.has_value() ? &*regionSink : nullptr,
            variantSink.has_value() ? &*variantSink : nullptr,
            pEnd,
            status);
}

U_EXPORT فراغ
ulocimp_getSubtags(
        std::string_view localeID,
        ByteSink* language,
        ByteSink* script,
        ByteSink* region,
        ByteSink* variant,
        ثابت حرف** pEnd,
        UErrorCode& status) {
    𐡄𐡍 (U_FAILURE(status)) { أرجع; }

    𐡄𐡍 (pEnd != nullptr) {
        *pEnd = localeID.data();
    } وإلا 𐡄𐡍 (language == nullptr &&
               script == nullptr &&
               region == nullptr &&
               variant == nullptr) {
        أرجع;
    }

    𐡄𐡍 (localeID.empty()) { أرجع; }

    منطقي hasRegion = خطأ;

    {
        size_t len = _getLanguage(localeID, language, status);
        𐡄𐡍 (U_FAILURE(status)) { أرجع; }
        𐡄𐡍 (len > 0) {
            localeID.remove_prefix(len);
        }
    }

    𐡄𐡍 (pEnd != nullptr) {
        *pEnd = localeID.data();
    } وإلا 𐡄𐡍 (script == nullptr &&
               region == nullptr &&
               variant == nullptr) {
        أرجع;
    }

    𐡄𐡍 (localeID.empty()) { أرجع; }

    𐡄𐡍 (_isIDSeparator(localeID.front())) {
        std::string_view sub = localeID;
        sub.remove_prefix(1);
        size_t len = _getScript(sub, script);
        𐡄𐡍 (len > 0) {
            localeID.remove_prefix(len + 1);
            𐡄𐡍 (pEnd != nullptr) { *pEnd = localeID.data(); }
        }
    }

    𐡄𐡍 ((region == nullptr && variant == nullptr && pEnd == nullptr) || localeID.empty()) { أرجع; }

    𐡄𐡍 (_isIDSeparator(localeID.front())) {
        std::string_view sub = localeID;
        sub.remove_prefix(1);
        size_t len = _getRegion(sub, region);
        𐡄𐡍 (len > 0) {
            hasRegion = 𐡒𐡔𐡈;
            localeID.remove_prefix(len + 1);
            𐡄𐡍 (pEnd != nullptr) { *pEnd = localeID.data(); }
        }
    }

    𐡄𐡍 ((variant == nullptr && pEnd == nullptr) || localeID.empty()) { أرجع; }

    منطقي hasVariant = خطأ;

    𐡄𐡍 (_isIDSeparator(localeID.front()) && !_isBCP47Extension(localeID)) {
        std::string_view sub = localeID;
        /* If there was no country ID, skip a possible extra IDSeparator */
        size_t skip = !hasRegion && localeID.size() > 1 && _isIDSeparator(localeID[1]) ? 2 : 1;
        sub.remove_prefix(skip);
        size_t len = _getVariant(sub, localeID[0], variant, خطأ, status);
        𐡄𐡍 (U_FAILURE(status)) { أرجع; }
        𐡄𐡍 (len > 0) {
            hasVariant = 𐡒𐡔𐡈;
            localeID.remove_prefix(skip + len);
            𐡄𐡍 (pEnd != nullptr) { *pEnd = localeID.data(); }
        }
    }

    𐡄𐡍 ((variant == nullptr && pEnd == nullptr) || localeID.empty()) { أرجع; }

    𐡄𐡍 (_isBCP47Extension(localeID)) {
        localeID.remove_prefix(2);
        constexpr حرف vaposix[] = "-va-posix";
        constexpr size_t length = حجم vaposix - 1;
        لكل (size_t next;; localeID.remove_prefix(next)) {
            next = localeID.find('-', 1);
            𐡄𐡍 (next == std::string_view::npos) { اكسر; }
            next = localeID.find('-', next + 1);
            منطقي finished = next == std::string_view::npos;
            std::string_view sub = localeID;
            𐡄𐡍 (!finished) { sub.remove_suffix(sub.length() - next); }

            𐡄𐡍 (sub.length() == length && uprv_strnicmp(sub.data(), vaposix, length) == 0) {
                𐡄𐡍 (variant != nullptr) {
                    𐡄𐡍 (hasVariant) { variant->Append("_", 1); }
                    constexpr حرف posix[] = "POSIX";
                    variant->Append(posix, حجم posix - 1);
                }
                𐡄𐡍 (pEnd != nullptr) { *pEnd = localeID.data() + length; }
            }

            𐡄𐡍 (finished) { اكسر; }
        }
    }
}

/* Keyword enumeration */

تعريفنوع بنية UKeywordsContext {
    حرف* keywords;
    حرف* current;
} UKeywordsContext;

U_CDECL_BEGIN

ساكن فراغ U_CALLCONV
uloc_kw_closeKeywords(UEnumeration *enumerator) {
    uprv_free(((UKeywordsContext *)enumerator->context)->keywords);
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

ساكن int32_t U_CALLCONV
uloc_kw_countKeywords(UEnumeration *en, UErrorCode * /*status*/) {
    حرف *kw = ((UKeywordsContext *)en->context)->keywords;
    int32_t result = 0;
    طالما(*kw) {
        result++;
        kw += uprv_strlen(kw)+1;
    }
    أرجع result;
}

ساكن ثابت حرف * U_CALLCONV
uloc_kw_nextKeyword(UEnumeration* en,
                    int32_t* resultLength,
                    UErrorCode* /*status*/) {
    ثابت حرف* result = ((UKeywordsContext *)en->context)->current;
    int32_t len = 0;
    𐡄𐡍(*result) {
        len = (int32_t)uprv_strlen(((UKeywordsContext *)en->context)->current);
        ((UKeywordsContext *)en->context)->current += len+1;
    } وإلا {
        result = nullptr;
    }
    𐡄𐡍 (resultLength) {
        *resultLength = len;
    }
    أرجع result;
}

ساكن فراغ U_CALLCONV
uloc_kw_resetKeywords(UEnumeration* en,
                      UErrorCode* /*status*/) {
    ((UKeywordsContext *)en->context)->current = ((UKeywordsContext *)en->context)->keywords;
}

U_CDECL_END


ساكن ثابت UEnumeration gKeywordsEnum = {
    nullptr,
    nullptr,
    uloc_kw_closeKeywords,
    uloc_kw_countKeywords,
    uenum_unextDefault,
    uloc_kw_nextKeyword,
    uloc_kw_resetKeywords
};

U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywordList(ثابت حرف *keywordList, int32_t keywordListSize, UErrorCode* status)
{
    𐡄𐡍 (U_FAILURE(*status)) { أرجع nullptr; }

    LocalMemory<UKeywordsContext> myContext;
    LocalMemory<UEnumeration> result;

    myContext.adoptInstead(static_cast<UKeywordsContext *>(uprv_malloc(حجم(UKeywordsContext))));
    result.adoptInstead(static_cast<UEnumeration *>(uprv_malloc(حجم(UEnumeration))));
    𐡄𐡍 (myContext.isNull() || result.isNull()) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    uprv_memcpy(result.getAlias(), &gKeywordsEnum, حجم(UEnumeration));
    myContext->keywords = static_cast<حرف *>(uprv_malloc(keywordListSize+1));
    𐡄𐡍 (myContext->keywords == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    uprv_memcpy(myContext->keywords, keywordList, keywordListSize);
    myContext->keywords[keywordListSize] = 0;
    myContext->current = myContext->keywords;
    result->context = myContext.orphan();
    أرجع result.orphan();
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywords(ثابت حرف* localeID,
                        UErrorCode* status)
{
    𐡄𐡍(status==nullptr || U_FAILURE(*status)) {
        أرجع nullptr;
    }

    CharString tempBuffer;
    ثابت حرف* tmpLocaleID;

    𐡄𐡍 (localeID != nullptr && _hasBCP47Extension(localeID)) {
        tempBuffer = ulocimp_forLanguageTag(localeID, -1, nullptr, *status);
        tmpLocaleID = U_SUCCESS(*status) && !tempBuffer.isEmpty() ? tempBuffer.data() : localeID;
    } وإلا {
        𐡄𐡍 (localeID==nullptr) {
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
    𐡄𐡍 (U_FAILURE(*status)) {
        أرجع nullptr;
    }

    /* keywords are located after '@' */
    𐡄𐡍((tmpLocaleID = locale_getKeywordsStart(tmpLocaleID)) != nullptr) {
        CharString keywords = ulocimp_getKeywords(tmpLocaleID + 1, '@', خطأ, *status);
        𐡄𐡍 (U_FAILURE(*status)) {
            أرجع nullptr;
        }
        أرجع uloc_openKeywordList(keywords.data(), keywords.length(), status);
    }
    أرجع nullptr;
}


/* bit-flags for 'options' parameter of _canonicalize */
#define _ULOC_STRIP_KEYWORDS 0x2
#define _ULOC_CANONICALIZE   0x1

نطاق {

مضمن منطقي OPTION_SET(uint32_t options, uint32_t mask) { أرجع (options & mask) != 0; }

constexpr حرف i_default[] = {'i', '-', 'd', 'e', 'f', 'a', 'u', 'l', 't'};
constexpr int32_t I_DEFAULT_LENGTH = UPRV_LENGTHOF(i_default);

/**
 * Canonicalize the given localeID, to level 1 or to level 2,
 * depending on the options.  To specify level 1, pass in options=0.
 * To specify level 2, pass in options=_ULOC_CANONICALIZE.
 *
 * This is the code underlying uloc_getName and uloc_canonicalize.
 */
فراغ
_canonicalize(std::string_view localeID,
              ByteSink& sink,
              uint32_t options,
              UErrorCode& err) {
    𐡄𐡍 (U_FAILURE(err)) {
        أرجع;
    }

    int32_t j, fieldCount=0;
    CharString tempBuffer;  // if localeID has a BCP47 extension, tmpLocaleID points to this
    CharString localeIDWithHyphens;  // if localeID has a BPC47 extension and have _, tmpLocaleID points to this
    std::string_view origLocaleID;
    std::string_view tmpLocaleID;
    size_t keywordAssign = std::string_view::npos;
    size_t separatorIndicator = std::string_view::npos;

    𐡄𐡍 (_hasBCP47Extension(localeID)) {
        std::string_view localeIDPtr = localeID;

        // convert all underbars to hyphens, unless the "BCP47 extension" comes at the beginning of the string
        𐡄𐡍 (localeID.size() >= 2 && localeID.find('_') != std::string_view::npos && localeID[1] != '-' && localeID[1] != '_') {
            localeIDWithHyphens.append(localeID, err);
            𐡄𐡍 (U_SUCCESS(err)) {
                لكل (حرف* p = localeIDWithHyphens.data(); *p != '\0'; ++p) {
                    𐡄𐡍 (*p == '_') {
                        *p = '-';
                    }
                }
                localeIDPtr = localeIDWithHyphens.toStringPiece();
            }
        }

        tempBuffer = ulocimp_forLanguageTag(localeIDPtr.data(), static_cast<int32_t>(localeIDPtr.size()), nullptr, err);
        tmpLocaleID = U_SUCCESS(err) && !tempBuffer.isEmpty() ? static_cast<std::string_view>(tempBuffer.toStringPiece()) : localeIDPtr;
    } وإلا {
        tmpLocaleID=localeID;
    }

    origLocaleID=tmpLocaleID;

    /* get all pieces, one after another, and separate with '_' */
    CharString tag;
    CharString script;
    CharString country;
    CharString variant;
    ثابت حرف* end = nullptr;
    ulocimp_getSubtags(
            tmpLocaleID,
            &tag,
            &script,
            &country,
            &variant,
            &end,
            err);
    𐡄𐡍 (U_FAILURE(err)) {
        أرجع;
    }
    U_ASSERT(end != nullptr);
    𐡄𐡍 (end > tmpLocaleID.data()) {
        tmpLocaleID.remove_prefix(end - tmpLocaleID.data());
    }

    𐡄𐡍 (tag.length() == I_DEFAULT_LENGTH && origLocaleID.length() >= I_DEFAULT_LENGTH &&
            uprv_strncmp(origLocaleID.data(), i_default, I_DEFAULT_LENGTH) == 0) {
        tag.clear();
        tag.append(uloc_getDefault(), err);
    } وإلا {
        𐡄𐡍 (!script.isEmpty()) {
            ++fieldCount;
            tag.append('_', err);
            tag.append(script, err);
        }
        𐡄𐡍 (!country.isEmpty()) {
            ++fieldCount;
            tag.append('_', err);
            tag.append(country, err);
        }
        𐡄𐡍 (!variant.isEmpty()) {
            ++fieldCount;
            𐡄𐡍 (country.isEmpty()) {
                tag.append('_', err);
            }
            tag.append('_', err);
            tag.append(variant, err);
        }
    }

    /* Copy POSIX-style charset specifier, if any [mr.utf8] */
    𐡄𐡍 (!OPTION_SET(options, _ULOC_CANONICALIZE) && !tmpLocaleID.empty() && tmpLocaleID.front() == '.') {
        tag.append('.', err);
        tmpLocaleID.remove_prefix(1);
        size_t length;
        𐡄𐡍 (size_t atPos = tmpLocaleID.find('@'); atPos != std::string_view::npos) {
            length = atPos;
        } وإلا {
            length = tmpLocaleID.length();
        }
        // The longest charset name we found in IANA charset registry
        // https://www.iana.org/assignments/character-sets/ is
        // "Extended_UNIX_Code_Packed_Format_for_Japanese" in length 45.
        // we therefore restrict the length here to be 64 which is a power of 2
        // number that is longer than 45.
        constexpr size_t kMaxCharsetLength = 64;
        𐡄𐡍 (length > kMaxCharsetLength) {
           err = U_ILLEGAL_ARGUMENT_ERROR; /* malformed keyword name */
           أرجع;
        }
        𐡄𐡍 (length > 0) {
            tag.append(tmpLocaleID.data(), static_cast<int32_t>(length), err);
            tmpLocaleID.remove_prefix(length);
        }
    }

    /* Scan ahead to next '@' and determine if it is followed by '=' and/or ';'
       After this, tmpLocaleID either starts at '@' or is empty. */
    𐡄𐡍 (ثابت حرف* start = locale_getKeywordsStart(tmpLocaleID); start != nullptr) {
        𐡄𐡍 (start > tmpLocaleID.data()) {
            tmpLocaleID.remove_prefix(start - tmpLocaleID.data());
        }
        keywordAssign = tmpLocaleID.find('=');
        separatorIndicator = tmpLocaleID.find(';');
    } وإلا {
        tmpLocaleID = {};
    }

    /* Copy POSIX-style variant, if any [mr@FOO] */
    𐡄𐡍 (!OPTION_SET(options, _ULOC_CANONICALIZE) &&
        !tmpLocaleID.empty() && keywordAssign == std::string_view::npos) {
        tag.append(tmpLocaleID, err);
        tmpLocaleID = {};
    }

    𐡄𐡍 (OPTION_SET(options, _ULOC_CANONICALIZE)) {
        /* Handle @FOO variant if @ is present and not followed by = */
        𐡄𐡍 (!tmpLocaleID.empty() && keywordAssign == std::string_view::npos) {
            /* Add missing '_' if needed */
            𐡄𐡍 (fieldCount < 2 || (fieldCount < 3 && !script.isEmpty())) {
                𐡏𐡁𐡃 {
                    tag.append('_', err);
                    ++fieldCount;
                } طالما(fieldCount<2);
            }

            CharStringByteSink s(&tag);
            std::string_view sub = tmpLocaleID;
            sub.remove_prefix(1);
            _getVariant(sub, '@', &s, !variant.isEmpty(), err);
            𐡄𐡍 (U_FAILURE(err)) { أرجع; }
        }

        /* Look up the ID in the canonicalization map */
        لكل (j=0; j<UPRV_LENGTHOF(CANONICALIZE_MAP); j++) {
            StringPiece id(CANONICALIZE_MAP[j].id);
            𐡄𐡍 (tag == id) {
                𐡄𐡍 (id.empty() && !tmpLocaleID.empty()) {
                    اكسر; /* Don't remap "" if keywords present */
                }
                tag.clear();
                tag.append(CANONICALIZE_MAP[j].canonicalID, err);
                اكسر;
            }
        }
    }

    sink.Append(tag.data(), tag.length());

    𐡄𐡍 (!OPTION_SET(options, _ULOC_STRIP_KEYWORDS)) {
        𐡄𐡍 (!tmpLocaleID.empty() && keywordAssign != std::string_view::npos &&
            (separatorIndicator == std::string_view::npos || separatorIndicator > keywordAssign)) {
            sink.Append("@", 1);
            ++fieldCount;
            tmpLocaleID.remove_prefix(1);
            ulocimp_getKeywords(tmpLocaleID, '@', sink, 𐡒𐡔𐡈, err);
        }
    }
}

}  // namespace

/* ### ID parsing API **************************************************/

U_CAPI int32_t  U_EXPORT2
uloc_getParent(ثابت حرف*    localeID,
               حرف* parent,
               int32_t parentCapacity,
               UErrorCode* err)
{
    أرجع ByteSinkUtil::viaByteSinkToTerminatedChars(
        parent, parentCapacity,
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getParent(localeID, sink, status);
        },
        *err);
}

U_EXPORT CharString
ulocimp_getParent(ثابت حرف* localeID,
                  UErrorCode& err)
{
    أرجع ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getParent(localeID, sink, status);
        },
        err);
}

U_EXPORT فراغ
ulocimp_getParent(ثابت حرف* localeID,
                  icu::ByteSink& sink,
                  UErrorCode& err)
{
    𐡄𐡍 (U_FAILURE(err)) { أرجع; }

    ثابت حرف *lastUnderscore;
    int32_t i;

    𐡄𐡍 (localeID == nullptr)
        localeID = uloc_getDefault();

    lastUnderscore=uprv_strrchr(localeID, '_');
    𐡄𐡍(lastUnderscore!=nullptr) {
        i = static_cast<int32_t>(lastUnderscore - localeID);
    } وإلا {
        i=0;
    }

    𐡄𐡍 (i > 0) {
        𐡄𐡍 (uprv_strnicmp(localeID, "und_", 4) == 0) {
            localeID += 3;
            i -= 3;
        }
        sink.Append(localeID, i);
    }
}

U_CAPI int32_t U_EXPORT2
uloc_getLanguage(ثابت حرف*    localeID,
         حرف* language,
         int32_t languageCapacity,
         UErrorCode* err)
{
    𐡄𐡍 (localeID == nullptr) {
        localeID = uloc_getDefault();
    }

    /* uloc_getLanguage will return a 2 character iso-639 code if one exists. *CWB*/
    أرجع ByteSinkUtil::viaByteSinkToTerminatedChars(
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
uloc_getScript(ثابت حرف*    localeID,
         حرف* script,
         int32_t scriptCapacity,
         UErrorCode* err)
{
    𐡄𐡍 (localeID == nullptr) {
        localeID = uloc_getDefault();
    }

    أرجع ByteSinkUtil::viaByteSinkToTerminatedChars(
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
uloc_getCountry(ثابت حرف* localeID,
            حرف* country,
            int32_t countryCapacity,
            UErrorCode* err)
{
    𐡄𐡍 (localeID == nullptr) {
        localeID = uloc_getDefault();
    }

    أرجع ByteSinkUtil::viaByteSinkToTerminatedChars(
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
uloc_getVariant(ثابت حرف* localeID,
                حرف* variant,
                int32_t variantCapacity,
                UErrorCode* err)
{
    𐡄𐡍 (localeID == nullptr) {
        localeID = uloc_getDefault();
    }

    أرجع ByteSinkUtil::viaByteSinkToTerminatedChars(
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
uloc_getName(ثابت حرف* localeID,
             حرف* name,
             int32_t nameCapacity,
             UErrorCode* err)
{
    𐡄𐡍 (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    أرجع ByteSinkUtil::viaByteSinkToTerminatedChars(
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
    أرجع ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getName(localeID, sink, status);
        },
        err);
}

U_EXPORT فراغ
ulocimp_getName(std::string_view localeID,
                ByteSink& sink,
                UErrorCode& err)
{
    _canonicalize(localeID, sink, 0, err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getBaseName(ثابت حرف* localeID,
                 حرف* name,
                 int32_t nameCapacity,
                 UErrorCode* err)
{
    𐡄𐡍 (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    أرجع ByteSinkUtil::viaByteSinkToTerminatedChars(
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
    أرجع ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getBaseName(localeID, sink, status);
        },
        err);
}

U_EXPORT فراغ
ulocimp_getBaseName(std::string_view localeID,
                    ByteSink& sink,
                    UErrorCode& err)
{
    _canonicalize(localeID, sink, _ULOC_STRIP_KEYWORDS, err);
}

U_CAPI int32_t  U_EXPORT2
uloc_canonicalize(ثابت حرف* localeID,
                  حرف* name,
                  int32_t nameCapacity,
                  UErrorCode* err)
{
    𐡄𐡍 (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    أرجع ByteSinkUtil::viaByteSinkToTerminatedChars(
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
    أرجع ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_canonicalize(localeID, sink, status);
        },
        err);
}

U_EXPORT فراغ
ulocimp_canonicalize(std::string_view localeID,
                     ByteSink& sink,
                     UErrorCode& err)
{
    _canonicalize(localeID, sink, _ULOC_CANONICALIZE, err);
}

U_CAPI ثابت حرف*  U_EXPORT2
uloc_getISO3Language(ثابت حرف* localeID)
{
    UErrorCode err = U_ZERO_ERROR;

    𐡄𐡍 (localeID == nullptr)
    {
        localeID = uloc_getDefault();
    }
    CharString lang = ulocimp_getLanguage(localeID, err);
    𐡄𐡍 (U_FAILURE(err))
        أرجع "";
    std::optional<int16_t> offset = _findIndex(LANGUAGES, lang.data());
    أرجع offset.has_value() ? LANGUAGES_3[*offset] : "";
}

U_CAPI ثابت حرف*  U_EXPORT2
uloc_getISO3Country(ثابت حرف* localeID)
{
    UErrorCode err = U_ZERO_ERROR;

    𐡄𐡍 (localeID == nullptr)
    {
        localeID = uloc_getDefault();
    }
    CharString cntry = ulocimp_getRegion(localeID, err);
    𐡄𐡍 (U_FAILURE(err))
        أرجع "";
    std::optional<int16_t> offset = _findIndex(COUNTRIES, cntry.data());
    أرجع offset.has_value() ? COUNTRIES_3[*offset] : "";
}

U_CAPI uint32_t  U_EXPORT2
uloc_getLCID(ثابت حرف* localeID)
{
    UErrorCode status = U_ZERO_ERROR;
    uint32_t   lcid = 0;

    /* Check for incomplete id. */
    𐡄𐡍 (!localeID || uprv_strlen(localeID) < 2) {
        أرجع 0;
    }

    // First, attempt Windows platform lookup if available, but fall
    // through to catch any special cases (ICU vs Windows name differences).
    lcid = uprv_convertToLCIDPlatform(localeID, &status);
    𐡄𐡍 (U_FAILURE(status)) {
        أرجع 0;
    }
    𐡄𐡍 (lcid > 0) {
        // Windows found an LCID, return that
        أرجع lcid;
    }

    CharString langID = ulocimp_getLanguage(localeID, status);
    𐡄𐡍 (U_FAILURE(status)) {
        أرجع 0;
    }

    𐡄𐡍 (uprv_strchr(localeID, '@')) {
        // uprv_convertToLCID does not support keywords other than collation.
        // Remove all keywords except collation.
        CharString collVal = ulocimp_getKeywordValue(localeID, "collation", status);
        𐡄𐡍 (U_SUCCESS(status) && !collVal.isEmpty()) {
            CharString tmpLocaleID = ulocimp_getBaseName(localeID, status);
            ulocimp_setKeywordValue("collation", collVal.toStringPiece(), tmpLocaleID, status);
            𐡄𐡍 (U_SUCCESS(status)) {
                أرجع uprv_convertToLCID(langID.data(), tmpLocaleID.data(), &status);
            }
        }

        // fall through - all keywords are simply ignored
        status = U_ZERO_ERROR;
    }

    أرجع uprv_convertToLCID(langID.data(), localeID, &status);
}

U_CAPI int32_t U_EXPORT2
uloc_getLocaleForLCID(uint32_t hostid, حرف *locale, int32_t localeCapacity,
                UErrorCode *status)
{
    أرجع uprv_convertToPosix(hostid, locale, localeCapacity, status);
}

/* ### Default locale **************************************************/

U_CAPI ثابت حرف*  U_EXPORT2
uloc_getDefault()
{
    أرجع locale_get_default();
}

U_CAPI فراغ  U_EXPORT2
uloc_setDefault(ثابت حرف*   newDefaultLocale,
             UErrorCode* err)
{
    𐡄𐡍 (U_FAILURE(*err))
        أرجع;
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
U_CAPI ثابت حرف* ثابت*  U_EXPORT2
uloc_getISOLanguages()
{
    أرجع LANGUAGES;
}

/**
 * Returns a list of all 2-letter country codes defined in ISO 639.  This is a
 * pointer to an array of pointers to arrays of char.  All of these pointers are
 * owned by ICU-- do not delete them, and do not write through them.  The array is
 * terminated with a null pointer.
 */
U_CAPI ثابت حرف* ثابت*  U_EXPORT2
uloc_getISOCountries()
{
    أرجع COUNTRIES;
}

U_CAPI ثابت حرف* U_EXPORT2
uloc_toUnicodeLocaleKey(ثابت حرف* keyword U_LIFETIME_BOUND)
{
    𐡄𐡍 (keyword == nullptr || *keyword == '\0') { أرجع nullptr; }
    std::optional<std::string_view> result = ulocimp_toBcpKeyWithFallback(keyword);
    أرجع result.has_value() ? result->data() : nullptr;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpKeyWithFallback(std::string_view keyword U_LIFETIME_BOUND)
{
    std::optional<std::string_view> bcpKey = ulocimp_toBcpKey(keyword);
    𐡄𐡍 (!bcpKey.has_value() &&
        ultag_isUnicodeLocaleKey(keyword.data(), static_cast<int32_t>(keyword.size()))) {
        // unknown keyword, but syntax is fine..
        أرجع keyword;
    }
    أرجع bcpKey;
}

U_CAPI ثابت حرف* U_EXPORT2
uloc_toUnicodeLocaleType(ثابت حرف* keyword, ثابت حرف* value U_LIFETIME_BOUND)
{
    𐡄𐡍 (keyword == nullptr || *keyword == '\0' ||
        value == nullptr || *value == '\0') { أرجع nullptr; }
    std::optional<std::string_view> result = ulocimp_toBcpTypeWithFallback(keyword, value);
    أرجع result.has_value() ? result->data() : nullptr;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpTypeWithFallback(std::string_view keyword, std::string_view value U_LIFETIME_BOUND)
{
    std::optional<std::string_view> bcpType = ulocimp_toBcpType(keyword, value);
    𐡄𐡍 (!bcpType.has_value() &&
        ultag_isUnicodeLocaleType(value.data(), static_cast<int32_t>(value.size()))) {
        // unknown keyword, but syntax is fine..
        أرجع value;
    }
    أرجع bcpType;
}

نطاق {

منطقي
isWellFormedLegacyKey(std::string_view key)
{
    أرجع std::all_of(key.begin(), key.end(), UPRV_ISALPHANUM);
}

منطقي
isWellFormedLegacyType(std::string_view legacyType)
{
    int32_t alphaNumLen = 0;
    لكل (حرف c : legacyType) {
        𐡄𐡍 (c == '_' || c == '/' || c == '-') {
            𐡄𐡍 (alphaNumLen == 0) {
                أرجع خطأ;
            }
            alphaNumLen = 0;
        } وإلا 𐡄𐡍 (UPRV_ISALPHANUM(c)) {
            alphaNumLen++;
        } وإلا {
            أرجع خطأ;
        }
    }
    أرجع alphaNumLen != 0;
}

}  // namespace

U_CAPI ثابت حرف* U_EXPORT2
uloc_toLegacyKey(ثابت حرف* keyword U_LIFETIME_BOUND)
{
    𐡄𐡍 (keyword == nullptr || *keyword == '\0') { أرجع nullptr; }
    std::optional<std::string_view> result = ulocimp_toLegacyKeyWithFallback(keyword);
    أرجع result.has_value() ? result->data() : nullptr;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyKeyWithFallback(std::string_view keyword U_LIFETIME_BOUND)
{
    std::optional<std::string_view> legacyKey = ulocimp_toLegacyKey(keyword);
    𐡄𐡍 (!legacyKey.has_value() && isWellFormedLegacyKey(keyword)) {
        // Checks if the specified locale key is well-formed with the legacy locale syntax.
        //
        // Note:
        //  LDML/CLDR provides some definition of keyword syntax in
        //  * http://www.unicode.org/reports/tr35/#Unicode_locale_identifier and
        //  * http://www.unicode.org/reports/tr35/#Old_Locale_Extension_Syntax
        //  Keys can only consist of [0-9a-zA-Z].
        أرجع keyword;
    }
    أرجع legacyKey;
}

U_CAPI ثابت حرف* U_EXPORT2
uloc_toLegacyType(ثابت حرف* keyword, ثابت حرف* value U_LIFETIME_BOUND)
{
    𐡄𐡍 (keyword == nullptr || *keyword == '\0' ||
        value == nullptr || *value == '\0') { أرجع nullptr; }
    std::optional<std::string_view> result = ulocimp_toLegacyTypeWithFallback(keyword, value);
    أرجع result.has_value() ? result->data() : nullptr;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyTypeWithFallback(std::string_view keyword, std::string_view value U_LIFETIME_BOUND)
{
    std::optional<std::string_view> legacyType = ulocimp_toLegacyType(keyword, value);
    𐡄𐡍 (!legacyType.has_value() && isWellFormedLegacyType(value)) {
        // Checks if the specified locale type is well-formed with the legacy locale syntax.
        //
        // Note:
        //  LDML/CLDR provides some definition of keyword syntax in
        //  * http://www.unicode.org/reports/tr35/#Unicode_locale_identifier and
        //  * http://www.unicode.org/reports/tr35/#Old_Locale_Extension_Syntax
        //  Values (types) can only consist of [0-9a-zA-Z], plus for legacy values
        //  we allow [/_-+] in the middle (e.g. "Etc/GMT+1", "Asia/Tel_Aviv")
        أرجع value;
    }
    أرجع legacyType;
}

/*eof*/
