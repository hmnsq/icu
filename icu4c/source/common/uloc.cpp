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

#شامل <algorithm>
#شامل <optional>
#شامل <string_view>

#شامل "unicode/bytestream.h"
#شامل "unicode/errorcode.h"
#شامل "unicode/stringpiece.h"
#شامل "unicode/utypes.h"
#شامل "unicode/ustring.h"
#شامل "unicode/uloc.h"

#شامل "bytesinkutil.h"
#شامل "putilimp.h"
#شامل "ustr_imp.h"
#شامل "ulocimp.h"
#شامل "umutex.h"
#شامل "cstring.h"
#شامل "cmemory.h"
#شامل "locmap.h"
#شامل "uarrsort.h"
#شامل "uenumimp.h"
#شامل "uassert.h"
#شامل "charstr.h"

U_NAMESPACE_USE

/* ### Declarations **************************************************/

/* Locale stuff from locid.cpp */
U_CFUNC خالی locale_سیٹ_default(ثابت حرف *id);
U_CFUNC ثابت حرف *locale_get_default();

نامخلا {

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
خالی_اشاریہ,
    "in",  "iw",  "ji",  "jw",  "mo",  "sh",  "swc", "tl",  /* obsolete language codes */
خالی_اشاریہ
};

constexpr ثابت حرف* DEPRECATED_LANGUAGES[]={
    "in", "iw", "ji", "jw", "mo", خالی_اشاریہ, خالی_اشاریہ
};
constexpr ثابت حرف* REPLACEMENT_LANGUAGES[]={
    "id", "he", "yi", "jv", "ro", خالی_اشاریہ, خالی_اشاریہ
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
خالی_اشاریہ,
/*  "in",  "iw",  "ji",  "jw",  "mo",  "sh",  "swc", "tl",  */
    "ind", "heb", "yid", "jaw", "mol", "srp", "swc", "tgl",
خالی_اشاریہ
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
خالی_اشاریہ,
    "AN",  "BU", "CS", "FX", "RO", "SU", "TP", "YD", "YU", "ZR",   /* obsolete country codes */
خالی_اشاریہ
};

constexpr ثابت حرف* DEPRECATED_COUNTRIES[] = {
    "AN", "BU", "CS", "DD", "DY", "FX", "HV", "NH", "RH", "SU", "TP", "UK", "VD", "YD", "YU", "ZR", خالی_اشاریہ, خالی_اشاریہ /* deprecated country list */
};
constexpr ثابت حرف* REPLACEMENT_COUNTRIES[] = {
/*  "AN", "BU", "CS", "DD", "DY", "FX", "HV", "NH", "RH", "SU", "TP", "UK", "VD", "YD", "YU", "ZR" */
    "CW", "MM", "RS", "DE", "BJ", "FR", "BF", "VU", "ZW", "RU", "TL", "GB", "VN", "YE", "RS", "CD", خالی_اشاریہ, خالی_اشاریہ  /* replacement country codes */
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
خالی_اشاریہ,
/*  "AN",  "BU",  "CS",  "FX",  "RO", "SU",  "TP",  "YD",  "YU",  "ZR" */
    "ANT", "BUR", "SCG", "FXX", "ROM", "SUN", "TMP", "YMD", "YUG", "ZAR",
خالی_اشاریہ
};

قسمتعریف ساخت CanonicalizationMap {
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
int32_t get_مختصر_ترین_Subtag_لمبائی(std::string_view localeID) {
    int32_t localeIDLength = static_cast<int32_t>(localeID.length());
    int32_t length = localeIDLength;
    int32_t tmp_لمبائی = 0;
    int32_t i;
    منطقی reset = سچ;

    برائے (i = 0; i < localeIDLength; i++) {
        اگر (localeID[i] != '_' && localeID[i] != '-') {
            اگر (reset) {
                tmp_لمبائی = 0;
                reset = جھوٹ;
            }
            tmp_لمبائی++;
        } ورنہ {
            اگر (tmp_لمبائی != 0 && tmp_لمبائی < length) {
                length = tmp_لمبائی;
            }
            reset = سچ;
        }
    }

    واپس length;
}
/* Test if the locale id has BCP47 u extension and does not have '@' */
اندرونی منطقی _hasBCP47Extension(std::string_view id) {
    واپس id.find('@') == std::string_view::npos && get_مختصر_ترین_Subtag_لمبائی(id) == 1;
}

/* ### Keywords **************************************************/
اندرونی منطقی UPRV_ISDIGIT(حرف c) { واپس c >= '0' && c <= '9'; }
اندرونی منطقی UPRV_ISALPHANUM(حرف c) { واپس uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }
/* Punctuation/symbols allowed in legacy key values */
اندرونی منطقی UPRV_OK_قدر_PUNCTUATION(حرف c) { واپس c == '_' || c == '-' || c == '+' || c == '/'; }

}  // namespace

#تعریف_کرو ULOC_KEYWORD_بفر_LEN 25
#تعریف_کرو ULOC_MAX_NO_KEYWORDS 25

U_CAPI ثابت حرف* U_EXPORT2
locale_get_Keywords_آغاز(std::string_view localeID U_LIFETIME_BOUND) {
    اگر (size_t pos = localeID.find('@'); pos != std::string_view::npos) {
        واپس localeID.ڈیٹا() + pos;
    }
#اگر (U_CHARSET_FAMILY == U_EBCDIC_FAMILY)
    ورنہ {
        /* We do this because the @ sign is variant, and the @ sign used on one
        EBCDIC machine won't be compiled the same way on other EBCDIC based
        machines. */
        ساکن ثابت uint8_t ebcdicSigns[] = { 0x7C, 0x44, 0x66, 0x80, 0xAC, 0xAE, 0xAF, 0xB5, 0xEC, 0xEF, 0x00 };
        ثابت uint8_t *char_مقصد_ڈھونڈو = ebcdicSigns;
        جب(*char_مقصد_ڈھونڈو) {
            اگر (size_t pos = localeID.find(*char_مقصد_ڈھونڈو); pos != std::string_view::npos) {
                واپس localeID.ڈیٹا() + pos;
            }
            char_مقصد_ڈھونڈو++;
        }
    }
#اگر_ختم
    واپس خالی_اشاریہ;
}

نامخلا {

/**
 * @param keywordName incoming name to be canonicalized
 * @param status return status (keyword too long)
 * @return the keyword name
 */
CharString locale_canonKeywordName(std::string_view keywordName, UError_رمز& حالت_2)
{
  اگر (U_FAILURE(حالت_2)) { واپس {}; }
  CharString نتیجہ;

  برائے (حرف c : keywordName) {
    اگر (!UPRV_ISALPHANUM(c)) {
      حالت_2 = U_ILLEGAL_دلیل_ERROR; /* malformed keyword name */
      واپس {};
    }
    نتیجہ.append(uprv_tolower(c), حالت_2);
  }
  اگر (نتیجہ.is_خالی_خانے()) {
    حالت_2 = U_ILLEGAL_دلیل_ERROR; /* empty keyword name */
    واپس {};
  }

  واپس نتیجہ;
}

قسمتعریف ساخت {
    حرف keyword[ULOC_KEYWORD_بفر_LEN];
    int32_t keywordLen;
    ثابت حرف *قدر_آغاز;
    int32_t قدر_Len;
} KeywordStruct;

int32_t U_CALLCONV
compareKeywordStructs(ثابت خالی * /*context*/, ثابت خالی *بائیں, ثابت خالی *دائیں) {
    ثابت حرف* بایاں_String = static_cast<ثابت KeywordStruct*>(بائیں)->keyword;
    ثابت حرف* دایاں_String = static_cast<ثابت KeywordStruct*>(دائیں)->keyword;
    واپس uprv_strcmp(بایاں_String, دایاں_String);
}

}  // namespace

U_EXPORT CharString
ulocimp_getKeywords(std::string_view localeID,
                    حرف پچھلا,
                    منطقی اقدار_Too,
                    UError_رمز& حالت_2)
{
    واپس ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Char_String(
        [&](ByteSink& sink, UError_رمز& حالت_2) {
            ulocimp_getKeywords(localeID,
                                پچھلا,
                                sink,
                                اقدار_Too,
                                حالت_2);
        },
        حالت_2);
}

U_EXPORT خالی
ulocimp_getKeywords(std::string_view localeID,
                    حرف پچھلا,
                    ByteSink& sink,
                    منطقی اقدار_Too,
                    UError_رمز& حالت_2)
{
    اگر (U_FAILURE(حالت_2)) { واپس; }

    KeywordStruct keywordList[ULOC_MAX_NO_KEYWORDS];

    int32_t maxKeywords = ULOC_MAX_NO_KEYWORDS;
    int32_t عدد_Keywords = 0;
    size_t equalSign = std::string_view::npos;
    size_t semicolon = std::string_view::npos;
    int32_t i = 0, j, n;

    اگر(پچھلا == '@') { /* start of keyword definition */
        /* we will grab pairs, trim spaces, lowercase keywords, sort and return */
        کرو {
            منطقی duplicate = جھوٹ;
            /* skip leading spaces */
            جب (!localeID.empty() && localeID.front() == ' ') {
                localeID.remove_سابقہ(1);
            }
            اگر (localeID.empty()) { /* handle trailing "; " */
                توڑو;
            }
            اگر(عدد_Keywords == maxKeywords) {
                حالت_2 = U_اندرونی_پروگرام_ERROR;
                واپس;
            }
            equalSign = localeID.find('=');
            semicolon = localeID.find(';');
            /* lack of '=' [foo@currency] is illegal */
            /* ';' before '=' [foo@currency;collation=pinyin] is illegal */
            اگر (equalSign == std::string_view::npos ||
                (semicolon != std::string_view::npos && semicolon < equalSign)) {
                حالت_2 = U_INVALID_FORMAT_ERROR;
                واپس;
            }
            /* zero-length keyword is an error. */
            اگر (equalSign == 0) {
                حالت_2 = U_INVALID_FORMAT_ERROR;
                واپس;
            }
            /* need to normalize both keyword and keyword name */
            اگر (equalSign >= ULOC_KEYWORD_بفر_LEN) {
                /* keyword name too long for internal buffer */
                حالت_2 = U_اندرونی_پروگرام_ERROR;
                واپس;
            }
            برائے (i = 0, n = 0; static_cast<size_t>(i) < equalSign; ++i) {
                اگر (localeID[i] != ' ') {
                    keywordList[عدد_Keywords].keyword[n++] = uprv_tolower(localeID[i]);
                }
            }

            keywordList[عدد_Keywords].keyword[n] = 0;
            keywordList[عدد_Keywords].keywordLen = n;
            /* now grab the value part. First we skip the '=' */
            equalSign++;
            /* then we leading spaces */
            جب (equalSign < localeID.length() && localeID[equalSign] == ' ') {
                equalSign++;
            }

            /* Premature end or zero-length value */
            اگر (equalSign == localeID.length() || equalSign == semicolon) {
                حالت_2 = U_INVALID_FORMAT_ERROR;
                واپس;
            }

            keywordList[عدد_Keywords].قدر_آغاز = localeID.ڈیٹا() + equalSign;

            std::string_view value = localeID;
            اگر (semicolon != std::string_view::npos) {
                value.remove_suffix(value.length() - semicolon);
                localeID.remove_سابقہ(semicolon + 1);
            } ورنہ {
                localeID = {};
            }
            value.remove_سابقہ(equalSign);
            اگر (size_t آخری = value.find_last_not_of(' '); آخری != std::string_view::npos) {
                value.remove_suffix(value.length() - آخری - 1);
            }
            keywordList[عدد_Keywords].قدر_Len = static_cast<int32_t>(value.length());

            /* If this is a duplicate keyword, then ignore it */
            برائے (j=0; j<عدد_Keywords; ++j) {
                اگر (uprv_strcmp(keywordList[j].keyword, keywordList[عدد_Keywords].keyword) == 0) {
                    duplicate = سچ;
                    توڑو;
                }
            }
            اگر (!duplicate) {
                ++عدد_Keywords;
            }
        } جب (!localeID.empty());

        /* now we have a list of keywords */
        /* we need to sort it */
        uprv_ترتیب_دو_Array(keywordList, عدد_Keywords, حجم(KeywordStruct), compareKeywordStructs, خالی_اشاریہ, جھوٹ, &حالت_2);

        /* Now construct the keyword part */
        برائے(i = 0; i < عدد_Keywords; i++) {
            sink.Append(keywordList[i].keyword, keywordList[i].keywordLen);
            اگر(اقدار_Too) {
                sink.Append("=", 1);
                sink.Append(keywordList[i].قدر_آغاز, keywordList[i].قدر_Len);
                اگر(i < عدد_Keywords - 1) {
                    sink.Append(";", 1);
                }
            } ورنہ {
                sink.Append("\0", 1);
            }
        }
    }
}

U_CAPI int32_t U_EXPORT2
uloc_get_Keyword_قدر(ثابت حرف* localeID,
                     ثابت حرف* keywordName,
                     حرف* بفر, int32_t بفر_استعداد,
                     UError_رمز* حالت_2)
{
    اگر (U_FAILURE(*حالت_2)) { واپس 0; }
    اگر (keywordName == خالی_اشاریہ || *keywordName == '\0') {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }
    واپس ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Terminated_Chars(
        بفر, بفر_استعداد,
        [&](ByteSink& sink, UError_رمز& حالت_2) {
            ulocimp_get_Keyword_قدر(localeID, keywordName, sink, حالت_2);
        },
        *حالت_2);
}

U_EXPORT CharString
ulocimp_get_Keyword_قدر(ثابت حرف* localeID,
                        std::string_view keywordName,
                        UError_رمز& حالت_2)
{
    واپس ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Char_String(
        [&](ByteSink& sink, UError_رمز& حالت_2) {
            ulocimp_get_Keyword_قدر(localeID, keywordName, sink, حالت_2);
        },
        حالت_2);
}

U_EXPORT خالی
ulocimp_get_Keyword_قدر(ثابت حرف* localeID,
                        std::string_view keywordName,
                        icu::ByteSink& sink,
                        UError_رمز& حالت_2)
{
    اگر (U_FAILURE(حالت_2)) { واپس; }

    اگر (localeID == خالی_اشاریہ || keywordName.empty()) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    ثابت حرف* آغاز_تلاش_یہاں = خالی_اشاریہ;
    ثابت حرف* اگلا_Separator = خالی_اشاریہ;

    CharString عارضی_بفر;
    ثابت حرف* tmpLocaleID;

    CharString canonKeywordName = locale_canonKeywordName(keywordName, حالت_2);
    اگر (U_FAILURE(حالت_2)) {
      واپس;
    }

    اگر (localeID != خالی_اشاریہ && _hasBCP47Extension(localeID)) {
        عارضی_بفر = ulocimp_for_Language_لیبل(localeID, -1, خالی_اشاریہ, حالت_2);
        tmpLocaleID = U_SUCCESS(حالت_2) && !عارضی_بفر.is_خالی_خانے() ? عارضی_بفر.ڈیٹا() : localeID;
    } ورنہ {
        tmpLocaleID=localeID;
    }

    آغاز_تلاش_یہاں = locale_get_Keywords_آغاز(tmpLocaleID);
    اگر(آغاز_تلاش_یہاں == خالی_اشاریہ) {
        /* no keywords, return at once */
        واپس;
    }

    /* find the first keyword */
    جب(آغاز_تلاش_یہاں) {
        ثابت حرف* کلید_قدر_دم;

        آغاز_تلاش_یہاں++; /* skip @ or ; */
        اگلا_Separator = uprv_strchr(آغاز_تلاش_یہاں, '=');
        اگر(!اگلا_Separator) {
            حالت_2 = U_ILLEGAL_دلیل_ERROR; /* key must have =value */
            واپس;
        }
        /* strip leading & trailing spaces (TC decided to tolerate these) */
        جب(*آغاز_تلاش_یہاں == ' ') {
            آغاز_تلاش_یہاں++;
        }
        کلید_قدر_دم = اگلا_Separator;
        جب (کلید_قدر_دم > آغاز_تلاش_یہاں && *(کلید_قدر_دم-1) == ' ') {
            کلید_قدر_دم--;
        }
        /* now keyValueTail points to first char after the keyName */
        /* copy & normalize keyName from locale */
        اگر (آغاز_تلاش_یہاں == کلید_قدر_دم) {
            حالت_2 = U_ILLEGAL_دلیل_ERROR; /* empty keyword name in passed-in locale */
            واپس;
        }
        CharString localeKeywordName;
        جب (آغاز_تلاش_یہاں < کلید_قدر_دم) {
          اگر (!UPRV_ISALPHANUM(*آغاز_تلاش_یہاں)) {
            حالت_2 = U_ILLEGAL_دلیل_ERROR; /* malformed keyword name */
            واپس;
          }
          localeKeywordName.append(uprv_tolower(*آغاز_تلاش_یہاں++), حالت_2);
        }
        اگر (U_FAILURE(حالت_2)) {
            واپس;
        }

        آغاز_تلاش_یہاں = uprv_strchr(اگلا_Separator, ';');

        اگر (canonKeywordName == localeKeywordName) {
             /* current entry matches the keyword. */
           اگلا_Separator++; /* skip '=' */
            /* First strip leading & trailing spaces (TC decided to tolerate these) */
            جب(*اگلا_Separator == ' ') {
              اگلا_Separator++;
            }
            کلید_قدر_دم = (آغاز_تلاش_یہاں)? آغاز_تلاش_یہاں: اگلا_Separator + uprv_strlen(اگلا_Separator);
            جب(کلید_قدر_دم > اگلا_Separator && *(کلید_قدر_دم-1) == ' ') {
              کلید_قدر_دم--;
            }
            /* Now copy the value, but check well-formedness */
            اگر (اگلا_Separator == کلید_قدر_دم) {
              حالت_2 = U_ILLEGAL_دلیل_ERROR; /* empty key value name in passed-in locale */
              واپس;
            }
            جب (اگلا_Separator < کلید_قدر_دم) {
              اگر (!UPRV_ISALPHANUM(*اگلا_Separator) && !UPRV_OK_قدر_PUNCTUATION(*اگلا_Separator)) {
                حالت_2 = U_ILLEGAL_دلیل_ERROR; /* malformed key value */
                واپس;
              }
              /* Should we lowercase value to return here? Tests expect as-is. */
              sink.Append(اگلا_Separator++, 1);
            }
            واپس;
        }
    }
}

U_CAPI int32_t U_EXPORT2
uloc_سیٹ_Keyword_قدر(ثابت حرف* keywordName,
                     ثابت حرف* keyword_قدر,
                     حرف* بفر, int32_t بفر_استعداد,
                     UError_رمز* حالت_2)
{
    اگر (U_FAILURE(*حالت_2)) { واپس 0; }

    اگر (keywordName == خالی_اشاریہ || *keywordName == 0) {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    اگر (بفر_استعداد <= 1) {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    int32_t bufLen = (int32_t)uprv_strlen(بفر);
    اگر(بفر_استعداد<bufLen) {
        /* The capacity is less than the length?! Is this NUL terminated? */
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    حرف* keywords = const_cast<حرف*>(
        locale_get_Keywords_آغاز({بفر, static_cast<std::string_view::size_type>(bufLen)}));
    int32_t بنیاد_Len = keywords == خالی_اشاریہ ? bufLen : keywords - بفر;
    // Remove -1 from the capacity so that this function can guarantee NUL termination.
    CheckedArrayByteSink sink(keywords == خالی_اشاریہ ? بفر + bufLen : keywords,
                              بفر_استعداد - بنیاد_Len - 1);
    int32_t reslen = ulocimp_سیٹ_Keyword_قدر(
        keywords == خالی_اشاریہ ? std::string_view() : keywords,
        keywordName,
        keyword_قدر == خالی_اشاریہ ? std::string_view() : keyword_قدر,
        sink,
        *حالت_2);

    اگر (U_FAILURE(*حالت_2)) {
        واپس *حالت_2 == U_بفر_حد_سے_تجاوز_ERROR ? reslen + بنیاد_Len : 0;
    }

    // See the documentation for this function, it's guaranteed to never
    // overflow the buffer but instead abort with BUFFER_OVERFLOW_ERROR.
    // In this case, nothing has been written to the sink, so it cannot have Overflowed().
    U_ASSERT(!sink.Overflowed());
    U_ASSERT(reslen >= 0);
    واپس u_terminateChars(بفر, بفر_استعداد, reslen + بنیاد_Len, حالت_2);
}

U_EXPORT خالی
ulocimp_سیٹ_Keyword_قدر(std::string_view keywordName,
                        std::string_view keyword_قدر,
                        CharString& localeID,
                        UError_رمز& حالت_2)
{
    اگر (U_FAILURE(حالت_2)) { واپس; }
    std::string_view keywords;
    اگر (ثابت حرف* آغاز = locale_get_Keywords_آغاز(localeID.مقصد_String_Piece()); آغاز != خالی_اشاریہ) {
        // This is safe because CharString::truncate() doesn't actually erase any
        // data, but simply sets the position for where new data will be written.
        int32_t size = آغاز - localeID.ڈیٹا();
        keywords = localeID.مقصد_String_Piece();
        keywords.remove_سابقہ(size);
        localeID.truncate(size);
    }
    CharStringByteSink sink(&localeID);
    ulocimp_سیٹ_Keyword_قدر(keywords, keywordName, keyword_قدر, sink, حالت_2);
}

U_EXPORT int32_t
ulocimp_سیٹ_Keyword_قدر(std::string_view keywords,
                        std::string_view keywordName,
                        std::string_view keyword_قدر,
                        ByteSink& sink,
                        UError_رمز& حالت_2)
{
    اگر (U_FAILURE(حالت_2)) { واپس 0; }

    /* TODO: sorting. removal. */
    int32_t needLen = 0;
    int32_t rc;
    CharString updated_کلیدیں_And_اقدار;
    منطقی handled_داخلہ_کلید_And_قدر = جھوٹ;
    حرف کلید_قدر_سابقہ = '@';

    اگر (حالت_2 == U_STRING_NOT_TERMINATED_WARNING) {
        حالت_2 = U_ZERO_ERROR;
    }
    اگر (keywordName.empty()) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }
    CharString canonKeywordName = locale_canonKeywordName(keywordName, حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        واپس 0;
    }

    CharString canon_Keyword_قدر;
    برائے (حرف c : keyword_قدر) {
        اگر (!UPRV_ISALPHANUM(c) && !UPRV_OK_قدر_PUNCTUATION(c)) {
            حالت_2 = U_ILLEGAL_دلیل_ERROR; /* malformed key value */
            واپس 0;
        }
        /* Should we force lowercase in value to set? */
        canon_Keyword_قدر.append(c, حالت_2);
    }
    اگر (U_FAILURE(حالت_2)) {
        واپس 0;
    }

    اگر (keywords.size() <= 1) {
        اگر (canon_Keyword_قدر.is_خالی_خانے()) { /* no keywords = nothing to remove */
            U_ASSERT(حالت_2 != U_STRING_NOT_TERMINATED_WARNING);
            واپس 0;
        }

        needLen = 1 + canonKeywordName.length() + 1 + canon_Keyword_قدر.length();
        int32_t capacity = 0;
        حرف* بفر = sink.Get_Append_بفر(
                needLen, needLen, خالی_اشاریہ, needLen, &capacity);
        اگر (capacity < needLen || بفر == خالی_اشاریہ) {
            حالت_2 = U_بفر_حد_سے_تجاوز_ERROR;
            واپس needLen; /* no change */
        }
        حرف* it = بفر;

        *it++ = '@';
        uprv_memcpy(it, canonKeywordName.ڈیٹا(), canonKeywordName.length());
        it += canonKeywordName.length();
        *it++ = '=';
        uprv_memcpy(it, canon_Keyword_قدر.ڈیٹا(), canon_Keyword_قدر.length());
        sink.Append(بفر, needLen);
        U_ASSERT(حالت_2 != U_STRING_NOT_TERMINATED_WARNING);
        واپس needLen;
    } /* end shortcut - no @ */

    /* search for keyword */
    برائے (size_t keyword_آغاز = 0; keyword_آغاز != std::string_view::npos;) {
        keyword_آغاز++; /* skip @ or ; */
        size_t اگلا_Equalsign = keywords.find('=', keyword_آغاز);
        اگر (اگلا_Equalsign == std::string_view::npos) {
            حالت_2 = U_ILLEGAL_دلیل_ERROR; /* key must have =value */
            واپس 0;
        }
        /* strip leading & trailing spaces (TC decided to tolerate these) */
        جب (keyword_آغاز < keywords.size() && keywords[keyword_آغاز] == ' ') {
            keyword_آغاز++;
        }
        size_t کلید_قدر_دم = اگلا_Equalsign;
        جب (کلید_قدر_دم > keyword_آغاز && keywords[کلید_قدر_دم - 1] == ' ') {
            کلید_قدر_دم--;
        }
        /* now keyValueTail points to first char after the keyName */
        /* copy & normalize keyName from locale */
        اگر (keyword_آغاز == کلید_قدر_دم) {
            حالت_2 = U_ILLEGAL_دلیل_ERROR; /* empty keyword name in passed-in locale */
            واپس 0;
        }
        CharString localeKeywordName;
        جب (keyword_آغاز < کلید_قدر_دم) {
            اگر (!UPRV_ISALPHANUM(keywords[keyword_آغاز])) {
                حالت_2 = U_ILLEGAL_دلیل_ERROR; /* malformed keyword name */
                واپس 0;
            }
            localeKeywordName.append(uprv_tolower(keywords[keyword_آغاز++]), حالت_2);
        }
        اگر (U_FAILURE(حالت_2)) {
            واپس 0;
        }

        size_t اگلا_Separator = keywords.find(';', اگلا_Equalsign);

        /* start processing the value part */
        اگلا_Equalsign++; /* skip '=' */
        /* First strip leading & trailing spaces (TC decided to tolerate these) */
        جب (اگلا_Equalsign < keywords.size() && keywords[اگلا_Equalsign] == ' ') {
            اگلا_Equalsign++;
        }
        کلید_قدر_دم = اگلا_Separator == std::string_view::npos ? keywords.size() : اگلا_Separator;
        جب (کلید_قدر_دم > اگلا_Equalsign && keywords[کلید_قدر_دم - 1] == ' ') {
            کلید_قدر_دم--;
        }
        اگر (اگلا_Equalsign == کلید_قدر_دم) {
            حالت_2 = U_ILLEGAL_دلیل_ERROR; /* empty key value in passed-in locale */
            واپس 0;
        }

        rc = uprv_strcmp(canonKeywordName.ڈیٹا(), localeKeywordName.ڈیٹا());
        اگر(rc == 0) {
            /* Current entry matches the input keyword. Update the entry */
            اگر (!canon_Keyword_قدر.is_خالی_خانے()) { /* updating a value */
                updated_کلیدیں_And_اقدار.append(کلید_قدر_سابقہ, حالت_2);
                کلید_قدر_سابقہ = ';'; /* for any subsequent key-value pair */
                updated_کلیدیں_And_اقدار.append(canonKeywordName, حالت_2);
                updated_کلیدیں_And_اقدار.append('=', حالت_2);
                updated_کلیدیں_And_اقدار.append(canon_Keyword_قدر, حالت_2);
            } /* else removing this entry, don't emit anything */
            handled_داخلہ_کلید_And_قدر = سچ;
        } ورنہ {
           /* input keyword sorts earlier than current entry, add before current entry */
            اگر (rc < 0 && !canon_Keyword_قدر.is_خالی_خانے() && !handled_داخلہ_کلید_And_قدر) {
                /* insert new entry at this location */
                updated_کلیدیں_And_اقدار.append(کلید_قدر_سابقہ, حالت_2);
                کلید_قدر_سابقہ = ';'; /* for any subsequent key-value pair */
                updated_کلیدیں_And_اقدار.append(canonKeywordName, حالت_2);
                updated_کلیدیں_And_اقدار.append('=', حالت_2);
                updated_کلیدیں_And_اقدار.append(canon_Keyword_قدر, حالت_2);
                handled_داخلہ_کلید_And_قدر = سچ;
            }
            /* copy the current entry */
            updated_کلیدیں_And_اقدار.append(کلید_قدر_سابقہ, حالت_2);
            کلید_قدر_سابقہ = ';'; /* for any subsequent key-value pair */
            updated_کلیدیں_And_اقدار.append(localeKeywordName, حالت_2);
            updated_کلیدیں_And_اقدار.append('=', حالت_2);
            updated_کلیدیں_And_اقدار.append(keywords.ڈیٹا() + اگلا_Equalsign,
                                        static_cast<int32_t>(کلید_قدر_دم - اگلا_Equalsign), حالت_2);
        }
        اگر (اگلا_Separator == std::string_view::npos && !canon_Keyword_قدر.is_خالی_خانے() && !handled_داخلہ_کلید_And_قدر) {
            /* append new entry at the end, it sorts later than existing entries */
            updated_کلیدیں_And_اقدار.append(کلید_قدر_سابقہ, حالت_2);
            /* skip keyValuePrefix update, no subsequent key-value pair */
            updated_کلیدیں_And_اقدار.append(canonKeywordName, حالت_2);
            updated_کلیدیں_And_اقدار.append('=', حالت_2);
            updated_کلیدیں_And_اقدار.append(canon_Keyword_قدر, حالت_2);
            handled_داخلہ_کلید_And_قدر = سچ;
        }
        keyword_آغاز = اگلا_Separator;
    } /* end loop searching */

    /* Any error from updatedKeysAndValues.append above would be internal and not due to
     * problems with the passed-in locale. So if we did encounter problems with the
     * passed-in locale above, those errors took precedence and overrode any error
     * status from updatedKeysAndValues.append, and also caused a return of 0. If there
     * are errors here they are from updatedKeysAndValues.append; they do cause an
     * error return but the passed-in locale is unmodified and the original bufLen is
     * returned.
     */
    اگر (!handled_داخلہ_کلید_And_قدر || U_FAILURE(حالت_2)) {
        /* if input key/value specified removal of a keyword not present in locale, or
         * there was an error in CharString.append, leave original locale alone. */
        U_ASSERT(حالت_2 != U_STRING_NOT_TERMINATED_WARNING);
        // The sink is expected to be a buffer which already contains the full
        // locale string, so when it isn't going to be modified there's no need
        // to actually write any data to it, as the data is already there. Only
        // the first character needs to be overwritten (changing '\0' to '@').
        needLen = static_cast<int32_t>(keywords.size());
        int32_t capacity = 0;
        حرف* بفر = sink.Get_Append_بفر(
                needLen, needLen, خالی_اشاریہ, needLen, &capacity);
        اگر (capacity < needLen || بفر == خالی_اشاریہ) {
            حالت_2 = U_بفر_حد_سے_تجاوز_ERROR;
        } ورنہ {
            *بفر = '@';
            sink.Append(بفر, needLen);
        }
        واپس needLen;
    }

    needLen = updated_کلیدیں_And_اقدار.length();
    // Check to see can we fit the updatedKeysAndValues, if not, return
    // U_BUFFER_OVERFLOW_ERROR without copy updatedKeysAndValues into it.
    // We do this because this API function does not behave like most others:
    // It promises never to set a U_STRING_NOT_TERMINATED_WARNING.
    // When the contents fits but without the terminating NUL, in this case we need to not change
    // the buffer contents and return with a buffer overflow error.
    اگر (needLen > 0) {
        int32_t capacity = 0;
        حرف* بفر = sink.Get_Append_بفر(
                needLen, needLen, خالی_اشاریہ, needLen, &capacity);
        اگر (capacity < needLen || بفر == خالی_اشاریہ) {
            حالت_2 = U_بفر_حد_سے_تجاوز_ERROR;
            واپس needLen;
        }
        uprv_memcpy(بفر, updated_کلیدیں_And_اقدار.ڈیٹا(), needLen);
        sink.Append(بفر, needLen);
    }
    U_ASSERT(حالت_2 != U_STRING_NOT_TERMINATED_WARNING);
    واپس needLen;
}

/* ### ID parsing implementation **************************************************/

نامخلا {

اندرونی منطقی is_سابقہ_Letter(حرف a) { واپس a == 'x' || a == 'X' || a == 'i' || a == 'I'; }

/*returns true if one of the special prefixes is here (s=string)
  'x-' or 'i-' */
اندرونی منطقی _isIDPrefix(std::string_view s) {
    واپس s.size() >= 2 && is_سابقہ_Letter(s[0]) && _isIDSeparator(s[1]);
}

/* Dot terminates it because of POSIX form  where dot precedes the codepage
 * except for variant
 */
اندرونی منطقی _isTerminator(حرف a) { واپس a == '.' || a == '@'; }

اندرونی منطقی _isBCP47Extension(std::string_view p) {
    واپس p.size() >= 3 &&
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
std::optional<int16_t> ڈھونڈو_اشاریہ_2(ثابت حرف* ثابت* list, ثابت حرف* کلید)
{
    ثابت حرف* ثابت* anchor = list;
    int32_t pass = 0;

    /* Make two passes through two nullptr-terminated arrays at 'list' */
    جب (pass++ < 2) {
        جب (*list) {
            اگر (uprv_strcmp(کلید, *list) == 0) {
                واپس static_cast<int16_t>(list - anchor);
            }
            list++;
        }
        ++list;     /* skip final nullptr *CWB*/
    }
    واپس std::nullopt;
}

}  // namespace

U_CFUNC ثابت حرف*
uloc_getCurrentCountryID(ثابت حرف* oldID U_LIFETIME_BOUND) {
    std::optional<int16_t> انحراف = ڈھونڈو_اشاریہ_2(DEPRECATED_COUNTRIES, oldID);
    واپس انحراف.has_value() ? REPLACEMENT_COUNTRIES[*انحراف] : oldID;
}
U_CFUNC ثابت حرف*
uloc_getCurrentLanguageID(ثابت حرف* oldID U_LIFETIME_BOUND) {
    std::optional<int16_t> انحراف = ڈھونڈو_اشاریہ_2(DEPRECATED_LANGUAGES, oldID);
    واپس انحراف.has_value() ? REPLACEMENT_LANGUAGES[*انحراف] : oldID;
}

نامخلا {

/*
 * the internal functions _getLanguage(), _getScript(), _getRegion(), _getVariant()
 * avoid duplicating code to handle the earlier locale ID pieces
 * in the functions for the later ones by
 * setting the *pEnd pointer to where they stopped parsing
 *
 * TODO try to use this in Locale
 */

size_t _getLanguage(std::string_view localeID, ByteSink* sink, UError_رمز& حالت_2) {
    size_t skip = 0;
    اگر (localeID.size() == 4 && uprv_strnicmp(localeID.ڈیٹا(), "root", 4) == 0) {
        skip = 4;
        localeID.remove_سابقہ(skip);
    } ورنہ اگر (localeID.size() >= 3 && uprv_strnicmp(localeID.ڈیٹا(), "und", 3) == 0 &&
               (localeID.size() == 3 ||
                localeID[3] == '-' ||
                localeID[3] == '_' ||
                localeID[3] == '@')) {
        skip = 3;
        localeID.remove_سابقہ(skip);
    }

    constexpr int32_t MAXLEN = ULOC_LANG_استعداد - 1;  // Minus NUL.

    /* if it starts with i- or x- then copy that prefix */
    size_t len = _isIDPrefix(localeID) ? 2 : 0;
    جب (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len])) {
        اگر (len == MAXLEN) {
            حالت_2 = U_ILLEGAL_دلیل_ERROR;
            واپس 0;
        }
        len++;
    }

    اگر (sink == خالی_اشاریہ || len == 0) { واپس skip + len; }

    int32_t min_استعداد = uprv_max(static_cast<int32_t>(len), 4);  // Minimum 3 letters plus NUL.
    حرف scratch[MAXLEN];
    int32_t capacity = 0;
    حرف* بفر = sink->Get_Append_بفر(
            min_استعداد, min_استعداد, scratch, UPRV_LENGTHOF(scratch), &capacity);

    برائے (size_t i = 0; i < len; ++i) {
        بفر[i] = uprv_tolower(localeID[i]);
    }
    اگر (localeID.size() >= 2 && _isIDSeparator(localeID[1])) {
        بفر[1] = '-';
    }

    اگر (len == 3) {
        /* convert 3 character code to 2 character code if possible *CWB*/
        U_ASSERT(capacity >= 4);
        بفر[3] = '\0';
        std::optional<int16_t> انحراف = ڈھونڈو_اشاریہ_2(LANGUAGES_3, بفر);
        اگر (انحراف.has_value()) {
            ثابت حرف* ثابت alias = LANGUAGES[*انحراف];
            sink->Append(alias, static_cast<int32_t>(uprv_strlen(alias)));
            واپس skip + len;
        }
    }

    sink->Append(بفر, static_cast<int32_t>(len));
    واپس skip + len;
}

size_t _getScript(std::string_view localeID, ByteSink* sink) {
    constexpr int32_t لمبائی_3 = 4;

    size_t len = 0;
    جب (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len]) &&
            uprv_isASCIILetter(localeID[len])) {
        اگر (len == لمبائی_3) { واپس 0; }
        len++;
    }
    اگر (len != لمبائی_3) { واپس 0; }

    اگر (sink == خالی_اشاریہ) { واپس len; }

    حرف scratch[لمبائی_3];
    int32_t capacity = 0;
    حرف* بفر = sink->Get_Append_بفر(
            لمبائی_3, لمبائی_3, scratch, UPRV_LENGTHOF(scratch), &capacity);

    بفر[0] = uprv_toupper(localeID[0]);
    برائے (int32_t i = 1; i < لمبائی_3; ++i) {
        بفر[i] = uprv_tolower(localeID[i]);
    }

    sink->Append(بفر, لمبائی_3);
    واپس len;
}

size_t _getRegion(std::string_view localeID, ByteSink* sink) {
    constexpr int32_t MINLEN = 2;
    constexpr int32_t MAXLEN = ULOC_COUNTRY_استعداد - 1;  // Minus NUL.

    size_t len = 0;
    جب (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len])) {
        اگر (len == MAXLEN) { واپس 0; }
        len++;
    }
    اگر (len < MINLEN) { واپس 0; }

    اگر (sink == خالی_اشاریہ) { واپس len; }

    حرف scratch[ULOC_COUNTRY_استعداد];
    int32_t capacity = 0;
    حرف* بفر = sink->Get_Append_بفر(
            ULOC_COUNTRY_استعداد,
            ULOC_COUNTRY_استعداد,
            scratch,
            UPRV_LENGTHOF(scratch),
            &capacity);

    برائے (size_t i = 0; i < len; ++i) {
        بفر[i] = uprv_toupper(localeID[i]);
    }

    اگر (len == 3) {
        /* convert 3 character code to 2 character code if possible *CWB*/
        U_ASSERT(capacity >= 4);
        بفر[3] = '\0';
        std::optional<int16_t> انحراف = ڈھونڈو_اشاریہ_2(COUNTRIES_3, بفر);
        اگر (انحراف.has_value()) {
            ثابت حرف* ثابت alias = COUNTRIES[*انحراف];
            sink->Append(alias, static_cast<int32_t>(uprv_strlen(alias)));
            واپس len;
        }
    }

    sink->Append(بفر, static_cast<int32_t>(len));
    واپس len;
}

/**
 * @param needSeparator if true, then add leading '_' if any variants
 * are added to 'variant'
 */
size_t
_getVariant(std::string_view localeID,
            حرف پچھلا,
            ByteSink* sink,
            منطقی needSeparator,
            UError_رمز& حالت_2) {
    اگر (U_FAILURE(حالت_2) || localeID.empty()) واپس 0;

    // Reasonable upper limit for variants
    // There are no strict limitation of the syntax of variant in the legacy
    // locale format. If the locale is constructed from unicode_locale_id
    // as defined in UTS35, then we know each unicode_variant_subtag
    // could have max length of 8 ((alphanum{5,8} | digit alphanum{3})
    // 179 would allow 20 unicode_variant_subtag with sep in the
    // unicode_locale_id
    // 8*20 + 1*(20-1) = 179
    constexpr int32_t MAX_VARIANTS_لمبائی = 179;

    /* get one or more variant tags and separate them with '_' */
    size_t اشاریہ = 0;
    اگر (_isIDSeparator(پچھلا)) {
        /* get a variant string after a '-' or '_' */
        برائے (std::string_view sub = localeID;;) {
            size_t next = sub.find_first_of(".@_-");
            // For historical reasons, a trailing separator is included in the variant.
            منطقی ختم_شدہ = next == std::string_view::npos || next + 1 == sub.length();
            size_t حد_بندی = ختم_شدہ ? sub.length() : next;
            اشاریہ += حد_بندی;
            اگر (اشاریہ > MAX_VARIANTS_لمبائی) {
                حالت_2 = U_ILLEGAL_دلیل_ERROR;
                واپس 0;
            }

            اگر (sink != خالی_اشاریہ) {
                اگر (needSeparator) {
                    sink->Append("_", 1);
                } ورنہ {
                    needSeparator = سچ;
                }

                int32_t length = static_cast<int32_t>(حد_بندی);
                int32_t min_استعداد = uprv_min(length, MAX_VARIANTS_لمبائی);
                حرف scratch[MAX_VARIANTS_لمبائی];
                int32_t capacity = 0;
                حرف* بفر = sink->Get_Append_بفر(
                        min_استعداد, min_استعداد, scratch, UPRV_LENGTHOF(scratch), &capacity);

                برائے (size_t i = 0; i < حد_بندی; ++i) {
                    بفر[i] = uprv_toupper(sub[i]);
                }
                sink->Append(بفر, length);
            }

            اگر (ختم_شدہ) { واپس اشاریہ; }
            sub.remove_سابقہ(next);
            اگر (_isTerminator(sub.front()) || _isBCP47Extension(sub)) { واپس اشاریہ; }
            sub.remove_سابقہ(1);
            اشاریہ++;
        }
    }

    size_t skip = 0;
    /* if there is no variant tag after a '-' or '_' then look for '@' */
    اگر (پچھلا == '@') {
        /* keep localeID */
    } ورنہ اگر (ثابت حرف* p = locale_get_Keywords_آغاز(localeID); p != خالی_اشاریہ) {
        skip = 1 + p - localeID.ڈیٹا(); /* point after the '@' */
        localeID.remove_سابقہ(skip);
    } ورنہ {
        واپس 0;
    }
    برائے (; اشاریہ < localeID.size() && !_isTerminator(localeID[اشاریہ]); اشاریہ++) {
        اگر (اشاریہ >= MAX_VARIANTS_لمبائی) { // same as length > MAX_VARIANTS_LENGTH
            حالت_2 = U_ILLEGAL_دلیل_ERROR;
            واپس 0;
        }
        اگر (needSeparator) {
            اگر (sink != خالی_اشاریہ) {
                sink->Append("_", 1);
            }
            needSeparator = جھوٹ;
        }
        اگر (sink != خالی_اشاریہ) {
            حرف c = uprv_toupper(localeID[اشاریہ]);
            اگر (c == '-' || c == ',') c = '_';
            sink->Append(&c, 1);
        }
    }
    واپس skip + اشاریہ;
}

}  // namespace

U_EXPORT CharString
ulocimp_getLanguage(std::string_view localeID, UError_رمز& حالت_2) {
    واپس ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Char_String(
        [&](ByteSink& sink, UError_رمز& حالت_2) {
            ulocimp_getSubtags(
                    localeID,
                    &sink,
                    خالی_اشاریہ,
                    خالی_اشاریہ,
                    خالی_اشاریہ,
                    خالی_اشاریہ,
                    حالت_2);
        },
        حالت_2);
}

U_EXPORT CharString
ulocimp_getScript(std::string_view localeID, UError_رمز& حالت_2) {
    واپس ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Char_String(
        [&](ByteSink& sink, UError_رمز& حالت_2) {
            ulocimp_getSubtags(
                    localeID,
                    خالی_اشاریہ,
                    &sink,
                    خالی_اشاریہ,
                    خالی_اشاریہ,
                    خالی_اشاریہ,
                    حالت_2);
        },
        حالت_2);
}

U_EXPORT CharString
ulocimp_getRegion(std::string_view localeID, UError_رمز& حالت_2) {
    واپس ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Char_String(
        [&](ByteSink& sink, UError_رمز& حالت_2) {
            ulocimp_getSubtags(
                    localeID,
                    خالی_اشاریہ,
                    خالی_اشاریہ,
                    &sink,
                    خالی_اشاریہ,
                    خالی_اشاریہ,
                    حالت_2);
        },
        حالت_2);
}

U_EXPORT CharString
ulocimp_getVariant(std::string_view localeID, UError_رمز& حالت_2) {
    واپس ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Char_String(
        [&](ByteSink& sink, UError_رمز& حالت_2) {
            ulocimp_getSubtags(
                    localeID,
                    خالی_اشاریہ,
                    خالی_اشاریہ,
                    خالی_اشاریہ,
                    &sink,
                    خالی_اشاریہ,
                    حالت_2);
        },
        حالت_2);
}

U_EXPORT خالی
ulocimp_getSubtags(
        std::string_view localeID,
        CharString* language,
        CharString* script,
        CharString* region,
        CharString* variant,
        ثابت حرف** p_انتہا,
        UError_رمز& حالت_2) {
    اگر (U_FAILURE(حالت_2)) { واپس; }

    std::optional<CharStringByteSink> languageSink;
    std::optional<CharStringByteSink> scriptSink;
    std::optional<CharStringByteSink> regionSink;
    std::optional<CharStringByteSink> variantSink;

    اگر (language != خالی_اشاریہ) { languageSink.emplace(language); }
    اگر (script != خالی_اشاریہ) { scriptSink.emplace(script); }
    اگر (region != خالی_اشاریہ) { regionSink.emplace(region); }
    اگر (variant != خالی_اشاریہ) { variantSink.emplace(variant); }

    ulocimp_getSubtags(
            localeID,
            languageSink.has_value() ? &*languageSink : خالی_اشاریہ,
            scriptSink.has_value() ? &*scriptSink : خالی_اشاریہ,
            regionSink.has_value() ? &*regionSink : خالی_اشاریہ,
            variantSink.has_value() ? &*variantSink : خالی_اشاریہ,
            p_انتہا,
            حالت_2);
}

U_EXPORT خالی
ulocimp_getSubtags(
        std::string_view localeID,
        ByteSink* language,
        ByteSink* script,
        ByteSink* region,
        ByteSink* variant,
        ثابت حرف** p_انتہا,
        UError_رمز& حالت_2) {
    اگر (U_FAILURE(حالت_2)) { واپس; }

    اگر (p_انتہا != خالی_اشاریہ) {
        *p_انتہا = localeID.ڈیٹا();
    } ورنہ اگر (language == خالی_اشاریہ &&
               script == خالی_اشاریہ &&
               region == خالی_اشاریہ &&
               variant == خالی_اشاریہ) {
        واپس;
    }

    اگر (localeID.empty()) { واپس; }

    منطقی hasRegion = جھوٹ;

    {
        size_t len = _getLanguage(localeID, language, حالت_2);
        اگر (U_FAILURE(حالت_2)) { واپس; }
        اگر (len > 0) {
            localeID.remove_سابقہ(len);
        }
    }

    اگر (p_انتہا != خالی_اشاریہ) {
        *p_انتہا = localeID.ڈیٹا();
    } ورنہ اگر (script == خالی_اشاریہ &&
               region == خالی_اشاریہ &&
               variant == خالی_اشاریہ) {
        واپس;
    }

    اگر (localeID.empty()) { واپس; }

    اگر (_isIDSeparator(localeID.front())) {
        std::string_view sub = localeID;
        sub.remove_سابقہ(1);
        size_t len = _getScript(sub, script);
        اگر (len > 0) {
            localeID.remove_سابقہ(len + 1);
            اگر (p_انتہا != خالی_اشاریہ) { *p_انتہا = localeID.ڈیٹا(); }
        }
    }

    اگر ((region == خالی_اشاریہ && variant == خالی_اشاریہ && p_انتہا == خالی_اشاریہ) || localeID.empty()) { واپس; }

    اگر (_isIDSeparator(localeID.front())) {
        std::string_view sub = localeID;
        sub.remove_سابقہ(1);
        size_t len = _getRegion(sub, region);
        اگر (len > 0) {
            hasRegion = سچ;
            localeID.remove_سابقہ(len + 1);
            اگر (p_انتہا != خالی_اشاریہ) { *p_انتہا = localeID.ڈیٹا(); }
        }
    }

    اگر ((variant == خالی_اشاریہ && p_انتہا == خالی_اشاریہ) || localeID.empty()) { واپس; }

    منطقی hasVariant = جھوٹ;

    اگر (_isIDSeparator(localeID.front()) && !_isBCP47Extension(localeID)) {
        std::string_view sub = localeID;
        /* If there was no country ID, skip a possible extra IDSeparator */
        size_t skip = !hasRegion && localeID.size() > 1 && _isIDSeparator(localeID[1]) ? 2 : 1;
        sub.remove_سابقہ(skip);
        size_t len = _getVariant(sub, localeID[0], variant, جھوٹ, حالت_2);
        اگر (U_FAILURE(حالت_2)) { واپس; }
        اگر (len > 0) {
            hasVariant = سچ;
            localeID.remove_سابقہ(skip + len);
            اگر (p_انتہا != خالی_اشاریہ) { *p_انتہا = localeID.ڈیٹا(); }
        }
    }

    اگر ((variant == خالی_اشاریہ && p_انتہا == خالی_اشاریہ) || localeID.empty()) { واپس; }

    اگر (_isBCP47Extension(localeID)) {
        localeID.remove_سابقہ(2);
        constexpr حرف vaposix[] = "-va-posix";
        constexpr size_t length = حجم vaposix - 1;
        برائے (size_t next;; localeID.remove_سابقہ(next)) {
            next = localeID.find('-', 1);
            اگر (next == std::string_view::npos) { توڑو; }
            next = localeID.find('-', next + 1);
            منطقی ختم_شدہ = next == std::string_view::npos;
            std::string_view sub = localeID;
            اگر (!ختم_شدہ) { sub.remove_suffix(sub.length() - next); }

            اگر (sub.length() == length && uprv_strnicmp(sub.ڈیٹا(), vaposix, length) == 0) {
                اگر (variant != خالی_اشاریہ) {
                    اگر (hasVariant) { variant->Append("_", 1); }
                    constexpr حرف posix[] = "POSIX";
                    variant->Append(posix, حجم posix - 1);
                }
                اگر (p_انتہا != خالی_اشاریہ) { *p_انتہا = localeID.ڈیٹا() + length; }
            }

            اگر (ختم_شدہ) { توڑو; }
        }
    }
}

/* Keyword enumeration */

قسمتعریف ساخت UKeywordsContext {
    حرف* keywords;
    حرف* current;
} UKeywordsContext;

U_CDECL_BEGIN

ساکن خالی U_CALLCONV
uloc_kw_closeKeywords(UEnumeration *enumerator) {
    uprv_free(((UKeywordsContext *)enumerator->context)->keywords);
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

ساکن int32_t U_CALLCONV
uloc_kw_گنتی_Keywords(UEnumeration *en, UError_رمز * /*حالت*/) {
    حرف *kw = ((UKeywordsContext *)en->context)->keywords;
    int32_t نتیجہ = 0;
    جب(*kw) {
        نتیجہ++;
        kw += uprv_strlen(kw)+1;
    }
    واپس نتیجہ;
}

ساکن ثابت حرف * U_CALLCONV
uloc_kw_اگلا_Keyword(UEnumeration* en,
                    int32_t* نتیجہ_لمبائی,
                    UError_رمز* /*حالت*/) {
    ثابت حرف* نتیجہ = ((UKeywordsContext *)en->context)->current;
    int32_t len = 0;
    اگر(*نتیجہ) {
        len = (int32_t)uprv_strlen(((UKeywordsContext *)en->context)->current);
        ((UKeywordsContext *)en->context)->current += len+1;
    } ورنہ {
        نتیجہ = خالی_اشاریہ;
    }
    اگر (نتیجہ_لمبائی) {
        *نتیجہ_لمبائی = len;
    }
    واپس نتیجہ;
}

ساکن خالی U_CALLCONV
uloc_kw_resetKeywords(UEnumeration* en,
                      UError_رمز* /*حالت*/) {
    ((UKeywordsContext *)en->context)->current = ((UKeywordsContext *)en->context)->keywords;
}

U_CDECL_انتہا


ساکن ثابت UEnumeration gKeywordsEnum = {
    خالی_اشاریہ,
    خالی_اشاریہ,
    uloc_kw_closeKeywords,
    uloc_kw_گنتی_Keywords,
    uenum_unextDefault,
    uloc_kw_اگلا_Keyword,
    uloc_kw_resetKeywords
};

U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywordList(ثابت حرف *keywordList, int32_t keyword_List_سائز, UError_رمز* حالت_2)
{
    اگر (U_FAILURE(*حالت_2)) { واپس خالی_اشاریہ; }

    Local_حافظہ<UKeywordsContext> myContext;
    Local_حافظہ<UEnumeration> نتیجہ;

    myContext.adoptInstead(static_cast<UKeywordsContext *>(uprv_malloc(حجم(UKeywordsContext))));
    نتیجہ.adoptInstead(static_cast<UEnumeration *>(uprv_malloc(حجم(UEnumeration))));
    اگر (myContext.isNull() || نتیجہ.isNull()) {
        *حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }
    uprv_memcpy(نتیجہ.getAlias(), &gKeywordsEnum, حجم(UEnumeration));
    myContext->keywords = static_cast<حرف *>(uprv_malloc(keyword_List_سائز+1));
    اگر (myContext->keywords == خالی_اشاریہ) {
        *حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }
    uprv_memcpy(myContext->keywords, keywordList, keyword_List_سائز);
    myContext->keywords[keyword_List_سائز] = 0;
    myContext->current = myContext->keywords;
    نتیجہ->context = myContext.orphan();
    واپس نتیجہ.orphan();
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywords(ثابت حرف* localeID,
                        UError_رمز* حالت_2)
{
    اگر(حالت_2==خالی_اشاریہ || U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }

    CharString عارضی_بفر;
    ثابت حرف* tmpLocaleID;

    اگر (localeID != خالی_اشاریہ && _hasBCP47Extension(localeID)) {
        عارضی_بفر = ulocimp_for_Language_لیبل(localeID, -1, خالی_اشاریہ, *حالت_2);
        tmpLocaleID = U_SUCCESS(*حالت_2) && !عارضی_بفر.is_خالی_خانے() ? عارضی_بفر.ڈیٹا() : localeID;
    } ورنہ {
        اگر (localeID==خالی_اشاریہ) {
            localeID=uloc_getDefault();
        }
        tmpLocaleID=localeID;
    }

    ulocimp_getSubtags(
            tmpLocaleID,
            خالی_اشاریہ,
            خالی_اشاریہ,
            خالی_اشاریہ,
            خالی_اشاریہ,
            &tmpLocaleID,
            *حالت_2);
    اگر (U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }

    /* keywords are located after '@' */
    اگر((tmpLocaleID = locale_get_Keywords_آغاز(tmpLocaleID)) != خالی_اشاریہ) {
        CharString keywords = ulocimp_getKeywords(tmpLocaleID + 1, '@', جھوٹ, *حالت_2);
        اگر (U_FAILURE(*حالت_2)) {
            واپس خالی_اشاریہ;
        }
        واپس uloc_openKeywordList(keywords.ڈیٹا(), keywords.length(), حالت_2);
    }
    واپس خالی_اشاریہ;
}


/* bit-flags for 'options' parameter of _canonicalize */
#تعریف_کرو _ULOC_STRIP_KEYWORDS 0x2
#تعریف_کرو _ULOC_CANONICALIZE   0x1

نامخلا {

اندرونی منطقی OPTION_سیٹ(uint32_t options, uint32_t mask) { واپس (options & mask) != 0; }

constexpr حرف i_default[] = {'i', '-', 'd', 'e', 'f', 'a', 'u', 'l', 't'};
constexpr int32_t I_DEFAULT_لمبائی = UPRV_LENGTHOF(i_default);

/**
 * Canonicalize the given localeID, to level 1 or to level 2,
 * depending on the options.  To specify level 1, pass in options=0.
 * To specify level 2, pass in options=_ULOC_CANONICALIZE.
 *
 * This is the code underlying uloc_getName and uloc_canonicalize.
 */
خالی
_canonicalize(std::string_view localeID,
              ByteSink& sink,
              uint32_t options,
              UError_رمز& err) {
    اگر (U_FAILURE(err)) {
        واپس;
    }

    int32_t j, میدان_گنتی=0;
    CharString عارضی_بفر;  // if localeID has a BCP47 extension, tmpLocaleID points to this
    CharString localeIDWithHyphens;  // if localeID has a BPC47 extension and have _, tmpLocaleID points to this
    std::string_view origLocaleID;
    std::string_view tmpLocaleID;
    size_t keyword_تفویض = std::string_view::npos;
    size_t separatorIndicator = std::string_view::npos;

    اگر (_hasBCP47Extension(localeID)) {
        std::string_view localeIDPtr = localeID;

        // convert all underbars to hyphens, unless the "BCP47 extension" comes at the beginning of the string
        اگر (localeID.size() >= 2 && localeID.find('_') != std::string_view::npos && localeID[1] != '-' && localeID[1] != '_') {
            localeIDWithHyphens.append(localeID, err);
            اگر (U_SUCCESS(err)) {
                برائے (حرف* p = localeIDWithHyphens.ڈیٹا(); *p != '\0'; ++p) {
                    اگر (*p == '_') {
                        *p = '-';
                    }
                }
                localeIDPtr = localeIDWithHyphens.مقصد_String_Piece();
            }
        }

        عارضی_بفر = ulocimp_for_Language_لیبل(localeIDPtr.ڈیٹا(), static_cast<int32_t>(localeIDPtr.size()), خالی_اشاریہ, err);
        tmpLocaleID = U_SUCCESS(err) && !عارضی_بفر.is_خالی_خانے() ? static_cast<std::string_view>(عارضی_بفر.مقصد_String_Piece()) : localeIDPtr;
    } ورنہ {
        tmpLocaleID=localeID;
    }

    origLocaleID=tmpLocaleID;

    /* get all pieces, one after another, and separate with '_' */
    CharString لیبل;
    CharString script;
    CharString country;
    CharString variant;
    ثابت حرف* انتہا = خالی_اشاریہ;
    ulocimp_getSubtags(
            tmpLocaleID,
            &لیبل,
            &script,
            &country,
            &variant,
            &انتہا,
            err);
    اگر (U_FAILURE(err)) {
        واپس;
    }
    U_ASSERT(انتہا != خالی_اشاریہ);
    اگر (انتہا > tmpLocaleID.ڈیٹا()) {
        tmpLocaleID.remove_سابقہ(انتہا - tmpLocaleID.ڈیٹا());
    }

    اگر (لیبل.length() == I_DEFAULT_لمبائی && origLocaleID.length() >= I_DEFAULT_لمبائی &&
            uprv_strncmp(origLocaleID.ڈیٹا(), i_default, I_DEFAULT_لمبائی) == 0) {
        لیبل.clear();
        لیبل.append(uloc_getDefault(), err);
    } ورنہ {
        اگر (!script.is_خالی_خانے()) {
            ++میدان_گنتی;
            لیبل.append('_', err);
            لیبل.append(script, err);
        }
        اگر (!country.is_خالی_خانے()) {
            ++میدان_گنتی;
            لیبل.append('_', err);
            لیبل.append(country, err);
        }
        اگر (!variant.is_خالی_خانے()) {
            ++میدان_گنتی;
            اگر (country.is_خالی_خانے()) {
                لیبل.append('_', err);
            }
            لیبل.append('_', err);
            لیبل.append(variant, err);
        }
    }

    /* Copy POSIX-style charset specifier, if any [mr.utf8] */
    اگر (!OPTION_سیٹ(options, _ULOC_CANONICALIZE) && !tmpLocaleID.empty() && tmpLocaleID.front() == '.') {
        لیبل.append('.', err);
        tmpLocaleID.remove_سابقہ(1);
        size_t length;
        اگر (size_t atPos = tmpLocaleID.find('@'); atPos != std::string_view::npos) {
            length = atPos;
        } ورنہ {
            length = tmpLocaleID.length();
        }
        // The longest charset name we found in IANA charset registry
        // https://www.iana.org/assignments/character-sets/ is
        // "Extended_UNIX_Code_Packed_Format_for_Japanese" in length 45.
        // we therefore restrict the length here to be 64 which is a power of 2
        // number that is longer than 45.
        constexpr size_t k_Max_Charset_لمبائی = 64;
        اگر (length > k_Max_Charset_لمبائی) {
           err = U_ILLEGAL_دلیل_ERROR; /* malformed keyword name */
           واپس;
        }
        اگر (length > 0) {
            لیبل.append(tmpLocaleID.ڈیٹا(), static_cast<int32_t>(length), err);
            tmpLocaleID.remove_سابقہ(length);
        }
    }

    /* Scan ahead to next '@' and determine if it is followed by '=' and/or ';'
       After this, tmpLocaleID either starts at '@' or is empty. */
    اگر (ثابت حرف* آغاز = locale_get_Keywords_آغاز(tmpLocaleID); آغاز != خالی_اشاریہ) {
        اگر (آغاز > tmpLocaleID.ڈیٹا()) {
            tmpLocaleID.remove_سابقہ(آغاز - tmpLocaleID.ڈیٹا());
        }
        keyword_تفویض = tmpLocaleID.find('=');
        separatorIndicator = tmpLocaleID.find(';');
    } ورنہ {
        tmpLocaleID = {};
    }

    /* Copy POSIX-style variant, if any [mr@FOO] */
    اگر (!OPTION_سیٹ(options, _ULOC_CANONICALIZE) &&
        !tmpLocaleID.empty() && keyword_تفویض == std::string_view::npos) {
        لیبل.append(tmpLocaleID, err);
        tmpLocaleID = {};
    }

    اگر (OPTION_سیٹ(options, _ULOC_CANONICALIZE)) {
        /* Handle @FOO variant if @ is present and not followed by = */
        اگر (!tmpLocaleID.empty() && keyword_تفویض == std::string_view::npos) {
            /* Add missing '_' if needed */
            اگر (میدان_گنتی < 2 || (میدان_گنتی < 3 && !script.is_خالی_خانے())) {
                کرو {
                    لیبل.append('_', err);
                    ++میدان_گنتی;
                } جب(میدان_گنتی<2);
            }

            CharStringByteSink s(&لیبل);
            std::string_view sub = tmpLocaleID;
            sub.remove_سابقہ(1);
            _getVariant(sub, '@', &s, !variant.is_خالی_خانے(), err);
            اگر (U_FAILURE(err)) { واپس; }
        }

        /* Look up the ID in the canonicalization map */
        برائے (j=0; j<UPRV_LENGTHOF(CANONICALIZE_MAP); j++) {
            StringPiece id(CANONICALIZE_MAP[j].id);
            اگر (لیبل == id) {
                اگر (id.empty() && !tmpLocaleID.empty()) {
                    توڑو; /* Don't remap "" if keywords present */
                }
                لیبل.clear();
                لیبل.append(CANONICALIZE_MAP[j].canonicalID, err);
                توڑو;
            }
        }
    }

    sink.Append(لیبل.ڈیٹا(), لیبل.length());

    اگر (!OPTION_سیٹ(options, _ULOC_STRIP_KEYWORDS)) {
        اگر (!tmpLocaleID.empty() && keyword_تفویض != std::string_view::npos &&
            (separatorIndicator == std::string_view::npos || separatorIndicator > keyword_تفویض)) {
            sink.Append("@", 1);
            ++میدان_گنتی;
            tmpLocaleID.remove_سابقہ(1);
            ulocimp_getKeywords(tmpLocaleID, '@', sink, سچ, err);
        }
    }
}

}  // namespace

/* ### ID parsing API **************************************************/

U_CAPI int32_t  U_EXPORT2
uloc_get_والد(ثابت حرف*    localeID,
               حرف* والد,
               int32_t والد_استعداد,
               UError_رمز* err)
{
    واپس ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Terminated_Chars(
        والد, والد_استعداد,
        [&](ByteSink& sink, UError_رمز& حالت_2) {
            ulocimp_get_والد(localeID, sink, حالت_2);
        },
        *err);
}

U_EXPORT CharString
ulocimp_get_والد(ثابت حرف* localeID,
                  UError_رمز& err)
{
    واپس ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Char_String(
        [&](ByteSink& sink, UError_رمز& حالت_2) {
            ulocimp_get_والد(localeID, sink, حالت_2);
        },
        err);
}

U_EXPORT خالی
ulocimp_get_والد(ثابت حرف* localeID,
                  icu::ByteSink& sink,
                  UError_رمز& err)
{
    اگر (U_FAILURE(err)) { واپس; }

    ثابت حرف *آخری_Underscore;
    int32_t i;

    اگر (localeID == خالی_اشاریہ)
        localeID = uloc_getDefault();

    آخری_Underscore=uprv_strrchr(localeID, '_');
    اگر(آخری_Underscore!=خالی_اشاریہ) {
        i = static_cast<int32_t>(آخری_Underscore - localeID);
    } ورنہ {
        i=0;
    }

    اگر (i > 0) {
        اگر (uprv_strnicmp(localeID, "und_", 4) == 0) {
            localeID += 3;
            i -= 3;
        }
        sink.Append(localeID, i);
    }
}

U_CAPI int32_t U_EXPORT2
uloc_getLanguage(ثابت حرف*    localeID,
         حرف* language,
         int32_t language_استعداد,
         UError_رمز* err)
{
    اگر (localeID == خالی_اشاریہ) {
        localeID = uloc_getDefault();
    }

    /* uloc_getLanguage will return a 2 character iso-639 code if one exists. *CWB*/
    واپس ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Terminated_Chars(
        language, language_استعداد,
        [&](ByteSink& sink, UError_رمز& حالت_2) {
            ulocimp_getSubtags(
                    localeID,
                    &sink,
                    خالی_اشاریہ,
                    خالی_اشاریہ,
                    خالی_اشاریہ,
                    خالی_اشاریہ,
                    حالت_2);
        },
        *err);
}

U_CAPI int32_t U_EXPORT2
uloc_getScript(ثابت حرف*    localeID,
         حرف* script,
         int32_t script_استعداد,
         UError_رمز* err)
{
    اگر (localeID == خالی_اشاریہ) {
        localeID = uloc_getDefault();
    }

    واپس ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Terminated_Chars(
        script, script_استعداد,
        [&](ByteSink& sink, UError_رمز& حالت_2) {
            ulocimp_getSubtags(
                    localeID,
                    خالی_اشاریہ,
                    &sink,
                    خالی_اشاریہ,
                    خالی_اشاریہ,
                    خالی_اشاریہ,
                    حالت_2);
        },
        *err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getCountry(ثابت حرف* localeID,
            حرف* country,
            int32_t country_استعداد,
            UError_رمز* err)
{
    اگر (localeID == خالی_اشاریہ) {
        localeID = uloc_getDefault();
    }

    واپس ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Terminated_Chars(
        country, country_استعداد,
        [&](ByteSink& sink, UError_رمز& حالت_2) {
            ulocimp_getSubtags(
                    localeID,
                    خالی_اشاریہ,
                    خالی_اشاریہ,
                    &sink,
                    خالی_اشاریہ,
                    خالی_اشاریہ,
                    حالت_2);
        },
        *err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getVariant(ثابت حرف* localeID,
                حرف* variant,
                int32_t variant_استعداد,
                UError_رمز* err)
{
    اگر (localeID == خالی_اشاریہ) {
        localeID = uloc_getDefault();
    }

    واپس ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Terminated_Chars(
        variant, variant_استعداد,
        [&](ByteSink& sink, UError_رمز& حالت_2) {
            ulocimp_getSubtags(
                    localeID,
                    خالی_اشاریہ,
                    خالی_اشاریہ,
                    خالی_اشاریہ,
                    &sink,
                    خالی_اشاریہ,
                    حالت_2);
        },
        *err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getName(ثابت حرف* localeID,
             حرف* name,
             int32_t name_استعداد,
             UError_رمز* err)
{
    اگر (localeID == خالی_اشاریہ) {
        localeID = uloc_getDefault();
    }
    واپس ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Terminated_Chars(
        name, name_استعداد,
        [&](ByteSink& sink, UError_رمز& حالت_2) {
            ulocimp_getName(localeID, sink, حالت_2);
        },
        *err);
}

U_EXPORT CharString
ulocimp_getName(std::string_view localeID,
                UError_رمز& err)
{
    واپس ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Char_String(
        [&](ByteSink& sink, UError_رمز& حالت_2) {
            ulocimp_getName(localeID, sink, حالت_2);
        },
        err);
}

U_EXPORT خالی
ulocimp_getName(std::string_view localeID,
                ByteSink& sink,
                UError_رمز& err)
{
    _canonicalize(localeID, sink, 0, err);
}

U_CAPI int32_t  U_EXPORT2
uloc_get_بنیاد_Name(ثابت حرف* localeID,
                 حرف* name,
                 int32_t name_استعداد,
                 UError_رمز* err)
{
    اگر (localeID == خالی_اشاریہ) {
        localeID = uloc_getDefault();
    }
    واپس ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Terminated_Chars(
        name, name_استعداد,
        [&](ByteSink& sink, UError_رمز& حالت_2) {
            ulocimp_get_بنیاد_Name(localeID, sink, حالت_2);
        },
        *err);
}

U_EXPORT CharString
ulocimp_get_بنیاد_Name(std::string_view localeID,
                    UError_رمز& err)
{
    واپس ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Char_String(
        [&](ByteSink& sink, UError_رمز& حالت_2) {
            ulocimp_get_بنیاد_Name(localeID, sink, حالت_2);
        },
        err);
}

U_EXPORT خالی
ulocimp_get_بنیاد_Name(std::string_view localeID,
                    ByteSink& sink,
                    UError_رمز& err)
{
    _canonicalize(localeID, sink, _ULOC_STRIP_KEYWORDS, err);
}

U_CAPI int32_t  U_EXPORT2
uloc_canonicalize(ثابت حرف* localeID,
                  حرف* name,
                  int32_t name_استعداد,
                  UError_رمز* err)
{
    اگر (localeID == خالی_اشاریہ) {
        localeID = uloc_getDefault();
    }
    واپس ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Terminated_Chars(
        name, name_استعداد,
        [&](ByteSink& sink, UError_رمز& حالت_2) {
            ulocimp_canonicalize(localeID, sink, حالت_2);
        },
        *err);
}

U_EXPORT CharString
ulocimp_canonicalize(std::string_view localeID,
                     UError_رمز& err)
{
    واپس ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Char_String(
        [&](ByteSink& sink, UError_رمز& حالت_2) {
            ulocimp_canonicalize(localeID, sink, حالت_2);
        },
        err);
}

U_EXPORT خالی
ulocimp_canonicalize(std::string_view localeID,
                     ByteSink& sink,
                     UError_رمز& err)
{
    _canonicalize(localeID, sink, _ULOC_CANONICALIZE, err);
}

U_CAPI ثابت حرف*  U_EXPORT2
uloc_getISO3Language(ثابت حرف* localeID)
{
    UError_رمز err = U_ZERO_ERROR;

    اگر (localeID == خالی_اشاریہ)
    {
        localeID = uloc_getDefault();
    }
    CharString lang = ulocimp_getLanguage(localeID, err);
    اگر (U_FAILURE(err))
        واپس "";
    std::optional<int16_t> انحراف = ڈھونڈو_اشاریہ_2(LANGUAGES, lang.ڈیٹا());
    واپس انحراف.has_value() ? LANGUAGES_3[*انحراف] : "";
}

U_CAPI ثابت حرف*  U_EXPORT2
uloc_getISO3Country(ثابت حرف* localeID)
{
    UError_رمز err = U_ZERO_ERROR;

    اگر (localeID == خالی_اشاریہ)
    {
        localeID = uloc_getDefault();
    }
    CharString cntry = ulocimp_getRegion(localeID, err);
    اگر (U_FAILURE(err))
        واپس "";
    std::optional<int16_t> انحراف = ڈھونڈو_اشاریہ_2(COUNTRIES, cntry.ڈیٹا());
    واپس انحراف.has_value() ? COUNTRIES_3[*انحراف] : "";
}

U_CAPI uint32_t  U_EXPORT2
uloc_getLCID(ثابت حرف* localeID)
{
    UError_رمز حالت_2 = U_ZERO_ERROR;
    uint32_t   lcid = 0;

    /* Check for incomplete id. */
    اگر (!localeID || uprv_strlen(localeID) < 2) {
        واپس 0;
    }

    // First, attempt Windows platform lookup if available, but fall
    // through to catch any special cases (ICU vs Windows name differences).
    lcid = uprv_convert_مقصد_LCIDPlatform(localeID, &حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        واپس 0;
    }
    اگر (lcid > 0) {
        // Windows found an LCID, return that
        واپس lcid;
    }

    CharString langID = ulocimp_getLanguage(localeID, حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        واپس 0;
    }

    اگر (uprv_strchr(localeID, '@')) {
        // uprv_convertToLCID does not support keywords other than collation.
        // Remove all keywords except collation.
        CharString collVal = ulocimp_get_Keyword_قدر(localeID, "collation", حالت_2);
        اگر (U_SUCCESS(حالت_2) && !collVal.is_خالی_خانے()) {
            CharString tmpLocaleID = ulocimp_get_بنیاد_Name(localeID, حالت_2);
            ulocimp_سیٹ_Keyword_قدر("collation", collVal.مقصد_String_Piece(), tmpLocaleID, حالت_2);
            اگر (U_SUCCESS(حالت_2)) {
                واپس uprv_convert_مقصد_LCID(langID.ڈیٹا(), tmpLocaleID.ڈیٹا(), &حالت_2);
            }
        }

        // fall through - all keywords are simply ignored
        حالت_2 = U_ZERO_ERROR;
    }

    واپس uprv_convert_مقصد_LCID(langID.ڈیٹا(), localeID, &حالت_2);
}

U_CAPI int32_t U_EXPORT2
uloc_getLocaleForLCID(uint32_t hostid, حرف *locale, int32_t locale_استعداد,
                UError_رمز *حالت_2)
{
    واپس uprv_convert_مقصد_Posix(hostid, locale, locale_استعداد, حالت_2);
}

/* ### Default locale **************************************************/

U_CAPI ثابت حرف*  U_EXPORT2
uloc_getDefault()
{
    واپس locale_get_default();
}

U_CAPI خالی  U_EXPORT2
uloc_سیٹ_Default(ثابت حرف*   newDefaultLocale,
             UError_رمز* err)
{
    اگر (U_FAILURE(*err))
        واپس;
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
    واپس LANGUAGES;
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
    واپس COUNTRIES;
}

U_CAPI ثابت حرف* U_EXPORT2
uloc_مقصد_Unicode_Locale_کلید(ثابت حرف* keyword U_LIFETIME_BOUND)
{
    اگر (keyword == خالی_اشاریہ || *keyword == '\0') { واپس خالی_اشاریہ; }
    std::optional<std::string_view> نتیجہ = ulocimp_مقصد_Bcp_کلید_With_Fallback(keyword);
    واپس نتیجہ.has_value() ? نتیجہ->ڈیٹا() : خالی_اشاریہ;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_مقصد_Bcp_کلید_With_Fallback(std::string_view keyword U_LIFETIME_BOUND)
{
    std::optional<std::string_view> bcp_کلید = ulocimp_مقصد_Bcp_کلید(keyword);
    اگر (!bcp_کلید.has_value() &&
        ultag_is_Unicode_Locale_کلید(keyword.ڈیٹا(), static_cast<int32_t>(keyword.size()))) {
        // unknown keyword, but syntax is fine..
        واپس keyword;
    }
    واپس bcp_کلید;
}

U_CAPI ثابت حرف* U_EXPORT2
uloc_مقصد_Unicode_Locale_Type(ثابت حرف* keyword, ثابت حرف* value U_LIFETIME_BOUND)
{
    اگر (keyword == خالی_اشاریہ || *keyword == '\0' ||
        value == خالی_اشاریہ || *value == '\0') { واپس خالی_اشاریہ; }
    std::optional<std::string_view> نتیجہ = ulocimp_مقصد_Bcp_Type_With_Fallback(keyword, value);
    واپس نتیجہ.has_value() ? نتیجہ->ڈیٹا() : خالی_اشاریہ;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_مقصد_Bcp_Type_With_Fallback(std::string_view keyword, std::string_view value U_LIFETIME_BOUND)
{
    std::optional<std::string_view> bcpType = ulocimp_مقصد_Bcp_Type(keyword, value);
    اگر (!bcpType.has_value() &&
        ultag_isUnicodeLocaleType(value.ڈیٹا(), static_cast<int32_t>(value.size()))) {
        // unknown keyword, but syntax is fine..
        واپس value;
    }
    واپس bcpType;
}

نامخلا {

منطقی
is_Well_Formed_Legacy_کلید(std::string_view کلید)
{
    واپس std::all_of(کلید.begin(), کلید.انتہا(), UPRV_ISALPHANUM);
}

منطقی
isWellFormedLegacyType(std::string_view legacyType)
{
    int32_t alpha_عدد_Len = 0;
    برائے (حرف c : legacyType) {
        اگر (c == '_' || c == '/' || c == '-') {
            اگر (alpha_عدد_Len == 0) {
                واپس جھوٹ;
            }
            alpha_عدد_Len = 0;
        } ورنہ اگر (UPRV_ISALPHANUM(c)) {
            alpha_عدد_Len++;
        } ورنہ {
            واپس جھوٹ;
        }
    }
    واپس alpha_عدد_Len != 0;
}

}  // namespace

U_CAPI ثابت حرف* U_EXPORT2
uloc_مقصد_Legacy_کلید(ثابت حرف* keyword U_LIFETIME_BOUND)
{
    اگر (keyword == خالی_اشاریہ || *keyword == '\0') { واپس خالی_اشاریہ; }
    std::optional<std::string_view> نتیجہ = ulocimp_مقصد_Legacy_کلید_With_Fallback(keyword);
    واپس نتیجہ.has_value() ? نتیجہ->ڈیٹا() : خالی_اشاریہ;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_مقصد_Legacy_کلید_With_Fallback(std::string_view keyword U_LIFETIME_BOUND)
{
    std::optional<std::string_view> legacy_کلید_2 = ulocimp_مقصد_Legacy_کلید(keyword);
    اگر (!legacy_کلید_2.has_value() && is_Well_Formed_Legacy_کلید(keyword)) {
        // Checks if the specified locale key is well-formed with the legacy locale syntax.
        //
        // Note:
        //  LDML/CLDR provides some definition of keyword syntax in
        //  * http://www.unicode.org/reports/tr35/#Unicode_locale_identifier and
        //  * http://www.unicode.org/reports/tr35/#Old_Locale_Extension_Syntax
        //  Keys can only consist of [0-9a-zA-Z].
        واپس keyword;
    }
    واپس legacy_کلید_2;
}

U_CAPI ثابت حرف* U_EXPORT2
uloc_مقصد_Legacy_Type(ثابت حرف* keyword, ثابت حرف* value U_LIFETIME_BOUND)
{
    اگر (keyword == خالی_اشاریہ || *keyword == '\0' ||
        value == خالی_اشاریہ || *value == '\0') { واپس خالی_اشاریہ; }
    std::optional<std::string_view> نتیجہ = ulocimp_مقصد_Legacy_Type_With_Fallback(keyword, value);
    واپس نتیجہ.has_value() ? نتیجہ->ڈیٹا() : خالی_اشاریہ;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_مقصد_Legacy_Type_With_Fallback(std::string_view keyword, std::string_view value U_LIFETIME_BOUND)
{
    std::optional<std::string_view> legacyType = ulocimp_مقصد_Legacy_Type(keyword, value);
    اگر (!legacyType.has_value() && isWellFormedLegacyType(value)) {
        // Checks if the specified locale type is well-formed with the legacy locale syntax.
        //
        // Note:
        //  LDML/CLDR provides some definition of keyword syntax in
        //  * http://www.unicode.org/reports/tr35/#Unicode_locale_identifier and
        //  * http://www.unicode.org/reports/tr35/#Old_Locale_Extension_Syntax
        //  Values (types) can only consist of [0-9a-zA-Z], plus for legacy values
        //  we allow [/_-+] in the middle (e.g. "Etc/GMT+1", "Asia/Tel_Aviv")
        واپس value;
    }
    واپس legacyType;
}

/*eof*/
