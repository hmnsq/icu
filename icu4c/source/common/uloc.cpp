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

#अन्तर्भाव <algorithm>
#अन्तर्भाव <optional>
#अन्तर्भाव <string_view>

#अन्तर्भाव "unicode/bytestream.h"
#अन्तर्भाव "unicode/errorcode.h"
#अन्तर्भाव "unicode/stringpiece.h"
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/uloc.h"

#अन्तर्भाव "bytesinkutil.h"
#अन्तर्भाव "putilimp.h"
#अन्तर्भाव "ustr_imp.h"
#अन्तर्भाव "ulocimp.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "locmap.h"
#अन्तर्भाव "uarrsort.h"
#अन्तर्भाव "uenumimp.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "charstr.h"

U_NAMESPACE_USE

/* ### Declarations **************************************************/

/* Locale stuff from locid.cpp */
U_CFUNC शून्य locale_समुच्चयः_default(ध्रुव वर्ण *id);
U_CFUNC ध्रुव वर्ण *locale_get_default();

नामाकाश {

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
constexpr ध्रुव वर्ण* LANGUAGES[] = {
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
शून्यसूचकः,
    "in",  "iw",  "ji",  "jw",  "mo",  "sh",  "swc", "tl",  /* obsolete language codes */
शून्यसूचकः
};

constexpr ध्रुव वर्ण* DEPRECATED_LANGUAGES[]={
    "in", "iw", "ji", "jw", "mo", शून्यसूचकः, शून्यसूचकः
};
constexpr ध्रुव वर्ण* REPLACEMENT_LANGUAGES[]={
    "id", "he", "yi", "jv", "ro", शून्यसूचकः, शून्यसूचकः
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
constexpr ध्रुव वर्ण* LANGUAGES_3[] = {
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
शून्यसूचकः,
/*  "in",  "iw",  "ji",  "jw",  "mo",  "sh",  "swc", "tl",  */
    "ind", "heb", "yid", "jaw", "mol", "srp", "swc", "tgl",
शून्यसूचकः
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
constexpr ध्रुव वर्ण* COUNTRIES[] = {
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
शून्यसूचकः,
    "AN",  "BU", "CS", "FX", "RO", "SU", "TP", "YD", "YU", "ZR",   /* obsolete country codes */
शून्यसूचकः
};

constexpr ध्रुव वर्ण* DEPRECATED_COUNTRIES[] = {
    "AN", "BU", "CS", "DD", "DY", "FX", "HV", "NH", "RH", "SU", "TP", "UK", "VD", "YD", "YU", "ZR", शून्यसूचकः, शून्यसूचकः /* deprecated country list */
};
constexpr ध्रुव वर्ण* REPLACEMENT_COUNTRIES[] = {
/*  "AN", "BU", "CS", "DD", "DY", "FX", "HV", "NH", "RH", "SU", "TP", "UK", "VD", "YD", "YU", "ZR" */
    "CW", "MM", "RS", "DE", "BJ", "FR", "BF", "VU", "ZW", "RU", "TL", "GB", "VN", "YE", "RS", "CD", शून्यसूचकः, शून्यसूचकः  /* replacement country codes */
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
constexpr ध्रुव वर्ण* COUNTRIES_3[] = {
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
शून्यसूचकः,
/*  "AN",  "BU",  "CS",  "FX",  "RO", "SU",  "TP",  "YD",  "YU",  "ZR" */
    "ANT", "BUR", "SCG", "FXX", "ROM", "SUN", "TMP", "YMD", "YUG", "ZAR",
शून्यसूचकः
};

प्रकारनिर्देश संरचना CanonicalizationMap {
    ध्रुव वर्ण *id;          /* input ID */
    ध्रुव वर्ण *canonicalID; /* canonicalized output ID */
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
int32_t get_ह्रस्वतमम्_Subtag_दीर्घता(std::string_view localeID) {
    int32_t localeIDLength = static_cast<int32_t>(localeID.length());
    int32_t length = localeIDLength;
    int32_t tmp_दीर्घता = 0;
    int32_t i;
    तार्किक reset = सत्य;

    प्रति (i = 0; i < localeIDLength; i++) {
        यदि (localeID[i] != '_' && localeID[i] != '-') {
            यदि (reset) {
                tmp_दीर्घता = 0;
                reset = असत्य;
            }
            tmp_दीर्घता++;
        } अन्यथा {
            यदि (tmp_दीर्घता != 0 && tmp_दीर्घता < length) {
                length = tmp_दीर्घता;
            }
            reset = सत्य;
        }
    }

    प्रत्यागम length;
}
/* Test if the locale id has BCP47 u extension and does not have '@' */
अन्तःस्थ तार्किक _hasBCP47Extension(std::string_view id) {
    प्रत्यागम id.find('@') == std::string_view::npos && get_ह्रस्वतमम्_Subtag_दीर्घता(id) == 1;
}

/* ### Keywords **************************************************/
अन्तःस्थ तार्किक UPRV_ISDIGIT(वर्ण c) { प्रत्यागम c >= '0' && c <= '9'; }
अन्तःस्थ तार्किक UPRV_ISALPHANUM(वर्ण c) { प्रत्यागम uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }
/* Punctuation/symbols allowed in legacy key values */
अन्तःस्थ तार्किक UPRV_OK_मानम्_PUNCTUATION(वर्ण c) { प्रत्यागम c == '_' || c == '-' || c == '+' || c == '/'; }

}  // namespace

#परिभाषय ULOC_KEYWORD_बफर_LEN 25
#परिभाषय ULOC_MAX_NO_KEYWORDS 25

U_CAPI ध्रुव वर्ण* U_EXPORT2
locale_getKeywordsStart(std::string_view localeID U_LIFETIME_BOUND) {
    यदि (size_t pos = localeID.find('@'); pos != std::string_view::npos) {
        प्रत्यागम localeID.दत्त() + pos;
    }
#यदि (U_CHARSET_FAMILY == U_EBCDIC_FAMILY)
    अन्यथा {
        /* We do this because the @ sign is variant, and the @ sign used on one
        EBCDIC machine won't be compiled the same way on other EBCDIC based
        machines. */
        स्थिर ध्रुव uint8_t ebcdicSigns[] = { 0x7C, 0x44, 0x66, 0x80, 0xAC, 0xAE, 0xAF, 0xB5, 0xEC, 0xEF, 0x00 };
        ध्रुव uint8_t *char_गन्तव्यम्_Find = ebcdicSigns;
        यावत्(*char_गन्तव्यम्_Find) {
            यदि (size_t pos = localeID.find(*char_गन्तव्यम्_Find); pos != std::string_view::npos) {
                प्रत्यागम localeID.दत्त() + pos;
            }
            char_गन्तव्यम्_Find++;
        }
    }
#यदि_अन्तः
    प्रत्यागम शून्यसूचकः;
}

नामाकाश {

/**
 * @param keywordName incoming name to be canonicalized
 * @param status return status (keyword too long)
 * @return the keyword name
 */
CharString locale_canonKeywordName(std::string_view keywordName, UError_संहिता& स्थितिः)
{
  यदि (U_FAILURE(स्थितिः)) { प्रत्यागम {}; }
  CharString परिणाम;

  प्रति (वर्ण c : keywordName) {
    यदि (!UPRV_ISALPHANUM(c)) {
      स्थितिः = U_ILLEGAL_तर्कः_ERROR; /* malformed keyword name */
      प्रत्यागम {};
    }
    परिणाम.append(uprv_tolower(c), स्थितिः);
  }
  यदि (परिणाम.is_रिक्तानि()) {
    स्थितिः = U_ILLEGAL_तर्कः_ERROR; /* empty keyword name */
    प्रत्यागम {};
  }

  प्रत्यागम परिणाम;
}

प्रकारनिर्देश संरचना {
    वर्ण keyword[ULOC_KEYWORD_बफर_LEN];
    int32_t keywordLen;
    ध्रुव वर्ण *मानम्_Start;
    int32_t मानम्_Len;
} KeywordStruct;

int32_t U_CALLCONV
compareKeywordStructs(ध्रुव शून्य * /*context*/, ध्रुव शून्य *वामतः, ध्रुव शून्य *दक्षिणतः) {
    ध्रुव वर्ण* वामः_String = static_cast<ध्रुव KeywordStruct*>(वामतः)->keyword;
    ध्रुव वर्ण* दक्षिणः_String = static_cast<ध्रुव KeywordStruct*>(दक्षिणतः)->keyword;
    प्रत्यागम uprv_strcmp(वामः_String, दक्षिणः_String);
}

}  // namespace

U_EXPORT CharString
ulocimp_getKeywords(std::string_view localeID,
                    वर्ण पूर्व,
                    तार्किक मानानि_Too,
                    UError_संहिता& स्थितिः)
{
    प्रत्यागम ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Char_String(
        [&](ByteSink& sink, UError_संहिता& स्थितिः) {
            ulocimp_getKeywords(localeID,
                                पूर्व,
                                sink,
                                मानानि_Too,
                                स्थितिः);
        },
        स्थितिः);
}

U_EXPORT शून्य
ulocimp_getKeywords(std::string_view localeID,
                    वर्ण पूर्व,
                    ByteSink& sink,
                    तार्किक मानानि_Too,
                    UError_संहिता& स्थितिः)
{
    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }

    KeywordStruct keywordList[ULOC_MAX_NO_KEYWORDS];

    int32_t maxKeywords = ULOC_MAX_NO_KEYWORDS;
    int32_t अङ्कः_Keywords = 0;
    size_t equalSign = std::string_view::npos;
    size_t semicolon = std::string_view::npos;
    int32_t i = 0, j, n;

    यदि(पूर्व == '@') { /* start of keyword definition */
        /* we will grab pairs, trim spaces, lowercase keywords, sort and return */
        कुरु {
            तार्किक duplicate = असत्य;
            /* skip leading spaces */
            यावत् (!localeID.empty() && localeID.front() == ' ') {
                localeID.remove_उपसर्गः(1);
            }
            यदि (localeID.empty()) { /* handle trailing "; " */
                भङ्ग;
            }
            यदि(अङ्कः_Keywords == maxKeywords) {
                स्थितिः = U_आन्तरिकम्_क्रमादेशः_ERROR;
                प्रत्यागम;
            }
            equalSign = localeID.find('=');
            semicolon = localeID.find(';');
            /* lack of '=' [foo@currency] is illegal */
            /* ';' before '=' [foo@currency;collation=pinyin] is illegal */
            यदि (equalSign == std::string_view::npos ||
                (semicolon != std::string_view::npos && semicolon < equalSign)) {
                स्थितिः = U_INVALID_FORMAT_ERROR;
                प्रत्यागम;
            }
            /* zero-length keyword is an error. */
            यदि (equalSign == 0) {
                स्थितिः = U_INVALID_FORMAT_ERROR;
                प्रत्यागम;
            }
            /* need to normalize both keyword and keyword name */
            यदि (equalSign >= ULOC_KEYWORD_बफर_LEN) {
                /* keyword name too long for internal buffer */
                स्थितिः = U_आन्तरिकम्_क्रमादेशः_ERROR;
                प्रत्यागम;
            }
            प्रति (i = 0, n = 0; static_cast<size_t>(i) < equalSign; ++i) {
                यदि (localeID[i] != ' ') {
                    keywordList[अङ्कः_Keywords].keyword[n++] = uprv_tolower(localeID[i]);
                }
            }

            keywordList[अङ्कः_Keywords].keyword[n] = 0;
            keywordList[अङ्कः_Keywords].keywordLen = n;
            /* now grab the value part. First we skip the '=' */
            equalSign++;
            /* then we leading spaces */
            यावत् (equalSign < localeID.length() && localeID[equalSign] == ' ') {
                equalSign++;
            }

            /* Premature end or zero-length value */
            यदि (equalSign == localeID.length() || equalSign == semicolon) {
                स्थितिः = U_INVALID_FORMAT_ERROR;
                प्रत्यागम;
            }

            keywordList[अङ्कः_Keywords].मानम्_Start = localeID.दत्त() + equalSign;

            std::string_view value = localeID;
            यदि (semicolon != std::string_view::npos) {
                value.remove_suffix(value.length() - semicolon);
                localeID.remove_उपसर्गः(semicolon + 1);
            } अन्यथा {
                localeID = {};
            }
            value.remove_उपसर्गः(equalSign);
            यदि (size_t चरमः = value.find_last_not_of(' '); चरमः != std::string_view::npos) {
                value.remove_suffix(value.length() - चरमः - 1);
            }
            keywordList[अङ्कः_Keywords].मानम्_Len = static_cast<int32_t>(value.length());

            /* If this is a duplicate keyword, then ignore it */
            प्रति (j=0; j<अङ्कः_Keywords; ++j) {
                यदि (uprv_strcmp(keywordList[j].keyword, keywordList[अङ्कः_Keywords].keyword) == 0) {
                    duplicate = सत्य;
                    भङ्ग;
                }
            }
            यदि (!duplicate) {
                ++अङ्कः_Keywords;
            }
        } यावत् (!localeID.empty());

        /* now we have a list of keywords */
        /* we need to sort it */
        uprv_क्रमय_Array(keywordList, अङ्कः_Keywords, परिमाण(KeywordStruct), compareKeywordStructs, शून्यसूचकः, असत्य, &स्थितिः);

        /* Now construct the keyword part */
        प्रति(i = 0; i < अङ्कः_Keywords; i++) {
            sink.Append(keywordList[i].keyword, keywordList[i].keywordLen);
            यदि(मानानि_Too) {
                sink.Append("=", 1);
                sink.Append(keywordList[i].मानम्_Start, keywordList[i].मानम्_Len);
                यदि(i < अङ्कः_Keywords - 1) {
                    sink.Append(";", 1);
                }
            } अन्यथा {
                sink.Append("\0", 1);
            }
        }
    }
}

U_CAPI int32_t U_EXPORT2
uloc_get_Keyword_मानम्(ध्रुव वर्ण* localeID,
                     ध्रुव वर्ण* keywordName,
                     वर्ण* बफर, int32_t बफर_क्षमता,
                     UError_संहिता* स्थितिः)
{
    यदि (U_FAILURE(*स्थितिः)) { प्रत्यागम 0; }
    यदि (keywordName == शून्यसूचकः || *keywordName == '\0') {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }
    प्रत्यागम ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Terminated_Chars(
        बफर, बफर_क्षमता,
        [&](ByteSink& sink, UError_संहिता& स्थितिः) {
            ulocimp_get_Keyword_मानम्(localeID, keywordName, sink, स्थितिः);
        },
        *स्थितिः);
}

U_EXPORT CharString
ulocimp_get_Keyword_मानम्(ध्रुव वर्ण* localeID,
                        std::string_view keywordName,
                        UError_संहिता& स्थितिः)
{
    प्रत्यागम ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Char_String(
        [&](ByteSink& sink, UError_संहिता& स्थितिः) {
            ulocimp_get_Keyword_मानम्(localeID, keywordName, sink, स्थितिः);
        },
        स्थितिः);
}

U_EXPORT शून्य
ulocimp_get_Keyword_मानम्(ध्रुव वर्ण* localeID,
                        std::string_view keywordName,
                        icu::ByteSink& sink,
                        UError_संहिता& स्थितिः)
{
    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }

    यदि (localeID == शून्यसूचकः || keywordName.empty()) {
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    ध्रुव वर्ण* start_अन्वेषणम्_अत्र = शून्यसूचकः;
    ध्रुव वर्ण* अग्र_Separator = शून्यसूचकः;

    CharString अस्थायी_बफर;
    ध्रुव वर्ण* tmpLocaleID;

    CharString canonKeywordName = locale_canonKeywordName(keywordName, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
      प्रत्यागम;
    }

    यदि (localeID != शून्यसूचकः && _hasBCP47Extension(localeID)) {
        अस्थायी_बफर = ulocimp_for_Language_चिह्नकम्(localeID, -1, शून्यसूचकः, स्थितिः);
        tmpLocaleID = U_SUCCESS(स्थितिः) && !अस्थायी_बफर.is_रिक्तानि() ? अस्थायी_बफर.दत्त() : localeID;
    } अन्यथा {
        tmpLocaleID=localeID;
    }

    start_अन्वेषणम्_अत्र = locale_getKeywordsStart(tmpLocaleID);
    यदि(start_अन्वेषणम्_अत्र == शून्यसूचकः) {
        /* no keywords, return at once */
        प्रत्यागम;
    }

    /* find the first keyword */
    यावत्(start_अन्वेषणम्_अत्र) {
        ध्रुव वर्ण* कुञ्चिका_मानम्_पुच्छ;

        start_अन्वेषणम्_अत्र++; /* skip @ or ; */
        अग्र_Separator = uprv_strchr(start_अन्वेषणम्_अत्र, '=');
        यदि(!अग्र_Separator) {
            स्थितिः = U_ILLEGAL_तर्कः_ERROR; /* key must have =value */
            प्रत्यागम;
        }
        /* strip leading & trailing spaces (TC decided to tolerate these) */
        यावत्(*start_अन्वेषणम्_अत्र == ' ') {
            start_अन्वेषणम्_अत्र++;
        }
        कुञ्चिका_मानम्_पुच्छ = अग्र_Separator;
        यावत् (कुञ्चिका_मानम्_पुच्छ > start_अन्वेषणम्_अत्र && *(कुञ्चिका_मानम्_पुच्छ-1) == ' ') {
            कुञ्चिका_मानम्_पुच्छ--;
        }
        /* now keyValueTail points to first char after the keyName */
        /* copy & normalize keyName from locale */
        यदि (start_अन्वेषणम्_अत्र == कुञ्चिका_मानम्_पुच्छ) {
            स्थितिः = U_ILLEGAL_तर्कः_ERROR; /* empty keyword name in passed-in locale */
            प्रत्यागम;
        }
        CharString localeKeywordName;
        यावत् (start_अन्वेषणम्_अत्र < कुञ्चिका_मानम्_पुच्छ) {
          यदि (!UPRV_ISALPHANUM(*start_अन्वेषणम्_अत्र)) {
            स्थितिः = U_ILLEGAL_तर्कः_ERROR; /* malformed keyword name */
            प्रत्यागम;
          }
          localeKeywordName.append(uprv_tolower(*start_अन्वेषणम्_अत्र++), स्थितिः);
        }
        यदि (U_FAILURE(स्थितिः)) {
            प्रत्यागम;
        }

        start_अन्वेषणम्_अत्र = uprv_strchr(अग्र_Separator, ';');

        यदि (canonKeywordName == localeKeywordName) {
             /* current entry matches the keyword. */
           अग्र_Separator++; /* skip '=' */
            /* First strip leading & trailing spaces (TC decided to tolerate these) */
            यावत्(*अग्र_Separator == ' ') {
              अग्र_Separator++;
            }
            कुञ्चिका_मानम्_पुच्छ = (start_अन्वेषणम्_अत्र)? start_अन्वेषणम्_अत्र: अग्र_Separator + uprv_strlen(अग्र_Separator);
            यावत्(कुञ्चिका_मानम्_पुच्छ > अग्र_Separator && *(कुञ्चिका_मानम्_पुच्छ-1) == ' ') {
              कुञ्चिका_मानम्_पुच्छ--;
            }
            /* Now copy the value, but check well-formedness */
            यदि (अग्र_Separator == कुञ्चिका_मानम्_पुच्छ) {
              स्थितिः = U_ILLEGAL_तर्कः_ERROR; /* empty key value name in passed-in locale */
              प्रत्यागम;
            }
            यावत् (अग्र_Separator < कुञ्चिका_मानम्_पुच्छ) {
              यदि (!UPRV_ISALPHANUM(*अग्र_Separator) && !UPRV_OK_मानम्_PUNCTUATION(*अग्र_Separator)) {
                स्थितिः = U_ILLEGAL_तर्कः_ERROR; /* malformed key value */
                प्रत्यागम;
              }
              /* Should we lowercase value to return here? Tests expect as-is. */
              sink.Append(अग्र_Separator++, 1);
            }
            प्रत्यागम;
        }
    }
}

U_CAPI int32_t U_EXPORT2
uloc_समुच्चयः_Keyword_मानम्(ध्रुव वर्ण* keywordName,
                     ध्रुव वर्ण* keyword_मानम्,
                     वर्ण* बफर, int32_t बफर_क्षमता,
                     UError_संहिता* स्थितिः)
{
    यदि (U_FAILURE(*स्थितिः)) { प्रत्यागम 0; }

    यदि (keywordName == शून्यसूचकः || *keywordName == 0) {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    यदि (बफर_क्षमता <= 1) {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    int32_t bufLen = (int32_t)uprv_strlen(बफर);
    यदि(बफर_क्षमता<bufLen) {
        /* The capacity is less than the length?! Is this NUL terminated? */
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    वर्ण* keywords = const_cast<वर्ण*>(
        locale_getKeywordsStart({बफर, static_cast<std::string_view::size_type>(bufLen)}));
    int32_t आधारः_Len = keywords == शून्यसूचकः ? bufLen : keywords - बफर;
    // Remove -1 from the capacity so that this function can guarantee NUL termination.
    CheckedArrayByteSink sink(keywords == शून्यसूचकः ? बफर + bufLen : keywords,
                              बफर_क्षमता - आधारः_Len - 1);
    int32_t reslen = ulocimp_समुच्चयः_Keyword_मानम्(
        keywords == शून्यसूचकः ? std::string_view() : keywords,
        keywordName,
        keyword_मानम् == शून्यसूचकः ? std::string_view() : keyword_मानम्,
        sink,
        *स्थितिः);

    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम *स्थितिः == U_बफर_अतिप्रवाहः_ERROR ? reslen + आधारः_Len : 0;
    }

    // See the documentation for this function, it's guaranteed to never
    // overflow the buffer but instead abort with BUFFER_OVERFLOW_ERROR.
    // In this case, nothing has been written to the sink, so it cannot have Overflowed().
    U_ASSERT(!sink.Overflowed());
    U_ASSERT(reslen >= 0);
    प्रत्यागम u_terminateChars(बफर, बफर_क्षमता, reslen + आधारः_Len, स्थितिः);
}

U_EXPORT शून्य
ulocimp_समुच्चयः_Keyword_मानम्(std::string_view keywordName,
                        std::string_view keyword_मानम्,
                        CharString& localeID,
                        UError_संहिता& स्थितिः)
{
    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }
    std::string_view keywords;
    यदि (ध्रुव वर्ण* start = locale_getKeywordsStart(localeID.गन्तव्यम्_String_Piece()); start != शून्यसूचकः) {
        // This is safe because CharString::truncate() doesn't actually erase any
        // data, but simply sets the position for where new data will be written.
        int32_t size = start - localeID.दत्त();
        keywords = localeID.गन्तव्यम्_String_Piece();
        keywords.remove_उपसर्गः(size);
        localeID.truncate(size);
    }
    CharStringByteSink sink(&localeID);
    ulocimp_समुच्चयः_Keyword_मानम्(keywords, keywordName, keyword_मानम्, sink, स्थितिः);
}

U_EXPORT int32_t
ulocimp_समुच्चयः_Keyword_मानम्(std::string_view keywords,
                        std::string_view keywordName,
                        std::string_view keyword_मानम्,
                        ByteSink& sink,
                        UError_संहिता& स्थितिः)
{
    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम 0; }

    /* TODO: sorting. removal. */
    int32_t needLen = 0;
    int32_t rc;
    CharString updated_कुञ्चिकाः_And_मानानि;
    तार्किक handled_निवेशः_कुञ्चिका_And_मानम् = असत्य;
    वर्ण कुञ्चिका_मानम्_उपसर्गः = '@';

    यदि (स्थितिः == U_STRING_NOT_TERMINATED_WARNING) {
        स्थितिः = U_ZERO_ERROR;
    }
    यदि (keywordName.empty()) {
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }
    CharString canonKeywordName = locale_canonKeywordName(keywordName, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम 0;
    }

    CharString canon_Keyword_मानम्;
    प्रति (वर्ण c : keyword_मानम्) {
        यदि (!UPRV_ISALPHANUM(c) && !UPRV_OK_मानम्_PUNCTUATION(c)) {
            स्थितिः = U_ILLEGAL_तर्कः_ERROR; /* malformed key value */
            प्रत्यागम 0;
        }
        /* Should we force lowercase in value to set? */
        canon_Keyword_मानम्.append(c, स्थितिः);
    }
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम 0;
    }

    यदि (keywords.size() <= 1) {
        यदि (canon_Keyword_मानम्.is_रिक्तानि()) { /* no keywords = nothing to remove */
            U_ASSERT(स्थितिः != U_STRING_NOT_TERMINATED_WARNING);
            प्रत्यागम 0;
        }

        needLen = 1 + canonKeywordName.length() + 1 + canon_Keyword_मानम्.length();
        int32_t capacity = 0;
        वर्ण* बफर = sink.Get_Append_बफर(
                needLen, needLen, शून्यसूचकः, needLen, &capacity);
        यदि (capacity < needLen || बफर == शून्यसूचकः) {
            स्थितिः = U_बफर_अतिप्रवाहः_ERROR;
            प्रत्यागम needLen; /* no change */
        }
        वर्ण* it = बफर;

        *it++ = '@';
        uprv_memcpy(it, canonKeywordName.दत्त(), canonKeywordName.length());
        it += canonKeywordName.length();
        *it++ = '=';
        uprv_memcpy(it, canon_Keyword_मानम्.दत्त(), canon_Keyword_मानम्.length());
        sink.Append(बफर, needLen);
        U_ASSERT(स्थितिः != U_STRING_NOT_TERMINATED_WARNING);
        प्रत्यागम needLen;
    } /* end shortcut - no @ */

    /* search for keyword */
    प्रति (size_t keywordStart = 0; keywordStart != std::string_view::npos;) {
        keywordStart++; /* skip @ or ; */
        size_t अग्र_Equalsign = keywords.find('=', keywordStart);
        यदि (अग्र_Equalsign == std::string_view::npos) {
            स्थितिः = U_ILLEGAL_तर्कः_ERROR; /* key must have =value */
            प्रत्यागम 0;
        }
        /* strip leading & trailing spaces (TC decided to tolerate these) */
        यावत् (keywordStart < keywords.size() && keywords[keywordStart] == ' ') {
            keywordStart++;
        }
        size_t कुञ्चिका_मानम्_पुच्छ = अग्र_Equalsign;
        यावत् (कुञ्चिका_मानम्_पुच्छ > keywordStart && keywords[कुञ्चिका_मानम्_पुच्छ - 1] == ' ') {
            कुञ्चिका_मानम्_पुच्छ--;
        }
        /* now keyValueTail points to first char after the keyName */
        /* copy & normalize keyName from locale */
        यदि (keywordStart == कुञ्चिका_मानम्_पुच्छ) {
            स्थितिः = U_ILLEGAL_तर्कः_ERROR; /* empty keyword name in passed-in locale */
            प्रत्यागम 0;
        }
        CharString localeKeywordName;
        यावत् (keywordStart < कुञ्चिका_मानम्_पुच्छ) {
            यदि (!UPRV_ISALPHANUM(keywords[keywordStart])) {
                स्थितिः = U_ILLEGAL_तर्कः_ERROR; /* malformed keyword name */
                प्रत्यागम 0;
            }
            localeKeywordName.append(uprv_tolower(keywords[keywordStart++]), स्थितिः);
        }
        यदि (U_FAILURE(स्थितिः)) {
            प्रत्यागम 0;
        }

        size_t अग्र_Separator = keywords.find(';', अग्र_Equalsign);

        /* start processing the value part */
        अग्र_Equalsign++; /* skip '=' */
        /* First strip leading & trailing spaces (TC decided to tolerate these) */
        यावत् (अग्र_Equalsign < keywords.size() && keywords[अग्र_Equalsign] == ' ') {
            अग्र_Equalsign++;
        }
        कुञ्चिका_मानम्_पुच्छ = अग्र_Separator == std::string_view::npos ? keywords.size() : अग्र_Separator;
        यावत् (कुञ्चिका_मानम्_पुच्छ > अग्र_Equalsign && keywords[कुञ्चिका_मानम्_पुच्छ - 1] == ' ') {
            कुञ्चिका_मानम्_पुच्छ--;
        }
        यदि (अग्र_Equalsign == कुञ्चिका_मानम्_पुच्छ) {
            स्थितिः = U_ILLEGAL_तर्कः_ERROR; /* empty key value in passed-in locale */
            प्रत्यागम 0;
        }

        rc = uprv_strcmp(canonKeywordName.दत्त(), localeKeywordName.दत्त());
        यदि(rc == 0) {
            /* Current entry matches the input keyword. Update the entry */
            यदि (!canon_Keyword_मानम्.is_रिक्तानि()) { /* updating a value */
                updated_कुञ्चिकाः_And_मानानि.append(कुञ्चिका_मानम्_उपसर्गः, स्थितिः);
                कुञ्चिका_मानम्_उपसर्गः = ';'; /* for any subsequent key-value pair */
                updated_कुञ्चिकाः_And_मानानि.append(canonKeywordName, स्थितिः);
                updated_कुञ्चिकाः_And_मानानि.append('=', स्थितिः);
                updated_कुञ्चिकाः_And_मानानि.append(canon_Keyword_मानम्, स्थितिः);
            } /* else removing this entry, don't emit anything */
            handled_निवेशः_कुञ्चिका_And_मानम् = सत्य;
        } अन्यथा {
           /* input keyword sorts earlier than current entry, add before current entry */
            यदि (rc < 0 && !canon_Keyword_मानम्.is_रिक्तानि() && !handled_निवेशः_कुञ्चिका_And_मानम्) {
                /* insert new entry at this location */
                updated_कुञ्चिकाः_And_मानानि.append(कुञ्चिका_मानम्_उपसर्गः, स्थितिः);
                कुञ्चिका_मानम्_उपसर्गः = ';'; /* for any subsequent key-value pair */
                updated_कुञ्चिकाः_And_मानानि.append(canonKeywordName, स्थितिः);
                updated_कुञ्चिकाः_And_मानानि.append('=', स्थितिः);
                updated_कुञ्चिकाः_And_मानानि.append(canon_Keyword_मानम्, स्थितिः);
                handled_निवेशः_कुञ्चिका_And_मानम् = सत्य;
            }
            /* copy the current entry */
            updated_कुञ्चिकाः_And_मानानि.append(कुञ्चिका_मानम्_उपसर्गः, स्थितिः);
            कुञ्चिका_मानम्_उपसर्गः = ';'; /* for any subsequent key-value pair */
            updated_कुञ्चिकाः_And_मानानि.append(localeKeywordName, स्थितिः);
            updated_कुञ्चिकाः_And_मानानि.append('=', स्थितिः);
            updated_कुञ्चिकाः_And_मानानि.append(keywords.दत्त() + अग्र_Equalsign,
                                        static_cast<int32_t>(कुञ्चिका_मानम्_पुच्छ - अग्र_Equalsign), स्थितिः);
        }
        यदि (अग्र_Separator == std::string_view::npos && !canon_Keyword_मानम्.is_रिक्तानि() && !handled_निवेशः_कुञ्चिका_And_मानम्) {
            /* append new entry at the end, it sorts later than existing entries */
            updated_कुञ्चिकाः_And_मानानि.append(कुञ्चिका_मानम्_उपसर्गः, स्थितिः);
            /* skip keyValuePrefix update, no subsequent key-value pair */
            updated_कुञ्चिकाः_And_मानानि.append(canonKeywordName, स्थितिः);
            updated_कुञ्चिकाः_And_मानानि.append('=', स्थितिः);
            updated_कुञ्चिकाः_And_मानानि.append(canon_Keyword_मानम्, स्थितिः);
            handled_निवेशः_कुञ्चिका_And_मानम् = सत्य;
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
    यदि (!handled_निवेशः_कुञ्चिका_And_मानम् || U_FAILURE(स्थितिः)) {
        /* if input key/value specified removal of a keyword not present in locale, or
         * there was an error in CharString.append, leave original locale alone. */
        U_ASSERT(स्थितिः != U_STRING_NOT_TERMINATED_WARNING);
        // The sink is expected to be a buffer which already contains the full
        // locale string, so when it isn't going to be modified there's no need
        // to actually write any data to it, as the data is already there. Only
        // the first character needs to be overwritten (changing '\0' to '@').
        needLen = static_cast<int32_t>(keywords.size());
        int32_t capacity = 0;
        वर्ण* बफर = sink.Get_Append_बफर(
                needLen, needLen, शून्यसूचकः, needLen, &capacity);
        यदि (capacity < needLen || बफर == शून्यसूचकः) {
            स्थितिः = U_बफर_अतिप्रवाहः_ERROR;
        } अन्यथा {
            *बफर = '@';
            sink.Append(बफर, needLen);
        }
        प्रत्यागम needLen;
    }

    needLen = updated_कुञ्चिकाः_And_मानानि.length();
    // Check to see can we fit the updatedKeysAndValues, if not, return
    // U_BUFFER_OVERFLOW_ERROR without copy updatedKeysAndValues into it.
    // We do this because this API function does not behave like most others:
    // It promises never to set a U_STRING_NOT_TERMINATED_WARNING.
    // When the contents fits but without the terminating NUL, in this case we need to not change
    // the buffer contents and return with a buffer overflow error.
    यदि (needLen > 0) {
        int32_t capacity = 0;
        वर्ण* बफर = sink.Get_Append_बफर(
                needLen, needLen, शून्यसूचकः, needLen, &capacity);
        यदि (capacity < needLen || बफर == शून्यसूचकः) {
            स्थितिः = U_बफर_अतिप्रवाहः_ERROR;
            प्रत्यागम needLen;
        }
        uprv_memcpy(बफर, updated_कुञ्चिकाः_And_मानानि.दत्त(), needLen);
        sink.Append(बफर, needLen);
    }
    U_ASSERT(स्थितिः != U_STRING_NOT_TERMINATED_WARNING);
    प्रत्यागम needLen;
}

/* ### ID parsing implementation **************************************************/

नामाकाश {

अन्तःस्थ तार्किक is_उपसर्गः_Letter(वर्ण a) { प्रत्यागम a == 'x' || a == 'X' || a == 'i' || a == 'I'; }

/*returns true if one of the special prefixes is here (s=string)
  'x-' or 'i-' */
अन्तःस्थ तार्किक _isIDPrefix(std::string_view s) {
    प्रत्यागम s.size() >= 2 && is_उपसर्गः_Letter(s[0]) && _isIDSeparator(s[1]);
}

/* Dot terminates it because of POSIX form  where dot precedes the codepage
 * except for variant
 */
अन्तःस्थ तार्किक _isTerminator(वर्ण a) { प्रत्यागम a == '.' || a == '@'; }

अन्तःस्थ तार्किक _isBCP47Extension(std::string_view p) {
    प्रत्यागम p.size() >= 3 &&
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
std::optional<int16_t> find_सूचकांक_2(ध्रुव वर्ण* ध्रुव* list, ध्रुव वर्ण* कुञ्चिका)
{
    ध्रुव वर्ण* ध्रुव* anchor = list;
    int32_t pass = 0;

    /* Make two passes through two nullptr-terminated arrays at 'list' */
    यावत् (pass++ < 2) {
        यावत् (*list) {
            यदि (uprv_strcmp(कुञ्चिका, *list) == 0) {
                प्रत्यागम static_cast<int16_t>(list - anchor);
            }
            list++;
        }
        ++list;     /* skip final nullptr *CWB*/
    }
    प्रत्यागम std::nullopt;
}

}  // namespace

U_CFUNC ध्रुव वर्ण*
uloc_getCurrentCountryID(ध्रुव वर्ण* oldID U_LIFETIME_BOUND) {
    std::optional<int16_t> विस्थापनम् = find_सूचकांक_2(DEPRECATED_COUNTRIES, oldID);
    प्रत्यागम विस्थापनम्.has_value() ? REPLACEMENT_COUNTRIES[*विस्थापनम्] : oldID;
}
U_CFUNC ध्रुव वर्ण*
uloc_getCurrentLanguageID(ध्रुव वर्ण* oldID U_LIFETIME_BOUND) {
    std::optional<int16_t> विस्थापनम् = find_सूचकांक_2(DEPRECATED_LANGUAGES, oldID);
    प्रत्यागम विस्थापनम्.has_value() ? REPLACEMENT_LANGUAGES[*विस्थापनम्] : oldID;
}

नामाकाश {

/*
 * the internal functions _getLanguage(), _getScript(), _getRegion(), _getVariant()
 * avoid duplicating code to handle the earlier locale ID pieces
 * in the functions for the later ones by
 * setting the *pEnd pointer to where they stopped parsing
 *
 * TODO try to use this in Locale
 */

size_t _getLanguage(std::string_view localeID, ByteSink* sink, UError_संहिता& स्थितिः) {
    size_t skip = 0;
    यदि (localeID.size() == 4 && uprv_strnicmp(localeID.दत्त(), "root", 4) == 0) {
        skip = 4;
        localeID.remove_उपसर्गः(skip);
    } अन्यथा यदि (localeID.size() >= 3 && uprv_strnicmp(localeID.दत्त(), "und", 3) == 0 &&
               (localeID.size() == 3 ||
                localeID[3] == '-' ||
                localeID[3] == '_' ||
                localeID[3] == '@')) {
        skip = 3;
        localeID.remove_उपसर्गः(skip);
    }

    constexpr int32_t MAXLEN = ULOC_LANG_क्षमता - 1;  // Minus NUL.

    /* if it starts with i- or x- then copy that prefix */
    size_t len = _isIDPrefix(localeID) ? 2 : 0;
    यावत् (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len])) {
        यदि (len == MAXLEN) {
            स्थितिः = U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम 0;
        }
        len++;
    }

    यदि (sink == शून्यसूचकः || len == 0) { प्रत्यागम skip + len; }

    int32_t min_क्षमता = uprv_max(static_cast<int32_t>(len), 4);  // Minimum 3 letters plus NUL.
    वर्ण scratch[MAXLEN];
    int32_t capacity = 0;
    वर्ण* बफर = sink->Get_Append_बफर(
            min_क्षमता, min_क्षमता, scratch, UPRV_LENGTHOF(scratch), &capacity);

    प्रति (size_t i = 0; i < len; ++i) {
        बफर[i] = uprv_tolower(localeID[i]);
    }
    यदि (localeID.size() >= 2 && _isIDSeparator(localeID[1])) {
        बफर[1] = '-';
    }

    यदि (len == 3) {
        /* convert 3 character code to 2 character code if possible *CWB*/
        U_ASSERT(capacity >= 4);
        बफर[3] = '\0';
        std::optional<int16_t> विस्थापनम् = find_सूचकांक_2(LANGUAGES_3, बफर);
        यदि (विस्थापनम्.has_value()) {
            ध्रुव वर्ण* ध्रुव alias = LANGUAGES[*विस्थापनम्];
            sink->Append(alias, static_cast<int32_t>(uprv_strlen(alias)));
            प्रत्यागम skip + len;
        }
    }

    sink->Append(बफर, static_cast<int32_t>(len));
    प्रत्यागम skip + len;
}

size_t _getScript(std::string_view localeID, ByteSink* sink) {
    constexpr int32_t दीर्घता_3 = 4;

    size_t len = 0;
    यावत् (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len]) &&
            uprv_isASCIILetter(localeID[len])) {
        यदि (len == दीर्घता_3) { प्रत्यागम 0; }
        len++;
    }
    यदि (len != दीर्घता_3) { प्रत्यागम 0; }

    यदि (sink == शून्यसूचकः) { प्रत्यागम len; }

    वर्ण scratch[दीर्घता_3];
    int32_t capacity = 0;
    वर्ण* बफर = sink->Get_Append_बफर(
            दीर्घता_3, दीर्घता_3, scratch, UPRV_LENGTHOF(scratch), &capacity);

    बफर[0] = uprv_toupper(localeID[0]);
    प्रति (int32_t i = 1; i < दीर्घता_3; ++i) {
        बफर[i] = uprv_tolower(localeID[i]);
    }

    sink->Append(बफर, दीर्घता_3);
    प्रत्यागम len;
}

size_t _getRegion(std::string_view localeID, ByteSink* sink) {
    constexpr int32_t MINLEN = 2;
    constexpr int32_t MAXLEN = ULOC_COUNTRY_क्षमता - 1;  // Minus NUL.

    size_t len = 0;
    यावत् (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len])) {
        यदि (len == MAXLEN) { प्रत्यागम 0; }
        len++;
    }
    यदि (len < MINLEN) { प्रत्यागम 0; }

    यदि (sink == शून्यसूचकः) { प्रत्यागम len; }

    वर्ण scratch[ULOC_COUNTRY_क्षमता];
    int32_t capacity = 0;
    वर्ण* बफर = sink->Get_Append_बफर(
            ULOC_COUNTRY_क्षमता,
            ULOC_COUNTRY_क्षमता,
            scratch,
            UPRV_LENGTHOF(scratch),
            &capacity);

    प्रति (size_t i = 0; i < len; ++i) {
        बफर[i] = uprv_toupper(localeID[i]);
    }

    यदि (len == 3) {
        /* convert 3 character code to 2 character code if possible *CWB*/
        U_ASSERT(capacity >= 4);
        बफर[3] = '\0';
        std::optional<int16_t> विस्थापनम् = find_सूचकांक_2(COUNTRIES_3, बफर);
        यदि (विस्थापनम्.has_value()) {
            ध्रुव वर्ण* ध्रुव alias = COUNTRIES[*विस्थापनम्];
            sink->Append(alias, static_cast<int32_t>(uprv_strlen(alias)));
            प्रत्यागम len;
        }
    }

    sink->Append(बफर, static_cast<int32_t>(len));
    प्रत्यागम len;
}

/**
 * @param needSeparator if true, then add leading '_' if any variants
 * are added to 'variant'
 */
size_t
_getVariant(std::string_view localeID,
            वर्ण पूर्व,
            ByteSink* sink,
            तार्किक needSeparator,
            UError_संहिता& स्थितिः) {
    यदि (U_FAILURE(स्थितिः) || localeID.empty()) प्रत्यागम 0;

    // Reasonable upper limit for variants
    // There are no strict limitation of the syntax of variant in the legacy
    // locale format. If the locale is constructed from unicode_locale_id
    // as defined in UTS35, then we know each unicode_variant_subtag
    // could have max length of 8 ((alphanum{5,8} | digit alphanum{3})
    // 179 would allow 20 unicode_variant_subtag with sep in the
    // unicode_locale_id
    // 8*20 + 1*(20-1) = 179
    constexpr int32_t MAX_VARIANTS_दीर्घता = 179;

    /* get one or more variant tags and separate them with '_' */
    size_t सूचकांक = 0;
    यदि (_isIDSeparator(पूर्व)) {
        /* get a variant string after a '-' or '_' */
        प्रति (std::string_view sub = localeID;;) {
            size_t next = sub.find_first_of(".@_-");
            // For historical reasons, a trailing separator is included in the variant.
            तार्किक परिसमाप्तम् = next == std::string_view::npos || next + 1 == sub.length();
            size_t सीमा = परिसमाप्तम् ? sub.length() : next;
            सूचकांक += सीमा;
            यदि (सूचकांक > MAX_VARIANTS_दीर्घता) {
                स्थितिः = U_ILLEGAL_तर्कः_ERROR;
                प्रत्यागम 0;
            }

            यदि (sink != शून्यसूचकः) {
                यदि (needSeparator) {
                    sink->Append("_", 1);
                } अन्यथा {
                    needSeparator = सत्य;
                }

                int32_t length = static_cast<int32_t>(सीमा);
                int32_t min_क्षमता = uprv_min(length, MAX_VARIANTS_दीर्घता);
                वर्ण scratch[MAX_VARIANTS_दीर्घता];
                int32_t capacity = 0;
                वर्ण* बफर = sink->Get_Append_बफर(
                        min_क्षमता, min_क्षमता, scratch, UPRV_LENGTHOF(scratch), &capacity);

                प्रति (size_t i = 0; i < सीमा; ++i) {
                    बफर[i] = uprv_toupper(sub[i]);
                }
                sink->Append(बफर, length);
            }

            यदि (परिसमाप्तम्) { प्रत्यागम सूचकांक; }
            sub.remove_उपसर्गः(next);
            यदि (_isTerminator(sub.front()) || _isBCP47Extension(sub)) { प्रत्यागम सूचकांक; }
            sub.remove_उपसर्गः(1);
            सूचकांक++;
        }
    }

    size_t skip = 0;
    /* if there is no variant tag after a '-' or '_' then look for '@' */
    यदि (पूर्व == '@') {
        /* keep localeID */
    } अन्यथा यदि (ध्रुव वर्ण* p = locale_getKeywordsStart(localeID); p != शून्यसूचकः) {
        skip = 1 + p - localeID.दत्त(); /* point after the '@' */
        localeID.remove_उपसर्गः(skip);
    } अन्यथा {
        प्रत्यागम 0;
    }
    प्रति (; सूचकांक < localeID.size() && !_isTerminator(localeID[सूचकांक]); सूचकांक++) {
        यदि (सूचकांक >= MAX_VARIANTS_दीर्घता) { // same as length > MAX_VARIANTS_LENGTH
            स्थितिः = U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम 0;
        }
        यदि (needSeparator) {
            यदि (sink != शून्यसूचकः) {
                sink->Append("_", 1);
            }
            needSeparator = असत्य;
        }
        यदि (sink != शून्यसूचकः) {
            वर्ण c = uprv_toupper(localeID[सूचकांक]);
            यदि (c == '-' || c == ',') c = '_';
            sink->Append(&c, 1);
        }
    }
    प्रत्यागम skip + सूचकांक;
}

}  // namespace

U_EXPORT CharString
ulocimp_getLanguage(std::string_view localeID, UError_संहिता& स्थितिः) {
    प्रत्यागम ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Char_String(
        [&](ByteSink& sink, UError_संहिता& स्थितिः) {
            ulocimp_getSubtags(
                    localeID,
                    &sink,
                    शून्यसूचकः,
                    शून्यसूचकः,
                    शून्यसूचकः,
                    शून्यसूचकः,
                    स्थितिः);
        },
        स्थितिः);
}

U_EXPORT CharString
ulocimp_getScript(std::string_view localeID, UError_संहिता& स्थितिः) {
    प्रत्यागम ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Char_String(
        [&](ByteSink& sink, UError_संहिता& स्थितिः) {
            ulocimp_getSubtags(
                    localeID,
                    शून्यसूचकः,
                    &sink,
                    शून्यसूचकः,
                    शून्यसूचकः,
                    शून्यसूचकः,
                    स्थितिः);
        },
        स्थितिः);
}

U_EXPORT CharString
ulocimp_getRegion(std::string_view localeID, UError_संहिता& स्थितिः) {
    प्रत्यागम ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Char_String(
        [&](ByteSink& sink, UError_संहिता& स्थितिः) {
            ulocimp_getSubtags(
                    localeID,
                    शून्यसूचकः,
                    शून्यसूचकः,
                    &sink,
                    शून्यसूचकः,
                    शून्यसूचकः,
                    स्थितिः);
        },
        स्थितिः);
}

U_EXPORT CharString
ulocimp_getVariant(std::string_view localeID, UError_संहिता& स्थितिः) {
    प्रत्यागम ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Char_String(
        [&](ByteSink& sink, UError_संहिता& स्थितिः) {
            ulocimp_getSubtags(
                    localeID,
                    शून्यसूचकः,
                    शून्यसूचकः,
                    शून्यसूचकः,
                    &sink,
                    शून्यसूचकः,
                    स्थितिः);
        },
        स्थितिः);
}

U_EXPORT शून्य
ulocimp_getSubtags(
        std::string_view localeID,
        CharString* language,
        CharString* script,
        CharString* region,
        CharString* variant,
        ध्रुव वर्ण** p_अन्तः,
        UError_संहिता& स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }

    std::optional<CharStringByteSink> languageSink;
    std::optional<CharStringByteSink> scriptSink;
    std::optional<CharStringByteSink> regionSink;
    std::optional<CharStringByteSink> variantSink;

    यदि (language != शून्यसूचकः) { languageSink.emplace(language); }
    यदि (script != शून्यसूचकः) { scriptSink.emplace(script); }
    यदि (region != शून्यसूचकः) { regionSink.emplace(region); }
    यदि (variant != शून्यसूचकः) { variantSink.emplace(variant); }

    ulocimp_getSubtags(
            localeID,
            languageSink.has_value() ? &*languageSink : शून्यसूचकः,
            scriptSink.has_value() ? &*scriptSink : शून्यसूचकः,
            regionSink.has_value() ? &*regionSink : शून्यसूचकः,
            variantSink.has_value() ? &*variantSink : शून्यसूचकः,
            p_अन्तः,
            स्थितिः);
}

U_EXPORT शून्य
ulocimp_getSubtags(
        std::string_view localeID,
        ByteSink* language,
        ByteSink* script,
        ByteSink* region,
        ByteSink* variant,
        ध्रुव वर्ण** p_अन्तः,
        UError_संहिता& स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }

    यदि (p_अन्तः != शून्यसूचकः) {
        *p_अन्तः = localeID.दत्त();
    } अन्यथा यदि (language == शून्यसूचकः &&
               script == शून्यसूचकः &&
               region == शून्यसूचकः &&
               variant == शून्यसूचकः) {
        प्रत्यागम;
    }

    यदि (localeID.empty()) { प्रत्यागम; }

    तार्किक hasRegion = असत्य;

    {
        size_t len = _getLanguage(localeID, language, स्थितिः);
        यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }
        यदि (len > 0) {
            localeID.remove_उपसर्गः(len);
        }
    }

    यदि (p_अन्तः != शून्यसूचकः) {
        *p_अन्तः = localeID.दत्त();
    } अन्यथा यदि (script == शून्यसूचकः &&
               region == शून्यसूचकः &&
               variant == शून्यसूचकः) {
        प्रत्यागम;
    }

    यदि (localeID.empty()) { प्रत्यागम; }

    यदि (_isIDSeparator(localeID.front())) {
        std::string_view sub = localeID;
        sub.remove_उपसर्गः(1);
        size_t len = _getScript(sub, script);
        यदि (len > 0) {
            localeID.remove_उपसर्गः(len + 1);
            यदि (p_अन्तः != शून्यसूचकः) { *p_अन्तः = localeID.दत्त(); }
        }
    }

    यदि ((region == शून्यसूचकः && variant == शून्यसूचकः && p_अन्तः == शून्यसूचकः) || localeID.empty()) { प्रत्यागम; }

    यदि (_isIDSeparator(localeID.front())) {
        std::string_view sub = localeID;
        sub.remove_उपसर्गः(1);
        size_t len = _getRegion(sub, region);
        यदि (len > 0) {
            hasRegion = सत्य;
            localeID.remove_उपसर्गः(len + 1);
            यदि (p_अन्तः != शून्यसूचकः) { *p_अन्तः = localeID.दत्त(); }
        }
    }

    यदि ((variant == शून्यसूचकः && p_अन्तः == शून्यसूचकः) || localeID.empty()) { प्रत्यागम; }

    तार्किक hasVariant = असत्य;

    यदि (_isIDSeparator(localeID.front()) && !_isBCP47Extension(localeID)) {
        std::string_view sub = localeID;
        /* If there was no country ID, skip a possible extra IDSeparator */
        size_t skip = !hasRegion && localeID.size() > 1 && _isIDSeparator(localeID[1]) ? 2 : 1;
        sub.remove_उपसर्गः(skip);
        size_t len = _getVariant(sub, localeID[0], variant, असत्य, स्थितिः);
        यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }
        यदि (len > 0) {
            hasVariant = सत्य;
            localeID.remove_उपसर्गः(skip + len);
            यदि (p_अन्तः != शून्यसूचकः) { *p_अन्तः = localeID.दत्त(); }
        }
    }

    यदि ((variant == शून्यसूचकः && p_अन्तः == शून्यसूचकः) || localeID.empty()) { प्रत्यागम; }

    यदि (_isBCP47Extension(localeID)) {
        localeID.remove_उपसर्गः(2);
        constexpr वर्ण vaposix[] = "-va-posix";
        constexpr size_t length = परिमाण vaposix - 1;
        प्रति (size_t next;; localeID.remove_उपसर्गः(next)) {
            next = localeID.find('-', 1);
            यदि (next == std::string_view::npos) { भङ्ग; }
            next = localeID.find('-', next + 1);
            तार्किक परिसमाप्तम् = next == std::string_view::npos;
            std::string_view sub = localeID;
            यदि (!परिसमाप्तम्) { sub.remove_suffix(sub.length() - next); }

            यदि (sub.length() == length && uprv_strnicmp(sub.दत्त(), vaposix, length) == 0) {
                यदि (variant != शून्यसूचकः) {
                    यदि (hasVariant) { variant->Append("_", 1); }
                    constexpr वर्ण posix[] = "POSIX";
                    variant->Append(posix, परिमाण posix - 1);
                }
                यदि (p_अन्तः != शून्यसूचकः) { *p_अन्तः = localeID.दत्त() + length; }
            }

            यदि (परिसमाप्तम्) { भङ्ग; }
        }
    }
}

/* Keyword enumeration */

प्रकारनिर्देश संरचना UKeywordsContext {
    वर्ण* keywords;
    वर्ण* current;
} UKeywordsContext;

U_CDECL_BEGIN

स्थिर शून्य U_CALLCONV
uloc_kw_closeKeywords(UEnumeration *enumerator) {
    uprv_free(((UKeywordsContext *)enumerator->context)->keywords);
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

स्थिर int32_t U_CALLCONV
uloc_kw_गणनम्_Keywords(UEnumeration *en, UError_संहिता * /*स्थितिः*/) {
    वर्ण *kw = ((UKeywordsContext *)en->context)->keywords;
    int32_t परिणाम = 0;
    यावत्(*kw) {
        परिणाम++;
        kw += uprv_strlen(kw)+1;
    }
    प्रत्यागम परिणाम;
}

स्थिर ध्रुव वर्ण * U_CALLCONV
uloc_kw_अग्र_Keyword(UEnumeration* en,
                    int32_t* परिणाम_दीर्घता,
                    UError_संहिता* /*स्थितिः*/) {
    ध्रुव वर्ण* परिणाम = ((UKeywordsContext *)en->context)->current;
    int32_t len = 0;
    यदि(*परिणाम) {
        len = (int32_t)uprv_strlen(((UKeywordsContext *)en->context)->current);
        ((UKeywordsContext *)en->context)->current += len+1;
    } अन्यथा {
        परिणाम = शून्यसूचकः;
    }
    यदि (परिणाम_दीर्घता) {
        *परिणाम_दीर्घता = len;
    }
    प्रत्यागम परिणाम;
}

स्थिर शून्य U_CALLCONV
uloc_kw_resetKeywords(UEnumeration* en,
                      UError_संहिता* /*स्थितिः*/) {
    ((UKeywordsContext *)en->context)->current = ((UKeywordsContext *)en->context)->keywords;
}

U_CDECL_अन्तः


स्थिर ध्रुव UEnumeration gKeywordsEnum = {
    शून्यसूचकः,
    शून्यसूचकः,
    uloc_kw_closeKeywords,
    uloc_kw_गणनम्_Keywords,
    uenum_unextDefault,
    uloc_kw_अग्र_Keyword,
    uloc_kw_resetKeywords
};

U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywordList(ध्रुव वर्ण *keywordList, int32_t keyword_List_आकार, UError_संहिता* स्थितिः)
{
    यदि (U_FAILURE(*स्थितिः)) { प्रत्यागम शून्यसूचकः; }

    Local_स्मृतिः<UKeywordsContext> myContext;
    Local_स्मृतिः<UEnumeration> परिणाम;

    myContext.adoptInstead(static_cast<UKeywordsContext *>(uprv_malloc(परिमाण(UKeywordsContext))));
    परिणाम.adoptInstead(static_cast<UEnumeration *>(uprv_malloc(परिमाण(UEnumeration))));
    यदि (myContext.isNull() || परिणाम.isNull()) {
        *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    uprv_memcpy(परिणाम.getAlias(), &gKeywordsEnum, परिमाण(UEnumeration));
    myContext->keywords = static_cast<वर्ण *>(uprv_malloc(keyword_List_आकार+1));
    यदि (myContext->keywords == शून्यसूचकः) {
        *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    uprv_memcpy(myContext->keywords, keywordList, keyword_List_आकार);
    myContext->keywords[keyword_List_आकार] = 0;
    myContext->current = myContext->keywords;
    परिणाम->context = myContext.orphan();
    प्रत्यागम परिणाम.orphan();
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywords(ध्रुव वर्ण* localeID,
                        UError_संहिता* स्थितिः)
{
    यदि(स्थितिः==शून्यसूचकः || U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    CharString अस्थायी_बफर;
    ध्रुव वर्ण* tmpLocaleID;

    यदि (localeID != शून्यसूचकः && _hasBCP47Extension(localeID)) {
        अस्थायी_बफर = ulocimp_for_Language_चिह्नकम्(localeID, -1, शून्यसूचकः, *स्थितिः);
        tmpLocaleID = U_SUCCESS(*स्थितिः) && !अस्थायी_बफर.is_रिक्तानि() ? अस्थायी_बफर.दत्त() : localeID;
    } अन्यथा {
        यदि (localeID==शून्यसूचकः) {
            localeID=uloc_getDefault();
        }
        tmpLocaleID=localeID;
    }

    ulocimp_getSubtags(
            tmpLocaleID,
            शून्यसूचकः,
            शून्यसूचकः,
            शून्यसूचकः,
            शून्यसूचकः,
            &tmpLocaleID,
            *स्थितिः);
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    /* keywords are located after '@' */
    यदि((tmpLocaleID = locale_getKeywordsStart(tmpLocaleID)) != शून्यसूचकः) {
        CharString keywords = ulocimp_getKeywords(tmpLocaleID + 1, '@', असत्य, *स्थितिः);
        यदि (U_FAILURE(*स्थितिः)) {
            प्रत्यागम शून्यसूचकः;
        }
        प्रत्यागम uloc_openKeywordList(keywords.दत्त(), keywords.length(), स्थितिः);
    }
    प्रत्यागम शून्यसूचकः;
}


/* bit-flags for 'options' parameter of _canonicalize */
#परिभाषय _ULOC_STRIP_KEYWORDS 0x2
#परिभाषय _ULOC_CANONICALIZE   0x1

नामाकाश {

अन्तःस्थ तार्किक OPTION_समुच्चयः(uint32_t options, uint32_t mask) { प्रत्यागम (options & mask) != 0; }

constexpr वर्ण i_default[] = {'i', '-', 'd', 'e', 'f', 'a', 'u', 'l', 't'};
constexpr int32_t I_DEFAULT_दीर्घता = UPRV_LENGTHOF(i_default);

/**
 * Canonicalize the given localeID, to level 1 or to level 2,
 * depending on the options.  To specify level 1, pass in options=0.
 * To specify level 2, pass in options=_ULOC_CANONICALIZE.
 *
 * This is the code underlying uloc_getName and uloc_canonicalize.
 */
शून्य
_canonicalize(std::string_view localeID,
              ByteSink& sink,
              uint32_t options,
              UError_संहिता& err) {
    यदि (U_FAILURE(err)) {
        प्रत्यागम;
    }

    int32_t j, क्षेत्रम्_गणनम्=0;
    CharString अस्थायी_बफर;  // if localeID has a BCP47 extension, tmpLocaleID points to this
    CharString localeIDWithHyphens;  // if localeID has a BPC47 extension and have _, tmpLocaleID points to this
    std::string_view origLocaleID;
    std::string_view tmpLocaleID;
    size_t keyword_नियोजय = std::string_view::npos;
    size_t separatorIndicator = std::string_view::npos;

    यदि (_hasBCP47Extension(localeID)) {
        std::string_view localeIDPtr = localeID;

        // convert all underbars to hyphens, unless the "BCP47 extension" comes at the beginning of the string
        यदि (localeID.size() >= 2 && localeID.find('_') != std::string_view::npos && localeID[1] != '-' && localeID[1] != '_') {
            localeIDWithHyphens.append(localeID, err);
            यदि (U_SUCCESS(err)) {
                प्रति (वर्ण* p = localeIDWithHyphens.दत्त(); *p != '\0'; ++p) {
                    यदि (*p == '_') {
                        *p = '-';
                    }
                }
                localeIDPtr = localeIDWithHyphens.गन्तव्यम्_String_Piece();
            }
        }

        अस्थायी_बफर = ulocimp_for_Language_चिह्नकम्(localeIDPtr.दत्त(), static_cast<int32_t>(localeIDPtr.size()), शून्यसूचकः, err);
        tmpLocaleID = U_SUCCESS(err) && !अस्थायी_बफर.is_रिक्तानि() ? static_cast<std::string_view>(अस्थायी_बफर.गन्तव्यम्_String_Piece()) : localeIDPtr;
    } अन्यथा {
        tmpLocaleID=localeID;
    }

    origLocaleID=tmpLocaleID;

    /* get all pieces, one after another, and separate with '_' */
    CharString चिह्नकम्;
    CharString script;
    CharString country;
    CharString variant;
    ध्रुव वर्ण* अन्तः = शून्यसूचकः;
    ulocimp_getSubtags(
            tmpLocaleID,
            &चिह्नकम्,
            &script,
            &country,
            &variant,
            &अन्तः,
            err);
    यदि (U_FAILURE(err)) {
        प्रत्यागम;
    }
    U_ASSERT(अन्तः != शून्यसूचकः);
    यदि (अन्तः > tmpLocaleID.दत्त()) {
        tmpLocaleID.remove_उपसर्गः(अन्तः - tmpLocaleID.दत्त());
    }

    यदि (चिह्नकम्.length() == I_DEFAULT_दीर्घता && origLocaleID.length() >= I_DEFAULT_दीर्घता &&
            uprv_strncmp(origLocaleID.दत्त(), i_default, I_DEFAULT_दीर्घता) == 0) {
        चिह्नकम्.clear();
        चिह्नकम्.append(uloc_getDefault(), err);
    } अन्यथा {
        यदि (!script.is_रिक्तानि()) {
            ++क्षेत्रम्_गणनम्;
            चिह्नकम्.append('_', err);
            चिह्नकम्.append(script, err);
        }
        यदि (!country.is_रिक्तानि()) {
            ++क्षेत्रम्_गणनम्;
            चिह्नकम्.append('_', err);
            चिह्नकम्.append(country, err);
        }
        यदि (!variant.is_रिक्तानि()) {
            ++क्षेत्रम्_गणनम्;
            यदि (country.is_रिक्तानि()) {
                चिह्नकम्.append('_', err);
            }
            चिह्नकम्.append('_', err);
            चिह्नकम्.append(variant, err);
        }
    }

    /* Copy POSIX-style charset specifier, if any [mr.utf8] */
    यदि (!OPTION_समुच्चयः(options, _ULOC_CANONICALIZE) && !tmpLocaleID.empty() && tmpLocaleID.front() == '.') {
        चिह्नकम्.append('.', err);
        tmpLocaleID.remove_उपसर्गः(1);
        size_t length;
        यदि (size_t atPos = tmpLocaleID.find('@'); atPos != std::string_view::npos) {
            length = atPos;
        } अन्यथा {
            length = tmpLocaleID.length();
        }
        // The longest charset name we found in IANA charset registry
        // https://www.iana.org/assignments/character-sets/ is
        // "Extended_UNIX_Code_Packed_Format_for_Japanese" in length 45.
        // we therefore restrict the length here to be 64 which is a power of 2
        // number that is longer than 45.
        constexpr size_t k_Max_Charset_दीर्घता = 64;
        यदि (length > k_Max_Charset_दीर्घता) {
           err = U_ILLEGAL_तर्कः_ERROR; /* malformed keyword name */
           प्रत्यागम;
        }
        यदि (length > 0) {
            चिह्नकम्.append(tmpLocaleID.दत्त(), static_cast<int32_t>(length), err);
            tmpLocaleID.remove_उपसर्गः(length);
        }
    }

    /* Scan ahead to next '@' and determine if it is followed by '=' and/or ';'
       After this, tmpLocaleID either starts at '@' or is empty. */
    यदि (ध्रुव वर्ण* start = locale_getKeywordsStart(tmpLocaleID); start != शून्यसूचकः) {
        यदि (start > tmpLocaleID.दत्त()) {
            tmpLocaleID.remove_उपसर्गः(start - tmpLocaleID.दत्त());
        }
        keyword_नियोजय = tmpLocaleID.find('=');
        separatorIndicator = tmpLocaleID.find(';');
    } अन्यथा {
        tmpLocaleID = {};
    }

    /* Copy POSIX-style variant, if any [mr@FOO] */
    यदि (!OPTION_समुच्चयः(options, _ULOC_CANONICALIZE) &&
        !tmpLocaleID.empty() && keyword_नियोजय == std::string_view::npos) {
        चिह्नकम्.append(tmpLocaleID, err);
        tmpLocaleID = {};
    }

    यदि (OPTION_समुच्चयः(options, _ULOC_CANONICALIZE)) {
        /* Handle @FOO variant if @ is present and not followed by = */
        यदि (!tmpLocaleID.empty() && keyword_नियोजय == std::string_view::npos) {
            /* Add missing '_' if needed */
            यदि (क्षेत्रम्_गणनम् < 2 || (क्षेत्रम्_गणनम् < 3 && !script.is_रिक्तानि())) {
                कुरु {
                    चिह्नकम्.append('_', err);
                    ++क्षेत्रम्_गणनम्;
                } यावत्(क्षेत्रम्_गणनम्<2);
            }

            CharStringByteSink s(&चिह्नकम्);
            std::string_view sub = tmpLocaleID;
            sub.remove_उपसर्गः(1);
            _getVariant(sub, '@', &s, !variant.is_रिक्तानि(), err);
            यदि (U_FAILURE(err)) { प्रत्यागम; }
        }

        /* Look up the ID in the canonicalization map */
        प्रति (j=0; j<UPRV_LENGTHOF(CANONICALIZE_MAP); j++) {
            StringPiece id(CANONICALIZE_MAP[j].id);
            यदि (चिह्नकम् == id) {
                यदि (id.empty() && !tmpLocaleID.empty()) {
                    भङ्ग; /* Don't remap "" if keywords present */
                }
                चिह्नकम्.clear();
                चिह्नकम्.append(CANONICALIZE_MAP[j].canonicalID, err);
                भङ्ग;
            }
        }
    }

    sink.Append(चिह्नकम्.दत्त(), चिह्नकम्.length());

    यदि (!OPTION_समुच्चयः(options, _ULOC_STRIP_KEYWORDS)) {
        यदि (!tmpLocaleID.empty() && keyword_नियोजय != std::string_view::npos &&
            (separatorIndicator == std::string_view::npos || separatorIndicator > keyword_नियोजय)) {
            sink.Append("@", 1);
            ++क्षेत्रम्_गणनम्;
            tmpLocaleID.remove_उपसर्गः(1);
            ulocimp_getKeywords(tmpLocaleID, '@', sink, सत्य, err);
        }
    }
}

}  // namespace

/* ### ID parsing API **************************************************/

U_CAPI int32_t  U_EXPORT2
uloc_getParent(ध्रुव वर्ण*    localeID,
               वर्ण* parent,
               int32_t parent_क्षमता,
               UError_संहिता* err)
{
    प्रत्यागम ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Terminated_Chars(
        parent, parent_क्षमता,
        [&](ByteSink& sink, UError_संहिता& स्थितिः) {
            ulocimp_getParent(localeID, sink, स्थितिः);
        },
        *err);
}

U_EXPORT CharString
ulocimp_getParent(ध्रुव वर्ण* localeID,
                  UError_संहिता& err)
{
    प्रत्यागम ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Char_String(
        [&](ByteSink& sink, UError_संहिता& स्थितिः) {
            ulocimp_getParent(localeID, sink, स्थितिः);
        },
        err);
}

U_EXPORT शून्य
ulocimp_getParent(ध्रुव वर्ण* localeID,
                  icu::ByteSink& sink,
                  UError_संहिता& err)
{
    यदि (U_FAILURE(err)) { प्रत्यागम; }

    ध्रुव वर्ण *चरमः_Underscore;
    int32_t i;

    यदि (localeID == शून्यसूचकः)
        localeID = uloc_getDefault();

    चरमः_Underscore=uprv_strrchr(localeID, '_');
    यदि(चरमः_Underscore!=शून्यसूचकः) {
        i = static_cast<int32_t>(चरमः_Underscore - localeID);
    } अन्यथा {
        i=0;
    }

    यदि (i > 0) {
        यदि (uprv_strnicmp(localeID, "und_", 4) == 0) {
            localeID += 3;
            i -= 3;
        }
        sink.Append(localeID, i);
    }
}

U_CAPI int32_t U_EXPORT2
uloc_getLanguage(ध्रुव वर्ण*    localeID,
         वर्ण* language,
         int32_t language_क्षमता,
         UError_संहिता* err)
{
    यदि (localeID == शून्यसूचकः) {
        localeID = uloc_getDefault();
    }

    /* uloc_getLanguage will return a 2 character iso-639 code if one exists. *CWB*/
    प्रत्यागम ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Terminated_Chars(
        language, language_क्षमता,
        [&](ByteSink& sink, UError_संहिता& स्थितिः) {
            ulocimp_getSubtags(
                    localeID,
                    &sink,
                    शून्यसूचकः,
                    शून्यसूचकः,
                    शून्यसूचकः,
                    शून्यसूचकः,
                    स्थितिः);
        },
        *err);
}

U_CAPI int32_t U_EXPORT2
uloc_getScript(ध्रुव वर्ण*    localeID,
         वर्ण* script,
         int32_t script_क्षमता,
         UError_संहिता* err)
{
    यदि (localeID == शून्यसूचकः) {
        localeID = uloc_getDefault();
    }

    प्रत्यागम ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Terminated_Chars(
        script, script_क्षमता,
        [&](ByteSink& sink, UError_संहिता& स्थितिः) {
            ulocimp_getSubtags(
                    localeID,
                    शून्यसूचकः,
                    &sink,
                    शून्यसूचकः,
                    शून्यसूचकः,
                    शून्यसूचकः,
                    स्थितिः);
        },
        *err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getCountry(ध्रुव वर्ण* localeID,
            वर्ण* country,
            int32_t country_क्षमता,
            UError_संहिता* err)
{
    यदि (localeID == शून्यसूचकः) {
        localeID = uloc_getDefault();
    }

    प्रत्यागम ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Terminated_Chars(
        country, country_क्षमता,
        [&](ByteSink& sink, UError_संहिता& स्थितिः) {
            ulocimp_getSubtags(
                    localeID,
                    शून्यसूचकः,
                    शून्यसूचकः,
                    &sink,
                    शून्यसूचकः,
                    शून्यसूचकः,
                    स्थितिः);
        },
        *err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getVariant(ध्रुव वर्ण* localeID,
                वर्ण* variant,
                int32_t variant_क्षमता,
                UError_संहिता* err)
{
    यदि (localeID == शून्यसूचकः) {
        localeID = uloc_getDefault();
    }

    प्रत्यागम ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Terminated_Chars(
        variant, variant_क्षमता,
        [&](ByteSink& sink, UError_संहिता& स्थितिः) {
            ulocimp_getSubtags(
                    localeID,
                    शून्यसूचकः,
                    शून्यसूचकः,
                    शून्यसूचकः,
                    &sink,
                    शून्यसूचकः,
                    स्थितिः);
        },
        *err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getName(ध्रुव वर्ण* localeID,
             वर्ण* name,
             int32_t name_क्षमता,
             UError_संहिता* err)
{
    यदि (localeID == शून्यसूचकः) {
        localeID = uloc_getDefault();
    }
    प्रत्यागम ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Terminated_Chars(
        name, name_क्षमता,
        [&](ByteSink& sink, UError_संहिता& स्थितिः) {
            ulocimp_getName(localeID, sink, स्थितिः);
        },
        *err);
}

U_EXPORT CharString
ulocimp_getName(std::string_view localeID,
                UError_संहिता& err)
{
    प्रत्यागम ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Char_String(
        [&](ByteSink& sink, UError_संहिता& स्थितिः) {
            ulocimp_getName(localeID, sink, स्थितिः);
        },
        err);
}

U_EXPORT शून्य
ulocimp_getName(std::string_view localeID,
                ByteSink& sink,
                UError_संहिता& err)
{
    _canonicalize(localeID, sink, 0, err);
}

U_CAPI int32_t  U_EXPORT2
uloc_get_आधारः_Name(ध्रुव वर्ण* localeID,
                 वर्ण* name,
                 int32_t name_क्षमता,
                 UError_संहिता* err)
{
    यदि (localeID == शून्यसूचकः) {
        localeID = uloc_getDefault();
    }
    प्रत्यागम ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Terminated_Chars(
        name, name_क्षमता,
        [&](ByteSink& sink, UError_संहिता& स्थितिः) {
            ulocimp_get_आधारः_Name(localeID, sink, स्थितिः);
        },
        *err);
}

U_EXPORT CharString
ulocimp_get_आधारः_Name(std::string_view localeID,
                    UError_संहिता& err)
{
    प्रत्यागम ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Char_String(
        [&](ByteSink& sink, UError_संहिता& स्थितिः) {
            ulocimp_get_आधारः_Name(localeID, sink, स्थितिः);
        },
        err);
}

U_EXPORT शून्य
ulocimp_get_आधारः_Name(std::string_view localeID,
                    ByteSink& sink,
                    UError_संहिता& err)
{
    _canonicalize(localeID, sink, _ULOC_STRIP_KEYWORDS, err);
}

U_CAPI int32_t  U_EXPORT2
uloc_canonicalize(ध्रुव वर्ण* localeID,
                  वर्ण* name,
                  int32_t name_क्षमता,
                  UError_संहिता* err)
{
    यदि (localeID == शून्यसूचकः) {
        localeID = uloc_getDefault();
    }
    प्रत्यागम ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Terminated_Chars(
        name, name_क्षमता,
        [&](ByteSink& sink, UError_संहिता& स्थितिः) {
            ulocimp_canonicalize(localeID, sink, स्थितिः);
        },
        *err);
}

U_EXPORT CharString
ulocimp_canonicalize(std::string_view localeID,
                     UError_संहिता& err)
{
    प्रत्यागम ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Char_String(
        [&](ByteSink& sink, UError_संहिता& स्थितिः) {
            ulocimp_canonicalize(localeID, sink, स्थितिः);
        },
        err);
}

U_EXPORT शून्य
ulocimp_canonicalize(std::string_view localeID,
                     ByteSink& sink,
                     UError_संहिता& err)
{
    _canonicalize(localeID, sink, _ULOC_CANONICALIZE, err);
}

U_CAPI ध्रुव वर्ण*  U_EXPORT2
uloc_getISO3Language(ध्रुव वर्ण* localeID)
{
    UError_संहिता err = U_ZERO_ERROR;

    यदि (localeID == शून्यसूचकः)
    {
        localeID = uloc_getDefault();
    }
    CharString lang = ulocimp_getLanguage(localeID, err);
    यदि (U_FAILURE(err))
        प्रत्यागम "";
    std::optional<int16_t> विस्थापनम् = find_सूचकांक_2(LANGUAGES, lang.दत्त());
    प्रत्यागम विस्थापनम्.has_value() ? LANGUAGES_3[*विस्थापनम्] : "";
}

U_CAPI ध्रुव वर्ण*  U_EXPORT2
uloc_getISO3Country(ध्रुव वर्ण* localeID)
{
    UError_संहिता err = U_ZERO_ERROR;

    यदि (localeID == शून्यसूचकः)
    {
        localeID = uloc_getDefault();
    }
    CharString cntry = ulocimp_getRegion(localeID, err);
    यदि (U_FAILURE(err))
        प्रत्यागम "";
    std::optional<int16_t> विस्थापनम् = find_सूचकांक_2(COUNTRIES, cntry.दत्त());
    प्रत्यागम विस्थापनम्.has_value() ? COUNTRIES_3[*विस्थापनम्] : "";
}

U_CAPI uint32_t  U_EXPORT2
uloc_getLCID(ध्रुव वर्ण* localeID)
{
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    uint32_t   lcid = 0;

    /* Check for incomplete id. */
    यदि (!localeID || uprv_strlen(localeID) < 2) {
        प्रत्यागम 0;
    }

    // First, attempt Windows platform lookup if available, but fall
    // through to catch any special cases (ICU vs Windows name differences).
    lcid = uprv_convert_गन्तव्यम्_LCIDPlatform(localeID, &स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम 0;
    }
    यदि (lcid > 0) {
        // Windows found an LCID, return that
        प्रत्यागम lcid;
    }

    CharString langID = ulocimp_getLanguage(localeID, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम 0;
    }

    यदि (uprv_strchr(localeID, '@')) {
        // uprv_convertToLCID does not support keywords other than collation.
        // Remove all keywords except collation.
        CharString collVal = ulocimp_get_Keyword_मानम्(localeID, "collation", स्थितिः);
        यदि (U_SUCCESS(स्थितिः) && !collVal.is_रिक्तानि()) {
            CharString tmpLocaleID = ulocimp_get_आधारः_Name(localeID, स्थितिः);
            ulocimp_समुच्चयः_Keyword_मानम्("collation", collVal.गन्तव्यम्_String_Piece(), tmpLocaleID, स्थितिः);
            यदि (U_SUCCESS(स्थितिः)) {
                प्रत्यागम uprv_convert_गन्तव्यम्_LCID(langID.दत्त(), tmpLocaleID.दत्त(), &स्थितिः);
            }
        }

        // fall through - all keywords are simply ignored
        स्थितिः = U_ZERO_ERROR;
    }

    प्रत्यागम uprv_convert_गन्तव्यम्_LCID(langID.दत्त(), localeID, &स्थितिः);
}

U_CAPI int32_t U_EXPORT2
uloc_getLocaleForLCID(uint32_t hostid, वर्ण *locale, int32_t locale_क्षमता,
                UError_संहिता *स्थितिः)
{
    प्रत्यागम uprv_convert_गन्तव्यम्_Posix(hostid, locale, locale_क्षमता, स्थितिः);
}

/* ### Default locale **************************************************/

U_CAPI ध्रुव वर्ण*  U_EXPORT2
uloc_getDefault()
{
    प्रत्यागम locale_get_default();
}

U_CAPI शून्य  U_EXPORT2
uloc_समुच्चयः_Default(ध्रुव वर्ण*   newDefaultLocale,
             UError_संहिता* err)
{
    यदि (U_FAILURE(*err))
        प्रत्यागम;
    /* the error code isn't currently used for anything by this function*/

    /* propagate change to C++ */
    locale_समुच्चयः_default(newDefaultLocale);
}

/**
 * Returns a list of all 2-letter language codes defined in ISO 639.  This is a pointer
 * to an array of pointers to arrays of char.  All of these pointers are owned
 * by ICU-- do not delete them, and do not write through them.  The array is
 * terminated with a null pointer.
 */
U_CAPI ध्रुव वर्ण* ध्रुव*  U_EXPORT2
uloc_getISOLanguages()
{
    प्रत्यागम LANGUAGES;
}

/**
 * Returns a list of all 2-letter country codes defined in ISO 639.  This is a
 * pointer to an array of pointers to arrays of char.  All of these pointers are
 * owned by ICU-- do not delete them, and do not write through them.  The array is
 * terminated with a null pointer.
 */
U_CAPI ध्रुव वर्ण* ध्रुव*  U_EXPORT2
uloc_getISOCountries()
{
    प्रत्यागम COUNTRIES;
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
uloc_गन्तव्यम्_Unicode_Locale_कुञ्चिका(ध्रुव वर्ण* keyword U_LIFETIME_BOUND)
{
    यदि (keyword == शून्यसूचकः || *keyword == '\0') { प्रत्यागम शून्यसूचकः; }
    std::optional<std::string_view> परिणाम = ulocimp_गन्तव्यम्_Bcp_कुञ्चिका_With_Fallback(keyword);
    प्रत्यागम परिणाम.has_value() ? परिणाम->दत्त() : शून्यसूचकः;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_गन्तव्यम्_Bcp_कुञ्चिका_With_Fallback(std::string_view keyword U_LIFETIME_BOUND)
{
    std::optional<std::string_view> bcp_कुञ्चिका = ulocimp_गन्तव्यम्_Bcp_कुञ्चिका(keyword);
    यदि (!bcp_कुञ्चिका.has_value() &&
        ultag_is_Unicode_Locale_कुञ्चिका(keyword.दत्त(), static_cast<int32_t>(keyword.size()))) {
        // unknown keyword, but syntax is fine..
        प्रत्यागम keyword;
    }
    प्रत्यागम bcp_कुञ्चिका;
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
uloc_गन्तव्यम्_Unicode_Locale_Type(ध्रुव वर्ण* keyword, ध्रुव वर्ण* value U_LIFETIME_BOUND)
{
    यदि (keyword == शून्यसूचकः || *keyword == '\0' ||
        value == शून्यसूचकः || *value == '\0') { प्रत्यागम शून्यसूचकः; }
    std::optional<std::string_view> परिणाम = ulocimp_गन्तव्यम्_Bcp_Type_With_Fallback(keyword, value);
    प्रत्यागम परिणाम.has_value() ? परिणाम->दत्त() : शून्यसूचकः;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_गन्तव्यम्_Bcp_Type_With_Fallback(std::string_view keyword, std::string_view value U_LIFETIME_BOUND)
{
    std::optional<std::string_view> bcpType = ulocimp_गन्तव्यम्_Bcp_Type(keyword, value);
    यदि (!bcpType.has_value() &&
        ultag_isUnicodeLocaleType(value.दत्त(), static_cast<int32_t>(value.size()))) {
        // unknown keyword, but syntax is fine..
        प्रत्यागम value;
    }
    प्रत्यागम bcpType;
}

नामाकाश {

तार्किक
is_Well_Formed_Legacy_कुञ्चिका(std::string_view कुञ्चिका)
{
    प्रत्यागम std::all_of(कुञ्चिका.begin(), कुञ्चिका.अन्तः(), UPRV_ISALPHANUM);
}

तार्किक
isWellFormedLegacyType(std::string_view legacyType)
{
    int32_t alpha_अङ्कः_Len = 0;
    प्रति (वर्ण c : legacyType) {
        यदि (c == '_' || c == '/' || c == '-') {
            यदि (alpha_अङ्कः_Len == 0) {
                प्रत्यागम असत्य;
            }
            alpha_अङ्कः_Len = 0;
        } अन्यथा यदि (UPRV_ISALPHANUM(c)) {
            alpha_अङ्कः_Len++;
        } अन्यथा {
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम alpha_अङ्कः_Len != 0;
}

}  // namespace

U_CAPI ध्रुव वर्ण* U_EXPORT2
uloc_गन्तव्यम्_Legacy_कुञ्चिका(ध्रुव वर्ण* keyword U_LIFETIME_BOUND)
{
    यदि (keyword == शून्यसूचकः || *keyword == '\0') { प्रत्यागम शून्यसूचकः; }
    std::optional<std::string_view> परिणाम = ulocimp_गन्तव्यम्_Legacy_कुञ्चिका_With_Fallback(keyword);
    प्रत्यागम परिणाम.has_value() ? परिणाम->दत्त() : शून्यसूचकः;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_गन्तव्यम्_Legacy_कुञ्चिका_With_Fallback(std::string_view keyword U_LIFETIME_BOUND)
{
    std::optional<std::string_view> legacy_कुञ्चिका_2 = ulocimp_गन्तव्यम्_Legacy_कुञ्चिका(keyword);
    यदि (!legacy_कुञ्चिका_2.has_value() && is_Well_Formed_Legacy_कुञ्चिका(keyword)) {
        // Checks if the specified locale key is well-formed with the legacy locale syntax.
        //
        // Note:
        //  LDML/CLDR provides some definition of keyword syntax in
        //  * http://www.unicode.org/reports/tr35/#Unicode_locale_identifier and
        //  * http://www.unicode.org/reports/tr35/#Old_Locale_Extension_Syntax
        //  Keys can only consist of [0-9a-zA-Z].
        प्रत्यागम keyword;
    }
    प्रत्यागम legacy_कुञ्चिका_2;
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
uloc_गन्तव्यम्_Legacy_Type(ध्रुव वर्ण* keyword, ध्रुव वर्ण* value U_LIFETIME_BOUND)
{
    यदि (keyword == शून्यसूचकः || *keyword == '\0' ||
        value == शून्यसूचकः || *value == '\0') { प्रत्यागम शून्यसूचकः; }
    std::optional<std::string_view> परिणाम = ulocimp_गन्तव्यम्_Legacy_Type_With_Fallback(keyword, value);
    प्रत्यागम परिणाम.has_value() ? परिणाम->दत्त() : शून्यसूचकः;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_गन्तव्यम्_Legacy_Type_With_Fallback(std::string_view keyword, std::string_view value U_LIFETIME_BOUND)
{
    std::optional<std::string_view> legacyType = ulocimp_गन्तव्यम्_Legacy_Type(keyword, value);
    यदि (!legacyType.has_value() && isWellFormedLegacyType(value)) {
        // Checks if the specified locale type is well-formed with the legacy locale syntax.
        //
        // Note:
        //  LDML/CLDR provides some definition of keyword syntax in
        //  * http://www.unicode.org/reports/tr35/#Unicode_locale_identifier and
        //  * http://www.unicode.org/reports/tr35/#Old_Locale_Extension_Syntax
        //  Values (types) can only consist of [0-9a-zA-Z], plus for legacy values
        //  we allow [/_-+] in the middle (e.g. "Etc/GMT+1", "Asia/Tel_Aviv")
        प्रत्यागम value;
    }
    प्रत्यागम legacyType;
}

/*eof*/
