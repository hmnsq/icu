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

#ਅਨ੍ਤਰ੍ਭਾਵ <algorithm>
#ਅਨ੍ਤਰ੍ਭਾਵ <optional>
#ਅਨ੍ਤਰ੍ਭਾਵ <string_view>

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/bytestream.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/errorcode.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/stringpiece.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uloc.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "bytesinkutil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "putilimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ulocimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "locmap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uarrsort.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uenumimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"

U_NAMESPACE_USE

/* ### Declarations **************************************************/

/* Locale stuff from locid.cpp */
U_CFUNC ਖਾਲੀ locale_ਸੈੱਟ_default(ਅਚਲ ਅੱਖਰ *id);
U_CFUNC ਅਚਲ ਅੱਖਰ *locale_get_default();

ਨਾਮਾਕਾਸ਼ {

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
constexpr ਅਚਲ ਅੱਖਰ* LANGUAGES[] = {
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
ਖਾਲੀ_ਸੂਚਕ,
    "in",  "iw",  "ji",  "jw",  "mo",  "sh",  "swc", "tl",  /* obsolete language codes */
ਖਾਲੀ_ਸੂਚਕ
};

constexpr ਅਚਲ ਅੱਖਰ* DEPRECATED_LANGUAGES[]={
    "in", "iw", "ji", "jw", "mo", ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ
};
constexpr ਅਚਲ ਅੱਖਰ* REPLACEMENT_LANGUAGES[]={
    "id", "he", "yi", "jv", "ro", ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ
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
constexpr ਅਚਲ ਅੱਖਰ* LANGUAGES_3[] = {
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
ਖਾਲੀ_ਸੂਚਕ,
/*  "in",  "iw",  "ji",  "jw",  "mo",  "sh",  "swc", "tl",  */
    "ind", "heb", "yid", "jaw", "mol", "srp", "swc", "tgl",
ਖਾਲੀ_ਸੂਚਕ
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
constexpr ਅਚਲ ਅੱਖਰ* COUNTRIES[] = {
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
ਖਾਲੀ_ਸੂਚਕ,
    "AN",  "BU", "CS", "FX", "RO", "SU", "TP", "YD", "YU", "ZR",   /* obsolete country codes */
ਖਾਲੀ_ਸੂਚਕ
};

constexpr ਅਚਲ ਅੱਖਰ* DEPRECATED_COUNTRIES[] = {
    "AN", "BU", "CS", "DD", "DY", "FX", "HV", "NH", "RH", "SU", "TP", "UK", "VD", "YD", "YU", "ZR", ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ /* deprecated country list */
};
constexpr ਅਚਲ ਅੱਖਰ* REPLACEMENT_COUNTRIES[] = {
/*  "AN", "BU", "CS", "DD", "DY", "FX", "HV", "NH", "RH", "SU", "TP", "UK", "VD", "YD", "YU", "ZR" */
    "CW", "MM", "RS", "DE", "BJ", "FR", "BF", "VU", "ZW", "RU", "TL", "GB", "VN", "YE", "RS", "CD", ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ  /* replacement country codes */
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
constexpr ਅਚਲ ਅੱਖਰ* COUNTRIES_3[] = {
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
ਖਾਲੀ_ਸੂਚਕ,
/*  "AN",  "BU",  "CS",  "FX",  "RO", "SU",  "TP",  "YD",  "YU",  "ZR" */
    "ANT", "BUR", "SCG", "FXX", "ROM", "SUN", "TMP", "YMD", "YUG", "ZAR",
ਖਾਲੀ_ਸੂਚਕ
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ CanonicalizationMap {
    ਅਚਲ ਅੱਖਰ *id;          /* input ID */
    ਅਚਲ ਅੱਖਰ *canonicalID; /* canonicalized output ID */
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
int32_t get_ਸਭ_ਤੋਂ_ਥੋੜ੍ਹਾ_Subtag_ਲੰਮਾਈ(std::string_view localeID) {
    int32_t localeIDLength = static_cast<int32_t>(localeID.length());
    int32_t length = localeIDLength;
    int32_t tmp_ਲੰਮਾਈ = 0;
    int32_t i;
    ਤਾਰਕਿਕ reset = ਸੱਚ;

    ਲਈ (i = 0; i < localeIDLength; i++) {
        ਜੇ (localeID[i] != '_' && localeID[i] != '-') {
            ਜੇ (reset) {
                tmp_ਲੰਮਾਈ = 0;
                reset = ਝੂਠ;
            }
            tmp_ਲੰਮਾਈ++;
        } ਨਹੀਂਤਾਂ {
            ਜੇ (tmp_ਲੰਮਾਈ != 0 && tmp_ਲੰਮਾਈ < length) {
                length = tmp_ਲੰਮਾਈ;
            }
            reset = ਸੱਚ;
        }
    }

    ਵਾਪਸ length;
}
/* Test if the locale id has BCP47 u extension and does not have '@' */
ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ _hasBCP47Extension(std::string_view id) {
    ਵਾਪਸ id.find('@') == std::string_view::npos && get_ਸਭ_ਤੋਂ_ਥੋੜ੍ਹਾ_Subtag_ਲੰਮਾਈ(id) == 1;
}

/* ### Keywords **************************************************/
ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ UPRV_ISDIGIT(ਅੱਖਰ c) { ਵਾਪਸ c >= '0' && c <= '9'; }
ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ UPRV_ISALPHANUM(ਅੱਖਰ c) { ਵਾਪਸ uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }
/* Punctuation/symbols allowed in legacy key values */
ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ UPRV_OK_ਮੁੱਲ_PUNCTUATION(ਅੱਖਰ c) { ਵਾਪਸ c == '_' || c == '-' || c == '+' || c == '/'; }

}  // namespace

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ULOC_KEYWORD_ਬਫਰ_LEN 25
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ULOC_MAX_NO_KEYWORDS 25

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
locale_get_Keywords_ਸ਼ੁਰੂ(std::string_view localeID U_LIFETIME_BOUND) {
    ਜੇ (size_t pos = localeID.find('@'); pos != std::string_view::npos) {
        ਵਾਪਸ localeID.ਅੰਕੜੇ() + pos;
    }
#ਜੇ (U_CHARSET_FAMILY == U_EBCDIC_FAMILY)
    ਨਹੀਂਤਾਂ {
        /* We do this because the @ sign is variant, and the @ sign used on one
        EBCDIC machine won't be compiled the same way on other EBCDIC based
        machines. */
        ਸਥਿਰ ਅਚਲ uint8_t ebcdicSigns[] = { 0x7C, 0x44, 0x66, 0x80, 0xAC, 0xAE, 0xAF, 0xB5, 0xEC, 0xEF, 0x00 };
        ਅਚਲ uint8_t *char_ਮੰਜ਼ਿਲ_ਲੱਭੋ = ebcdicSigns;
        ਜਦੋਂ(*char_ਮੰਜ਼ਿਲ_ਲੱਭੋ) {
            ਜੇ (size_t pos = localeID.find(*char_ਮੰਜ਼ਿਲ_ਲੱਭੋ); pos != std::string_view::npos) {
                ਵਾਪਸ localeID.ਅੰਕੜੇ() + pos;
            }
            char_ਮੰਜ਼ਿਲ_ਲੱਭੋ++;
        }
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

ਨਾਮਾਕਾਸ਼ {

/**
 * @param keywordName incoming name to be canonicalized
 * @param status return status (keyword too long)
 * @return the keyword name
 */
CharString locale_canonKeywordName(std::string_view keywordName, UError_ਕੋਡ& ਸਥਿਤੀ)
{
  ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ {}; }
  CharString ਨਤੀਜਾ;

  ਲਈ (ਅੱਖਰ c : keywordName) {
    ਜੇ (!UPRV_ISALPHANUM(c)) {
      ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR; /* malformed keyword name */
      ਵਾਪਸ {};
    }
    ਨਤੀਜਾ.append(uprv_tolower(c), ਸਥਿਤੀ);
  }
  ਜੇ (ਨਤੀਜਾ.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
    ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR; /* empty keyword name */
    ਵਾਪਸ {};
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਅੱਖਰ keyword[ULOC_KEYWORD_ਬਫਰ_LEN];
    int32_t keywordLen;
    ਅਚਲ ਅੱਖਰ *ਮੁੱਲ_ਸ਼ੁਰੂ;
    int32_t ਮੁੱਲ_Len;
} KeywordStruct;

int32_t U_CALLCONV
compareKeywordStructs(ਅਚਲ ਖਾਲੀ * /*context*/, ਅਚਲ ਖਾਲੀ *ਖੱਬੇ, ਅਚਲ ਖਾਲੀ *ਸੱਜੇ) {
    ਅਚਲ ਅੱਖਰ* ਖੱਬਾ_String = static_cast<ਅਚਲ KeywordStruct*>(ਖੱਬੇ)->keyword;
    ਅਚਲ ਅੱਖਰ* ਸੱਜਾ_String = static_cast<ਅਚਲ KeywordStruct*>(ਸੱਜੇ)->keyword;
    ਵਾਪਸ uprv_strcmp(ਖੱਬਾ_String, ਸੱਜਾ_String);
}

}  // namespace

U_EXPORT CharString
ulocimp_getKeywords(std::string_view localeID,
                    ਅੱਖਰ prev,
                    ਤਾਰਕਿਕ ਮੁੱਲਾਂ_Too,
                    UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਵਾਪਸ ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Char_String(
        [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            ulocimp_getKeywords(localeID,
                                prev,
                                sink,
                                ਮੁੱਲਾਂ_Too,
                                ਸਥਿਤੀ);
        },
        ਸਥਿਤੀ);
}

U_EXPORT ਖਾਲੀ
ulocimp_getKeywords(std::string_view localeID,
                    ਅੱਖਰ prev,
                    ByteSink& sink,
                    ਤਾਰਕਿਕ ਮੁੱਲਾਂ_Too,
                    UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }

    KeywordStruct keywordList[ULOC_MAX_NO_KEYWORDS];

    int32_t maxKeywords = ULOC_MAX_NO_KEYWORDS;
    int32_t ਅੰਕ_Keywords = 0;
    size_t equalSign = std::string_view::npos;
    size_t semicolon = std::string_view::npos;
    int32_t i = 0, j, n;

    ਜੇ(prev == '@') { /* start of keyword definition */
        /* we will grab pairs, trim spaces, lowercase keywords, sort and return */
        ਕਰੋ {
            ਤਾਰਕਿਕ duplicate = ਝੂਠ;
            /* skip leading spaces */
            ਜਦੋਂ (!localeID.empty() && localeID.front() == ' ') {
                localeID.remove_ਅਗੇਤਰ(1);
            }
            ਜੇ (localeID.empty()) { /* handle trailing "; " */
                ਰੋਕੋ;
            }
            ਜੇ(ਅੰਕ_Keywords == maxKeywords) {
                ਸਥਿਤੀ = U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR;
                ਵਾਪਸ;
            }
            equalSign = localeID.find('=');
            semicolon = localeID.find(';');
            /* lack of '=' [foo@currency] is illegal */
            /* ';' before '=' [foo@currency;collation=pinyin] is illegal */
            ਜੇ (equalSign == std::string_view::npos ||
                (semicolon != std::string_view::npos && semicolon < equalSign)) {
                ਸਥਿਤੀ = U_INVALID_FORMAT_ERROR;
                ਵਾਪਸ;
            }
            /* zero-length keyword is an error. */
            ਜੇ (equalSign == 0) {
                ਸਥਿਤੀ = U_INVALID_FORMAT_ERROR;
                ਵਾਪਸ;
            }
            /* need to normalize both keyword and keyword name */
            ਜੇ (equalSign >= ULOC_KEYWORD_ਬਫਰ_LEN) {
                /* keyword name too long for internal buffer */
                ਸਥਿਤੀ = U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR;
                ਵਾਪਸ;
            }
            ਲਈ (i = 0, n = 0; static_cast<size_t>(i) < equalSign; ++i) {
                ਜੇ (localeID[i] != ' ') {
                    keywordList[ਅੰਕ_Keywords].keyword[n++] = uprv_tolower(localeID[i]);
                }
            }

            keywordList[ਅੰਕ_Keywords].keyword[n] = 0;
            keywordList[ਅੰਕ_Keywords].keywordLen = n;
            /* now grab the value part. First we skip the '=' */
            equalSign++;
            /* then we leading spaces */
            ਜਦੋਂ (equalSign < localeID.length() && localeID[equalSign] == ' ') {
                equalSign++;
            }

            /* Premature end or zero-length value */
            ਜੇ (equalSign == localeID.length() || equalSign == semicolon) {
                ਸਥਿਤੀ = U_INVALID_FORMAT_ERROR;
                ਵਾਪਸ;
            }

            keywordList[ਅੰਕ_Keywords].ਮੁੱਲ_ਸ਼ੁਰੂ = localeID.ਅੰਕੜੇ() + equalSign;

            std::string_view value = localeID;
            ਜੇ (semicolon != std::string_view::npos) {
                value.remove_suffix(value.length() - semicolon);
                localeID.remove_ਅਗੇਤਰ(semicolon + 1);
            } ਨਹੀਂਤਾਂ {
                localeID = {};
            }
            value.remove_ਅਗੇਤਰ(equalSign);
            ਜੇ (size_t ਆਖਰੀ = value.find_last_not_of(' '); ਆਖਰੀ != std::string_view::npos) {
                value.remove_suffix(value.length() - ਆਖਰੀ - 1);
            }
            keywordList[ਅੰਕ_Keywords].ਮੁੱਲ_Len = static_cast<int32_t>(value.length());

            /* If this is a duplicate keyword, then ignore it */
            ਲਈ (j=0; j<ਅੰਕ_Keywords; ++j) {
                ਜੇ (uprv_strcmp(keywordList[j].keyword, keywordList[ਅੰਕ_Keywords].keyword) == 0) {
                    duplicate = ਸੱਚ;
                    ਰੋਕੋ;
                }
            }
            ਜੇ (!duplicate) {
                ++ਅੰਕ_Keywords;
            }
        } ਜਦੋਂ (!localeID.empty());

        /* now we have a list of keywords */
        /* we need to sort it */
        uprv_ਛਾਂਟੋ_Array(keywordList, ਅੰਕ_Keywords, ਆਕਾਰ(KeywordStruct), compareKeywordStructs, ਖਾਲੀ_ਸੂਚਕ, ਝੂਠ, &ਸਥਿਤੀ);

        /* Now construct the keyword part */
        ਲਈ(i = 0; i < ਅੰਕ_Keywords; i++) {
            sink.Append(keywordList[i].keyword, keywordList[i].keywordLen);
            ਜੇ(ਮੁੱਲਾਂ_Too) {
                sink.Append("=", 1);
                sink.Append(keywordList[i].ਮੁੱਲ_ਸ਼ੁਰੂ, keywordList[i].ਮੁੱਲ_Len);
                ਜੇ(i < ਅੰਕ_Keywords - 1) {
                    sink.Append(";", 1);
                }
            } ਨਹੀਂਤਾਂ {
                sink.Append("\0", 1);
            }
        }
    }
}

U_CAPI int32_t U_EXPORT2
uloc_get_Keyword_ਮੁੱਲ(ਅਚਲ ਅੱਖਰ* localeID,
                     ਅਚਲ ਅੱਖਰ* keywordName,
                     ਅੱਖਰ* ਬਫਰ, int32_t ਬਫਰ_ਸਮਰੱਥਾ,
                     UError_ਕੋਡ* ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) { ਵਾਪਸ 0; }
    ਜੇ (keywordName == ਖਾਲੀ_ਸੂਚਕ || *keywordName == '\0') {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    ਵਾਪਸ ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Terminated_Chars(
        ਬਫਰ, ਬਫਰ_ਸਮਰੱਥਾ,
        [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            ulocimp_get_Keyword_ਮੁੱਲ(localeID, keywordName, sink, ਸਥਿਤੀ);
        },
        *ਸਥਿਤੀ);
}

U_EXPORT CharString
ulocimp_get_Keyword_ਮੁੱਲ(ਅਚਲ ਅੱਖਰ* localeID,
                        std::string_view keywordName,
                        UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਵਾਪਸ ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Char_String(
        [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            ulocimp_get_Keyword_ਮੁੱਲ(localeID, keywordName, sink, ਸਥਿਤੀ);
        },
        ਸਥਿਤੀ);
}

U_EXPORT ਖਾਲੀ
ulocimp_get_Keyword_ਮੁੱਲ(ਅਚਲ ਅੱਖਰ* localeID,
                        std::string_view keywordName,
                        icu::ByteSink& sink,
                        UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }

    ਜੇ (localeID == ਖਾਲੀ_ਸੂਚਕ || keywordName.empty()) {
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    ਅਚਲ ਅੱਖਰ* ਸ਼ੁਰੂ_ਖੋਜ_ਇੱਥੇ = ਖਾਲੀ_ਸੂਚਕ;
    ਅਚਲ ਅੱਖਰ* ਅਗਲਾ_Separator = ਖਾਲੀ_ਸੂਚਕ;

    CharString ਆਰਜ਼ੀ_ਬਫਰ;
    ਅਚਲ ਅੱਖਰ* tmpLocaleID;

    CharString canonKeywordName = locale_canonKeywordName(keywordName, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
      ਵਾਪਸ;
    }

    ਜੇ (localeID != ਖਾਲੀ_ਸੂਚਕ && _hasBCP47Extension(localeID)) {
        ਆਰਜ਼ੀ_ਬਫਰ = ulocimp_for_Language_ਟੈਗ(localeID, -1, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);
        tmpLocaleID = U_SUCCESS(ਸਥਿਤੀ) && !ਆਰਜ਼ੀ_ਬਫਰ.is_ਖਾਲੀ_ਥਾਵਾਂ() ? ਆਰਜ਼ੀ_ਬਫਰ.ਅੰਕੜੇ() : localeID;
    } ਨਹੀਂਤਾਂ {
        tmpLocaleID=localeID;
    }

    ਸ਼ੁਰੂ_ਖੋਜ_ਇੱਥੇ = locale_get_Keywords_ਸ਼ੁਰੂ(tmpLocaleID);
    ਜੇ(ਸ਼ੁਰੂ_ਖੋਜ_ਇੱਥੇ == ਖਾਲੀ_ਸੂਚਕ) {
        /* no keywords, return at once */
        ਵਾਪਸ;
    }

    /* find the first keyword */
    ਜਦੋਂ(ਸ਼ੁਰੂ_ਖੋਜ_ਇੱਥੇ) {
        ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ_ਮੁੱਲ_ਪੂਛ;

        ਸ਼ੁਰੂ_ਖੋਜ_ਇੱਥੇ++; /* skip @ or ; */
        ਅਗਲਾ_Separator = uprv_strchr(ਸ਼ੁਰੂ_ਖੋਜ_ਇੱਥੇ, '=');
        ਜੇ(!ਅਗਲਾ_Separator) {
            ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR; /* key must have =value */
            ਵਾਪਸ;
        }
        /* strip leading & trailing spaces (TC decided to tolerate these) */
        ਜਦੋਂ(*ਸ਼ੁਰੂ_ਖੋਜ_ਇੱਥੇ == ' ') {
            ਸ਼ੁਰੂ_ਖੋਜ_ਇੱਥੇ++;
        }
        ਕੁੰਜੀ_ਮੁੱਲ_ਪੂਛ = ਅਗਲਾ_Separator;
        ਜਦੋਂ (ਕੁੰਜੀ_ਮੁੱਲ_ਪੂਛ > ਸ਼ੁਰੂ_ਖੋਜ_ਇੱਥੇ && *(ਕੁੰਜੀ_ਮੁੱਲ_ਪੂਛ-1) == ' ') {
            ਕੁੰਜੀ_ਮੁੱਲ_ਪੂਛ--;
        }
        /* now keyValueTail points to first char after the keyName */
        /* copy & normalize keyName from locale */
        ਜੇ (ਸ਼ੁਰੂ_ਖੋਜ_ਇੱਥੇ == ਕੁੰਜੀ_ਮੁੱਲ_ਪੂਛ) {
            ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR; /* empty keyword name in passed-in locale */
            ਵਾਪਸ;
        }
        CharString localeKeywordName;
        ਜਦੋਂ (ਸ਼ੁਰੂ_ਖੋਜ_ਇੱਥੇ < ਕੁੰਜੀ_ਮੁੱਲ_ਪੂਛ) {
          ਜੇ (!UPRV_ISALPHANUM(*ਸ਼ੁਰੂ_ਖੋਜ_ਇੱਥੇ)) {
            ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR; /* malformed keyword name */
            ਵਾਪਸ;
          }
          localeKeywordName.append(uprv_tolower(*ਸ਼ੁਰੂ_ਖੋਜ_ਇੱਥੇ++), ਸਥਿਤੀ);
        }
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਵਾਪਸ;
        }

        ਸ਼ੁਰੂ_ਖੋਜ_ਇੱਥੇ = uprv_strchr(ਅਗਲਾ_Separator, ';');

        ਜੇ (canonKeywordName == localeKeywordName) {
             /* current entry matches the keyword. */
           ਅਗਲਾ_Separator++; /* skip '=' */
            /* First strip leading & trailing spaces (TC decided to tolerate these) */
            ਜਦੋਂ(*ਅਗਲਾ_Separator == ' ') {
              ਅਗਲਾ_Separator++;
            }
            ਕੁੰਜੀ_ਮੁੱਲ_ਪੂਛ = (ਸ਼ੁਰੂ_ਖੋਜ_ਇੱਥੇ)? ਸ਼ੁਰੂ_ਖੋਜ_ਇੱਥੇ: ਅਗਲਾ_Separator + uprv_strlen(ਅਗਲਾ_Separator);
            ਜਦੋਂ(ਕੁੰਜੀ_ਮੁੱਲ_ਪੂਛ > ਅਗਲਾ_Separator && *(ਕੁੰਜੀ_ਮੁੱਲ_ਪੂਛ-1) == ' ') {
              ਕੁੰਜੀ_ਮੁੱਲ_ਪੂਛ--;
            }
            /* Now copy the value, but check well-formedness */
            ਜੇ (ਅਗਲਾ_Separator == ਕੁੰਜੀ_ਮੁੱਲ_ਪੂਛ) {
              ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR; /* empty key value name in passed-in locale */
              ਵਾਪਸ;
            }
            ਜਦੋਂ (ਅਗਲਾ_Separator < ਕੁੰਜੀ_ਮੁੱਲ_ਪੂਛ) {
              ਜੇ (!UPRV_ISALPHANUM(*ਅਗਲਾ_Separator) && !UPRV_OK_ਮੁੱਲ_PUNCTUATION(*ਅਗਲਾ_Separator)) {
                ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR; /* malformed key value */
                ਵਾਪਸ;
              }
              /* Should we lowercase value to return here? Tests expect as-is. */
              sink.Append(ਅਗਲਾ_Separator++, 1);
            }
            ਵਾਪਸ;
        }
    }
}

U_CAPI int32_t U_EXPORT2
uloc_ਸੈੱਟ_Keyword_ਮੁੱਲ(ਅਚਲ ਅੱਖਰ* keywordName,
                     ਅਚਲ ਅੱਖਰ* keyword_ਮੁੱਲ,
                     ਅੱਖਰ* ਬਫਰ, int32_t ਬਫਰ_ਸਮਰੱਥਾ,
                     UError_ਕੋਡ* ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) { ਵਾਪਸ 0; }

    ਜੇ (keywordName == ਖਾਲੀ_ਸੂਚਕ || *keywordName == 0) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    ਜੇ (ਬਫਰ_ਸਮਰੱਥਾ <= 1) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    int32_t bufLen = (int32_t)uprv_strlen(ਬਫਰ);
    ਜੇ(ਬਫਰ_ਸਮਰੱਥਾ<bufLen) {
        /* The capacity is less than the length?! Is this NUL terminated? */
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    ਅੱਖਰ* keywords = const_cast<ਅੱਖਰ*>(
        locale_get_Keywords_ਸ਼ੁਰੂ({ਬਫਰ, static_cast<std::string_view::size_type>(bufLen)}));
    int32_t ਅਧਾਰ_Len = keywords == ਖਾਲੀ_ਸੂਚਕ ? bufLen : keywords - ਬਫਰ;
    // Remove -1 from the capacity so that this function can guarantee NUL termination.
    CheckedArrayByteSink sink(keywords == ਖਾਲੀ_ਸੂਚਕ ? ਬਫਰ + bufLen : keywords,
                              ਬਫਰ_ਸਮਰੱਥਾ - ਅਧਾਰ_Len - 1);
    int32_t reslen = ulocimp_ਸੈੱਟ_Keyword_ਮੁੱਲ(
        keywords == ਖਾਲੀ_ਸੂਚਕ ? std::string_view() : keywords,
        keywordName,
        keyword_ਮੁੱਲ == ਖਾਲੀ_ਸੂਚਕ ? std::string_view() : keyword_ਮੁੱਲ,
        sink,
        *ਸਥਿਤੀ);

    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ *ਸਥਿਤੀ == U_ਬਫਰ_ਉਛਾਲ_ERROR ? reslen + ਅਧਾਰ_Len : 0;
    }

    // See the documentation for this function, it's guaranteed to never
    // overflow the buffer but instead abort with BUFFER_OVERFLOW_ERROR.
    // In this case, nothing has been written to the sink, so it cannot have Overflowed().
    U_ASSERT(!sink.Overflowed());
    U_ASSERT(reslen >= 0);
    ਵਾਪਸ u_terminateChars(ਬਫਰ, ਬਫਰ_ਸਮਰੱਥਾ, reslen + ਅਧਾਰ_Len, ਸਥਿਤੀ);
}

U_EXPORT ਖਾਲੀ
ulocimp_ਸੈੱਟ_Keyword_ਮੁੱਲ(std::string_view keywordName,
                        std::string_view keyword_ਮੁੱਲ,
                        CharString& localeID,
                        UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }
    std::string_view keywords;
    ਜੇ (ਅਚਲ ਅੱਖਰ* ਸ਼ੁਰੂ = locale_get_Keywords_ਸ਼ੁਰੂ(localeID.ਮੰਜ਼ਿਲ_String_Piece()); ਸ਼ੁਰੂ != ਖਾਲੀ_ਸੂਚਕ) {
        // This is safe because CharString::truncate() doesn't actually erase any
        // data, but simply sets the position for where new data will be written.
        int32_t size = ਸ਼ੁਰੂ - localeID.ਅੰਕੜੇ();
        keywords = localeID.ਮੰਜ਼ਿਲ_String_Piece();
        keywords.remove_ਅਗੇਤਰ(size);
        localeID.truncate(size);
    }
    CharStringByteSink sink(&localeID);
    ulocimp_ਸੈੱਟ_Keyword_ਮੁੱਲ(keywords, keywordName, keyword_ਮੁੱਲ, sink, ਸਥਿਤੀ);
}

U_EXPORT int32_t
ulocimp_ਸੈੱਟ_Keyword_ਮੁੱਲ(std::string_view keywords,
                        std::string_view keywordName,
                        std::string_view keyword_ਮੁੱਲ,
                        ByteSink& sink,
                        UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ 0; }

    /* TODO: sorting. removal. */
    int32_t needLen = 0;
    int32_t rc;
    CharString updated_ਕੁੰਜੀਆਂ_And_ਮੁੱਲਾਂ;
    ਤਾਰਕਿਕ handled_ਨਿਵੇਸ਼_ਕੁੰਜੀ_And_ਮੁੱਲ = ਝੂਠ;
    ਅੱਖਰ ਕੁੰਜੀ_ਮੁੱਲ_ਅਗੇਤਰ = '@';

    ਜੇ (ਸਥਿਤੀ == U_STRING_NOT_TERMINATED_WARNING) {
        ਸਥਿਤੀ = U_ZERO_ERROR;
    }
    ਜੇ (keywordName.empty()) {
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    CharString canonKeywordName = locale_canonKeywordName(keywordName, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ 0;
    }

    CharString canon_Keyword_ਮੁੱਲ;
    ਲਈ (ਅੱਖਰ c : keyword_ਮੁੱਲ) {
        ਜੇ (!UPRV_ISALPHANUM(c) && !UPRV_OK_ਮੁੱਲ_PUNCTUATION(c)) {
            ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR; /* malformed key value */
            ਵਾਪਸ 0;
        }
        /* Should we force lowercase in value to set? */
        canon_Keyword_ਮੁੱਲ.append(c, ਸਥਿਤੀ);
    }
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ 0;
    }

    ਜੇ (keywords.size() <= 1) {
        ਜੇ (canon_Keyword_ਮੁੱਲ.is_ਖਾਲੀ_ਥਾਵਾਂ()) { /* no keywords = nothing to remove */
            U_ASSERT(ਸਥਿਤੀ != U_STRING_NOT_TERMINATED_WARNING);
            ਵਾਪਸ 0;
        }

        needLen = 1 + canonKeywordName.length() + 1 + canon_Keyword_ਮੁੱਲ.length();
        int32_t capacity = 0;
        ਅੱਖਰ* ਬਫਰ = sink.Get_Append_ਬਫਰ(
                needLen, needLen, ਖਾਲੀ_ਸੂਚਕ, needLen, &capacity);
        ਜੇ (capacity < needLen || ਬਫਰ == ਖਾਲੀ_ਸੂਚਕ) {
            ਸਥਿਤੀ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਵਾਪਸ needLen; /* no change */
        }
        ਅੱਖਰ* it = ਬਫਰ;

        *it++ = '@';
        uprv_memcpy(it, canonKeywordName.ਅੰਕੜੇ(), canonKeywordName.length());
        it += canonKeywordName.length();
        *it++ = '=';
        uprv_memcpy(it, canon_Keyword_ਮੁੱਲ.ਅੰਕੜੇ(), canon_Keyword_ਮੁੱਲ.length());
        sink.Append(ਬਫਰ, needLen);
        U_ASSERT(ਸਥਿਤੀ != U_STRING_NOT_TERMINATED_WARNING);
        ਵਾਪਸ needLen;
    } /* end shortcut - no @ */

    /* search for keyword */
    ਲਈ (size_t keyword_ਸ਼ੁਰੂ = 0; keyword_ਸ਼ੁਰੂ != std::string_view::npos;) {
        keyword_ਸ਼ੁਰੂ++; /* skip @ or ; */
        size_t ਅਗਲਾ_Equalsign = keywords.find('=', keyword_ਸ਼ੁਰੂ);
        ਜੇ (ਅਗਲਾ_Equalsign == std::string_view::npos) {
            ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR; /* key must have =value */
            ਵਾਪਸ 0;
        }
        /* strip leading & trailing spaces (TC decided to tolerate these) */
        ਜਦੋਂ (keyword_ਸ਼ੁਰੂ < keywords.size() && keywords[keyword_ਸ਼ੁਰੂ] == ' ') {
            keyword_ਸ਼ੁਰੂ++;
        }
        size_t ਕੁੰਜੀ_ਮੁੱਲ_ਪੂਛ = ਅਗਲਾ_Equalsign;
        ਜਦੋਂ (ਕੁੰਜੀ_ਮੁੱਲ_ਪੂਛ > keyword_ਸ਼ੁਰੂ && keywords[ਕੁੰਜੀ_ਮੁੱਲ_ਪੂਛ - 1] == ' ') {
            ਕੁੰਜੀ_ਮੁੱਲ_ਪੂਛ--;
        }
        /* now keyValueTail points to first char after the keyName */
        /* copy & normalize keyName from locale */
        ਜੇ (keyword_ਸ਼ੁਰੂ == ਕੁੰਜੀ_ਮੁੱਲ_ਪੂਛ) {
            ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR; /* empty keyword name in passed-in locale */
            ਵਾਪਸ 0;
        }
        CharString localeKeywordName;
        ਜਦੋਂ (keyword_ਸ਼ੁਰੂ < ਕੁੰਜੀ_ਮੁੱਲ_ਪੂਛ) {
            ਜੇ (!UPRV_ISALPHANUM(keywords[keyword_ਸ਼ੁਰੂ])) {
                ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR; /* malformed keyword name */
                ਵਾਪਸ 0;
            }
            localeKeywordName.append(uprv_tolower(keywords[keyword_ਸ਼ੁਰੂ++]), ਸਥਿਤੀ);
        }
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਵਾਪਸ 0;
        }

        size_t ਅਗਲਾ_Separator = keywords.find(';', ਅਗਲਾ_Equalsign);

        /* start processing the value part */
        ਅਗਲਾ_Equalsign++; /* skip '=' */
        /* First strip leading & trailing spaces (TC decided to tolerate these) */
        ਜਦੋਂ (ਅਗਲਾ_Equalsign < keywords.size() && keywords[ਅਗਲਾ_Equalsign] == ' ') {
            ਅਗਲਾ_Equalsign++;
        }
        ਕੁੰਜੀ_ਮੁੱਲ_ਪੂਛ = ਅਗਲਾ_Separator == std::string_view::npos ? keywords.size() : ਅਗਲਾ_Separator;
        ਜਦੋਂ (ਕੁੰਜੀ_ਮੁੱਲ_ਪੂਛ > ਅਗਲਾ_Equalsign && keywords[ਕੁੰਜੀ_ਮੁੱਲ_ਪੂਛ - 1] == ' ') {
            ਕੁੰਜੀ_ਮੁੱਲ_ਪੂਛ--;
        }
        ਜੇ (ਅਗਲਾ_Equalsign == ਕੁੰਜੀ_ਮੁੱਲ_ਪੂਛ) {
            ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR; /* empty key value in passed-in locale */
            ਵਾਪਸ 0;
        }

        rc = uprv_strcmp(canonKeywordName.ਅੰਕੜੇ(), localeKeywordName.ਅੰਕੜੇ());
        ਜੇ(rc == 0) {
            /* Current entry matches the input keyword. Update the entry */
            ਜੇ (!canon_Keyword_ਮੁੱਲ.is_ਖਾਲੀ_ਥਾਵਾਂ()) { /* updating a value */
                updated_ਕੁੰਜੀਆਂ_And_ਮੁੱਲਾਂ.append(ਕੁੰਜੀ_ਮੁੱਲ_ਅਗੇਤਰ, ਸਥਿਤੀ);
                ਕੁੰਜੀ_ਮੁੱਲ_ਅਗੇਤਰ = ';'; /* for any subsequent key-value pair */
                updated_ਕੁੰਜੀਆਂ_And_ਮੁੱਲਾਂ.append(canonKeywordName, ਸਥਿਤੀ);
                updated_ਕੁੰਜੀਆਂ_And_ਮੁੱਲਾਂ.append('=', ਸਥਿਤੀ);
                updated_ਕੁੰਜੀਆਂ_And_ਮੁੱਲਾਂ.append(canon_Keyword_ਮੁੱਲ, ਸਥਿਤੀ);
            } /* else removing this entry, don't emit anything */
            handled_ਨਿਵੇਸ਼_ਕੁੰਜੀ_And_ਮੁੱਲ = ਸੱਚ;
        } ਨਹੀਂਤਾਂ {
           /* input keyword sorts earlier than current entry, add before current entry */
            ਜੇ (rc < 0 && !canon_Keyword_ਮੁੱਲ.is_ਖਾਲੀ_ਥਾਵਾਂ() && !handled_ਨਿਵੇਸ਼_ਕੁੰਜੀ_And_ਮੁੱਲ) {
                /* insert new entry at this location */
                updated_ਕੁੰਜੀਆਂ_And_ਮੁੱਲਾਂ.append(ਕੁੰਜੀ_ਮੁੱਲ_ਅਗੇਤਰ, ਸਥਿਤੀ);
                ਕੁੰਜੀ_ਮੁੱਲ_ਅਗੇਤਰ = ';'; /* for any subsequent key-value pair */
                updated_ਕੁੰਜੀਆਂ_And_ਮੁੱਲਾਂ.append(canonKeywordName, ਸਥਿਤੀ);
                updated_ਕੁੰਜੀਆਂ_And_ਮੁੱਲਾਂ.append('=', ਸਥਿਤੀ);
                updated_ਕੁੰਜੀਆਂ_And_ਮੁੱਲਾਂ.append(canon_Keyword_ਮੁੱਲ, ਸਥਿਤੀ);
                handled_ਨਿਵੇਸ਼_ਕੁੰਜੀ_And_ਮੁੱਲ = ਸੱਚ;
            }
            /* copy the current entry */
            updated_ਕੁੰਜੀਆਂ_And_ਮੁੱਲਾਂ.append(ਕੁੰਜੀ_ਮੁੱਲ_ਅਗੇਤਰ, ਸਥਿਤੀ);
            ਕੁੰਜੀ_ਮੁੱਲ_ਅਗੇਤਰ = ';'; /* for any subsequent key-value pair */
            updated_ਕੁੰਜੀਆਂ_And_ਮੁੱਲਾਂ.append(localeKeywordName, ਸਥਿਤੀ);
            updated_ਕੁੰਜੀਆਂ_And_ਮੁੱਲਾਂ.append('=', ਸਥਿਤੀ);
            updated_ਕੁੰਜੀਆਂ_And_ਮੁੱਲਾਂ.append(keywords.ਅੰਕੜੇ() + ਅਗਲਾ_Equalsign,
                                        static_cast<int32_t>(ਕੁੰਜੀ_ਮੁੱਲ_ਪੂਛ - ਅਗਲਾ_Equalsign), ਸਥਿਤੀ);
        }
        ਜੇ (ਅਗਲਾ_Separator == std::string_view::npos && !canon_Keyword_ਮੁੱਲ.is_ਖਾਲੀ_ਥਾਵਾਂ() && !handled_ਨਿਵੇਸ਼_ਕੁੰਜੀ_And_ਮੁੱਲ) {
            /* append new entry at the end, it sorts later than existing entries */
            updated_ਕੁੰਜੀਆਂ_And_ਮੁੱਲਾਂ.append(ਕੁੰਜੀ_ਮੁੱਲ_ਅਗੇਤਰ, ਸਥਿਤੀ);
            /* skip keyValuePrefix update, no subsequent key-value pair */
            updated_ਕੁੰਜੀਆਂ_And_ਮੁੱਲਾਂ.append(canonKeywordName, ਸਥਿਤੀ);
            updated_ਕੁੰਜੀਆਂ_And_ਮੁੱਲਾਂ.append('=', ਸਥਿਤੀ);
            updated_ਕੁੰਜੀਆਂ_And_ਮੁੱਲਾਂ.append(canon_Keyword_ਮੁੱਲ, ਸਥਿਤੀ);
            handled_ਨਿਵੇਸ਼_ਕੁੰਜੀ_And_ਮੁੱਲ = ਸੱਚ;
        }
        keyword_ਸ਼ੁਰੂ = ਅਗਲਾ_Separator;
    } /* end loop searching */

    /* Any error from updatedKeysAndValues.append above would be internal and not due to
     * problems with the passed-in locale. So if we did encounter problems with the
     * passed-in locale above, those errors took precedence and overrode any error
     * status from updatedKeysAndValues.append, and also caused a return of 0. If there
     * are errors here they are from updatedKeysAndValues.append; they do cause an
     * error return but the passed-in locale is unmodified and the original bufLen is
     * returned.
     */
    ਜੇ (!handled_ਨਿਵੇਸ਼_ਕੁੰਜੀ_And_ਮੁੱਲ || U_FAILURE(ਸਥਿਤੀ)) {
        /* if input key/value specified removal of a keyword not present in locale, or
         * there was an error in CharString.append, leave original locale alone. */
        U_ASSERT(ਸਥਿਤੀ != U_STRING_NOT_TERMINATED_WARNING);
        // The sink is expected to be a buffer which already contains the full
        // locale string, so when it isn't going to be modified there's no need
        // to actually write any data to it, as the data is already there. Only
        // the first character needs to be overwritten (changing '\0' to '@').
        needLen = static_cast<int32_t>(keywords.size());
        int32_t capacity = 0;
        ਅੱਖਰ* ਬਫਰ = sink.Get_Append_ਬਫਰ(
                needLen, needLen, ਖਾਲੀ_ਸੂਚਕ, needLen, &capacity);
        ਜੇ (capacity < needLen || ਬਫਰ == ਖਾਲੀ_ਸੂਚਕ) {
            ਸਥਿਤੀ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
        } ਨਹੀਂਤਾਂ {
            *ਬਫਰ = '@';
            sink.Append(ਬਫਰ, needLen);
        }
        ਵਾਪਸ needLen;
    }

    needLen = updated_ਕੁੰਜੀਆਂ_And_ਮੁੱਲਾਂ.length();
    // Check to see can we fit the updatedKeysAndValues, if not, return
    // U_BUFFER_OVERFLOW_ERROR without copy updatedKeysAndValues into it.
    // We do this because this API function does not behave like most others:
    // It promises never to set a U_STRING_NOT_TERMINATED_WARNING.
    // When the contents fits but without the terminating NUL, in this case we need to not change
    // the buffer contents and return with a buffer overflow error.
    ਜੇ (needLen > 0) {
        int32_t capacity = 0;
        ਅੱਖਰ* ਬਫਰ = sink.Get_Append_ਬਫਰ(
                needLen, needLen, ਖਾਲੀ_ਸੂਚਕ, needLen, &capacity);
        ਜੇ (capacity < needLen || ਬਫਰ == ਖਾਲੀ_ਸੂਚਕ) {
            ਸਥਿਤੀ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਵਾਪਸ needLen;
        }
        uprv_memcpy(ਬਫਰ, updated_ਕੁੰਜੀਆਂ_And_ਮੁੱਲਾਂ.ਅੰਕੜੇ(), needLen);
        sink.Append(ਬਫਰ, needLen);
    }
    U_ASSERT(ਸਥਿਤੀ != U_STRING_NOT_TERMINATED_WARNING);
    ਵਾਪਸ needLen;
}

/* ### ID parsing implementation **************************************************/

ਨਾਮਾਕਾਸ਼ {

ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ is_ਅਗੇਤਰ_Letter(ਅੱਖਰ a) { ਵਾਪਸ a == 'x' || a == 'X' || a == 'i' || a == 'I'; }

/*returns true if one of the special prefixes is here (s=string)
  'x-' or 'i-' */
ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ _isIDPrefix(std::string_view s) {
    ਵਾਪਸ s.size() >= 2 && is_ਅਗੇਤਰ_Letter(s[0]) && _isIDSeparator(s[1]);
}

/* Dot terminates it because of POSIX form  where dot precedes the codepage
 * except for variant
 */
ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ _isTerminator(ਅੱਖਰ a) { ਵਾਪਸ a == '.' || a == '@'; }

ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ _isBCP47Extension(std::string_view p) {
    ਵਾਪਸ p.size() >= 3 &&
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
std::optional<int16_t> ਲੱਭੋ_ਸੂਚਕਾਂਕ_2(ਅਚਲ ਅੱਖਰ* ਅਚਲ* list, ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ)
{
    ਅਚਲ ਅੱਖਰ* ਅਚਲ* anchor = list;
    int32_t pass = 0;

    /* Make two passes through two nullptr-terminated arrays at 'list' */
    ਜਦੋਂ (pass++ < 2) {
        ਜਦੋਂ (*list) {
            ਜੇ (uprv_strcmp(ਕੁੰਜੀ, *list) == 0) {
                ਵਾਪਸ static_cast<int16_t>(list - anchor);
            }
            list++;
        }
        ++list;     /* skip final nullptr *CWB*/
    }
    ਵਾਪਸ std::nullopt;
}

}  // namespace

U_CFUNC ਅਚਲ ਅੱਖਰ*
uloc_getCurrentCountryID(ਅਚਲ ਅੱਖਰ* oldID U_LIFETIME_BOUND) {
    std::optional<int16_t> ਵਿੱਥ = ਲੱਭੋ_ਸੂਚਕਾਂਕ_2(DEPRECATED_COUNTRIES, oldID);
    ਵਾਪਸ ਵਿੱਥ.has_value() ? REPLACEMENT_COUNTRIES[*ਵਿੱਥ] : oldID;
}
U_CFUNC ਅਚਲ ਅੱਖਰ*
uloc_getCurrentLanguageID(ਅਚਲ ਅੱਖਰ* oldID U_LIFETIME_BOUND) {
    std::optional<int16_t> ਵਿੱਥ = ਲੱਭੋ_ਸੂਚਕਾਂਕ_2(DEPRECATED_LANGUAGES, oldID);
    ਵਾਪਸ ਵਿੱਥ.has_value() ? REPLACEMENT_LANGUAGES[*ਵਿੱਥ] : oldID;
}

ਨਾਮਾਕਾਸ਼ {

/*
 * the internal functions _getLanguage(), _getScript(), _getRegion(), _getVariant()
 * avoid duplicating code to handle the earlier locale ID pieces
 * in the functions for the later ones by
 * setting the *pEnd pointer to where they stopped parsing
 *
 * TODO try to use this in Locale
 */

size_t _getLanguage(std::string_view localeID, ByteSink* sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
    size_t skip = 0;
    ਜੇ (localeID.size() == 4 && uprv_strnicmp(localeID.ਅੰਕੜੇ(), "root", 4) == 0) {
        skip = 4;
        localeID.remove_ਅਗੇਤਰ(skip);
    } ਨਹੀਂਤਾਂ ਜੇ (localeID.size() >= 3 && uprv_strnicmp(localeID.ਅੰਕੜੇ(), "und", 3) == 0 &&
               (localeID.size() == 3 ||
                localeID[3] == '-' ||
                localeID[3] == '_' ||
                localeID[3] == '@')) {
        skip = 3;
        localeID.remove_ਅਗੇਤਰ(skip);
    }

    constexpr int32_t MAXLEN = ULOC_LANG_ਸਮਰੱਥਾ - 1;  // Minus NUL.

    /* if it starts with i- or x- then copy that prefix */
    size_t len = _isIDPrefix(localeID) ? 2 : 0;
    ਜਦੋਂ (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len])) {
        ਜੇ (len == MAXLEN) {
            ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ 0;
        }
        len++;
    }

    ਜੇ (sink == ਖਾਲੀ_ਸੂਚਕ || len == 0) { ਵਾਪਸ skip + len; }

    int32_t min_ਸਮਰੱਥਾ = uprv_max(static_cast<int32_t>(len), 4);  // Minimum 3 letters plus NUL.
    ਅੱਖਰ scratch[MAXLEN];
    int32_t capacity = 0;
    ਅੱਖਰ* ਬਫਰ = sink->Get_Append_ਬਫਰ(
            min_ਸਮਰੱਥਾ, min_ਸਮਰੱਥਾ, scratch, UPRV_LENGTHOF(scratch), &capacity);

    ਲਈ (size_t i = 0; i < len; ++i) {
        ਬਫਰ[i] = uprv_tolower(localeID[i]);
    }
    ਜੇ (localeID.size() >= 2 && _isIDSeparator(localeID[1])) {
        ਬਫਰ[1] = '-';
    }

    ਜੇ (len == 3) {
        /* convert 3 character code to 2 character code if possible *CWB*/
        U_ASSERT(capacity >= 4);
        ਬਫਰ[3] = '\0';
        std::optional<int16_t> ਵਿੱਥ = ਲੱਭੋ_ਸੂਚਕਾਂਕ_2(LANGUAGES_3, ਬਫਰ);
        ਜੇ (ਵਿੱਥ.has_value()) {
            ਅਚਲ ਅੱਖਰ* ਅਚਲ alias = LANGUAGES[*ਵਿੱਥ];
            sink->Append(alias, static_cast<int32_t>(uprv_strlen(alias)));
            ਵਾਪਸ skip + len;
        }
    }

    sink->Append(ਬਫਰ, static_cast<int32_t>(len));
    ਵਾਪਸ skip + len;
}

size_t _getScript(std::string_view localeID, ByteSink* sink) {
    constexpr int32_t ਲੰਮਾਈ_3 = 4;

    size_t len = 0;
    ਜਦੋਂ (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len]) &&
            uprv_isASCIILetter(localeID[len])) {
        ਜੇ (len == ਲੰਮਾਈ_3) { ਵਾਪਸ 0; }
        len++;
    }
    ਜੇ (len != ਲੰਮਾਈ_3) { ਵਾਪਸ 0; }

    ਜੇ (sink == ਖਾਲੀ_ਸੂਚਕ) { ਵਾਪਸ len; }

    ਅੱਖਰ scratch[ਲੰਮਾਈ_3];
    int32_t capacity = 0;
    ਅੱਖਰ* ਬਫਰ = sink->Get_Append_ਬਫਰ(
            ਲੰਮਾਈ_3, ਲੰਮਾਈ_3, scratch, UPRV_LENGTHOF(scratch), &capacity);

    ਬਫਰ[0] = uprv_toupper(localeID[0]);
    ਲਈ (int32_t i = 1; i < ਲੰਮਾਈ_3; ++i) {
        ਬਫਰ[i] = uprv_tolower(localeID[i]);
    }

    sink->Append(ਬਫਰ, ਲੰਮਾਈ_3);
    ਵਾਪਸ len;
}

size_t _getRegion(std::string_view localeID, ByteSink* sink) {
    constexpr int32_t MINLEN = 2;
    constexpr int32_t MAXLEN = ULOC_COUNTRY_ਸਮਰੱਥਾ - 1;  // Minus NUL.

    size_t len = 0;
    ਜਦੋਂ (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len])) {
        ਜੇ (len == MAXLEN) { ਵਾਪਸ 0; }
        len++;
    }
    ਜੇ (len < MINLEN) { ਵਾਪਸ 0; }

    ਜੇ (sink == ਖਾਲੀ_ਸੂਚਕ) { ਵਾਪਸ len; }

    ਅੱਖਰ scratch[ULOC_COUNTRY_ਸਮਰੱਥਾ];
    int32_t capacity = 0;
    ਅੱਖਰ* ਬਫਰ = sink->Get_Append_ਬਫਰ(
            ULOC_COUNTRY_ਸਮਰੱਥਾ,
            ULOC_COUNTRY_ਸਮਰੱਥਾ,
            scratch,
            UPRV_LENGTHOF(scratch),
            &capacity);

    ਲਈ (size_t i = 0; i < len; ++i) {
        ਬਫਰ[i] = uprv_toupper(localeID[i]);
    }

    ਜੇ (len == 3) {
        /* convert 3 character code to 2 character code if possible *CWB*/
        U_ASSERT(capacity >= 4);
        ਬਫਰ[3] = '\0';
        std::optional<int16_t> ਵਿੱਥ = ਲੱਭੋ_ਸੂਚਕਾਂਕ_2(COUNTRIES_3, ਬਫਰ);
        ਜੇ (ਵਿੱਥ.has_value()) {
            ਅਚਲ ਅੱਖਰ* ਅਚਲ alias = COUNTRIES[*ਵਿੱਥ];
            sink->Append(alias, static_cast<int32_t>(uprv_strlen(alias)));
            ਵਾਪਸ len;
        }
    }

    sink->Append(ਬਫਰ, static_cast<int32_t>(len));
    ਵਾਪਸ len;
}

/**
 * @param needSeparator if true, then add leading '_' if any variants
 * are added to 'variant'
 */
size_t
_getVariant(std::string_view localeID,
            ਅੱਖਰ prev,
            ByteSink* sink,
            ਤਾਰਕਿਕ needSeparator,
            UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ) || localeID.empty()) ਵਾਪਸ 0;

    // Reasonable upper limit for variants
    // There are no strict limitation of the syntax of variant in the legacy
    // locale format. If the locale is constructed from unicode_locale_id
    // as defined in UTS35, then we know each unicode_variant_subtag
    // could have max length of 8 ((alphanum{5,8} | digit alphanum{3})
    // 179 would allow 20 unicode_variant_subtag with sep in the
    // unicode_locale_id
    // 8*20 + 1*(20-1) = 179
    constexpr int32_t MAX_VARIANTS_ਲੰਮਾਈ = 179;

    /* get one or more variant tags and separate them with '_' */
    size_t ਸੂਚਕਾਂਕ = 0;
    ਜੇ (_isIDSeparator(prev)) {
        /* get a variant string after a '-' or '_' */
        ਲਈ (std::string_view sub = localeID;;) {
            size_t next = sub.find_first_of(".@_-");
            // For historical reasons, a trailing separator is included in the variant.
            ਤਾਰਕਿਕ ਮੁਕੰਮਲ = next == std::string_view::npos || next + 1 == sub.length();
            size_t ਹੱਦਬੰਦੀ = ਮੁਕੰਮਲ ? sub.length() : next;
            ਸੂਚਕਾਂਕ += ਹੱਦਬੰਦੀ;
            ਜੇ (ਸੂਚਕਾਂਕ > MAX_VARIANTS_ਲੰਮਾਈ) {
                ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
                ਵਾਪਸ 0;
            }

            ਜੇ (sink != ਖਾਲੀ_ਸੂਚਕ) {
                ਜੇ (needSeparator) {
                    sink->Append("_", 1);
                } ਨਹੀਂਤਾਂ {
                    needSeparator = ਸੱਚ;
                }

                int32_t length = static_cast<int32_t>(ਹੱਦਬੰਦੀ);
                int32_t min_ਸਮਰੱਥਾ = uprv_min(length, MAX_VARIANTS_ਲੰਮਾਈ);
                ਅੱਖਰ scratch[MAX_VARIANTS_ਲੰਮਾਈ];
                int32_t capacity = 0;
                ਅੱਖਰ* ਬਫਰ = sink->Get_Append_ਬਫਰ(
                        min_ਸਮਰੱਥਾ, min_ਸਮਰੱਥਾ, scratch, UPRV_LENGTHOF(scratch), &capacity);

                ਲਈ (size_t i = 0; i < ਹੱਦਬੰਦੀ; ++i) {
                    ਬਫਰ[i] = uprv_toupper(sub[i]);
                }
                sink->Append(ਬਫਰ, length);
            }

            ਜੇ (ਮੁਕੰਮਲ) { ਵਾਪਸ ਸੂਚਕਾਂਕ; }
            sub.remove_ਅਗੇਤਰ(next);
            ਜੇ (_isTerminator(sub.front()) || _isBCP47Extension(sub)) { ਵਾਪਸ ਸੂਚਕਾਂਕ; }
            sub.remove_ਅਗੇਤਰ(1);
            ਸੂਚਕਾਂਕ++;
        }
    }

    size_t skip = 0;
    /* if there is no variant tag after a '-' or '_' then look for '@' */
    ਜੇ (prev == '@') {
        /* keep localeID */
    } ਨਹੀਂਤਾਂ ਜੇ (ਅਚਲ ਅੱਖਰ* p = locale_get_Keywords_ਸ਼ੁਰੂ(localeID); p != ਖਾਲੀ_ਸੂਚਕ) {
        skip = 1 + p - localeID.ਅੰਕੜੇ(); /* point after the '@' */
        localeID.remove_ਅਗੇਤਰ(skip);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ 0;
    }
    ਲਈ (; ਸੂਚਕਾਂਕ < localeID.size() && !_isTerminator(localeID[ਸੂਚਕਾਂਕ]); ਸੂਚਕਾਂਕ++) {
        ਜੇ (ਸੂਚਕਾਂਕ >= MAX_VARIANTS_ਲੰਮਾਈ) { // same as length > MAX_VARIANTS_LENGTH
            ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ 0;
        }
        ਜੇ (needSeparator) {
            ਜੇ (sink != ਖਾਲੀ_ਸੂਚਕ) {
                sink->Append("_", 1);
            }
            needSeparator = ਝੂਠ;
        }
        ਜੇ (sink != ਖਾਲੀ_ਸੂਚਕ) {
            ਅੱਖਰ c = uprv_toupper(localeID[ਸੂਚਕਾਂਕ]);
            ਜੇ (c == '-' || c == ',') c = '_';
            sink->Append(&c, 1);
        }
    }
    ਵਾਪਸ skip + ਸੂਚਕਾਂਕ;
}

}  // namespace

U_EXPORT CharString
ulocimp_getLanguage(std::string_view localeID, UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਵਾਪਸ ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Char_String(
        [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            ulocimp_getSubtags(
                    localeID,
                    &sink,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਸਥਿਤੀ);
        },
        ਸਥਿਤੀ);
}

U_EXPORT CharString
ulocimp_getScript(std::string_view localeID, UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਵਾਪਸ ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Char_String(
        [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            ulocimp_getSubtags(
                    localeID,
                    ਖਾਲੀ_ਸੂਚਕ,
                    &sink,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਸਥਿਤੀ);
        },
        ਸਥਿਤੀ);
}

U_EXPORT CharString
ulocimp_getRegion(std::string_view localeID, UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਵਾਪਸ ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Char_String(
        [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            ulocimp_getSubtags(
                    localeID,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਖਾਲੀ_ਸੂਚਕ,
                    &sink,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਸਥਿਤੀ);
        },
        ਸਥਿਤੀ);
}

U_EXPORT CharString
ulocimp_getVariant(std::string_view localeID, UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਵਾਪਸ ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Char_String(
        [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            ulocimp_getSubtags(
                    localeID,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਖਾਲੀ_ਸੂਚਕ,
                    &sink,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਸਥਿਤੀ);
        },
        ਸਥਿਤੀ);
}

U_EXPORT ਖਾਲੀ
ulocimp_getSubtags(
        std::string_view localeID,
        CharString* language,
        CharString* script,
        CharString* region,
        CharString* variant,
        ਅਚਲ ਅੱਖਰ** p_ਸਿਰਾ,
        UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }

    std::optional<CharStringByteSink> languageSink;
    std::optional<CharStringByteSink> scriptSink;
    std::optional<CharStringByteSink> regionSink;
    std::optional<CharStringByteSink> variantSink;

    ਜੇ (language != ਖਾਲੀ_ਸੂਚਕ) { languageSink.emplace(language); }
    ਜੇ (script != ਖਾਲੀ_ਸੂਚਕ) { scriptSink.emplace(script); }
    ਜੇ (region != ਖਾਲੀ_ਸੂਚਕ) { regionSink.emplace(region); }
    ਜੇ (variant != ਖਾਲੀ_ਸੂਚਕ) { variantSink.emplace(variant); }

    ulocimp_getSubtags(
            localeID,
            languageSink.has_value() ? &*languageSink : ਖਾਲੀ_ਸੂਚਕ,
            scriptSink.has_value() ? &*scriptSink : ਖਾਲੀ_ਸੂਚਕ,
            regionSink.has_value() ? &*regionSink : ਖਾਲੀ_ਸੂਚਕ,
            variantSink.has_value() ? &*variantSink : ਖਾਲੀ_ਸੂਚਕ,
            p_ਸਿਰਾ,
            ਸਥਿਤੀ);
}

U_EXPORT ਖਾਲੀ
ulocimp_getSubtags(
        std::string_view localeID,
        ByteSink* language,
        ByteSink* script,
        ByteSink* region,
        ByteSink* variant,
        ਅਚਲ ਅੱਖਰ** p_ਸਿਰਾ,
        UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }

    ਜੇ (p_ਸਿਰਾ != ਖਾਲੀ_ਸੂਚਕ) {
        *p_ਸਿਰਾ = localeID.ਅੰਕੜੇ();
    } ਨਹੀਂਤਾਂ ਜੇ (language == ਖਾਲੀ_ਸੂਚਕ &&
               script == ਖਾਲੀ_ਸੂਚਕ &&
               region == ਖਾਲੀ_ਸੂਚਕ &&
               variant == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ;
    }

    ਜੇ (localeID.empty()) { ਵਾਪਸ; }

    ਤਾਰਕਿਕ hasRegion = ਝੂਠ;

    {
        size_t len = _getLanguage(localeID, language, ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }
        ਜੇ (len > 0) {
            localeID.remove_ਅਗੇਤਰ(len);
        }
    }

    ਜੇ (p_ਸਿਰਾ != ਖਾਲੀ_ਸੂਚਕ) {
        *p_ਸਿਰਾ = localeID.ਅੰਕੜੇ();
    } ਨਹੀਂਤਾਂ ਜੇ (script == ਖਾਲੀ_ਸੂਚਕ &&
               region == ਖਾਲੀ_ਸੂਚਕ &&
               variant == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ;
    }

    ਜੇ (localeID.empty()) { ਵਾਪਸ; }

    ਜੇ (_isIDSeparator(localeID.front())) {
        std::string_view sub = localeID;
        sub.remove_ਅਗੇਤਰ(1);
        size_t len = _getScript(sub, script);
        ਜੇ (len > 0) {
            localeID.remove_ਅਗੇਤਰ(len + 1);
            ਜੇ (p_ਸਿਰਾ != ਖਾਲੀ_ਸੂਚਕ) { *p_ਸਿਰਾ = localeID.ਅੰਕੜੇ(); }
        }
    }

    ਜੇ ((region == ਖਾਲੀ_ਸੂਚਕ && variant == ਖਾਲੀ_ਸੂਚਕ && p_ਸਿਰਾ == ਖਾਲੀ_ਸੂਚਕ) || localeID.empty()) { ਵਾਪਸ; }

    ਜੇ (_isIDSeparator(localeID.front())) {
        std::string_view sub = localeID;
        sub.remove_ਅਗੇਤਰ(1);
        size_t len = _getRegion(sub, region);
        ਜੇ (len > 0) {
            hasRegion = ਸੱਚ;
            localeID.remove_ਅਗੇਤਰ(len + 1);
            ਜੇ (p_ਸਿਰਾ != ਖਾਲੀ_ਸੂਚਕ) { *p_ਸਿਰਾ = localeID.ਅੰਕੜੇ(); }
        }
    }

    ਜੇ ((variant == ਖਾਲੀ_ਸੂਚਕ && p_ਸਿਰਾ == ਖਾਲੀ_ਸੂਚਕ) || localeID.empty()) { ਵਾਪਸ; }

    ਤਾਰਕਿਕ hasVariant = ਝੂਠ;

    ਜੇ (_isIDSeparator(localeID.front()) && !_isBCP47Extension(localeID)) {
        std::string_view sub = localeID;
        /* If there was no country ID, skip a possible extra IDSeparator */
        size_t skip = !hasRegion && localeID.size() > 1 && _isIDSeparator(localeID[1]) ? 2 : 1;
        sub.remove_ਅਗੇਤਰ(skip);
        size_t len = _getVariant(sub, localeID[0], variant, ਝੂਠ, ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }
        ਜੇ (len > 0) {
            hasVariant = ਸੱਚ;
            localeID.remove_ਅਗੇਤਰ(skip + len);
            ਜੇ (p_ਸਿਰਾ != ਖਾਲੀ_ਸੂਚਕ) { *p_ਸਿਰਾ = localeID.ਅੰਕੜੇ(); }
        }
    }

    ਜੇ ((variant == ਖਾਲੀ_ਸੂਚਕ && p_ਸਿਰਾ == ਖਾਲੀ_ਸੂਚਕ) || localeID.empty()) { ਵਾਪਸ; }

    ਜੇ (_isBCP47Extension(localeID)) {
        localeID.remove_ਅਗੇਤਰ(2);
        constexpr ਅੱਖਰ vaposix[] = "-va-posix";
        constexpr size_t length = ਆਕਾਰ vaposix - 1;
        ਲਈ (size_t next;; localeID.remove_ਅਗੇਤਰ(next)) {
            next = localeID.find('-', 1);
            ਜੇ (next == std::string_view::npos) { ਰੋਕੋ; }
            next = localeID.find('-', next + 1);
            ਤਾਰਕਿਕ ਮੁਕੰਮਲ = next == std::string_view::npos;
            std::string_view sub = localeID;
            ਜੇ (!ਮੁਕੰਮਲ) { sub.remove_suffix(sub.length() - next); }

            ਜੇ (sub.length() == length && uprv_strnicmp(sub.ਅੰਕੜੇ(), vaposix, length) == 0) {
                ਜੇ (variant != ਖਾਲੀ_ਸੂਚਕ) {
                    ਜੇ (hasVariant) { variant->Append("_", 1); }
                    constexpr ਅੱਖਰ posix[] = "POSIX";
                    variant->Append(posix, ਆਕਾਰ posix - 1);
                }
                ਜੇ (p_ਸਿਰਾ != ਖਾਲੀ_ਸੂਚਕ) { *p_ਸਿਰਾ = localeID.ਅੰਕੜੇ() + length; }
            }

            ਜੇ (ਮੁਕੰਮਲ) { ਰੋਕੋ; }
        }
    }
}

/* Keyword enumeration */

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ UKeywordsContext {
    ਅੱਖਰ* keywords;
    ਅੱਖਰ* current;
} UKeywordsContext;

U_CDECL_BEGIN

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
uloc_kw_closeKeywords(UEnumeration *enumerator) {
    uprv_free(((UKeywordsContext *)enumerator->context)->keywords);
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

ਸਥਿਰ int32_t U_CALLCONV
uloc_kw_ਗਿਣਤੀ_Keywords(UEnumeration *en, UError_ਕੋਡ * /*ਸਥਿਤੀ*/) {
    ਅੱਖਰ *kw = ((UKeywordsContext *)en->context)->keywords;
    int32_t ਨਤੀਜਾ = 0;
    ਜਦੋਂ(*kw) {
        ਨਤੀਜਾ++;
        kw += uprv_strlen(kw)+1;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ * U_CALLCONV
uloc_kw_ਅਗਲਾ_Keyword(UEnumeration* en,
                    int32_t* ਨਤੀਜਾ_ਲੰਮਾਈ,
                    UError_ਕੋਡ* /*ਸਥਿਤੀ*/) {
    ਅਚਲ ਅੱਖਰ* ਨਤੀਜਾ = ((UKeywordsContext *)en->context)->current;
    int32_t len = 0;
    ਜੇ(*ਨਤੀਜਾ) {
        len = (int32_t)uprv_strlen(((UKeywordsContext *)en->context)->current);
        ((UKeywordsContext *)en->context)->current += len+1;
    } ਨਹੀਂਤਾਂ {
        ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (ਨਤੀਜਾ_ਲੰਮਾਈ) {
        *ਨਤੀਜਾ_ਲੰਮਾਈ = len;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
uloc_kw_resetKeywords(UEnumeration* en,
                      UError_ਕੋਡ* /*ਸਥਿਤੀ*/) {
    ((UKeywordsContext *)en->context)->current = ((UKeywordsContext *)en->context)->keywords;
}

U_CDECL_ਸਿਰਾ


ਸਥਿਰ ਅਚਲ UEnumeration gKeywordsEnum = {
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    uloc_kw_closeKeywords,
    uloc_kw_ਗਿਣਤੀ_Keywords,
    uenum_unextDefault,
    uloc_kw_ਅਗਲਾ_Keyword,
    uloc_kw_resetKeywords
};

U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywordList(ਅਚਲ ਅੱਖਰ *keywordList, int32_t keyword_List_ਮਾਪ, UError_ਕੋਡ* ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }

    Local_ਸਮ੍ਰਿਤੀ<UKeywordsContext> myContext;
    Local_ਸਮ੍ਰਿਤੀ<UEnumeration> ਨਤੀਜਾ;

    myContext.adoptInstead(static_cast<UKeywordsContext *>(uprv_malloc(ਆਕਾਰ(UKeywordsContext))));
    ਨਤੀਜਾ.adoptInstead(static_cast<UEnumeration *>(uprv_malloc(ਆਕਾਰ(UEnumeration))));
    ਜੇ (myContext.isNull() || ਨਤੀਜਾ.isNull()) {
        *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    uprv_memcpy(ਨਤੀਜਾ.getAlias(), &gKeywordsEnum, ਆਕਾਰ(UEnumeration));
    myContext->keywords = static_cast<ਅੱਖਰ *>(uprv_malloc(keyword_List_ਮਾਪ+1));
    ਜੇ (myContext->keywords == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    uprv_memcpy(myContext->keywords, keywordList, keyword_List_ਮਾਪ);
    myContext->keywords[keyword_List_ਮਾਪ] = 0;
    myContext->current = myContext->keywords;
    ਨਤੀਜਾ->context = myContext.orphan();
    ਵਾਪਸ ਨਤੀਜਾ.orphan();
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywords(ਅਚਲ ਅੱਖਰ* localeID,
                        UError_ਕੋਡ* ਸਥਿਤੀ)
{
    ਜੇ(ਸਥਿਤੀ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    CharString ਆਰਜ਼ੀ_ਬਫਰ;
    ਅਚਲ ਅੱਖਰ* tmpLocaleID;

    ਜੇ (localeID != ਖਾਲੀ_ਸੂਚਕ && _hasBCP47Extension(localeID)) {
        ਆਰਜ਼ੀ_ਬਫਰ = ulocimp_for_Language_ਟੈਗ(localeID, -1, ਖਾਲੀ_ਸੂਚਕ, *ਸਥਿਤੀ);
        tmpLocaleID = U_SUCCESS(*ਸਥਿਤੀ) && !ਆਰਜ਼ੀ_ਬਫਰ.is_ਖਾਲੀ_ਥਾਵਾਂ() ? ਆਰਜ਼ੀ_ਬਫਰ.ਅੰਕੜੇ() : localeID;
    } ਨਹੀਂਤਾਂ {
        ਜੇ (localeID==ਖਾਲੀ_ਸੂਚਕ) {
            localeID=uloc_getDefault();
        }
        tmpLocaleID=localeID;
    }

    ulocimp_getSubtags(
            tmpLocaleID,
            ਖਾਲੀ_ਸੂਚਕ,
            ਖਾਲੀ_ਸੂਚਕ,
            ਖਾਲੀ_ਸੂਚਕ,
            ਖਾਲੀ_ਸੂਚਕ,
            &tmpLocaleID,
            *ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    /* keywords are located after '@' */
    ਜੇ((tmpLocaleID = locale_get_Keywords_ਸ਼ੁਰੂ(tmpLocaleID)) != ਖਾਲੀ_ਸੂਚਕ) {
        CharString keywords = ulocimp_getKeywords(tmpLocaleID + 1, '@', ਝੂਠ, *ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        ਵਾਪਸ uloc_openKeywordList(keywords.ਅੰਕੜੇ(), keywords.length(), ਸਥਿਤੀ);
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}


/* bit-flags for 'options' parameter of _canonicalize */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _ULOC_STRIP_KEYWORDS 0x2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _ULOC_CANONICALIZE   0x1

ਨਾਮਾਕਾਸ਼ {

ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ OPTION_ਸੈੱਟ(uint32_t options, uint32_t mask) { ਵਾਪਸ (options & mask) != 0; }

constexpr ਅੱਖਰ i_default[] = {'i', '-', 'd', 'e', 'f', 'a', 'u', 'l', 't'};
constexpr int32_t I_DEFAULT_ਲੰਮਾਈ = UPRV_LENGTHOF(i_default);

/**
 * Canonicalize the given localeID, to level 1 or to level 2,
 * depending on the options.  To specify level 1, pass in options=0.
 * To specify level 2, pass in options=_ULOC_CANONICALIZE.
 *
 * This is the code underlying uloc_getName and uloc_canonicalize.
 */
ਖਾਲੀ
_canonicalize(std::string_view localeID,
              ByteSink& sink,
              uint32_t options,
              UError_ਕੋਡ& err) {
    ਜੇ (U_FAILURE(err)) {
        ਵਾਪਸ;
    }

    int32_t j, ਖੇਤਰ_ਗਿਣਤੀ=0;
    CharString ਆਰਜ਼ੀ_ਬਫਰ;  // if localeID has a BCP47 extension, tmpLocaleID points to this
    CharString localeIDWithHyphens;  // if localeID has a BPC47 extension and have _, tmpLocaleID points to this
    std::string_view origLocaleID;
    std::string_view tmpLocaleID;
    size_t keyword_ਸੌਂਪੋ = std::string_view::npos;
    size_t separatorIndicator = std::string_view::npos;

    ਜੇ (_hasBCP47Extension(localeID)) {
        std::string_view localeIDPtr = localeID;

        // convert all underbars to hyphens, unless the "BCP47 extension" comes at the beginning of the string
        ਜੇ (localeID.size() >= 2 && localeID.find('_') != std::string_view::npos && localeID[1] != '-' && localeID[1] != '_') {
            localeIDWithHyphens.append(localeID, err);
            ਜੇ (U_SUCCESS(err)) {
                ਲਈ (ਅੱਖਰ* p = localeIDWithHyphens.ਅੰਕੜੇ(); *p != '\0'; ++p) {
                    ਜੇ (*p == '_') {
                        *p = '-';
                    }
                }
                localeIDPtr = localeIDWithHyphens.ਮੰਜ਼ਿਲ_String_Piece();
            }
        }

        ਆਰਜ਼ੀ_ਬਫਰ = ulocimp_for_Language_ਟੈਗ(localeIDPtr.ਅੰਕੜੇ(), static_cast<int32_t>(localeIDPtr.size()), ਖਾਲੀ_ਸੂਚਕ, err);
        tmpLocaleID = U_SUCCESS(err) && !ਆਰਜ਼ੀ_ਬਫਰ.is_ਖਾਲੀ_ਥਾਵਾਂ() ? static_cast<std::string_view>(ਆਰਜ਼ੀ_ਬਫਰ.ਮੰਜ਼ਿਲ_String_Piece()) : localeIDPtr;
    } ਨਹੀਂਤਾਂ {
        tmpLocaleID=localeID;
    }

    origLocaleID=tmpLocaleID;

    /* get all pieces, one after another, and separate with '_' */
    CharString ਟੈਗ;
    CharString script;
    CharString country;
    CharString variant;
    ਅਚਲ ਅੱਖਰ* ਸਿਰਾ = ਖਾਲੀ_ਸੂਚਕ;
    ulocimp_getSubtags(
            tmpLocaleID,
            &ਟੈਗ,
            &script,
            &country,
            &variant,
            &ਸਿਰਾ,
            err);
    ਜੇ (U_FAILURE(err)) {
        ਵਾਪਸ;
    }
    U_ASSERT(ਸਿਰਾ != ਖਾਲੀ_ਸੂਚਕ);
    ਜੇ (ਸਿਰਾ > tmpLocaleID.ਅੰਕੜੇ()) {
        tmpLocaleID.remove_ਅਗੇਤਰ(ਸਿਰਾ - tmpLocaleID.ਅੰਕੜੇ());
    }

    ਜੇ (ਟੈਗ.length() == I_DEFAULT_ਲੰਮਾਈ && origLocaleID.length() >= I_DEFAULT_ਲੰਮਾਈ &&
            uprv_strncmp(origLocaleID.ਅੰਕੜੇ(), i_default, I_DEFAULT_ਲੰਮਾਈ) == 0) {
        ਟੈਗ.clear();
        ਟੈਗ.append(uloc_getDefault(), err);
    } ਨਹੀਂਤਾਂ {
        ਜੇ (!script.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
            ++ਖੇਤਰ_ਗਿਣਤੀ;
            ਟੈਗ.append('_', err);
            ਟੈਗ.append(script, err);
        }
        ਜੇ (!country.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
            ++ਖੇਤਰ_ਗਿਣਤੀ;
            ਟੈਗ.append('_', err);
            ਟੈਗ.append(country, err);
        }
        ਜੇ (!variant.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
            ++ਖੇਤਰ_ਗਿਣਤੀ;
            ਜੇ (country.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                ਟੈਗ.append('_', err);
            }
            ਟੈਗ.append('_', err);
            ਟੈਗ.append(variant, err);
        }
    }

    /* Copy POSIX-style charset specifier, if any [mr.utf8] */
    ਜੇ (!OPTION_ਸੈੱਟ(options, _ULOC_CANONICALIZE) && !tmpLocaleID.empty() && tmpLocaleID.front() == '.') {
        ਟੈਗ.append('.', err);
        tmpLocaleID.remove_ਅਗੇਤਰ(1);
        size_t length;
        ਜੇ (size_t atPos = tmpLocaleID.find('@'); atPos != std::string_view::npos) {
            length = atPos;
        } ਨਹੀਂਤਾਂ {
            length = tmpLocaleID.length();
        }
        // The longest charset name we found in IANA charset registry
        // https://www.iana.org/assignments/character-sets/ is
        // "Extended_UNIX_Code_Packed_Format_for_Japanese" in length 45.
        // we therefore restrict the length here to be 64 which is a power of 2
        // number that is longer than 45.
        constexpr size_t k_Max_Charset_ਲੰਮਾਈ = 64;
        ਜੇ (length > k_Max_Charset_ਲੰਮਾਈ) {
           err = U_ILLEGAL_ਦਲੀਲ_ERROR; /* malformed keyword name */
           ਵਾਪਸ;
        }
        ਜੇ (length > 0) {
            ਟੈਗ.append(tmpLocaleID.ਅੰਕੜੇ(), static_cast<int32_t>(length), err);
            tmpLocaleID.remove_ਅਗੇਤਰ(length);
        }
    }

    /* Scan ahead to next '@' and determine if it is followed by '=' and/or ';'
       After this, tmpLocaleID either starts at '@' or is empty. */
    ਜੇ (ਅਚਲ ਅੱਖਰ* ਸ਼ੁਰੂ = locale_get_Keywords_ਸ਼ੁਰੂ(tmpLocaleID); ਸ਼ੁਰੂ != ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ (ਸ਼ੁਰੂ > tmpLocaleID.ਅੰਕੜੇ()) {
            tmpLocaleID.remove_ਅਗੇਤਰ(ਸ਼ੁਰੂ - tmpLocaleID.ਅੰਕੜੇ());
        }
        keyword_ਸੌਂਪੋ = tmpLocaleID.find('=');
        separatorIndicator = tmpLocaleID.find(';');
    } ਨਹੀਂਤਾਂ {
        tmpLocaleID = {};
    }

    /* Copy POSIX-style variant, if any [mr@FOO] */
    ਜੇ (!OPTION_ਸੈੱਟ(options, _ULOC_CANONICALIZE) &&
        !tmpLocaleID.empty() && keyword_ਸੌਂਪੋ == std::string_view::npos) {
        ਟੈਗ.append(tmpLocaleID, err);
        tmpLocaleID = {};
    }

    ਜੇ (OPTION_ਸੈੱਟ(options, _ULOC_CANONICALIZE)) {
        /* Handle @FOO variant if @ is present and not followed by = */
        ਜੇ (!tmpLocaleID.empty() && keyword_ਸੌਂਪੋ == std::string_view::npos) {
            /* Add missing '_' if needed */
            ਜੇ (ਖੇਤਰ_ਗਿਣਤੀ < 2 || (ਖੇਤਰ_ਗਿਣਤੀ < 3 && !script.is_ਖਾਲੀ_ਥਾਵਾਂ())) {
                ਕਰੋ {
                    ਟੈਗ.append('_', err);
                    ++ਖੇਤਰ_ਗਿਣਤੀ;
                } ਜਦੋਂ(ਖੇਤਰ_ਗਿਣਤੀ<2);
            }

            CharStringByteSink s(&ਟੈਗ);
            std::string_view sub = tmpLocaleID;
            sub.remove_ਅਗੇਤਰ(1);
            _getVariant(sub, '@', &s, !variant.is_ਖਾਲੀ_ਥਾਵਾਂ(), err);
            ਜੇ (U_FAILURE(err)) { ਵਾਪਸ; }
        }

        /* Look up the ID in the canonicalization map */
        ਲਈ (j=0; j<UPRV_LENGTHOF(CANONICALIZE_MAP); j++) {
            StringPiece id(CANONICALIZE_MAP[j].id);
            ਜੇ (ਟੈਗ == id) {
                ਜੇ (id.empty() && !tmpLocaleID.empty()) {
                    ਰੋਕੋ; /* Don't remap "" if keywords present */
                }
                ਟੈਗ.clear();
                ਟੈਗ.append(CANONICALIZE_MAP[j].canonicalID, err);
                ਰੋਕੋ;
            }
        }
    }

    sink.Append(ਟੈਗ.ਅੰਕੜੇ(), ਟੈਗ.length());

    ਜੇ (!OPTION_ਸੈੱਟ(options, _ULOC_STRIP_KEYWORDS)) {
        ਜੇ (!tmpLocaleID.empty() && keyword_ਸੌਂਪੋ != std::string_view::npos &&
            (separatorIndicator == std::string_view::npos || separatorIndicator > keyword_ਸੌਂਪੋ)) {
            sink.Append("@", 1);
            ++ਖੇਤਰ_ਗਿਣਤੀ;
            tmpLocaleID.remove_ਅਗੇਤਰ(1);
            ulocimp_getKeywords(tmpLocaleID, '@', sink, ਸੱਚ, err);
        }
    }
}

}  // namespace

/* ### ID parsing API **************************************************/

U_CAPI int32_t  U_EXPORT2
uloc_get_ਮਾਪਾ(ਅਚਲ ਅੱਖਰ*    localeID,
               ਅੱਖਰ* ਮਾਪਾ,
               int32_t ਮਾਪਾ_ਸਮਰੱਥਾ,
               UError_ਕੋਡ* err)
{
    ਵਾਪਸ ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Terminated_Chars(
        ਮਾਪਾ, ਮਾਪਾ_ਸਮਰੱਥਾ,
        [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            ulocimp_get_ਮਾਪਾ(localeID, sink, ਸਥਿਤੀ);
        },
        *err);
}

U_EXPORT CharString
ulocimp_get_ਮਾਪਾ(ਅਚਲ ਅੱਖਰ* localeID,
                  UError_ਕੋਡ& err)
{
    ਵਾਪਸ ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Char_String(
        [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            ulocimp_get_ਮਾਪਾ(localeID, sink, ਸਥਿਤੀ);
        },
        err);
}

U_EXPORT ਖਾਲੀ
ulocimp_get_ਮਾਪਾ(ਅਚਲ ਅੱਖਰ* localeID,
                  icu::ByteSink& sink,
                  UError_ਕੋਡ& err)
{
    ਜੇ (U_FAILURE(err)) { ਵਾਪਸ; }

    ਅਚਲ ਅੱਖਰ *ਆਖਰੀ_Underscore;
    int32_t i;

    ਜੇ (localeID == ਖਾਲੀ_ਸੂਚਕ)
        localeID = uloc_getDefault();

    ਆਖਰੀ_Underscore=uprv_strrchr(localeID, '_');
    ਜੇ(ਆਖਰੀ_Underscore!=ਖਾਲੀ_ਸੂਚਕ) {
        i = static_cast<int32_t>(ਆਖਰੀ_Underscore - localeID);
    } ਨਹੀਂਤਾਂ {
        i=0;
    }

    ਜੇ (i > 0) {
        ਜੇ (uprv_strnicmp(localeID, "und_", 4) == 0) {
            localeID += 3;
            i -= 3;
        }
        sink.Append(localeID, i);
    }
}

U_CAPI int32_t U_EXPORT2
uloc_getLanguage(ਅਚਲ ਅੱਖਰ*    localeID,
         ਅੱਖਰ* language,
         int32_t language_ਸਮਰੱਥਾ,
         UError_ਕੋਡ* err)
{
    ਜੇ (localeID == ਖਾਲੀ_ਸੂਚਕ) {
        localeID = uloc_getDefault();
    }

    /* uloc_getLanguage will return a 2 character iso-639 code if one exists. *CWB*/
    ਵਾਪਸ ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Terminated_Chars(
        language, language_ਸਮਰੱਥਾ,
        [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            ulocimp_getSubtags(
                    localeID,
                    &sink,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਸਥਿਤੀ);
        },
        *err);
}

U_CAPI int32_t U_EXPORT2
uloc_getScript(ਅਚਲ ਅੱਖਰ*    localeID,
         ਅੱਖਰ* script,
         int32_t script_ਸਮਰੱਥਾ,
         UError_ਕੋਡ* err)
{
    ਜੇ (localeID == ਖਾਲੀ_ਸੂਚਕ) {
        localeID = uloc_getDefault();
    }

    ਵਾਪਸ ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Terminated_Chars(
        script, script_ਸਮਰੱਥਾ,
        [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            ulocimp_getSubtags(
                    localeID,
                    ਖਾਲੀ_ਸੂਚਕ,
                    &sink,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਸਥਿਤੀ);
        },
        *err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getCountry(ਅਚਲ ਅੱਖਰ* localeID,
            ਅੱਖਰ* country,
            int32_t country_ਸਮਰੱਥਾ,
            UError_ਕੋਡ* err)
{
    ਜੇ (localeID == ਖਾਲੀ_ਸੂਚਕ) {
        localeID = uloc_getDefault();
    }

    ਵਾਪਸ ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Terminated_Chars(
        country, country_ਸਮਰੱਥਾ,
        [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            ulocimp_getSubtags(
                    localeID,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਖਾਲੀ_ਸੂਚਕ,
                    &sink,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਸਥਿਤੀ);
        },
        *err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getVariant(ਅਚਲ ਅੱਖਰ* localeID,
                ਅੱਖਰ* variant,
                int32_t variant_ਸਮਰੱਥਾ,
                UError_ਕੋਡ* err)
{
    ਜੇ (localeID == ਖਾਲੀ_ਸੂਚਕ) {
        localeID = uloc_getDefault();
    }

    ਵਾਪਸ ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Terminated_Chars(
        variant, variant_ਸਮਰੱਥਾ,
        [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            ulocimp_getSubtags(
                    localeID,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਖਾਲੀ_ਸੂਚਕ,
                    &sink,
                    ਖਾਲੀ_ਸੂਚਕ,
                    ਸਥਿਤੀ);
        },
        *err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getName(ਅਚਲ ਅੱਖਰ* localeID,
             ਅੱਖਰ* name,
             int32_t name_ਸਮਰੱਥਾ,
             UError_ਕੋਡ* err)
{
    ਜੇ (localeID == ਖਾਲੀ_ਸੂਚਕ) {
        localeID = uloc_getDefault();
    }
    ਵਾਪਸ ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Terminated_Chars(
        name, name_ਸਮਰੱਥਾ,
        [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            ulocimp_getName(localeID, sink, ਸਥਿਤੀ);
        },
        *err);
}

U_EXPORT CharString
ulocimp_getName(std::string_view localeID,
                UError_ਕੋਡ& err)
{
    ਵਾਪਸ ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Char_String(
        [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            ulocimp_getName(localeID, sink, ਸਥਿਤੀ);
        },
        err);
}

U_EXPORT ਖਾਲੀ
ulocimp_getName(std::string_view localeID,
                ByteSink& sink,
                UError_ਕੋਡ& err)
{
    _canonicalize(localeID, sink, 0, err);
}

U_CAPI int32_t  U_EXPORT2
uloc_get_ਅਧਾਰ_Name(ਅਚਲ ਅੱਖਰ* localeID,
                 ਅੱਖਰ* name,
                 int32_t name_ਸਮਰੱਥਾ,
                 UError_ਕੋਡ* err)
{
    ਜੇ (localeID == ਖਾਲੀ_ਸੂਚਕ) {
        localeID = uloc_getDefault();
    }
    ਵਾਪਸ ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Terminated_Chars(
        name, name_ਸਮਰੱਥਾ,
        [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            ulocimp_get_ਅਧਾਰ_Name(localeID, sink, ਸਥਿਤੀ);
        },
        *err);
}

U_EXPORT CharString
ulocimp_get_ਅਧਾਰ_Name(std::string_view localeID,
                    UError_ਕੋਡ& err)
{
    ਵਾਪਸ ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Char_String(
        [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            ulocimp_get_ਅਧਾਰ_Name(localeID, sink, ਸਥਿਤੀ);
        },
        err);
}

U_EXPORT ਖਾਲੀ
ulocimp_get_ਅਧਾਰ_Name(std::string_view localeID,
                    ByteSink& sink,
                    UError_ਕੋਡ& err)
{
    _canonicalize(localeID, sink, _ULOC_STRIP_KEYWORDS, err);
}

U_CAPI int32_t  U_EXPORT2
uloc_canonicalize(ਅਚਲ ਅੱਖਰ* localeID,
                  ਅੱਖਰ* name,
                  int32_t name_ਸਮਰੱਥਾ,
                  UError_ਕੋਡ* err)
{
    ਜੇ (localeID == ਖਾਲੀ_ਸੂਚਕ) {
        localeID = uloc_getDefault();
    }
    ਵਾਪਸ ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Terminated_Chars(
        name, name_ਸਮਰੱਥਾ,
        [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            ulocimp_canonicalize(localeID, sink, ਸਥਿਤੀ);
        },
        *err);
}

U_EXPORT CharString
ulocimp_canonicalize(std::string_view localeID,
                     UError_ਕੋਡ& err)
{
    ਵਾਪਸ ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Char_String(
        [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            ulocimp_canonicalize(localeID, sink, ਸਥਿਤੀ);
        },
        err);
}

U_EXPORT ਖਾਲੀ
ulocimp_canonicalize(std::string_view localeID,
                     ByteSink& sink,
                     UError_ਕੋਡ& err)
{
    _canonicalize(localeID, sink, _ULOC_CANONICALIZE, err);
}

U_CAPI ਅਚਲ ਅੱਖਰ*  U_EXPORT2
uloc_getISO3Language(ਅਚਲ ਅੱਖਰ* localeID)
{
    UError_ਕੋਡ err = U_ZERO_ERROR;

    ਜੇ (localeID == ਖਾਲੀ_ਸੂਚਕ)
    {
        localeID = uloc_getDefault();
    }
    CharString lang = ulocimp_getLanguage(localeID, err);
    ਜੇ (U_FAILURE(err))
        ਵਾਪਸ "";
    std::optional<int16_t> ਵਿੱਥ = ਲੱਭੋ_ਸੂਚਕਾਂਕ_2(LANGUAGES, lang.ਅੰਕੜੇ());
    ਵਾਪਸ ਵਿੱਥ.has_value() ? LANGUAGES_3[*ਵਿੱਥ] : "";
}

U_CAPI ਅਚਲ ਅੱਖਰ*  U_EXPORT2
uloc_getISO3Country(ਅਚਲ ਅੱਖਰ* localeID)
{
    UError_ਕੋਡ err = U_ZERO_ERROR;

    ਜੇ (localeID == ਖਾਲੀ_ਸੂਚਕ)
    {
        localeID = uloc_getDefault();
    }
    CharString cntry = ulocimp_getRegion(localeID, err);
    ਜੇ (U_FAILURE(err))
        ਵਾਪਸ "";
    std::optional<int16_t> ਵਿੱਥ = ਲੱਭੋ_ਸੂਚਕਾਂਕ_2(COUNTRIES, cntry.ਅੰਕੜੇ());
    ਵਾਪਸ ਵਿੱਥ.has_value() ? COUNTRIES_3[*ਵਿੱਥ] : "";
}

U_CAPI uint32_t  U_EXPORT2
uloc_getLCID(ਅਚਲ ਅੱਖਰ* localeID)
{
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    uint32_t   lcid = 0;

    /* Check for incomplete id. */
    ਜੇ (!localeID || uprv_strlen(localeID) < 2) {
        ਵਾਪਸ 0;
    }

    // First, attempt Windows platform lookup if available, but fall
    // through to catch any special cases (ICU vs Windows name differences).
    lcid = uprv_convert_ਮੰਜ਼ਿਲ_LCIDPlatform(localeID, &ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ (lcid > 0) {
        // Windows found an LCID, return that
        ਵਾਪਸ lcid;
    }

    CharString langID = ulocimp_getLanguage(localeID, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ 0;
    }

    ਜੇ (uprv_strchr(localeID, '@')) {
        // uprv_convertToLCID does not support keywords other than collation.
        // Remove all keywords except collation.
        CharString collVal = ulocimp_get_Keyword_ਮੁੱਲ(localeID, "collation", ਸਥਿਤੀ);
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ) && !collVal.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
            CharString tmpLocaleID = ulocimp_get_ਅਧਾਰ_Name(localeID, ਸਥਿਤੀ);
            ulocimp_ਸੈੱਟ_Keyword_ਮੁੱਲ("collation", collVal.ਮੰਜ਼ਿਲ_String_Piece(), tmpLocaleID, ਸਥਿਤੀ);
            ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
                ਵਾਪਸ uprv_convert_ਮੰਜ਼ਿਲ_LCID(langID.ਅੰਕੜੇ(), tmpLocaleID.ਅੰਕੜੇ(), &ਸਥਿਤੀ);
            }
        }

        // fall through - all keywords are simply ignored
        ਸਥਿਤੀ = U_ZERO_ERROR;
    }

    ਵਾਪਸ uprv_convert_ਮੰਜ਼ਿਲ_LCID(langID.ਅੰਕੜੇ(), localeID, &ਸਥਿਤੀ);
}

U_CAPI int32_t U_EXPORT2
uloc_getLocaleForLCID(uint32_t hostid, ਅੱਖਰ *locale, int32_t locale_ਸਮਰੱਥਾ,
                UError_ਕੋਡ *ਸਥਿਤੀ)
{
    ਵਾਪਸ uprv_convert_ਮੰਜ਼ਿਲ_Posix(hostid, locale, locale_ਸਮਰੱਥਾ, ਸਥਿਤੀ);
}

/* ### Default locale **************************************************/

U_CAPI ਅਚਲ ਅੱਖਰ*  U_EXPORT2
uloc_getDefault()
{
    ਵਾਪਸ locale_get_default();
}

U_CAPI ਖਾਲੀ  U_EXPORT2
uloc_ਸੈੱਟ_Default(ਅਚਲ ਅੱਖਰ*   newDefaultLocale,
             UError_ਕੋਡ* err)
{
    ਜੇ (U_FAILURE(*err))
        ਵਾਪਸ;
    /* the error code isn't currently used for anything by this function*/

    /* propagate change to C++ */
    locale_ਸੈੱਟ_default(newDefaultLocale);
}

/**
 * Returns a list of all 2-letter language codes defined in ISO 639.  This is a pointer
 * to an array of pointers to arrays of char.  All of these pointers are owned
 * by ICU-- do not delete them, and do not write through them.  The array is
 * terminated with a null pointer.
 */
U_CAPI ਅਚਲ ਅੱਖਰ* ਅਚਲ*  U_EXPORT2
uloc_getISOLanguages()
{
    ਵਾਪਸ LANGUAGES;
}

/**
 * Returns a list of all 2-letter country codes defined in ISO 639.  This is a
 * pointer to an array of pointers to arrays of char.  All of these pointers are
 * owned by ICU-- do not delete them, and do not write through them.  The array is
 * terminated with a null pointer.
 */
U_CAPI ਅਚਲ ਅੱਖਰ* ਅਚਲ*  U_EXPORT2
uloc_getISOCountries()
{
    ਵਾਪਸ COUNTRIES;
}

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
uloc_ਮੰਜ਼ਿਲ_Unicode_Locale_ਕੁੰਜੀ(ਅਚਲ ਅੱਖਰ* keyword U_LIFETIME_BOUND)
{
    ਜੇ (keyword == ਖਾਲੀ_ਸੂਚਕ || *keyword == '\0') { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    std::optional<std::string_view> ਨਤੀਜਾ = ulocimp_ਮੰਜ਼ਿਲ_Bcp_ਕੁੰਜੀ_With_Fallback(keyword);
    ਵਾਪਸ ਨਤੀਜਾ.has_value() ? ਨਤੀਜਾ->ਅੰਕੜੇ() : ਖਾਲੀ_ਸੂਚਕ;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_ਮੰਜ਼ਿਲ_Bcp_ਕੁੰਜੀ_With_Fallback(std::string_view keyword U_LIFETIME_BOUND)
{
    std::optional<std::string_view> bcp_ਕੁੰਜੀ = ulocimp_ਮੰਜ਼ਿਲ_Bcp_ਕੁੰਜੀ(keyword);
    ਜੇ (!bcp_ਕੁੰਜੀ.has_value() &&
        ultag_is_Unicode_Locale_ਕੁੰਜੀ(keyword.ਅੰਕੜੇ(), static_cast<int32_t>(keyword.size()))) {
        // unknown keyword, but syntax is fine..
        ਵਾਪਸ keyword;
    }
    ਵਾਪਸ bcp_ਕੁੰਜੀ;
}

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
uloc_ਮੰਜ਼ਿਲ_Unicode_Locale_Type(ਅਚਲ ਅੱਖਰ* keyword, ਅਚਲ ਅੱਖਰ* value U_LIFETIME_BOUND)
{
    ਜੇ (keyword == ਖਾਲੀ_ਸੂਚਕ || *keyword == '\0' ||
        value == ਖਾਲੀ_ਸੂਚਕ || *value == '\0') { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    std::optional<std::string_view> ਨਤੀਜਾ = ulocimp_ਮੰਜ਼ਿਲ_Bcp_Type_With_Fallback(keyword, value);
    ਵਾਪਸ ਨਤੀਜਾ.has_value() ? ਨਤੀਜਾ->ਅੰਕੜੇ() : ਖਾਲੀ_ਸੂਚਕ;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_ਮੰਜ਼ਿਲ_Bcp_Type_With_Fallback(std::string_view keyword, std::string_view value U_LIFETIME_BOUND)
{
    std::optional<std::string_view> bcpType = ulocimp_ਮੰਜ਼ਿਲ_Bcp_Type(keyword, value);
    ਜੇ (!bcpType.has_value() &&
        ultag_isUnicodeLocaleType(value.ਅੰਕੜੇ(), static_cast<int32_t>(value.size()))) {
        // unknown keyword, but syntax is fine..
        ਵਾਪਸ value;
    }
    ਵਾਪਸ bcpType;
}

ਨਾਮਾਕਾਸ਼ {

ਤਾਰਕਿਕ
is_Well_Formed_Legacy_ਕੁੰਜੀ(std::string_view ਕੁੰਜੀ)
{
    ਵਾਪਸ std::all_of(ਕੁੰਜੀ.begin(), ਕੁੰਜੀ.ਸਿਰਾ(), UPRV_ISALPHANUM);
}

ਤਾਰਕਿਕ
isWellFormedLegacyType(std::string_view legacyType)
{
    int32_t alpha_ਅੰਕ_Len = 0;
    ਲਈ (ਅੱਖਰ c : legacyType) {
        ਜੇ (c == '_' || c == '/' || c == '-') {
            ਜੇ (alpha_ਅੰਕ_Len == 0) {
                ਵਾਪਸ ਝੂਠ;
            }
            alpha_ਅੰਕ_Len = 0;
        } ਨਹੀਂਤਾਂ ਜੇ (UPRV_ISALPHANUM(c)) {
            alpha_ਅੰਕ_Len++;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ ਝੂਠ;
        }
    }
    ਵਾਪਸ alpha_ਅੰਕ_Len != 0;
}

}  // namespace

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
uloc_ਮੰਜ਼ਿਲ_Legacy_ਕੁੰਜੀ(ਅਚਲ ਅੱਖਰ* keyword U_LIFETIME_BOUND)
{
    ਜੇ (keyword == ਖਾਲੀ_ਸੂਚਕ || *keyword == '\0') { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    std::optional<std::string_view> ਨਤੀਜਾ = ulocimp_ਮੰਜ਼ਿਲ_Legacy_ਕੁੰਜੀ_With_Fallback(keyword);
    ਵਾਪਸ ਨਤੀਜਾ.has_value() ? ਨਤੀਜਾ->ਅੰਕੜੇ() : ਖਾਲੀ_ਸੂਚਕ;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_ਮੰਜ਼ਿਲ_Legacy_ਕੁੰਜੀ_With_Fallback(std::string_view keyword U_LIFETIME_BOUND)
{
    std::optional<std::string_view> legacy_ਕੁੰਜੀ_2 = ulocimp_ਮੰਜ਼ਿਲ_Legacy_ਕੁੰਜੀ(keyword);
    ਜੇ (!legacy_ਕੁੰਜੀ_2.has_value() && is_Well_Formed_Legacy_ਕੁੰਜੀ(keyword)) {
        // Checks if the specified locale key is well-formed with the legacy locale syntax.
        //
        // Note:
        //  LDML/CLDR provides some definition of keyword syntax in
        //  * http://www.unicode.org/reports/tr35/#Unicode_locale_identifier and
        //  * http://www.unicode.org/reports/tr35/#Old_Locale_Extension_Syntax
        //  Keys can only consist of [0-9a-zA-Z].
        ਵਾਪਸ keyword;
    }
    ਵਾਪਸ legacy_ਕੁੰਜੀ_2;
}

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
uloc_ਮੰਜ਼ਿਲ_Legacy_Type(ਅਚਲ ਅੱਖਰ* keyword, ਅਚਲ ਅੱਖਰ* value U_LIFETIME_BOUND)
{
    ਜੇ (keyword == ਖਾਲੀ_ਸੂਚਕ || *keyword == '\0' ||
        value == ਖਾਲੀ_ਸੂਚਕ || *value == '\0') { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    std::optional<std::string_view> ਨਤੀਜਾ = ulocimp_ਮੰਜ਼ਿਲ_Legacy_Type_With_Fallback(keyword, value);
    ਵਾਪਸ ਨਤੀਜਾ.has_value() ? ਨਤੀਜਾ->ਅੰਕੜੇ() : ਖਾਲੀ_ਸੂਚਕ;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_ਮੰਜ਼ਿਲ_Legacy_Type_With_Fallback(std::string_view keyword, std::string_view value U_LIFETIME_BOUND)
{
    std::optional<std::string_view> legacyType = ulocimp_ਮੰਜ਼ਿਲ_Legacy_Type(keyword, value);
    ਜੇ (!legacyType.has_value() && isWellFormedLegacyType(value)) {
        // Checks if the specified locale type is well-formed with the legacy locale syntax.
        //
        // Note:
        //  LDML/CLDR provides some definition of keyword syntax in
        //  * http://www.unicode.org/reports/tr35/#Unicode_locale_identifier and
        //  * http://www.unicode.org/reports/tr35/#Old_Locale_Extension_Syntax
        //  Values (types) can only consist of [0-9a-zA-Z], plus for legacy values
        //  we allow [/_-+] in the middle (e.g. "Etc/GMT+1", "Asia/Tel_Aviv")
        ਵਾਪਸ value;
    }
    ਵਾਪਸ legacyType;
}

/*eof*/
