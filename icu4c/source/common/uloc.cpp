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
U_CFUNC فراغ locale_سیٹ_default(ثابت حرف *id);
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
خالی_سوچک,
    "in",  "iw",  "ji",  "jw",  "mo",  "sh",  "swc", "tl",  /* obsolete language codes */
خالی_سوچک
};

constexpr ثابت حرف* DEPRECATED_LANGUAGES[]={
    "in", "iw", "ji", "jw", "mo", خالی_سوچک, خالی_سوچک
};
constexpr ثابت حرف* REPLACEMENT_LANGUAGES[]={
    "id", "he", "yi", "jv", "ro", خالی_سوچک, خالی_سوچک
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
خالی_سوچک,
/*  "in",  "iw",  "ji",  "jw",  "mo",  "sh",  "swc", "tl",  */
    "ind", "heb", "yid", "jaw", "mol", "srp", "swc", "tgl",
خالی_سوچک
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
خالی_سوچک,
    "AN",  "BU", "CS", "FX", "RO", "SU", "TP", "YD", "YU", "ZR",   /* obsolete country codes */
خالی_سوچک
};

constexpr ثابت حرف* DEPRECATED_COUNTRIES[] = {
    "AN", "BU", "CS", "DD", "DY", "FX", "HV", "NH", "RH", "SU", "TP", "UK", "VD", "YD", "YU", "ZR", خالی_سوچک, خالی_سوچک /* deprecated country list */
};
constexpr ثابت حرف* REPLACEMENT_COUNTRIES[] = {
/*  "AN", "BU", "CS", "DD", "DY", "FX", "HV", "NH", "RH", "SU", "TP", "UK", "VD", "YD", "YU", "ZR" */
    "CW", "MM", "RS", "DE", "BJ", "FR", "BF", "VU", "ZW", "RU", "TL", "GB", "VN", "YE", "RS", "CD", خالی_سوچک, خالی_سوچک  /* replacement country codes */
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
خالی_سوچک,
/*  "AN",  "BU",  "CS",  "FX",  "RO", "SU",  "TP",  "YD",  "YU",  "ZR" */
    "ANT", "BUR", "SCG", "FXX", "ROM", "SUN", "TMP", "YMD", "YUG", "ZAR",
خالی_سوچک
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
int32_t get_سبھ_توں_تھوڑا_Subtag_لمائی(std::string_view localeID) {
    int32_t localeIDLength = static_cast<int32_t>(localeID.length());
    int32_t length = localeIDLength;
    int32_t tmp_لمائی = 0;
    int32_t i;
    منطقي reset = سچ;

    لكل (i = 0; i < localeIDLength; i++) {
        إذا (localeID[i] != '_' && localeID[i] != '-') {
            إذا (reset) {
                tmp_لمائی = 0;
                reset = خطأ;
            }
            tmp_لمائی++;
        } وإلا {
            إذا (tmp_لمائی != 0 && tmp_لمائی < length) {
                length = tmp_لمائی;
            }
            reset = سچ;
        }
    }

    أرجع length;
}
/* Test if the locale id has BCP47 u extension and does not have '@' */
مضمن منطقي _hasBCP47Extension(std::string_view id) {
    أرجع id.find('@') == std::string_view::npos && get_سبھ_توں_تھوڑا_Subtag_لمائی(id) == 1;
}

/* ### Keywords **************************************************/
مضمن منطقي UPRV_ISDIGIT(حرف c) { أرجع c >= '0' && c <= '9'; }
مضمن منطقي UPRV_ISALPHANUM(حرف c) { أرجع uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }
/* Punctuation/symbols allowed in legacy key values */
مضمن منطقي UPRV_OK_قدر_PUNCTUATION(حرف c) { أرجع c == '_' || c == '-' || c == '+' || c == '/'; }

}  // namespace

#تعریف_کرو ULOC_KEYWORD_بفر_LEN 25
#تعریف_کرو ULOC_MAX_NO_KEYWORDS 25

U_CAPI ثابت حرف* U_EXPORT2
locale_get_Keywords_شروع(std::string_view localeID U_LIFETIME_BOUND) {
    إذا (size_t pos = localeID.find('@'); pos != std::string_view::npos) {
        أرجع localeID.انکڑے() + pos;
    }
#إذا (U_CHARSET_FAMILY == U_EBCDIC_FAMILY)
    وإلا {
        /* We do this because the @ sign is variant, and the @ sign used on one
        EBCDIC machine won't be compiled the same way on other EBCDIC based
        machines. */
        ساكن ثابت uint8_t ebcdicSigns[] = { 0x7C, 0x44, 0x66, 0x80, 0xAC, 0xAE, 0xAF, 0xB5, 0xEC, 0xEF, 0x00 };
        ثابت uint8_t *char_منزل_لبھو = ebcdicSigns;
        طالما(*char_منزل_لبھو) {
            إذا (size_t pos = localeID.find(*char_منزل_لبھو); pos != std::string_view::npos) {
                أرجع localeID.انکڑے() + pos;
            }
            char_منزل_لبھو++;
        }
    }
#جے_انت
    أرجع خالی_سوچک;
}

نطاق {

/**
 * @param keywordName incoming name to be canonicalized
 * @param status return status (keyword too long)
 * @return the keyword name
 */
CharString locale_canonKeywordName(std::string_view keywordName, UError_کوڈ& حالت)
{
  إذا (U_FAILURE(حالت)) { أرجع {}; }
  CharString نتیجہ;

  لكل (حرف c : keywordName) {
    إذا (!UPRV_ISALPHANUM(c)) {
      حالت = U_ILLEGAL_دلیل_ERROR; /* malformed keyword name */
      أرجع {};
    }
    نتیجہ.append(uprv_tolower(c), حالت);
  }
  إذا (نتیجہ.is_خالی_تھاواں()) {
    حالت = U_ILLEGAL_دلیل_ERROR; /* empty keyword name */
    أرجع {};
  }

  أرجع نتیجہ;
}

تعريفنوع بنية {
    حرف keyword[ULOC_KEYWORD_بفر_LEN];
    int32_t keywordLen;
    ثابت حرف *قدر_شروع;
    int32_t قدر_Len;
} KeywordStruct;

int32_t U_CALLCONV
compareKeywordStructs(ثابت فراغ * /*context*/, ثابت فراغ *کھبے, ثابت فراغ *سجے) {
    ثابت حرف* بایاں_String = static_cast<ثابت KeywordStruct*>(کھبے)->keyword;
    ثابت حرف* دایاں_String = static_cast<ثابت KeywordStruct*>(سجے)->keyword;
    أرجع uprv_strcmp(بایاں_String, دایاں_String);
}

}  // namespace

U_EXPORT CharString
ulocimp_getKeywords(std::string_view localeID,
                    حرف prev,
                    منطقي اقدار_Too,
                    UError_کوڈ& حالت)
{
    أرجع ByteSinkUtil::راہیں_Byte_Sink_منزل_Char_String(
        [&](ByteSink& sink, UError_کوڈ& حالت) {
            ulocimp_getKeywords(localeID,
                                prev,
                                sink,
                                اقدار_Too,
                                حالت);
        },
        حالت);
}

U_EXPORT فراغ
ulocimp_getKeywords(std::string_view localeID,
                    حرف prev,
                    ByteSink& sink,
                    منطقي اقدار_Too,
                    UError_کوڈ& حالت)
{
    إذا (U_FAILURE(حالت)) { أرجع; }

    KeywordStruct keywordList[ULOC_MAX_NO_KEYWORDS];

    int32_t maxKeywords = ULOC_MAX_NO_KEYWORDS;
    int32_t عدد_Keywords = 0;
    size_t equalSign = std::string_view::npos;
    size_t semicolon = std::string_view::npos;
    int32_t i = 0, j, n;

    إذا(prev == '@') { /* start of keyword definition */
        /* we will grab pairs, trim spaces, lowercase keywords, sort and return */
        افعل {
            منطقي duplicate = خطأ;
            /* skip leading spaces */
            طالما (!localeID.empty() && localeID.front() == ' ') {
                localeID.remove_اگیتر(1);
            }
            إذا (localeID.empty()) { /* handle trailing "; " */
                اكسر;
            }
            إذا(عدد_Keywords == maxKeywords) {
                حالت = U_اندرونی_پروگرام_ERROR;
                أرجع;
            }
            equalSign = localeID.find('=');
            semicolon = localeID.find(';');
            /* lack of '=' [foo@currency] is illegal */
            /* ';' before '=' [foo@currency;collation=pinyin] is illegal */
            إذا (equalSign == std::string_view::npos ||
                (semicolon != std::string_view::npos && semicolon < equalSign)) {
                حالت = U_INVALID_FORMAT_ERROR;
                أرجع;
            }
            /* zero-length keyword is an error. */
            إذا (equalSign == 0) {
                حالت = U_INVALID_FORMAT_ERROR;
                أرجع;
            }
            /* need to normalize both keyword and keyword name */
            إذا (equalSign >= ULOC_KEYWORD_بفر_LEN) {
                /* keyword name too long for internal buffer */
                حالت = U_اندرونی_پروگرام_ERROR;
                أرجع;
            }
            لكل (i = 0, n = 0; static_cast<size_t>(i) < equalSign; ++i) {
                إذا (localeID[i] != ' ') {
                    keywordList[عدد_Keywords].keyword[n++] = uprv_tolower(localeID[i]);
                }
            }

            keywordList[عدد_Keywords].keyword[n] = 0;
            keywordList[عدد_Keywords].keywordLen = n;
            /* now grab the value part. First we skip the '=' */
            equalSign++;
            /* then we leading spaces */
            طالما (equalSign < localeID.length() && localeID[equalSign] == ' ') {
                equalSign++;
            }

            /* Premature end or zero-length value */
            إذا (equalSign == localeID.length() || equalSign == semicolon) {
                حالت = U_INVALID_FORMAT_ERROR;
                أرجع;
            }

            keywordList[عدد_Keywords].قدر_شروع = localeID.انکڑے() + equalSign;

            std::string_view value = localeID;
            إذا (semicolon != std::string_view::npos) {
                value.remove_suffix(value.length() - semicolon);
                localeID.remove_اگیتر(semicolon + 1);
            } وإلا {
                localeID = {};
            }
            value.remove_اگیتر(equalSign);
            إذا (size_t آخری = value.find_last_not_of(' '); آخری != std::string_view::npos) {
                value.remove_suffix(value.length() - آخری - 1);
            }
            keywordList[عدد_Keywords].قدر_Len = static_cast<int32_t>(value.length());

            /* If this is a duplicate keyword, then ignore it */
            لكل (j=0; j<عدد_Keywords; ++j) {
                إذا (uprv_strcmp(keywordList[j].keyword, keywordList[عدد_Keywords].keyword) == 0) {
                    duplicate = سچ;
                    اكسر;
                }
            }
            إذا (!duplicate) {
                ++عدد_Keywords;
            }
        } طالما (!localeID.empty());

        /* now we have a list of keywords */
        /* we need to sort it */
        uprv_ترتیب_دیو_Array(keywordList, عدد_Keywords, حجم(KeywordStruct), compareKeywordStructs, خالی_سوچک, خطأ, &حالت);

        /* Now construct the keyword part */
        لكل(i = 0; i < عدد_Keywords; i++) {
            sink.Append(keywordList[i].keyword, keywordList[i].keywordLen);
            إذا(اقدار_Too) {
                sink.Append("=", 1);
                sink.Append(keywordList[i].قدر_شروع, keywordList[i].قدر_Len);
                إذا(i < عدد_Keywords - 1) {
                    sink.Append(";", 1);
                }
            } وإلا {
                sink.Append("\0", 1);
            }
        }
    }
}

U_CAPI int32_t U_EXPORT2
uloc_get_Keyword_قدر(ثابت حرف* localeID,
                     ثابت حرف* keywordName,
                     حرف* بفر, int32_t بفر_سمرتھا,
                     UError_کوڈ* حالت)
{
    إذا (U_FAILURE(*حالت)) { أرجع 0; }
    إذا (keywordName == خالی_سوچک || *keywordName == '\0') {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    أرجع ByteSinkUtil::راہیں_Byte_Sink_منزل_Terminated_Chars(
        بفر, بفر_سمرتھا,
        [&](ByteSink& sink, UError_کوڈ& حالت) {
            ulocimp_get_Keyword_قدر(localeID, keywordName, sink, حالت);
        },
        *حالت);
}

U_EXPORT CharString
ulocimp_get_Keyword_قدر(ثابت حرف* localeID,
                        std::string_view keywordName,
                        UError_کوڈ& حالت)
{
    أرجع ByteSinkUtil::راہیں_Byte_Sink_منزل_Char_String(
        [&](ByteSink& sink, UError_کوڈ& حالت) {
            ulocimp_get_Keyword_قدر(localeID, keywordName, sink, حالت);
        },
        حالت);
}

U_EXPORT فراغ
ulocimp_get_Keyword_قدر(ثابت حرف* localeID,
                        std::string_view keywordName,
                        icu::ByteSink& sink,
                        UError_کوڈ& حالت)
{
    إذا (U_FAILURE(حالت)) { أرجع; }

    إذا (localeID == خالی_سوچک || keywordName.empty()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    ثابت حرف* شروع_تلاش_ایتھے = خالی_سوچک;
    ثابت حرف* اگلا_Separator = خالی_سوچک;

    CharString عارضی_بفر;
    ثابت حرف* tmpLocaleID;

    CharString canonKeywordName = locale_canonKeywordName(keywordName, حالت);
    إذا (U_FAILURE(حالت)) {
      أرجع;
    }

    إذا (localeID != خالی_سوچک && _hasBCP47Extension(localeID)) {
        عارضی_بفر = ulocimp_for_Language_ٹیگ(localeID, -1, خالی_سوچک, حالت);
        tmpLocaleID = U_SUCCESS(حالت) && !عارضی_بفر.is_خالی_تھاواں() ? عارضی_بفر.انکڑے() : localeID;
    } وإلا {
        tmpLocaleID=localeID;
    }

    شروع_تلاش_ایتھے = locale_get_Keywords_شروع(tmpLocaleID);
    إذا(شروع_تلاش_ایتھے == خالی_سوچک) {
        /* no keywords, return at once */
        أرجع;
    }

    /* find the first keyword */
    طالما(شروع_تلاش_ایتھے) {
        ثابت حرف* کلید_قدر_پوچھ;

        شروع_تلاش_ایتھے++; /* skip @ or ; */
        اگلا_Separator = uprv_strchr(شروع_تلاش_ایتھے, '=');
        إذا(!اگلا_Separator) {
            حالت = U_ILLEGAL_دلیل_ERROR; /* key must have =value */
            أرجع;
        }
        /* strip leading & trailing spaces (TC decided to tolerate these) */
        طالما(*شروع_تلاش_ایتھے == ' ') {
            شروع_تلاش_ایتھے++;
        }
        کلید_قدر_پوچھ = اگلا_Separator;
        طالما (کلید_قدر_پوچھ > شروع_تلاش_ایتھے && *(کلید_قدر_پوچھ-1) == ' ') {
            کلید_قدر_پوچھ--;
        }
        /* now keyValueTail points to first char after the keyName */
        /* copy & normalize keyName from locale */
        إذا (شروع_تلاش_ایتھے == کلید_قدر_پوچھ) {
            حالت = U_ILLEGAL_دلیل_ERROR; /* empty keyword name in passed-in locale */
            أرجع;
        }
        CharString localeKeywordName;
        طالما (شروع_تلاش_ایتھے < کلید_قدر_پوچھ) {
          إذا (!UPRV_ISALPHANUM(*شروع_تلاش_ایتھے)) {
            حالت = U_ILLEGAL_دلیل_ERROR; /* malformed keyword name */
            أرجع;
          }
          localeKeywordName.append(uprv_tolower(*شروع_تلاش_ایتھے++), حالت);
        }
        إذا (U_FAILURE(حالت)) {
            أرجع;
        }

        شروع_تلاش_ایتھے = uprv_strchr(اگلا_Separator, ';');

        إذا (canonKeywordName == localeKeywordName) {
             /* current entry matches the keyword. */
           اگلا_Separator++; /* skip '=' */
            /* First strip leading & trailing spaces (TC decided to tolerate these) */
            طالما(*اگلا_Separator == ' ') {
              اگلا_Separator++;
            }
            کلید_قدر_پوچھ = (شروع_تلاش_ایتھے)? شروع_تلاش_ایتھے: اگلا_Separator + uprv_strlen(اگلا_Separator);
            طالما(کلید_قدر_پوچھ > اگلا_Separator && *(کلید_قدر_پوچھ-1) == ' ') {
              کلید_قدر_پوچھ--;
            }
            /* Now copy the value, but check well-formedness */
            إذا (اگلا_Separator == کلید_قدر_پوچھ) {
              حالت = U_ILLEGAL_دلیل_ERROR; /* empty key value name in passed-in locale */
              أرجع;
            }
            طالما (اگلا_Separator < کلید_قدر_پوچھ) {
              إذا (!UPRV_ISALPHANUM(*اگلا_Separator) && !UPRV_OK_قدر_PUNCTUATION(*اگلا_Separator)) {
                حالت = U_ILLEGAL_دلیل_ERROR; /* malformed key value */
                أرجع;
              }
              /* Should we lowercase value to return here? Tests expect as-is. */
              sink.Append(اگلا_Separator++, 1);
            }
            أرجع;
        }
    }
}

U_CAPI int32_t U_EXPORT2
uloc_سیٹ_Keyword_قدر(ثابت حرف* keywordName,
                     ثابت حرف* keyword_قدر,
                     حرف* بفر, int32_t بفر_سمرتھا,
                     UError_کوڈ* حالت)
{
    إذا (U_FAILURE(*حالت)) { أرجع 0; }

    إذا (keywordName == خالی_سوچک || *keywordName == 0) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    إذا (بفر_سمرتھا <= 1) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    int32_t bufLen = (int32_t)uprv_strlen(بفر);
    إذا(بفر_سمرتھا<bufLen) {
        /* The capacity is less than the length?! Is this NUL terminated? */
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    حرف* keywords = const_cast<حرف*>(
        locale_get_Keywords_شروع({بفر, static_cast<std::string_view::size_type>(bufLen)}));
    int32_t ادھار_Len = keywords == خالی_سوچک ? bufLen : keywords - بفر;
    // Remove -1 from the capacity so that this function can guarantee NUL termination.
    CheckedArrayByteSink sink(keywords == خالی_سوچک ? بفر + bufLen : keywords,
                              بفر_سمرتھا - ادھار_Len - 1);
    int32_t reslen = ulocimp_سیٹ_Keyword_قدر(
        keywords == خالی_سوچک ? std::string_view() : keywords,
        keywordName,
        keyword_قدر == خالی_سوچک ? std::string_view() : keyword_قدر,
        sink,
        *حالت);

    إذا (U_FAILURE(*حالت)) {
        أرجع *حالت == U_بفر_حد_توں_ودھ_ERROR ? reslen + ادھار_Len : 0;
    }

    // See the documentation for this function, it's guaranteed to never
    // overflow the buffer but instead abort with BUFFER_OVERFLOW_ERROR.
    // In this case, nothing has been written to the sink, so it cannot have Overflowed().
    U_ASSERT(!sink.Overflowed());
    U_ASSERT(reslen >= 0);
    أرجع u_terminateChars(بفر, بفر_سمرتھا, reslen + ادھار_Len, حالت);
}

U_EXPORT فراغ
ulocimp_سیٹ_Keyword_قدر(std::string_view keywordName,
                        std::string_view keyword_قدر,
                        CharString& localeID,
                        UError_کوڈ& حالت)
{
    إذا (U_FAILURE(حالت)) { أرجع; }
    std::string_view keywords;
    إذا (ثابت حرف* شروع = locale_get_Keywords_شروع(localeID.منزل_String_Piece()); شروع != خالی_سوچک) {
        // This is safe because CharString::truncate() doesn't actually erase any
        // data, but simply sets the position for where new data will be written.
        int32_t size = شروع - localeID.انکڑے();
        keywords = localeID.منزل_String_Piece();
        keywords.remove_اگیتر(size);
        localeID.truncate(size);
    }
    CharStringByteSink sink(&localeID);
    ulocimp_سیٹ_Keyword_قدر(keywords, keywordName, keyword_قدر, sink, حالت);
}

U_EXPORT int32_t
ulocimp_سیٹ_Keyword_قدر(std::string_view keywords,
                        std::string_view keywordName,
                        std::string_view keyword_قدر,
                        ByteSink& sink,
                        UError_کوڈ& حالت)
{
    إذا (U_FAILURE(حالت)) { أرجع 0; }

    /* TODO: sorting. removal. */
    int32_t needLen = 0;
    int32_t rc;
    CharString updated_کلیدیں_And_اقدار;
    منطقي handled_داخلہ_کلید_And_قدر = خطأ;
    حرف کلید_قدر_اگیتر = '@';

    إذا (حالت == U_STRING_NOT_TERMINATED_WARNING) {
        حالت = U_ZERO_ERROR;
    }
    إذا (keywordName.empty()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    CharString canonKeywordName = locale_canonKeywordName(keywordName, حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع 0;
    }

    CharString canon_Keyword_قدر;
    لكل (حرف c : keyword_قدر) {
        إذا (!UPRV_ISALPHANUM(c) && !UPRV_OK_قدر_PUNCTUATION(c)) {
            حالت = U_ILLEGAL_دلیل_ERROR; /* malformed key value */
            أرجع 0;
        }
        /* Should we force lowercase in value to set? */
        canon_Keyword_قدر.append(c, حالت);
    }
    إذا (U_FAILURE(حالت)) {
        أرجع 0;
    }

    إذا (keywords.size() <= 1) {
        إذا (canon_Keyword_قدر.is_خالی_تھاواں()) { /* no keywords = nothing to remove */
            U_ASSERT(حالت != U_STRING_NOT_TERMINATED_WARNING);
            أرجع 0;
        }

        needLen = 1 + canonKeywordName.length() + 1 + canon_Keyword_قدر.length();
        int32_t capacity = 0;
        حرف* بفر = sink.Get_Append_بفر(
                needLen, needLen, خالی_سوچک, needLen, &capacity);
        إذا (capacity < needLen || بفر == خالی_سوچک) {
            حالت = U_بفر_حد_توں_ودھ_ERROR;
            أرجع needLen; /* no change */
        }
        حرف* it = بفر;

        *it++ = '@';
        uprv_memcpy(it, canonKeywordName.انکڑے(), canonKeywordName.length());
        it += canonKeywordName.length();
        *it++ = '=';
        uprv_memcpy(it, canon_Keyword_قدر.انکڑے(), canon_Keyword_قدر.length());
        sink.Append(بفر, needLen);
        U_ASSERT(حالت != U_STRING_NOT_TERMINATED_WARNING);
        أرجع needLen;
    } /* end shortcut - no @ */

    /* search for keyword */
    لكل (size_t keyword_شروع = 0; keyword_شروع != std::string_view::npos;) {
        keyword_شروع++; /* skip @ or ; */
        size_t اگلا_Equalsign = keywords.find('=', keyword_شروع);
        إذا (اگلا_Equalsign == std::string_view::npos) {
            حالت = U_ILLEGAL_دلیل_ERROR; /* key must have =value */
            أرجع 0;
        }
        /* strip leading & trailing spaces (TC decided to tolerate these) */
        طالما (keyword_شروع < keywords.size() && keywords[keyword_شروع] == ' ') {
            keyword_شروع++;
        }
        size_t کلید_قدر_پوچھ = اگلا_Equalsign;
        طالما (کلید_قدر_پوچھ > keyword_شروع && keywords[کلید_قدر_پوچھ - 1] == ' ') {
            کلید_قدر_پوچھ--;
        }
        /* now keyValueTail points to first char after the keyName */
        /* copy & normalize keyName from locale */
        إذا (keyword_شروع == کلید_قدر_پوچھ) {
            حالت = U_ILLEGAL_دلیل_ERROR; /* empty keyword name in passed-in locale */
            أرجع 0;
        }
        CharString localeKeywordName;
        طالما (keyword_شروع < کلید_قدر_پوچھ) {
            إذا (!UPRV_ISALPHANUM(keywords[keyword_شروع])) {
                حالت = U_ILLEGAL_دلیل_ERROR; /* malformed keyword name */
                أرجع 0;
            }
            localeKeywordName.append(uprv_tolower(keywords[keyword_شروع++]), حالت);
        }
        إذا (U_FAILURE(حالت)) {
            أرجع 0;
        }

        size_t اگلا_Separator = keywords.find(';', اگلا_Equalsign);

        /* start processing the value part */
        اگلا_Equalsign++; /* skip '=' */
        /* First strip leading & trailing spaces (TC decided to tolerate these) */
        طالما (اگلا_Equalsign < keywords.size() && keywords[اگلا_Equalsign] == ' ') {
            اگلا_Equalsign++;
        }
        کلید_قدر_پوچھ = اگلا_Separator == std::string_view::npos ? keywords.size() : اگلا_Separator;
        طالما (کلید_قدر_پوچھ > اگلا_Equalsign && keywords[کلید_قدر_پوچھ - 1] == ' ') {
            کلید_قدر_پوچھ--;
        }
        إذا (اگلا_Equalsign == کلید_قدر_پوچھ) {
            حالت = U_ILLEGAL_دلیل_ERROR; /* empty key value in passed-in locale */
            أرجع 0;
        }

        rc = uprv_strcmp(canonKeywordName.انکڑے(), localeKeywordName.انکڑے());
        إذا(rc == 0) {
            /* Current entry matches the input keyword. Update the entry */
            إذا (!canon_Keyword_قدر.is_خالی_تھاواں()) { /* updating a value */
                updated_کلیدیں_And_اقدار.append(کلید_قدر_اگیتر, حالت);
                کلید_قدر_اگیتر = ';'; /* for any subsequent key-value pair */
                updated_کلیدیں_And_اقدار.append(canonKeywordName, حالت);
                updated_کلیدیں_And_اقدار.append('=', حالت);
                updated_کلیدیں_And_اقدار.append(canon_Keyword_قدر, حالت);
            } /* else removing this entry, don't emit anything */
            handled_داخلہ_کلید_And_قدر = سچ;
        } وإلا {
           /* input keyword sorts earlier than current entry, add before current entry */
            إذا (rc < 0 && !canon_Keyword_قدر.is_خالی_تھاواں() && !handled_داخلہ_کلید_And_قدر) {
                /* insert new entry at this location */
                updated_کلیدیں_And_اقدار.append(کلید_قدر_اگیتر, حالت);
                کلید_قدر_اگیتر = ';'; /* for any subsequent key-value pair */
                updated_کلیدیں_And_اقدار.append(canonKeywordName, حالت);
                updated_کلیدیں_And_اقدار.append('=', حالت);
                updated_کلیدیں_And_اقدار.append(canon_Keyword_قدر, حالت);
                handled_داخلہ_کلید_And_قدر = سچ;
            }
            /* copy the current entry */
            updated_کلیدیں_And_اقدار.append(کلید_قدر_اگیتر, حالت);
            کلید_قدر_اگیتر = ';'; /* for any subsequent key-value pair */
            updated_کلیدیں_And_اقدار.append(localeKeywordName, حالت);
            updated_کلیدیں_And_اقدار.append('=', حالت);
            updated_کلیدیں_And_اقدار.append(keywords.انکڑے() + اگلا_Equalsign,
                                        static_cast<int32_t>(کلید_قدر_پوچھ - اگلا_Equalsign), حالت);
        }
        إذا (اگلا_Separator == std::string_view::npos && !canon_Keyword_قدر.is_خالی_تھاواں() && !handled_داخلہ_کلید_And_قدر) {
            /* append new entry at the end, it sorts later than existing entries */
            updated_کلیدیں_And_اقدار.append(کلید_قدر_اگیتر, حالت);
            /* skip keyValuePrefix update, no subsequent key-value pair */
            updated_کلیدیں_And_اقدار.append(canonKeywordName, حالت);
            updated_کلیدیں_And_اقدار.append('=', حالت);
            updated_کلیدیں_And_اقدار.append(canon_Keyword_قدر, حالت);
            handled_داخلہ_کلید_And_قدر = سچ;
        }
        keyword_شروع = اگلا_Separator;
    } /* end loop searching */

    /* Any error from updatedKeysAndValues.append above would be internal and not due to
     * problems with the passed-in locale. So if we did encounter problems with the
     * passed-in locale above, those errors took precedence and overrode any error
     * status from updatedKeysAndValues.append, and also caused a return of 0. If there
     * are errors here they are from updatedKeysAndValues.append; they do cause an
     * error return but the passed-in locale is unmodified and the original bufLen is
     * returned.
     */
    إذا (!handled_داخلہ_کلید_And_قدر || U_FAILURE(حالت)) {
        /* if input key/value specified removal of a keyword not present in locale, or
         * there was an error in CharString.append, leave original locale alone. */
        U_ASSERT(حالت != U_STRING_NOT_TERMINATED_WARNING);
        // The sink is expected to be a buffer which already contains the full
        // locale string, so when it isn't going to be modified there's no need
        // to actually write any data to it, as the data is already there. Only
        // the first character needs to be overwritten (changing '\0' to '@').
        needLen = static_cast<int32_t>(keywords.size());
        int32_t capacity = 0;
        حرف* بفر = sink.Get_Append_بفر(
                needLen, needLen, خالی_سوچک, needLen, &capacity);
        إذا (capacity < needLen || بفر == خالی_سوچک) {
            حالت = U_بفر_حد_توں_ودھ_ERROR;
        } وإلا {
            *بفر = '@';
            sink.Append(بفر, needLen);
        }
        أرجع needLen;
    }

    needLen = updated_کلیدیں_And_اقدار.length();
    // Check to see can we fit the updatedKeysAndValues, if not, return
    // U_BUFFER_OVERFLOW_ERROR without copy updatedKeysAndValues into it.
    // We do this because this API function does not behave like most others:
    // It promises never to set a U_STRING_NOT_TERMINATED_WARNING.
    // When the contents fits but without the terminating NUL, in this case we need to not change
    // the buffer contents and return with a buffer overflow error.
    إذا (needLen > 0) {
        int32_t capacity = 0;
        حرف* بفر = sink.Get_Append_بفر(
                needLen, needLen, خالی_سوچک, needLen, &capacity);
        إذا (capacity < needLen || بفر == خالی_سوچک) {
            حالت = U_بفر_حد_توں_ودھ_ERROR;
            أرجع needLen;
        }
        uprv_memcpy(بفر, updated_کلیدیں_And_اقدار.انکڑے(), needLen);
        sink.Append(بفر, needLen);
    }
    U_ASSERT(حالت != U_STRING_NOT_TERMINATED_WARNING);
    أرجع needLen;
}

/* ### ID parsing implementation **************************************************/

نطاق {

مضمن منطقي is_اگیتر_Letter(حرف a) { أرجع a == 'x' || a == 'X' || a == 'i' || a == 'I'; }

/*returns true if one of the special prefixes is here (s=string)
  'x-' or 'i-' */
مضمن منطقي _isIDPrefix(std::string_view s) {
    أرجع s.size() >= 2 && is_اگیتر_Letter(s[0]) && _isIDSeparator(s[1]);
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
std::optional<int16_t> لبھو_اشاریہ_2(ثابت حرف* ثابت* list, ثابت حرف* کلید)
{
    ثابت حرف* ثابت* anchor = list;
    int32_t pass = 0;

    /* Make two passes through two nullptr-terminated arrays at 'list' */
    طالما (pass++ < 2) {
        طالما (*list) {
            إذا (uprv_strcmp(کلید, *list) == 0) {
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
    std::optional<int16_t> وتھ = لبھو_اشاریہ_2(DEPRECATED_COUNTRIES, oldID);
    أرجع وتھ.has_value() ? REPLACEMENT_COUNTRIES[*وتھ] : oldID;
}
U_CFUNC ثابت حرف*
uloc_getCurrentLanguageID(ثابت حرف* oldID U_LIFETIME_BOUND) {
    std::optional<int16_t> وتھ = لبھو_اشاریہ_2(DEPRECATED_LANGUAGES, oldID);
    أرجع وتھ.has_value() ? REPLACEMENT_LANGUAGES[*وتھ] : oldID;
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

size_t _getLanguage(std::string_view localeID, ByteSink* sink, UError_کوڈ& حالت) {
    size_t skip = 0;
    إذا (localeID.size() == 4 && uprv_strnicmp(localeID.انکڑے(), "root", 4) == 0) {
        skip = 4;
        localeID.remove_اگیتر(skip);
    } وإلا إذا (localeID.size() >= 3 && uprv_strnicmp(localeID.انکڑے(), "und", 3) == 0 &&
               (localeID.size() == 3 ||
                localeID[3] == '-' ||
                localeID[3] == '_' ||
                localeID[3] == '@')) {
        skip = 3;
        localeID.remove_اگیتر(skip);
    }

    constexpr int32_t MAXLEN = ULOC_LANG_سمرتھا - 1;  // Minus NUL.

    /* if it starts with i- or x- then copy that prefix */
    size_t len = _isIDPrefix(localeID) ? 2 : 0;
    طالما (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len])) {
        إذا (len == MAXLEN) {
            حالت = U_ILLEGAL_دلیل_ERROR;
            أرجع 0;
        }
        len++;
    }

    إذا (sink == خالی_سوچک || len == 0) { أرجع skip + len; }

    int32_t min_سمرتھا = uprv_max(static_cast<int32_t>(len), 4);  // Minimum 3 letters plus NUL.
    حرف scratch[MAXLEN];
    int32_t capacity = 0;
    حرف* بفر = sink->Get_Append_بفر(
            min_سمرتھا, min_سمرتھا, scratch, UPRV_LENGTHOF(scratch), &capacity);

    لكل (size_t i = 0; i < len; ++i) {
        بفر[i] = uprv_tolower(localeID[i]);
    }
    إذا (localeID.size() >= 2 && _isIDSeparator(localeID[1])) {
        بفر[1] = '-';
    }

    إذا (len == 3) {
        /* convert 3 character code to 2 character code if possible *CWB*/
        U_ASSERT(capacity >= 4);
        بفر[3] = '\0';
        std::optional<int16_t> وتھ = لبھو_اشاریہ_2(LANGUAGES_3, بفر);
        إذا (وتھ.has_value()) {
            ثابت حرف* ثابت alias = LANGUAGES[*وتھ];
            sink->Append(alias, static_cast<int32_t>(uprv_strlen(alias)));
            أرجع skip + len;
        }
    }

    sink->Append(بفر, static_cast<int32_t>(len));
    أرجع skip + len;
}

size_t _getScript(std::string_view localeID, ByteSink* sink) {
    constexpr int32_t لمائی_3 = 4;

    size_t len = 0;
    طالما (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len]) &&
            uprv_isASCIILetter(localeID[len])) {
        إذا (len == لمائی_3) { أرجع 0; }
        len++;
    }
    إذا (len != لمائی_3) { أرجع 0; }

    إذا (sink == خالی_سوچک) { أرجع len; }

    حرف scratch[لمائی_3];
    int32_t capacity = 0;
    حرف* بفر = sink->Get_Append_بفر(
            لمائی_3, لمائی_3, scratch, UPRV_LENGTHOF(scratch), &capacity);

    بفر[0] = uprv_toupper(localeID[0]);
    لكل (int32_t i = 1; i < لمائی_3; ++i) {
        بفر[i] = uprv_tolower(localeID[i]);
    }

    sink->Append(بفر, لمائی_3);
    أرجع len;
}

size_t _getRegion(std::string_view localeID, ByteSink* sink) {
    constexpr int32_t MINLEN = 2;
    constexpr int32_t MAXLEN = ULOC_COUNTRY_سمرتھا - 1;  // Minus NUL.

    size_t len = 0;
    طالما (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len])) {
        إذا (len == MAXLEN) { أرجع 0; }
        len++;
    }
    إذا (len < MINLEN) { أرجع 0; }

    إذا (sink == خالی_سوچک) { أرجع len; }

    حرف scratch[ULOC_COUNTRY_سمرتھا];
    int32_t capacity = 0;
    حرف* بفر = sink->Get_Append_بفر(
            ULOC_COUNTRY_سمرتھا,
            ULOC_COUNTRY_سمرتھا,
            scratch,
            UPRV_LENGTHOF(scratch),
            &capacity);

    لكل (size_t i = 0; i < len; ++i) {
        بفر[i] = uprv_toupper(localeID[i]);
    }

    إذا (len == 3) {
        /* convert 3 character code to 2 character code if possible *CWB*/
        U_ASSERT(capacity >= 4);
        بفر[3] = '\0';
        std::optional<int16_t> وتھ = لبھو_اشاریہ_2(COUNTRIES_3, بفر);
        إذا (وتھ.has_value()) {
            ثابت حرف* ثابت alias = COUNTRIES[*وتھ];
            sink->Append(alias, static_cast<int32_t>(uprv_strlen(alias)));
            أرجع len;
        }
    }

    sink->Append(بفر, static_cast<int32_t>(len));
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
            UError_کوڈ& حالت) {
    إذا (U_FAILURE(حالت) || localeID.empty()) أرجع 0;

    // Reasonable upper limit for variants
    // There are no strict limitation of the syntax of variant in the legacy
    // locale format. If the locale is constructed from unicode_locale_id
    // as defined in UTS35, then we know each unicode_variant_subtag
    // could have max length of 8 ((alphanum{5,8} | digit alphanum{3})
    // 179 would allow 20 unicode_variant_subtag with sep in the
    // unicode_locale_id
    // 8*20 + 1*(20-1) = 179
    constexpr int32_t MAX_VARIANTS_لمائی = 179;

    /* get one or more variant tags and separate them with '_' */
    size_t اشاریہ = 0;
    إذا (_isIDSeparator(prev)) {
        /* get a variant string after a '-' or '_' */
        لكل (std::string_view sub = localeID;;) {
            size_t next = sub.find_first_of(".@_-");
            // For historical reasons, a trailing separator is included in the variant.
            منطقي مکمل = next == std::string_view::npos || next + 1 == sub.length();
            size_t حد_بندی = مکمل ? sub.length() : next;
            اشاریہ += حد_بندی;
            إذا (اشاریہ > MAX_VARIANTS_لمائی) {
                حالت = U_ILLEGAL_دلیل_ERROR;
                أرجع 0;
            }

            إذا (sink != خالی_سوچک) {
                إذا (needSeparator) {
                    sink->Append("_", 1);
                } وإلا {
                    needSeparator = سچ;
                }

                int32_t length = static_cast<int32_t>(حد_بندی);
                int32_t min_سمرتھا = uprv_min(length, MAX_VARIANTS_لمائی);
                حرف scratch[MAX_VARIANTS_لمائی];
                int32_t capacity = 0;
                حرف* بفر = sink->Get_Append_بفر(
                        min_سمرتھا, min_سمرتھا, scratch, UPRV_LENGTHOF(scratch), &capacity);

                لكل (size_t i = 0; i < حد_بندی; ++i) {
                    بفر[i] = uprv_toupper(sub[i]);
                }
                sink->Append(بفر, length);
            }

            إذا (مکمل) { أرجع اشاریہ; }
            sub.remove_اگیتر(next);
            إذا (_isTerminator(sub.front()) || _isBCP47Extension(sub)) { أرجع اشاریہ; }
            sub.remove_اگیتر(1);
            اشاریہ++;
        }
    }

    size_t skip = 0;
    /* if there is no variant tag after a '-' or '_' then look for '@' */
    إذا (prev == '@') {
        /* keep localeID */
    } وإلا إذا (ثابت حرف* p = locale_get_Keywords_شروع(localeID); p != خالی_سوچک) {
        skip = 1 + p - localeID.انکڑے(); /* point after the '@' */
        localeID.remove_اگیتر(skip);
    } وإلا {
        أرجع 0;
    }
    لكل (; اشاریہ < localeID.size() && !_isTerminator(localeID[اشاریہ]); اشاریہ++) {
        إذا (اشاریہ >= MAX_VARIANTS_لمائی) { // same as length > MAX_VARIANTS_LENGTH
            حالت = U_ILLEGAL_دلیل_ERROR;
            أرجع 0;
        }
        إذا (needSeparator) {
            إذا (sink != خالی_سوچک) {
                sink->Append("_", 1);
            }
            needSeparator = خطأ;
        }
        إذا (sink != خالی_سوچک) {
            حرف c = uprv_toupper(localeID[اشاریہ]);
            إذا (c == '-' || c == ',') c = '_';
            sink->Append(&c, 1);
        }
    }
    أرجع skip + اشاریہ;
}

}  // namespace

U_EXPORT CharString
ulocimp_getLanguage(std::string_view localeID, UError_کوڈ& حالت) {
    أرجع ByteSinkUtil::راہیں_Byte_Sink_منزل_Char_String(
        [&](ByteSink& sink, UError_کوڈ& حالت) {
            ulocimp_getSubtags(
                    localeID,
                    &sink,
                    خالی_سوچک,
                    خالی_سوچک,
                    خالی_سوچک,
                    خالی_سوچک,
                    حالت);
        },
        حالت);
}

U_EXPORT CharString
ulocimp_getScript(std::string_view localeID, UError_کوڈ& حالت) {
    أرجع ByteSinkUtil::راہیں_Byte_Sink_منزل_Char_String(
        [&](ByteSink& sink, UError_کوڈ& حالت) {
            ulocimp_getSubtags(
                    localeID,
                    خالی_سوچک,
                    &sink,
                    خالی_سوچک,
                    خالی_سوچک,
                    خالی_سوچک,
                    حالت);
        },
        حالت);
}

U_EXPORT CharString
ulocimp_getRegion(std::string_view localeID, UError_کوڈ& حالت) {
    أرجع ByteSinkUtil::راہیں_Byte_Sink_منزل_Char_String(
        [&](ByteSink& sink, UError_کوڈ& حالت) {
            ulocimp_getSubtags(
                    localeID,
                    خالی_سوچک,
                    خالی_سوچک,
                    &sink,
                    خالی_سوچک,
                    خالی_سوچک,
                    حالت);
        },
        حالت);
}

U_EXPORT CharString
ulocimp_getVariant(std::string_view localeID, UError_کوڈ& حالت) {
    أرجع ByteSinkUtil::راہیں_Byte_Sink_منزل_Char_String(
        [&](ByteSink& sink, UError_کوڈ& حالت) {
            ulocimp_getSubtags(
                    localeID,
                    خالی_سوچک,
                    خالی_سوچک,
                    خالی_سوچک,
                    &sink,
                    خالی_سوچک,
                    حالت);
        },
        حالت);
}

U_EXPORT فراغ
ulocimp_getSubtags(
        std::string_view localeID,
        CharString* language,
        CharString* script,
        CharString* region,
        CharString* variant,
        ثابت حرف** p_سرا,
        UError_کوڈ& حالت) {
    إذا (U_FAILURE(حالت)) { أرجع; }

    std::optional<CharStringByteSink> languageSink;
    std::optional<CharStringByteSink> scriptSink;
    std::optional<CharStringByteSink> regionSink;
    std::optional<CharStringByteSink> variantSink;

    إذا (language != خالی_سوچک) { languageSink.emplace(language); }
    إذا (script != خالی_سوچک) { scriptSink.emplace(script); }
    إذا (region != خالی_سوچک) { regionSink.emplace(region); }
    إذا (variant != خالی_سوچک) { variantSink.emplace(variant); }

    ulocimp_getSubtags(
            localeID,
            languageSink.has_value() ? &*languageSink : خالی_سوچک,
            scriptSink.has_value() ? &*scriptSink : خالی_سوچک,
            regionSink.has_value() ? &*regionSink : خالی_سوچک,
            variantSink.has_value() ? &*variantSink : خالی_سوچک,
            p_سرا,
            حالت);
}

U_EXPORT فراغ
ulocimp_getSubtags(
        std::string_view localeID,
        ByteSink* language,
        ByteSink* script,
        ByteSink* region,
        ByteSink* variant,
        ثابت حرف** p_سرا,
        UError_کوڈ& حالت) {
    إذا (U_FAILURE(حالت)) { أرجع; }

    إذا (p_سرا != خالی_سوچک) {
        *p_سرا = localeID.انکڑے();
    } وإلا إذا (language == خالی_سوچک &&
               script == خالی_سوچک &&
               region == خالی_سوچک &&
               variant == خالی_سوچک) {
        أرجع;
    }

    إذا (localeID.empty()) { أرجع; }

    منطقي hasRegion = خطأ;

    {
        size_t len = _getLanguage(localeID, language, حالت);
        إذا (U_FAILURE(حالت)) { أرجع; }
        إذا (len > 0) {
            localeID.remove_اگیتر(len);
        }
    }

    إذا (p_سرا != خالی_سوچک) {
        *p_سرا = localeID.انکڑے();
    } وإلا إذا (script == خالی_سوچک &&
               region == خالی_سوچک &&
               variant == خالی_سوچک) {
        أرجع;
    }

    إذا (localeID.empty()) { أرجع; }

    إذا (_isIDSeparator(localeID.front())) {
        std::string_view sub = localeID;
        sub.remove_اگیتر(1);
        size_t len = _getScript(sub, script);
        إذا (len > 0) {
            localeID.remove_اگیتر(len + 1);
            إذا (p_سرا != خالی_سوچک) { *p_سرا = localeID.انکڑے(); }
        }
    }

    إذا ((region == خالی_سوچک && variant == خالی_سوچک && p_سرا == خالی_سوچک) || localeID.empty()) { أرجع; }

    إذا (_isIDSeparator(localeID.front())) {
        std::string_view sub = localeID;
        sub.remove_اگیتر(1);
        size_t len = _getRegion(sub, region);
        إذا (len > 0) {
            hasRegion = سچ;
            localeID.remove_اگیتر(len + 1);
            إذا (p_سرا != خالی_سوچک) { *p_سرا = localeID.انکڑے(); }
        }
    }

    إذا ((variant == خالی_سوچک && p_سرا == خالی_سوچک) || localeID.empty()) { أرجع; }

    منطقي hasVariant = خطأ;

    إذا (_isIDSeparator(localeID.front()) && !_isBCP47Extension(localeID)) {
        std::string_view sub = localeID;
        /* If there was no country ID, skip a possible extra IDSeparator */
        size_t skip = !hasRegion && localeID.size() > 1 && _isIDSeparator(localeID[1]) ? 2 : 1;
        sub.remove_اگیتر(skip);
        size_t len = _getVariant(sub, localeID[0], variant, خطأ, حالت);
        إذا (U_FAILURE(حالت)) { أرجع; }
        إذا (len > 0) {
            hasVariant = سچ;
            localeID.remove_اگیتر(skip + len);
            إذا (p_سرا != خالی_سوچک) { *p_سرا = localeID.انکڑے(); }
        }
    }

    إذا ((variant == خالی_سوچک && p_سرا == خالی_سوچک) || localeID.empty()) { أرجع; }

    إذا (_isBCP47Extension(localeID)) {
        localeID.remove_اگیتر(2);
        constexpr حرف vaposix[] = "-va-posix";
        constexpr size_t length = حجم vaposix - 1;
        لكل (size_t next;; localeID.remove_اگیتر(next)) {
            next = localeID.find('-', 1);
            إذا (next == std::string_view::npos) { اكسر; }
            next = localeID.find('-', next + 1);
            منطقي مکمل = next == std::string_view::npos;
            std::string_view sub = localeID;
            إذا (!مکمل) { sub.remove_suffix(sub.length() - next); }

            إذا (sub.length() == length && uprv_strnicmp(sub.انکڑے(), vaposix, length) == 0) {
                إذا (variant != خالی_سوچک) {
                    إذا (hasVariant) { variant->Append("_", 1); }
                    constexpr حرف posix[] = "POSIX";
                    variant->Append(posix, حجم posix - 1);
                }
                إذا (p_سرا != خالی_سوچک) { *p_سرا = localeID.انکڑے() + length; }
            }

            إذا (مکمل) { اكسر; }
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
uloc_kw_گنتی_Keywords(UEnumeration *en, UError_کوڈ * /*حالت*/) {
    حرف *kw = ((UKeywordsContext *)en->context)->keywords;
    int32_t نتیجہ = 0;
    طالما(*kw) {
        نتیجہ++;
        kw += uprv_strlen(kw)+1;
    }
    أرجع نتیجہ;
}

ساكن ثابت حرف * U_CALLCONV
uloc_kw_اگلا_Keyword(UEnumeration* en,
                    int32_t* نتیجہ_لمائی,
                    UError_کوڈ* /*حالت*/) {
    ثابت حرف* نتیجہ = ((UKeywordsContext *)en->context)->current;
    int32_t len = 0;
    إذا(*نتیجہ) {
        len = (int32_t)uprv_strlen(((UKeywordsContext *)en->context)->current);
        ((UKeywordsContext *)en->context)->current += len+1;
    } وإلا {
        نتیجہ = خالی_سوچک;
    }
    إذا (نتیجہ_لمائی) {
        *نتیجہ_لمائی = len;
    }
    أرجع نتیجہ;
}

ساكن فراغ U_CALLCONV
uloc_kw_resetKeywords(UEnumeration* en,
                      UError_کوڈ* /*حالت*/) {
    ((UKeywordsContext *)en->context)->current = ((UKeywordsContext *)en->context)->keywords;
}

U_CDECL_سرا


ساكن ثابت UEnumeration gKeywordsEnum = {
    خالی_سوچک,
    خالی_سوچک,
    uloc_kw_closeKeywords,
    uloc_kw_گنتی_Keywords,
    uenum_unextDefault,
    uloc_kw_اگلا_Keyword,
    uloc_kw_resetKeywords
};

U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywordList(ثابت حرف *keywordList, int32_t keyword_List_ناپ, UError_کوڈ* حالت)
{
    إذا (U_FAILURE(*حالت)) { أرجع خالی_سوچک; }

    Local_حافظہ<UKeywordsContext> myContext;
    Local_حافظہ<UEnumeration> نتیجہ;

    myContext.adoptInstead(static_cast<UKeywordsContext *>(uprv_malloc(حجم(UKeywordsContext))));
    نتیجہ.adoptInstead(static_cast<UEnumeration *>(uprv_malloc(حجم(UEnumeration))));
    إذا (myContext.isNull() || نتیجہ.isNull()) {
        *حالت = U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }
    uprv_memcpy(نتیجہ.getAlias(), &gKeywordsEnum, حجم(UEnumeration));
    myContext->keywords = static_cast<حرف *>(uprv_malloc(keyword_List_ناپ+1));
    إذا (myContext->keywords == خالی_سوچک) {
        *حالت = U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }
    uprv_memcpy(myContext->keywords, keywordList, keyword_List_ناپ);
    myContext->keywords[keyword_List_ناپ] = 0;
    myContext->current = myContext->keywords;
    نتیجہ->context = myContext.orphan();
    أرجع نتیجہ.orphan();
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywords(ثابت حرف* localeID,
                        UError_کوڈ* حالت)
{
    إذا(حالت==خالی_سوچک || U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }

    CharString عارضی_بفر;
    ثابت حرف* tmpLocaleID;

    إذا (localeID != خالی_سوچک && _hasBCP47Extension(localeID)) {
        عارضی_بفر = ulocimp_for_Language_ٹیگ(localeID, -1, خالی_سوچک, *حالت);
        tmpLocaleID = U_SUCCESS(*حالت) && !عارضی_بفر.is_خالی_تھاواں() ? عارضی_بفر.انکڑے() : localeID;
    } وإلا {
        إذا (localeID==خالی_سوچک) {
            localeID=uloc_getDefault();
        }
        tmpLocaleID=localeID;
    }

    ulocimp_getSubtags(
            tmpLocaleID,
            خالی_سوچک,
            خالی_سوچک,
            خالی_سوچک,
            خالی_سوچک,
            &tmpLocaleID,
            *حالت);
    إذا (U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }

    /* keywords are located after '@' */
    إذا((tmpLocaleID = locale_get_Keywords_شروع(tmpLocaleID)) != خالی_سوچک) {
        CharString keywords = ulocimp_getKeywords(tmpLocaleID + 1, '@', خطأ, *حالت);
        إذا (U_FAILURE(*حالت)) {
            أرجع خالی_سوچک;
        }
        أرجع uloc_openKeywordList(keywords.انکڑے(), keywords.length(), حالت);
    }
    أرجع خالی_سوچک;
}


/* bit-flags for 'options' parameter of _canonicalize */
#تعریف_کرو _ULOC_STRIP_KEYWORDS 0x2
#تعریف_کرو _ULOC_CANONICALIZE   0x1

نطاق {

مضمن منطقي OPTION_سیٹ(uint32_t options, uint32_t mask) { أرجع (options & mask) != 0; }

constexpr حرف i_default[] = {'i', '-', 'd', 'e', 'f', 'a', 'u', 'l', 't'};
constexpr int32_t I_DEFAULT_لمائی = UPRV_LENGTHOF(i_default);

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
              UError_کوڈ& err) {
    إذا (U_FAILURE(err)) {
        أرجع;
    }

    int32_t j, میدان_گنتی=0;
    CharString عارضی_بفر;  // if localeID has a BCP47 extension, tmpLocaleID points to this
    CharString localeIDWithHyphens;  // if localeID has a BPC47 extension and have _, tmpLocaleID points to this
    std::string_view origLocaleID;
    std::string_view tmpLocaleID;
    size_t keyword_سونپو = std::string_view::npos;
    size_t separatorIndicator = std::string_view::npos;

    إذا (_hasBCP47Extension(localeID)) {
        std::string_view localeIDPtr = localeID;

        // convert all underbars to hyphens, unless the "BCP47 extension" comes at the beginning of the string
        إذا (localeID.size() >= 2 && localeID.find('_') != std::string_view::npos && localeID[1] != '-' && localeID[1] != '_') {
            localeIDWithHyphens.append(localeID, err);
            إذا (U_SUCCESS(err)) {
                لكل (حرف* p = localeIDWithHyphens.انکڑے(); *p != '\0'; ++p) {
                    إذا (*p == '_') {
                        *p = '-';
                    }
                }
                localeIDPtr = localeIDWithHyphens.منزل_String_Piece();
            }
        }

        عارضی_بفر = ulocimp_for_Language_ٹیگ(localeIDPtr.انکڑے(), static_cast<int32_t>(localeIDPtr.size()), خالی_سوچک, err);
        tmpLocaleID = U_SUCCESS(err) && !عارضی_بفر.is_خالی_تھاواں() ? static_cast<std::string_view>(عارضی_بفر.منزل_String_Piece()) : localeIDPtr;
    } وإلا {
        tmpLocaleID=localeID;
    }

    origLocaleID=tmpLocaleID;

    /* get all pieces, one after another, and separate with '_' */
    CharString ٹیگ;
    CharString script;
    CharString country;
    CharString variant;
    ثابت حرف* سرا = خالی_سوچک;
    ulocimp_getSubtags(
            tmpLocaleID,
            &ٹیگ,
            &script,
            &country,
            &variant,
            &سرا,
            err);
    إذا (U_FAILURE(err)) {
        أرجع;
    }
    U_ASSERT(سرا != خالی_سوچک);
    إذا (سرا > tmpLocaleID.انکڑے()) {
        tmpLocaleID.remove_اگیتر(سرا - tmpLocaleID.انکڑے());
    }

    إذا (ٹیگ.length() == I_DEFAULT_لمائی && origLocaleID.length() >= I_DEFAULT_لمائی &&
            uprv_strncmp(origLocaleID.انکڑے(), i_default, I_DEFAULT_لمائی) == 0) {
        ٹیگ.clear();
        ٹیگ.append(uloc_getDefault(), err);
    } وإلا {
        إذا (!script.is_خالی_تھاواں()) {
            ++میدان_گنتی;
            ٹیگ.append('_', err);
            ٹیگ.append(script, err);
        }
        إذا (!country.is_خالی_تھاواں()) {
            ++میدان_گنتی;
            ٹیگ.append('_', err);
            ٹیگ.append(country, err);
        }
        إذا (!variant.is_خالی_تھاواں()) {
            ++میدان_گنتی;
            إذا (country.is_خالی_تھاواں()) {
                ٹیگ.append('_', err);
            }
            ٹیگ.append('_', err);
            ٹیگ.append(variant, err);
        }
    }

    /* Copy POSIX-style charset specifier, if any [mr.utf8] */
    إذا (!OPTION_سیٹ(options, _ULOC_CANONICALIZE) && !tmpLocaleID.empty() && tmpLocaleID.front() == '.') {
        ٹیگ.append('.', err);
        tmpLocaleID.remove_اگیتر(1);
        size_t length;
        إذا (size_t atPos = tmpLocaleID.find('@'); atPos != std::string_view::npos) {
            length = atPos;
        } وإلا {
            length = tmpLocaleID.length();
        }
        // The longest charset name we found in IANA charset registry
        // https://www.iana.org/assignments/character-sets/ is
        // "Extended_UNIX_Code_Packed_Format_for_Japanese" in length 45.
        // we therefore restrict the length here to be 64 which is a power of 2
        // number that is longer than 45.
        constexpr size_t k_Max_Charset_لمائی = 64;
        إذا (length > k_Max_Charset_لمائی) {
           err = U_ILLEGAL_دلیل_ERROR; /* malformed keyword name */
           أرجع;
        }
        إذا (length > 0) {
            ٹیگ.append(tmpLocaleID.انکڑے(), static_cast<int32_t>(length), err);
            tmpLocaleID.remove_اگیتر(length);
        }
    }

    /* Scan ahead to next '@' and determine if it is followed by '=' and/or ';'
       After this, tmpLocaleID either starts at '@' or is empty. */
    إذا (ثابت حرف* شروع = locale_get_Keywords_شروع(tmpLocaleID); شروع != خالی_سوچک) {
        إذا (شروع > tmpLocaleID.انکڑے()) {
            tmpLocaleID.remove_اگیتر(شروع - tmpLocaleID.انکڑے());
        }
        keyword_سونپو = tmpLocaleID.find('=');
        separatorIndicator = tmpLocaleID.find(';');
    } وإلا {
        tmpLocaleID = {};
    }

    /* Copy POSIX-style variant, if any [mr@FOO] */
    إذا (!OPTION_سیٹ(options, _ULOC_CANONICALIZE) &&
        !tmpLocaleID.empty() && keyword_سونپو == std::string_view::npos) {
        ٹیگ.append(tmpLocaleID, err);
        tmpLocaleID = {};
    }

    إذا (OPTION_سیٹ(options, _ULOC_CANONICALIZE)) {
        /* Handle @FOO variant if @ is present and not followed by = */
        إذا (!tmpLocaleID.empty() && keyword_سونپو == std::string_view::npos) {
            /* Add missing '_' if needed */
            إذا (میدان_گنتی < 2 || (میدان_گنتی < 3 && !script.is_خالی_تھاواں())) {
                افعل {
                    ٹیگ.append('_', err);
                    ++میدان_گنتی;
                } طالما(میدان_گنتی<2);
            }

            CharStringByteSink s(&ٹیگ);
            std::string_view sub = tmpLocaleID;
            sub.remove_اگیتر(1);
            _getVariant(sub, '@', &s, !variant.is_خالی_تھاواں(), err);
            إذا (U_FAILURE(err)) { أرجع; }
        }

        /* Look up the ID in the canonicalization map */
        لكل (j=0; j<UPRV_LENGTHOF(CANONICALIZE_MAP); j++) {
            StringPiece id(CANONICALIZE_MAP[j].id);
            إذا (ٹیگ == id) {
                إذا (id.empty() && !tmpLocaleID.empty()) {
                    اكسر; /* Don't remap "" if keywords present */
                }
                ٹیگ.clear();
                ٹیگ.append(CANONICALIZE_MAP[j].canonicalID, err);
                اكسر;
            }
        }
    }

    sink.Append(ٹیگ.انکڑے(), ٹیگ.length());

    إذا (!OPTION_سیٹ(options, _ULOC_STRIP_KEYWORDS)) {
        إذا (!tmpLocaleID.empty() && keyword_سونپو != std::string_view::npos &&
            (separatorIndicator == std::string_view::npos || separatorIndicator > keyword_سونپو)) {
            sink.Append("@", 1);
            ++میدان_گنتی;
            tmpLocaleID.remove_اگیتر(1);
            ulocimp_getKeywords(tmpLocaleID, '@', sink, سچ, err);
        }
    }
}

}  // namespace

/* ### ID parsing API **************************************************/

U_CAPI int32_t  U_EXPORT2
uloc_get_ماپا(ثابت حرف*    localeID,
               حرف* ماپا,
               int32_t ماپا_سمرتھا,
               UError_کوڈ* err)
{
    أرجع ByteSinkUtil::راہیں_Byte_Sink_منزل_Terminated_Chars(
        ماپا, ماپا_سمرتھا,
        [&](ByteSink& sink, UError_کوڈ& حالت) {
            ulocimp_get_ماپا(localeID, sink, حالت);
        },
        *err);
}

U_EXPORT CharString
ulocimp_get_ماپا(ثابت حرف* localeID,
                  UError_کوڈ& err)
{
    أرجع ByteSinkUtil::راہیں_Byte_Sink_منزل_Char_String(
        [&](ByteSink& sink, UError_کوڈ& حالت) {
            ulocimp_get_ماپا(localeID, sink, حالت);
        },
        err);
}

U_EXPORT فراغ
ulocimp_get_ماپا(ثابت حرف* localeID,
                  icu::ByteSink& sink,
                  UError_کوڈ& err)
{
    إذا (U_FAILURE(err)) { أرجع; }

    ثابت حرف *آخری_Underscore;
    int32_t i;

    إذا (localeID == خالی_سوچک)
        localeID = uloc_getDefault();

    آخری_Underscore=uprv_strrchr(localeID, '_');
    إذا(آخری_Underscore!=خالی_سوچک) {
        i = static_cast<int32_t>(آخری_Underscore - localeID);
    } وإلا {
        i=0;
    }

    إذا (i > 0) {
        إذا (uprv_strnicmp(localeID, "und_", 4) == 0) {
            localeID += 3;
            i -= 3;
        }
        sink.Append(localeID, i);
    }
}

U_CAPI int32_t U_EXPORT2
uloc_getLanguage(ثابت حرف*    localeID,
         حرف* language,
         int32_t language_سمرتھا,
         UError_کوڈ* err)
{
    إذا (localeID == خالی_سوچک) {
        localeID = uloc_getDefault();
    }

    /* uloc_getLanguage will return a 2 character iso-639 code if one exists. *CWB*/
    أرجع ByteSinkUtil::راہیں_Byte_Sink_منزل_Terminated_Chars(
        language, language_سمرتھا,
        [&](ByteSink& sink, UError_کوڈ& حالت) {
            ulocimp_getSubtags(
                    localeID,
                    &sink,
                    خالی_سوچک,
                    خالی_سوچک,
                    خالی_سوچک,
                    خالی_سوچک,
                    حالت);
        },
        *err);
}

U_CAPI int32_t U_EXPORT2
uloc_getScript(ثابت حرف*    localeID,
         حرف* script,
         int32_t script_سمرتھا,
         UError_کوڈ* err)
{
    إذا (localeID == خالی_سوچک) {
        localeID = uloc_getDefault();
    }

    أرجع ByteSinkUtil::راہیں_Byte_Sink_منزل_Terminated_Chars(
        script, script_سمرتھا,
        [&](ByteSink& sink, UError_کوڈ& حالت) {
            ulocimp_getSubtags(
                    localeID,
                    خالی_سوچک,
                    &sink,
                    خالی_سوچک,
                    خالی_سوچک,
                    خالی_سوچک,
                    حالت);
        },
        *err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getCountry(ثابت حرف* localeID,
            حرف* country,
            int32_t country_سمرتھا,
            UError_کوڈ* err)
{
    إذا (localeID == خالی_سوچک) {
        localeID = uloc_getDefault();
    }

    أرجع ByteSinkUtil::راہیں_Byte_Sink_منزل_Terminated_Chars(
        country, country_سمرتھا,
        [&](ByteSink& sink, UError_کوڈ& حالت) {
            ulocimp_getSubtags(
                    localeID,
                    خالی_سوچک,
                    خالی_سوچک,
                    &sink,
                    خالی_سوچک,
                    خالی_سوچک,
                    حالت);
        },
        *err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getVariant(ثابت حرف* localeID,
                حرف* variant,
                int32_t variant_سمرتھا,
                UError_کوڈ* err)
{
    إذا (localeID == خالی_سوچک) {
        localeID = uloc_getDefault();
    }

    أرجع ByteSinkUtil::راہیں_Byte_Sink_منزل_Terminated_Chars(
        variant, variant_سمرتھا,
        [&](ByteSink& sink, UError_کوڈ& حالت) {
            ulocimp_getSubtags(
                    localeID,
                    خالی_سوچک,
                    خالی_سوچک,
                    خالی_سوچک,
                    &sink,
                    خالی_سوچک,
                    حالت);
        },
        *err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getName(ثابت حرف* localeID,
             حرف* name,
             int32_t name_سمرتھا,
             UError_کوڈ* err)
{
    إذا (localeID == خالی_سوچک) {
        localeID = uloc_getDefault();
    }
    أرجع ByteSinkUtil::راہیں_Byte_Sink_منزل_Terminated_Chars(
        name, name_سمرتھا,
        [&](ByteSink& sink, UError_کوڈ& حالت) {
            ulocimp_getName(localeID, sink, حالت);
        },
        *err);
}

U_EXPORT CharString
ulocimp_getName(std::string_view localeID,
                UError_کوڈ& err)
{
    أرجع ByteSinkUtil::راہیں_Byte_Sink_منزل_Char_String(
        [&](ByteSink& sink, UError_کوڈ& حالت) {
            ulocimp_getName(localeID, sink, حالت);
        },
        err);
}

U_EXPORT فراغ
ulocimp_getName(std::string_view localeID,
                ByteSink& sink,
                UError_کوڈ& err)
{
    _canonicalize(localeID, sink, 0, err);
}

U_CAPI int32_t  U_EXPORT2
uloc_get_ادھار_Name(ثابت حرف* localeID,
                 حرف* name,
                 int32_t name_سمرتھا,
                 UError_کوڈ* err)
{
    إذا (localeID == خالی_سوچک) {
        localeID = uloc_getDefault();
    }
    أرجع ByteSinkUtil::راہیں_Byte_Sink_منزل_Terminated_Chars(
        name, name_سمرتھا,
        [&](ByteSink& sink, UError_کوڈ& حالت) {
            ulocimp_get_ادھار_Name(localeID, sink, حالت);
        },
        *err);
}

U_EXPORT CharString
ulocimp_get_ادھار_Name(std::string_view localeID,
                    UError_کوڈ& err)
{
    أرجع ByteSinkUtil::راہیں_Byte_Sink_منزل_Char_String(
        [&](ByteSink& sink, UError_کوڈ& حالت) {
            ulocimp_get_ادھار_Name(localeID, sink, حالت);
        },
        err);
}

U_EXPORT فراغ
ulocimp_get_ادھار_Name(std::string_view localeID,
                    ByteSink& sink,
                    UError_کوڈ& err)
{
    _canonicalize(localeID, sink, _ULOC_STRIP_KEYWORDS, err);
}

U_CAPI int32_t  U_EXPORT2
uloc_canonicalize(ثابت حرف* localeID,
                  حرف* name,
                  int32_t name_سمرتھا,
                  UError_کوڈ* err)
{
    إذا (localeID == خالی_سوچک) {
        localeID = uloc_getDefault();
    }
    أرجع ByteSinkUtil::راہیں_Byte_Sink_منزل_Terminated_Chars(
        name, name_سمرتھا,
        [&](ByteSink& sink, UError_کوڈ& حالت) {
            ulocimp_canonicalize(localeID, sink, حالت);
        },
        *err);
}

U_EXPORT CharString
ulocimp_canonicalize(std::string_view localeID,
                     UError_کوڈ& err)
{
    أرجع ByteSinkUtil::راہیں_Byte_Sink_منزل_Char_String(
        [&](ByteSink& sink, UError_کوڈ& حالت) {
            ulocimp_canonicalize(localeID, sink, حالت);
        },
        err);
}

U_EXPORT فراغ
ulocimp_canonicalize(std::string_view localeID,
                     ByteSink& sink,
                     UError_کوڈ& err)
{
    _canonicalize(localeID, sink, _ULOC_CANONICALIZE, err);
}

U_CAPI ثابت حرف*  U_EXPORT2
uloc_getISO3Language(ثابت حرف* localeID)
{
    UError_کوڈ err = U_ZERO_ERROR;

    إذا (localeID == خالی_سوچک)
    {
        localeID = uloc_getDefault();
    }
    CharString lang = ulocimp_getLanguage(localeID, err);
    إذا (U_FAILURE(err))
        أرجع "";
    std::optional<int16_t> وتھ = لبھو_اشاریہ_2(LANGUAGES, lang.انکڑے());
    أرجع وتھ.has_value() ? LANGUAGES_3[*وتھ] : "";
}

U_CAPI ثابت حرف*  U_EXPORT2
uloc_getISO3Country(ثابت حرف* localeID)
{
    UError_کوڈ err = U_ZERO_ERROR;

    إذا (localeID == خالی_سوچک)
    {
        localeID = uloc_getDefault();
    }
    CharString cntry = ulocimp_getRegion(localeID, err);
    إذا (U_FAILURE(err))
        أرجع "";
    std::optional<int16_t> وتھ = لبھو_اشاریہ_2(COUNTRIES, cntry.انکڑے());
    أرجع وتھ.has_value() ? COUNTRIES_3[*وتھ] : "";
}

U_CAPI uint32_t  U_EXPORT2
uloc_getLCID(ثابت حرف* localeID)
{
    UError_کوڈ حالت = U_ZERO_ERROR;
    uint32_t   lcid = 0;

    /* Check for incomplete id. */
    إذا (!localeID || uprv_strlen(localeID) < 2) {
        أرجع 0;
    }

    // First, attempt Windows platform lookup if available, but fall
    // through to catch any special cases (ICU vs Windows name differences).
    lcid = uprv_convert_منزل_LCIDPlatform(localeID, &حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع 0;
    }
    إذا (lcid > 0) {
        // Windows found an LCID, return that
        أرجع lcid;
    }

    CharString langID = ulocimp_getLanguage(localeID, حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع 0;
    }

    إذا (uprv_strchr(localeID, '@')) {
        // uprv_convertToLCID does not support keywords other than collation.
        // Remove all keywords except collation.
        CharString collVal = ulocimp_get_Keyword_قدر(localeID, "collation", حالت);
        إذا (U_SUCCESS(حالت) && !collVal.is_خالی_تھاواں()) {
            CharString tmpLocaleID = ulocimp_get_ادھار_Name(localeID, حالت);
            ulocimp_سیٹ_Keyword_قدر("collation", collVal.منزل_String_Piece(), tmpLocaleID, حالت);
            إذا (U_SUCCESS(حالت)) {
                أرجع uprv_convert_منزل_LCID(langID.انکڑے(), tmpLocaleID.انکڑے(), &حالت);
            }
        }

        // fall through - all keywords are simply ignored
        حالت = U_ZERO_ERROR;
    }

    أرجع uprv_convert_منزل_LCID(langID.انکڑے(), localeID, &حالت);
}

U_CAPI int32_t U_EXPORT2
uloc_getLocaleForLCID(uint32_t hostid, حرف *locale, int32_t locale_سمرتھا,
                UError_کوڈ *حالت)
{
    أرجع uprv_convert_منزل_Posix(hostid, locale, locale_سمرتھا, حالت);
}

/* ### Default locale **************************************************/

U_CAPI ثابت حرف*  U_EXPORT2
uloc_getDefault()
{
    أرجع locale_get_default();
}

U_CAPI فراغ  U_EXPORT2
uloc_سیٹ_Default(ثابت حرف*   newDefaultLocale,
             UError_کوڈ* err)
{
    إذا (U_FAILURE(*err))
        أرجع;
    /* the error code isn't currently used for anything by this function*/

    /* propagate change to C++ */
    locale_سیٹ_default(newDefaultLocale);
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
uloc_منزل_Unicode_Locale_کلید(ثابت حرف* keyword U_LIFETIME_BOUND)
{
    إذا (keyword == خالی_سوچک || *keyword == '\0') { أرجع خالی_سوچک; }
    std::optional<std::string_view> نتیجہ = ulocimp_منزل_Bcp_کلید_With_Fallback(keyword);
    أرجع نتیجہ.has_value() ? نتیجہ->انکڑے() : خالی_سوچک;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_منزل_Bcp_کلید_With_Fallback(std::string_view keyword U_LIFETIME_BOUND)
{
    std::optional<std::string_view> bcp_کلید = ulocimp_منزل_Bcp_کلید(keyword);
    إذا (!bcp_کلید.has_value() &&
        ultag_is_Unicode_Locale_کلید(keyword.انکڑے(), static_cast<int32_t>(keyword.size()))) {
        // unknown keyword, but syntax is fine..
        أرجع keyword;
    }
    أرجع bcp_کلید;
}

U_CAPI ثابت حرف* U_EXPORT2
uloc_منزل_Unicode_Locale_Type(ثابت حرف* keyword, ثابت حرف* value U_LIFETIME_BOUND)
{
    إذا (keyword == خالی_سوچک || *keyword == '\0' ||
        value == خالی_سوچک || *value == '\0') { أرجع خالی_سوچک; }
    std::optional<std::string_view> نتیجہ = ulocimp_منزل_Bcp_Type_With_Fallback(keyword, value);
    أرجع نتیجہ.has_value() ? نتیجہ->انکڑے() : خالی_سوچک;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_منزل_Bcp_Type_With_Fallback(std::string_view keyword, std::string_view value U_LIFETIME_BOUND)
{
    std::optional<std::string_view> bcpType = ulocimp_منزل_Bcp_Type(keyword, value);
    إذا (!bcpType.has_value() &&
        ultag_isUnicodeLocaleType(value.انکڑے(), static_cast<int32_t>(value.size()))) {
        // unknown keyword, but syntax is fine..
        أرجع value;
    }
    أرجع bcpType;
}

نطاق {

منطقي
is_Well_Formed_Legacy_کلید(std::string_view کلید)
{
    أرجع std::all_of(کلید.begin(), کلید.سرا(), UPRV_ISALPHANUM);
}

منطقي
isWellFormedLegacyType(std::string_view legacyType)
{
    int32_t alpha_عدد_Len = 0;
    لكل (حرف c : legacyType) {
        إذا (c == '_' || c == '/' || c == '-') {
            إذا (alpha_عدد_Len == 0) {
                أرجع خطأ;
            }
            alpha_عدد_Len = 0;
        } وإلا إذا (UPRV_ISALPHANUM(c)) {
            alpha_عدد_Len++;
        } وإلا {
            أرجع خطأ;
        }
    }
    أرجع alpha_عدد_Len != 0;
}

}  // namespace

U_CAPI ثابت حرف* U_EXPORT2
uloc_منزل_Legacy_کلید(ثابت حرف* keyword U_LIFETIME_BOUND)
{
    إذا (keyword == خالی_سوچک || *keyword == '\0') { أرجع خالی_سوچک; }
    std::optional<std::string_view> نتیجہ = ulocimp_منزل_Legacy_کلید_With_Fallback(keyword);
    أرجع نتیجہ.has_value() ? نتیجہ->انکڑے() : خالی_سوچک;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_منزل_Legacy_کلید_With_Fallback(std::string_view keyword U_LIFETIME_BOUND)
{
    std::optional<std::string_view> legacy_کلید_2 = ulocimp_منزل_Legacy_کلید(keyword);
    إذا (!legacy_کلید_2.has_value() && is_Well_Formed_Legacy_کلید(keyword)) {
        // Checks if the specified locale key is well-formed with the legacy locale syntax.
        //
        // Note:
        //  LDML/CLDR provides some definition of keyword syntax in
        //  * http://www.unicode.org/reports/tr35/#Unicode_locale_identifier and
        //  * http://www.unicode.org/reports/tr35/#Old_Locale_Extension_Syntax
        //  Keys can only consist of [0-9a-zA-Z].
        أرجع keyword;
    }
    أرجع legacy_کلید_2;
}

U_CAPI ثابت حرف* U_EXPORT2
uloc_منزل_Legacy_Type(ثابت حرف* keyword, ثابت حرف* value U_LIFETIME_BOUND)
{
    إذا (keyword == خالی_سوچک || *keyword == '\0' ||
        value == خالی_سوچک || *value == '\0') { أرجع خالی_سوچک; }
    std::optional<std::string_view> نتیجہ = ulocimp_منزل_Legacy_Type_With_Fallback(keyword, value);
    أرجع نتیجہ.has_value() ? نتیجہ->انکڑے() : خالی_سوچک;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_منزل_Legacy_Type_With_Fallback(std::string_view keyword, std::string_view value U_LIFETIME_BOUND)
{
    std::optional<std::string_view> legacyType = ulocimp_منزل_Legacy_Type(keyword, value);
    إذا (!legacyType.has_value() && isWellFormedLegacyType(value)) {
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
