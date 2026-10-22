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
U_CFUNC فراغ locale_سيٽ_default(ثابت حرف *id);
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
خالي_اشارو,
    "in",  "iw",  "ji",  "jw",  "mo",  "sh",  "swc", "tl",  /* obsolete language codes */
خالي_اشارو
};

constexpr ثابت حرف* DEPRECATED_LANGUAGES[]={
    "in", "iw", "ji", "jw", "mo", خالي_اشارو, خالي_اشارو
};
constexpr ثابت حرف* REPLACEMENT_LANGUAGES[]={
    "id", "he", "yi", "jv", "ro", خالي_اشارو, خالي_اشارو
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
خالي_اشارو,
/*  "in",  "iw",  "ji",  "jw",  "mo",  "sh",  "swc", "tl",  */
    "ind", "heb", "yid", "jaw", "mol", "srp", "swc", "tgl",
خالي_اشارو
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
خالي_اشارو,
    "AN",  "BU", "CS", "FX", "RO", "SU", "TP", "YD", "YU", "ZR",   /* obsolete country codes */
خالي_اشارو
};

constexpr ثابت حرف* DEPRECATED_COUNTRIES[] = {
    "AN", "BU", "CS", "DD", "DY", "FX", "HV", "NH", "RH", "SU", "TP", "UK", "VD", "YD", "YU", "ZR", خالي_اشارو, خالي_اشارو /* deprecated country list */
};
constexpr ثابت حرف* REPLACEMENT_COUNTRIES[] = {
/*  "AN", "BU", "CS", "DD", "DY", "FX", "HV", "NH", "RH", "SU", "TP", "UK", "VD", "YD", "YU", "ZR" */
    "CW", "MM", "RS", "DE", "BJ", "FR", "BF", "VU", "ZW", "RU", "TL", "GB", "VN", "YE", "RS", "CD", خالي_اشارو, خالي_اشارو  /* replacement country codes */
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
خالي_اشارو,
/*  "AN",  "BU",  "CS",  "FX",  "RO", "SU",  "TP",  "YD",  "YU",  "ZR" */
    "ANT", "BUR", "SCG", "FXX", "ROM", "SUN", "TMP", "YMD", "YUG", "ZAR",
خالي_اشارو
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
int32_t get_سڀ_کان_ٿورو_Subtag_ڊيگهائي(std::string_view localeID) {
    int32_t localeIDLength = static_cast<int32_t>(localeID.length());
    int32_t length = localeIDLength;
    int32_t tmp_ڊيگهائي = 0;
    int32_t i;
    منطقي reset = سچ;

    لكل (i = 0; i < localeIDLength; i++) {
        إذا (localeID[i] != '_' && localeID[i] != '-') {
            إذا (reset) {
                tmp_ڊيگهائي = 0;
                reset = خطأ;
            }
            tmp_ڊيگهائي++;
        } وإلا {
            إذا (tmp_ڊيگهائي != 0 && tmp_ڊيگهائي < length) {
                length = tmp_ڊيگهائي;
            }
            reset = سچ;
        }
    }

    أرجع length;
}
/* Test if the locale id has BCP47 u extension and does not have '@' */
مضمن منطقي _hasBCP47Extension(std::string_view id) {
    أرجع id.find('@') == std::string_view::npos && get_سڀ_کان_ٿورو_Subtag_ڊيگهائي(id) == 1;
}

/* ### Keywords **************************************************/
مضمن منطقي UPRV_ISDIGIT(حرف c) { أرجع c >= '0' && c <= '9'; }
مضمن منطقي UPRV_ISALPHANUM(حرف c) { أرجع uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }
/* Punctuation/symbols allowed in legacy key values */
مضمن منطقي UPRV_OK_قدر_PUNCTUATION(حرف c) { أرجع c == '_' || c == '-' || c == '+' || c == '/'; }

}  // namespace

#وصف_ڪريو ULOC_KEYWORD_بفر_LEN 25
#وصف_ڪريو ULOC_MAX_NO_KEYWORDS 25

U_CAPI ثابت حرف* U_EXPORT2
locale_get_Keywords_شروعات(std::string_view localeID U_LIFETIME_BOUND) {
    إذا (size_t pos = localeID.find('@'); pos != std::string_view::npos) {
        أرجع localeID.معلومات() + pos;
    }
#إذا (U_CHARSET_FAMILY == U_EBCDIC_FAMILY)
    وإلا {
        /* We do this because the @ sign is variant, and the @ sign used on one
        EBCDIC machine won't be compiled the same way on other EBCDIC based
        machines. */
        ساكن ثابت uint8_t ebcdicSigns[] = { 0x7C, 0x44, 0x66, 0x80, 0xAC, 0xAE, 0xAF, 0xB5, 0xEC, 0xEF, 0x00 };
        ثابت uint8_t *char_منزل_ڳوليو = ebcdicSigns;
        طالما(*char_منزل_ڳوليو) {
            إذا (size_t pos = localeID.find(*char_منزل_ڳوليو); pos != std::string_view::npos) {
                أرجع localeID.معلومات() + pos;
            }
            char_منزل_ڳوليو++;
        }
    }
#جيڪڏهن_پورو
    أرجع خالي_اشارو;
}

نطاق {

/**
 * @param keywordName incoming name to be canonicalized
 * @param status return status (keyword too long)
 * @return the keyword name
 */
CharString locale_canonKeywordName(std::string_view keywordName, UError_ڪوڊ& حالت)
{
  إذا (U_FAILURE(حالت)) { أرجع {}; }
  CharString نتيجو;

  لكل (حرف c : keywordName) {
    إذا (!UPRV_ISALPHANUM(c)) {
      حالت = U_ILLEGAL_دليل_ERROR; /* malformed keyword name */
      أرجع {};
    }
    نتيجو.append(uprv_tolower(c), حالت);
  }
  إذا (نتيجو.is_خالي_جايون()) {
    حالت = U_ILLEGAL_دليل_ERROR; /* empty keyword name */
    أرجع {};
  }

  أرجع نتيجو;
}

تعريفنوع بنية {
    حرف keyword[ULOC_KEYWORD_بفر_LEN];
    int32_t keywordLen;
    ثابت حرف *قدر_شروعات;
    int32_t قدر_Len;
} KeywordStruct;

int32_t U_CALLCONV
compareKeywordStructs(ثابت فراغ * /*context*/, ثابت فراغ *کاٻي, ثابت فراغ *ساڄي) {
    ثابت حرف* کاٻو_String = static_cast<ثابت KeywordStruct*>(کاٻي)->keyword;
    ثابت حرف* ساڄو_String = static_cast<ثابت KeywordStruct*>(ساڄي)->keyword;
    أرجع uprv_strcmp(کاٻو_String, ساڄو_String);
}

}  // namespace

U_EXPORT CharString
ulocimp_getKeywords(std::string_view localeID,
                    حرف prev,
                    منطقي قدرون_Too,
                    UError_ڪوڊ& حالت)
{
    أرجع ByteSinkUtil::ذريعي_Byte_Sink_منزل_Char_String(
        [&](ByteSink& sink, UError_ڪوڊ& حالت) {
            ulocimp_getKeywords(localeID,
                                prev,
                                sink,
                                قدرون_Too,
                                حالت);
        },
        حالت);
}

U_EXPORT فراغ
ulocimp_getKeywords(std::string_view localeID,
                    حرف prev,
                    ByteSink& sink,
                    منطقي قدرون_Too,
                    UError_ڪوڊ& حالت)
{
    إذا (U_FAILURE(حالت)) { أرجع; }

    KeywordStruct keywordList[ULOC_MAX_NO_KEYWORDS];

    int32_t maxKeywords = ULOC_MAX_NO_KEYWORDS;
    int32_t انگ_Keywords = 0;
    size_t equalSign = std::string_view::npos;
    size_t semicolon = std::string_view::npos;
    int32_t i = 0, j, n;

    إذا(prev == '@') { /* start of keyword definition */
        /* we will grab pairs, trim spaces, lowercase keywords, sort and return */
        افعل {
            منطقي duplicate = خطأ;
            /* skip leading spaces */
            طالما (!localeID.empty() && localeID.front() == ' ') {
                localeID.remove_اڳياڙي(1);
            }
            إذا (localeID.empty()) { /* handle trailing "; " */
                اكسر;
            }
            إذا(انگ_Keywords == maxKeywords) {
                حالت = U_اندروني_پروگرام_ERROR;
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
                حالت = U_اندروني_پروگرام_ERROR;
                أرجع;
            }
            لكل (i = 0, n = 0; static_cast<size_t>(i) < equalSign; ++i) {
                إذا (localeID[i] != ' ') {
                    keywordList[انگ_Keywords].keyword[n++] = uprv_tolower(localeID[i]);
                }
            }

            keywordList[انگ_Keywords].keyword[n] = 0;
            keywordList[انگ_Keywords].keywordLen = n;
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

            keywordList[انگ_Keywords].قدر_شروعات = localeID.معلومات() + equalSign;

            std::string_view value = localeID;
            إذا (semicolon != std::string_view::npos) {
                value.remove_suffix(value.length() - semicolon);
                localeID.remove_اڳياڙي(semicolon + 1);
            } وإلا {
                localeID = {};
            }
            value.remove_اڳياڙي(equalSign);
            إذا (size_t آخري = value.find_last_not_of(' '); آخري != std::string_view::npos) {
                value.remove_suffix(value.length() - آخري - 1);
            }
            keywordList[انگ_Keywords].قدر_Len = static_cast<int32_t>(value.length());

            /* If this is a duplicate keyword, then ignore it */
            لكل (j=0; j<انگ_Keywords; ++j) {
                إذا (uprv_strcmp(keywordList[j].keyword, keywordList[انگ_Keywords].keyword) == 0) {
                    duplicate = سچ;
                    اكسر;
                }
            }
            إذا (!duplicate) {
                ++انگ_Keywords;
            }
        } طالما (!localeID.empty());

        /* now we have a list of keywords */
        /* we need to sort it */
        uprv_ترتيب_ڏيو_Array(keywordList, انگ_Keywords, حجم(KeywordStruct), compareKeywordStructs, خالي_اشارو, خطأ, &حالت);

        /* Now construct the keyword part */
        لكل(i = 0; i < انگ_Keywords; i++) {
            sink.Append(keywordList[i].keyword, keywordList[i].keywordLen);
            إذا(قدرون_Too) {
                sink.Append("=", 1);
                sink.Append(keywordList[i].قدر_شروعات, keywordList[i].قدر_Len);
                إذا(i < انگ_Keywords - 1) {
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
                     حرف* بفر, int32_t بفر_صلاحيت,
                     UError_ڪوڊ* حالت)
{
    إذا (U_FAILURE(*حالت)) { أرجع 0; }
    إذا (keywordName == خالي_اشارو || *keywordName == '\0') {
        *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }
    أرجع ByteSinkUtil::ذريعي_Byte_Sink_منزل_Terminated_Chars(
        بفر, بفر_صلاحيت,
        [&](ByteSink& sink, UError_ڪوڊ& حالت) {
            ulocimp_get_Keyword_قدر(localeID, keywordName, sink, حالت);
        },
        *حالت);
}

U_EXPORT CharString
ulocimp_get_Keyword_قدر(ثابت حرف* localeID,
                        std::string_view keywordName,
                        UError_ڪوڊ& حالت)
{
    أرجع ByteSinkUtil::ذريعي_Byte_Sink_منزل_Char_String(
        [&](ByteSink& sink, UError_ڪوڊ& حالت) {
            ulocimp_get_Keyword_قدر(localeID, keywordName, sink, حالت);
        },
        حالت);
}

U_EXPORT فراغ
ulocimp_get_Keyword_قدر(ثابت حرف* localeID,
                        std::string_view keywordName,
                        icu::ByteSink& sink,
                        UError_ڪوڊ& حالت)
{
    إذا (U_FAILURE(حالت)) { أرجع; }

    إذا (localeID == خالي_اشارو || keywordName.empty()) {
        حالت = U_ILLEGAL_دليل_ERROR;
        أرجع;
    }

    ثابت حرف* شروعات_ڳولا_هتي = خالي_اشارو;
    ثابت حرف* ايندڙ_Separator = خالي_اشارو;

    CharString عارضي_بفر;
    ثابت حرف* tmpLocaleID;

    CharString canonKeywordName = locale_canonKeywordName(keywordName, حالت);
    إذا (U_FAILURE(حالت)) {
      أرجع;
    }

    إذا (localeID != خالي_اشارو && _hasBCP47Extension(localeID)) {
        عارضي_بفر = ulocimp_for_Language_ٽيگ(localeID, -1, خالي_اشارو, حالت);
        tmpLocaleID = U_SUCCESS(حالت) && !عارضي_بفر.is_خالي_جايون() ? عارضي_بفر.معلومات() : localeID;
    } وإلا {
        tmpLocaleID=localeID;
    }

    شروعات_ڳولا_هتي = locale_get_Keywords_شروعات(tmpLocaleID);
    إذا(شروعات_ڳولا_هتي == خالي_اشارو) {
        /* no keywords, return at once */
        أرجع;
    }

    /* find the first keyword */
    طالما(شروعات_ڳولا_هتي) {
        ثابت حرف* ڪنجي_قدر_پڇ;

        شروعات_ڳولا_هتي++; /* skip @ or ; */
        ايندڙ_Separator = uprv_strchr(شروعات_ڳولا_هتي, '=');
        إذا(!ايندڙ_Separator) {
            حالت = U_ILLEGAL_دليل_ERROR; /* key must have =value */
            أرجع;
        }
        /* strip leading & trailing spaces (TC decided to tolerate these) */
        طالما(*شروعات_ڳولا_هتي == ' ') {
            شروعات_ڳولا_هتي++;
        }
        ڪنجي_قدر_پڇ = ايندڙ_Separator;
        طالما (ڪنجي_قدر_پڇ > شروعات_ڳولا_هتي && *(ڪنجي_قدر_پڇ-1) == ' ') {
            ڪنجي_قدر_پڇ--;
        }
        /* now keyValueTail points to first char after the keyName */
        /* copy & normalize keyName from locale */
        إذا (شروعات_ڳولا_هتي == ڪنجي_قدر_پڇ) {
            حالت = U_ILLEGAL_دليل_ERROR; /* empty keyword name in passed-in locale */
            أرجع;
        }
        CharString localeKeywordName;
        طالما (شروعات_ڳولا_هتي < ڪنجي_قدر_پڇ) {
          إذا (!UPRV_ISALPHANUM(*شروعات_ڳولا_هتي)) {
            حالت = U_ILLEGAL_دليل_ERROR; /* malformed keyword name */
            أرجع;
          }
          localeKeywordName.append(uprv_tolower(*شروعات_ڳولا_هتي++), حالت);
        }
        إذا (U_FAILURE(حالت)) {
            أرجع;
        }

        شروعات_ڳولا_هتي = uprv_strchr(ايندڙ_Separator, ';');

        إذا (canonKeywordName == localeKeywordName) {
             /* current entry matches the keyword. */
           ايندڙ_Separator++; /* skip '=' */
            /* First strip leading & trailing spaces (TC decided to tolerate these) */
            طالما(*ايندڙ_Separator == ' ') {
              ايندڙ_Separator++;
            }
            ڪنجي_قدر_پڇ = (شروعات_ڳولا_هتي)? شروعات_ڳولا_هتي: ايندڙ_Separator + uprv_strlen(ايندڙ_Separator);
            طالما(ڪنجي_قدر_پڇ > ايندڙ_Separator && *(ڪنجي_قدر_پڇ-1) == ' ') {
              ڪنجي_قدر_پڇ--;
            }
            /* Now copy the value, but check well-formedness */
            إذا (ايندڙ_Separator == ڪنجي_قدر_پڇ) {
              حالت = U_ILLEGAL_دليل_ERROR; /* empty key value name in passed-in locale */
              أرجع;
            }
            طالما (ايندڙ_Separator < ڪنجي_قدر_پڇ) {
              إذا (!UPRV_ISALPHANUM(*ايندڙ_Separator) && !UPRV_OK_قدر_PUNCTUATION(*ايندڙ_Separator)) {
                حالت = U_ILLEGAL_دليل_ERROR; /* malformed key value */
                أرجع;
              }
              /* Should we lowercase value to return here? Tests expect as-is. */
              sink.Append(ايندڙ_Separator++, 1);
            }
            أرجع;
        }
    }
}

U_CAPI int32_t U_EXPORT2
uloc_سيٽ_Keyword_قدر(ثابت حرف* keywordName,
                     ثابت حرف* keyword_قدر,
                     حرف* بفر, int32_t بفر_صلاحيت,
                     UError_ڪوڊ* حالت)
{
    إذا (U_FAILURE(*حالت)) { أرجع 0; }

    إذا (keywordName == خالي_اشارو || *keywordName == 0) {
        *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    إذا (بفر_صلاحيت <= 1) {
        *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    int32_t bufLen = (int32_t)uprv_strlen(بفر);
    إذا(بفر_صلاحيت<bufLen) {
        /* The capacity is less than the length?! Is this NUL terminated? */
        *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    حرف* keywords = const_cast<حرف*>(
        locale_get_Keywords_شروعات({بفر, static_cast<std::string_view::size_type>(bufLen)}));
    int32_t بنياد_Len = keywords == خالي_اشارو ? bufLen : keywords - بفر;
    // Remove -1 from the capacity so that this function can guarantee NUL termination.
    CheckedArrayByteSink sink(keywords == خالي_اشارو ? بفر + bufLen : keywords,
                              بفر_صلاحيت - بنياد_Len - 1);
    int32_t reslen = ulocimp_سيٽ_Keyword_قدر(
        keywords == خالي_اشارو ? std::string_view() : keywords,
        keywordName,
        keyword_قدر == خالي_اشارو ? std::string_view() : keyword_قدر,
        sink,
        *حالت);

    إذا (U_FAILURE(*حالت)) {
        أرجع *حالت == U_بفر_حد_کان_مٿي_ERROR ? reslen + بنياد_Len : 0;
    }

    // See the documentation for this function, it's guaranteed to never
    // overflow the buffer but instead abort with BUFFER_OVERFLOW_ERROR.
    // In this case, nothing has been written to the sink, so it cannot have Overflowed().
    U_ASSERT(!sink.Overflowed());
    U_ASSERT(reslen >= 0);
    أرجع u_terminateChars(بفر, بفر_صلاحيت, reslen + بنياد_Len, حالت);
}

U_EXPORT فراغ
ulocimp_سيٽ_Keyword_قدر(std::string_view keywordName,
                        std::string_view keyword_قدر,
                        CharString& localeID,
                        UError_ڪوڊ& حالت)
{
    إذا (U_FAILURE(حالت)) { أرجع; }
    std::string_view keywords;
    إذا (ثابت حرف* شروعات = locale_get_Keywords_شروعات(localeID.منزل_String_Piece()); شروعات != خالي_اشارو) {
        // This is safe because CharString::truncate() doesn't actually erase any
        // data, but simply sets the position for where new data will be written.
        int32_t size = شروعات - localeID.معلومات();
        keywords = localeID.منزل_String_Piece();
        keywords.remove_اڳياڙي(size);
        localeID.truncate(size);
    }
    CharStringByteSink sink(&localeID);
    ulocimp_سيٽ_Keyword_قدر(keywords, keywordName, keyword_قدر, sink, حالت);
}

U_EXPORT int32_t
ulocimp_سيٽ_Keyword_قدر(std::string_view keywords,
                        std::string_view keywordName,
                        std::string_view keyword_قدر,
                        ByteSink& sink,
                        UError_ڪوڊ& حالت)
{
    إذا (U_FAILURE(حالت)) { أرجع 0; }

    /* TODO: sorting. removal. */
    int32_t needLen = 0;
    int32_t rc;
    CharString updated_ڪنجيون_And_قدرون;
    منطقي handled_داخلا_ڪنجي_And_قدر = خطأ;
    حرف ڪنجي_قدر_اڳياڙي = '@';

    إذا (حالت == U_STRING_NOT_TERMINATED_WARNING) {
        حالت = U_ZERO_ERROR;
    }
    إذا (keywordName.empty()) {
        حالت = U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }
    CharString canonKeywordName = locale_canonKeywordName(keywordName, حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع 0;
    }

    CharString canon_Keyword_قدر;
    لكل (حرف c : keyword_قدر) {
        إذا (!UPRV_ISALPHANUM(c) && !UPRV_OK_قدر_PUNCTUATION(c)) {
            حالت = U_ILLEGAL_دليل_ERROR; /* malformed key value */
            أرجع 0;
        }
        /* Should we force lowercase in value to set? */
        canon_Keyword_قدر.append(c, حالت);
    }
    إذا (U_FAILURE(حالت)) {
        أرجع 0;
    }

    إذا (keywords.size() <= 1) {
        إذا (canon_Keyword_قدر.is_خالي_جايون()) { /* no keywords = nothing to remove */
            U_ASSERT(حالت != U_STRING_NOT_TERMINATED_WARNING);
            أرجع 0;
        }

        needLen = 1 + canonKeywordName.length() + 1 + canon_Keyword_قدر.length();
        int32_t capacity = 0;
        حرف* بفر = sink.Get_Append_بفر(
                needLen, needLen, خالي_اشارو, needLen, &capacity);
        إذا (capacity < needLen || بفر == خالي_اشارو) {
            حالت = U_بفر_حد_کان_مٿي_ERROR;
            أرجع needLen; /* no change */
        }
        حرف* it = بفر;

        *it++ = '@';
        uprv_memcpy(it, canonKeywordName.معلومات(), canonKeywordName.length());
        it += canonKeywordName.length();
        *it++ = '=';
        uprv_memcpy(it, canon_Keyword_قدر.معلومات(), canon_Keyword_قدر.length());
        sink.Append(بفر, needLen);
        U_ASSERT(حالت != U_STRING_NOT_TERMINATED_WARNING);
        أرجع needLen;
    } /* end shortcut - no @ */

    /* search for keyword */
    لكل (size_t keyword_شروعات = 0; keyword_شروعات != std::string_view::npos;) {
        keyword_شروعات++; /* skip @ or ; */
        size_t ايندڙ_Equalsign = keywords.find('=', keyword_شروعات);
        إذا (ايندڙ_Equalsign == std::string_view::npos) {
            حالت = U_ILLEGAL_دليل_ERROR; /* key must have =value */
            أرجع 0;
        }
        /* strip leading & trailing spaces (TC decided to tolerate these) */
        طالما (keyword_شروعات < keywords.size() && keywords[keyword_شروعات] == ' ') {
            keyword_شروعات++;
        }
        size_t ڪنجي_قدر_پڇ = ايندڙ_Equalsign;
        طالما (ڪنجي_قدر_پڇ > keyword_شروعات && keywords[ڪنجي_قدر_پڇ - 1] == ' ') {
            ڪنجي_قدر_پڇ--;
        }
        /* now keyValueTail points to first char after the keyName */
        /* copy & normalize keyName from locale */
        إذا (keyword_شروعات == ڪنجي_قدر_پڇ) {
            حالت = U_ILLEGAL_دليل_ERROR; /* empty keyword name in passed-in locale */
            أرجع 0;
        }
        CharString localeKeywordName;
        طالما (keyword_شروعات < ڪنجي_قدر_پڇ) {
            إذا (!UPRV_ISALPHANUM(keywords[keyword_شروعات])) {
                حالت = U_ILLEGAL_دليل_ERROR; /* malformed keyword name */
                أرجع 0;
            }
            localeKeywordName.append(uprv_tolower(keywords[keyword_شروعات++]), حالت);
        }
        إذا (U_FAILURE(حالت)) {
            أرجع 0;
        }

        size_t ايندڙ_Separator = keywords.find(';', ايندڙ_Equalsign);

        /* start processing the value part */
        ايندڙ_Equalsign++; /* skip '=' */
        /* First strip leading & trailing spaces (TC decided to tolerate these) */
        طالما (ايندڙ_Equalsign < keywords.size() && keywords[ايندڙ_Equalsign] == ' ') {
            ايندڙ_Equalsign++;
        }
        ڪنجي_قدر_پڇ = ايندڙ_Separator == std::string_view::npos ? keywords.size() : ايندڙ_Separator;
        طالما (ڪنجي_قدر_پڇ > ايندڙ_Equalsign && keywords[ڪنجي_قدر_پڇ - 1] == ' ') {
            ڪنجي_قدر_پڇ--;
        }
        إذا (ايندڙ_Equalsign == ڪنجي_قدر_پڇ) {
            حالت = U_ILLEGAL_دليل_ERROR; /* empty key value in passed-in locale */
            أرجع 0;
        }

        rc = uprv_strcmp(canonKeywordName.معلومات(), localeKeywordName.معلومات());
        إذا(rc == 0) {
            /* Current entry matches the input keyword. Update the entry */
            إذا (!canon_Keyword_قدر.is_خالي_جايون()) { /* updating a value */
                updated_ڪنجيون_And_قدرون.append(ڪنجي_قدر_اڳياڙي, حالت);
                ڪنجي_قدر_اڳياڙي = ';'; /* for any subsequent key-value pair */
                updated_ڪنجيون_And_قدرون.append(canonKeywordName, حالت);
                updated_ڪنجيون_And_قدرون.append('=', حالت);
                updated_ڪنجيون_And_قدرون.append(canon_Keyword_قدر, حالت);
            } /* else removing this entry, don't emit anything */
            handled_داخلا_ڪنجي_And_قدر = سچ;
        } وإلا {
           /* input keyword sorts earlier than current entry, add before current entry */
            إذا (rc < 0 && !canon_Keyword_قدر.is_خالي_جايون() && !handled_داخلا_ڪنجي_And_قدر) {
                /* insert new entry at this location */
                updated_ڪنجيون_And_قدرون.append(ڪنجي_قدر_اڳياڙي, حالت);
                ڪنجي_قدر_اڳياڙي = ';'; /* for any subsequent key-value pair */
                updated_ڪنجيون_And_قدرون.append(canonKeywordName, حالت);
                updated_ڪنجيون_And_قدرون.append('=', حالت);
                updated_ڪنجيون_And_قدرون.append(canon_Keyword_قدر, حالت);
                handled_داخلا_ڪنجي_And_قدر = سچ;
            }
            /* copy the current entry */
            updated_ڪنجيون_And_قدرون.append(ڪنجي_قدر_اڳياڙي, حالت);
            ڪنجي_قدر_اڳياڙي = ';'; /* for any subsequent key-value pair */
            updated_ڪنجيون_And_قدرون.append(localeKeywordName, حالت);
            updated_ڪنجيون_And_قدرون.append('=', حالت);
            updated_ڪنجيون_And_قدرون.append(keywords.معلومات() + ايندڙ_Equalsign,
                                        static_cast<int32_t>(ڪنجي_قدر_پڇ - ايندڙ_Equalsign), حالت);
        }
        إذا (ايندڙ_Separator == std::string_view::npos && !canon_Keyword_قدر.is_خالي_جايون() && !handled_داخلا_ڪنجي_And_قدر) {
            /* append new entry at the end, it sorts later than existing entries */
            updated_ڪنجيون_And_قدرون.append(ڪنجي_قدر_اڳياڙي, حالت);
            /* skip keyValuePrefix update, no subsequent key-value pair */
            updated_ڪنجيون_And_قدرون.append(canonKeywordName, حالت);
            updated_ڪنجيون_And_قدرون.append('=', حالت);
            updated_ڪنجيون_And_قدرون.append(canon_Keyword_قدر, حالت);
            handled_داخلا_ڪنجي_And_قدر = سچ;
        }
        keyword_شروعات = ايندڙ_Separator;
    } /* end loop searching */

    /* Any error from updatedKeysAndValues.append above would be internal and not due to
     * problems with the passed-in locale. So if we did encounter problems with the
     * passed-in locale above, those errors took precedence and overrode any error
     * status from updatedKeysAndValues.append, and also caused a return of 0. If there
     * are errors here they are from updatedKeysAndValues.append; they do cause an
     * error return but the passed-in locale is unmodified and the original bufLen is
     * returned.
     */
    إذا (!handled_داخلا_ڪنجي_And_قدر || U_FAILURE(حالت)) {
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
                needLen, needLen, خالي_اشارو, needLen, &capacity);
        إذا (capacity < needLen || بفر == خالي_اشارو) {
            حالت = U_بفر_حد_کان_مٿي_ERROR;
        } وإلا {
            *بفر = '@';
            sink.Append(بفر, needLen);
        }
        أرجع needLen;
    }

    needLen = updated_ڪنجيون_And_قدرون.length();
    // Check to see can we fit the updatedKeysAndValues, if not, return
    // U_BUFFER_OVERFLOW_ERROR without copy updatedKeysAndValues into it.
    // We do this because this API function does not behave like most others:
    // It promises never to set a U_STRING_NOT_TERMINATED_WARNING.
    // When the contents fits but without the terminating NUL, in this case we need to not change
    // the buffer contents and return with a buffer overflow error.
    إذا (needLen > 0) {
        int32_t capacity = 0;
        حرف* بفر = sink.Get_Append_بفر(
                needLen, needLen, خالي_اشارو, needLen, &capacity);
        إذا (capacity < needLen || بفر == خالي_اشارو) {
            حالت = U_بفر_حد_کان_مٿي_ERROR;
            أرجع needLen;
        }
        uprv_memcpy(بفر, updated_ڪنجيون_And_قدرون.معلومات(), needLen);
        sink.Append(بفر, needLen);
    }
    U_ASSERT(حالت != U_STRING_NOT_TERMINATED_WARNING);
    أرجع needLen;
}

/* ### ID parsing implementation **************************************************/

نطاق {

مضمن منطقي is_اڳياڙي_Letter(حرف a) { أرجع a == 'x' || a == 'X' || a == 'i' || a == 'I'; }

/*returns true if one of the special prefixes is here (s=string)
  'x-' or 'i-' */
مضمن منطقي _isIDPrefix(std::string_view s) {
    أرجع s.size() >= 2 && is_اڳياڙي_Letter(s[0]) && _isIDSeparator(s[1]);
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
std::optional<int16_t> ڳوليو_اشاريو_2(ثابت حرف* ثابت* list, ثابت حرف* ڪنجي)
{
    ثابت حرف* ثابت* anchor = list;
    int32_t pass = 0;

    /* Make two passes through two nullptr-terminated arrays at 'list' */
    طالما (pass++ < 2) {
        طالما (*list) {
            إذا (uprv_strcmp(ڪنجي, *list) == 0) {
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
    std::optional<int16_t> وٿي = ڳوليو_اشاريو_2(DEPRECATED_COUNTRIES, oldID);
    أرجع وٿي.has_value() ? REPLACEMENT_COUNTRIES[*وٿي] : oldID;
}
U_CFUNC ثابت حرف*
uloc_getCurrentLanguageID(ثابت حرف* oldID U_LIFETIME_BOUND) {
    std::optional<int16_t> وٿي = ڳوليو_اشاريو_2(DEPRECATED_LANGUAGES, oldID);
    أرجع وٿي.has_value() ? REPLACEMENT_LANGUAGES[*وٿي] : oldID;
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

size_t _getLanguage(std::string_view localeID, ByteSink* sink, UError_ڪوڊ& حالت) {
    size_t skip = 0;
    إذا (localeID.size() == 4 && uprv_strnicmp(localeID.معلومات(), "root", 4) == 0) {
        skip = 4;
        localeID.remove_اڳياڙي(skip);
    } وإلا إذا (localeID.size() >= 3 && uprv_strnicmp(localeID.معلومات(), "und", 3) == 0 &&
               (localeID.size() == 3 ||
                localeID[3] == '-' ||
                localeID[3] == '_' ||
                localeID[3] == '@')) {
        skip = 3;
        localeID.remove_اڳياڙي(skip);
    }

    constexpr int32_t MAXLEN = ULOC_LANG_صلاحيت - 1;  // Minus NUL.

    /* if it starts with i- or x- then copy that prefix */
    size_t len = _isIDPrefix(localeID) ? 2 : 0;
    طالما (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len])) {
        إذا (len == MAXLEN) {
            حالت = U_ILLEGAL_دليل_ERROR;
            أرجع 0;
        }
        len++;
    }

    إذا (sink == خالي_اشارو || len == 0) { أرجع skip + len; }

    int32_t min_صلاحيت = uprv_max(static_cast<int32_t>(len), 4);  // Minimum 3 letters plus NUL.
    حرف scratch[MAXLEN];
    int32_t capacity = 0;
    حرف* بفر = sink->Get_Append_بفر(
            min_صلاحيت, min_صلاحيت, scratch, UPRV_LENGTHOF(scratch), &capacity);

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
        std::optional<int16_t> وٿي = ڳوليو_اشاريو_2(LANGUAGES_3, بفر);
        إذا (وٿي.has_value()) {
            ثابت حرف* ثابت alias = LANGUAGES[*وٿي];
            sink->Append(alias, static_cast<int32_t>(uprv_strlen(alias)));
            أرجع skip + len;
        }
    }

    sink->Append(بفر, static_cast<int32_t>(len));
    أرجع skip + len;
}

size_t _getScript(std::string_view localeID, ByteSink* sink) {
    constexpr int32_t ڊيگهائي_3 = 4;

    size_t len = 0;
    طالما (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len]) &&
            uprv_isASCIILetter(localeID[len])) {
        إذا (len == ڊيگهائي_3) { أرجع 0; }
        len++;
    }
    إذا (len != ڊيگهائي_3) { أرجع 0; }

    إذا (sink == خالي_اشارو) { أرجع len; }

    حرف scratch[ڊيگهائي_3];
    int32_t capacity = 0;
    حرف* بفر = sink->Get_Append_بفر(
            ڊيگهائي_3, ڊيگهائي_3, scratch, UPRV_LENGTHOF(scratch), &capacity);

    بفر[0] = uprv_toupper(localeID[0]);
    لكل (int32_t i = 1; i < ڊيگهائي_3; ++i) {
        بفر[i] = uprv_tolower(localeID[i]);
    }

    sink->Append(بفر, ڊيگهائي_3);
    أرجع len;
}

size_t _getRegion(std::string_view localeID, ByteSink* sink) {
    constexpr int32_t MINLEN = 2;
    constexpr int32_t MAXLEN = ULOC_COUNTRY_صلاحيت - 1;  // Minus NUL.

    size_t len = 0;
    طالما (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len])) {
        إذا (len == MAXLEN) { أرجع 0; }
        len++;
    }
    إذا (len < MINLEN) { أرجع 0; }

    إذا (sink == خالي_اشارو) { أرجع len; }

    حرف scratch[ULOC_COUNTRY_صلاحيت];
    int32_t capacity = 0;
    حرف* بفر = sink->Get_Append_بفر(
            ULOC_COUNTRY_صلاحيت,
            ULOC_COUNTRY_صلاحيت,
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
        std::optional<int16_t> وٿي = ڳوليو_اشاريو_2(COUNTRIES_3, بفر);
        إذا (وٿي.has_value()) {
            ثابت حرف* ثابت alias = COUNTRIES[*وٿي];
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
            UError_ڪوڊ& حالت) {
    إذا (U_FAILURE(حالت) || localeID.empty()) أرجع 0;

    // Reasonable upper limit for variants
    // There are no strict limitation of the syntax of variant in the legacy
    // locale format. If the locale is constructed from unicode_locale_id
    // as defined in UTS35, then we know each unicode_variant_subtag
    // could have max length of 8 ((alphanum{5,8} | digit alphanum{3})
    // 179 would allow 20 unicode_variant_subtag with sep in the
    // unicode_locale_id
    // 8*20 + 1*(20-1) = 179
    constexpr int32_t MAX_VARIANTS_ڊيگهائي = 179;

    /* get one or more variant tags and separate them with '_' */
    size_t اشاريو = 0;
    إذا (_isIDSeparator(prev)) {
        /* get a variant string after a '-' or '_' */
        لكل (std::string_view sub = localeID;;) {
            size_t next = sub.find_first_of(".@_-");
            // For historical reasons, a trailing separator is included in the variant.
            منطقي ختم = next == std::string_view::npos || next + 1 == sub.length();
            size_t حد_بندي = ختم ? sub.length() : next;
            اشاريو += حد_بندي;
            إذا (اشاريو > MAX_VARIANTS_ڊيگهائي) {
                حالت = U_ILLEGAL_دليل_ERROR;
                أرجع 0;
            }

            إذا (sink != خالي_اشارو) {
                إذا (needSeparator) {
                    sink->Append("_", 1);
                } وإلا {
                    needSeparator = سچ;
                }

                int32_t length = static_cast<int32_t>(حد_بندي);
                int32_t min_صلاحيت = uprv_min(length, MAX_VARIANTS_ڊيگهائي);
                حرف scratch[MAX_VARIANTS_ڊيگهائي];
                int32_t capacity = 0;
                حرف* بفر = sink->Get_Append_بفر(
                        min_صلاحيت, min_صلاحيت, scratch, UPRV_LENGTHOF(scratch), &capacity);

                لكل (size_t i = 0; i < حد_بندي; ++i) {
                    بفر[i] = uprv_toupper(sub[i]);
                }
                sink->Append(بفر, length);
            }

            إذا (ختم) { أرجع اشاريو; }
            sub.remove_اڳياڙي(next);
            إذا (_isTerminator(sub.front()) || _isBCP47Extension(sub)) { أرجع اشاريو; }
            sub.remove_اڳياڙي(1);
            اشاريو++;
        }
    }

    size_t skip = 0;
    /* if there is no variant tag after a '-' or '_' then look for '@' */
    إذا (prev == '@') {
        /* keep localeID */
    } وإلا إذا (ثابت حرف* p = locale_get_Keywords_شروعات(localeID); p != خالي_اشارو) {
        skip = 1 + p - localeID.معلومات(); /* point after the '@' */
        localeID.remove_اڳياڙي(skip);
    } وإلا {
        أرجع 0;
    }
    لكل (; اشاريو < localeID.size() && !_isTerminator(localeID[اشاريو]); اشاريو++) {
        إذا (اشاريو >= MAX_VARIANTS_ڊيگهائي) { // same as length > MAX_VARIANTS_LENGTH
            حالت = U_ILLEGAL_دليل_ERROR;
            أرجع 0;
        }
        إذا (needSeparator) {
            إذا (sink != خالي_اشارو) {
                sink->Append("_", 1);
            }
            needSeparator = خطأ;
        }
        إذا (sink != خالي_اشارو) {
            حرف c = uprv_toupper(localeID[اشاريو]);
            إذا (c == '-' || c == ',') c = '_';
            sink->Append(&c, 1);
        }
    }
    أرجع skip + اشاريو;
}

}  // namespace

U_EXPORT CharString
ulocimp_getLanguage(std::string_view localeID, UError_ڪوڊ& حالت) {
    أرجع ByteSinkUtil::ذريعي_Byte_Sink_منزل_Char_String(
        [&](ByteSink& sink, UError_ڪوڊ& حالت) {
            ulocimp_getSubtags(
                    localeID,
                    &sink,
                    خالي_اشارو,
                    خالي_اشارو,
                    خالي_اشارو,
                    خالي_اشارو,
                    حالت);
        },
        حالت);
}

U_EXPORT CharString
ulocimp_getScript(std::string_view localeID, UError_ڪوڊ& حالت) {
    أرجع ByteSinkUtil::ذريعي_Byte_Sink_منزل_Char_String(
        [&](ByteSink& sink, UError_ڪوڊ& حالت) {
            ulocimp_getSubtags(
                    localeID,
                    خالي_اشارو,
                    &sink,
                    خالي_اشارو,
                    خالي_اشارو,
                    خالي_اشارو,
                    حالت);
        },
        حالت);
}

U_EXPORT CharString
ulocimp_getRegion(std::string_view localeID, UError_ڪوڊ& حالت) {
    أرجع ByteSinkUtil::ذريعي_Byte_Sink_منزل_Char_String(
        [&](ByteSink& sink, UError_ڪوڊ& حالت) {
            ulocimp_getSubtags(
                    localeID,
                    خالي_اشارو,
                    خالي_اشارو,
                    &sink,
                    خالي_اشارو,
                    خالي_اشارو,
                    حالت);
        },
        حالت);
}

U_EXPORT CharString
ulocimp_getVariant(std::string_view localeID, UError_ڪوڊ& حالت) {
    أرجع ByteSinkUtil::ذريعي_Byte_Sink_منزل_Char_String(
        [&](ByteSink& sink, UError_ڪوڊ& حالت) {
            ulocimp_getSubtags(
                    localeID,
                    خالي_اشارو,
                    خالي_اشارو,
                    خالي_اشارو,
                    &sink,
                    خالي_اشارو,
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
        ثابت حرف** p_پڇاڙي,
        UError_ڪوڊ& حالت) {
    إذا (U_FAILURE(حالت)) { أرجع; }

    std::optional<CharStringByteSink> languageSink;
    std::optional<CharStringByteSink> scriptSink;
    std::optional<CharStringByteSink> regionSink;
    std::optional<CharStringByteSink> variantSink;

    إذا (language != خالي_اشارو) { languageSink.emplace(language); }
    إذا (script != خالي_اشارو) { scriptSink.emplace(script); }
    إذا (region != خالي_اشارو) { regionSink.emplace(region); }
    إذا (variant != خالي_اشارو) { variantSink.emplace(variant); }

    ulocimp_getSubtags(
            localeID,
            languageSink.has_value() ? &*languageSink : خالي_اشارو,
            scriptSink.has_value() ? &*scriptSink : خالي_اشارو,
            regionSink.has_value() ? &*regionSink : خالي_اشارو,
            variantSink.has_value() ? &*variantSink : خالي_اشارو,
            p_پڇاڙي,
            حالت);
}

U_EXPORT فراغ
ulocimp_getSubtags(
        std::string_view localeID,
        ByteSink* language,
        ByteSink* script,
        ByteSink* region,
        ByteSink* variant,
        ثابت حرف** p_پڇاڙي,
        UError_ڪوڊ& حالت) {
    إذا (U_FAILURE(حالت)) { أرجع; }

    إذا (p_پڇاڙي != خالي_اشارو) {
        *p_پڇاڙي = localeID.معلومات();
    } وإلا إذا (language == خالي_اشارو &&
               script == خالي_اشارو &&
               region == خالي_اشارو &&
               variant == خالي_اشارو) {
        أرجع;
    }

    إذا (localeID.empty()) { أرجع; }

    منطقي hasRegion = خطأ;

    {
        size_t len = _getLanguage(localeID, language, حالت);
        إذا (U_FAILURE(حالت)) { أرجع; }
        إذا (len > 0) {
            localeID.remove_اڳياڙي(len);
        }
    }

    إذا (p_پڇاڙي != خالي_اشارو) {
        *p_پڇاڙي = localeID.معلومات();
    } وإلا إذا (script == خالي_اشارو &&
               region == خالي_اشارو &&
               variant == خالي_اشارو) {
        أرجع;
    }

    إذا (localeID.empty()) { أرجع; }

    إذا (_isIDSeparator(localeID.front())) {
        std::string_view sub = localeID;
        sub.remove_اڳياڙي(1);
        size_t len = _getScript(sub, script);
        إذا (len > 0) {
            localeID.remove_اڳياڙي(len + 1);
            إذا (p_پڇاڙي != خالي_اشارو) { *p_پڇاڙي = localeID.معلومات(); }
        }
    }

    إذا ((region == خالي_اشارو && variant == خالي_اشارو && p_پڇاڙي == خالي_اشارو) || localeID.empty()) { أرجع; }

    إذا (_isIDSeparator(localeID.front())) {
        std::string_view sub = localeID;
        sub.remove_اڳياڙي(1);
        size_t len = _getRegion(sub, region);
        إذا (len > 0) {
            hasRegion = سچ;
            localeID.remove_اڳياڙي(len + 1);
            إذا (p_پڇاڙي != خالي_اشارو) { *p_پڇاڙي = localeID.معلومات(); }
        }
    }

    إذا ((variant == خالي_اشارو && p_پڇاڙي == خالي_اشارو) || localeID.empty()) { أرجع; }

    منطقي hasVariant = خطأ;

    إذا (_isIDSeparator(localeID.front()) && !_isBCP47Extension(localeID)) {
        std::string_view sub = localeID;
        /* If there was no country ID, skip a possible extra IDSeparator */
        size_t skip = !hasRegion && localeID.size() > 1 && _isIDSeparator(localeID[1]) ? 2 : 1;
        sub.remove_اڳياڙي(skip);
        size_t len = _getVariant(sub, localeID[0], variant, خطأ, حالت);
        إذا (U_FAILURE(حالت)) { أرجع; }
        إذا (len > 0) {
            hasVariant = سچ;
            localeID.remove_اڳياڙي(skip + len);
            إذا (p_پڇاڙي != خالي_اشارو) { *p_پڇاڙي = localeID.معلومات(); }
        }
    }

    إذا ((variant == خالي_اشارو && p_پڇاڙي == خالي_اشارو) || localeID.empty()) { أرجع; }

    إذا (_isBCP47Extension(localeID)) {
        localeID.remove_اڳياڙي(2);
        constexpr حرف vaposix[] = "-va-posix";
        constexpr size_t length = حجم vaposix - 1;
        لكل (size_t next;; localeID.remove_اڳياڙي(next)) {
            next = localeID.find('-', 1);
            إذا (next == std::string_view::npos) { اكسر; }
            next = localeID.find('-', next + 1);
            منطقي ختم = next == std::string_view::npos;
            std::string_view sub = localeID;
            إذا (!ختم) { sub.remove_suffix(sub.length() - next); }

            إذا (sub.length() == length && uprv_strnicmp(sub.معلومات(), vaposix, length) == 0) {
                إذا (variant != خالي_اشارو) {
                    إذا (hasVariant) { variant->Append("_", 1); }
                    constexpr حرف posix[] = "POSIX";
                    variant->Append(posix, حجم posix - 1);
                }
                إذا (p_پڇاڙي != خالي_اشارو) { *p_پڇاڙي = localeID.معلومات() + length; }
            }

            إذا (ختم) { اكسر; }
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
uloc_kw_ڳڻپ_Keywords(UEnumeration *en, UError_ڪوڊ * /*حالت*/) {
    حرف *kw = ((UKeywordsContext *)en->context)->keywords;
    int32_t نتيجو = 0;
    طالما(*kw) {
        نتيجو++;
        kw += uprv_strlen(kw)+1;
    }
    أرجع نتيجو;
}

ساكن ثابت حرف * U_CALLCONV
uloc_kw_ايندڙ_Keyword(UEnumeration* en,
                    int32_t* نتيجو_ڊيگهائي,
                    UError_ڪوڊ* /*حالت*/) {
    ثابت حرف* نتيجو = ((UKeywordsContext *)en->context)->current;
    int32_t len = 0;
    إذا(*نتيجو) {
        len = (int32_t)uprv_strlen(((UKeywordsContext *)en->context)->current);
        ((UKeywordsContext *)en->context)->current += len+1;
    } وإلا {
        نتيجو = خالي_اشارو;
    }
    إذا (نتيجو_ڊيگهائي) {
        *نتيجو_ڊيگهائي = len;
    }
    أرجع نتيجو;
}

ساكن فراغ U_CALLCONV
uloc_kw_resetKeywords(UEnumeration* en,
                      UError_ڪوڊ* /*حالت*/) {
    ((UKeywordsContext *)en->context)->current = ((UKeywordsContext *)en->context)->keywords;
}

U_CDECL_پڇاڙي


ساكن ثابت UEnumeration gKeywordsEnum = {
    خالي_اشارو,
    خالي_اشارو,
    uloc_kw_closeKeywords,
    uloc_kw_ڳڻپ_Keywords,
    uenum_unextDefault,
    uloc_kw_ايندڙ_Keyword,
    uloc_kw_resetKeywords
};

U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywordList(ثابت حرف *keywordList, int32_t keyword_List_سائيز, UError_ڪوڊ* حالت)
{
    إذا (U_FAILURE(*حالت)) { أرجع خالي_اشارو; }

    Local_يادگيري<UKeywordsContext> myContext;
    Local_يادگيري<UEnumeration> نتيجو;

    myContext.adoptInstead(static_cast<UKeywordsContext *>(uprv_malloc(حجم(UKeywordsContext))));
    نتيجو.adoptInstead(static_cast<UEnumeration *>(uprv_malloc(حجم(UEnumeration))));
    إذا (myContext.isNull() || نتيجو.isNull()) {
        *حالت = U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }
    uprv_memcpy(نتيجو.getAlias(), &gKeywordsEnum, حجم(UEnumeration));
    myContext->keywords = static_cast<حرف *>(uprv_malloc(keyword_List_سائيز+1));
    إذا (myContext->keywords == خالي_اشارو) {
        *حالت = U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }
    uprv_memcpy(myContext->keywords, keywordList, keyword_List_سائيز);
    myContext->keywords[keyword_List_سائيز] = 0;
    myContext->current = myContext->keywords;
    نتيجو->context = myContext.orphan();
    أرجع نتيجو.orphan();
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywords(ثابت حرف* localeID,
                        UError_ڪوڊ* حالت)
{
    إذا(حالت==خالي_اشارو || U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }

    CharString عارضي_بفر;
    ثابت حرف* tmpLocaleID;

    إذا (localeID != خالي_اشارو && _hasBCP47Extension(localeID)) {
        عارضي_بفر = ulocimp_for_Language_ٽيگ(localeID, -1, خالي_اشارو, *حالت);
        tmpLocaleID = U_SUCCESS(*حالت) && !عارضي_بفر.is_خالي_جايون() ? عارضي_بفر.معلومات() : localeID;
    } وإلا {
        إذا (localeID==خالي_اشارو) {
            localeID=uloc_getDefault();
        }
        tmpLocaleID=localeID;
    }

    ulocimp_getSubtags(
            tmpLocaleID,
            خالي_اشارو,
            خالي_اشارو,
            خالي_اشارو,
            خالي_اشارو,
            &tmpLocaleID,
            *حالت);
    إذا (U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }

    /* keywords are located after '@' */
    إذا((tmpLocaleID = locale_get_Keywords_شروعات(tmpLocaleID)) != خالي_اشارو) {
        CharString keywords = ulocimp_getKeywords(tmpLocaleID + 1, '@', خطأ, *حالت);
        إذا (U_FAILURE(*حالت)) {
            أرجع خالي_اشارو;
        }
        أرجع uloc_openKeywordList(keywords.معلومات(), keywords.length(), حالت);
    }
    أرجع خالي_اشارو;
}


/* bit-flags for 'options' parameter of _canonicalize */
#وصف_ڪريو _ULOC_STRIP_KEYWORDS 0x2
#وصف_ڪريو _ULOC_CANONICALIZE   0x1

نطاق {

مضمن منطقي OPTION_سيٽ(uint32_t options, uint32_t mask) { أرجع (options & mask) != 0; }

constexpr حرف i_default[] = {'i', '-', 'd', 'e', 'f', 'a', 'u', 'l', 't'};
constexpr int32_t I_DEFAULT_ڊيگهائي = UPRV_LENGTHOF(i_default);

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
              UError_ڪوڊ& err) {
    إذا (U_FAILURE(err)) {
        أرجع;
    }

    int32_t j, ميدان_ڳڻپ=0;
    CharString عارضي_بفر;  // if localeID has a BCP47 extension, tmpLocaleID points to this
    CharString localeIDWithHyphens;  // if localeID has a BPC47 extension and have _, tmpLocaleID points to this
    std::string_view origLocaleID;
    std::string_view tmpLocaleID;
    size_t keyword_سونپيو = std::string_view::npos;
    size_t separatorIndicator = std::string_view::npos;

    إذا (_hasBCP47Extension(localeID)) {
        std::string_view localeIDPtr = localeID;

        // convert all underbars to hyphens, unless the "BCP47 extension" comes at the beginning of the string
        إذا (localeID.size() >= 2 && localeID.find('_') != std::string_view::npos && localeID[1] != '-' && localeID[1] != '_') {
            localeIDWithHyphens.append(localeID, err);
            إذا (U_SUCCESS(err)) {
                لكل (حرف* p = localeIDWithHyphens.معلومات(); *p != '\0'; ++p) {
                    إذا (*p == '_') {
                        *p = '-';
                    }
                }
                localeIDPtr = localeIDWithHyphens.منزل_String_Piece();
            }
        }

        عارضي_بفر = ulocimp_for_Language_ٽيگ(localeIDPtr.معلومات(), static_cast<int32_t>(localeIDPtr.size()), خالي_اشارو, err);
        tmpLocaleID = U_SUCCESS(err) && !عارضي_بفر.is_خالي_جايون() ? static_cast<std::string_view>(عارضي_بفر.منزل_String_Piece()) : localeIDPtr;
    } وإلا {
        tmpLocaleID=localeID;
    }

    origLocaleID=tmpLocaleID;

    /* get all pieces, one after another, and separate with '_' */
    CharString ٽيگ;
    CharString script;
    CharString country;
    CharString variant;
    ثابت حرف* پڇاڙي = خالي_اشارو;
    ulocimp_getSubtags(
            tmpLocaleID,
            &ٽيگ,
            &script,
            &country,
            &variant,
            &پڇاڙي,
            err);
    إذا (U_FAILURE(err)) {
        أرجع;
    }
    U_ASSERT(پڇاڙي != خالي_اشارو);
    إذا (پڇاڙي > tmpLocaleID.معلومات()) {
        tmpLocaleID.remove_اڳياڙي(پڇاڙي - tmpLocaleID.معلومات());
    }

    إذا (ٽيگ.length() == I_DEFAULT_ڊيگهائي && origLocaleID.length() >= I_DEFAULT_ڊيگهائي &&
            uprv_strncmp(origLocaleID.معلومات(), i_default, I_DEFAULT_ڊيگهائي) == 0) {
        ٽيگ.clear();
        ٽيگ.append(uloc_getDefault(), err);
    } وإلا {
        إذا (!script.is_خالي_جايون()) {
            ++ميدان_ڳڻپ;
            ٽيگ.append('_', err);
            ٽيگ.append(script, err);
        }
        إذا (!country.is_خالي_جايون()) {
            ++ميدان_ڳڻپ;
            ٽيگ.append('_', err);
            ٽيگ.append(country, err);
        }
        إذا (!variant.is_خالي_جايون()) {
            ++ميدان_ڳڻپ;
            إذا (country.is_خالي_جايون()) {
                ٽيگ.append('_', err);
            }
            ٽيگ.append('_', err);
            ٽيگ.append(variant, err);
        }
    }

    /* Copy POSIX-style charset specifier, if any [mr.utf8] */
    إذا (!OPTION_سيٽ(options, _ULOC_CANONICALIZE) && !tmpLocaleID.empty() && tmpLocaleID.front() == '.') {
        ٽيگ.append('.', err);
        tmpLocaleID.remove_اڳياڙي(1);
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
        constexpr size_t k_Max_Charset_ڊيگهائي = 64;
        إذا (length > k_Max_Charset_ڊيگهائي) {
           err = U_ILLEGAL_دليل_ERROR; /* malformed keyword name */
           أرجع;
        }
        إذا (length > 0) {
            ٽيگ.append(tmpLocaleID.معلومات(), static_cast<int32_t>(length), err);
            tmpLocaleID.remove_اڳياڙي(length);
        }
    }

    /* Scan ahead to next '@' and determine if it is followed by '=' and/or ';'
       After this, tmpLocaleID either starts at '@' or is empty. */
    إذا (ثابت حرف* شروعات = locale_get_Keywords_شروعات(tmpLocaleID); شروعات != خالي_اشارو) {
        إذا (شروعات > tmpLocaleID.معلومات()) {
            tmpLocaleID.remove_اڳياڙي(شروعات - tmpLocaleID.معلومات());
        }
        keyword_سونپيو = tmpLocaleID.find('=');
        separatorIndicator = tmpLocaleID.find(';');
    } وإلا {
        tmpLocaleID = {};
    }

    /* Copy POSIX-style variant, if any [mr@FOO] */
    إذا (!OPTION_سيٽ(options, _ULOC_CANONICALIZE) &&
        !tmpLocaleID.empty() && keyword_سونپيو == std::string_view::npos) {
        ٽيگ.append(tmpLocaleID, err);
        tmpLocaleID = {};
    }

    إذا (OPTION_سيٽ(options, _ULOC_CANONICALIZE)) {
        /* Handle @FOO variant if @ is present and not followed by = */
        إذا (!tmpLocaleID.empty() && keyword_سونپيو == std::string_view::npos) {
            /* Add missing '_' if needed */
            إذا (ميدان_ڳڻپ < 2 || (ميدان_ڳڻپ < 3 && !script.is_خالي_جايون())) {
                افعل {
                    ٽيگ.append('_', err);
                    ++ميدان_ڳڻپ;
                } طالما(ميدان_ڳڻپ<2);
            }

            CharStringByteSink s(&ٽيگ);
            std::string_view sub = tmpLocaleID;
            sub.remove_اڳياڙي(1);
            _getVariant(sub, '@', &s, !variant.is_خالي_جايون(), err);
            إذا (U_FAILURE(err)) { أرجع; }
        }

        /* Look up the ID in the canonicalization map */
        لكل (j=0; j<UPRV_LENGTHOF(CANONICALIZE_MAP); j++) {
            StringPiece id(CANONICALIZE_MAP[j].id);
            إذا (ٽيگ == id) {
                إذا (id.empty() && !tmpLocaleID.empty()) {
                    اكسر; /* Don't remap "" if keywords present */
                }
                ٽيگ.clear();
                ٽيگ.append(CANONICALIZE_MAP[j].canonicalID, err);
                اكسر;
            }
        }
    }

    sink.Append(ٽيگ.معلومات(), ٽيگ.length());

    إذا (!OPTION_سيٽ(options, _ULOC_STRIP_KEYWORDS)) {
        إذا (!tmpLocaleID.empty() && keyword_سونپيو != std::string_view::npos &&
            (separatorIndicator == std::string_view::npos || separatorIndicator > keyword_سونپيو)) {
            sink.Append("@", 1);
            ++ميدان_ڳڻپ;
            tmpLocaleID.remove_اڳياڙي(1);
            ulocimp_getKeywords(tmpLocaleID, '@', sink, سچ, err);
        }
    }
}

}  // namespace

/* ### ID parsing API **************************************************/

U_CAPI int32_t  U_EXPORT2
uloc_get_والد(ثابت حرف*    localeID,
               حرف* والد,
               int32_t والد_صلاحيت,
               UError_ڪوڊ* err)
{
    أرجع ByteSinkUtil::ذريعي_Byte_Sink_منزل_Terminated_Chars(
        والد, والد_صلاحيت,
        [&](ByteSink& sink, UError_ڪوڊ& حالت) {
            ulocimp_get_والد(localeID, sink, حالت);
        },
        *err);
}

U_EXPORT CharString
ulocimp_get_والد(ثابت حرف* localeID,
                  UError_ڪوڊ& err)
{
    أرجع ByteSinkUtil::ذريعي_Byte_Sink_منزل_Char_String(
        [&](ByteSink& sink, UError_ڪوڊ& حالت) {
            ulocimp_get_والد(localeID, sink, حالت);
        },
        err);
}

U_EXPORT فراغ
ulocimp_get_والد(ثابت حرف* localeID,
                  icu::ByteSink& sink,
                  UError_ڪوڊ& err)
{
    إذا (U_FAILURE(err)) { أرجع; }

    ثابت حرف *آخري_Underscore;
    int32_t i;

    إذا (localeID == خالي_اشارو)
        localeID = uloc_getDefault();

    آخري_Underscore=uprv_strrchr(localeID, '_');
    إذا(آخري_Underscore!=خالي_اشارو) {
        i = static_cast<int32_t>(آخري_Underscore - localeID);
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
         int32_t language_صلاحيت,
         UError_ڪوڊ* err)
{
    إذا (localeID == خالي_اشارو) {
        localeID = uloc_getDefault();
    }

    /* uloc_getLanguage will return a 2 character iso-639 code if one exists. *CWB*/
    أرجع ByteSinkUtil::ذريعي_Byte_Sink_منزل_Terminated_Chars(
        language, language_صلاحيت,
        [&](ByteSink& sink, UError_ڪوڊ& حالت) {
            ulocimp_getSubtags(
                    localeID,
                    &sink,
                    خالي_اشارو,
                    خالي_اشارو,
                    خالي_اشارو,
                    خالي_اشارو,
                    حالت);
        },
        *err);
}

U_CAPI int32_t U_EXPORT2
uloc_getScript(ثابت حرف*    localeID,
         حرف* script,
         int32_t script_صلاحيت,
         UError_ڪوڊ* err)
{
    إذا (localeID == خالي_اشارو) {
        localeID = uloc_getDefault();
    }

    أرجع ByteSinkUtil::ذريعي_Byte_Sink_منزل_Terminated_Chars(
        script, script_صلاحيت,
        [&](ByteSink& sink, UError_ڪوڊ& حالت) {
            ulocimp_getSubtags(
                    localeID,
                    خالي_اشارو,
                    &sink,
                    خالي_اشارو,
                    خالي_اشارو,
                    خالي_اشارو,
                    حالت);
        },
        *err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getCountry(ثابت حرف* localeID,
            حرف* country,
            int32_t country_صلاحيت,
            UError_ڪوڊ* err)
{
    إذا (localeID == خالي_اشارو) {
        localeID = uloc_getDefault();
    }

    أرجع ByteSinkUtil::ذريعي_Byte_Sink_منزل_Terminated_Chars(
        country, country_صلاحيت,
        [&](ByteSink& sink, UError_ڪوڊ& حالت) {
            ulocimp_getSubtags(
                    localeID,
                    خالي_اشارو,
                    خالي_اشارو,
                    &sink,
                    خالي_اشارو,
                    خالي_اشارو,
                    حالت);
        },
        *err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getVariant(ثابت حرف* localeID,
                حرف* variant,
                int32_t variant_صلاحيت,
                UError_ڪوڊ* err)
{
    إذا (localeID == خالي_اشارو) {
        localeID = uloc_getDefault();
    }

    أرجع ByteSinkUtil::ذريعي_Byte_Sink_منزل_Terminated_Chars(
        variant, variant_صلاحيت,
        [&](ByteSink& sink, UError_ڪوڊ& حالت) {
            ulocimp_getSubtags(
                    localeID,
                    خالي_اشارو,
                    خالي_اشارو,
                    خالي_اشارو,
                    &sink,
                    خالي_اشارو,
                    حالت);
        },
        *err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getName(ثابت حرف* localeID,
             حرف* name,
             int32_t name_صلاحيت,
             UError_ڪوڊ* err)
{
    إذا (localeID == خالي_اشارو) {
        localeID = uloc_getDefault();
    }
    أرجع ByteSinkUtil::ذريعي_Byte_Sink_منزل_Terminated_Chars(
        name, name_صلاحيت,
        [&](ByteSink& sink, UError_ڪوڊ& حالت) {
            ulocimp_getName(localeID, sink, حالت);
        },
        *err);
}

U_EXPORT CharString
ulocimp_getName(std::string_view localeID,
                UError_ڪوڊ& err)
{
    أرجع ByteSinkUtil::ذريعي_Byte_Sink_منزل_Char_String(
        [&](ByteSink& sink, UError_ڪوڊ& حالت) {
            ulocimp_getName(localeID, sink, حالت);
        },
        err);
}

U_EXPORT فراغ
ulocimp_getName(std::string_view localeID,
                ByteSink& sink,
                UError_ڪوڊ& err)
{
    _canonicalize(localeID, sink, 0, err);
}

U_CAPI int32_t  U_EXPORT2
uloc_get_بنياد_Name(ثابت حرف* localeID,
                 حرف* name,
                 int32_t name_صلاحيت,
                 UError_ڪوڊ* err)
{
    إذا (localeID == خالي_اشارو) {
        localeID = uloc_getDefault();
    }
    أرجع ByteSinkUtil::ذريعي_Byte_Sink_منزل_Terminated_Chars(
        name, name_صلاحيت,
        [&](ByteSink& sink, UError_ڪوڊ& حالت) {
            ulocimp_get_بنياد_Name(localeID, sink, حالت);
        },
        *err);
}

U_EXPORT CharString
ulocimp_get_بنياد_Name(std::string_view localeID,
                    UError_ڪوڊ& err)
{
    أرجع ByteSinkUtil::ذريعي_Byte_Sink_منزل_Char_String(
        [&](ByteSink& sink, UError_ڪوڊ& حالت) {
            ulocimp_get_بنياد_Name(localeID, sink, حالت);
        },
        err);
}

U_EXPORT فراغ
ulocimp_get_بنياد_Name(std::string_view localeID,
                    ByteSink& sink,
                    UError_ڪوڊ& err)
{
    _canonicalize(localeID, sink, _ULOC_STRIP_KEYWORDS, err);
}

U_CAPI int32_t  U_EXPORT2
uloc_canonicalize(ثابت حرف* localeID,
                  حرف* name,
                  int32_t name_صلاحيت,
                  UError_ڪوڊ* err)
{
    إذا (localeID == خالي_اشارو) {
        localeID = uloc_getDefault();
    }
    أرجع ByteSinkUtil::ذريعي_Byte_Sink_منزل_Terminated_Chars(
        name, name_صلاحيت,
        [&](ByteSink& sink, UError_ڪوڊ& حالت) {
            ulocimp_canonicalize(localeID, sink, حالت);
        },
        *err);
}

U_EXPORT CharString
ulocimp_canonicalize(std::string_view localeID,
                     UError_ڪوڊ& err)
{
    أرجع ByteSinkUtil::ذريعي_Byte_Sink_منزل_Char_String(
        [&](ByteSink& sink, UError_ڪوڊ& حالت) {
            ulocimp_canonicalize(localeID, sink, حالت);
        },
        err);
}

U_EXPORT فراغ
ulocimp_canonicalize(std::string_view localeID,
                     ByteSink& sink,
                     UError_ڪوڊ& err)
{
    _canonicalize(localeID, sink, _ULOC_CANONICALIZE, err);
}

U_CAPI ثابت حرف*  U_EXPORT2
uloc_getISO3Language(ثابت حرف* localeID)
{
    UError_ڪوڊ err = U_ZERO_ERROR;

    إذا (localeID == خالي_اشارو)
    {
        localeID = uloc_getDefault();
    }
    CharString lang = ulocimp_getLanguage(localeID, err);
    إذا (U_FAILURE(err))
        أرجع "";
    std::optional<int16_t> وٿي = ڳوليو_اشاريو_2(LANGUAGES, lang.معلومات());
    أرجع وٿي.has_value() ? LANGUAGES_3[*وٿي] : "";
}

U_CAPI ثابت حرف*  U_EXPORT2
uloc_getISO3Country(ثابت حرف* localeID)
{
    UError_ڪوڊ err = U_ZERO_ERROR;

    إذا (localeID == خالي_اشارو)
    {
        localeID = uloc_getDefault();
    }
    CharString cntry = ulocimp_getRegion(localeID, err);
    إذا (U_FAILURE(err))
        أرجع "";
    std::optional<int16_t> وٿي = ڳوليو_اشاريو_2(COUNTRIES, cntry.معلومات());
    أرجع وٿي.has_value() ? COUNTRIES_3[*وٿي] : "";
}

U_CAPI uint32_t  U_EXPORT2
uloc_getLCID(ثابت حرف* localeID)
{
    UError_ڪوڊ حالت = U_ZERO_ERROR;
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
        إذا (U_SUCCESS(حالت) && !collVal.is_خالي_جايون()) {
            CharString tmpLocaleID = ulocimp_get_بنياد_Name(localeID, حالت);
            ulocimp_سيٽ_Keyword_قدر("collation", collVal.منزل_String_Piece(), tmpLocaleID, حالت);
            إذا (U_SUCCESS(حالت)) {
                أرجع uprv_convert_منزل_LCID(langID.معلومات(), tmpLocaleID.معلومات(), &حالت);
            }
        }

        // fall through - all keywords are simply ignored
        حالت = U_ZERO_ERROR;
    }

    أرجع uprv_convert_منزل_LCID(langID.معلومات(), localeID, &حالت);
}

U_CAPI int32_t U_EXPORT2
uloc_getLocaleForLCID(uint32_t hostid, حرف *locale, int32_t locale_صلاحيت,
                UError_ڪوڊ *حالت)
{
    أرجع uprv_convert_منزل_Posix(hostid, locale, locale_صلاحيت, حالت);
}

/* ### Default locale **************************************************/

U_CAPI ثابت حرف*  U_EXPORT2
uloc_getDefault()
{
    أرجع locale_get_default();
}

U_CAPI فراغ  U_EXPORT2
uloc_سيٽ_Default(ثابت حرف*   newDefaultLocale,
             UError_ڪوڊ* err)
{
    إذا (U_FAILURE(*err))
        أرجع;
    /* the error code isn't currently used for anything by this function*/

    /* propagate change to C++ */
    locale_سيٽ_default(newDefaultLocale);
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
uloc_منزل_Unicode_Locale_ڪنجي(ثابت حرف* keyword U_LIFETIME_BOUND)
{
    إذا (keyword == خالي_اشارو || *keyword == '\0') { أرجع خالي_اشارو; }
    std::optional<std::string_view> نتيجو = ulocimp_منزل_Bcp_ڪنجي_With_Fallback(keyword);
    أرجع نتيجو.has_value() ? نتيجو->معلومات() : خالي_اشارو;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_منزل_Bcp_ڪنجي_With_Fallback(std::string_view keyword U_LIFETIME_BOUND)
{
    std::optional<std::string_view> bcp_ڪنجي = ulocimp_منزل_Bcp_ڪنجي(keyword);
    إذا (!bcp_ڪنجي.has_value() &&
        ultag_is_Unicode_Locale_ڪنجي(keyword.معلومات(), static_cast<int32_t>(keyword.size()))) {
        // unknown keyword, but syntax is fine..
        أرجع keyword;
    }
    أرجع bcp_ڪنجي;
}

U_CAPI ثابت حرف* U_EXPORT2
uloc_منزل_Unicode_Locale_Type(ثابت حرف* keyword, ثابت حرف* value U_LIFETIME_BOUND)
{
    إذا (keyword == خالي_اشارو || *keyword == '\0' ||
        value == خالي_اشارو || *value == '\0') { أرجع خالي_اشارو; }
    std::optional<std::string_view> نتيجو = ulocimp_منزل_Bcp_Type_With_Fallback(keyword, value);
    أرجع نتيجو.has_value() ? نتيجو->معلومات() : خالي_اشارو;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_منزل_Bcp_Type_With_Fallback(std::string_view keyword, std::string_view value U_LIFETIME_BOUND)
{
    std::optional<std::string_view> bcpType = ulocimp_منزل_Bcp_Type(keyword, value);
    إذا (!bcpType.has_value() &&
        ultag_isUnicodeLocaleType(value.معلومات(), static_cast<int32_t>(value.size()))) {
        // unknown keyword, but syntax is fine..
        أرجع value;
    }
    أرجع bcpType;
}

نطاق {

منطقي
is_Well_Formed_Legacy_ڪنجي(std::string_view ڪنجي)
{
    أرجع std::all_of(ڪنجي.begin(), ڪنجي.پڇاڙي(), UPRV_ISALPHANUM);
}

منطقي
isWellFormedLegacyType(std::string_view legacyType)
{
    int32_t alpha_انگ_Len = 0;
    لكل (حرف c : legacyType) {
        إذا (c == '_' || c == '/' || c == '-') {
            إذا (alpha_انگ_Len == 0) {
                أرجع خطأ;
            }
            alpha_انگ_Len = 0;
        } وإلا إذا (UPRV_ISALPHANUM(c)) {
            alpha_انگ_Len++;
        } وإلا {
            أرجع خطأ;
        }
    }
    أرجع alpha_انگ_Len != 0;
}

}  // namespace

U_CAPI ثابت حرف* U_EXPORT2
uloc_منزل_Legacy_ڪنجي(ثابت حرف* keyword U_LIFETIME_BOUND)
{
    إذا (keyword == خالي_اشارو || *keyword == '\0') { أرجع خالي_اشارو; }
    std::optional<std::string_view> نتيجو = ulocimp_منزل_Legacy_ڪنجي_With_Fallback(keyword);
    أرجع نتيجو.has_value() ? نتيجو->معلومات() : خالي_اشارو;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_منزل_Legacy_ڪنجي_With_Fallback(std::string_view keyword U_LIFETIME_BOUND)
{
    std::optional<std::string_view> legacy_ڪنجي_2 = ulocimp_منزل_Legacy_ڪنجي(keyword);
    إذا (!legacy_ڪنجي_2.has_value() && is_Well_Formed_Legacy_ڪنجي(keyword)) {
        // Checks if the specified locale key is well-formed with the legacy locale syntax.
        //
        // Note:
        //  LDML/CLDR provides some definition of keyword syntax in
        //  * http://www.unicode.org/reports/tr35/#Unicode_locale_identifier and
        //  * http://www.unicode.org/reports/tr35/#Old_Locale_Extension_Syntax
        //  Keys can only consist of [0-9a-zA-Z].
        أرجع keyword;
    }
    أرجع legacy_ڪنجي_2;
}

U_CAPI ثابت حرف* U_EXPORT2
uloc_منزل_Legacy_Type(ثابت حرف* keyword, ثابت حرف* value U_LIFETIME_BOUND)
{
    إذا (keyword == خالي_اشارو || *keyword == '\0' ||
        value == خالي_اشارو || *value == '\0') { أرجع خالي_اشارو; }
    std::optional<std::string_view> نتيجو = ulocimp_منزل_Legacy_Type_With_Fallback(keyword, value);
    أرجع نتيجو.has_value() ? نتيجو->معلومات() : خالي_اشارو;  // Known to be NUL terminated.
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
