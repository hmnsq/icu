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
U_CFUNC ಶೂನ್ಯ locale_ಗಣ_default(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *id);
U_CFUNC ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale_get_default();

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
constexpr ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* LANGUAGES[] = {
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
ಶೂನ್ಯಸೂಚಕ,
    "in",  "iw",  "ji",  "jw",  "mo",  "sh",  "swc", "tl",  /* obsolete language codes */
ಶೂನ್ಯಸೂಚಕ
};

constexpr ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* DEPRECATED_LANGUAGES[]={
    "in", "iw", "ji", "jw", "mo", ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ
};
constexpr ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* REPLACEMENT_LANGUAGES[]={
    "id", "he", "yi", "jv", "ro", ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ
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
constexpr ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* LANGUAGES_3[] = {
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
ಶೂನ್ಯಸೂಚಕ,
/*  "in",  "iw",  "ji",  "jw",  "mo",  "sh",  "swc", "tl",  */
    "ind", "heb", "yid", "jaw", "mol", "srp", "swc", "tgl",
ಶೂನ್ಯಸೂಚಕ
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
constexpr ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* COUNTRIES[] = {
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
ಶೂನ್ಯಸೂಚಕ,
    "AN",  "BU", "CS", "FX", "RO", "SU", "TP", "YD", "YU", "ZR",   /* obsolete country codes */
ಶೂನ್ಯಸೂಚಕ
};

constexpr ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* DEPRECATED_COUNTRIES[] = {
    "AN", "BU", "CS", "DD", "DY", "FX", "HV", "NH", "RH", "SU", "TP", "UK", "VD", "YD", "YU", "ZR", ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ /* deprecated country list */
};
constexpr ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* REPLACEMENT_COUNTRIES[] = {
/*  "AN", "BU", "CS", "DD", "DY", "FX", "HV", "NH", "RH", "SU", "TP", "UK", "VD", "YD", "YU", "ZR" */
    "CW", "MM", "RS", "DE", "BJ", "FR", "BF", "VU", "ZW", "RU", "TL", "GB", "VN", "YE", "RS", "CD", ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ  /* replacement country codes */
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
constexpr ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* COUNTRIES_3[] = {
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
ಶೂನ್ಯಸೂಚಕ,
/*  "AN",  "BU",  "CS",  "FX",  "RO", "SU",  "TP",  "YD",  "YU",  "ZR" */
    "ANT", "BUR", "SCG", "FXX", "ROM", "SUN", "TMP", "YMD", "YUG", "ZAR",
ಶೂನ್ಯಸೂಚಕ
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ CanonicalizationMap {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *id;          /* input ID */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *canonicalID; /* canonicalized output ID */
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
int32_t get_ಅತಿಹ್ರಸ್ವ_Subtag_ಉದ್ದಳತೆ(std::string_view localeID) {
    int32_t localeIDLength = static_cast<int32_t>(localeID.length());
    int32_t length = localeIDLength;
    int32_t tmp_ಉದ್ದಳತೆ = 0;
    int32_t i;
    ತಾರ್ಕಿಕ reset = ನಿಜ;

    ಗಾಗಿ (i = 0; i < localeIDLength; i++) {
        ಆದರೆ (localeID[i] != '_' && localeID[i] != '-') {
            ಆದರೆ (reset) {
                tmp_ಉದ್ದಳತೆ = 0;
                reset = ಸುಳ್ಳು;
            }
            tmp_ಉದ್ದಳತೆ++;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ (tmp_ಉದ್ದಳತೆ != 0 && tmp_ಉದ್ದಳತೆ < length) {
                length = tmp_ಉದ್ದಳತೆ;
            }
            reset = ನಿಜ;
        }
    }

    ಹಿಂತಿರುಗಿ length;
}
/* Test if the locale id has BCP47 u extension and does not have '@' */
ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ _hasBCP47Extension(std::string_view id) {
    ಹಿಂತಿರುಗಿ id.find('@') == std::string_view::npos && get_ಅತಿಹ್ರಸ್ವ_Subtag_ಉದ್ದಳತೆ(id) == 1;
}

/* ### Keywords **************************************************/
ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ UPRV_ISDIGIT(ಅಕ್ಷರ c) { ಹಿಂತಿರುಗಿ c >= '0' && c <= '9'; }
ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ UPRV_ISALPHANUM(ಅಕ್ಷರ c) { ಹಿಂತಿರುಗಿ uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }
/* Punctuation/symbols allowed in legacy key values */
ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ UPRV_OK_ಮೌಲ್ಯ_PUNCTUATION(ಅಕ್ಷರ c) { ಹಿಂತಿರುಗಿ c == '_' || c == '-' || c == '+' || c == '/'; }

}  // namespace

#ವ್ಯಾಖ್ಯಾನಿಸು ULOC_KEYWORD_ಬಫರ್_LEN 25
#ವ್ಯಾಖ್ಯಾನಿಸು ULOC_MAX_NO_KEYWORDS 25

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
locale_get_Keywords_ಆರಂಭ(std::string_view localeID U_LIFETIME_BOUND) {
    ಆದರೆ (size_t pos = localeID.find('@'); pos != std::string_view::npos) {
        ಹಿಂತಿರುಗಿ localeID.ದತ್ತಾಂಶ() + pos;
    }
#ಆದರೆ (U_CHARSET_FAMILY == U_EBCDIC_FAMILY)
    ಇಲ್ಲದಿದ್ದರೆ {
        /* We do this because the @ sign is variant, and the @ sign used on one
        EBCDIC machine won't be compiled the same way on other EBCDIC based
        machines. */
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint8_t ebcdicSigns[] = { 0x7C, 0x44, 0x66, 0x80, 0xAC, 0xAE, 0xAF, 0xB5, 0xEC, 0xEF, 0x00 };
        ಸ್ಥಿರಾಂಕ uint8_t *char_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ = ebcdicSigns;
        ಆಗ(*char_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ) {
            ಆದರೆ (size_t pos = localeID.find(*char_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ); pos != std::string_view::npos) {
                ಹಿಂತಿರುಗಿ localeID.ದತ್ತಾಂಶ() + pos;
            }
            char_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ++;
        }
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

ನಾಮಾಕಾಶ {

/**
 * @param keywordName incoming name to be canonicalized
 * @param status return status (keyword too long)
 * @return the keyword name
 */
CharString locale_canonKeywordName(std::string_view keywordName, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
  ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ {}; }
  CharString ಫಲಿತಾಂಶ;

  ಗಾಗಿ (ಅಕ್ಷರ c : keywordName) {
    ಆದರೆ (!UPRV_ISALPHANUM(c)) {
      ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR; /* malformed keyword name */
      ಹಿಂತಿರುಗಿ {};
    }
    ಫಲಿತಾಂಶ.append(uprv_tolower(c), ಸ್ಥಿತಿ_2);
  }
  ಆದರೆ (ಫಲಿತಾಂಶ.is_ಖಾಲಿಗಳು()) {
    ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR; /* empty keyword name */
    ಹಿಂತಿರುಗಿ {};
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಅಕ್ಷರ keyword[ULOC_KEYWORD_ಬಫರ್_LEN];
    int32_t keywordLen;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ_ಆರಂಭ;
    int32_t ಮೌಲ್ಯ_Len;
} KeywordStruct;

int32_t U_CALLCONV
compareKeywordStructs(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ * /*context*/, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಎಡ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಬಲ) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಎಡ_String = static_cast<ಸ್ಥಿರಾಂಕ KeywordStruct*>(ಎಡ)->keyword;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಬಲ_String = static_cast<ಸ್ಥಿರಾಂಕ KeywordStruct*>(ಬಲ)->keyword;
    ಹಿಂತಿರುಗಿ uprv_strcmp(ಎಡ_String, ಬಲ_String);
}

}  // namespace

U_EXPORT CharString
ulocimp_getKeywords(std::string_view localeID,
                    ಅಕ್ಷರ prev,
                    ತಾರ್ಕಿಕ ಮೌಲ್ಯಗಳು_Too,
                    UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಹಿಂತಿರುಗಿ ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Char_String(
        [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            ulocimp_getKeywords(localeID,
                                prev,
                                sink,
                                ಮೌಲ್ಯಗಳು_Too,
                                ಸ್ಥಿತಿ_2);
        },
        ಸ್ಥಿತಿ_2);
}

U_EXPORT ಶೂನ್ಯ
ulocimp_getKeywords(std::string_view localeID,
                    ಅಕ್ಷರ prev,
                    ByteSink& sink,
                    ತಾರ್ಕಿಕ ಮೌಲ್ಯಗಳು_Too,
                    UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }

    KeywordStruct keywordList[ULOC_MAX_NO_KEYWORDS];

    int32_t maxKeywords = ULOC_MAX_NO_KEYWORDS;
    int32_t ಅಂಕೆ_Keywords = 0;
    size_t equalSign = std::string_view::npos;
    size_t semicolon = std::string_view::npos;
    int32_t i = 0, j, n;

    ಆದರೆ(prev == '@') { /* start of keyword definition */
        /* we will grab pairs, trim spaces, lowercase keywords, sort and return */
        ಮಾಡು {
            ತಾರ್ಕಿಕ duplicate = ಸುಳ್ಳು;
            /* skip leading spaces */
            ಆಗ (!localeID.empty() && localeID.front() == ' ') {
                localeID.remove_ಪೂರ್ವಪ್ರತ್ಯಯ(1);
            }
            ಆದರೆ (localeID.empty()) { /* handle trailing "; " */
                ನಿಲ್ಲಿಸು;
            }
            ಆದರೆ(ಅಂಕೆ_Keywords == maxKeywords) {
                ಸ್ಥಿತಿ_2 = U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR;
                ಹಿಂತಿರುಗಿ;
            }
            equalSign = localeID.find('=');
            semicolon = localeID.find(';');
            /* lack of '=' [foo@currency] is illegal */
            /* ';' before '=' [foo@currency;collation=pinyin] is illegal */
            ಆದರೆ (equalSign == std::string_view::npos ||
                (semicolon != std::string_view::npos && semicolon < equalSign)) {
                ಸ್ಥಿತಿ_2 = U_INVALID_FORMAT_ERROR;
                ಹಿಂತಿರುಗಿ;
            }
            /* zero-length keyword is an error. */
            ಆದರೆ (equalSign == 0) {
                ಸ್ಥಿತಿ_2 = U_INVALID_FORMAT_ERROR;
                ಹಿಂತಿರುಗಿ;
            }
            /* need to normalize both keyword and keyword name */
            ಆದರೆ (equalSign >= ULOC_KEYWORD_ಬಫರ್_LEN) {
                /* keyword name too long for internal buffer */
                ಸ್ಥಿತಿ_2 = U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR;
                ಹಿಂತಿರುಗಿ;
            }
            ಗಾಗಿ (i = 0, n = 0; static_cast<size_t>(i) < equalSign; ++i) {
                ಆದರೆ (localeID[i] != ' ') {
                    keywordList[ಅಂಕೆ_Keywords].keyword[n++] = uprv_tolower(localeID[i]);
                }
            }

            keywordList[ಅಂಕೆ_Keywords].keyword[n] = 0;
            keywordList[ಅಂಕೆ_Keywords].keywordLen = n;
            /* now grab the value part. First we skip the '=' */
            equalSign++;
            /* then we leading spaces */
            ಆಗ (equalSign < localeID.length() && localeID[equalSign] == ' ') {
                equalSign++;
            }

            /* Premature end or zero-length value */
            ಆದರೆ (equalSign == localeID.length() || equalSign == semicolon) {
                ಸ್ಥಿತಿ_2 = U_INVALID_FORMAT_ERROR;
                ಹಿಂತಿರುಗಿ;
            }

            keywordList[ಅಂಕೆ_Keywords].ಮೌಲ್ಯ_ಆರಂಭ = localeID.ದತ್ತಾಂಶ() + equalSign;

            std::string_view value = localeID;
            ಆದರೆ (semicolon != std::string_view::npos) {
                value.remove_suffix(value.length() - semicolon);
                localeID.remove_ಪೂರ್ವಪ್ರತ್ಯಯ(semicolon + 1);
            } ಇಲ್ಲದಿದ್ದರೆ {
                localeID = {};
            }
            value.remove_ಪೂರ್ವಪ್ರತ್ಯಯ(equalSign);
            ಆದರೆ (size_t ಕೊನೆಯ = value.find_last_not_of(' '); ಕೊನೆಯ != std::string_view::npos) {
                value.remove_suffix(value.length() - ಕೊನೆಯ - 1);
            }
            keywordList[ಅಂಕೆ_Keywords].ಮೌಲ್ಯ_Len = static_cast<int32_t>(value.length());

            /* If this is a duplicate keyword, then ignore it */
            ಗಾಗಿ (j=0; j<ಅಂಕೆ_Keywords; ++j) {
                ಆದರೆ (uprv_strcmp(keywordList[j].keyword, keywordList[ಅಂಕೆ_Keywords].keyword) == 0) {
                    duplicate = ನಿಜ;
                    ನಿಲ್ಲಿಸು;
                }
            }
            ಆದರೆ (!duplicate) {
                ++ಅಂಕೆ_Keywords;
            }
        } ಆಗ (!localeID.empty());

        /* now we have a list of keywords */
        /* we need to sort it */
        uprv_ವಿಂಗಡಿಸು_Array(keywordList, ಅಂಕೆ_Keywords, ಗಾತ್ರ(KeywordStruct), compareKeywordStructs, ಶೂನ್ಯಸೂಚಕ, ಸುಳ್ಳು, &ಸ್ಥಿತಿ_2);

        /* Now construct the keyword part */
        ಗಾಗಿ(i = 0; i < ಅಂಕೆ_Keywords; i++) {
            sink.Append(keywordList[i].keyword, keywordList[i].keywordLen);
            ಆದರೆ(ಮೌಲ್ಯಗಳು_Too) {
                sink.Append("=", 1);
                sink.Append(keywordList[i].ಮೌಲ್ಯ_ಆರಂಭ, keywordList[i].ಮೌಲ್ಯ_Len);
                ಆದರೆ(i < ಅಂಕೆ_Keywords - 1) {
                    sink.Append(";", 1);
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                sink.Append("\0", 1);
            }
        }
    }
}

U_CAPI int32_t U_EXPORT2
uloc_get_Keyword_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* keywordName,
                     ಅಕ್ಷರ* ಬಫರ್, int32_t ಬಫರ್_ಸಾಮರ್ಥ್ಯ,
                     UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ 0; }
    ಆದರೆ (keywordName == ಶೂನ್ಯಸೂಚಕ || *keywordName == '\0') {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Terminated_Chars(
        ಬಫರ್, ಬಫರ್_ಸಾಮರ್ಥ್ಯ,
        [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            ulocimp_get_Keyword_ಮೌಲ್ಯ(localeID, keywordName, sink, ಸ್ಥಿತಿ_2);
        },
        *ಸ್ಥಿತಿ_2);
}

U_EXPORT CharString
ulocimp_get_Keyword_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                        std::string_view keywordName,
                        UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಹಿಂತಿರುಗಿ ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Char_String(
        [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            ulocimp_get_Keyword_ಮೌಲ್ಯ(localeID, keywordName, sink, ಸ್ಥಿತಿ_2);
        },
        ಸ್ಥಿತಿ_2);
}

U_EXPORT ಶೂನ್ಯ
ulocimp_get_Keyword_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                        std::string_view keywordName,
                        icu::ByteSink& sink,
                        UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }

    ಆದರೆ (localeID == ಶೂನ್ಯಸೂಚಕ || keywordName.empty()) {
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಆರಂಭ_ಹುಡುಕು_ಇಲ್ಲಿ = ಶೂನ್ಯಸೂಚಕ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮುಂದಿನ_Separator = ಶೂನ್ಯಸೂಚಕ;

    CharString ತಾತ್ಕಾಲಿಕ_ಬಫರ್;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* tmpLocaleID;

    CharString canonKeywordName = locale_canonKeywordName(keywordName, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
      ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (localeID != ಶೂನ್ಯಸೂಚಕ && _hasBCP47Extension(localeID)) {
        ತಾತ್ಕಾಲಿಕ_ಬಫರ್ = ulocimp_for_Language_ಟ್ಯಾಗ್(localeID, -1, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);
        tmpLocaleID = U_SUCCESS(ಸ್ಥಿತಿ_2) && !ತಾತ್ಕಾಲಿಕ_ಬಫರ್.is_ಖಾಲಿಗಳು() ? ತಾತ್ಕಾಲಿಕ_ಬಫರ್.ದತ್ತಾಂಶ() : localeID;
    } ಇಲ್ಲದಿದ್ದರೆ {
        tmpLocaleID=localeID;
    }

    ಆರಂಭ_ಹುಡುಕು_ಇಲ್ಲಿ = locale_get_Keywords_ಆರಂಭ(tmpLocaleID);
    ಆದರೆ(ಆರಂಭ_ಹುಡುಕು_ಇಲ್ಲಿ == ಶೂನ್ಯಸೂಚಕ) {
        /* no keywords, return at once */
        ಹಿಂತಿರುಗಿ;
    }

    /* find the first keyword */
    ಆಗ(ಆರಂಭ_ಹುಡುಕು_ಇಲ್ಲಿ) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ_ಮೌಲ್ಯ_ಬಾಲ;

        ಆರಂಭ_ಹುಡುಕು_ಇಲ್ಲಿ++; /* skip @ or ; */
        ಮುಂದಿನ_Separator = uprv_strchr(ಆರಂಭ_ಹುಡುಕು_ಇಲ್ಲಿ, '=');
        ಆದರೆ(!ಮುಂದಿನ_Separator) {
            ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR; /* key must have =value */
            ಹಿಂತಿರುಗಿ;
        }
        /* strip leading & trailing spaces (TC decided to tolerate these) */
        ಆಗ(*ಆರಂಭ_ಹುಡುಕು_ಇಲ್ಲಿ == ' ') {
            ಆರಂಭ_ಹುಡುಕು_ಇಲ್ಲಿ++;
        }
        ಕೀಲಿ_ಮೌಲ್ಯ_ಬಾಲ = ಮುಂದಿನ_Separator;
        ಆಗ (ಕೀಲಿ_ಮೌಲ್ಯ_ಬಾಲ > ಆರಂಭ_ಹುಡುಕು_ಇಲ್ಲಿ && *(ಕೀಲಿ_ಮೌಲ್ಯ_ಬಾಲ-1) == ' ') {
            ಕೀಲಿ_ಮೌಲ್ಯ_ಬಾಲ--;
        }
        /* now keyValueTail points to first char after the keyName */
        /* copy & normalize keyName from locale */
        ಆದರೆ (ಆರಂಭ_ಹುಡುಕು_ಇಲ್ಲಿ == ಕೀಲಿ_ಮೌಲ್ಯ_ಬಾಲ) {
            ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR; /* empty keyword name in passed-in locale */
            ಹಿಂತಿರುಗಿ;
        }
        CharString localeKeywordName;
        ಆಗ (ಆರಂಭ_ಹುಡುಕು_ಇಲ್ಲಿ < ಕೀಲಿ_ಮೌಲ್ಯ_ಬಾಲ) {
          ಆದರೆ (!UPRV_ISALPHANUM(*ಆರಂಭ_ಹುಡುಕು_ಇಲ್ಲಿ)) {
            ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR; /* malformed keyword name */
            ಹಿಂತಿರುಗಿ;
          }
          localeKeywordName.append(uprv_tolower(*ಆರಂಭ_ಹುಡುಕು_ಇಲ್ಲಿ++), ಸ್ಥಿತಿ_2);
        }
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ;
        }

        ಆರಂಭ_ಹುಡುಕು_ಇಲ್ಲಿ = uprv_strchr(ಮುಂದಿನ_Separator, ';');

        ಆದರೆ (canonKeywordName == localeKeywordName) {
             /* current entry matches the keyword. */
           ಮುಂದಿನ_Separator++; /* skip '=' */
            /* First strip leading & trailing spaces (TC decided to tolerate these) */
            ಆಗ(*ಮುಂದಿನ_Separator == ' ') {
              ಮುಂದಿನ_Separator++;
            }
            ಕೀಲಿ_ಮೌಲ್ಯ_ಬಾಲ = (ಆರಂಭ_ಹುಡುಕು_ಇಲ್ಲಿ)? ಆರಂಭ_ಹುಡುಕು_ಇಲ್ಲಿ: ಮುಂದಿನ_Separator + uprv_strlen(ಮುಂದಿನ_Separator);
            ಆಗ(ಕೀಲಿ_ಮೌಲ್ಯ_ಬಾಲ > ಮುಂದಿನ_Separator && *(ಕೀಲಿ_ಮೌಲ್ಯ_ಬಾಲ-1) == ' ') {
              ಕೀಲಿ_ಮೌಲ್ಯ_ಬಾಲ--;
            }
            /* Now copy the value, but check well-formedness */
            ಆದರೆ (ಮುಂದಿನ_Separator == ಕೀಲಿ_ಮೌಲ್ಯ_ಬಾಲ) {
              ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR; /* empty key value name in passed-in locale */
              ಹಿಂತಿರುಗಿ;
            }
            ಆಗ (ಮುಂದಿನ_Separator < ಕೀಲಿ_ಮೌಲ್ಯ_ಬಾಲ) {
              ಆದರೆ (!UPRV_ISALPHANUM(*ಮುಂದಿನ_Separator) && !UPRV_OK_ಮೌಲ್ಯ_PUNCTUATION(*ಮುಂದಿನ_Separator)) {
                ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR; /* malformed key value */
                ಹಿಂತಿರುಗಿ;
              }
              /* Should we lowercase value to return here? Tests expect as-is. */
              sink.Append(ಮುಂದಿನ_Separator++, 1);
            }
            ಹಿಂತಿರುಗಿ;
        }
    }
}

U_CAPI int32_t U_EXPORT2
uloc_ಗಣ_Keyword_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* keywordName,
                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* keyword_ಮೌಲ್ಯ,
                     ಅಕ್ಷರ* ಬಫರ್, int32_t ಬಫರ್_ಸಾಮರ್ಥ್ಯ,
                     UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ 0; }

    ಆದರೆ (keywordName == ಶೂನ್ಯಸೂಚಕ || *keywordName == 0) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ (ಬಫರ್_ಸಾಮರ್ಥ್ಯ <= 1) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    int32_t bufLen = (int32_t)uprv_strlen(ಬಫರ್);
    ಆದರೆ(ಬಫರ್_ಸಾಮರ್ಥ್ಯ<bufLen) {
        /* The capacity is less than the length?! Is this NUL terminated? */
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಅಕ್ಷರ* keywords = const_cast<ಅಕ್ಷರ*>(
        locale_get_Keywords_ಆರಂಭ({ಬಫರ್, static_cast<std::string_view::size_type>(bufLen)}));
    int32_t ತಳ_Len = keywords == ಶೂನ್ಯಸೂಚಕ ? bufLen : keywords - ಬಫರ್;
    // Remove -1 from the capacity so that this function can guarantee NUL termination.
    CheckedArrayByteSink sink(keywords == ಶೂನ್ಯಸೂಚಕ ? ಬಫರ್ + bufLen : keywords,
                              ಬಫರ್_ಸಾಮರ್ಥ್ಯ - ತಳ_Len - 1);
    int32_t reslen = ulocimp_ಗಣ_Keyword_ಮೌಲ್ಯ(
        keywords == ಶೂನ್ಯಸೂಚಕ ? std::string_view() : keywords,
        keywordName,
        keyword_ಮೌಲ್ಯ == ಶೂನ್ಯಸೂಚಕ ? std::string_view() : keyword_ಮೌಲ್ಯ,
        sink,
        *ಸ್ಥಿತಿ_2);

    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ *ಸ್ಥಿತಿ_2 == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR ? reslen + ತಳ_Len : 0;
    }

    // See the documentation for this function, it's guaranteed to never
    // overflow the buffer but instead abort with BUFFER_OVERFLOW_ERROR.
    // In this case, nothing has been written to the sink, so it cannot have Overflowed().
    U_ASSERT(!sink.Overflowed());
    U_ASSERT(reslen >= 0);
    ಹಿಂತಿರುಗಿ u_terminateChars(ಬಫರ್, ಬಫರ್_ಸಾಮರ್ಥ್ಯ, reslen + ತಳ_Len, ಸ್ಥಿತಿ_2);
}

U_EXPORT ಶೂನ್ಯ
ulocimp_ಗಣ_Keyword_ಮೌಲ್ಯ(std::string_view keywordName,
                        std::string_view keyword_ಮೌಲ್ಯ,
                        CharString& localeID,
                        UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }
    std::string_view keywords;
    ಆದರೆ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಆರಂಭ = locale_get_Keywords_ಆರಂಭ(localeID.ಗಮ್ಯ_String_Piece()); ಆರಂಭ != ಶೂನ್ಯಸೂಚಕ) {
        // This is safe because CharString::truncate() doesn't actually erase any
        // data, but simply sets the position for where new data will be written.
        int32_t size = ಆರಂಭ - localeID.ದತ್ತಾಂಶ();
        keywords = localeID.ಗಮ್ಯ_String_Piece();
        keywords.remove_ಪೂರ್ವಪ್ರತ್ಯಯ(size);
        localeID.truncate(size);
    }
    CharStringByteSink sink(&localeID);
    ulocimp_ಗಣ_Keyword_ಮೌಲ್ಯ(keywords, keywordName, keyword_ಮೌಲ್ಯ, sink, ಸ್ಥಿತಿ_2);
}

U_EXPORT int32_t
ulocimp_ಗಣ_Keyword_ಮೌಲ್ಯ(std::string_view keywords,
                        std::string_view keywordName,
                        std::string_view keyword_ಮೌಲ್ಯ,
                        ByteSink& sink,
                        UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ 0; }

    /* TODO: sorting. removal. */
    int32_t needLen = 0;
    int32_t rc;
    CharString updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು;
    ತಾರ್ಕಿಕ handled_ಒಳಹರಿವು_ಕೀಲಿ_And_ಮೌಲ್ಯ = ಸುಳ್ಳು;
    ಅಕ್ಷರ ಕೀಲಿ_ಮೌಲ್ಯ_ಪೂರ್ವಪ್ರತ್ಯಯ = '@';

    ಆದರೆ (ಸ್ಥಿತಿ_2 == U_STRING_NOT_TERMINATED_WARNING) {
        ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    }
    ಆದರೆ (keywordName.empty()) {
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    CharString canonKeywordName = locale_canonKeywordName(keywordName, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    CharString canon_Keyword_ಮೌಲ್ಯ;
    ಗಾಗಿ (ಅಕ್ಷರ c : keyword_ಮೌಲ್ಯ) {
        ಆದರೆ (!UPRV_ISALPHANUM(c) && !UPRV_OK_ಮೌಲ್ಯ_PUNCTUATION(c)) {
            ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR; /* malformed key value */
            ಹಿಂತಿರುಗಿ 0;
        }
        /* Should we force lowercase in value to set? */
        canon_Keyword_ಮೌಲ್ಯ.append(c, ಸ್ಥಿತಿ_2);
    }
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ (keywords.size() <= 1) {
        ಆದರೆ (canon_Keyword_ಮೌಲ್ಯ.is_ಖಾಲಿಗಳು()) { /* no keywords = nothing to remove */
            U_ASSERT(ಸ್ಥಿತಿ_2 != U_STRING_NOT_TERMINATED_WARNING);
            ಹಿಂತಿರುಗಿ 0;
        }

        needLen = 1 + canonKeywordName.length() + 1 + canon_Keyword_ಮೌಲ್ಯ.length();
        int32_t capacity = 0;
        ಅಕ್ಷರ* ಬಫರ್ = sink.Get_Append_ಬಫರ್(
                needLen, needLen, ಶೂನ್ಯಸೂಚಕ, needLen, &capacity);
        ಆದರೆ (capacity < needLen || ಬಫರ್ == ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿತಿ_2 = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ಹಿಂತಿರುಗಿ needLen; /* no change */
        }
        ಅಕ್ಷರ* it = ಬಫರ್;

        *it++ = '@';
        uprv_memcpy(it, canonKeywordName.ದತ್ತಾಂಶ(), canonKeywordName.length());
        it += canonKeywordName.length();
        *it++ = '=';
        uprv_memcpy(it, canon_Keyword_ಮೌಲ್ಯ.ದತ್ತಾಂಶ(), canon_Keyword_ಮೌಲ್ಯ.length());
        sink.Append(ಬಫರ್, needLen);
        U_ASSERT(ಸ್ಥಿತಿ_2 != U_STRING_NOT_TERMINATED_WARNING);
        ಹಿಂತಿರುಗಿ needLen;
    } /* end shortcut - no @ */

    /* search for keyword */
    ಗಾಗಿ (size_t keyword_ಆರಂಭ = 0; keyword_ಆರಂಭ != std::string_view::npos;) {
        keyword_ಆರಂಭ++; /* skip @ or ; */
        size_t ಮುಂದಿನ_Equalsign = keywords.find('=', keyword_ಆರಂಭ);
        ಆದರೆ (ಮುಂದಿನ_Equalsign == std::string_view::npos) {
            ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR; /* key must have =value */
            ಹಿಂತಿರುಗಿ 0;
        }
        /* strip leading & trailing spaces (TC decided to tolerate these) */
        ಆಗ (keyword_ಆರಂಭ < keywords.size() && keywords[keyword_ಆರಂಭ] == ' ') {
            keyword_ಆರಂಭ++;
        }
        size_t ಕೀಲಿ_ಮೌಲ್ಯ_ಬಾಲ = ಮುಂದಿನ_Equalsign;
        ಆಗ (ಕೀಲಿ_ಮೌಲ್ಯ_ಬಾಲ > keyword_ಆರಂಭ && keywords[ಕೀಲಿ_ಮೌಲ್ಯ_ಬಾಲ - 1] == ' ') {
            ಕೀಲಿ_ಮೌಲ್ಯ_ಬಾಲ--;
        }
        /* now keyValueTail points to first char after the keyName */
        /* copy & normalize keyName from locale */
        ಆದರೆ (keyword_ಆರಂಭ == ಕೀಲಿ_ಮೌಲ್ಯ_ಬಾಲ) {
            ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR; /* empty keyword name in passed-in locale */
            ಹಿಂತಿರುಗಿ 0;
        }
        CharString localeKeywordName;
        ಆಗ (keyword_ಆರಂಭ < ಕೀಲಿ_ಮೌಲ್ಯ_ಬಾಲ) {
            ಆದರೆ (!UPRV_ISALPHANUM(keywords[keyword_ಆರಂಭ])) {
                ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR; /* malformed keyword name */
                ಹಿಂತಿರುಗಿ 0;
            }
            localeKeywordName.append(uprv_tolower(keywords[keyword_ಆರಂಭ++]), ಸ್ಥಿತಿ_2);
        }
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ 0;
        }

        size_t ಮುಂದಿನ_Separator = keywords.find(';', ಮುಂದಿನ_Equalsign);

        /* start processing the value part */
        ಮುಂದಿನ_Equalsign++; /* skip '=' */
        /* First strip leading & trailing spaces (TC decided to tolerate these) */
        ಆಗ (ಮುಂದಿನ_Equalsign < keywords.size() && keywords[ಮುಂದಿನ_Equalsign] == ' ') {
            ಮುಂದಿನ_Equalsign++;
        }
        ಕೀಲಿ_ಮೌಲ್ಯ_ಬಾಲ = ಮುಂದಿನ_Separator == std::string_view::npos ? keywords.size() : ಮುಂದಿನ_Separator;
        ಆಗ (ಕೀಲಿ_ಮೌಲ್ಯ_ಬಾಲ > ಮುಂದಿನ_Equalsign && keywords[ಕೀಲಿ_ಮೌಲ್ಯ_ಬಾಲ - 1] == ' ') {
            ಕೀಲಿ_ಮೌಲ್ಯ_ಬಾಲ--;
        }
        ಆದರೆ (ಮುಂದಿನ_Equalsign == ಕೀಲಿ_ಮೌಲ್ಯ_ಬಾಲ) {
            ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR; /* empty key value in passed-in locale */
            ಹಿಂತಿರುಗಿ 0;
        }

        rc = uprv_strcmp(canonKeywordName.ದತ್ತಾಂಶ(), localeKeywordName.ದತ್ತಾಂಶ());
        ಆದರೆ(rc == 0) {
            /* Current entry matches the input keyword. Update the entry */
            ಆದರೆ (!canon_Keyword_ಮೌಲ್ಯ.is_ಖಾಲಿಗಳು()) { /* updating a value */
                updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append(ಕೀಲಿ_ಮೌಲ್ಯ_ಪೂರ್ವಪ್ರತ್ಯಯ, ಸ್ಥಿತಿ_2);
                ಕೀಲಿ_ಮೌಲ್ಯ_ಪೂರ್ವಪ್ರತ್ಯಯ = ';'; /* for any subsequent key-value pair */
                updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append(canonKeywordName, ಸ್ಥಿತಿ_2);
                updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append('=', ಸ್ಥಿತಿ_2);
                updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append(canon_Keyword_ಮೌಲ್ಯ, ಸ್ಥಿತಿ_2);
            } /* else removing this entry, don't emit anything */
            handled_ಒಳಹರಿವು_ಕೀಲಿ_And_ಮೌಲ್ಯ = ನಿಜ;
        } ಇಲ್ಲದಿದ್ದರೆ {
           /* input keyword sorts earlier than current entry, add before current entry */
            ಆದರೆ (rc < 0 && !canon_Keyword_ಮೌಲ್ಯ.is_ಖಾಲಿಗಳು() && !handled_ಒಳಹರಿವು_ಕೀಲಿ_And_ಮೌಲ್ಯ) {
                /* insert new entry at this location */
                updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append(ಕೀಲಿ_ಮೌಲ್ಯ_ಪೂರ್ವಪ್ರತ್ಯಯ, ಸ್ಥಿತಿ_2);
                ಕೀಲಿ_ಮೌಲ್ಯ_ಪೂರ್ವಪ್ರತ್ಯಯ = ';'; /* for any subsequent key-value pair */
                updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append(canonKeywordName, ಸ್ಥಿತಿ_2);
                updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append('=', ಸ್ಥಿತಿ_2);
                updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append(canon_Keyword_ಮೌಲ್ಯ, ಸ್ಥಿತಿ_2);
                handled_ಒಳಹರಿವು_ಕೀಲಿ_And_ಮೌಲ್ಯ = ನಿಜ;
            }
            /* copy the current entry */
            updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append(ಕೀಲಿ_ಮೌಲ್ಯ_ಪೂರ್ವಪ್ರತ್ಯಯ, ಸ್ಥಿತಿ_2);
            ಕೀಲಿ_ಮೌಲ್ಯ_ಪೂರ್ವಪ್ರತ್ಯಯ = ';'; /* for any subsequent key-value pair */
            updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append(localeKeywordName, ಸ್ಥಿತಿ_2);
            updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append('=', ಸ್ಥಿತಿ_2);
            updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append(keywords.ದತ್ತಾಂಶ() + ಮುಂದಿನ_Equalsign,
                                        static_cast<int32_t>(ಕೀಲಿ_ಮೌಲ್ಯ_ಬಾಲ - ಮುಂದಿನ_Equalsign), ಸ್ಥಿತಿ_2);
        }
        ಆದರೆ (ಮುಂದಿನ_Separator == std::string_view::npos && !canon_Keyword_ಮೌಲ್ಯ.is_ಖಾಲಿಗಳು() && !handled_ಒಳಹರಿವು_ಕೀಲಿ_And_ಮೌಲ್ಯ) {
            /* append new entry at the end, it sorts later than existing entries */
            updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append(ಕೀಲಿ_ಮೌಲ್ಯ_ಪೂರ್ವಪ್ರತ್ಯಯ, ಸ್ಥಿತಿ_2);
            /* skip keyValuePrefix update, no subsequent key-value pair */
            updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append(canonKeywordName, ಸ್ಥಿತಿ_2);
            updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append('=', ಸ್ಥಿತಿ_2);
            updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.append(canon_Keyword_ಮೌಲ್ಯ, ಸ್ಥಿತಿ_2);
            handled_ಒಳಹರಿವು_ಕೀಲಿ_And_ಮೌಲ್ಯ = ನಿಜ;
        }
        keyword_ಆರಂಭ = ಮುಂದಿನ_Separator;
    } /* end loop searching */

    /* Any error from updatedKeysAndValues.append above would be internal and not due to
     * problems with the passed-in locale. So if we did encounter problems with the
     * passed-in locale above, those errors took precedence and overrode any error
     * status from updatedKeysAndValues.append, and also caused a return of 0. If there
     * are errors here they are from updatedKeysAndValues.append; they do cause an
     * error return but the passed-in locale is unmodified and the original bufLen is
     * returned.
     */
    ಆದರೆ (!handled_ಒಳಹರಿವು_ಕೀಲಿ_And_ಮೌಲ್ಯ || U_FAILURE(ಸ್ಥಿತಿ_2)) {
        /* if input key/value specified removal of a keyword not present in locale, or
         * there was an error in CharString.append, leave original locale alone. */
        U_ASSERT(ಸ್ಥಿತಿ_2 != U_STRING_NOT_TERMINATED_WARNING);
        // The sink is expected to be a buffer which already contains the full
        // locale string, so when it isn't going to be modified there's no need
        // to actually write any data to it, as the data is already there. Only
        // the first character needs to be overwritten (changing '\0' to '@').
        needLen = static_cast<int32_t>(keywords.size());
        int32_t capacity = 0;
        ಅಕ್ಷರ* ಬಫರ್ = sink.Get_Append_ಬಫರ್(
                needLen, needLen, ಶೂನ್ಯಸೂಚಕ, needLen, &capacity);
        ಆದರೆ (capacity < needLen || ಬಫರ್ == ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿತಿ_2 = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        } ಇಲ್ಲದಿದ್ದರೆ {
            *ಬಫರ್ = '@';
            sink.Append(ಬಫರ್, needLen);
        }
        ಹಿಂತಿರುಗಿ needLen;
    }

    needLen = updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.length();
    // Check to see can we fit the updatedKeysAndValues, if not, return
    // U_BUFFER_OVERFLOW_ERROR without copy updatedKeysAndValues into it.
    // We do this because this API function does not behave like most others:
    // It promises never to set a U_STRING_NOT_TERMINATED_WARNING.
    // When the contents fits but without the terminating NUL, in this case we need to not change
    // the buffer contents and return with a buffer overflow error.
    ಆದರೆ (needLen > 0) {
        int32_t capacity = 0;
        ಅಕ್ಷರ* ಬಫರ್ = sink.Get_Append_ಬಫರ್(
                needLen, needLen, ಶೂನ್ಯಸೂಚಕ, needLen, &capacity);
        ಆದರೆ (capacity < needLen || ಬಫರ್ == ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿತಿ_2 = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ಹಿಂತಿರುಗಿ needLen;
        }
        uprv_memcpy(ಬಫರ್, updated_ಕೀಲಿಗಳು_And_ಮೌಲ್ಯಗಳು.ದತ್ತಾಂಶ(), needLen);
        sink.Append(ಬಫರ್, needLen);
    }
    U_ASSERT(ಸ್ಥಿತಿ_2 != U_STRING_NOT_TERMINATED_WARNING);
    ಹಿಂತಿರುಗಿ needLen;
}

/* ### ID parsing implementation **************************************************/

ನಾಮಾಕಾಶ {

ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ is_ಪೂರ್ವಪ್ರತ್ಯಯ_Letter(ಅಕ್ಷರ a) { ಹಿಂತಿರುಗಿ a == 'x' || a == 'X' || a == 'i' || a == 'I'; }

/*returns true if one of the special prefixes is here (s=string)
  'x-' or 'i-' */
ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ _isIDPrefix(std::string_view s) {
    ಹಿಂತಿರುಗಿ s.size() >= 2 && is_ಪೂರ್ವಪ್ರತ್ಯಯ_Letter(s[0]) && _isIDSeparator(s[1]);
}

/* Dot terminates it because of POSIX form  where dot precedes the codepage
 * except for variant
 */
ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ _isTerminator(ಅಕ್ಷರ a) { ಹಿಂತಿರುಗಿ a == '.' || a == '@'; }

ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ _isBCP47Extension(std::string_view p) {
    ಹಿಂತಿರುಗಿ p.size() >= 3 &&
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
std::optional<int16_t> ಕಂಡುಹಿಡಿ_ಸೂಚ್ಯಂಕ_2(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ* list, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ)
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ* anchor = list;
    int32_t pass = 0;

    /* Make two passes through two nullptr-terminated arrays at 'list' */
    ಆಗ (pass++ < 2) {
        ಆಗ (*list) {
            ಆದರೆ (uprv_strcmp(ಕೀಲಿ, *list) == 0) {
                ಹಿಂತಿರುಗಿ static_cast<int16_t>(list - anchor);
            }
            list++;
        }
        ++list;     /* skip final nullptr *CWB*/
    }
    ಹಿಂತಿರುಗಿ std::nullopt;
}

}  // namespace

U_CFUNC ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*
uloc_getCurrentCountryID(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* oldID U_LIFETIME_BOUND) {
    std::optional<int16_t> ಸ್ಥಾನಾಂತರ = ಕಂಡುಹಿಡಿ_ಸೂಚ್ಯಂಕ_2(DEPRECATED_COUNTRIES, oldID);
    ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ.has_value() ? REPLACEMENT_COUNTRIES[*ಸ್ಥಾನಾಂತರ] : oldID;
}
U_CFUNC ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*
uloc_getCurrentLanguageID(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* oldID U_LIFETIME_BOUND) {
    std::optional<int16_t> ಸ್ಥಾನಾಂತರ = ಕಂಡುಹಿಡಿ_ಸೂಚ್ಯಂಕ_2(DEPRECATED_LANGUAGES, oldID);
    ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ.has_value() ? REPLACEMENT_LANGUAGES[*ಸ್ಥಾನಾಂತರ] : oldID;
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

size_t _getLanguage(std::string_view localeID, ByteSink* sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    size_t skip = 0;
    ಆದರೆ (localeID.size() == 4 && uprv_strnicmp(localeID.ದತ್ತಾಂಶ(), "root", 4) == 0) {
        skip = 4;
        localeID.remove_ಪೂರ್ವಪ್ರತ್ಯಯ(skip);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (localeID.size() >= 3 && uprv_strnicmp(localeID.ದತ್ತಾಂಶ(), "und", 3) == 0 &&
               (localeID.size() == 3 ||
                localeID[3] == '-' ||
                localeID[3] == '_' ||
                localeID[3] == '@')) {
        skip = 3;
        localeID.remove_ಪೂರ್ವಪ್ರತ್ಯಯ(skip);
    }

    constexpr int32_t MAXLEN = ULOC_LANG_ಸಾಮರ್ಥ್ಯ - 1;  // Minus NUL.

    /* if it starts with i- or x- then copy that prefix */
    size_t len = _isIDPrefix(localeID) ? 2 : 0;
    ಆಗ (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len])) {
        ಆದರೆ (len == MAXLEN) {
            ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
        len++;
    }

    ಆದರೆ (sink == ಶೂನ್ಯಸೂಚಕ || len == 0) { ಹಿಂತಿರುಗಿ skip + len; }

    int32_t min_ಸಾಮರ್ಥ್ಯ = uprv_max(static_cast<int32_t>(len), 4);  // Minimum 3 letters plus NUL.
    ಅಕ್ಷರ scratch[MAXLEN];
    int32_t capacity = 0;
    ಅಕ್ಷರ* ಬಫರ್ = sink->Get_Append_ಬಫರ್(
            min_ಸಾಮರ್ಥ್ಯ, min_ಸಾಮರ್ಥ್ಯ, scratch, UPRV_LENGTHOF(scratch), &capacity);

    ಗಾಗಿ (size_t i = 0; i < len; ++i) {
        ಬಫರ್[i] = uprv_tolower(localeID[i]);
    }
    ಆದರೆ (localeID.size() >= 2 && _isIDSeparator(localeID[1])) {
        ಬಫರ್[1] = '-';
    }

    ಆದರೆ (len == 3) {
        /* convert 3 character code to 2 character code if possible *CWB*/
        U_ASSERT(capacity >= 4);
        ಬಫರ್[3] = '\0';
        std::optional<int16_t> ಸ್ಥಾನಾಂತರ = ಕಂಡುಹಿಡಿ_ಸೂಚ್ಯಂಕ_2(LANGUAGES_3, ಬಫರ್);
        ಆದರೆ (ಸ್ಥಾನಾಂತರ.has_value()) {
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ alias = LANGUAGES[*ಸ್ಥಾನಾಂತರ];
            sink->Append(alias, static_cast<int32_t>(uprv_strlen(alias)));
            ಹಿಂತಿರುಗಿ skip + len;
        }
    }

    sink->Append(ಬಫರ್, static_cast<int32_t>(len));
    ಹಿಂತಿರುಗಿ skip + len;
}

size_t _getScript(std::string_view localeID, ByteSink* sink) {
    constexpr int32_t ಉದ್ದಳತೆ_3 = 4;

    size_t len = 0;
    ಆಗ (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len]) &&
            uprv_isASCIILetter(localeID[len])) {
        ಆದರೆ (len == ಉದ್ದಳತೆ_3) { ಹಿಂತಿರುಗಿ 0; }
        len++;
    }
    ಆದರೆ (len != ಉದ್ದಳತೆ_3) { ಹಿಂತಿರುಗಿ 0; }

    ಆದರೆ (sink == ಶೂನ್ಯಸೂಚಕ) { ಹಿಂತಿರುಗಿ len; }

    ಅಕ್ಷರ scratch[ಉದ್ದಳತೆ_3];
    int32_t capacity = 0;
    ಅಕ್ಷರ* ಬಫರ್ = sink->Get_Append_ಬಫರ್(
            ಉದ್ದಳತೆ_3, ಉದ್ದಳತೆ_3, scratch, UPRV_LENGTHOF(scratch), &capacity);

    ಬಫರ್[0] = uprv_toupper(localeID[0]);
    ಗಾಗಿ (int32_t i = 1; i < ಉದ್ದಳತೆ_3; ++i) {
        ಬಫರ್[i] = uprv_tolower(localeID[i]);
    }

    sink->Append(ಬಫರ್, ಉದ್ದಳತೆ_3);
    ಹಿಂತಿರುಗಿ len;
}

size_t _getRegion(std::string_view localeID, ByteSink* sink) {
    constexpr int32_t MINLEN = 2;
    constexpr int32_t MAXLEN = ULOC_COUNTRY_ಸಾಮರ್ಥ್ಯ - 1;  // Minus NUL.

    size_t len = 0;
    ಆಗ (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len])) {
        ಆದರೆ (len == MAXLEN) { ಹಿಂತಿರುಗಿ 0; }
        len++;
    }
    ಆದರೆ (len < MINLEN) { ಹಿಂತಿರುಗಿ 0; }

    ಆದರೆ (sink == ಶೂನ್ಯಸೂಚಕ) { ಹಿಂತಿರುಗಿ len; }

    ಅಕ್ಷರ scratch[ULOC_COUNTRY_ಸಾಮರ್ಥ್ಯ];
    int32_t capacity = 0;
    ಅಕ್ಷರ* ಬಫರ್ = sink->Get_Append_ಬಫರ್(
            ULOC_COUNTRY_ಸಾಮರ್ಥ್ಯ,
            ULOC_COUNTRY_ಸಾಮರ್ಥ್ಯ,
            scratch,
            UPRV_LENGTHOF(scratch),
            &capacity);

    ಗಾಗಿ (size_t i = 0; i < len; ++i) {
        ಬಫರ್[i] = uprv_toupper(localeID[i]);
    }

    ಆದರೆ (len == 3) {
        /* convert 3 character code to 2 character code if possible *CWB*/
        U_ASSERT(capacity >= 4);
        ಬಫರ್[3] = '\0';
        std::optional<int16_t> ಸ್ಥಾನಾಂತರ = ಕಂಡುಹಿಡಿ_ಸೂಚ್ಯಂಕ_2(COUNTRIES_3, ಬಫರ್);
        ಆದರೆ (ಸ್ಥಾನಾಂತರ.has_value()) {
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ alias = COUNTRIES[*ಸ್ಥಾನಾಂತರ];
            sink->Append(alias, static_cast<int32_t>(uprv_strlen(alias)));
            ಹಿಂತಿರುಗಿ len;
        }
    }

    sink->Append(ಬಫರ್, static_cast<int32_t>(len));
    ಹಿಂತಿರುಗಿ len;
}

/**
 * @param needSeparator if true, then add leading '_' if any variants
 * are added to 'variant'
 */
size_t
_getVariant(std::string_view localeID,
            ಅಕ್ಷರ prev,
            ByteSink* sink,
            ತಾರ್ಕಿಕ needSeparator,
            UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2) || localeID.empty()) ಹಿಂತಿರುಗಿ 0;

    // Reasonable upper limit for variants
    // There are no strict limitation of the syntax of variant in the legacy
    // locale format. If the locale is constructed from unicode_locale_id
    // as defined in UTS35, then we know each unicode_variant_subtag
    // could have max length of 8 ((alphanum{5,8} | digit alphanum{3})
    // 179 would allow 20 unicode_variant_subtag with sep in the
    // unicode_locale_id
    // 8*20 + 1*(20-1) = 179
    constexpr int32_t MAX_VARIANTS_ಉದ್ದಳತೆ = 179;

    /* get one or more variant tags and separate them with '_' */
    size_t ಸೂಚ್ಯಂಕ = 0;
    ಆದರೆ (_isIDSeparator(prev)) {
        /* get a variant string after a '-' or '_' */
        ಗಾಗಿ (std::string_view sub = localeID;;) {
            size_t next = sub.find_first_of(".@_-");
            // For historical reasons, a trailing separator is included in the variant.
            ತಾರ್ಕಿಕ ಪೂರ್ಣಗೊಂಡ = next == std::string_view::npos || next + 1 == sub.length();
            size_t ಎಲ್ಲೆ = ಪೂರ್ಣಗೊಂಡ ? sub.length() : next;
            ಸೂಚ್ಯಂಕ += ಎಲ್ಲೆ;
            ಆದರೆ (ಸೂಚ್ಯಂಕ > MAX_VARIANTS_ಉದ್ದಳತೆ) {
                ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
                ಹಿಂತಿರುಗಿ 0;
            }

            ಆದರೆ (sink != ಶೂನ್ಯಸೂಚಕ) {
                ಆದರೆ (needSeparator) {
                    sink->Append("_", 1);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    needSeparator = ನಿಜ;
                }

                int32_t length = static_cast<int32_t>(ಎಲ್ಲೆ);
                int32_t min_ಸಾಮರ್ಥ್ಯ = uprv_min(length, MAX_VARIANTS_ಉದ್ದಳತೆ);
                ಅಕ್ಷರ scratch[MAX_VARIANTS_ಉದ್ದಳತೆ];
                int32_t capacity = 0;
                ಅಕ್ಷರ* ಬಫರ್ = sink->Get_Append_ಬಫರ್(
                        min_ಸಾಮರ್ಥ್ಯ, min_ಸಾಮರ್ಥ್ಯ, scratch, UPRV_LENGTHOF(scratch), &capacity);

                ಗಾಗಿ (size_t i = 0; i < ಎಲ್ಲೆ; ++i) {
                    ಬಫರ್[i] = uprv_toupper(sub[i]);
                }
                sink->Append(ಬಫರ್, length);
            }

            ಆದರೆ (ಪೂರ್ಣಗೊಂಡ) { ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ; }
            sub.remove_ಪೂರ್ವಪ್ರತ್ಯಯ(next);
            ಆದರೆ (_isTerminator(sub.front()) || _isBCP47Extension(sub)) { ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ; }
            sub.remove_ಪೂರ್ವಪ್ರತ್ಯಯ(1);
            ಸೂಚ್ಯಂಕ++;
        }
    }

    size_t skip = 0;
    /* if there is no variant tag after a '-' or '_' then look for '@' */
    ಆದರೆ (prev == '@') {
        /* keep localeID */
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* p = locale_get_Keywords_ಆರಂಭ(localeID); p != ಶೂನ್ಯಸೂಚಕ) {
        skip = 1 + p - localeID.ದತ್ತಾಂಶ(); /* point after the '@' */
        localeID.remove_ಪೂರ್ವಪ್ರತ್ಯಯ(skip);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಗಾಗಿ (; ಸೂಚ್ಯಂಕ < localeID.size() && !_isTerminator(localeID[ಸೂಚ್ಯಂಕ]); ಸೂಚ್ಯಂಕ++) {
        ಆದರೆ (ಸೂಚ್ಯಂಕ >= MAX_VARIANTS_ಉದ್ದಳತೆ) { // same as length > MAX_VARIANTS_LENGTH
            ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
        ಆದರೆ (needSeparator) {
            ಆದರೆ (sink != ಶೂನ್ಯಸೂಚಕ) {
                sink->Append("_", 1);
            }
            needSeparator = ಸುಳ್ಳು;
        }
        ಆದರೆ (sink != ಶೂನ್ಯಸೂಚಕ) {
            ಅಕ್ಷರ c = uprv_toupper(localeID[ಸೂಚ್ಯಂಕ]);
            ಆದರೆ (c == '-' || c == ',') c = '_';
            sink->Append(&c, 1);
        }
    }
    ಹಿಂತಿರುಗಿ skip + ಸೂಚ್ಯಂಕ;
}

}  // namespace

U_EXPORT CharString
ulocimp_getLanguage(std::string_view localeID, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಹಿಂತಿರುಗಿ ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Char_String(
        [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            ulocimp_getSubtags(
                    localeID,
                    &sink,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಸ್ಥಿತಿ_2);
        },
        ಸ್ಥಿತಿ_2);
}

U_EXPORT CharString
ulocimp_getScript(std::string_view localeID, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಹಿಂತಿರುಗಿ ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Char_String(
        [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            ulocimp_getSubtags(
                    localeID,
                    ಶೂನ್ಯಸೂಚಕ,
                    &sink,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಸ್ಥಿತಿ_2);
        },
        ಸ್ಥಿತಿ_2);
}

U_EXPORT CharString
ulocimp_getRegion(std::string_view localeID, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಹಿಂತಿರುಗಿ ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Char_String(
        [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            ulocimp_getSubtags(
                    localeID,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಶೂನ್ಯಸೂಚಕ,
                    &sink,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಸ್ಥಿತಿ_2);
        },
        ಸ್ಥಿತಿ_2);
}

U_EXPORT CharString
ulocimp_getVariant(std::string_view localeID, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಹಿಂತಿರುಗಿ ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Char_String(
        [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            ulocimp_getSubtags(
                    localeID,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಶೂನ್ಯಸೂಚಕ,
                    &sink,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಸ್ಥಿತಿ_2);
        },
        ಸ್ಥಿತಿ_2);
}

U_EXPORT ಶೂನ್ಯ
ulocimp_getSubtags(
        std::string_view localeID,
        CharString* language,
        CharString* script,
        CharString* region,
        CharString* variant,
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ** p_ಕೊನೆ,
        UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }

    std::optional<CharStringByteSink> languageSink;
    std::optional<CharStringByteSink> scriptSink;
    std::optional<CharStringByteSink> regionSink;
    std::optional<CharStringByteSink> variantSink;

    ಆದರೆ (language != ಶೂನ್ಯಸೂಚಕ) { languageSink.emplace(language); }
    ಆದರೆ (script != ಶೂನ್ಯಸೂಚಕ) { scriptSink.emplace(script); }
    ಆದರೆ (region != ಶೂನ್ಯಸೂಚಕ) { regionSink.emplace(region); }
    ಆದರೆ (variant != ಶೂನ್ಯಸೂಚಕ) { variantSink.emplace(variant); }

    ulocimp_getSubtags(
            localeID,
            languageSink.has_value() ? &*languageSink : ಶೂನ್ಯಸೂಚಕ,
            scriptSink.has_value() ? &*scriptSink : ಶೂನ್ಯಸೂಚಕ,
            regionSink.has_value() ? &*regionSink : ಶೂನ್ಯಸೂಚಕ,
            variantSink.has_value() ? &*variantSink : ಶೂನ್ಯಸೂಚಕ,
            p_ಕೊನೆ,
            ಸ್ಥಿತಿ_2);
}

U_EXPORT ಶೂನ್ಯ
ulocimp_getSubtags(
        std::string_view localeID,
        ByteSink* language,
        ByteSink* script,
        ByteSink* region,
        ByteSink* variant,
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ** p_ಕೊನೆ,
        UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }

    ಆದರೆ (p_ಕೊನೆ != ಶೂನ್ಯಸೂಚಕ) {
        *p_ಕೊನೆ = localeID.ದತ್ತಾಂಶ();
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (language == ಶೂನ್ಯಸೂಚಕ &&
               script == ಶೂನ್ಯಸೂಚಕ &&
               region == ಶೂನ್ಯಸೂಚಕ &&
               variant == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (localeID.empty()) { ಹಿಂತಿರುಗಿ; }

    ತಾರ್ಕಿಕ hasRegion = ಸುಳ್ಳು;

    {
        size_t len = _getLanguage(localeID, language, ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }
        ಆದರೆ (len > 0) {
            localeID.remove_ಪೂರ್ವಪ್ರತ್ಯಯ(len);
        }
    }

    ಆದರೆ (p_ಕೊನೆ != ಶೂನ್ಯಸೂಚಕ) {
        *p_ಕೊನೆ = localeID.ದತ್ತಾಂಶ();
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (script == ಶೂನ್ಯಸೂಚಕ &&
               region == ಶೂನ್ಯಸೂಚಕ &&
               variant == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (localeID.empty()) { ಹಿಂತಿರುಗಿ; }

    ಆದರೆ (_isIDSeparator(localeID.front())) {
        std::string_view sub = localeID;
        sub.remove_ಪೂರ್ವಪ್ರತ್ಯಯ(1);
        size_t len = _getScript(sub, script);
        ಆದರೆ (len > 0) {
            localeID.remove_ಪೂರ್ವಪ್ರತ್ಯಯ(len + 1);
            ಆದರೆ (p_ಕೊನೆ != ಶೂನ್ಯಸೂಚಕ) { *p_ಕೊನೆ = localeID.ದತ್ತಾಂಶ(); }
        }
    }

    ಆದರೆ ((region == ಶೂನ್ಯಸೂಚಕ && variant == ಶೂನ್ಯಸೂಚಕ && p_ಕೊನೆ == ಶೂನ್ಯಸೂಚಕ) || localeID.empty()) { ಹಿಂತಿರುಗಿ; }

    ಆದರೆ (_isIDSeparator(localeID.front())) {
        std::string_view sub = localeID;
        sub.remove_ಪೂರ್ವಪ್ರತ್ಯಯ(1);
        size_t len = _getRegion(sub, region);
        ಆದರೆ (len > 0) {
            hasRegion = ನಿಜ;
            localeID.remove_ಪೂರ್ವಪ್ರತ್ಯಯ(len + 1);
            ಆದರೆ (p_ಕೊನೆ != ಶೂನ್ಯಸೂಚಕ) { *p_ಕೊನೆ = localeID.ದತ್ತಾಂಶ(); }
        }
    }

    ಆದರೆ ((variant == ಶೂನ್ಯಸೂಚಕ && p_ಕೊನೆ == ಶೂನ್ಯಸೂಚಕ) || localeID.empty()) { ಹಿಂತಿರುಗಿ; }

    ತಾರ್ಕಿಕ hasVariant = ಸುಳ್ಳು;

    ಆದರೆ (_isIDSeparator(localeID.front()) && !_isBCP47Extension(localeID)) {
        std::string_view sub = localeID;
        /* If there was no country ID, skip a possible extra IDSeparator */
        size_t skip = !hasRegion && localeID.size() > 1 && _isIDSeparator(localeID[1]) ? 2 : 1;
        sub.remove_ಪೂರ್ವಪ್ರತ್ಯಯ(skip);
        size_t len = _getVariant(sub, localeID[0], variant, ಸುಳ್ಳು, ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }
        ಆದರೆ (len > 0) {
            hasVariant = ನಿಜ;
            localeID.remove_ಪೂರ್ವಪ್ರತ್ಯಯ(skip + len);
            ಆದರೆ (p_ಕೊನೆ != ಶೂನ್ಯಸೂಚಕ) { *p_ಕೊನೆ = localeID.ದತ್ತಾಂಶ(); }
        }
    }

    ಆದರೆ ((variant == ಶೂನ್ಯಸೂಚಕ && p_ಕೊನೆ == ಶೂನ್ಯಸೂಚಕ) || localeID.empty()) { ಹಿಂತಿರುಗಿ; }

    ಆದರೆ (_isBCP47Extension(localeID)) {
        localeID.remove_ಪೂರ್ವಪ್ರತ್ಯಯ(2);
        constexpr ಅಕ್ಷರ vaposix[] = "-va-posix";
        constexpr size_t length = ಗಾತ್ರ vaposix - 1;
        ಗಾಗಿ (size_t next;; localeID.remove_ಪೂರ್ವಪ್ರತ್ಯಯ(next)) {
            next = localeID.find('-', 1);
            ಆದರೆ (next == std::string_view::npos) { ನಿಲ್ಲಿಸು; }
            next = localeID.find('-', next + 1);
            ತಾರ್ಕಿಕ ಪೂರ್ಣಗೊಂಡ = next == std::string_view::npos;
            std::string_view sub = localeID;
            ಆದರೆ (!ಪೂರ್ಣಗೊಂಡ) { sub.remove_suffix(sub.length() - next); }

            ಆದರೆ (sub.length() == length && uprv_strnicmp(sub.ದತ್ತಾಂಶ(), vaposix, length) == 0) {
                ಆದರೆ (variant != ಶೂನ್ಯಸೂಚಕ) {
                    ಆದರೆ (hasVariant) { variant->Append("_", 1); }
                    constexpr ಅಕ್ಷರ posix[] = "POSIX";
                    variant->Append(posix, ಗಾತ್ರ posix - 1);
                }
                ಆದರೆ (p_ಕೊನೆ != ಶೂನ್ಯಸೂಚಕ) { *p_ಕೊನೆ = localeID.ದತ್ತಾಂಶ() + length; }
            }

            ಆದರೆ (ಪೂರ್ಣಗೊಂಡ) { ನಿಲ್ಲಿಸು; }
        }
    }
}

/* Keyword enumeration */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ UKeywordsContext {
    ಅಕ್ಷರ* keywords;
    ಅಕ್ಷರ* current;
} UKeywordsContext;

U_CDECL_BEGIN

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
uloc_kw_closeKeywords(UEnumeration *enumerator) {
    uprv_free(((UKeywordsContext *)enumerator->context)->keywords);
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

ಸ್ಥಿರ int32_t U_CALLCONV
uloc_kw_ಎಣಿಕೆ_Keywords(UEnumeration *en, UError_ಸಂಕೇತ * /*ಸ್ಥಿತಿ*/) {
    ಅಕ್ಷರ *kw = ((UKeywordsContext *)en->context)->keywords;
    int32_t ಫಲಿತಾಂಶ = 0;
    ಆಗ(*kw) {
        ಫಲಿತಾಂಶ++;
        kw += uprv_strlen(kw)+1;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * U_CALLCONV
uloc_kw_ಮುಂದಿನ_Keyword(UEnumeration* en,
                    int32_t* ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ,
                    UError_ಸಂಕೇತ* /*ಸ್ಥಿತಿ*/) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಫಲಿತಾಂಶ = ((UKeywordsContext *)en->context)->current;
    int32_t len = 0;
    ಆದರೆ(*ಫಲಿತಾಂಶ) {
        len = (int32_t)uprv_strlen(((UKeywordsContext *)en->context)->current);
        ((UKeywordsContext *)en->context)->current += len+1;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ) {
        *ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ = len;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
uloc_kw_resetKeywords(UEnumeration* en,
                      UError_ಸಂಕೇತ* /*ಸ್ಥಿತಿ*/) {
    ((UKeywordsContext *)en->context)->current = ((UKeywordsContext *)en->context)->keywords;
}

U_CDECL_ಕೊನೆ


ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UEnumeration gKeywordsEnum = {
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    uloc_kw_closeKeywords,
    uloc_kw_ಎಣಿಕೆ_Keywords,
    uenum_unextDefault,
    uloc_kw_ಮುಂದಿನ_Keyword,
    uloc_kw_resetKeywords
};

U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywordList(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *keywordList, int32_t keyword_List_ಗಾತ್ರ, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }

    Local_ಸ್ಮರಣೆ<UKeywordsContext> myContext;
    Local_ಸ್ಮರಣೆ<UEnumeration> ಫಲಿತಾಂಶ;

    myContext.adoptInstead(static_cast<UKeywordsContext *>(uprv_malloc(ಗಾತ್ರ(UKeywordsContext))));
    ಫಲಿತಾಂಶ.adoptInstead(static_cast<UEnumeration *>(uprv_malloc(ಗಾತ್ರ(UEnumeration))));
    ಆದರೆ (myContext.isNull() || ಫಲಿತಾಂಶ.isNull()) {
        *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    uprv_memcpy(ಫಲಿತಾಂಶ.getAlias(), &gKeywordsEnum, ಗಾತ್ರ(UEnumeration));
    myContext->keywords = static_cast<ಅಕ್ಷರ *>(uprv_malloc(keyword_List_ಗಾತ್ರ+1));
    ಆದರೆ (myContext->keywords == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    uprv_memcpy(myContext->keywords, keywordList, keyword_List_ಗಾತ್ರ);
    myContext->keywords[keyword_List_ಗಾತ್ರ] = 0;
    myContext->current = myContext->keywords;
    ಫಲಿತಾಂಶ->context = myContext.orphan();
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ.orphan();
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywords(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                        UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2)
{
    ಆದರೆ(ಸ್ಥಿತಿ_2==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    CharString ತಾತ್ಕಾಲಿಕ_ಬಫರ್;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* tmpLocaleID;

    ಆದರೆ (localeID != ಶೂನ್ಯಸೂಚಕ && _hasBCP47Extension(localeID)) {
        ತಾತ್ಕಾಲಿಕ_ಬಫರ್ = ulocimp_for_Language_ಟ್ಯಾಗ್(localeID, -1, ಶೂನ್ಯಸೂಚಕ, *ಸ್ಥಿತಿ_2);
        tmpLocaleID = U_SUCCESS(*ಸ್ಥಿತಿ_2) && !ತಾತ್ಕಾಲಿಕ_ಬಫರ್.is_ಖಾಲಿಗಳು() ? ತಾತ್ಕಾಲಿಕ_ಬಫರ್.ದತ್ತಾಂಶ() : localeID;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ (localeID==ಶೂನ್ಯಸೂಚಕ) {
            localeID=uloc_getDefault();
        }
        tmpLocaleID=localeID;
    }

    ulocimp_getSubtags(
            tmpLocaleID,
            ಶೂನ್ಯಸೂಚಕ,
            ಶೂನ್ಯಸೂಚಕ,
            ಶೂನ್ಯಸೂಚಕ,
            ಶೂನ್ಯಸೂಚಕ,
            &tmpLocaleID,
            *ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    /* keywords are located after '@' */
    ಆದರೆ((tmpLocaleID = locale_get_Keywords_ಆರಂಭ(tmpLocaleID)) != ಶೂನ್ಯಸೂಚಕ) {
        CharString keywords = ulocimp_getKeywords(tmpLocaleID + 1, '@', ಸುಳ್ಳು, *ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        ಹಿಂತಿರುಗಿ uloc_openKeywordList(keywords.ದತ್ತಾಂಶ(), keywords.length(), ಸ್ಥಿತಿ_2);
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}


/* bit-flags for 'options' parameter of _canonicalize */
#ವ್ಯಾಖ್ಯಾನಿಸು _ULOC_STRIP_KEYWORDS 0x2
#ವ್ಯಾಖ್ಯಾನಿಸು _ULOC_CANONICALIZE   0x1

ನಾಮಾಕಾಶ {

ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ OPTION_ಗಣ(uint32_t options, uint32_t mask) { ಹಿಂತಿರುಗಿ (options & mask) != 0; }

constexpr ಅಕ್ಷರ i_default[] = {'i', '-', 'd', 'e', 'f', 'a', 'u', 'l', 't'};
constexpr int32_t I_DEFAULT_ಉದ್ದಳತೆ = UPRV_LENGTHOF(i_default);

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
              UError_ಸಂಕೇತ& err) {
    ಆದರೆ (U_FAILURE(err)) {
        ಹಿಂತಿರುಗಿ;
    }

    int32_t j, ಕ್ಷೇತ್ರ_ಎಣಿಕೆ=0;
    CharString ತಾತ್ಕಾಲಿಕ_ಬಫರ್;  // if localeID has a BCP47 extension, tmpLocaleID points to this
    CharString localeIDWithHyphens;  // if localeID has a BPC47 extension and have _, tmpLocaleID points to this
    std::string_view origLocaleID;
    std::string_view tmpLocaleID;
    size_t keyword_ನಿಯೋಜಿಸು = std::string_view::npos;
    size_t separatorIndicator = std::string_view::npos;

    ಆದರೆ (_hasBCP47Extension(localeID)) {
        std::string_view localeIDPtr = localeID;

        // convert all underbars to hyphens, unless the "BCP47 extension" comes at the beginning of the string
        ಆದರೆ (localeID.size() >= 2 && localeID.find('_') != std::string_view::npos && localeID[1] != '-' && localeID[1] != '_') {
            localeIDWithHyphens.append(localeID, err);
            ಆದರೆ (U_SUCCESS(err)) {
                ಗಾಗಿ (ಅಕ್ಷರ* p = localeIDWithHyphens.ದತ್ತಾಂಶ(); *p != '\0'; ++p) {
                    ಆದರೆ (*p == '_') {
                        *p = '-';
                    }
                }
                localeIDPtr = localeIDWithHyphens.ಗಮ್ಯ_String_Piece();
            }
        }

        ತಾತ್ಕಾಲಿಕ_ಬಫರ್ = ulocimp_for_Language_ಟ್ಯಾಗ್(localeIDPtr.ದತ್ತಾಂಶ(), static_cast<int32_t>(localeIDPtr.size()), ಶೂನ್ಯಸೂಚಕ, err);
        tmpLocaleID = U_SUCCESS(err) && !ತಾತ್ಕಾಲಿಕ_ಬಫರ್.is_ಖಾಲಿಗಳು() ? static_cast<std::string_view>(ತಾತ್ಕಾಲಿಕ_ಬಫರ್.ಗಮ್ಯ_String_Piece()) : localeIDPtr;
    } ಇಲ್ಲದಿದ್ದರೆ {
        tmpLocaleID=localeID;
    }

    origLocaleID=tmpLocaleID;

    /* get all pieces, one after another, and separate with '_' */
    CharString ಟ್ಯಾಗ್;
    CharString script;
    CharString country;
    CharString variant;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೊನೆ = ಶೂನ್ಯಸೂಚಕ;
    ulocimp_getSubtags(
            tmpLocaleID,
            &ಟ್ಯಾಗ್,
            &script,
            &country,
            &variant,
            &ಕೊನೆ,
            err);
    ಆದರೆ (U_FAILURE(err)) {
        ಹಿಂತಿರುಗಿ;
    }
    U_ASSERT(ಕೊನೆ != ಶೂನ್ಯಸೂಚಕ);
    ಆದರೆ (ಕೊನೆ > tmpLocaleID.ದತ್ತಾಂಶ()) {
        tmpLocaleID.remove_ಪೂರ್ವಪ್ರತ್ಯಯ(ಕೊನೆ - tmpLocaleID.ದತ್ತಾಂಶ());
    }

    ಆದರೆ (ಟ್ಯಾಗ್.length() == I_DEFAULT_ಉದ್ದಳತೆ && origLocaleID.length() >= I_DEFAULT_ಉದ್ದಳತೆ &&
            uprv_strncmp(origLocaleID.ದತ್ತಾಂಶ(), i_default, I_DEFAULT_ಉದ್ದಳತೆ) == 0) {
        ಟ್ಯಾಗ್.clear();
        ಟ್ಯಾಗ್.append(uloc_getDefault(), err);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ (!script.is_ಖಾಲಿಗಳು()) {
            ++ಕ್ಷೇತ್ರ_ಎಣಿಕೆ;
            ಟ್ಯಾಗ್.append('_', err);
            ಟ್ಯಾಗ್.append(script, err);
        }
        ಆದರೆ (!country.is_ಖಾಲಿಗಳು()) {
            ++ಕ್ಷೇತ್ರ_ಎಣಿಕೆ;
            ಟ್ಯಾಗ್.append('_', err);
            ಟ್ಯಾಗ್.append(country, err);
        }
        ಆದರೆ (!variant.is_ಖಾಲಿಗಳು()) {
            ++ಕ್ಷೇತ್ರ_ಎಣಿಕೆ;
            ಆದರೆ (country.is_ಖಾಲಿಗಳು()) {
                ಟ್ಯಾಗ್.append('_', err);
            }
            ಟ್ಯಾಗ್.append('_', err);
            ಟ್ಯಾಗ್.append(variant, err);
        }
    }

    /* Copy POSIX-style charset specifier, if any [mr.utf8] */
    ಆದರೆ (!OPTION_ಗಣ(options, _ULOC_CANONICALIZE) && !tmpLocaleID.empty() && tmpLocaleID.front() == '.') {
        ಟ್ಯಾಗ್.append('.', err);
        tmpLocaleID.remove_ಪೂರ್ವಪ್ರತ್ಯಯ(1);
        size_t length;
        ಆದರೆ (size_t atPos = tmpLocaleID.find('@'); atPos != std::string_view::npos) {
            length = atPos;
        } ಇಲ್ಲದಿದ್ದರೆ {
            length = tmpLocaleID.length();
        }
        // The longest charset name we found in IANA charset registry
        // https://www.iana.org/assignments/character-sets/ is
        // "Extended_UNIX_Code_Packed_Format_for_Japanese" in length 45.
        // we therefore restrict the length here to be 64 which is a power of 2
        // number that is longer than 45.
        constexpr size_t k_Max_Charset_ಉದ್ದಳತೆ = 64;
        ಆದರೆ (length > k_Max_Charset_ಉದ್ದಳತೆ) {
           err = U_ILLEGAL_ವಾದ_ERROR; /* malformed keyword name */
           ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ (length > 0) {
            ಟ್ಯಾಗ್.append(tmpLocaleID.ದತ್ತಾಂಶ(), static_cast<int32_t>(length), err);
            tmpLocaleID.remove_ಪೂರ್ವಪ್ರತ್ಯಯ(length);
        }
    }

    /* Scan ahead to next '@' and determine if it is followed by '=' and/or ';'
       After this, tmpLocaleID either starts at '@' or is empty. */
    ಆದರೆ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಆರಂಭ = locale_get_Keywords_ಆರಂಭ(tmpLocaleID); ಆರಂಭ != ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ (ಆರಂಭ > tmpLocaleID.ದತ್ತಾಂಶ()) {
            tmpLocaleID.remove_ಪೂರ್ವಪ್ರತ್ಯಯ(ಆರಂಭ - tmpLocaleID.ದತ್ತಾಂಶ());
        }
        keyword_ನಿಯೋಜಿಸು = tmpLocaleID.find('=');
        separatorIndicator = tmpLocaleID.find(';');
    } ಇಲ್ಲದಿದ್ದರೆ {
        tmpLocaleID = {};
    }

    /* Copy POSIX-style variant, if any [mr@FOO] */
    ಆದರೆ (!OPTION_ಗಣ(options, _ULOC_CANONICALIZE) &&
        !tmpLocaleID.empty() && keyword_ನಿಯೋಜಿಸು == std::string_view::npos) {
        ಟ್ಯಾಗ್.append(tmpLocaleID, err);
        tmpLocaleID = {};
    }

    ಆದರೆ (OPTION_ಗಣ(options, _ULOC_CANONICALIZE)) {
        /* Handle @FOO variant if @ is present and not followed by = */
        ಆದರೆ (!tmpLocaleID.empty() && keyword_ನಿಯೋಜಿಸು == std::string_view::npos) {
            /* Add missing '_' if needed */
            ಆದರೆ (ಕ್ಷೇತ್ರ_ಎಣಿಕೆ < 2 || (ಕ್ಷೇತ್ರ_ಎಣಿಕೆ < 3 && !script.is_ಖಾಲಿಗಳು())) {
                ಮಾಡು {
                    ಟ್ಯಾಗ್.append('_', err);
                    ++ಕ್ಷೇತ್ರ_ಎಣಿಕೆ;
                } ಆಗ(ಕ್ಷೇತ್ರ_ಎಣಿಕೆ<2);
            }

            CharStringByteSink s(&ಟ್ಯಾಗ್);
            std::string_view sub = tmpLocaleID;
            sub.remove_ಪೂರ್ವಪ್ರತ್ಯಯ(1);
            _getVariant(sub, '@', &s, !variant.is_ಖಾಲಿಗಳು(), err);
            ಆದರೆ (U_FAILURE(err)) { ಹಿಂತಿರುಗಿ; }
        }

        /* Look up the ID in the canonicalization map */
        ಗಾಗಿ (j=0; j<UPRV_LENGTHOF(CANONICALIZE_MAP); j++) {
            StringPiece id(CANONICALIZE_MAP[j].id);
            ಆದರೆ (ಟ್ಯಾಗ್ == id) {
                ಆದರೆ (id.empty() && !tmpLocaleID.empty()) {
                    ನಿಲ್ಲಿಸು; /* Don't remap "" if keywords present */
                }
                ಟ್ಯಾಗ್.clear();
                ಟ್ಯಾಗ್.append(CANONICALIZE_MAP[j].canonicalID, err);
                ನಿಲ್ಲಿಸು;
            }
        }
    }

    sink.Append(ಟ್ಯಾಗ್.ದತ್ತಾಂಶ(), ಟ್ಯಾಗ್.length());

    ಆದರೆ (!OPTION_ಗಣ(options, _ULOC_STRIP_KEYWORDS)) {
        ಆದರೆ (!tmpLocaleID.empty() && keyword_ನಿಯೋಜಿಸು != std::string_view::npos &&
            (separatorIndicator == std::string_view::npos || separatorIndicator > keyword_ನಿಯೋಜಿಸು)) {
            sink.Append("@", 1);
            ++ಕ್ಷೇತ್ರ_ಎಣಿಕೆ;
            tmpLocaleID.remove_ಪೂರ್ವಪ್ರತ್ಯಯ(1);
            ulocimp_getKeywords(tmpLocaleID, '@', sink, ನಿಜ, err);
        }
    }
}

}  // namespace

/* ### ID parsing API **************************************************/

U_CAPI int32_t  U_EXPORT2
uloc_get_ಪೋಷಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*    localeID,
               ಅಕ್ಷರ* ಪೋಷಕ,
               int32_t ಪೋಷಕ_ಸಾಮರ್ಥ್ಯ,
               UError_ಸಂಕೇತ* err)
{
    ಹಿಂತಿರುಗಿ ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Terminated_Chars(
        ಪೋಷಕ, ಪೋಷಕ_ಸಾಮರ್ಥ್ಯ,
        [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            ulocimp_get_ಪೋಷಕ(localeID, sink, ಸ್ಥಿತಿ_2);
        },
        *err);
}

U_EXPORT CharString
ulocimp_get_ಪೋಷಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                  UError_ಸಂಕೇತ& err)
{
    ಹಿಂತಿರುಗಿ ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Char_String(
        [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            ulocimp_get_ಪೋಷಕ(localeID, sink, ಸ್ಥಿತಿ_2);
        },
        err);
}

U_EXPORT ಶೂನ್ಯ
ulocimp_get_ಪೋಷಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                  icu::ByteSink& sink,
                  UError_ಸಂಕೇತ& err)
{
    ಆದರೆ (U_FAILURE(err)) { ಹಿಂತಿರುಗಿ; }

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆಯ_Underscore;
    int32_t i;

    ಆದರೆ (localeID == ಶೂನ್ಯಸೂಚಕ)
        localeID = uloc_getDefault();

    ಕೊನೆಯ_Underscore=uprv_strrchr(localeID, '_');
    ಆದರೆ(ಕೊನೆಯ_Underscore!=ಶೂನ್ಯಸೂಚಕ) {
        i = static_cast<int32_t>(ಕೊನೆಯ_Underscore - localeID);
    } ಇಲ್ಲದಿದ್ದರೆ {
        i=0;
    }

    ಆದರೆ (i > 0) {
        ಆದರೆ (uprv_strnicmp(localeID, "und_", 4) == 0) {
            localeID += 3;
            i -= 3;
        }
        sink.Append(localeID, i);
    }
}

U_CAPI int32_t U_EXPORT2
uloc_getLanguage(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*    localeID,
         ಅಕ್ಷರ* language,
         int32_t language_ಸಾಮರ್ಥ್ಯ,
         UError_ಸಂಕೇತ* err)
{
    ಆದರೆ (localeID == ಶೂನ್ಯಸೂಚಕ) {
        localeID = uloc_getDefault();
    }

    /* uloc_getLanguage will return a 2 character iso-639 code if one exists. *CWB*/
    ಹಿಂತಿರುಗಿ ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Terminated_Chars(
        language, language_ಸಾಮರ್ಥ್ಯ,
        [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            ulocimp_getSubtags(
                    localeID,
                    &sink,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಸ್ಥಿತಿ_2);
        },
        *err);
}

U_CAPI int32_t U_EXPORT2
uloc_getScript(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*    localeID,
         ಅಕ್ಷರ* script,
         int32_t script_ಸಾಮರ್ಥ್ಯ,
         UError_ಸಂಕೇತ* err)
{
    ಆದರೆ (localeID == ಶೂನ್ಯಸೂಚಕ) {
        localeID = uloc_getDefault();
    }

    ಹಿಂತಿರುಗಿ ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Terminated_Chars(
        script, script_ಸಾಮರ್ಥ್ಯ,
        [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            ulocimp_getSubtags(
                    localeID,
                    ಶೂನ್ಯಸೂಚಕ,
                    &sink,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಸ್ಥಿತಿ_2);
        },
        *err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getCountry(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
            ಅಕ್ಷರ* country,
            int32_t country_ಸಾಮರ್ಥ್ಯ,
            UError_ಸಂಕೇತ* err)
{
    ಆದರೆ (localeID == ಶೂನ್ಯಸೂಚಕ) {
        localeID = uloc_getDefault();
    }

    ಹಿಂತಿರುಗಿ ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Terminated_Chars(
        country, country_ಸಾಮರ್ಥ್ಯ,
        [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            ulocimp_getSubtags(
                    localeID,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಶೂನ್ಯಸೂಚಕ,
                    &sink,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಸ್ಥಿತಿ_2);
        },
        *err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getVariant(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                ಅಕ್ಷರ* variant,
                int32_t variant_ಸಾಮರ್ಥ್ಯ,
                UError_ಸಂಕೇತ* err)
{
    ಆದರೆ (localeID == ಶೂನ್ಯಸೂಚಕ) {
        localeID = uloc_getDefault();
    }

    ಹಿಂತಿರುಗಿ ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Terminated_Chars(
        variant, variant_ಸಾಮರ್ಥ್ಯ,
        [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            ulocimp_getSubtags(
                    localeID,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಶೂನ್ಯಸೂಚಕ,
                    &sink,
                    ಶೂನ್ಯಸೂಚಕ,
                    ಸ್ಥಿತಿ_2);
        },
        *err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
             ಅಕ್ಷರ* name,
             int32_t name_ಸಾಮರ್ಥ್ಯ,
             UError_ಸಂಕೇತ* err)
{
    ಆದರೆ (localeID == ಶೂನ್ಯಸೂಚಕ) {
        localeID = uloc_getDefault();
    }
    ಹಿಂತಿರುಗಿ ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Terminated_Chars(
        name, name_ಸಾಮರ್ಥ್ಯ,
        [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            ulocimp_getName(localeID, sink, ಸ್ಥಿತಿ_2);
        },
        *err);
}

U_EXPORT CharString
ulocimp_getName(std::string_view localeID,
                UError_ಸಂಕೇತ& err)
{
    ಹಿಂತಿರುಗಿ ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Char_String(
        [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            ulocimp_getName(localeID, sink, ಸ್ಥಿತಿ_2);
        },
        err);
}

U_EXPORT ಶೂನ್ಯ
ulocimp_getName(std::string_view localeID,
                ByteSink& sink,
                UError_ಸಂಕೇತ& err)
{
    _canonicalize(localeID, sink, 0, err);
}

U_CAPI int32_t  U_EXPORT2
uloc_get_ತಳ_Name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                 ಅಕ್ಷರ* name,
                 int32_t name_ಸಾಮರ್ಥ್ಯ,
                 UError_ಸಂಕೇತ* err)
{
    ಆದರೆ (localeID == ಶೂನ್ಯಸೂಚಕ) {
        localeID = uloc_getDefault();
    }
    ಹಿಂತಿರುಗಿ ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Terminated_Chars(
        name, name_ಸಾಮರ್ಥ್ಯ,
        [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            ulocimp_get_ತಳ_Name(localeID, sink, ಸ್ಥಿತಿ_2);
        },
        *err);
}

U_EXPORT CharString
ulocimp_get_ತಳ_Name(std::string_view localeID,
                    UError_ಸಂಕೇತ& err)
{
    ಹಿಂತಿರುಗಿ ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Char_String(
        [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            ulocimp_get_ತಳ_Name(localeID, sink, ಸ್ಥಿತಿ_2);
        },
        err);
}

U_EXPORT ಶೂನ್ಯ
ulocimp_get_ತಳ_Name(std::string_view localeID,
                    ByteSink& sink,
                    UError_ಸಂಕೇತ& err)
{
    _canonicalize(localeID, sink, _ULOC_STRIP_KEYWORDS, err);
}

U_CAPI int32_t  U_EXPORT2
uloc_canonicalize(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                  ಅಕ್ಷರ* name,
                  int32_t name_ಸಾಮರ್ಥ್ಯ,
                  UError_ಸಂಕೇತ* err)
{
    ಆದರೆ (localeID == ಶೂನ್ಯಸೂಚಕ) {
        localeID = uloc_getDefault();
    }
    ಹಿಂತಿರುಗಿ ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Terminated_Chars(
        name, name_ಸಾಮರ್ಥ್ಯ,
        [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            ulocimp_canonicalize(localeID, sink, ಸ್ಥಿತಿ_2);
        },
        *err);
}

U_EXPORT CharString
ulocimp_canonicalize(std::string_view localeID,
                     UError_ಸಂಕೇತ& err)
{
    ಹಿಂತಿರುಗಿ ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Char_String(
        [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            ulocimp_canonicalize(localeID, sink, ಸ್ಥಿತಿ_2);
        },
        err);
}

U_EXPORT ಶೂನ್ಯ
ulocimp_canonicalize(std::string_view localeID,
                     ByteSink& sink,
                     UError_ಸಂಕೇತ& err)
{
    _canonicalize(localeID, sink, _ULOC_CANONICALIZE, err);
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*  U_EXPORT2
uloc_getISO3Language(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID)
{
    UError_ಸಂಕೇತ err = U_ZERO_ERROR;

    ಆದರೆ (localeID == ಶೂನ್ಯಸೂಚಕ)
    {
        localeID = uloc_getDefault();
    }
    CharString lang = ulocimp_getLanguage(localeID, err);
    ಆದರೆ (U_FAILURE(err))
        ಹಿಂತಿರುಗಿ "";
    std::optional<int16_t> ಸ್ಥಾನಾಂತರ = ಕಂಡುಹಿಡಿ_ಸೂಚ್ಯಂಕ_2(LANGUAGES, lang.ದತ್ತಾಂಶ());
    ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ.has_value() ? LANGUAGES_3[*ಸ್ಥಾನಾಂತರ] : "";
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*  U_EXPORT2
uloc_getISO3Country(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID)
{
    UError_ಸಂಕೇತ err = U_ZERO_ERROR;

    ಆದರೆ (localeID == ಶೂನ್ಯಸೂಚಕ)
    {
        localeID = uloc_getDefault();
    }
    CharString cntry = ulocimp_getRegion(localeID, err);
    ಆದರೆ (U_FAILURE(err))
        ಹಿಂತಿರುಗಿ "";
    std::optional<int16_t> ಸ್ಥಾನಾಂತರ = ಕಂಡುಹಿಡಿ_ಸೂಚ್ಯಂಕ_2(COUNTRIES, cntry.ದತ್ತಾಂಶ());
    ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ.has_value() ? COUNTRIES_3[*ಸ್ಥಾನಾಂತರ] : "";
}

U_CAPI uint32_t  U_EXPORT2
uloc_getLCID(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID)
{
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    uint32_t   lcid = 0;

    /* Check for incomplete id. */
    ಆದರೆ (!localeID || uprv_strlen(localeID) < 2) {
        ಹಿಂತಿರುಗಿ 0;
    }

    // First, attempt Windows platform lookup if available, but fall
    // through to catch any special cases (ICU vs Windows name differences).
    lcid = uprv_convert_ಗಮ್ಯ_LCIDPlatform(localeID, &ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (lcid > 0) {
        // Windows found an LCID, return that
        ಹಿಂತಿರುಗಿ lcid;
    }

    CharString langID = ulocimp_getLanguage(localeID, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ (uprv_strchr(localeID, '@')) {
        // uprv_convertToLCID does not support keywords other than collation.
        // Remove all keywords except collation.
        CharString collVal = ulocimp_get_Keyword_ಮೌಲ್ಯ(localeID, "collation", ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2) && !collVal.is_ಖಾಲಿಗಳು()) {
            CharString tmpLocaleID = ulocimp_get_ತಳ_Name(localeID, ಸ್ಥಿತಿ_2);
            ulocimp_ಗಣ_Keyword_ಮೌಲ್ಯ("collation", collVal.ಗಮ್ಯ_String_Piece(), tmpLocaleID, ಸ್ಥಿತಿ_2);
            ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
                ಹಿಂತಿರುಗಿ uprv_convert_ಗಮ್ಯ_LCID(langID.ದತ್ತಾಂಶ(), tmpLocaleID.ದತ್ತಾಂಶ(), &ಸ್ಥಿತಿ_2);
            }
        }

        // fall through - all keywords are simply ignored
        ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    }

    ಹಿಂತಿರುಗಿ uprv_convert_ಗಮ್ಯ_LCID(langID.ದತ್ತಾಂಶ(), localeID, &ಸ್ಥಿತಿ_2);
}

U_CAPI int32_t U_EXPORT2
uloc_getLocaleForLCID(uint32_t hostid, ಅಕ್ಷರ *locale, int32_t locale_ಸಾಮರ್ಥ್ಯ,
                UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
    ಹಿಂತಿರುಗಿ uprv_convert_ಗಮ್ಯ_Posix(hostid, locale, locale_ಸಾಮರ್ಥ್ಯ, ಸ್ಥಿತಿ_2);
}

/* ### Default locale **************************************************/

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*  U_EXPORT2
uloc_getDefault()
{
    ಹಿಂತಿರುಗಿ locale_get_default();
}

U_CAPI ಶೂನ್ಯ  U_EXPORT2
uloc_ಗಣ_Default(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*   newDefaultLocale,
             UError_ಸಂಕೇತ* err)
{
    ಆದರೆ (U_FAILURE(*err))
        ಹಿಂತಿರುಗಿ;
    /* the error code isn't currently used for anything by this function*/

    /* propagate change to C++ */
    locale_ಗಣ_default(newDefaultLocale);
}

/**
 * Returns a list of all 2-letter language codes defined in ISO 639.  This is a pointer
 * to an array of pointers to arrays of char.  All of these pointers are owned
 * by ICU-- do not delete them, and do not write through them.  The array is
 * terminated with a null pointer.
 */
U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ*  U_EXPORT2
uloc_getISOLanguages()
{
    ಹಿಂತಿರುಗಿ LANGUAGES;
}

/**
 * Returns a list of all 2-letter country codes defined in ISO 639.  This is a
 * pointer to an array of pointers to arrays of char.  All of these pointers are
 * owned by ICU-- do not delete them, and do not write through them.  The array is
 * terminated with a null pointer.
 */
U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ*  U_EXPORT2
uloc_getISOCountries()
{
    ಹಿಂತಿರುಗಿ COUNTRIES;
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
uloc_ಗಮ್ಯ_Unicode_Locale_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* keyword U_LIFETIME_BOUND)
{
    ಆದರೆ (keyword == ಶೂನ್ಯಸೂಚಕ || *keyword == '\0') { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    std::optional<std::string_view> ಫಲಿತಾಂಶ = ulocimp_ಗಮ್ಯ_Bcp_ಕೀಲಿ_With_Fallback(keyword);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ.has_value() ? ಫಲಿತಾಂಶ->ದತ್ತಾಂಶ() : ಶೂನ್ಯಸೂಚಕ;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_ಗಮ್ಯ_Bcp_ಕೀಲಿ_With_Fallback(std::string_view keyword U_LIFETIME_BOUND)
{
    std::optional<std::string_view> bcp_ಕೀಲಿ = ulocimp_ಗಮ್ಯ_Bcp_ಕೀಲಿ(keyword);
    ಆದರೆ (!bcp_ಕೀಲಿ.has_value() &&
        ultag_is_Unicode_Locale_ಕೀಲಿ(keyword.ದತ್ತಾಂಶ(), static_cast<int32_t>(keyword.size()))) {
        // unknown keyword, but syntax is fine..
        ಹಿಂತಿರುಗಿ keyword;
    }
    ಹಿಂತಿರುಗಿ bcp_ಕೀಲಿ;
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
uloc_ಗಮ್ಯ_Unicode_Locale_Type(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* keyword, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* value U_LIFETIME_BOUND)
{
    ಆದರೆ (keyword == ಶೂನ್ಯಸೂಚಕ || *keyword == '\0' ||
        value == ಶೂನ್ಯಸೂಚಕ || *value == '\0') { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    std::optional<std::string_view> ಫಲಿತಾಂಶ = ulocimp_ಗಮ್ಯ_Bcp_Type_With_Fallback(keyword, value);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ.has_value() ? ಫಲಿತಾಂಶ->ದತ್ತಾಂಶ() : ಶೂನ್ಯಸೂಚಕ;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_ಗಮ್ಯ_Bcp_Type_With_Fallback(std::string_view keyword, std::string_view value U_LIFETIME_BOUND)
{
    std::optional<std::string_view> bcpType = ulocimp_ಗಮ್ಯ_Bcp_Type(keyword, value);
    ಆದರೆ (!bcpType.has_value() &&
        ultag_isUnicodeLocaleType(value.ದತ್ತಾಂಶ(), static_cast<int32_t>(value.size()))) {
        // unknown keyword, but syntax is fine..
        ಹಿಂತಿರುಗಿ value;
    }
    ಹಿಂತಿರುಗಿ bcpType;
}

ನಾಮಾಕಾಶ {

ತಾರ್ಕಿಕ
is_Well_Formed_Legacy_ಕೀಲಿ(std::string_view ಕೀಲಿ)
{
    ಹಿಂತಿರುಗಿ std::all_of(ಕೀಲಿ.begin(), ಕೀಲಿ.ಕೊನೆ(), UPRV_ISALPHANUM);
}

ತಾರ್ಕಿಕ
isWellFormedLegacyType(std::string_view legacyType)
{
    int32_t alpha_ಅಂಕೆ_Len = 0;
    ಗಾಗಿ (ಅಕ್ಷರ c : legacyType) {
        ಆದರೆ (c == '_' || c == '/' || c == '-') {
            ಆದರೆ (alpha_ಅಂಕೆ_Len == 0) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            alpha_ಅಂಕೆ_Len = 0;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (UPRV_ISALPHANUM(c)) {
            alpha_ಅಂಕೆ_Len++;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
    ಹಿಂತಿರುಗಿ alpha_ಅಂಕೆ_Len != 0;
}

}  // namespace

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
uloc_ಗಮ್ಯ_Legacy_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* keyword U_LIFETIME_BOUND)
{
    ಆದರೆ (keyword == ಶೂನ್ಯಸೂಚಕ || *keyword == '\0') { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    std::optional<std::string_view> ಫಲಿತಾಂಶ = ulocimp_ಗಮ್ಯ_Legacy_ಕೀಲಿ_With_Fallback(keyword);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ.has_value() ? ಫಲಿತಾಂಶ->ದತ್ತಾಂಶ() : ಶೂನ್ಯಸೂಚಕ;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_ಗಮ್ಯ_Legacy_ಕೀಲಿ_With_Fallback(std::string_view keyword U_LIFETIME_BOUND)
{
    std::optional<std::string_view> legacy_ಕೀಲಿ_2 = ulocimp_ಗಮ್ಯ_Legacy_ಕೀಲಿ(keyword);
    ಆದರೆ (!legacy_ಕೀಲಿ_2.has_value() && is_Well_Formed_Legacy_ಕೀಲಿ(keyword)) {
        // Checks if the specified locale key is well-formed with the legacy locale syntax.
        //
        // Note:
        //  LDML/CLDR provides some definition of keyword syntax in
        //  * http://www.unicode.org/reports/tr35/#Unicode_locale_identifier and
        //  * http://www.unicode.org/reports/tr35/#Old_Locale_Extension_Syntax
        //  Keys can only consist of [0-9a-zA-Z].
        ಹಿಂತಿರುಗಿ keyword;
    }
    ಹಿಂತಿರುಗಿ legacy_ಕೀಲಿ_2;
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
uloc_ಗಮ್ಯ_Legacy_Type(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* keyword, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* value U_LIFETIME_BOUND)
{
    ಆದರೆ (keyword == ಶೂನ್ಯಸೂಚಕ || *keyword == '\0' ||
        value == ಶೂನ್ಯಸೂಚಕ || *value == '\0') { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    std::optional<std::string_view> ಫಲಿತಾಂಶ = ulocimp_ಗಮ್ಯ_Legacy_Type_With_Fallback(keyword, value);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ.has_value() ? ಫಲಿತಾಂಶ->ದತ್ತಾಂಶ() : ಶೂನ್ಯಸೂಚಕ;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_ಗಮ್ಯ_Legacy_Type_With_Fallback(std::string_view keyword, std::string_view value U_LIFETIME_BOUND)
{
    std::optional<std::string_view> legacyType = ulocimp_ಗಮ್ಯ_Legacy_Type(keyword, value);
    ಆದರೆ (!legacyType.has_value() && isWellFormedLegacyType(value)) {
        // Checks if the specified locale type is well-formed with the legacy locale syntax.
        //
        // Note:
        //  LDML/CLDR provides some definition of keyword syntax in
        //  * http://www.unicode.org/reports/tr35/#Unicode_locale_identifier and
        //  * http://www.unicode.org/reports/tr35/#Old_Locale_Extension_Syntax
        //  Values (types) can only consist of [0-9a-zA-Z], plus for legacy values
        //  we allow [/_-+] in the middle (e.g. "Etc/GMT+1", "Asia/Tel_Aviv")
        ಹಿಂತಿರುಗಿ value;
    }
    ಹಿಂತಿರುಗಿ legacyType;
}

/*eof*/
