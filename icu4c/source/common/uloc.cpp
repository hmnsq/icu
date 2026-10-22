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

#ଅନ୍ତର୍ଭାଵ <algorithm>
#ଅନ୍ତର୍ଭାଵ <optional>
#ଅନ୍ତର୍ଭାଵ <string_view>

#ଅନ୍ତର୍ଭାଵ "unicode/bytestream.h"
#ଅନ୍ତର୍ଭାଵ "unicode/errorcode.h"
#ଅନ୍ତର୍ଭାଵ "unicode/stringpiece.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uloc.h"

#ଅନ୍ତର୍ଭାଵ "bytesinkutil.h"
#ଅନ୍ତର୍ଭାଵ "putilimp.h"
#ଅନ୍ତର୍ଭାଵ "ustr_imp.h"
#ଅନ୍ତର୍ଭାଵ "ulocimp.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "locmap.h"
#ଅନ୍ତର୍ଭାଵ "uarrsort.h"
#ଅନ୍ତର୍ଭାଵ "uenumimp.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"

U_NAMESPACE_USE

/* ### Declarations **************************************************/

/* Locale stuff from locid.cpp */
U_CFUNC ଶୂନ୍ୟ locale_set_default(ଅଚଳ ଅକ୍ଷର *id);
U_CFUNC ଅଚଳ ଅକ୍ଷର *locale_get_default();

ନାମାକାଶ {

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
constexpr ଅଚଳ ଅକ୍ଷର* LANGUAGES[] = {
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

constexpr ଅଚଳ ଅକ୍ଷର* DEPRECATED_LANGUAGES[]={
    "in", "iw", "ji", "jw", "mo", nullptr, nullptr
};
constexpr ଅଚଳ ଅକ୍ଷର* REPLACEMENT_LANGUAGES[]={
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
constexpr ଅଚଳ ଅକ୍ଷର* LANGUAGES_3[] = {
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
constexpr ଅଚଳ ଅକ୍ଷର* COUNTRIES[] = {
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

constexpr ଅଚଳ ଅକ୍ଷର* DEPRECATED_COUNTRIES[] = {
    "AN", "BU", "CS", "DD", "DY", "FX", "HV", "NH", "RH", "SU", "TP", "UK", "VD", "YD", "YU", "ZR", nullptr, nullptr /* deprecated country list */
};
constexpr ଅଚଳ ଅକ୍ଷର* REPLACEMENT_COUNTRIES[] = {
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
constexpr ଅଚଳ ଅକ୍ଷର* COUNTRIES_3[] = {
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

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା CanonicalizationMap {
    ଅଚଳ ଅକ୍ଷର *id;          /* input ID */
    ଅଚଳ ଅକ୍ଷର *canonicalID; /* canonicalized output ID */
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
    ତାର୍କିକ reset = ସତ୍ୟ;

    ପାଇଁ (i = 0; i < localeIDLength; i++) {
        ଯଦି (localeID[i] != '_' && localeID[i] != '-') {
            ଯଦି (reset) {
                tmpLength = 0;
                reset = ମିଥ୍ୟା;
            }
            tmpLength++;
        } ଅନ୍ୟଥା {
            ଯଦି (tmpLength != 0 && tmpLength < length) {
                length = tmpLength;
            }
            reset = ସତ୍ୟ;
        }
    }

    ଫେରନ୍ତୁ length;
}
/* Test if the locale id has BCP47 u extension and does not have '@' */
ଧାଡ଼ିଭିତରେ ତାର୍କିକ _hasBCP47Extension(std::string_view id) {
    ଫେରନ୍ତୁ id.find('@') == std::string_view::npos && getShortestSubtagLength(id) == 1;
}

/* ### Keywords **************************************************/
ଧାଡ଼ିଭିତରେ ତାର୍କିକ UPRV_ISDIGIT(ଅକ୍ଷର c) { ଫେରନ୍ତୁ c >= '0' && c <= '9'; }
ଧାଡ଼ିଭିତରେ ତାର୍କିକ UPRV_ISALPHANUM(ଅକ୍ଷର c) { ଫେରନ୍ତୁ uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }
/* Punctuation/symbols allowed in legacy key values */
ଧାଡ଼ିଭିତରେ ତାର୍କିକ UPRV_OK_ମୂଲ୍ୟ_PUNCTUATION(ଅକ୍ଷର c) { ଫେରନ୍ତୁ c == '_' || c == '-' || c == '+' || c == '/'; }

}  // namespace

#define ULOC_KEYWORD_BUFFER_LEN 25
#define ULOC_MAX_NO_KEYWORDS 25

U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
locale_getKeywordsStart(std::string_view localeID U_LIFETIME_BOUND) {
    ଯଦି (size_t pos = localeID.find('@'); pos != std::string_view::npos) {
        ଫେରନ୍ତୁ localeID.data() + pos;
    }
#ଯଦି (U_CHARSET_FAMILY == U_EBCDIC_FAMILY)
    ଅନ୍ୟଥା {
        /* We do this because the @ sign is variant, and the @ sign used on one
        EBCDIC machine won't be compiled the same way on other EBCDIC based
        machines. */
        ସ୍ଥିର ଅଚଳ uint8_t ebcdicSigns[] = { 0x7C, 0x44, 0x66, 0x80, 0xAC, 0xAE, 0xAF, 0xB5, 0xEC, 0xEF, 0x00 };
        ଅଚଳ uint8_t *charToFind = ebcdicSigns;
        ଯେତେବେଳେ(*charToFind) {
            ଯଦି (size_t pos = localeID.find(*charToFind); pos != std::string_view::npos) {
                ଫେରନ୍ତୁ localeID.data() + pos;
            }
            charToFind++;
        }
    }
#endif
    ଫେରନ୍ତୁ nullptr;
}

ନାମାକାଶ {

/**
 * @param keywordName incoming name to be canonicalized
 * @param status return status (keyword too long)
 * @return the keyword name
 */
CharString locale_canonKeywordName(std::string_view keywordName, UErrorCode& status)
{
  ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ {}; }
  CharString result;

  ପାଇଁ (ଅକ୍ଷର c : keywordName) {
    ଯଦି (!UPRV_ISALPHANUM(c)) {
      status = U_ILLEGAL_ARGUMENT_ERROR; /* malformed keyword name */
      ଫେରନ୍ତୁ {};
    }
    result.append(uprv_tolower(c), status);
  }
  ଯଦି (result.isEmpty()) {
    status = U_ILLEGAL_ARGUMENT_ERROR; /* empty keyword name */
    ଫେରନ୍ତୁ {};
  }

  ଫେରନ୍ତୁ result;
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ଅକ୍ଷର keyword[ULOC_KEYWORD_BUFFER_LEN];
    int32_t keywordLen;
    ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ_Start;
    int32_t ମୂଲ୍ୟ_Len;
} KeywordStruct;

int32_t U_CALLCONV
compareKeywordStructs(ଅଚଳ ଶୂନ୍ୟ * /*context*/, ଅଚଳ ଶୂନ୍ୟ *left, ଅଚଳ ଶୂନ୍ୟ *ଡାହାଣ) {
    ଅଚଳ ଅକ୍ଷର* ବାମ_String = static_cast<ଅଚଳ KeywordStruct*>(left)->keyword;
    ଅଚଳ ଅକ୍ଷର* ଡାହାଣ_String = static_cast<ଅଚଳ KeywordStruct*>(ଡାହାଣ)->keyword;
    ଫେରନ୍ତୁ uprv_strcmp(ବାମ_String, ଡାହାଣ_String);
}

}  // namespace

U_EXPORT CharString
ulocimp_getKeywords(std::string_view localeID,
                    ଅକ୍ଷର prev,
                    ତାର୍କିକ ମୂଲ୍ୟଗୁଡ଼ିକ_Too,
                    UErrorCode& status)
{
    ଫେରନ୍ତୁ ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getKeywords(localeID,
                                prev,
                                sink,
                                ମୂଲ୍ୟଗୁଡ଼ିକ_Too,
                                status);
        },
        status);
}

U_EXPORT ଶୂନ୍ୟ
ulocimp_getKeywords(std::string_view localeID,
                    ଅକ୍ଷର prev,
                    ByteSink& sink,
                    ତାର୍କିକ ମୂଲ୍ୟଗୁଡ଼ିକ_Too,
                    UErrorCode& status)
{
    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }

    KeywordStruct keywordList[ULOC_MAX_NO_KEYWORDS];

    int32_t maxKeywords = ULOC_MAX_NO_KEYWORDS;
    int32_t ଅଙ୍କ_Keywords = 0;
    size_t equalSign = std::string_view::npos;
    size_t semicolon = std::string_view::npos;
    int32_t i = 0, j, n;

    ଯଦି(prev == '@') { /* start of keyword definition */
        /* we will grab pairs, trim spaces, lowercase keywords, sort and return */
        କର {
            ତାର୍କିକ duplicate = ମିଥ୍ୟା;
            /* skip leading spaces */
            ଯେତେବେଳେ (!localeID.empty() && localeID.front() == ' ') {
                localeID.remove_prefix(1);
            }
            ଯଦି (localeID.empty()) { /* handle trailing "; " */
                ଭାଙ୍ଗ;
            }
            ଯଦି(ଅଙ୍କ_Keywords == maxKeywords) {
                status = U_INTERNAL_PROGRAM_ERROR;
                ଫେରନ୍ତୁ;
            }
            equalSign = localeID.find('=');
            semicolon = localeID.find(';');
            /* lack of '=' [foo@currency] is illegal */
            /* ';' before '=' [foo@currency;collation=pinyin] is illegal */
            ଯଦି (equalSign == std::string_view::npos ||
                (semicolon != std::string_view::npos && semicolon < equalSign)) {
                status = U_INVALID_FORMAT_ERROR;
                ଫେରନ୍ତୁ;
            }
            /* zero-length keyword is an error. */
            ଯଦି (equalSign == 0) {
                status = U_INVALID_FORMAT_ERROR;
                ଫେରନ୍ତୁ;
            }
            /* need to normalize both keyword and keyword name */
            ଯଦି (equalSign >= ULOC_KEYWORD_BUFFER_LEN) {
                /* keyword name too long for internal buffer */
                status = U_INTERNAL_PROGRAM_ERROR;
                ଫେରନ୍ତୁ;
            }
            ପାଇଁ (i = 0, n = 0; static_cast<size_t>(i) < equalSign; ++i) {
                ଯଦି (localeID[i] != ' ') {
                    keywordList[ଅଙ୍କ_Keywords].keyword[n++] = uprv_tolower(localeID[i]);
                }
            }

            keywordList[ଅଙ୍କ_Keywords].keyword[n] = 0;
            keywordList[ଅଙ୍କ_Keywords].keywordLen = n;
            /* now grab the value part. First we skip the '=' */
            equalSign++;
            /* then we leading spaces */
            ଯେତେବେଳେ (equalSign < localeID.length() && localeID[equalSign] == ' ') {
                equalSign++;
            }

            /* Premature end or zero-length value */
            ଯଦି (equalSign == localeID.length() || equalSign == semicolon) {
                status = U_INVALID_FORMAT_ERROR;
                ଫେରନ୍ତୁ;
            }

            keywordList[ଅଙ୍କ_Keywords].ମୂଲ୍ୟ_Start = localeID.data() + equalSign;

            std::string_view value = localeID;
            ଯଦି (semicolon != std::string_view::npos) {
                value.remove_suffix(value.length() - semicolon);
                localeID.remove_prefix(semicolon + 1);
            } ଅନ୍ୟଥା {
                localeID = {};
            }
            value.remove_prefix(equalSign);
            ଯଦି (size_t last = value.find_last_not_of(' '); last != std::string_view::npos) {
                value.remove_suffix(value.length() - last - 1);
            }
            keywordList[ଅଙ୍କ_Keywords].ମୂଲ୍ୟ_Len = static_cast<int32_t>(value.length());

            /* If this is a duplicate keyword, then ignore it */
            ପାଇଁ (j=0; j<ଅଙ୍କ_Keywords; ++j) {
                ଯଦି (uprv_strcmp(keywordList[j].keyword, keywordList[ଅଙ୍କ_Keywords].keyword) == 0) {
                    duplicate = ସତ୍ୟ;
                    ଭାଙ୍ଗ;
                }
            }
            ଯଦି (!duplicate) {
                ++ଅଙ୍କ_Keywords;
            }
        } ଯେତେବେଳେ (!localeID.empty());

        /* now we have a list of keywords */
        /* we need to sort it */
        uprv_sortArray(keywordList, ଅଙ୍କ_Keywords, ଆକାର(KeywordStruct), compareKeywordStructs, nullptr, ମିଥ୍ୟା, &status);

        /* Now construct the keyword part */
        ପାଇଁ(i = 0; i < ଅଙ୍କ_Keywords; i++) {
            sink.Append(keywordList[i].keyword, keywordList[i].keywordLen);
            ଯଦି(ମୂଲ୍ୟଗୁଡ଼ିକ_Too) {
                sink.Append("=", 1);
                sink.Append(keywordList[i].ମୂଲ୍ୟ_Start, keywordList[i].ମୂଲ୍ୟ_Len);
                ଯଦି(i < ଅଙ୍କ_Keywords - 1) {
                    sink.Append(";", 1);
                }
            } ଅନ୍ୟଥା {
                sink.Append("\0", 1);
            }
        }
    }
}

U_CAPI int32_t U_EXPORT2
uloc_get_Keyword_ମୂଲ୍ୟ(ଅଚଳ ଅକ୍ଷର* localeID,
                     ଅଚଳ ଅକ୍ଷର* keywordName,
                     ଅକ୍ଷର* buffer, int32_t bufferCapacity,
                     UErrorCode* status)
{
    ଯଦି (U_FAILURE(*status)) { ଫେରନ୍ତୁ 0; }
    ଯଦି (keywordName == nullptr || *keywordName == '\0') {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ ByteSinkUtil::viaByteSinkToTerminatedChars(
        buffer, bufferCapacity,
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_get_Keyword_ମୂଲ୍ୟ(localeID, keywordName, sink, status);
        },
        *status);
}

U_EXPORT CharString
ulocimp_get_Keyword_ମୂଲ୍ୟ(ଅଚଳ ଅକ୍ଷର* localeID,
                        std::string_view keywordName,
                        UErrorCode& status)
{
    ଫେରନ୍ତୁ ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_get_Keyword_ମୂଲ୍ୟ(localeID, keywordName, sink, status);
        },
        status);
}

U_EXPORT ଶୂନ୍ୟ
ulocimp_get_Keyword_ମୂଲ୍ୟ(ଅଚଳ ଅକ୍ଷର* localeID,
                        std::string_view keywordName,
                        icu::ByteSink& sink,
                        UErrorCode& status)
{
    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }

    ଯଦି (localeID == nullptr || keywordName.empty()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }

    ଅଚଳ ଅକ୍ଷର* start_ସନ୍ଧାନ_Here = nullptr;
    ଅଚଳ ଅକ୍ଷର* nextSeparator = nullptr;

    CharString tempBuffer;
    ଅଚଳ ଅକ୍ଷର* tmpLocaleID;

    CharString canonKeywordName = locale_canonKeywordName(keywordName, status);
    ଯଦି (U_FAILURE(status)) {
      ଫେରନ୍ତୁ;
    }

    ଯଦି (localeID != nullptr && _hasBCP47Extension(localeID)) {
        tempBuffer = ulocimp_forLanguageTag(localeID, -1, nullptr, status);
        tmpLocaleID = U_SUCCESS(status) && !tempBuffer.isEmpty() ? tempBuffer.data() : localeID;
    } ଅନ୍ୟଥା {
        tmpLocaleID=localeID;
    }

    start_ସନ୍ଧାନ_Here = locale_getKeywordsStart(tmpLocaleID);
    ଯଦି(start_ସନ୍ଧାନ_Here == nullptr) {
        /* no keywords, return at once */
        ଫେରନ୍ତୁ;
    }

    /* find the first keyword */
    ଯେତେବେଳେ(start_ସନ୍ଧାନ_Here) {
        ଅଚଳ ଅକ୍ଷର* ଚାବି_ମୂଲ୍ୟ_Tail;

        start_ସନ୍ଧାନ_Here++; /* skip @ or ; */
        nextSeparator = uprv_strchr(start_ସନ୍ଧାନ_Here, '=');
        ଯଦି(!nextSeparator) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* key must have =value */
            ଫେରନ୍ତୁ;
        }
        /* strip leading & trailing spaces (TC decided to tolerate these) */
        ଯେତେବେଳେ(*start_ସନ୍ଧାନ_Here == ' ') {
            start_ସନ୍ଧାନ_Here++;
        }
        ଚାବି_ମୂଲ୍ୟ_Tail = nextSeparator;
        ଯେତେବେଳେ (ଚାବି_ମୂଲ୍ୟ_Tail > start_ସନ୍ଧାନ_Here && *(ଚାବି_ମୂଲ୍ୟ_Tail-1) == ' ') {
            ଚାବି_ମୂଲ୍ୟ_Tail--;
        }
        /* now keyValueTail points to first char after the keyName */
        /* copy & normalize keyName from locale */
        ଯଦି (start_ସନ୍ଧାନ_Here == ଚାବି_ମୂଲ୍ୟ_Tail) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* empty keyword name in passed-in locale */
            ଫେରନ୍ତୁ;
        }
        CharString localeKeywordName;
        ଯେତେବେଳେ (start_ସନ୍ଧାନ_Here < ଚାବି_ମୂଲ୍ୟ_Tail) {
          ଯଦି (!UPRV_ISALPHANUM(*start_ସନ୍ଧାନ_Here)) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* malformed keyword name */
            ଫେରନ୍ତୁ;
          }
          localeKeywordName.append(uprv_tolower(*start_ସନ୍ଧାନ_Here++), status);
        }
        ଯଦି (U_FAILURE(status)) {
            ଫେରନ୍ତୁ;
        }

        start_ସନ୍ଧାନ_Here = uprv_strchr(nextSeparator, ';');

        ଯଦି (canonKeywordName == localeKeywordName) {
             /* current entry matches the keyword. */
           nextSeparator++; /* skip '=' */
            /* First strip leading & trailing spaces (TC decided to tolerate these) */
            ଯେତେବେଳେ(*nextSeparator == ' ') {
              nextSeparator++;
            }
            ଚାବି_ମୂଲ୍ୟ_Tail = (start_ସନ୍ଧାନ_Here)? start_ସନ୍ଧାନ_Here: nextSeparator + uprv_strlen(nextSeparator);
            ଯେତେବେଳେ(ଚାବି_ମୂଲ୍ୟ_Tail > nextSeparator && *(ଚାବି_ମୂଲ୍ୟ_Tail-1) == ' ') {
              ଚାବି_ମୂଲ୍ୟ_Tail--;
            }
            /* Now copy the value, but check well-formedness */
            ଯଦି (nextSeparator == ଚାବି_ମୂଲ୍ୟ_Tail) {
              status = U_ILLEGAL_ARGUMENT_ERROR; /* empty key value name in passed-in locale */
              ଫେରନ୍ତୁ;
            }
            ଯେତେବେଳେ (nextSeparator < ଚାବି_ମୂଲ୍ୟ_Tail) {
              ଯଦି (!UPRV_ISALPHANUM(*nextSeparator) && !UPRV_OK_ମୂଲ୍ୟ_PUNCTUATION(*nextSeparator)) {
                status = U_ILLEGAL_ARGUMENT_ERROR; /* malformed key value */
                ଫେରନ୍ତୁ;
              }
              /* Should we lowercase value to return here? Tests expect as-is. */
              sink.Append(nextSeparator++, 1);
            }
            ଫେରନ୍ତୁ;
        }
    }
}

U_CAPI int32_t U_EXPORT2
uloc_set_Keyword_ମୂଲ୍ୟ(ଅଚଳ ଅକ୍ଷର* keywordName,
                     ଅଚଳ ଅକ୍ଷର* keyword_ମୂଲ୍ୟ,
                     ଅକ୍ଷର* buffer, int32_t bufferCapacity,
                     UErrorCode* status)
{
    ଯଦି (U_FAILURE(*status)) { ଫେରନ୍ତୁ 0; }

    ଯଦି (keywordName == nullptr || *keywordName == 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    ଯଦି (bufferCapacity <= 1) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    int32_t bufLen = (int32_t)uprv_strlen(buffer);
    ଯଦି(bufferCapacity<bufLen) {
        /* The capacity is less than the length?! Is this NUL terminated? */
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    ଅକ୍ଷର* keywords = const_cast<ଅକ୍ଷର*>(
        locale_getKeywordsStart({buffer, static_cast<std::string_view::size_type>(bufLen)}));
    int32_t baseLen = keywords == nullptr ? bufLen : keywords - buffer;
    // Remove -1 from the capacity so that this function can guarantee NUL termination.
    CheckedArrayByteSink sink(keywords == nullptr ? buffer + bufLen : keywords,
                              bufferCapacity - baseLen - 1);
    int32_t reslen = ulocimp_set_Keyword_ମୂଲ୍ୟ(
        keywords == nullptr ? std::string_view() : keywords,
        keywordName,
        keyword_ମୂଲ୍ୟ == nullptr ? std::string_view() : keyword_ମୂଲ୍ୟ,
        sink,
        *status);

    ଯଦି (U_FAILURE(*status)) {
        ଫେରନ୍ତୁ *status == U_BUFFER_OVERFLOW_ERROR ? reslen + baseLen : 0;
    }

    // See the documentation for this function, it's guaranteed to never
    // overflow the buffer but instead abort with BUFFER_OVERFLOW_ERROR.
    // In this case, nothing has been written to the sink, so it cannot have Overflowed().
    U_ASSERT(!sink.Overflowed());
    U_ASSERT(reslen >= 0);
    ଫେରନ୍ତୁ u_terminateChars(buffer, bufferCapacity, reslen + baseLen, status);
}

U_EXPORT ଶୂନ୍ୟ
ulocimp_set_Keyword_ମୂଲ୍ୟ(std::string_view keywordName,
                        std::string_view keyword_ମୂଲ୍ୟ,
                        CharString& localeID,
                        UErrorCode& status)
{
    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }
    std::string_view keywords;
    ଯଦି (ଅଚଳ ଅକ୍ଷର* start = locale_getKeywordsStart(localeID.toStringPiece()); start != nullptr) {
        // This is safe because CharString::truncate() doesn't actually erase any
        // data, but simply sets the position for where new data will be written.
        int32_t size = start - localeID.data();
        keywords = localeID.toStringPiece();
        keywords.remove_prefix(size);
        localeID.truncate(size);
    }
    CharStringByteSink sink(&localeID);
    ulocimp_set_Keyword_ମୂଲ୍ୟ(keywords, keywordName, keyword_ମୂଲ୍ୟ, sink, status);
}

U_EXPORT int32_t
ulocimp_set_Keyword_ମୂଲ୍ୟ(std::string_view keywords,
                        std::string_view keywordName,
                        std::string_view keyword_ମୂଲ୍ୟ,
                        ByteSink& sink,
                        UErrorCode& status)
{
    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ 0; }

    /* TODO: sorting. removal. */
    int32_t needLen = 0;
    int32_t rc;
    CharString updated_ଚାବିଗୁଡ଼ିକ_And_ମୂଲ୍ୟଗୁଡ଼ିକ;
    ତାର୍କିକ handled_ନିବେଶ_ଚାବି_And_ମୂଲ୍ୟ = ମିଥ୍ୟା;
    ଅକ୍ଷର ଚାବି_ମୂଲ୍ୟ_Prefix = '@';

    ଯଦି (status == U_STRING_NOT_TERMINATED_WARNING) {
        status = U_ZERO_ERROR;
    }
    ଯଦି (keywordName.empty()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    CharString canonKeywordName = locale_canonKeywordName(keywordName, status);
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ 0;
    }

    CharString canon_Keyword_ମୂଲ୍ୟ;
    ପାଇଁ (ଅକ୍ଷର c : keyword_ମୂଲ୍ୟ) {
        ଯଦି (!UPRV_ISALPHANUM(c) && !UPRV_OK_ମୂଲ୍ୟ_PUNCTUATION(c)) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* malformed key value */
            ଫେରନ୍ତୁ 0;
        }
        /* Should we force lowercase in value to set? */
        canon_Keyword_ମୂଲ୍ୟ.append(c, status);
    }
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ 0;
    }

    ଯଦି (keywords.size() <= 1) {
        ଯଦି (canon_Keyword_ମୂଲ୍ୟ.isEmpty()) { /* no keywords = nothing to remove */
            U_ASSERT(status != U_STRING_NOT_TERMINATED_WARNING);
            ଫେରନ୍ତୁ 0;
        }

        needLen = 1 + canonKeywordName.length() + 1 + canon_Keyword_ମୂଲ୍ୟ.length();
        int32_t capacity = 0;
        ଅକ୍ଷର* buffer = sink.GetAppendBuffer(
                needLen, needLen, nullptr, needLen, &capacity);
        ଯଦି (capacity < needLen || buffer == nullptr) {
            status = U_BUFFER_OVERFLOW_ERROR;
            ଫେରନ୍ତୁ needLen; /* no change */
        }
        ଅକ୍ଷର* it = buffer;

        *it++ = '@';
        uprv_memcpy(it, canonKeywordName.data(), canonKeywordName.length());
        it += canonKeywordName.length();
        *it++ = '=';
        uprv_memcpy(it, canon_Keyword_ମୂଲ୍ୟ.data(), canon_Keyword_ମୂଲ୍ୟ.length());
        sink.Append(buffer, needLen);
        U_ASSERT(status != U_STRING_NOT_TERMINATED_WARNING);
        ଫେରନ୍ତୁ needLen;
    } /* end shortcut - no @ */

    /* search for keyword */
    ପାଇଁ (size_t keywordStart = 0; keywordStart != std::string_view::npos;) {
        keywordStart++; /* skip @ or ; */
        size_t nextEqualsign = keywords.find('=', keywordStart);
        ଯଦି (nextEqualsign == std::string_view::npos) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* key must have =value */
            ଫେରନ୍ତୁ 0;
        }
        /* strip leading & trailing spaces (TC decided to tolerate these) */
        ଯେତେବେଳେ (keywordStart < keywords.size() && keywords[keywordStart] == ' ') {
            keywordStart++;
        }
        size_t ଚାବି_ମୂଲ୍ୟ_Tail = nextEqualsign;
        ଯେତେବେଳେ (ଚାବି_ମୂଲ୍ୟ_Tail > keywordStart && keywords[ଚାବି_ମୂଲ୍ୟ_Tail - 1] == ' ') {
            ଚାବି_ମୂଲ୍ୟ_Tail--;
        }
        /* now keyValueTail points to first char after the keyName */
        /* copy & normalize keyName from locale */
        ଯଦି (keywordStart == ଚାବି_ମୂଲ୍ୟ_Tail) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* empty keyword name in passed-in locale */
            ଫେରନ୍ତୁ 0;
        }
        CharString localeKeywordName;
        ଯେତେବେଳେ (keywordStart < ଚାବି_ମୂଲ୍ୟ_Tail) {
            ଯଦି (!UPRV_ISALPHANUM(keywords[keywordStart])) {
                status = U_ILLEGAL_ARGUMENT_ERROR; /* malformed keyword name */
                ଫେରନ୍ତୁ 0;
            }
            localeKeywordName.append(uprv_tolower(keywords[keywordStart++]), status);
        }
        ଯଦି (U_FAILURE(status)) {
            ଫେରନ୍ତୁ 0;
        }

        size_t nextSeparator = keywords.find(';', nextEqualsign);

        /* start processing the value part */
        nextEqualsign++; /* skip '=' */
        /* First strip leading & trailing spaces (TC decided to tolerate these) */
        ଯେତେବେଳେ (nextEqualsign < keywords.size() && keywords[nextEqualsign] == ' ') {
            nextEqualsign++;
        }
        ଚାବି_ମୂଲ୍ୟ_Tail = nextSeparator == std::string_view::npos ? keywords.size() : nextSeparator;
        ଯେତେବେଳେ (ଚାବି_ମୂଲ୍ୟ_Tail > nextEqualsign && keywords[ଚାବି_ମୂଲ୍ୟ_Tail - 1] == ' ') {
            ଚାବି_ମୂଲ୍ୟ_Tail--;
        }
        ଯଦି (nextEqualsign == ଚାବି_ମୂଲ୍ୟ_Tail) {
            status = U_ILLEGAL_ARGUMENT_ERROR; /* empty key value in passed-in locale */
            ଫେରନ୍ତୁ 0;
        }

        rc = uprv_strcmp(canonKeywordName.data(), localeKeywordName.data());
        ଯଦି(rc == 0) {
            /* Current entry matches the input keyword. Update the entry */
            ଯଦି (!canon_Keyword_ମୂଲ୍ୟ.isEmpty()) { /* updating a value */
                updated_ଚାବିଗୁଡ଼ିକ_And_ମୂଲ୍ୟଗୁଡ଼ିକ.append(ଚାବି_ମୂଲ୍ୟ_Prefix, status);
                ଚାବି_ମୂଲ୍ୟ_Prefix = ';'; /* for any subsequent key-value pair */
                updated_ଚାବିଗୁଡ଼ିକ_And_ମୂଲ୍ୟଗୁଡ଼ିକ.append(canonKeywordName, status);
                updated_ଚାବିଗୁଡ଼ିକ_And_ମୂଲ୍ୟଗୁଡ଼ିକ.append('=', status);
                updated_ଚାବିଗୁଡ଼ିକ_And_ମୂଲ୍ୟଗୁଡ଼ିକ.append(canon_Keyword_ମୂଲ୍ୟ, status);
            } /* else removing this entry, don't emit anything */
            handled_ନିବେଶ_ଚାବି_And_ମୂଲ୍ୟ = ସତ୍ୟ;
        } ଅନ୍ୟଥା {
           /* input keyword sorts earlier than current entry, add before current entry */
            ଯଦି (rc < 0 && !canon_Keyword_ମୂଲ୍ୟ.isEmpty() && !handled_ନିବେଶ_ଚାବି_And_ମୂଲ୍ୟ) {
                /* insert new entry at this location */
                updated_ଚାବିଗୁଡ଼ିକ_And_ମୂଲ୍ୟଗୁଡ଼ିକ.append(ଚାବି_ମୂଲ୍ୟ_Prefix, status);
                ଚାବି_ମୂଲ୍ୟ_Prefix = ';'; /* for any subsequent key-value pair */
                updated_ଚାବିଗୁଡ଼ିକ_And_ମୂଲ୍ୟଗୁଡ଼ିକ.append(canonKeywordName, status);
                updated_ଚାବିଗୁଡ଼ିକ_And_ମୂଲ୍ୟଗୁଡ଼ିକ.append('=', status);
                updated_ଚାବିଗୁଡ଼ିକ_And_ମୂଲ୍ୟଗୁଡ଼ିକ.append(canon_Keyword_ମୂଲ୍ୟ, status);
                handled_ନିବେଶ_ଚାବି_And_ମୂଲ୍ୟ = ସତ୍ୟ;
            }
            /* copy the current entry */
            updated_ଚାବିଗୁଡ଼ିକ_And_ମୂଲ୍ୟଗୁଡ଼ିକ.append(ଚାବି_ମୂଲ୍ୟ_Prefix, status);
            ଚାବି_ମୂଲ୍ୟ_Prefix = ';'; /* for any subsequent key-value pair */
            updated_ଚାବିଗୁଡ଼ିକ_And_ମୂଲ୍ୟଗୁଡ଼ିକ.append(localeKeywordName, status);
            updated_ଚାବିଗୁଡ଼ିକ_And_ମୂଲ୍ୟଗୁଡ଼ିକ.append('=', status);
            updated_ଚାବିଗୁଡ଼ିକ_And_ମୂଲ୍ୟଗୁଡ଼ିକ.append(keywords.data() + nextEqualsign,
                                        static_cast<int32_t>(ଚାବି_ମୂଲ୍ୟ_Tail - nextEqualsign), status);
        }
        ଯଦି (nextSeparator == std::string_view::npos && !canon_Keyword_ମୂଲ୍ୟ.isEmpty() && !handled_ନିବେଶ_ଚାବି_And_ମୂଲ୍ୟ) {
            /* append new entry at the end, it sorts later than existing entries */
            updated_ଚାବିଗୁଡ଼ିକ_And_ମୂଲ୍ୟଗୁଡ଼ିକ.append(ଚାବି_ମୂଲ୍ୟ_Prefix, status);
            /* skip keyValuePrefix update, no subsequent key-value pair */
            updated_ଚାବିଗୁଡ଼ିକ_And_ମୂଲ୍ୟଗୁଡ଼ିକ.append(canonKeywordName, status);
            updated_ଚାବିଗୁଡ଼ିକ_And_ମୂଲ୍ୟଗୁଡ଼ିକ.append('=', status);
            updated_ଚାବିଗୁଡ଼ିକ_And_ମୂଲ୍ୟଗୁଡ଼ିକ.append(canon_Keyword_ମୂଲ୍ୟ, status);
            handled_ନିବେଶ_ଚାବି_And_ମୂଲ୍ୟ = ସତ୍ୟ;
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
    ଯଦି (!handled_ନିବେଶ_ଚାବି_And_ମୂଲ୍ୟ || U_FAILURE(status)) {
        /* if input key/value specified removal of a keyword not present in locale, or
         * there was an error in CharString.append, leave original locale alone. */
        U_ASSERT(status != U_STRING_NOT_TERMINATED_WARNING);
        // The sink is expected to be a buffer which already contains the full
        // locale string, so when it isn't going to be modified there's no need
        // to actually write any data to it, as the data is already there. Only
        // the first character needs to be overwritten (changing '\0' to '@').
        needLen = static_cast<int32_t>(keywords.size());
        int32_t capacity = 0;
        ଅକ୍ଷର* buffer = sink.GetAppendBuffer(
                needLen, needLen, nullptr, needLen, &capacity);
        ଯଦି (capacity < needLen || buffer == nullptr) {
            status = U_BUFFER_OVERFLOW_ERROR;
        } ଅନ୍ୟଥା {
            *buffer = '@';
            sink.Append(buffer, needLen);
        }
        ଫେରନ୍ତୁ needLen;
    }

    needLen = updated_ଚାବିଗୁଡ଼ିକ_And_ମୂଲ୍ୟଗୁଡ଼ିକ.length();
    // Check to see can we fit the updatedKeysAndValues, if not, return
    // U_BUFFER_OVERFLOW_ERROR without copy updatedKeysAndValues into it.
    // We do this because this API function does not behave like most others:
    // It promises never to set a U_STRING_NOT_TERMINATED_WARNING.
    // When the contents fits but without the terminating NUL, in this case we need to not change
    // the buffer contents and return with a buffer overflow error.
    ଯଦି (needLen > 0) {
        int32_t capacity = 0;
        ଅକ୍ଷର* buffer = sink.GetAppendBuffer(
                needLen, needLen, nullptr, needLen, &capacity);
        ଯଦି (capacity < needLen || buffer == nullptr) {
            status = U_BUFFER_OVERFLOW_ERROR;
            ଫେରନ୍ତୁ needLen;
        }
        uprv_memcpy(buffer, updated_ଚାବିଗୁଡ଼ିକ_And_ମୂଲ୍ୟଗୁଡ଼ିକ.data(), needLen);
        sink.Append(buffer, needLen);
    }
    U_ASSERT(status != U_STRING_NOT_TERMINATED_WARNING);
    ଫେରନ୍ତୁ needLen;
}

/* ### ID parsing implementation **************************************************/

ନାମାକାଶ {

ଧାଡ଼ିଭିତରେ ତାର୍କିକ _isPrefixLetter(ଅକ୍ଷର a) { ଫେରନ୍ତୁ a == 'x' || a == 'X' || a == 'i' || a == 'I'; }

/*returns true if one of the special prefixes is here (s=string)
  'x-' or 'i-' */
ଧାଡ଼ିଭିତରେ ତାର୍କିକ _isIDPrefix(std::string_view s) {
    ଫେରନ୍ତୁ s.size() >= 2 && _isPrefixLetter(s[0]) && _isIDSeparator(s[1]);
}

/* Dot terminates it because of POSIX form  where dot precedes the codepage
 * except for variant
 */
ଧାଡ଼ିଭିତରେ ତାର୍କିକ _isTerminator(ଅକ୍ଷର a) { ଫେରନ୍ତୁ a == '.' || a == '@'; }

ଧାଡ଼ିଭିତରେ ତାର୍କିକ _isBCP47Extension(std::string_view p) {
    ଫେରନ୍ତୁ p.size() >= 3 &&
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
std::optional<int16_t> _findIndex(ଅଚଳ ଅକ୍ଷର* ଅଚଳ* list, ଅଚଳ ଅକ୍ଷର* ଚାବି)
{
    ଅଚଳ ଅକ୍ଷର* ଅଚଳ* anchor = list;
    int32_t pass = 0;

    /* Make two passes through two nullptr-terminated arrays at 'list' */
    ଯେତେବେଳେ (pass++ < 2) {
        ଯେତେବେଳେ (*list) {
            ଯଦି (uprv_strcmp(ଚାବି, *list) == 0) {
                ଫେରନ୍ତୁ static_cast<int16_t>(list - anchor);
            }
            list++;
        }
        ++list;     /* skip final nullptr *CWB*/
    }
    ଫେରନ୍ତୁ std::nullopt;
}

}  // namespace

U_CFUNC ଅଚଳ ଅକ୍ଷର*
uloc_getCurrentCountryID(ଅଚଳ ଅକ୍ଷର* oldID U_LIFETIME_BOUND) {
    std::optional<int16_t> offset = _findIndex(DEPRECATED_COUNTRIES, oldID);
    ଫେରନ୍ତୁ offset.has_value() ? REPLACEMENT_COUNTRIES[*offset] : oldID;
}
U_CFUNC ଅଚଳ ଅକ୍ଷର*
uloc_getCurrentLanguageID(ଅଚଳ ଅକ୍ଷର* oldID U_LIFETIME_BOUND) {
    std::optional<int16_t> offset = _findIndex(DEPRECATED_LANGUAGES, oldID);
    ଫେରନ୍ତୁ offset.has_value() ? REPLACEMENT_LANGUAGES[*offset] : oldID;
}

ନାମାକାଶ {

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
    ଯଦି (localeID.size() == 4 && uprv_strnicmp(localeID.data(), "root", 4) == 0) {
        skip = 4;
        localeID.remove_prefix(skip);
    } ଅନ୍ୟଥା ଯଦି (localeID.size() >= 3 && uprv_strnicmp(localeID.data(), "und", 3) == 0 &&
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
    ଯେତେବେଳେ (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len])) {
        ଯଦି (len == MAXLEN) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ଫେରନ୍ତୁ 0;
        }
        len++;
    }

    ଯଦି (sink == nullptr || len == 0) { ଫେରନ୍ତୁ skip + len; }

    int32_t minCapacity = uprv_max(static_cast<int32_t>(len), 4);  // Minimum 3 letters plus NUL.
    ଅକ୍ଷର scratch[MAXLEN];
    int32_t capacity = 0;
    ଅକ୍ଷର* buffer = sink->GetAppendBuffer(
            minCapacity, minCapacity, scratch, UPRV_LENGTHOF(scratch), &capacity);

    ପାଇଁ (size_t i = 0; i < len; ++i) {
        buffer[i] = uprv_tolower(localeID[i]);
    }
    ଯଦି (localeID.size() >= 2 && _isIDSeparator(localeID[1])) {
        buffer[1] = '-';
    }

    ଯଦି (len == 3) {
        /* convert 3 character code to 2 character code if possible *CWB*/
        U_ASSERT(capacity >= 4);
        buffer[3] = '\0';
        std::optional<int16_t> offset = _findIndex(LANGUAGES_3, buffer);
        ଯଦି (offset.has_value()) {
            ଅଚଳ ଅକ୍ଷର* ଅଚଳ alias = LANGUAGES[*offset];
            sink->Append(alias, static_cast<int32_t>(uprv_strlen(alias)));
            ଫେରନ୍ତୁ skip + len;
        }
    }

    sink->Append(buffer, static_cast<int32_t>(len));
    ଫେରନ୍ତୁ skip + len;
}

size_t _getScript(std::string_view localeID, ByteSink* sink) {
    constexpr int32_t LENGTH = 4;

    size_t len = 0;
    ଯେତେବେଳେ (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len]) &&
            uprv_isASCIILetter(localeID[len])) {
        ଯଦି (len == LENGTH) { ଫେରନ୍ତୁ 0; }
        len++;
    }
    ଯଦି (len != LENGTH) { ଫେରନ୍ତୁ 0; }

    ଯଦି (sink == nullptr) { ଫେରନ୍ତୁ len; }

    ଅକ୍ଷର scratch[LENGTH];
    int32_t capacity = 0;
    ଅକ୍ଷର* buffer = sink->GetAppendBuffer(
            LENGTH, LENGTH, scratch, UPRV_LENGTHOF(scratch), &capacity);

    buffer[0] = uprv_toupper(localeID[0]);
    ପାଇଁ (int32_t i = 1; i < LENGTH; ++i) {
        buffer[i] = uprv_tolower(localeID[i]);
    }

    sink->Append(buffer, LENGTH);
    ଫେରନ୍ତୁ len;
}

size_t _getRegion(std::string_view localeID, ByteSink* sink) {
    constexpr int32_t MINLEN = 2;
    constexpr int32_t MAXLEN = ULOC_COUNTRY_CAPACITY - 1;  // Minus NUL.

    size_t len = 0;
    ଯେତେବେଳେ (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len])) {
        ଯଦି (len == MAXLEN) { ଫେରନ୍ତୁ 0; }
        len++;
    }
    ଯଦି (len < MINLEN) { ଫେରନ୍ତୁ 0; }

    ଯଦି (sink == nullptr) { ଫେରନ୍ତୁ len; }

    ଅକ୍ଷର scratch[ULOC_COUNTRY_CAPACITY];
    int32_t capacity = 0;
    ଅକ୍ଷର* buffer = sink->GetAppendBuffer(
            ULOC_COUNTRY_CAPACITY,
            ULOC_COUNTRY_CAPACITY,
            scratch,
            UPRV_LENGTHOF(scratch),
            &capacity);

    ପାଇଁ (size_t i = 0; i < len; ++i) {
        buffer[i] = uprv_toupper(localeID[i]);
    }

    ଯଦି (len == 3) {
        /* convert 3 character code to 2 character code if possible *CWB*/
        U_ASSERT(capacity >= 4);
        buffer[3] = '\0';
        std::optional<int16_t> offset = _findIndex(COUNTRIES_3, buffer);
        ଯଦି (offset.has_value()) {
            ଅଚଳ ଅକ୍ଷର* ଅଚଳ alias = COUNTRIES[*offset];
            sink->Append(alias, static_cast<int32_t>(uprv_strlen(alias)));
            ଫେରନ୍ତୁ len;
        }
    }

    sink->Append(buffer, static_cast<int32_t>(len));
    ଫେରନ୍ତୁ len;
}

/**
 * @param needSeparator if true, then add leading '_' if any variants
 * are added to 'variant'
 */
size_t
_getVariant(std::string_view localeID,
            ଅକ୍ଷର prev,
            ByteSink* sink,
            ତାର୍କିକ needSeparator,
            UErrorCode& status) {
    ଯଦି (U_FAILURE(status) || localeID.empty()) ଫେରନ୍ତୁ 0;

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
    ଯଦି (_isIDSeparator(prev)) {
        /* get a variant string after a '-' or '_' */
        ପାଇଁ (std::string_view sub = localeID;;) {
            size_t next = sub.find_first_of(".@_-");
            // For historical reasons, a trailing separator is included in the variant.
            ତାର୍କିକ finished = next == std::string_view::npos || next + 1 == sub.length();
            size_t limit = finished ? sub.length() : next;
            index += limit;
            ଯଦି (index > MAX_VARIANTS_LENGTH) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
                ଫେରନ୍ତୁ 0;
            }

            ଯଦି (sink != nullptr) {
                ଯଦି (needSeparator) {
                    sink->Append("_", 1);
                } ଅନ୍ୟଥା {
                    needSeparator = ସତ୍ୟ;
                }

                int32_t length = static_cast<int32_t>(limit);
                int32_t minCapacity = uprv_min(length, MAX_VARIANTS_LENGTH);
                ଅକ୍ଷର scratch[MAX_VARIANTS_LENGTH];
                int32_t capacity = 0;
                ଅକ୍ଷର* buffer = sink->GetAppendBuffer(
                        minCapacity, minCapacity, scratch, UPRV_LENGTHOF(scratch), &capacity);

                ପାଇଁ (size_t i = 0; i < limit; ++i) {
                    buffer[i] = uprv_toupper(sub[i]);
                }
                sink->Append(buffer, length);
            }

            ଯଦି (finished) { ଫେରନ୍ତୁ index; }
            sub.remove_prefix(next);
            ଯଦି (_isTerminator(sub.front()) || _isBCP47Extension(sub)) { ଫେରନ୍ତୁ index; }
            sub.remove_prefix(1);
            index++;
        }
    }

    size_t skip = 0;
    /* if there is no variant tag after a '-' or '_' then look for '@' */
    ଯଦି (prev == '@') {
        /* keep localeID */
    } ଅନ୍ୟଥା ଯଦି (ଅଚଳ ଅକ୍ଷର* p = locale_getKeywordsStart(localeID); p != nullptr) {
        skip = 1 + p - localeID.data(); /* point after the '@' */
        localeID.remove_prefix(skip);
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ 0;
    }
    ପାଇଁ (; index < localeID.size() && !_isTerminator(localeID[index]); index++) {
        ଯଦି (index >= MAX_VARIANTS_LENGTH) { // same as length > MAX_VARIANTS_LENGTH
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ଫେରନ୍ତୁ 0;
        }
        ଯଦି (needSeparator) {
            ଯଦି (sink != nullptr) {
                sink->Append("_", 1);
            }
            needSeparator = ମିଥ୍ୟା;
        }
        ଯଦି (sink != nullptr) {
            ଅକ୍ଷର c = uprv_toupper(localeID[index]);
            ଯଦି (c == '-' || c == ',') c = '_';
            sink->Append(&c, 1);
        }
    }
    ଫେରନ୍ତୁ skip + index;
}

}  // namespace

U_EXPORT CharString
ulocimp_getLanguage(std::string_view localeID, UErrorCode& status) {
    ଫେରନ୍ତୁ ByteSinkUtil::viaByteSinkToCharString(
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
    ଫେରନ୍ତୁ ByteSinkUtil::viaByteSinkToCharString(
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
    ଫେରନ୍ତୁ ByteSinkUtil::viaByteSinkToCharString(
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
    ଫେରନ୍ତୁ ByteSinkUtil::viaByteSinkToCharString(
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

U_EXPORT ଶୂନ୍ୟ
ulocimp_getSubtags(
        std::string_view localeID,
        CharString* language,
        CharString* script,
        CharString* region,
        CharString* variant,
        ଅଚଳ ଅକ୍ଷର** pEnd,
        UErrorCode& status) {
    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }

    std::optional<CharStringByteSink> languageSink;
    std::optional<CharStringByteSink> scriptSink;
    std::optional<CharStringByteSink> regionSink;
    std::optional<CharStringByteSink> variantSink;

    ଯଦି (language != nullptr) { languageSink.emplace(language); }
    ଯଦି (script != nullptr) { scriptSink.emplace(script); }
    ଯଦି (region != nullptr) { regionSink.emplace(region); }
    ଯଦି (variant != nullptr) { variantSink.emplace(variant); }

    ulocimp_getSubtags(
            localeID,
            languageSink.has_value() ? &*languageSink : nullptr,
            scriptSink.has_value() ? &*scriptSink : nullptr,
            regionSink.has_value() ? &*regionSink : nullptr,
            variantSink.has_value() ? &*variantSink : nullptr,
            pEnd,
            status);
}

U_EXPORT ଶୂନ୍ୟ
ulocimp_getSubtags(
        std::string_view localeID,
        ByteSink* language,
        ByteSink* script,
        ByteSink* region,
        ByteSink* variant,
        ଅଚଳ ଅକ୍ଷର** pEnd,
        UErrorCode& status) {
    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }

    ଯଦି (pEnd != nullptr) {
        *pEnd = localeID.data();
    } ଅନ୍ୟଥା ଯଦି (language == nullptr &&
               script == nullptr &&
               region == nullptr &&
               variant == nullptr) {
        ଫେରନ୍ତୁ;
    }

    ଯଦି (localeID.empty()) { ଫେରନ୍ତୁ; }

    ତାର୍କିକ hasRegion = ମିଥ୍ୟା;

    {
        size_t len = _getLanguage(localeID, language, status);
        ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }
        ଯଦି (len > 0) {
            localeID.remove_prefix(len);
        }
    }

    ଯଦି (pEnd != nullptr) {
        *pEnd = localeID.data();
    } ଅନ୍ୟଥା ଯଦି (script == nullptr &&
               region == nullptr &&
               variant == nullptr) {
        ଫେରନ୍ତୁ;
    }

    ଯଦି (localeID.empty()) { ଫେରନ୍ତୁ; }

    ଯଦି (_isIDSeparator(localeID.front())) {
        std::string_view sub = localeID;
        sub.remove_prefix(1);
        size_t len = _getScript(sub, script);
        ଯଦି (len > 0) {
            localeID.remove_prefix(len + 1);
            ଯଦି (pEnd != nullptr) { *pEnd = localeID.data(); }
        }
    }

    ଯଦି ((region == nullptr && variant == nullptr && pEnd == nullptr) || localeID.empty()) { ଫେରନ୍ତୁ; }

    ଯଦି (_isIDSeparator(localeID.front())) {
        std::string_view sub = localeID;
        sub.remove_prefix(1);
        size_t len = _getRegion(sub, region);
        ଯଦି (len > 0) {
            hasRegion = ସତ୍ୟ;
            localeID.remove_prefix(len + 1);
            ଯଦି (pEnd != nullptr) { *pEnd = localeID.data(); }
        }
    }

    ଯଦି ((variant == nullptr && pEnd == nullptr) || localeID.empty()) { ଫେରନ୍ତୁ; }

    ତାର୍କିକ hasVariant = ମିଥ୍ୟା;

    ଯଦି (_isIDSeparator(localeID.front()) && !_isBCP47Extension(localeID)) {
        std::string_view sub = localeID;
        /* If there was no country ID, skip a possible extra IDSeparator */
        size_t skip = !hasRegion && localeID.size() > 1 && _isIDSeparator(localeID[1]) ? 2 : 1;
        sub.remove_prefix(skip);
        size_t len = _getVariant(sub, localeID[0], variant, ମିଥ୍ୟା, status);
        ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }
        ଯଦି (len > 0) {
            hasVariant = ସତ୍ୟ;
            localeID.remove_prefix(skip + len);
            ଯଦି (pEnd != nullptr) { *pEnd = localeID.data(); }
        }
    }

    ଯଦି ((variant == nullptr && pEnd == nullptr) || localeID.empty()) { ଫେରନ୍ତୁ; }

    ଯଦି (_isBCP47Extension(localeID)) {
        localeID.remove_prefix(2);
        constexpr ଅକ୍ଷର vaposix[] = "-va-posix";
        constexpr size_t length = ଆକାର vaposix - 1;
        ପାଇଁ (size_t next;; localeID.remove_prefix(next)) {
            next = localeID.find('-', 1);
            ଯଦି (next == std::string_view::npos) { ଭାଙ୍ଗ; }
            next = localeID.find('-', next + 1);
            ତାର୍କିକ finished = next == std::string_view::npos;
            std::string_view sub = localeID;
            ଯଦି (!finished) { sub.remove_suffix(sub.length() - next); }

            ଯଦି (sub.length() == length && uprv_strnicmp(sub.data(), vaposix, length) == 0) {
                ଯଦି (variant != nullptr) {
                    ଯଦି (hasVariant) { variant->Append("_", 1); }
                    constexpr ଅକ୍ଷର posix[] = "POSIX";
                    variant->Append(posix, ଆକାର posix - 1);
                }
                ଯଦି (pEnd != nullptr) { *pEnd = localeID.data() + length; }
            }

            ଯଦି (finished) { ଭାଙ୍ଗ; }
        }
    }
}

/* Keyword enumeration */

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା UKeywordsContext {
    ଅକ୍ଷର* keywords;
    ଅକ୍ଷର* current;
} UKeywordsContext;

U_CDECL_BEGIN

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
uloc_kw_closeKeywords(UEnumeration *enumerator) {
    uprv_free(((UKeywordsContext *)enumerator->context)->keywords);
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

ସ୍ଥିର int32_t U_CALLCONV
uloc_kw_ଗଣନା_Keywords(UEnumeration *en, UErrorCode * /*status*/) {
    ଅକ୍ଷର *kw = ((UKeywordsContext *)en->context)->keywords;
    int32_t result = 0;
    ଯେତେବେଳେ(*kw) {
        result++;
        kw += uprv_strlen(kw)+1;
    }
    ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * U_CALLCONV
uloc_kw_nextKeyword(UEnumeration* en,
                    int32_t* resultLength,
                    UErrorCode* /*status*/) {
    ଅଚଳ ଅକ୍ଷର* result = ((UKeywordsContext *)en->context)->current;
    int32_t len = 0;
    ଯଦି(*result) {
        len = (int32_t)uprv_strlen(((UKeywordsContext *)en->context)->current);
        ((UKeywordsContext *)en->context)->current += len+1;
    } ଅନ୍ୟଥା {
        result = nullptr;
    }
    ଯଦି (resultLength) {
        *resultLength = len;
    }
    ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
uloc_kw_resetKeywords(UEnumeration* en,
                      UErrorCode* /*status*/) {
    ((UKeywordsContext *)en->context)->current = ((UKeywordsContext *)en->context)->keywords;
}

U_CDECL_END


ସ୍ଥିର ଅଚଳ UEnumeration gKeywordsEnum = {
    nullptr,
    nullptr,
    uloc_kw_closeKeywords,
    uloc_kw_ଗଣନା_Keywords,
    uenum_unextDefault,
    uloc_kw_nextKeyword,
    uloc_kw_resetKeywords
};

U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywordList(ଅଚଳ ଅକ୍ଷର *keywordList, int32_t keywordListSize, UErrorCode* status)
{
    ଯଦି (U_FAILURE(*status)) { ଫେରନ୍ତୁ nullptr; }

    LocalMemory<UKeywordsContext> myContext;
    LocalMemory<UEnumeration> result;

    myContext.adoptInstead(static_cast<UKeywordsContext *>(uprv_malloc(ଆକାର(UKeywordsContext))));
    result.adoptInstead(static_cast<UEnumeration *>(uprv_malloc(ଆକାର(UEnumeration))));
    ଯଦି (myContext.isNull() || result.isNull()) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    uprv_memcpy(result.getAlias(), &gKeywordsEnum, ଆକାର(UEnumeration));
    myContext->keywords = static_cast<ଅକ୍ଷର *>(uprv_malloc(keywordListSize+1));
    ଯଦି (myContext->keywords == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    uprv_memcpy(myContext->keywords, keywordList, keywordListSize);
    myContext->keywords[keywordListSize] = 0;
    myContext->current = myContext->keywords;
    result->context = myContext.orphan();
    ଫେରନ୍ତୁ result.orphan();
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywords(ଅଚଳ ଅକ୍ଷର* localeID,
                        UErrorCode* status)
{
    ଯଦି(status==nullptr || U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }

    CharString tempBuffer;
    ଅଚଳ ଅକ୍ଷର* tmpLocaleID;

    ଯଦି (localeID != nullptr && _hasBCP47Extension(localeID)) {
        tempBuffer = ulocimp_forLanguageTag(localeID, -1, nullptr, *status);
        tmpLocaleID = U_SUCCESS(*status) && !tempBuffer.isEmpty() ? tempBuffer.data() : localeID;
    } ଅନ୍ୟଥା {
        ଯଦି (localeID==nullptr) {
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
    ଯଦି (U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }

    /* keywords are located after '@' */
    ଯଦି((tmpLocaleID = locale_getKeywordsStart(tmpLocaleID)) != nullptr) {
        CharString keywords = ulocimp_getKeywords(tmpLocaleID + 1, '@', ମିଥ୍ୟା, *status);
        ଯଦି (U_FAILURE(*status)) {
            ଫେରନ୍ତୁ nullptr;
        }
        ଫେରନ୍ତୁ uloc_openKeywordList(keywords.data(), keywords.length(), status);
    }
    ଫେରନ୍ତୁ nullptr;
}


/* bit-flags for 'options' parameter of _canonicalize */
#define _ULOC_STRIP_KEYWORDS 0x2
#define _ULOC_CANONICALIZE   0x1

ନାମାକାଶ {

ଧାଡ଼ିଭିତରେ ତାର୍କିକ OPTION_SET(uint32_t options, uint32_t mask) { ଫେରନ୍ତୁ (options & mask) != 0; }

constexpr ଅକ୍ଷର i_default[] = {'i', '-', 'd', 'e', 'f', 'a', 'u', 'l', 't'};
constexpr int32_t I_DEFAULT_LENGTH = UPRV_LENGTHOF(i_default);

/**
 * Canonicalize the given localeID, to level 1 or to level 2,
 * depending on the options.  To specify level 1, pass in options=0.
 * To specify level 2, pass in options=_ULOC_CANONICALIZE.
 *
 * This is the code underlying uloc_getName and uloc_canonicalize.
 */
ଶୂନ୍ୟ
_canonicalize(std::string_view localeID,
              ByteSink& sink,
              uint32_t options,
              UErrorCode& err) {
    ଯଦି (U_FAILURE(err)) {
        ଫେରନ୍ତୁ;
    }

    int32_t j, field_ଗଣନା=0;
    CharString tempBuffer;  // if localeID has a BCP47 extension, tmpLocaleID points to this
    CharString localeIDWithHyphens;  // if localeID has a BPC47 extension and have _, tmpLocaleID points to this
    std::string_view origLocaleID;
    std::string_view tmpLocaleID;
    size_t keywordAssign = std::string_view::npos;
    size_t separatorIndicator = std::string_view::npos;

    ଯଦି (_hasBCP47Extension(localeID)) {
        std::string_view localeIDPtr = localeID;

        // convert all underbars to hyphens, unless the "BCP47 extension" comes at the beginning of the string
        ଯଦି (localeID.size() >= 2 && localeID.find('_') != std::string_view::npos && localeID[1] != '-' && localeID[1] != '_') {
            localeIDWithHyphens.append(localeID, err);
            ଯଦି (U_SUCCESS(err)) {
                ପାଇଁ (ଅକ୍ଷର* p = localeIDWithHyphens.data(); *p != '\0'; ++p) {
                    ଯଦି (*p == '_') {
                        *p = '-';
                    }
                }
                localeIDPtr = localeIDWithHyphens.toStringPiece();
            }
        }

        tempBuffer = ulocimp_forLanguageTag(localeIDPtr.data(), static_cast<int32_t>(localeIDPtr.size()), nullptr, err);
        tmpLocaleID = U_SUCCESS(err) && !tempBuffer.isEmpty() ? static_cast<std::string_view>(tempBuffer.toStringPiece()) : localeIDPtr;
    } ଅନ୍ୟଥା {
        tmpLocaleID=localeID;
    }

    origLocaleID=tmpLocaleID;

    /* get all pieces, one after another, and separate with '_' */
    CharString tag;
    CharString script;
    CharString country;
    CharString variant;
    ଅଚଳ ଅକ୍ଷର* end = nullptr;
    ulocimp_getSubtags(
            tmpLocaleID,
            &tag,
            &script,
            &country,
            &variant,
            &end,
            err);
    ଯଦି (U_FAILURE(err)) {
        ଫେରନ୍ତୁ;
    }
    U_ASSERT(end != nullptr);
    ଯଦି (end > tmpLocaleID.data()) {
        tmpLocaleID.remove_prefix(end - tmpLocaleID.data());
    }

    ଯଦି (tag.length() == I_DEFAULT_LENGTH && origLocaleID.length() >= I_DEFAULT_LENGTH &&
            uprv_strncmp(origLocaleID.data(), i_default, I_DEFAULT_LENGTH) == 0) {
        tag.clear();
        tag.append(uloc_getDefault(), err);
    } ଅନ୍ୟଥା {
        ଯଦି (!script.isEmpty()) {
            ++field_ଗଣନା;
            tag.append('_', err);
            tag.append(script, err);
        }
        ଯଦି (!country.isEmpty()) {
            ++field_ଗଣନା;
            tag.append('_', err);
            tag.append(country, err);
        }
        ଯଦି (!variant.isEmpty()) {
            ++field_ଗଣନା;
            ଯଦି (country.isEmpty()) {
                tag.append('_', err);
            }
            tag.append('_', err);
            tag.append(variant, err);
        }
    }

    /* Copy POSIX-style charset specifier, if any [mr.utf8] */
    ଯଦି (!OPTION_SET(options, _ULOC_CANONICALIZE) && !tmpLocaleID.empty() && tmpLocaleID.front() == '.') {
        tag.append('.', err);
        tmpLocaleID.remove_prefix(1);
        size_t length;
        ଯଦି (size_t atPos = tmpLocaleID.find('@'); atPos != std::string_view::npos) {
            length = atPos;
        } ଅନ୍ୟଥା {
            length = tmpLocaleID.length();
        }
        // The longest charset name we found in IANA charset registry
        // https://www.iana.org/assignments/character-sets/ is
        // "Extended_UNIX_Code_Packed_Format_for_Japanese" in length 45.
        // we therefore restrict the length here to be 64 which is a power of 2
        // number that is longer than 45.
        constexpr size_t kMaxCharsetLength = 64;
        ଯଦି (length > kMaxCharsetLength) {
           err = U_ILLEGAL_ARGUMENT_ERROR; /* malformed keyword name */
           ଫେରନ୍ତୁ;
        }
        ଯଦି (length > 0) {
            tag.append(tmpLocaleID.data(), static_cast<int32_t>(length), err);
            tmpLocaleID.remove_prefix(length);
        }
    }

    /* Scan ahead to next '@' and determine if it is followed by '=' and/or ';'
       After this, tmpLocaleID either starts at '@' or is empty. */
    ଯଦି (ଅଚଳ ଅକ୍ଷର* start = locale_getKeywordsStart(tmpLocaleID); start != nullptr) {
        ଯଦି (start > tmpLocaleID.data()) {
            tmpLocaleID.remove_prefix(start - tmpLocaleID.data());
        }
        keywordAssign = tmpLocaleID.find('=');
        separatorIndicator = tmpLocaleID.find(';');
    } ଅନ୍ୟଥା {
        tmpLocaleID = {};
    }

    /* Copy POSIX-style variant, if any [mr@FOO] */
    ଯଦି (!OPTION_SET(options, _ULOC_CANONICALIZE) &&
        !tmpLocaleID.empty() && keywordAssign == std::string_view::npos) {
        tag.append(tmpLocaleID, err);
        tmpLocaleID = {};
    }

    ଯଦି (OPTION_SET(options, _ULOC_CANONICALIZE)) {
        /* Handle @FOO variant if @ is present and not followed by = */
        ଯଦି (!tmpLocaleID.empty() && keywordAssign == std::string_view::npos) {
            /* Add missing '_' if needed */
            ଯଦି (field_ଗଣନା < 2 || (field_ଗଣନା < 3 && !script.isEmpty())) {
                କର {
                    tag.append('_', err);
                    ++field_ଗଣନା;
                } ଯେତେବେଳେ(field_ଗଣନା<2);
            }

            CharStringByteSink s(&tag);
            std::string_view sub = tmpLocaleID;
            sub.remove_prefix(1);
            _getVariant(sub, '@', &s, !variant.isEmpty(), err);
            ଯଦି (U_FAILURE(err)) { ଫେରନ୍ତୁ; }
        }

        /* Look up the ID in the canonicalization map */
        ପାଇଁ (j=0; j<UPRV_LENGTHOF(CANONICALIZE_MAP); j++) {
            StringPiece id(CANONICALIZE_MAP[j].id);
            ଯଦି (tag == id) {
                ଯଦି (id.empty() && !tmpLocaleID.empty()) {
                    ଭାଙ୍ଗ; /* Don't remap "" if keywords present */
                }
                tag.clear();
                tag.append(CANONICALIZE_MAP[j].canonicalID, err);
                ଭାଙ୍ଗ;
            }
        }
    }

    sink.Append(tag.data(), tag.length());

    ଯଦି (!OPTION_SET(options, _ULOC_STRIP_KEYWORDS)) {
        ଯଦି (!tmpLocaleID.empty() && keywordAssign != std::string_view::npos &&
            (separatorIndicator == std::string_view::npos || separatorIndicator > keywordAssign)) {
            sink.Append("@", 1);
            ++field_ଗଣନା;
            tmpLocaleID.remove_prefix(1);
            ulocimp_getKeywords(tmpLocaleID, '@', sink, ସତ୍ୟ, err);
        }
    }
}

}  // namespace

/* ### ID parsing API **************************************************/

U_CAPI int32_t  U_EXPORT2
uloc_getParent(ଅଚଳ ଅକ୍ଷର*    localeID,
               ଅକ୍ଷର* parent,
               int32_t parentCapacity,
               UErrorCode* err)
{
    ଫେରନ୍ତୁ ByteSinkUtil::viaByteSinkToTerminatedChars(
        parent, parentCapacity,
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getParent(localeID, sink, status);
        },
        *err);
}

U_EXPORT CharString
ulocimp_getParent(ଅଚଳ ଅକ୍ଷର* localeID,
                  UErrorCode& err)
{
    ଫେରନ୍ତୁ ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getParent(localeID, sink, status);
        },
        err);
}

U_EXPORT ଶୂନ୍ୟ
ulocimp_getParent(ଅଚଳ ଅକ୍ଷର* localeID,
                  icu::ByteSink& sink,
                  UErrorCode& err)
{
    ଯଦି (U_FAILURE(err)) { ଫେରନ୍ତୁ; }

    ଅଚଳ ଅକ୍ଷର *lastUnderscore;
    int32_t i;

    ଯଦି (localeID == nullptr)
        localeID = uloc_getDefault();

    lastUnderscore=uprv_strrchr(localeID, '_');
    ଯଦି(lastUnderscore!=nullptr) {
        i = static_cast<int32_t>(lastUnderscore - localeID);
    } ଅନ୍ୟଥା {
        i=0;
    }

    ଯଦି (i > 0) {
        ଯଦି (uprv_strnicmp(localeID, "und_", 4) == 0) {
            localeID += 3;
            i -= 3;
        }
        sink.Append(localeID, i);
    }
}

U_CAPI int32_t U_EXPORT2
uloc_getLanguage(ଅଚଳ ଅକ୍ଷର*    localeID,
         ଅକ୍ଷର* language,
         int32_t languageCapacity,
         UErrorCode* err)
{
    ଯଦି (localeID == nullptr) {
        localeID = uloc_getDefault();
    }

    /* uloc_getLanguage will return a 2 character iso-639 code if one exists. *CWB*/
    ଫେରନ୍ତୁ ByteSinkUtil::viaByteSinkToTerminatedChars(
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
uloc_getScript(ଅଚଳ ଅକ୍ଷର*    localeID,
         ଅକ୍ଷର* script,
         int32_t scriptCapacity,
         UErrorCode* err)
{
    ଯଦି (localeID == nullptr) {
        localeID = uloc_getDefault();
    }

    ଫେରନ୍ତୁ ByteSinkUtil::viaByteSinkToTerminatedChars(
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
uloc_getCountry(ଅଚଳ ଅକ୍ଷର* localeID,
            ଅକ୍ଷର* country,
            int32_t countryCapacity,
            UErrorCode* err)
{
    ଯଦି (localeID == nullptr) {
        localeID = uloc_getDefault();
    }

    ଫେରନ୍ତୁ ByteSinkUtil::viaByteSinkToTerminatedChars(
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
uloc_getVariant(ଅଚଳ ଅକ୍ଷର* localeID,
                ଅକ୍ଷର* variant,
                int32_t variantCapacity,
                UErrorCode* err)
{
    ଯଦି (localeID == nullptr) {
        localeID = uloc_getDefault();
    }

    ଫେରନ୍ତୁ ByteSinkUtil::viaByteSinkToTerminatedChars(
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
uloc_getName(ଅଚଳ ଅକ୍ଷର* localeID,
             ଅକ୍ଷର* name,
             int32_t nameCapacity,
             UErrorCode* err)
{
    ଯଦି (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    ଫେରନ୍ତୁ ByteSinkUtil::viaByteSinkToTerminatedChars(
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
    ଫେରନ୍ତୁ ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getName(localeID, sink, status);
        },
        err);
}

U_EXPORT ଶୂନ୍ୟ
ulocimp_getName(std::string_view localeID,
                ByteSink& sink,
                UErrorCode& err)
{
    _canonicalize(localeID, sink, 0, err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getBaseName(ଅଚଳ ଅକ୍ଷର* localeID,
                 ଅକ୍ଷର* name,
                 int32_t nameCapacity,
                 UErrorCode* err)
{
    ଯଦି (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    ଫେରନ୍ତୁ ByteSinkUtil::viaByteSinkToTerminatedChars(
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
    ଫେରନ୍ତୁ ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_getBaseName(localeID, sink, status);
        },
        err);
}

U_EXPORT ଶୂନ୍ୟ
ulocimp_getBaseName(std::string_view localeID,
                    ByteSink& sink,
                    UErrorCode& err)
{
    _canonicalize(localeID, sink, _ULOC_STRIP_KEYWORDS, err);
}

U_CAPI int32_t  U_EXPORT2
uloc_canonicalize(ଅଚଳ ଅକ୍ଷର* localeID,
                  ଅକ୍ଷର* name,
                  int32_t nameCapacity,
                  UErrorCode* err)
{
    ଯଦି (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    ଫେରନ୍ତୁ ByteSinkUtil::viaByteSinkToTerminatedChars(
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
    ଫେରନ୍ତୁ ByteSinkUtil::viaByteSinkToCharString(
        [&](ByteSink& sink, UErrorCode& status) {
            ulocimp_canonicalize(localeID, sink, status);
        },
        err);
}

U_EXPORT ଶୂନ୍ୟ
ulocimp_canonicalize(std::string_view localeID,
                     ByteSink& sink,
                     UErrorCode& err)
{
    _canonicalize(localeID, sink, _ULOC_CANONICALIZE, err);
}

U_CAPI ଅଚଳ ଅକ୍ଷର*  U_EXPORT2
uloc_getISO3Language(ଅଚଳ ଅକ୍ଷର* localeID)
{
    UErrorCode err = U_ZERO_ERROR;

    ଯଦି (localeID == nullptr)
    {
        localeID = uloc_getDefault();
    }
    CharString lang = ulocimp_getLanguage(localeID, err);
    ଯଦି (U_FAILURE(err))
        ଫେରନ୍ତୁ "";
    std::optional<int16_t> offset = _findIndex(LANGUAGES, lang.data());
    ଫେରନ୍ତୁ offset.has_value() ? LANGUAGES_3[*offset] : "";
}

U_CAPI ଅଚଳ ଅକ୍ଷର*  U_EXPORT2
uloc_getISO3Country(ଅଚଳ ଅକ୍ଷର* localeID)
{
    UErrorCode err = U_ZERO_ERROR;

    ଯଦି (localeID == nullptr)
    {
        localeID = uloc_getDefault();
    }
    CharString cntry = ulocimp_getRegion(localeID, err);
    ଯଦି (U_FAILURE(err))
        ଫେରନ୍ତୁ "";
    std::optional<int16_t> offset = _findIndex(COUNTRIES, cntry.data());
    ଫେରନ୍ତୁ offset.has_value() ? COUNTRIES_3[*offset] : "";
}

U_CAPI uint32_t  U_EXPORT2
uloc_getLCID(ଅଚଳ ଅକ୍ଷର* localeID)
{
    UErrorCode status = U_ZERO_ERROR;
    uint32_t   lcid = 0;

    /* Check for incomplete id. */
    ଯଦି (!localeID || uprv_strlen(localeID) < 2) {
        ଫେରନ୍ତୁ 0;
    }

    // First, attempt Windows platform lookup if available, but fall
    // through to catch any special cases (ICU vs Windows name differences).
    lcid = uprv_convertToLCIDPlatform(localeID, &status);
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି (lcid > 0) {
        // Windows found an LCID, return that
        ଫେରନ୍ତୁ lcid;
    }

    CharString langID = ulocimp_getLanguage(localeID, status);
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ 0;
    }

    ଯଦି (uprv_strchr(localeID, '@')) {
        // uprv_convertToLCID does not support keywords other than collation.
        // Remove all keywords except collation.
        CharString collVal = ulocimp_get_Keyword_ମୂଲ୍ୟ(localeID, "collation", status);
        ଯଦି (U_SUCCESS(status) && !collVal.isEmpty()) {
            CharString tmpLocaleID = ulocimp_getBaseName(localeID, status);
            ulocimp_set_Keyword_ମୂଲ୍ୟ("collation", collVal.toStringPiece(), tmpLocaleID, status);
            ଯଦି (U_SUCCESS(status)) {
                ଫେରନ୍ତୁ uprv_convertToLCID(langID.data(), tmpLocaleID.data(), &status);
            }
        }

        // fall through - all keywords are simply ignored
        status = U_ZERO_ERROR;
    }

    ଫେରନ୍ତୁ uprv_convertToLCID(langID.data(), localeID, &status);
}

U_CAPI int32_t U_EXPORT2
uloc_getLocaleForLCID(uint32_t hostid, ଅକ୍ଷର *locale, int32_t localeCapacity,
                UErrorCode *status)
{
    ଫେରନ୍ତୁ uprv_convertToPosix(hostid, locale, localeCapacity, status);
}

/* ### Default locale **************************************************/

U_CAPI ଅଚଳ ଅକ୍ଷର*  U_EXPORT2
uloc_getDefault()
{
    ଫେରନ୍ତୁ locale_get_default();
}

U_CAPI ଶୂନ୍ୟ  U_EXPORT2
uloc_setDefault(ଅଚଳ ଅକ୍ଷର*   newDefaultLocale,
             UErrorCode* err)
{
    ଯଦି (U_FAILURE(*err))
        ଫେରନ୍ତୁ;
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
U_CAPI ଅଚଳ ଅକ୍ଷର* ଅଚଳ*  U_EXPORT2
uloc_getISOLanguages()
{
    ଫେରନ୍ତୁ LANGUAGES;
}

/**
 * Returns a list of all 2-letter country codes defined in ISO 639.  This is a
 * pointer to an array of pointers to arrays of char.  All of these pointers are
 * owned by ICU-- do not delete them, and do not write through them.  The array is
 * terminated with a null pointer.
 */
U_CAPI ଅଚଳ ଅକ୍ଷର* ଅଚଳ*  U_EXPORT2
uloc_getISOCountries()
{
    ଫେରନ୍ତୁ COUNTRIES;
}

U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
uloc_to_Unicode_Locale_ଚାବି(ଅଚଳ ଅକ୍ଷର* keyword U_LIFETIME_BOUND)
{
    ଯଦି (keyword == nullptr || *keyword == '\0') { ଫେରନ୍ତୁ nullptr; }
    std::optional<std::string_view> result = ulocimp_to_Bcp_ଚାବି_With_Fallback(keyword);
    ଫେରନ୍ତୁ result.has_value() ? result->data() : nullptr;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_to_Bcp_ଚାବି_With_Fallback(std::string_view keyword U_LIFETIME_BOUND)
{
    std::optional<std::string_view> bcp_ଚାବି = ulocimp_to_Bcp_ଚାବି(keyword);
    ଯଦି (!bcp_ଚାବି.has_value() &&
        ultag_is_Unicode_Locale_ଚାବି(keyword.data(), static_cast<int32_t>(keyword.size()))) {
        // unknown keyword, but syntax is fine..
        ଫେରନ୍ତୁ keyword;
    }
    ଫେରନ୍ତୁ bcp_ଚାବି;
}

U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
uloc_toUnicodeLocaleType(ଅଚଳ ଅକ୍ଷର* keyword, ଅଚଳ ଅକ୍ଷର* value U_LIFETIME_BOUND)
{
    ଯଦି (keyword == nullptr || *keyword == '\0' ||
        value == nullptr || *value == '\0') { ଫେରନ୍ତୁ nullptr; }
    std::optional<std::string_view> result = ulocimp_toBcpTypeWithFallback(keyword, value);
    ଫେରନ୍ତୁ result.has_value() ? result->data() : nullptr;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpTypeWithFallback(std::string_view keyword, std::string_view value U_LIFETIME_BOUND)
{
    std::optional<std::string_view> bcpType = ulocimp_toBcpType(keyword, value);
    ଯଦି (!bcpType.has_value() &&
        ultag_isUnicodeLocaleType(value.data(), static_cast<int32_t>(value.size()))) {
        // unknown keyword, but syntax is fine..
        ଫେରନ୍ତୁ value;
    }
    ଫେରନ୍ତୁ bcpType;
}

ନାମାକାଶ {

ତାର୍କିକ
is_Well_Formed_Legacy_ଚାବି(std::string_view ଚାବି)
{
    ଫେରନ୍ତୁ std::all_of(ଚାବି.begin(), ଚାବି.end(), UPRV_ISALPHANUM);
}

ତାର୍କିକ
isWellFormedLegacyType(std::string_view legacyType)
{
    int32_t alpha_ଅଙ୍କ_Len = 0;
    ପାଇଁ (ଅକ୍ଷର c : legacyType) {
        ଯଦି (c == '_' || c == '/' || c == '-') {
            ଯଦି (alpha_ଅଙ୍କ_Len == 0) {
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
            alpha_ଅଙ୍କ_Len = 0;
        } ଅନ୍ୟଥା ଯଦି (UPRV_ISALPHANUM(c)) {
            alpha_ଅଙ୍କ_Len++;
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    }
    ଫେରନ୍ତୁ alpha_ଅଙ୍କ_Len != 0;
}

}  // namespace

U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
uloc_to_Legacy_ଚାବି(ଅଚଳ ଅକ୍ଷର* keyword U_LIFETIME_BOUND)
{
    ଯଦି (keyword == nullptr || *keyword == '\0') { ଫେରନ୍ତୁ nullptr; }
    std::optional<std::string_view> result = ulocimp_to_Legacy_ଚାବି_With_Fallback(keyword);
    ଫେରନ୍ତୁ result.has_value() ? result->data() : nullptr;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_to_Legacy_ଚାବି_With_Fallback(std::string_view keyword U_LIFETIME_BOUND)
{
    std::optional<std::string_view> legacy_ଚାବି_2 = ulocimp_to_Legacy_ଚାବି(keyword);
    ଯଦି (!legacy_ଚାବି_2.has_value() && is_Well_Formed_Legacy_ଚାବି(keyword)) {
        // Checks if the specified locale key is well-formed with the legacy locale syntax.
        //
        // Note:
        //  LDML/CLDR provides some definition of keyword syntax in
        //  * http://www.unicode.org/reports/tr35/#Unicode_locale_identifier and
        //  * http://www.unicode.org/reports/tr35/#Old_Locale_Extension_Syntax
        //  Keys can only consist of [0-9a-zA-Z].
        ଫେରନ୍ତୁ keyword;
    }
    ଫେରନ୍ତୁ legacy_ଚାବି_2;
}

U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
uloc_toLegacyType(ଅଚଳ ଅକ୍ଷର* keyword, ଅଚଳ ଅକ୍ଷର* value U_LIFETIME_BOUND)
{
    ଯଦି (keyword == nullptr || *keyword == '\0' ||
        value == nullptr || *value == '\0') { ଫେରନ୍ତୁ nullptr; }
    std::optional<std::string_view> result = ulocimp_toLegacyTypeWithFallback(keyword, value);
    ଫେରନ୍ତୁ result.has_value() ? result->data() : nullptr;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyTypeWithFallback(std::string_view keyword, std::string_view value U_LIFETIME_BOUND)
{
    std::optional<std::string_view> legacyType = ulocimp_toLegacyType(keyword, value);
    ଯଦି (!legacyType.has_value() && isWellFormedLegacyType(value)) {
        // Checks if the specified locale type is well-formed with the legacy locale syntax.
        //
        // Note:
        //  LDML/CLDR provides some definition of keyword syntax in
        //  * http://www.unicode.org/reports/tr35/#Unicode_locale_identifier and
        //  * http://www.unicode.org/reports/tr35/#Old_Locale_Extension_Syntax
        //  Values (types) can only consist of [0-9a-zA-Z], plus for legacy values
        //  we allow [/_-+] in the middle (e.g. "Etc/GMT+1", "Asia/Tel_Aviv")
        ଫେରନ୍ତୁ value;
    }
    ଫେରନ୍ତୁ legacyType;
}

/*eof*/
