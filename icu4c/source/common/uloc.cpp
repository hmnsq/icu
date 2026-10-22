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

#ಅನ್ತರ್ಭಾವ <algorithm>
#ಅನ್ತರ್ಭಾವ <optional>
#ಅನ್ತರ್ಭಾವ <string_view>

#ಅನ್ತರ್ಭಾವ "unicode/bytestream.h"
#ಅನ್ತರ್ಭಾವ "unicode/errorcode.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/uloc.h"

#ಅನ್ತರ್ಭಾವ "bytesinkutil.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"
#ಅನ್ತರ್ಭಾವ "ulocimp.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "locmap.h"
#ಅನ್ತರ್ಭಾವ "uarrsort.h"
#ಅನ್ತರ್ಭಾವ "uenumimp.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"

U_NAMESPACE_USE

/* ### Declarations **************************************************/

/* Locale stuff from locid.cpp */
U_CFUNC ಶೂನ್ಯ locale_set_default(ಧ್ರುವ ವರ್ಣ *id);
U_CFUNC ಧ್ರುವ ವರ್ಣ *locale_get_default();

ನಾಮಾಕಾಶ {

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
constexpr ಧ್ರುವ ವರ್ಣ* LANGUAGES[] = {
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

constexpr ಧ್ರುವ ವರ್ಣ* DEPRECATED_LANGUAGES[]={
    "in", "iw", "ji", "jw", "mo", nullptr, nullptr
};
constexpr ಧ್ರುವ ವರ್ಣ* REPLACEMENT_LANGUAGES[]={
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
constexpr ಧ್ರುವ ವರ್ಣ* LANGUAGES_3[] = {
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
constexpr ಧ್ರುವ ವರ್ಣ* COUNTRIES[] = {
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

constexpr ಧ್ರುವ ವರ್ಣ* DEPRECATED_COUNTRIES[] = {
    "AN", "BU", "CS", "DD", "DY", "FX", "HV", "NH", "RH", "SU", "TP", "UK", "VD", "YD", "YU", "ZR", nullptr, nullptr /* deprecated country list */
};
constexpr ಧ್ರುವ ವರ್ಣ* REPLACEMENT_COUNTRIES[] = {
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
constexpr ಧ್ರುವ ವರ್ಣ* COUNTRIES_3[] = {
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

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ CanonicalizationMap {
    ಧ್ರುವ ವರ್ಣ *id;          /* input ID */
    ಧ್ರುವ ವರ್ಣ *canonicalID; /* canonicalized output ID */
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
    ತಾರ್ಕಿಕ reset = ಸತ್ಯ;

    ಪ್ರತಿ (i = 0; i < localeIDLength; i++) {
        ಯದಿ (localeID[i] != '_' && localeID[i] != '-') {
            ಯದಿ (reset) {
                tmpLength = 0;
                reset = ಅಸತ್ಯ;
            }
            tmpLength++;
        } ಅನ್ಯಥಾ {
            ಯದಿ (tmpLength != 0 && tmpLength < length) {
                length = tmpLength;
            }
            reset = ಸತ್ಯ;
        }
    }

    ಪ್ರತ್ಯಾಗಮ length;
}
/* Test if the locale id has BCP47 u extension and does not have '@' */
ಅನ್ತಃಸ್ಥ ತಾರ್ಕಿಕ _hasBCP47Extension(std::string_view id) {
    ಪ್ರತ್ಯಾಗಮ id.find('@') == std::string_view::npos && getShortestSubtagLength(id) == 1;
}

/* ### Keywords **************************************************/
ಅನ್ತಃಸ್ಥ ತಾರ್ಕಿಕ UPRV_ISDIGIT(ವರ್ಣ c) { ಪ್ರತ್ಯಾಗಮ c >= '0' && c <= '9'; }
ಅನ್ತಃಸ್ಥ ತಾರ್ಕಿಕ UPRV_ISALPHANUM(ವರ್ಣ c) { ಪ್ರತ್ಯಾಗಮ uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }
/* Punctuation/symbols allowed in legacy key values */
ಅನ್ತಃಸ್ಥ ತಾರ್ಕಿಕ UPRV_OK_ಮೌಲ್ಯ_PUNCTUATION(ವರ್ಣ c) { ಪ್ರತ್ಯಾಗಮ c == '_' || c == '-' || c == '+' || c == '/'; }

}  // namespace

#define ULOC_KEYWORD_BUFFER_LEN 25
#define ULOC_MAX_NO_KEYWORDS 25

U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
locale_getKeywordsStart(std::string_view localeID U_LIFETIME_BOUND) {
    ಯದಿ (size_t pos = localeID.find('@'); pos != std::string_view::npos) {
        ಪ್ರತ್ಯಾಗಮ localeID.data() + pos;
    }
#ಯದಿ (U_CHARSET_FAMILY == U_EBCDIC_FAMILY)
    ಅನ್ಯಥಾ {
        /* We do this because the @ sign is variant, and the @ sign used on one
        EBCDIC machine won't be compiled the same way on other EBCDIC based
        machines. */
        ಸ್ಥಿರ ಧ್ರುವ uint8_t ebcdicSigns[] = { 0x7C, 0x44, 0x66, 0x80, 0xAC, 0xAE, 0xAF, 0xB5, 0xEC, 0xEF, 0x00 };
        ಧ್ರುವ uint8_t *charToFind = ebcdicSigns;
        ಯಾವತ್(*charToFind) {
            ಯದಿ (size_t pos = localeID.find(*charToFind); pos != std::string_view::npos) {
                ಪ್ರತ್ಯಾಗಮ localeID.data() + pos;
            }
            charToFind++;
        }
    }
#endif
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

ನಾಮಾಕಾಶ {

/**
 * @param keywordName incoming name to be canonicalized
 * @param status return status (keyword too long)
 * @return the keyword name
 */
CharString locale_canonKeywordName(std::string_view keywordName, UErrorCode& status)
{
  ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ {}; }
  CharString result;

  ಪ್ರತಿ (ವರ್ಣ c : keywordName) {
    ಯದಿ (!UPRV_ISALPHANUM(c)) {
      status = U_ILLEGAL_ARGUMENT_ERROR; /* malformed keyword name */
      ಪ್ರತ್ಯಾಗಮ {};
    }
    result.append(uprv_tolower(c), status);
  }
  ಯದಿ (result.isEmpty()) {
    status = U_ILLEGAL_ARGUMENT_ERROR; /* empty keyword name */
    ಪ್ರತ್ಯಾಗಮ {};
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ವರ್ಣ keyword[ULOC_KEYWORD_BUFFER_LEN];
    int32_t keywordLen;
    ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ_Start;
    int32_t ಮೌಲ್ಯ_Len;
} KeywordStruct;

int32_t U_CALLCONV
compareKeywordStructs(ಧ್ರುವ ಶೂನ್ಯ * /*context*/, ಧ್ರುವ ಶೂನ್ಯ *left, ಧ್ರುವ ಶೂನ್ಯ *ಬಲ) {
    ಧ್ರುವ ವರ್ಣ* ಎಡ_String = static_cast<ಧ್ರುವ KeywordStruct*>(left)->keyword;
    ಧ್ರುವ ವರ್ಣ* ಬಲ_String = static_cast<ಧ್ರುವ KeywordStruct*>(ಬಲ)->keyword;
    ಪ್ರತ್ಯಾಗಮ uprv_strcmp(ಎಡ_String, ಬಲ_String);
}

}  // namespace

U_EXPORT CharString
ulocimp_getKeywords(std::string_view localeID,
                    ವರ್ಣ prev,
                    ತಾರ್ಕಿಕ ಮೌಲ್ಯಗಳು_Too,
                    UErrorCode& status)
{
    ಪ್ರತ್ಯಾಗಮ ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getKeywords(localeID,
                                prev,
                                sink,
                                ಮೌಲ್ಯಗಳು_Too,
                                status);
        },
        status);
}

U_EXPORT ಶೂನ್ಯ
ulocimp_getKeywords(std::string_view localeID,
                    ವರ್ಣ prev,
                    ByteSink& sink,
                    ತಾರ್ಕಿಕ ಮೌಲ್ಯಗಳು_Too,
                    UErrorCode& status)
{
    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }

    KeywordStruct keywordList[ULOC_MAX_NO_KEYWORDS];

    int32_t maxKeywords = ULOC_MAX_NO_KEYWORDS;
    int32_t ಅಂಕೆ_Keywords = 0;
    size_t equalSign = std::string_view::npos;
    size_t semicolon = std::string_view::npos;
    int32_t i = 0, j, n;

    ಯದಿ(prev == '@') { /* start of keyword definition */
        /* we will grab pairs, trim spaces, lowercase keywords, sort and return */
        ಕುರು {
            ತಾರ್ಕಿಕ duplicate = ಅಸತ್ಯ;
            /* skip leading spaces */
            ಯಾವತ್ (!localeID.empty() && localeID.front() == ' ') {
                localeID.remove_prefix(1);
            }
            ಯದಿ (localeID.empty()) { /* handle trailing "; " */
                ಭಙ್ಗ;
            }
            ಯದಿ(ಅಂಕೆ_Keywords == maxKeywords) {
                status = U_INTERNAL_PROGRAM_ERROR;
                ಪ್ರತ್ಯಾಗಮ;
            }
            equalSign = localeID.find('=');
            semicolon = localeID.find(';');
            /* lack of '=' [foo@currency] is illegal */
            /* ';' before '=' [foo@currency;collation=pinyin] is illegal */
            ಯದಿ (equalSign == std::string_view::npos ||
                (semicolon != std::string_view::npos && semicolon < equalSign)) {
                status = U_INVALID_FORMAT_ERROR;
                ಪ್ರತ್ಯಾಗಮ;
            }
            /* zero-length keyword is an error. */
            ಯದಿ (equalSign == 0) {
                status = U_INVALID_FORMAT_ERROR;
                ಪ್ರತ್ಯಾಗಮ;
            }
            /* need to normalize both keyword and keyword name */
            ಯದಿ (equalSign >= ULOC_KEYWORD_BUFFER_LEN) {
                /* keyword name too long for internal buffer */
                status = U_INTERNAL_PROGRAM_ERROR;
                ಪ್ರತ್ಯಾಗಮ;
            }
            ಪ್ರತಿ (i = 0, n = 0; static_cast<size_t>(i) < equalSign; ++i) {
                ಯದಿ (localeID[i] != ' ') {
                    keywordList[ಅಂಕೆ_Keywords].keyword[n++] = uprv_tolower(localeID[i]);
                }
            }

            keywordList[ಅಂಕೆ_Keywords].keyword[n] = 0;
            keywordList[ಅಂಕೆ_Keywords].keywordLen = n;
            /* now grab the value part. First we skip the '=' */
            equalSign++;
            /* then we leading spaces */
            ಯಾವತ್ (equalSign < localeID.length() && localeID[equalSign] == ' ') {
                equalSign++;
            }

            /* Premature end or zero-length value */
            ಯದಿ (equalSign == localeID.length() || equalSign == semicolon) {
                status = U_INVALID_FORMAT_ERROR;
                ಪ್ರತ್ಯಾಗಮ;
            }

            keywordList[ಅಂಕೆ_Keywords].ಮೌಲ್ಯ_Start = localeID.data() + equalSign;

            std::string_view value = localeID;
            ಯದಿ (semicolon != std::string_view::npos) {
                value.remove_suffix(value.length() - semicolon);
                localeID.remove_prefix(semicolon + 1);
            } ಅನ್ಯಥಾ {
                localeID = {};
            }
            value.remove_prefix(equalSign);
            ಯದಿ (size_t last = value.find_last_not_of(' '); last != std::string_view::npos) {
                value.remove_suffix(value.length() - last - 1);
            }
            keywordList[ಅಂಕೆ_Keywords].ಮೌಲ್ಯ_Len = static_cast<int32_t>(value.length());

            /* If this is a duplicate keyword, then ignore it */
            ಪ್ರತಿ (j=0; j<ಅಂಕೆ_Keywords; ++j) {
                ಯದಿ (uprv_strcmp(keywordList[j].keyword, keywordList[ಅಂಕೆ_Keywords].keyword) == 0) {
                    duplicate = ಸತ್ಯ;
                    ಭಙ್ಗ;
                }
            }
            ಯದಿ (!duplicate) {
                ++ಅಂಕೆ_Keywords;
            }
        } ಯಾವತ್ (!localeID.empty());

        /* now we have a list of keywords */
        /* we need to sort it */
        uprv_sortArray(keywordList, ಅಂಕೆ_Keywords, ಪರಿಮಾಣ(KeywordStruct), compareKeywordStructs, nullptr, ಅಸತ್ಯ, &status);

        /* Now construct the keyword part */
        ಪ್ರತಿ(i = 0; i < ಅಂಕೆ_Keywords; i++) {
            sink.Append(keywordList[i].keyword, keywordList[i].keywordLen);
            ಯದಿ(ಮೌಲ್ಯಗಳು_Too) {
                sink.Append("=", 1);
                sink.Append(keywordList[i].ಮೌಲ್ಯ_Start, keywordList[i].ಮೌಲ್ಯ_Len);
                ಯದಿ(i < ಅಂಕೆ_Keywords - 1) {
                    sink.Append(";", 1);
                }
            } ಅನ್ಯಥಾ {
                sink.Append("\0", 1);
            }
        }
    }
}

U_CAPI int32_t U_EXPORT2
uloc_get_Keyword_ಮೌಲ್ಯ(ಧ್ರುವ ವರ್ಣ* localeID,
                     ಧ್ರುವ ವರ್ಣ* keywordName,
                     ವರ್ಣ* buffer, int32_t bufferCapacity,
                     UErrorCode* status)
{
    ಯದಿ (U_FAILURE(*status)) { ಪ್ರತ್ಯಾಗಮ 0; }
    ಯದಿ (keywordName == nullptr || *keywordName == '\0') {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತ್ಯಾಗಮ ByteSinkUtil::viaByteSinkToTerminatedChars(
        buffer, bufferCapacity,
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_get_Keyword_ಮೌಲ್ಯ(localeID, keywordName, sink, status);
        },
        *status);
}

U_EXPORT CharString
ulocimp_get_Keyword_ಮೌಲ್ಯ(ಧ್ರುವ ವರ್ಣ* localeID,
                        std::string_view keywordName,
                        UErrorCode& status)
{
    ಪ್ರತ್ಯಾಗಮ ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_get_Keyword_ಮೌಲ್ಯ(localeID, keywordName, sink, status);
        },
        status);
}

U_EXPORT ಶೂನ್ಯ
ulocimp_get_Keyword_ಮೌಲ್ಯ(ಧ್ರುವ ವರ್ಣ* localeID,
                        std::string_view keywordName,
                        icu::ByteSink& sink,
                        UErrorCode& status)
{
    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }

    ಯದಿ (localeID == nullptr || keywordName.empty()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಧ್ರುವ ವರ್ಣ* start_ಹುಡುಕು_Here = nullptr;
    ಧ್ರುವ ವರ್ಣ* nextSeparator = nullptr;

    CharString tempBuffer;
    ಧ್ರುವ ವರ್ಣ* tmpLocaleID;

    CharString canonKeywordName = locale_canonKeywordName(keywordName, status);
    ಯದಿ (U_FAILURE(status)) {
      ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (localeID != nullptr && _hasBCP47Extension(localeID)) {
        tempBuffer = ulocimp_forLanguageTag(localeID, -1, nullptr, status);
        tmpLocaleID = U_SUCCESS(status) && !tempBuffer.isEmpty() ? tempBuffer.data() : localeID;
    } ಅನ್ಯಥಾ {
        tmpLocaleID=localeID;
    }

    start_ಹುಡುಕು_Here = locale_getKeywordsStart(tmpLocaleID);
    ಯದಿ(start_ಹುಡುಕು_Here == nullptr) {
        /* no keywords, return at once */
        ಪ್ರತ್ಯಾಗಮ;
    }

    /* find the first keyword */
    ಯಾವತ್(start_ಹುಡುಕು_Here) {
        ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ_ಮೌಲ್ಯ_Tail;

        start_ಹುಡುಕು_Here++; /* skip @ or ; */
        nextSeparator = uprv_strchr(start_ಹುಡುಕು_Here, '=');
        ಯದಿ(!nextSeparator) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* key must have =value */
            ಪ್ರತ್ಯಾಗಮ;
        }
        /* strip leading & trailing spaces (TC decided to tolerate these) */
        ಯಾವತ್(*start_ಹುಡುಕು_Here == ' ') {
            start_ಹುಡುಕು_Here++;
        }
        ಕೀಲಿ_ಮೌಲ್ಯ_Tail = nextSeparator;
        ಯಾವತ್ (ಕೀಲಿ_ಮೌಲ್ಯ_Tail > start_ಹುಡುಕು_Here && *(ಕೀಲಿ_ಮೌಲ್ಯ_Tail-1) == ' ') {
            ಕೀಲಿ_ಮೌಲ್ಯ_Tail--;
        }
        /* now keyValueTail points to first char after the keyName */
        /* copy & normalize keyName from locale */
        ಯದಿ (start_ಹುಡುಕು_Here == ಕೀಲಿ_ಮೌಲ್ಯ_Tail) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* empty keyword name in passed-in locale */
            ಪ್ರತ್ಯಾಗಮ;
        }
        CharString localeKeywordName;
        ಯಾವತ್ (start_ಹುಡುಕು_Here < ಕೀಲಿ_ಮೌಲ್ಯ_Tail) {
          ಯದಿ (!UPRV_ISALPHANUM(*start_ಹುಡುಕು_Here)) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* malformed keyword name */
            ಪ್ರತ್ಯಾಗಮ;
          }
          localeKeywordName.append(uprv_tolower(*start_ಹುಡುಕು_Here++), status);
        }
        ಯದಿ (U_FAILURE(status)) {
            ಪ್ರತ್ಯಾಗಮ;
        }

        start_ಹುಡುಕು_Here = uprv_strchr(nextSeparator, ';');

        ಯದಿ (canonKeywordName == localeKeywordName) {
             /* current entry matches the keyword. */
           nextSeparator++; /* skip '=' */
            /* First strip leading & trailing spaces (TC decided to tolerate these) */
            ಯಾವತ್(*nextSeparator == ' ') {
              nextSeparator++;
            }
            ಕೀಲಿ_ಮೌಲ್ಯ_Tail = (start_ಹುಡುಕು_Here)? start_ಹುಡುಕು_Here: nextSeparator + uprv_strlen(nextSeparator);
            ಯಾವತ್(ಕೀಲಿ_ಮೌಲ್ಯ_Tail > nextSeparator && *(ಕೀಲಿ_ಮೌಲ್ಯ_Tail-1) == ' ') {
              ಕೀಲಿ_ಮೌಲ್ಯ_Tail--;
            }
            /* Now copy the value, but check well-formedness */
            ಯದಿ (nextSeparator == ಕೀಲಿ_ಮೌಲ್ಯ_Tail) {
              status = U_ILLEGAL_ARGUMENT_ERROR; /* empty key value name in passed-in locale */
              ಪ್ರತ್ಯಾಗಮ;
            }
            ಯಾವತ್ (nextSeparator < ಕೀಲಿ_ಮೌಲ್ಯ_Tail) {
              ಯದಿ (!UPRV_ISALPHANUM(*nextSeparator) && !UPRV_OK_ಮೌಲ್ಯ_PUNCTUATION(*nextSeparator)) {
                status = U_ILLEGAL_ARGUMENT_ERROR; /* malformed key value */
                ಪ್ರತ್ಯಾಗಮ;
              }
              /* Should we lowercase value to return here? Tests expect as-is. */
              sink.Append(nextSeparator++, 1);
            }
            ಪ್ರತ್ಯಾಗಮ;
        }
    }
}

U_CAPI int32_t U_EXPORT2
uloc_set_Keyword_ಮೌಲ್ಯ(ಧ್ರುವ ವರ್ಣ* keywordName,
                     ಧ್ರುವ ವರ್ಣ* keyword_ಮೌಲ್ಯ,
                     ವರ್ಣ* buffer, int32_t bufferCapacity,
                     UErrorCode* status)
{
    ಯದಿ (U_FAILURE(*status)) { ಪ್ರತ್ಯಾಗಮ 0; }

    ಯದಿ (keywordName == nullptr || *keywordName == 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ (bufferCapacity <= 1) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    int32_t bufLen = (int32_t)uprv_strlen(buffer);
    ಯದಿ(bufferCapacity<bufLen) {
        /* The capacity is less than the length?! Is this NUL terminated? */
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ವರ್ಣ* keywords = const_cast<ವರ್ಣ*>(
        locale_getKeywordsStart({buffer, static_cast<std::string_view::size_type>(bufLen)}));
    int32_t baseLen = keywords == nullptr ? bufLen : keywords - buffer;
    // Remove -1 from the capacity so that this function can guarantee NUL termination.
    CheckedArrayByteSink sink(keywords == nullptr ? buffer + bufLen : keywords,
                              bufferCapacity - baseLen - 1);
    int32_t reslen = ulocimp_set_Keyword_ಮೌಲ್ಯ(
        keywords == nullptr ? std::string_view() : keywords,
        keywordName,
        keyword_ಮೌಲ್ಯ == nullptr ? std::string_view() : keyword_ಮೌಲ್ಯ,
        sink,
        *status);

    ಯದಿ (U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ *status == U_BUFFER_OVERFLOW_ERROR ? reslen + baseLen : 0;
    }

    // See the documentation for this function, it's guaranteed to never
    // overflow the buffer but instead abort with BUFFER_OVERFLOW_ERROR.
    // In this case, nothing has been written to the sink, so it cannot have Overflowed().
    U_ASSERT(!sink.Overflowed());
    U_ASSERT(reslen >= 0);
    ಪ್ರತ್ಯಾಗಮ u_terminateChars(buffer, bufferCapacity, reslen + baseLen, status);
}

U_EXPORT ಶೂನ್ಯ
ulocimp_set_Keyword_ಮೌಲ್ಯ(std::string_view keywordName,
                        std::string_view keyword_ಮೌಲ್ಯ,
                        CharString& localeID,
                        UErrorCode& status)
{
    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }
    std::string_view keywords;
    ಯದಿ (ಧ್ರುವ ವರ್ಣ* start = locale_getKeywordsStart(localeID.toStringPiece()); start != nullptr) {
        // This is safe because CharString::truncate() doesn't actually erase any
        // data, but simply sets the position for where new data will be written.
        int32_t size = start - localeID.data();
        keywords = localeID.toStringPiece();
        keywords.remove_prefix(size);
        localeID.truncate(size);
    }
    CharStringByteSink sink(&localeID);
    ulocimp_set_Keyword_ಮೌಲ್ಯ(keywords, keywordName, keyword_ಮೌಲ್ಯ, sink, status);
}

U_EXPORT int32_t
ulocimp_set_Keyword_ಮೌಲ್ಯ(std::string_view keywords,
                        std::string_view keywordName,
                        std::string_view keyword_ಮೌಲ್ಯ,
                        ByteSink& sink,
                        UErrorCode& status)
{
    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ 0; }

    /* TODO: sorting. removal. */
    int32_t needLen = 0;
    int32_t rc;
    CharString updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು;
    ತಾರ್ಕಿಕ handled_ಒಳಹರಿವು_ಕೀಲಿ_And_ಮೌಲ್ಯ = ಅಸತ್ಯ;
    ವರ್ಣ ಕೀಲಿ_ಮೌಲ್ಯ_Prefix = '@';

    ಯದಿ (status == U_STRING_NOT_TERMINATED_WARNING) {
        status = U_ZERO_ERROR;
    }
    ಯದಿ (keywordName.empty()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    CharString canonKeywordName = locale_canonKeywordName(keywordName, status);
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    CharString canon_Keyword_ಮೌಲ್ಯ;
    ಪ್ರತಿ (ವರ್ಣ c : keyword_ಮೌಲ್ಯ) {
        ಯದಿ (!UPRV_ISALPHANUM(c) && !UPRV_OK_ಮೌಲ್ಯ_PUNCTUATION(c)) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* malformed key value */
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        /* Should we force lowercase in value to set? */
        canon_Keyword_ಮೌಲ್ಯ.append(c, status);
    }
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ (keywords.size() <= 1) {
        ಯದಿ (canon_Keyword_ಮೌಲ್ಯ.isEmpty()) { /* no keywords = nothing to remove */
            U_ASSERT(status != U_STRING_NOT_TERMINATED_WARNING);
            ಪ್ರತ್ಯಾಗಮ 0;
        }

        needLen = 1 + canonKeywordName.length() + 1 + canon_Keyword_ಮೌಲ್ಯ.length();
        int32_t capacity = 0;
        ವರ್ಣ* buffer = sink.GetAppendBuffer(
                needLen, needLen, nullptr, needLen, &capacity);
        ಯದಿ (capacity < needLen || buffer == nullptr) {
            status = U_BUFFER_OVERFLOW_ERROR;
            ಪ್ರತ್ಯಾಗಮ needLen; /* no change */
        }
        ವರ್ಣ* it = buffer;

        *it++ = '@';
        uprv_memcpy(it, canonKeywordName.data(), canonKeywordName.length());
        it += canonKeywordName.length();
        *it++ = '=';
        uprv_memcpy(it, canon_Keyword_ಮೌಲ್ಯ.data(), canon_Keyword_ಮೌಲ್ಯ.length());
        sink.Append(buffer, needLen);
        U_ASSERT(status != U_STRING_NOT_TERMINATED_WARNING);
        ಪ್ರತ್ಯಾಗಮ needLen;
    } /* end shortcut - no @ */

    /* search for keyword */
    ಪ್ರತಿ (size_t keywordStart = 0; keywordStart != std::string_view::npos;) {
        keywordStart++; /* skip @ or ; */
        size_t nextEqualsign = keywords.find('=', keywordStart);
        ಯದಿ (nextEqualsign == std::string_view::npos) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* key must have =value */
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        /* strip leading & trailing spaces (TC decided to tolerate these) */
        ಯಾವತ್ (keywordStart < keywords.size() && keywords[keywordStart] == ' ') {
            keywordStart++;
        }
        size_t ಕೀಲಿ_ಮೌಲ್ಯ_Tail = nextEqualsign;
        ಯಾವತ್ (ಕೀಲಿ_ಮೌಲ್ಯ_Tail > keywordStart && keywords[ಕೀಲಿ_ಮೌಲ್ಯ_Tail - 1] == ' ') {
            ಕೀಲಿ_ಮೌಲ್ಯ_Tail--;
        }
        /* now keyValueTail points to first char after the keyName */
        /* copy & normalize keyName from locale */
        ಯದಿ (keywordStart == ಕೀಲಿ_ಮೌಲ್ಯ_Tail) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* empty keyword name in passed-in locale */
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        CharString localeKeywordName;
        ಯಾವತ್ (keywordStart < ಕೀಲಿ_ಮೌಲ್ಯ_Tail) {
            ಯದಿ (!UPRV_ISALPHANUM(keywords[keywordStart])) {
                status = U_ILLEGAL_ARGUMENT_ERROR; /* malformed keyword name */
                ಪ್ರತ್ಯಾಗಮ 0;
            }
            localeKeywordName.append(uprv_tolower(keywords[keywordStart++]), status);
        }
        ಯದಿ (U_FAILURE(status)) {
            ಪ್ರತ್ಯಾಗಮ 0;
        }

        size_t nextSeparator = keywords.find(';', nextEqualsign);

        /* start processing the value part */
        nextEqualsign++; /* skip '=' */
        /* First strip leading & trailing spaces (TC decided to tolerate these) */
        ಯಾವತ್ (nextEqualsign < keywords.size() && keywords[nextEqualsign] == ' ') {
            nextEqualsign++;
        }
        ಕೀಲಿ_ಮೌಲ್ಯ_Tail = nextSeparator == std::string_view::npos ? keywords.size() : nextSeparator;
        ಯಾವತ್ (ಕೀಲಿ_ಮೌಲ್ಯ_Tail > nextEqualsign && keywords[ಕೀಲಿ_ಮೌಲ್ಯ_Tail - 1] == ' ') {
            ಕೀಲಿ_ಮೌಲ್ಯ_Tail--;
        }
        ಯದಿ (nextEqualsign == ಕೀಲಿ_ಮೌಲ್ಯ_Tail) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* empty key value in passed-in locale */
            ಪ್ರತ್ಯಾಗಮ 0;
        }

        rc = uprv_strcmp(canonKeywordName.data(), localeKeywordName.data());
        ಯದಿ(rc == 0) {
            /* Current entry matches the input keyword. Update the entry */
            ಯದಿ (!canon_Keyword_ಮೌಲ್ಯ.isEmpty()) { /* updating a value */
                updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append(ಕೀಲಿ_ಮೌಲ್ಯ_Prefix, status);
                ಕೀಲಿ_ಮೌಲ್ಯ_Prefix = ';'; /* for any subsequent key-value pair */
                updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append(canonKeywordName, status);
                updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append('=', status);
                updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append(canon_Keyword_ಮೌಲ್ಯ, status);
            } /* else removing this entry, don't emit anything */
            handled_ಒಳಹರಿವು_ಕೀಲಿ_And_ಮೌಲ್ಯ = ಸತ್ಯ;
        } ಅನ್ಯಥಾ {
           /* input keyword sorts earlier than current entry, add before current entry */
            ಯದಿ (rc < 0 && !canon_Keyword_ಮೌಲ್ಯ.isEmpty() && !handled_ಒಳಹರಿವು_ಕೀಲಿ_And_ಮೌಲ್ಯ) {
                /* insert new entry at this location */
                updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append(ಕೀಲಿ_ಮೌಲ್ಯ_Prefix, status);
                ಕೀಲಿ_ಮೌಲ್ಯ_Prefix = ';'; /* for any subsequent key-value pair */
                updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append(canonKeywordName, status);
                updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append('=', status);
                updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append(canon_Keyword_ಮೌಲ್ಯ, status);
                handled_ಒಳಹರಿವು_ಕೀಲಿ_And_ಮೌಲ್ಯ = ಸತ್ಯ;
            }
            /* copy the current entry */
            updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append(ಕೀಲಿ_ಮೌಲ್ಯ_Prefix, status);
            ಕೀಲಿ_ಮೌಲ್ಯ_Prefix = ';'; /* for any subsequent key-value pair */
            updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append(localeKeywordName, status);
            updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append('=', status);
            updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append(keywords.data() + nextEqualsign,
                                        static_cast<int32_t>(ಕೀಲಿ_ಮೌಲ್ಯ_Tail - nextEqualsign), status);
        }
        ಯದಿ (nextSeparator == std::string_view::npos && !canon_Keyword_ಮೌಲ್ಯ.isEmpty() && !handled_ಒಳಹರಿವು_ಕೀಲಿ_And_ಮೌಲ್ಯ) {
            /* append new entry at the end, it sorts later than existing entries */
            updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append(ಕೀಲಿ_ಮೌಲ್ಯ_Prefix, status);
            /* skip keyValuePrefix update, no subsequent key-value pair */
            updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append(canonKeywordName, status);
            updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append('=', status);
            updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append(canon_Keyword_ಮೌಲ್ಯ, status);
            handled_ಒಳಹರಿವು_ಕೀಲಿ_And_ಮೌಲ್ಯ = ಸತ್ಯ;
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
    ಯದಿ (!handled_ಒಳಹರಿವು_ಕೀಲಿ_And_ಮೌಲ್ಯ || U_FAILURE(status)) {
        /* if input key/value specified removal of a keyword not present in locale, or
         * there was an error in CharString.append, leave original locale alone. */
        U_ASSERT(status != U_STRING_NOT_TERMINATED_WARNING);
        // The sink is expected to be a buffer which already contains the full
        // locale string, so when it isn't going to be modified there's no need
        // to actually write any data to it, as the data is already there. Only
        // the first character needs to be overwritten (changing '\0' to '@').
        needLen = static_cast<int32_t>(keywords.size());
        int32_t capacity = 0;
        ವರ್ಣ* buffer = sink.GetAppendBuffer(
                needLen, needLen, nullptr, needLen, &capacity);
        ಯದಿ (capacity < needLen || buffer == nullptr) {
            status = U_BUFFER_OVERFLOW_ERROR;
        } ಅನ್ಯಥಾ {
            *buffer = '@';
            sink.Append(buffer, needLen);
        }
        ಪ್ರತ್ಯಾಗಮ needLen;
    }

    needLen = updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.length();
    // Check to see can we fit the updatedKeysAndValues, if not, return
    // U_BUFFER_OVERFLOW_ERROR without copy updatedKeysAndValues into it.
    // We do this because this API function does not behave like most others:
    // It promises never to set a U_STRING_NOT_TERMINATED_WARNING.
    // When the contents fits but without the terminating NUL, in this case we need to not change
    // the buffer contents and return with a buffer overflow error.
    ಯದಿ (needLen > 0) {
        int32_t capacity = 0;
        ವರ್ಣ* buffer = sink.GetAppendBuffer(
                needLen, needLen, nullptr, needLen, &capacity);
        ಯದಿ (capacity < needLen || buffer == nullptr) {
            status = U_BUFFER_OVERFLOW_ERROR;
            ಪ್ರತ್ಯಾಗಮ needLen;
        }
        uprv_memcpy(buffer, updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.data(), needLen);
        sink.Append(buffer, needLen);
    }
    U_ASSERT(status != U_STRING_NOT_TERMINATED_WARNING);
    ಪ್ರತ್ಯಾಗಮ needLen;
}

/* ### ID parsing implementation **************************************************/

ನಾಮಾಕಾಶ {

ಅನ್ತಃಸ್ಥ ತಾರ್ಕಿಕ _isPrefixLetter(ವರ್ಣ a) { ಪ್ರತ್ಯಾಗಮ a == 'x' || a == 'X' || a == 'i' || a == 'I'; }

/*returns true if one of the special prefixes is here (s=string)
  'x-' or 'i-' */
ಅನ್ತಃಸ್ಥ ತಾರ್ಕಿಕ _isIDPrefix(std::string_view s) {
    ಪ್ರತ್ಯಾಗಮ s.size() >= 2 && _isPrefixLetter(s[0]) && _isIDSeparator(s[1]);
}

/* Dot terminates it because of POSIX form  where dot precedes the codepage
 * except for variant
 */
ಅನ್ತಃಸ್ಥ ತಾರ್ಕಿಕ _isTerminator(ವರ್ಣ a) { ಪ್ರತ್ಯಾಗಮ a == '.' || a == '@'; }

ಅನ್ತಃಸ್ಥ ತಾರ್ಕಿಕ _isBCP47Extension(std::string_view p) {
    ಪ್ರತ್ಯಾಗಮ p.size() >= 3 &&
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
std::optional<int16_t> _findIndex(ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ* list, ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ)
{
    ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ* anchor = list;
    int32_t pass = 0;

    /* Make two passes through two nullptr-terminated arrays at 'list' */
    ಯಾವತ್ (pass++ < 2) {
        ಯಾವತ್ (*list) {
            ಯದಿ (uprv_strcmp(ಕೀಲಿ, *list) == 0) {
                ಪ್ರತ್ಯಾಗಮ static_cast<int16_t>(list - anchor);
            }
            list++;
        }
        ++list;     /* skip final nullptr *CWB*/
    }
    ಪ್ರತ್ಯಾಗಮ std::nullopt;
}

}  // namespace

U_CFUNC ಧ್ರುವ ವರ್ಣ*
uloc_getCurrentCountryID(ಧ್ರುವ ವರ್ಣ* oldID U_LIFETIME_BOUND) {
    std::optional<int16_t> offset = _findIndex(DEPRECATED_COUNTRIES, oldID);
    ಪ್ರತ್ಯಾಗಮ offset.has_value() ? REPLACEMENT_COUNTRIES[*offset] : oldID;
}
U_CFUNC ಧ್ರುವ ವರ್ಣ*
uloc_getCurrentLanguageID(ಧ್ರುವ ವರ್ಣ* oldID U_LIFETIME_BOUND) {
    std::optional<int16_t> offset = _findIndex(DEPRECATED_LANGUAGES, oldID);
    ಪ್ರತ್ಯಾಗಮ offset.has_value() ? REPLACEMENT_LANGUAGES[*offset] : oldID;
}

ನಾಮಾಕಾಶ {

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
    ಯದಿ (localeID.size() == 4 && uprv_strnicmp(localeID.data(), "root", 4) == 0) {
        skip = 4;
        localeID.remove_prefix(skip);
    } ಅನ್ಯಥಾ ಯದಿ (localeID.size() >= 3 && uprv_strnicmp(localeID.data(), "und", 3) == 0 &&
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
    ಯಾವತ್ (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len])) {
        ಯದಿ (len == MAXLEN) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        len++;
    }

    ಯದಿ (sink == nullptr || len == 0) { ಪ್ರತ್ಯಾಗಮ skip + len; }

    int32_t minCapacity = uprv_max(static_cast<int32_t>(len), 4);  // Minimum 3 letters plus NUL.
    ವರ್ಣ scratch[MAXLEN];
    int32_t capacity = 0;
    ವರ್ಣ* buffer = sink->GetAppendBuffer(
            minCapacity, minCapacity, scratch, UPRV_LENGTHOF(scratch), &capacity);

    ಪ್ರತಿ (size_t i = 0; i < len; ++i) {
        buffer[i] = uprv_tolower(localeID[i]);
    }
    ಯದಿ (localeID.size() >= 2 && _isIDSeparator(localeID[1])) {
        buffer[1] = '-';
    }

    ಯದಿ (len == 3) {
        /* convert 3 character code to 2 character code if possible *CWB*/
        U_ASSERT(capacity >= 4);
        buffer[3] = '\0';
        std::optional<int16_t> offset = _findIndex(LANGUAGES_3, buffer);
        ಯದಿ (offset.has_value()) {
            ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ alias = LANGUAGES[*offset];
            sink->Append(alias, static_cast<int32_t>(uprv_strlen(alias)));
            ಪ್ರತ್ಯಾಗಮ skip + len;
        }
    }

    sink->Append(buffer, static_cast<int32_t>(len));
    ಪ್ರತ್ಯಾಗಮ skip + len;
}

size_t _getScript(std::string_view localeID, ByteSink* sink) {
    constexpr int32_t LENGTH = 4;

    size_t len = 0;
    ಯಾವತ್ (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len]) &&
            uprv_isASCIILetter(localeID[len])) {
        ಯದಿ (len == LENGTH) { ಪ್ರತ್ಯಾಗಮ 0; }
        len++;
    }
    ಯದಿ (len != LENGTH) { ಪ್ರತ್ಯಾಗಮ 0; }

    ಯದಿ (sink == nullptr) { ಪ್ರತ್ಯಾಗಮ len; }

    ವರ್ಣ scratch[LENGTH];
    int32_t capacity = 0;
    ವರ್ಣ* buffer = sink->GetAppendBuffer(
            LENGTH, LENGTH, scratch, UPRV_LENGTHOF(scratch), &capacity);

    buffer[0] = uprv_toupper(localeID[0]);
    ಪ್ರತಿ (int32_t i = 1; i < LENGTH; ++i) {
        buffer[i] = uprv_tolower(localeID[i]);
    }

    sink->Append(buffer, LENGTH);
    ಪ್ರತ್ಯಾಗಮ len;
}

size_t _getRegion(std::string_view localeID, ByteSink* sink) {
    constexpr int32_t MINLEN = 2;
    constexpr int32_t MAXLEN = ULOC_COUNTRY_CAPACITY - 1;  // Minus NUL.

    size_t len = 0;
    ಯಾವತ್ (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len])) {
        ಯದಿ (len == MAXLEN) { ಪ್ರತ್ಯಾಗಮ 0; }
        len++;
    }
    ಯದಿ (len < MINLEN) { ಪ್ರತ್ಯಾಗಮ 0; }

    ಯದಿ (sink == nullptr) { ಪ್ರತ್ಯಾಗಮ len; }

    ವರ್ಣ scratch[ULOC_COUNTRY_CAPACITY];
    int32_t capacity = 0;
    ವರ್ಣ* buffer = sink->GetAppendBuffer(
            ULOC_COUNTRY_CAPACITY,
            ULOC_COUNTRY_CAPACITY,
            scratch,
            UPRV_LENGTHOF(scratch),
            &capacity);

    ಪ್ರತಿ (size_t i = 0; i < len; ++i) {
        buffer[i] = uprv_toupper(localeID[i]);
    }

    ಯದಿ (len == 3) {
        /* convert 3 character code to 2 character code if possible *CWB*/
        U_ASSERT(capacity >= 4);
        buffer[3] = '\0';
        std::optional<int16_t> offset = _findIndex(COUNTRIES_3, buffer);
        ಯದಿ (offset.has_value()) {
            ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ alias = COUNTRIES[*offset];
            sink->Append(alias, static_cast<int32_t>(uprv_strlen(alias)));
            ಪ್ರತ್ಯಾಗಮ len;
        }
    }

    sink->Append(buffer, static_cast<int32_t>(len));
    ಪ್ರತ್ಯಾಗಮ len;
}

/**
 * @param needSeparator if true, then add leading '_' if any variants
 * are added to 'variant'
 */
size_t
_getVariant(std::string_view localeID,
            ವರ್ಣ prev,
            ByteSink* sink,
            ತಾರ್ಕಿಕ needSeparator,
            UErrorCode& status) {
    ಯದಿ (U_FAILURE(status) || localeID.empty()) ಪ್ರತ್ಯಾಗಮ 0;

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
    ಯದಿ (_isIDSeparator(prev)) {
        /* get a variant string after a '-' or '_' */
        ಪ್ರತಿ (std::string_view sub = localeID;;) {
            size_t next = sub.find_first_of(".@_-");
            // For historical reasons, a trailing separator is included in the variant.
            ತಾರ್ಕಿಕ finished = next == std::string_view::npos || next + 1 == sub.length();
            size_t limit = finished ? sub.length() : next;
            index += limit;
            ಯದಿ (index > MAX_VARIANTS_LENGTH) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
                ಪ್ರತ್ಯಾಗಮ 0;
            }

            ಯದಿ (sink != nullptr) {
                ಯದಿ (needSeparator) {
                    sink->Append("_", 1);
                } ಅನ್ಯಥಾ {
                    needSeparator = ಸತ್ಯ;
                }

                int32_t length = static_cast<int32_t>(limit);
                int32_t minCapacity = uprv_min(length, MAX_VARIANTS_LENGTH);
                ವರ್ಣ scratch[MAX_VARIANTS_LENGTH];
                int32_t capacity = 0;
                ವರ್ಣ* buffer = sink->GetAppendBuffer(
                        minCapacity, minCapacity, scratch, UPRV_LENGTHOF(scratch), &capacity);

                ಪ್ರತಿ (size_t i = 0; i < limit; ++i) {
                    buffer[i] = uprv_toupper(sub[i]);
                }
                sink->Append(buffer, length);
            }

            ಯದಿ (finished) { ಪ್ರತ್ಯಾಗಮ index; }
            sub.remove_prefix(next);
            ಯದಿ (_isTerminator(sub.front()) || _isBCP47Extension(sub)) { ಪ್ರತ್ಯಾಗಮ index; }
            sub.remove_prefix(1);
            index++;
        }
    }

    size_t skip = 0;
    /* if there is no variant tag after a '-' or '_' then look for '@' */
    ಯದಿ (prev == '@') {
        /* keep localeID */
    } ಅನ್ಯಥಾ ಯದಿ (ಧ್ರುವ ವರ್ಣ* p = locale_getKeywordsStart(localeID); p != nullptr) {
        skip = 1 + p - localeID.data(); /* point after the '@' */
        localeID.remove_prefix(skip);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತಿ (; index < localeID.size() && !_isTerminator(localeID[index]); index++) {
        ಯದಿ (index >= MAX_VARIANTS_LENGTH) { // same as length > MAX_VARIANTS_LENGTH
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        ಯದಿ (needSeparator) {
            ಯದಿ (sink != nullptr) {
                sink->Append("_", 1);
            }
            needSeparator = ಅಸತ್ಯ;
        }
        ಯದಿ (sink != nullptr) {
            ವರ್ಣ c = uprv_toupper(localeID[index]);
            ಯದಿ (c == '-' || c == ',') c = '_';
            sink->Append(&c, 1);
        }
    }
    ಪ್ರತ್ಯಾಗಮ skip + index;
}

}  // namespace

U_EXPORT CharString
ulocimp_getLanguage(std::string_view localeID, UErrorCode& status) {
    ಪ್ರತ್ಯಾಗಮ ByteSinkUtil::viaByteSinkToCharString(
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
    ಪ್ರತ್ಯಾಗಮ ByteSinkUtil::viaByteSinkToCharString(
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
    ಪ್ರತ್ಯಾಗಮ ByteSinkUtil::viaByteSinkToCharString(
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
    ಪ್ರತ್ಯಾಗಮ ByteSinkUtil::viaByteSinkToCharString(
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

U_EXPORT ಶೂನ್ಯ
ulocimp_getSubtags(
        std::string_view localeID,
        CharString* language,
        CharString* script,
        CharString* region,
        CharString* variant,
        ಧ್ರುವ ವರ್ಣ** pEnd,
        UErrorCode& status) {
    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }

    std::optional<CharStringByteSink> languageSink;
    std::optional<CharStringByteSink> scriptSink;
    std::optional<CharStringByteSink> regionSink;
    std::optional<CharStringByteSink> variantSink;

    ಯದಿ (language != nullptr) { languageSink.emplace(language); }
    ಯದಿ (script != nullptr) { scriptSink.emplace(script); }
    ಯದಿ (region != nullptr) { regionSink.emplace(region); }
    ಯದಿ (variant != nullptr) { variantSink.emplace(variant); }

    ulocimp_getSubtags(
            localeID,
            languageSink.has_value() ? &*languageSink : nullptr,
            scriptSink.has_value() ? &*scriptSink : nullptr,
            regionSink.has_value() ? &*regionSink : nullptr,
            variantSink.has_value() ? &*variantSink : nullptr,
            pEnd,
            status);
}

U_EXPORT ಶೂನ್ಯ
ulocimp_getSubtags(
        std::string_view localeID,
        ByteSink* language,
        ByteSink* script,
        ByteSink* region,
        ByteSink* variant,
        ಧ್ರುವ ವರ್ಣ** pEnd,
        UErrorCode& status) {
    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }

    ಯದಿ (pEnd != nullptr) {
        *pEnd = localeID.data();
    } ಅನ್ಯಥಾ ಯದಿ (language == nullptr &&
               script == nullptr &&
               region == nullptr &&
               variant == nullptr) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (localeID.empty()) { ಪ್ರತ್ಯಾಗಮ; }

    ತಾರ್ಕಿಕ hasRegion = ಅಸತ್ಯ;

    {
        size_t len = _getLanguage(localeID, language, status);
        ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }
        ಯದಿ (len > 0) {
            localeID.remove_prefix(len);
        }
    }

    ಯದಿ (pEnd != nullptr) {
        *pEnd = localeID.data();
    } ಅನ್ಯಥಾ ಯದಿ (script == nullptr &&
               region == nullptr &&
               variant == nullptr) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (localeID.empty()) { ಪ್ರತ್ಯಾಗಮ; }

    ಯದಿ (_isIDSeparator(localeID.front())) {
        std::string_view sub = localeID;
        sub.remove_prefix(1);
        size_t len = _getScript(sub, script);
        ಯದಿ (len > 0) {
            localeID.remove_prefix(len + 1);
            ಯದಿ (pEnd != nullptr) { *pEnd = localeID.data(); }
        }
    }

    ಯದಿ ((region == nullptr && variant == nullptr && pEnd == nullptr) || localeID.empty()) { ಪ್ರತ್ಯಾಗಮ; }

    ಯದಿ (_isIDSeparator(localeID.front())) {
        std::string_view sub = localeID;
        sub.remove_prefix(1);
        size_t len = _getRegion(sub, region);
        ಯದಿ (len > 0) {
            hasRegion = ಸತ್ಯ;
            localeID.remove_prefix(len + 1);
            ಯದಿ (pEnd != nullptr) { *pEnd = localeID.data(); }
        }
    }

    ಯದಿ ((variant == nullptr && pEnd == nullptr) || localeID.empty()) { ಪ್ರತ್ಯಾಗಮ; }

    ತಾರ್ಕಿಕ hasVariant = ಅಸತ್ಯ;

    ಯದಿ (_isIDSeparator(localeID.front()) && !_isBCP47Extension(localeID)) {
        std::string_view sub = localeID;
        /* If there was no country ID, skip a possible extra IDSeparator */
        size_t skip = !hasRegion && localeID.size() > 1 && _isIDSeparator(localeID[1]) ? 2 : 1;
        sub.remove_prefix(skip);
        size_t len = _getVariant(sub, localeID[0], variant, ಅಸತ್ಯ, status);
        ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }
        ಯದಿ (len > 0) {
            hasVariant = ಸತ್ಯ;
            localeID.remove_prefix(skip + len);
            ಯದಿ (pEnd != nullptr) { *pEnd = localeID.data(); }
        }
    }

    ಯದಿ ((variant == nullptr && pEnd == nullptr) || localeID.empty()) { ಪ್ರತ್ಯಾಗಮ; }

    ಯದಿ (_isBCP47Extension(localeID)) {
        localeID.remove_prefix(2);
        constexpr ವರ್ಣ vaposix[] = "-va-posix";
        constexpr size_t length = ಪರಿಮಾಣ vaposix - 1;
        ಪ್ರತಿ (size_t next;; localeID.remove_prefix(next)) {
            next = localeID.find('-', 1);
            ಯದಿ (next == std::string_view::npos) { ಭಙ್ಗ; }
            next = localeID.find('-', next + 1);
            ತಾರ್ಕಿಕ finished = next == std::string_view::npos;
            std::string_view sub = localeID;
            ಯದಿ (!finished) { sub.remove_suffix(sub.length() - next); }

            ಯದಿ (sub.length() == length && uprv_strnicmp(sub.data(), vaposix, length) == 0) {
                ಯದಿ (variant != nullptr) {
                    ಯದಿ (hasVariant) { variant->Append("_", 1); }
                    constexpr ವರ್ಣ posix[] = "POSIX";
                    variant->Append(posix, ಪರಿಮಾಣ posix - 1);
                }
                ಯದಿ (pEnd != nullptr) { *pEnd = localeID.data() + length; }
            }

            ಯದಿ (finished) { ಭಙ್ಗ; }
        }
    }
}

/* Keyword enumeration */

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ UKeywordsContext {
    ವರ್ಣ* keywords;
    ವರ್ಣ* current;
} UKeywordsContext;

U_CDECL_BEGIN

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
uloc_kw_closeKeywords(UEnumeration *enumerator) {
    uprv_free(((UKeywordsContext *)enumerator->context)->keywords);
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

ಸ್ಥಿರ int32_t U_CALLCONV
uloc_kw_ಎಣಿಕೆ_Keywords(UEnumeration *en, UErrorCode * /*status*/) {
    ವರ್ಣ *kw = ((UKeywordsContext *)en->context)->keywords;
    int32_t result = 0;
    ಯಾವತ್(*kw) {
        result++;
        kw += uprv_strlen(kw)+1;
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * U_CALLCONV
uloc_kw_nextKeyword(UEnumeration* en,
                    int32_t* resultLength,
                    UErrorCode* /*status*/) {
    ಧ್ರುವ ವರ್ಣ* result = ((UKeywordsContext *)en->context)->current;
    int32_t len = 0;
    ಯದಿ(*result) {
        len = (int32_t)uprv_strlen(((UKeywordsContext *)en->context)->current);
        ((UKeywordsContext *)en->context)->current += len+1;
    } ಅನ್ಯಥಾ {
        result = nullptr;
    }
    ಯದಿ (resultLength) {
        *resultLength = len;
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
uloc_kw_resetKeywords(UEnumeration* en,
                      UErrorCode* /*status*/) {
    ((UKeywordsContext *)en->context)->current = ((UKeywordsContext *)en->context)->keywords;
}

U_CDECL_END


ಸ್ಥಿರ ಧ್ರುವ UEnumeration gKeywordsEnum = {
    nullptr,
    nullptr,
    uloc_kw_closeKeywords,
    uloc_kw_ಎಣಿಕೆ_Keywords,
    uenum_unextDefault,
    uloc_kw_nextKeyword,
    uloc_kw_resetKeywords
};

U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywordList(ಧ್ರುವ ವರ್ಣ *keywordList, int32_t keywordListSize, UErrorCode* status)
{
    ಯದಿ (U_FAILURE(*status)) { ಪ್ರತ್ಯಾಗಮ nullptr; }

    LocalMemory<UKeywordsContext> myContext;
    LocalMemory<UEnumeration> result;

    myContext.adoptInstead(static_cast<UKeywordsContext *>(uprv_malloc(ಪರಿಮಾಣ(UKeywordsContext))));
    result.adoptInstead(static_cast<UEnumeration *>(uprv_malloc(ಪರಿಮಾಣ(UEnumeration))));
    ಯದಿ (myContext.isNull() || result.isNull()) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    uprv_memcpy(result.getAlias(), &gKeywordsEnum, ಪರಿಮಾಣ(UEnumeration));
    myContext->keywords = static_cast<ವರ್ಣ *>(uprv_malloc(keywordListSize+1));
    ಯದಿ (myContext->keywords == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    uprv_memcpy(myContext->keywords, keywordList, keywordListSize);
    myContext->keywords[keywordListSize] = 0;
    myContext->current = myContext->keywords;
    result->context = myContext.orphan();
    ಪ್ರತ್ಯಾಗಮ result.orphan();
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywords(ಧ್ರುವ ವರ್ಣ* localeID,
                        UErrorCode* status)
{
    ಯದಿ(status==nullptr || U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    CharString tempBuffer;
    ಧ್ರುವ ವರ್ಣ* tmpLocaleID;

    ಯದಿ (localeID != nullptr && _hasBCP47Extension(localeID)) {
        tempBuffer = ulocimp_forLanguageTag(localeID, -1, nullptr, *status);
        tmpLocaleID = U_SUCCESS(*status) && !tempBuffer.isEmpty() ? tempBuffer.data() : localeID;
    } ಅನ್ಯಥಾ {
        ಯದಿ (localeID==nullptr) {
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
    ಯದಿ (U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    /* keywords are located after '@' */
    ಯದಿ((tmpLocaleID = locale_getKeywordsStart(tmpLocaleID)) != nullptr) {
        CharString keywords = ulocimp_getKeywords(tmpLocaleID + 1, '@', ಅಸತ್ಯ, *status);
        ಯದಿ (U_FAILURE(*status)) {
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        ಪ್ರತ್ಯಾಗಮ uloc_openKeywordList(keywords.data(), keywords.length(), status);
    }
    ಪ್ರತ್ಯಾಗಮ nullptr;
}


/* bit-flags for 'options' parameter of _canonicalize */
#define _ULOC_STRIP_KEYWORDS 0x2
#define _ULOC_CANONICALIZE   0x1

ನಾಮಾಕಾಶ {

ಅನ್ತಃಸ್ಥ ತಾರ್ಕಿಕ OPTION_SET(uint32_t options, uint32_t mask) { ಪ್ರತ್ಯಾಗಮ (options & mask) != 0; }

constexpr ವರ್ಣ i_default[] = {'i', '-', 'd', 'e', 'f', 'a', 'u', 'l', 't'};
constexpr int32_t I_DEFAULT_LENGTH = UPRV_LENGTHOF(i_default);

/**
 * Canonicalize the given localeID, to level 1 or to level 2,
 * depending on the options.  To specify level 1, pass in options=0.
 * To specify level 2, pass in options=_ULOC_CANONICALIZE.
 *
 * This is the code underlying uloc_getName and uloc_canonicalize.
 */
ಶೂನ್ಯ
_canonicalize(std::string_view localeID,
              ByteSink& sink,
              uint32_t options,
              UErrorCode& err) {
    ಯದಿ (U_FAILURE(err)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    int32_t j, field_ಎಣಿಕೆ=0;
    CharString tempBuffer;  // if localeID has a BCP47 extension, tmpLocaleID points to this
    CharString localeIDWithHyphens;  // if localeID has a BPC47 extension and have _, tmpLocaleID points to this
    std::string_view origLocaleID;
    std::string_view tmpLocaleID;
    size_t keywordAssign = std::string_view::npos;
    size_t separatorIndicator = std::string_view::npos;

    ಯದಿ (_hasBCP47Extension(localeID)) {
        std::string_view localeIDPtr = localeID;

        // convert all underbars to hyphens, unless the "BCP47 extension" comes at the beginning of the string
        ಯದಿ (localeID.size() >= 2 && localeID.find('_') != std::string_view::npos && localeID[1] != '-' && localeID[1] != '_') {
            localeIDWithHyphens.append(localeID, err);
            ಯದಿ (U_SUCCESS(err)) {
                ಪ್ರತಿ (ವರ್ಣ* p = localeIDWithHyphens.data(); *p != '\0'; ++p) {
                    ಯದಿ (*p == '_') {
                        *p = '-';
                    }
                }
                localeIDPtr = localeIDWithHyphens.toStringPiece();
            }
        }

        tempBuffer = ulocimp_forLanguageTag(localeIDPtr.data(), static_cast<int32_t>(localeIDPtr.size()), nullptr, err);
        tmpLocaleID = U_SUCCESS(err) && !tempBuffer.isEmpty() ? static_cast<std::string_view>(tempBuffer.toStringPiece()) : localeIDPtr;
    } ಅನ್ಯಥಾ {
        tmpLocaleID=localeID;
    }

    origLocaleID=tmpLocaleID;

    /* get all pieces, one after another, and separate with '_' */
    CharString tag;
    CharString script;
    CharString country;
    CharString variant;
    ಧ್ರುವ ವರ್ಣ* end = nullptr;
    ulocimp_getSubtags(
            tmpLocaleID,
            &tag,
            &script,
            &country,
            &variant,
            &end,
            err);
    ಯದಿ (U_FAILURE(err)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    U_ASSERT(end != nullptr);
    ಯದಿ (end > tmpLocaleID.data()) {
        tmpLocaleID.remove_prefix(end - tmpLocaleID.data());
    }

    ಯದಿ (tag.length() == I_DEFAULT_LENGTH && origLocaleID.length() >= I_DEFAULT_LENGTH &&
            uprv_strncmp(origLocaleID.data(), i_default, I_DEFAULT_LENGTH) == 0) {
        tag.clear();
        tag.append(uloc_getDefault(), err);
    } ಅನ್ಯಥಾ {
        ಯದಿ (!script.isEmpty()) {
            ++field_ಎಣಿಕೆ;
            tag.append('_', err);
            tag.append(script, err);
        }
        ಯದಿ (!country.isEmpty()) {
            ++field_ಎಣಿಕೆ;
            tag.append('_', err);
            tag.append(country, err);
        }
        ಯದಿ (!variant.isEmpty()) {
            ++field_ಎಣಿಕೆ;
            ಯದಿ (country.isEmpty()) {
                tag.append('_', err);
            }
            tag.append('_', err);
            tag.append(variant, err);
        }
    }

    /* Copy POSIX-style charset specifier, if any [mr.utf8] */
    ಯದಿ (!OPTION_SET(options, _ULOC_CANONICALIZE) && !tmpLocaleID.empty() && tmpLocaleID.front() == '.') {
        tag.append('.', err);
        tmpLocaleID.remove_prefix(1);
        size_t length;
        ಯದಿ (size_t atPos = tmpLocaleID.find('@'); atPos != std::string_view::npos) {
            length = atPos;
        } ಅನ್ಯಥಾ {
            length = tmpLocaleID.length();
        }
        // The longest charset name we found in IANA charset registry
        // https://www.iana.org/assignments/character-sets/ is
        // "Extended_UNIX_Code_Packed_Format_for_Japanese" in length 45.
        // we therefore restrict the length here to be 64 which is a power of 2
        // number that is longer than 45.
        constexpr size_t kMaxCharsetLength = 64;
        ಯದಿ (length > kMaxCharsetLength) {
           err = U_ILLEGAL_ARGUMENT_ERROR; /* malformed keyword name */
           ಪ್ರತ್ಯಾಗಮ;
        }
        ಯದಿ (length > 0) {
            tag.append(tmpLocaleID.data(), static_cast<int32_t>(length), err);
            tmpLocaleID.remove_prefix(length);
        }
    }

    /* Scan ahead to next '@' and determine if it is followed by '=' and/or ';'
       After this, tmpLocaleID either starts at '@' or is empty. */
    ಯದಿ (ಧ್ರುವ ವರ್ಣ* start = locale_getKeywordsStart(tmpLocaleID); start != nullptr) {
        ಯದಿ (start > tmpLocaleID.data()) {
            tmpLocaleID.remove_prefix(start - tmpLocaleID.data());
        }
        keywordAssign = tmpLocaleID.find('=');
        separatorIndicator = tmpLocaleID.find(';');
    } ಅನ್ಯಥಾ {
        tmpLocaleID = {};
    }

    /* Copy POSIX-style variant, if any [mr@FOO] */
    ಯದಿ (!OPTION_SET(options, _ULOC_CANONICALIZE) &&
        !tmpLocaleID.empty() && keywordAssign == std::string_view::npos) {
        tag.append(tmpLocaleID, err);
        tmpLocaleID = {};
    }

    ಯದಿ (OPTION_SET(options, _ULOC_CANONICALIZE)) {
        /* Handle @FOO variant if @ is present and not followed by = */
        ಯದಿ (!tmpLocaleID.empty() && keywordAssign == std::string_view::npos) {
            /* Add missing '_' if needed */
            ಯದಿ (field_ಎಣಿಕೆ < 2 || (field_ಎಣಿಕೆ < 3 && !script.isEmpty())) {
                ಕುರು {
                    tag.append('_', err);
                    ++field_ಎಣಿಕೆ;
                } ಯಾವತ್(field_ಎಣಿಕೆ<2);
            }

            CharStringByteSink s(&tag);
            std::string_view sub = tmpLocaleID;
            sub.remove_prefix(1);
            _getVariant(sub, '@', &s, !variant.isEmpty(), err);
            ಯದಿ (U_FAILURE(err)) { ಪ್ರತ್ಯಾಗಮ; }
        }

        /* Look up the ID in the canonicalization map */
        ಪ್ರತಿ (j=0; j<UPRV_LENGTHOF(CANONICALIZE_MAP); j++) {
            StringPiece id(CANONICALIZE_MAP[j].id);
            ಯದಿ (tag == id) {
                ಯದಿ (id.empty() && !tmpLocaleID.empty()) {
                    ಭಙ್ಗ; /* Don't remap "" if keywords present */
                }
                tag.clear();
                tag.append(CANONICALIZE_MAP[j].canonicalID, err);
                ಭಙ್ಗ;
            }
        }
    }

    sink.Append(tag.data(), tag.length());

    ಯದಿ (!OPTION_SET(options, _ULOC_STRIP_KEYWORDS)) {
        ಯದಿ (!tmpLocaleID.empty() && keywordAssign != std::string_view::npos &&
            (separatorIndicator == std::string_view::npos || separatorIndicator > keywordAssign)) {
            sink.Append("@", 1);
            ++field_ಎಣಿಕೆ;
            tmpLocaleID.remove_prefix(1);
            ulocimp_getKeywords(tmpLocaleID, '@', sink, ಸತ್ಯ, err);
        }
    }
}

}  // namespace

/* ### ID parsing API **************************************************/

U_CAPI int32_t  U_EXPORT2
uloc_getParent(ಧ್ರುವ ವರ್ಣ*    localeID,
               ವರ್ಣ* parent,
               int32_t parentCapacity,
               UErrorCode* err)
{
    ಪ್ರತ್ಯಾಗಮ ByteSinkUtil::viaByteSinkToTerminatedChars(
        parent, parentCapacity,
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getParent(localeID, sink, status);
        },
        *err);
}

U_EXPORT CharString
ulocimp_getParent(ಧ್ರುವ ವರ್ಣ* localeID,
                  UErrorCode& err)
{
    ಪ್ರತ್ಯಾಗಮ ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getParent(localeID, sink, status);
        },
        err);
}

U_EXPORT ಶೂನ್ಯ
ulocimp_getParent(ಧ್ರುವ ವರ್ಣ* localeID,
                  icu::ByteSink& sink,
                  UErrorCode& err)
{
    ಯದಿ (U_FAILURE(err)) { ಪ್ರತ್ಯಾಗಮ; }

    ಧ್ರುವ ವರ್ಣ *lastUnderscore;
    int32_t i;

    ಯದಿ (localeID == nullptr)
        localeID = uloc_getDefault();

    lastUnderscore=uprv_strrchr(localeID, '_');
    ಯದಿ(lastUnderscore!=nullptr) {
        i = static_cast<int32_t>(lastUnderscore - localeID);
    } ಅನ್ಯಥಾ {
        i=0;
    }

    ಯದಿ (i > 0) {
        ಯದಿ (uprv_strnicmp(localeID, "und_", 4) == 0) {
            localeID += 3;
            i -= 3;
        }
        sink.Append(localeID, i);
    }
}

U_CAPI int32_t U_EXPORT2
uloc_getLanguage(ಧ್ರುವ ವರ್ಣ*    localeID,
         ವರ್ಣ* language,
         int32_t languageCapacity,
         UErrorCode* err)
{
    ಯದಿ (localeID == nullptr) {
        localeID = uloc_getDefault();
    }

    /* uloc_getLanguage will return a 2 character iso-639 code if one exists. *CWB*/
    ಪ್ರತ್ಯಾಗಮ ByteSinkUtil::viaByteSinkToTerminatedChars(
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
uloc_getScript(ಧ್ರುವ ವರ್ಣ*    localeID,
         ವರ್ಣ* script,
         int32_t scriptCapacity,
         UErrorCode* err)
{
    ಯದಿ (localeID == nullptr) {
        localeID = uloc_getDefault();
    }

    ಪ್ರತ್ಯಾಗಮ ByteSinkUtil::viaByteSinkToTerminatedChars(
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
uloc_getCountry(ಧ್ರುವ ವರ್ಣ* localeID,
            ವರ್ಣ* country,
            int32_t countryCapacity,
            UErrorCode* err)
{
    ಯದಿ (localeID == nullptr) {
        localeID = uloc_getDefault();
    }

    ಪ್ರತ್ಯಾಗಮ ByteSinkUtil::viaByteSinkToTerminatedChars(
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
uloc_getVariant(ಧ್ರುವ ವರ್ಣ* localeID,
                ವರ್ಣ* variant,
                int32_t variantCapacity,
                UErrorCode* err)
{
    ಯದಿ (localeID == nullptr) {
        localeID = uloc_getDefault();
    }

    ಪ್ರತ್ಯಾಗಮ ByteSinkUtil::viaByteSinkToTerminatedChars(
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
uloc_getName(ಧ್ರುವ ವರ್ಣ* localeID,
             ವರ್ಣ* name,
             int32_t nameCapacity,
             UErrorCode* err)
{
    ಯದಿ (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    ಪ್ರತ್ಯಾಗಮ ByteSinkUtil::viaByteSinkToTerminatedChars(
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
    ಪ್ರತ್ಯಾಗಮ ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getName(localeID, sink, status);
        },
        err);
}

U_EXPORT ಶೂನ್ಯ
ulocimp_getName(std::string_view localeID,
                ByteSink& sink,
                UErrorCode& err)
{
    _canonicalize(localeID, sink, 0, err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getBaseName(ಧ್ರುವ ವರ್ಣ* localeID,
                 ವರ್ಣ* name,
                 int32_t nameCapacity,
                 UErrorCode* err)
{
    ಯದಿ (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    ಪ್ರತ್ಯಾಗಮ ByteSinkUtil::viaByteSinkToTerminatedChars(
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
    ಪ್ರತ್ಯಾಗಮ ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getBaseName(localeID, sink, status);
        },
        err);
}

U_EXPORT ಶೂನ್ಯ
ulocimp_getBaseName(std::string_view localeID,
                    ByteSink& sink,
                    UErrorCode& err)
{
    _canonicalize(localeID, sink, _ULOC_STRIP_KEYWORDS, err);
}

U_CAPI int32_t  U_EXPORT2
uloc_canonicalize(ಧ್ರುವ ವರ್ಣ* localeID,
                  ವರ್ಣ* name,
                  int32_t nameCapacity,
                  UErrorCode* err)
{
    ಯದಿ (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    ಪ್ರತ್ಯಾಗಮ ByteSinkUtil::viaByteSinkToTerminatedChars(
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
    ಪ್ರತ್ಯಾಗಮ ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_canonicalize(localeID, sink, status);
        },
        err);
}

U_EXPORT ಶೂನ್ಯ
ulocimp_canonicalize(std::string_view localeID,
                     ByteSink& sink,
                     UErrorCode& err)
{
    _canonicalize(localeID, sink, _ULOC_CANONICALIZE, err);
}

U_CAPI ಧ್ರುವ ವರ್ಣ*  U_EXPORT2
uloc_getISO3Language(ಧ್ರುವ ವರ್ಣ* localeID)
{
    UErrorCode err = U_ZERO_ERROR;

    ಯದಿ (localeID == nullptr)
    {
        localeID = uloc_getDefault();
    }
    CharString lang = ulocimp_getLanguage(localeID, err);
    ಯದಿ (U_FAILURE(err))
        ಪ್ರತ್ಯಾಗಮ "";
    std::optional<int16_t> offset = _findIndex(LANGUAGES, lang.data());
    ಪ್ರತ್ಯಾಗಮ offset.has_value() ? LANGUAGES_3[*offset] : "";
}

U_CAPI ಧ್ರುವ ವರ್ಣ*  U_EXPORT2
uloc_getISO3Country(ಧ್ರುವ ವರ್ಣ* localeID)
{
    UErrorCode err = U_ZERO_ERROR;

    ಯದಿ (localeID == nullptr)
    {
        localeID = uloc_getDefault();
    }
    CharString cntry = ulocimp_getRegion(localeID, err);
    ಯದಿ (U_FAILURE(err))
        ಪ್ರತ್ಯಾಗಮ "";
    std::optional<int16_t> offset = _findIndex(COUNTRIES, cntry.data());
    ಪ್ರತ್ಯಾಗಮ offset.has_value() ? COUNTRIES_3[*offset] : "";
}

U_CAPI uint32_t  U_EXPORT2
uloc_getLCID(ಧ್ರುವ ವರ್ಣ* localeID)
{
    UErrorCode status = U_ZERO_ERROR;
    uint32_t   lcid = 0;

    /* Check for incomplete id. */
    ಯದಿ (!localeID || uprv_strlen(localeID) < 2) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    // First, attempt Windows platform lookup if available, but fall
    // through to catch any special cases (ICU vs Windows name differences).
    lcid = uprv_convertToLCIDPlatform(localeID, &status);
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ (lcid > 0) {
        // Windows found an LCID, return that
        ಪ್ರತ್ಯಾಗಮ lcid;
    }

    CharString langID = ulocimp_getLanguage(localeID, status);
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ (uprv_strchr(localeID, '@')) {
        // uprv_convertToLCID does not support keywords other than collation.
        // Remove all keywords except collation.
        CharString collVal = ulocimp_get_Keyword_ಮೌಲ್ಯ(localeID, "collation", status);
        ಯದಿ (U_SUCCESS(status) && !collVal.isEmpty()) {
            CharString tmpLocaleID = ulocimp_getBaseName(localeID, status);
            ulocimp_set_Keyword_ಮೌಲ್ಯ("collation", collVal.toStringPiece(), tmpLocaleID, status);
            ಯದಿ (U_SUCCESS(status)) {
                ಪ್ರತ್ಯಾಗಮ uprv_convertToLCID(langID.data(), tmpLocaleID.data(), &status);
            }
        }

        // fall through - all keywords are simply ignored
        status = U_ZERO_ERROR;
    }

    ಪ್ರತ್ಯಾಗಮ uprv_convertToLCID(langID.data(), localeID, &status);
}

U_CAPI int32_t U_EXPORT2
uloc_getLocaleForLCID(uint32_t hostid, ವರ್ಣ *locale, int32_t localeCapacity,
                UErrorCode *status)
{
    ಪ್ರತ್ಯಾಗಮ uprv_convertToPosix(hostid, locale, localeCapacity, status);
}

/* ### Default locale **************************************************/

U_CAPI ಧ್ರುವ ವರ್ಣ*  U_EXPORT2
uloc_getDefault()
{
    ಪ್ರತ್ಯಾಗಮ locale_get_default();
}

U_CAPI ಶೂನ್ಯ  U_EXPORT2
uloc_setDefault(ಧ್ರುವ ವರ್ಣ*   newDefaultLocale,
             UErrorCode* err)
{
    ಯದಿ (U_FAILURE(*err))
        ಪ್ರತ್ಯಾಗಮ;
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
U_CAPI ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ*  U_EXPORT2
uloc_getISOLanguages()
{
    ಪ್ರತ್ಯಾಗಮ LANGUAGES;
}

/**
 * Returns a list of all 2-letter country codes defined in ISO 639.  This is a
 * pointer to an array of pointers to arrays of char.  All of these pointers are
 * owned by ICU-- do not delete them, and do not write through them.  The array is
 * terminated with a null pointer.
 */
U_CAPI ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ*  U_EXPORT2
uloc_getISOCountries()
{
    ಪ್ರತ್ಯಾಗಮ COUNTRIES;
}

U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
uloc_to_Unicode_Locale_ಕೀಲಿ(ಧ್ರುವ ವರ್ಣ* keyword U_LIFETIME_BOUND)
{
    ಯದಿ (keyword == nullptr || *keyword == '\0') { ಪ್ರತ್ಯಾಗಮ nullptr; }
    std::optional<std::string_view> result = ulocimp_to_Bcp_ಕೀಲಿ_With_Fallback(keyword);
    ಪ್ರತ್ಯಾಗಮ result.has_value() ? result->data() : nullptr;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_to_Bcp_ಕೀಲಿ_With_Fallback(std::string_view keyword U_LIFETIME_BOUND)
{
    std::optional<std::string_view> bcp_ಕೀಲಿ = ulocimp_to_Bcp_ಕೀಲಿ(keyword);
    ಯದಿ (!bcp_ಕೀಲಿ.has_value() &&
        ultag_is_Unicode_Locale_ಕೀಲಿ(keyword.data(), static_cast<int32_t>(keyword.size()))) {
        // unknown keyword, but syntax is fine..
        ಪ್ರತ್ಯಾಗಮ keyword;
    }
    ಪ್ರತ್ಯಾಗಮ bcp_ಕೀಲಿ;
}

U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
uloc_toUnicodeLocaleType(ಧ್ರುವ ವರ್ಣ* keyword, ಧ್ರುವ ವರ್ಣ* value U_LIFETIME_BOUND)
{
    ಯದಿ (keyword == nullptr || *keyword == '\0' ||
        value == nullptr || *value == '\0') { ಪ್ರತ್ಯಾಗಮ nullptr; }
    std::optional<std::string_view> result = ulocimp_toBcpTypeWithFallback(keyword, value);
    ಪ್ರತ್ಯಾಗಮ result.has_value() ? result->data() : nullptr;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpTypeWithFallback(std::string_view keyword, std::string_view value U_LIFETIME_BOUND)
{
    std::optional<std::string_view> bcpType = ulocimp_toBcpType(keyword, value);
    ಯದಿ (!bcpType.has_value() &&
        ultag_isUnicodeLocaleType(value.data(), static_cast<int32_t>(value.size()))) {
        // unknown keyword, but syntax is fine..
        ಪ್ರತ್ಯಾಗಮ value;
    }
    ಪ್ರತ್ಯಾಗಮ bcpType;
}

ನಾಮಾಕಾಶ {

ತಾರ್ಕಿಕ
is_Well_Formed_Legacy_ಕೀಲಿ(std::string_view ಕೀಲಿ)
{
    ಪ್ರತ್ಯಾಗಮ std::all_of(ಕೀಲಿ.begin(), ಕೀಲಿ.end(), UPRV_ISALPHANUM);
}

ತಾರ್ಕಿಕ
isWellFormedLegacyType(std::string_view legacyType)
{
    int32_t alpha_ಅಂಕೆ_Len = 0;
    ಪ್ರತಿ (ವರ್ಣ c : legacyType) {
        ಯದಿ (c == '_' || c == '/' || c == '-') {
            ಯದಿ (alpha_ಅಂಕೆ_Len == 0) {
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
            alpha_ಅಂಕೆ_Len = 0;
        } ಅನ್ಯಥಾ ಯದಿ (UPRV_ISALPHANUM(c)) {
            alpha_ಅಂಕೆ_Len++;
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ alpha_ಅಂಕೆ_Len != 0;
}

}  // namespace

U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
uloc_to_Legacy_ಕೀಲಿ(ಧ್ರುವ ವರ್ಣ* keyword U_LIFETIME_BOUND)
{
    ಯದಿ (keyword == nullptr || *keyword == '\0') { ಪ್ರತ್ಯಾಗಮ nullptr; }
    std::optional<std::string_view> result = ulocimp_to_Legacy_ಕೀಲಿ_With_Fallback(keyword);
    ಪ್ರತ್ಯಾಗಮ result.has_value() ? result->data() : nullptr;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_to_Legacy_ಕೀಲಿ_With_Fallback(std::string_view keyword U_LIFETIME_BOUND)
{
    std::optional<std::string_view> legacy_ಕೀಲಿ_2 = ulocimp_to_Legacy_ಕೀಲಿ(keyword);
    ಯದಿ (!legacy_ಕೀಲಿ_2.has_value() && is_Well_Formed_Legacy_ಕೀಲಿ(keyword)) {
        // Checks if the specified locale key is well-formed with the legacy locale syntax.
        //
        // Note:
        //  LDML/CLDR provides some definition of keyword syntax in
        //  * http://www.unicode.org/reports/tr35/#Unicode_locale_identifier and
        //  * http://www.unicode.org/reports/tr35/#Old_Locale_Extension_Syntax
        //  Keys can only consist of [0-9a-zA-Z].
        ಪ್ರತ್ಯಾಗಮ keyword;
    }
    ಪ್ರತ್ಯಾಗಮ legacy_ಕೀಲಿ_2;
}

U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
uloc_toLegacyType(ಧ್ರುವ ವರ್ಣ* keyword, ಧ್ರುವ ವರ್ಣ* value U_LIFETIME_BOUND)
{
    ಯದಿ (keyword == nullptr || *keyword == '\0' ||
        value == nullptr || *value == '\0') { ಪ್ರತ್ಯಾಗಮ nullptr; }
    std::optional<std::string_view> result = ulocimp_toLegacyTypeWithFallback(keyword, value);
    ಪ್ರತ್ಯಾಗಮ result.has_value() ? result->data() : nullptr;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyTypeWithFallback(std::string_view keyword, std::string_view value U_LIFETIME_BOUND)
{
    std::optional<std::string_view> legacyType = ulocimp_toLegacyType(keyword, value);
    ಯದಿ (!legacyType.has_value() && isWellFormedLegacyType(value)) {
        // Checks if the specified locale type is well-formed with the legacy locale syntax.
        //
        // Note:
        //  LDML/CLDR provides some definition of keyword syntax in
        //  * http://www.unicode.org/reports/tr35/#Unicode_locale_identifier and
        //  * http://www.unicode.org/reports/tr35/#Old_Locale_Extension_Syntax
        //  Values (types) can only consist of [0-9a-zA-Z], plus for legacy values
        //  we allow [/_-+] in the middle (e.g. "Etc/GMT+1", "Asia/Tel_Aviv")
        ಪ್ರತ್ಯಾಗಮ value;
    }
    ಪ್ರತ್ಯಾಗಮ legacyType;
}

/*eof*/
