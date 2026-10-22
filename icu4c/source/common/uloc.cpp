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

#అన్తర్భావ <algorithm>
#అన్తర్భావ <optional>
#అన్తర్భావ <string_view>

#అన్తర్భావ "unicode/bytestream.h"
#అన్తర్భావ "unicode/errorcode.h"
#అన్తర్భావ "unicode/stringpiece.h"
#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/uloc.h"

#అన్తర్భావ "bytesinkutil.h"
#అన్తర్భావ "putilimp.h"
#అన్తర్భావ "ustr_imp.h"
#అన్తర్భావ "ulocimp.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "locmap.h"
#అన్తర్భావ "uarrsort.h"
#అన్తర్భావ "uenumimp.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "charstr.h"

U_NAMESPACE_USE

/* ### Declarations **************************************************/

/* Locale stuff from locid.cpp */
U_CFUNC శూన్య locale_సమితి_default(స్థిరరాశి అక్షరం *id);
U_CFUNC స్థిరరాశి అక్షరం *locale_get_default();

నామాకాశ {

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
constexpr స్థిరరాశి అక్షరం* LANGUAGES[] = {
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
శూన్యసూచిక,
    "in",  "iw",  "ji",  "jw",  "mo",  "sh",  "swc", "tl",  /* obsolete language codes */
శూన్యసూచిక
};

constexpr స్థిరరాశి అక్షరం* DEPRECATED_LANGUAGES[]={
    "in", "iw", "ji", "jw", "mo", శూన్యసూచిక, శూన్యసూచిక
};
constexpr స్థిరరాశి అక్షరం* REPLACEMENT_LANGUAGES[]={
    "id", "he", "yi", "jv", "ro", శూన్యసూచిక, శూన్యసూచిక
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
constexpr స్థిరరాశి అక్షరం* LANGUAGES_3[] = {
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
శూన్యసూచిక,
/*  "in",  "iw",  "ji",  "jw",  "mo",  "sh",  "swc", "tl",  */
    "ind", "heb", "yid", "jaw", "mol", "srp", "swc", "tgl",
శూన్యసూచిక
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
constexpr స్థిరరాశి అక్షరం* COUNTRIES[] = {
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
శూన్యసూచిక,
    "AN",  "BU", "CS", "FX", "RO", "SU", "TP", "YD", "YU", "ZR",   /* obsolete country codes */
శూన్యసూచిక
};

constexpr స్థిరరాశి అక్షరం* DEPRECATED_COUNTRIES[] = {
    "AN", "BU", "CS", "DD", "DY", "FX", "HV", "NH", "RH", "SU", "TP", "UK", "VD", "YD", "YU", "ZR", శూన్యసూచిక, శూన్యసూచిక /* deprecated country list */
};
constexpr స్థిరరాశి అక్షరం* REPLACEMENT_COUNTRIES[] = {
/*  "AN", "BU", "CS", "DD", "DY", "FX", "HV", "NH", "RH", "SU", "TP", "UK", "VD", "YD", "YU", "ZR" */
    "CW", "MM", "RS", "DE", "BJ", "FR", "BF", "VU", "ZW", "RU", "TL", "GB", "VN", "YE", "RS", "CD", శూన్యసూచిక, శూన్యసూచిక  /* replacement country codes */
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
constexpr స్థిరరాశి అక్షరం* COUNTRIES_3[] = {
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
శూన్యసూచిక,
/*  "AN",  "BU",  "CS",  "FX",  "RO", "SU",  "TP",  "YD",  "YU",  "ZR" */
    "ANT", "BUR", "SCG", "FXX", "ROM", "SUN", "TMP", "YMD", "YUG", "ZAR",
శూన్యసూచిక
};

రకనిర్వచనం నిర్మాణం CanonicalizationMap {
    స్థిరరాశి అక్షరం *id;          /* input ID */
    స్థిరరాశి అక్షరం *canonicalID; /* canonicalized output ID */
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
int32_t get_అతితక్కువ_Subtag_నిడివి(std::string_view localeID) {
    int32_t localeIDLength = static_cast<int32_t>(localeID.length());
    int32_t length = localeIDLength;
    int32_t tmp_నిడివి = 0;
    int32_t i;
    తార్కిక reset = నిజం;

    కొరకు (i = 0; i < localeIDLength; i++) {
        ఒకవేళ (localeID[i] != '_' && localeID[i] != '-') {
            ఒకవేళ (reset) {
                tmp_నిడివి = 0;
                reset = అబద్ధం;
            }
            tmp_నిడివి++;
        } లేకపోతే {
            ఒకవేళ (tmp_నిడివి != 0 && tmp_నిడివి < length) {
                length = tmp_నిడివి;
            }
            reset = నిజం;
        }
    }

    తిరిగి length;
}
/* Test if the locale id has BCP47 u extension and does not have '@' */
అంతఃవరుస తార్కిక _hasBCP47Extension(std::string_view id) {
    తిరిగి id.find('@') == std::string_view::npos && get_అతితక్కువ_Subtag_నిడివి(id) == 1;
}

/* ### Keywords **************************************************/
అంతఃవరుస తార్కిక UPRV_ISDIGIT(అక్షరం c) { తిరిగి c >= '0' && c <= '9'; }
అంతఃవరుస తార్కిక UPRV_ISALPHANUM(అక్షరం c) { తిరిగి uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }
/* Punctuation/symbols allowed in legacy key values */
అంతఃవరుస తార్కిక UPRV_OK_విలువ_PUNCTUATION(అక్షరం c) { తిరిగి c == '_' || c == '-' || c == '+' || c == '/'; }

}  // namespace

#నిర్వచించు ULOC_KEYWORD_బఫరు_LEN 25
#నిర్వచించు ULOC_MAX_NO_KEYWORDS 25

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
locale_get_Keywords_ప్రారంభం(std::string_view localeID U_LIFETIME_BOUND) {
    ఒకవేళ (size_t pos = localeID.find('@'); pos != std::string_view::npos) {
        తిరిగి localeID.దత్తాంశం() + pos;
    }
#ఒకవేళ (U_CHARSET_FAMILY == U_EBCDIC_FAMILY)
    లేకపోతే {
        /* We do this because the @ sign is variant, and the @ sign used on one
        EBCDIC machine won't be compiled the same way on other EBCDIC based
        machines. */
        స్థిర స్థిరరాశి uint8_t ebcdicSigns[] = { 0x7C, 0x44, 0x66, 0x80, 0xAC, 0xAE, 0xAF, 0xB5, 0xEC, 0xEF, 0x00 };
        స్థిరరాశి uint8_t *char_గమ్యం_కనుగొను = ebcdicSigns;
        అయితే(*char_గమ్యం_కనుగొను) {
            ఒకవేళ (size_t pos = localeID.find(*char_గమ్యం_కనుగొను); pos != std::string_view::npos) {
                తిరిగి localeID.దత్తాంశం() + pos;
            }
            char_గమ్యం_కనుగొను++;
        }
    }
#అయితే_ముగింపు
    తిరిగి శూన్యసూచిక;
}

నామాకాశ {

/**
 * @param keywordName incoming name to be canonicalized
 * @param status return status (keyword too long)
 * @return the keyword name
 */
CharString locale_canonKeywordName(std::string_view keywordName, UError_సంకేతం& స్థితి_2)
{
  ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి {}; }
  CharString ఫలితం;

  కొరకు (అక్షరం c : keywordName) {
    ఒకవేళ (!UPRV_ISALPHANUM(c)) {
      స్థితి_2 = U_ILLEGAL_వాదం_ERROR; /* malformed keyword name */
      తిరిగి {};
    }
    ఫలితం.append(uprv_tolower(c), స్థితి_2);
  }
  ఒకవేళ (ఫలితం.is_ఖాళీలు()) {
    స్థితి_2 = U_ILLEGAL_వాదం_ERROR; /* empty keyword name */
    తిరిగి {};
  }

  తిరిగి ఫలితం;
}

రకనిర్వచనం నిర్మాణం {
    అక్షరం keyword[ULOC_KEYWORD_బఫరు_LEN];
    int32_t keywordLen;
    స్థిరరాశి అక్షరం *విలువ_ప్రారంభం;
    int32_t విలువ_Len;
} KeywordStruct;

int32_t U_CALLCONV
compareKeywordStructs(స్థిరరాశి శూన్య * /*context*/, స్థిరరాశి శూన్య *ఎడమ, స్థిరరాశి శూన్య *కుడి) {
    స్థిరరాశి అక్షరం* ఎడమ_String = static_cast<స్థిరరాశి KeywordStruct*>(ఎడమ)->keyword;
    స్థిరరాశి అక్షరం* కుడి_String = static_cast<స్థిరరాశి KeywordStruct*>(కుడి)->keyword;
    తిరిగి uprv_strcmp(ఎడమ_String, కుడి_String);
}

}  // namespace

U_EXPORT CharString
ulocimp_getKeywords(std::string_view localeID,
                    అక్షరం prev,
                    తార్కిక విలువలు_Too,
                    UError_సంకేతం& స్థితి_2)
{
    తిరిగి ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Char_String(
        [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            ulocimp_getKeywords(localeID,
                                prev,
                                sink,
                                విలువలు_Too,
                                స్థితి_2);
        },
        స్థితి_2);
}

U_EXPORT శూన్య
ulocimp_getKeywords(std::string_view localeID,
                    అక్షరం prev,
                    ByteSink& sink,
                    తార్కిక విలువలు_Too,
                    UError_సంకేతం& స్థితి_2)
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }

    KeywordStruct keywordList[ULOC_MAX_NO_KEYWORDS];

    int32_t maxKeywords = ULOC_MAX_NO_KEYWORDS;
    int32_t అంకె_Keywords = 0;
    size_t equalSign = std::string_view::npos;
    size_t semicolon = std::string_view::npos;
    int32_t i = 0, j, n;

    ఒకవేళ(prev == '@') { /* start of keyword definition */
        /* we will grab pairs, trim spaces, lowercase keywords, sort and return */
        చేయి {
            తార్కిక duplicate = అబద్ధం;
            /* skip leading spaces */
            అయితే (!localeID.empty() && localeID.front() == ' ') {
                localeID.remove_ఉపసర్గ(1);
            }
            ఒకవేళ (localeID.empty()) { /* handle trailing "; " */
                ఆపు;
            }
            ఒకవేళ(అంకె_Keywords == maxKeywords) {
                స్థితి_2 = U_అంతర్గత_కార్యక్రమం_ERROR;
                తిరిగి;
            }
            equalSign = localeID.find('=');
            semicolon = localeID.find(';');
            /* lack of '=' [foo@currency] is illegal */
            /* ';' before '=' [foo@currency;collation=pinyin] is illegal */
            ఒకవేళ (equalSign == std::string_view::npos ||
                (semicolon != std::string_view::npos && semicolon < equalSign)) {
                స్థితి_2 = U_INVALID_FORMAT_ERROR;
                తిరిగి;
            }
            /* zero-length keyword is an error. */
            ఒకవేళ (equalSign == 0) {
                స్థితి_2 = U_INVALID_FORMAT_ERROR;
                తిరిగి;
            }
            /* need to normalize both keyword and keyword name */
            ఒకవేళ (equalSign >= ULOC_KEYWORD_బఫరు_LEN) {
                /* keyword name too long for internal buffer */
                స్థితి_2 = U_అంతర్గత_కార్యక్రమం_ERROR;
                తిరిగి;
            }
            కొరకు (i = 0, n = 0; static_cast<size_t>(i) < equalSign; ++i) {
                ఒకవేళ (localeID[i] != ' ') {
                    keywordList[అంకె_Keywords].keyword[n++] = uprv_tolower(localeID[i]);
                }
            }

            keywordList[అంకె_Keywords].keyword[n] = 0;
            keywordList[అంకె_Keywords].keywordLen = n;
            /* now grab the value part. First we skip the '=' */
            equalSign++;
            /* then we leading spaces */
            అయితే (equalSign < localeID.length() && localeID[equalSign] == ' ') {
                equalSign++;
            }

            /* Premature end or zero-length value */
            ఒకవేళ (equalSign == localeID.length() || equalSign == semicolon) {
                స్థితి_2 = U_INVALID_FORMAT_ERROR;
                తిరిగి;
            }

            keywordList[అంకె_Keywords].విలువ_ప్రారంభం = localeID.దత్తాంశం() + equalSign;

            std::string_view value = localeID;
            ఒకవేళ (semicolon != std::string_view::npos) {
                value.remove_suffix(value.length() - semicolon);
                localeID.remove_ఉపసర్గ(semicolon + 1);
            } లేకపోతే {
                localeID = {};
            }
            value.remove_ఉపసర్గ(equalSign);
            ఒకవేళ (size_t చివరి = value.find_last_not_of(' '); చివరి != std::string_view::npos) {
                value.remove_suffix(value.length() - చివరి - 1);
            }
            keywordList[అంకె_Keywords].విలువ_Len = static_cast<int32_t>(value.length());

            /* If this is a duplicate keyword, then ignore it */
            కొరకు (j=0; j<అంకె_Keywords; ++j) {
                ఒకవేళ (uprv_strcmp(keywordList[j].keyword, keywordList[అంకె_Keywords].keyword) == 0) {
                    duplicate = నిజం;
                    ఆపు;
                }
            }
            ఒకవేళ (!duplicate) {
                ++అంకె_Keywords;
            }
        } అయితే (!localeID.empty());

        /* now we have a list of keywords */
        /* we need to sort it */
        uprv_క్రమీకరించు_Array(keywordList, అంకె_Keywords, పరిమాణం(KeywordStruct), compareKeywordStructs, శూన్యసూచిక, అబద్ధం, &స్థితి_2);

        /* Now construct the keyword part */
        కొరకు(i = 0; i < అంకె_Keywords; i++) {
            sink.Append(keywordList[i].keyword, keywordList[i].keywordLen);
            ఒకవేళ(విలువలు_Too) {
                sink.Append("=", 1);
                sink.Append(keywordList[i].విలువ_ప్రారంభం, keywordList[i].విలువ_Len);
                ఒకవేళ(i < అంకె_Keywords - 1) {
                    sink.Append(";", 1);
                }
            } లేకపోతే {
                sink.Append("\0", 1);
            }
        }
    }
}

U_CAPI int32_t U_EXPORT2
uloc_get_Keyword_విలువ(స్థిరరాశి అక్షరం* localeID,
                     స్థిరరాశి అక్షరం* keywordName,
                     అక్షరం* బఫరు, int32_t బఫరు_సామర్థ్యం,
                     UError_సంకేతం* స్థితి_2)
{
    ఒకవేళ (U_FAILURE(*స్థితి_2)) { తిరిగి 0; }
    ఒకవేళ (keywordName == శూన్యసూచిక || *keywordName == '\0') {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    తిరిగి ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Terminated_Chars(
        బఫరు, బఫరు_సామర్థ్యం,
        [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            ulocimp_get_Keyword_విలువ(localeID, keywordName, sink, స్థితి_2);
        },
        *స్థితి_2);
}

U_EXPORT CharString
ulocimp_get_Keyword_విలువ(స్థిరరాశి అక్షరం* localeID,
                        std::string_view keywordName,
                        UError_సంకేతం& స్థితి_2)
{
    తిరిగి ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Char_String(
        [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            ulocimp_get_Keyword_విలువ(localeID, keywordName, sink, స్థితి_2);
        },
        స్థితి_2);
}

U_EXPORT శూన్య
ulocimp_get_Keyword_విలువ(స్థిరరాశి అక్షరం* localeID,
                        std::string_view keywordName,
                        icu::ByteSink& sink,
                        UError_సంకేతం& స్థితి_2)
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }

    ఒకవేళ (localeID == శూన్యసూచిక || keywordName.empty()) {
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    స్థిరరాశి అక్షరం* ప్రారంభం_శోధన_ఇక్కడ = శూన్యసూచిక;
    స్థిరరాశి అక్షరం* తదుపరి_Separator = శూన్యసూచిక;

    CharString తాత్కాలిక_బఫరు;
    స్థిరరాశి అక్షరం* tmpLocaleID;

    CharString canonKeywordName = locale_canonKeywordName(keywordName, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
      తిరిగి;
    }

    ఒకవేళ (localeID != శూన్యసూచిక && _hasBCP47Extension(localeID)) {
        తాత్కాలిక_బఫరు = ulocimp_for_Language_ట్యాగ్(localeID, -1, శూన్యసూచిక, స్థితి_2);
        tmpLocaleID = U_SUCCESS(స్థితి_2) && !తాత్కాలిక_బఫరు.is_ఖాళీలు() ? తాత్కాలిక_బఫరు.దత్తాంశం() : localeID;
    } లేకపోతే {
        tmpLocaleID=localeID;
    }

    ప్రారంభం_శోధన_ఇక్కడ = locale_get_Keywords_ప్రారంభం(tmpLocaleID);
    ఒకవేళ(ప్రారంభం_శోధన_ఇక్కడ == శూన్యసూచిక) {
        /* no keywords, return at once */
        తిరిగి;
    }

    /* find the first keyword */
    అయితే(ప్రారంభం_శోధన_ఇక్కడ) {
        స్థిరరాశి అక్షరం* కీలకం_విలువ_తోక;

        ప్రారంభం_శోధన_ఇక్కడ++; /* skip @ or ; */
        తదుపరి_Separator = uprv_strchr(ప్రారంభం_శోధన_ఇక్కడ, '=');
        ఒకవేళ(!తదుపరి_Separator) {
            స్థితి_2 = U_ILLEGAL_వాదం_ERROR; /* key must have =value */
            తిరిగి;
        }
        /* strip leading & trailing spaces (TC decided to tolerate these) */
        అయితే(*ప్రారంభం_శోధన_ఇక్కడ == ' ') {
            ప్రారంభం_శోధన_ఇక్కడ++;
        }
        కీలకం_విలువ_తోక = తదుపరి_Separator;
        అయితే (కీలకం_విలువ_తోక > ప్రారంభం_శోధన_ఇక్కడ && *(కీలకం_విలువ_తోక-1) == ' ') {
            కీలకం_విలువ_తోక--;
        }
        /* now keyValueTail points to first char after the keyName */
        /* copy & normalize keyName from locale */
        ఒకవేళ (ప్రారంభం_శోధన_ఇక్కడ == కీలకం_విలువ_తోక) {
            స్థితి_2 = U_ILLEGAL_వాదం_ERROR; /* empty keyword name in passed-in locale */
            తిరిగి;
        }
        CharString localeKeywordName;
        అయితే (ప్రారంభం_శోధన_ఇక్కడ < కీలకం_విలువ_తోక) {
          ఒకవేళ (!UPRV_ISALPHANUM(*ప్రారంభం_శోధన_ఇక్కడ)) {
            స్థితి_2 = U_ILLEGAL_వాదం_ERROR; /* malformed keyword name */
            తిరిగి;
          }
          localeKeywordName.append(uprv_tolower(*ప్రారంభం_శోధన_ఇక్కడ++), స్థితి_2);
        }
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తిరిగి;
        }

        ప్రారంభం_శోధన_ఇక్కడ = uprv_strchr(తదుపరి_Separator, ';');

        ఒకవేళ (canonKeywordName == localeKeywordName) {
             /* current entry matches the keyword. */
           తదుపరి_Separator++; /* skip '=' */
            /* First strip leading & trailing spaces (TC decided to tolerate these) */
            అయితే(*తదుపరి_Separator == ' ') {
              తదుపరి_Separator++;
            }
            కీలకం_విలువ_తోక = (ప్రారంభం_శోధన_ఇక్కడ)? ప్రారంభం_శోధన_ఇక్కడ: తదుపరి_Separator + uprv_strlen(తదుపరి_Separator);
            అయితే(కీలకం_విలువ_తోక > తదుపరి_Separator && *(కీలకం_విలువ_తోక-1) == ' ') {
              కీలకం_విలువ_తోక--;
            }
            /* Now copy the value, but check well-formedness */
            ఒకవేళ (తదుపరి_Separator == కీలకం_విలువ_తోక) {
              స్థితి_2 = U_ILLEGAL_వాదం_ERROR; /* empty key value name in passed-in locale */
              తిరిగి;
            }
            అయితే (తదుపరి_Separator < కీలకం_విలువ_తోక) {
              ఒకవేళ (!UPRV_ISALPHANUM(*తదుపరి_Separator) && !UPRV_OK_విలువ_PUNCTUATION(*తదుపరి_Separator)) {
                స్థితి_2 = U_ILLEGAL_వాదం_ERROR; /* malformed key value */
                తిరిగి;
              }
              /* Should we lowercase value to return here? Tests expect as-is. */
              sink.Append(తదుపరి_Separator++, 1);
            }
            తిరిగి;
        }
    }
}

U_CAPI int32_t U_EXPORT2
uloc_సమితి_Keyword_విలువ(స్థిరరాశి అక్షరం* keywordName,
                     స్థిరరాశి అక్షరం* keyword_విలువ,
                     అక్షరం* బఫరు, int32_t బఫరు_సామర్థ్యం,
                     UError_సంకేతం* స్థితి_2)
{
    ఒకవేళ (U_FAILURE(*స్థితి_2)) { తిరిగి 0; }

    ఒకవేళ (keywordName == శూన్యసూచిక || *keywordName == 0) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    ఒకవేళ (బఫరు_సామర్థ్యం <= 1) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    int32_t bufLen = (int32_t)uprv_strlen(బఫరు);
    ఒకవేళ(బఫరు_సామర్థ్యం<bufLen) {
        /* The capacity is less than the length?! Is this NUL terminated? */
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    అక్షరం* keywords = const_cast<అక్షరం*>(
        locale_get_Keywords_ప్రారంభం({బఫరు, static_cast<std::string_view::size_type>(bufLen)}));
    int32_t పునాది_Len = keywords == శూన్యసూచిక ? bufLen : keywords - బఫరు;
    // Remove -1 from the capacity so that this function can guarantee NUL termination.
    CheckedArrayByteSink sink(keywords == శూన్యసూచిక ? బఫరు + bufLen : keywords,
                              బఫరు_సామర్థ్యం - పునాది_Len - 1);
    int32_t reslen = ulocimp_సమితి_Keyword_విలువ(
        keywords == శూన్యసూచిక ? std::string_view() : keywords,
        keywordName,
        keyword_విలువ == శూన్యసూచిక ? std::string_view() : keyword_విలువ,
        sink,
        *స్థితి_2);

    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి *స్థితి_2 == U_బఫరు_అధికప్రవాహం_ERROR ? reslen + పునాది_Len : 0;
    }

    // See the documentation for this function, it's guaranteed to never
    // overflow the buffer but instead abort with BUFFER_OVERFLOW_ERROR.
    // In this case, nothing has been written to the sink, so it cannot have Overflowed().
    U_ASSERT(!sink.Overflowed());
    U_ASSERT(reslen >= 0);
    తిరిగి u_terminateChars(బఫరు, బఫరు_సామర్థ్యం, reslen + పునాది_Len, స్థితి_2);
}

U_EXPORT శూన్య
ulocimp_సమితి_Keyword_విలువ(std::string_view keywordName,
                        std::string_view keyword_విలువ,
                        CharString& localeID,
                        UError_సంకేతం& స్థితి_2)
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }
    std::string_view keywords;
    ఒకవేళ (స్థిరరాశి అక్షరం* ప్రారంభం = locale_get_Keywords_ప్రారంభం(localeID.గమ్యం_String_Piece()); ప్రారంభం != శూన్యసూచిక) {
        // This is safe because CharString::truncate() doesn't actually erase any
        // data, but simply sets the position for where new data will be written.
        int32_t size = ప్రారంభం - localeID.దత్తాంశం();
        keywords = localeID.గమ్యం_String_Piece();
        keywords.remove_ఉపసర్గ(size);
        localeID.truncate(size);
    }
    CharStringByteSink sink(&localeID);
    ulocimp_సమితి_Keyword_విలువ(keywords, keywordName, keyword_విలువ, sink, స్థితి_2);
}

U_EXPORT int32_t
ulocimp_సమితి_Keyword_విలువ(std::string_view keywords,
                        std::string_view keywordName,
                        std::string_view keyword_విలువ,
                        ByteSink& sink,
                        UError_సంకేతం& స్థితి_2)
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి 0; }

    /* TODO: sorting. removal. */
    int32_t needLen = 0;
    int32_t rc;
    CharString updated_కీలకాలు_And_విలువలు;
    తార్కిక handled_ఆదానం_కీలకం_And_విలువ = అబద్ధం;
    అక్షరం కీలకం_విలువ_ఉపసర్గ = '@';

    ఒకవేళ (స్థితి_2 == U_STRING_NOT_TERMINATED_WARNING) {
        స్థితి_2 = U_ZERO_ERROR;
    }
    ఒకవేళ (keywordName.empty()) {
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    CharString canonKeywordName = locale_canonKeywordName(keywordName, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి 0;
    }

    CharString canon_Keyword_విలువ;
    కొరకు (అక్షరం c : keyword_విలువ) {
        ఒకవేళ (!UPRV_ISALPHANUM(c) && !UPRV_OK_విలువ_PUNCTUATION(c)) {
            స్థితి_2 = U_ILLEGAL_వాదం_ERROR; /* malformed key value */
            తిరిగి 0;
        }
        /* Should we force lowercase in value to set? */
        canon_Keyword_విలువ.append(c, స్థితి_2);
    }
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి 0;
    }

    ఒకవేళ (keywords.size() <= 1) {
        ఒకవేళ (canon_Keyword_విలువ.is_ఖాళీలు()) { /* no keywords = nothing to remove */
            U_ASSERT(స్థితి_2 != U_STRING_NOT_TERMINATED_WARNING);
            తిరిగి 0;
        }

        needLen = 1 + canonKeywordName.length() + 1 + canon_Keyword_విలువ.length();
        int32_t capacity = 0;
        అక్షరం* బఫరు = sink.Get_Append_బఫరు(
                needLen, needLen, శూన్యసూచిక, needLen, &capacity);
        ఒకవేళ (capacity < needLen || బఫరు == శూన్యసూచిక) {
            స్థితి_2 = U_బఫరు_అధికప్రవాహం_ERROR;
            తిరిగి needLen; /* no change */
        }
        అక్షరం* it = బఫరు;

        *it++ = '@';
        uprv_memcpy(it, canonKeywordName.దత్తాంశం(), canonKeywordName.length());
        it += canonKeywordName.length();
        *it++ = '=';
        uprv_memcpy(it, canon_Keyword_విలువ.దత్తాంశం(), canon_Keyword_విలువ.length());
        sink.Append(బఫరు, needLen);
        U_ASSERT(స్థితి_2 != U_STRING_NOT_TERMINATED_WARNING);
        తిరిగి needLen;
    } /* end shortcut - no @ */

    /* search for keyword */
    కొరకు (size_t keyword_ప్రారంభం = 0; keyword_ప్రారంభం != std::string_view::npos;) {
        keyword_ప్రారంభం++; /* skip @ or ; */
        size_t తదుపరి_Equalsign = keywords.find('=', keyword_ప్రారంభం);
        ఒకవేళ (తదుపరి_Equalsign == std::string_view::npos) {
            స్థితి_2 = U_ILLEGAL_వాదం_ERROR; /* key must have =value */
            తిరిగి 0;
        }
        /* strip leading & trailing spaces (TC decided to tolerate these) */
        అయితే (keyword_ప్రారంభం < keywords.size() && keywords[keyword_ప్రారంభం] == ' ') {
            keyword_ప్రారంభం++;
        }
        size_t కీలకం_విలువ_తోక = తదుపరి_Equalsign;
        అయితే (కీలకం_విలువ_తోక > keyword_ప్రారంభం && keywords[కీలకం_విలువ_తోక - 1] == ' ') {
            కీలకం_విలువ_తోక--;
        }
        /* now keyValueTail points to first char after the keyName */
        /* copy & normalize keyName from locale */
        ఒకవేళ (keyword_ప్రారంభం == కీలకం_విలువ_తోక) {
            స్థితి_2 = U_ILLEGAL_వాదం_ERROR; /* empty keyword name in passed-in locale */
            తిరిగి 0;
        }
        CharString localeKeywordName;
        అయితే (keyword_ప్రారంభం < కీలకం_విలువ_తోక) {
            ఒకవేళ (!UPRV_ISALPHANUM(keywords[keyword_ప్రారంభం])) {
                స్థితి_2 = U_ILLEGAL_వాదం_ERROR; /* malformed keyword name */
                తిరిగి 0;
            }
            localeKeywordName.append(uprv_tolower(keywords[keyword_ప్రారంభం++]), స్థితి_2);
        }
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తిరిగి 0;
        }

        size_t తదుపరి_Separator = keywords.find(';', తదుపరి_Equalsign);

        /* start processing the value part */
        తదుపరి_Equalsign++; /* skip '=' */
        /* First strip leading & trailing spaces (TC decided to tolerate these) */
        అయితే (తదుపరి_Equalsign < keywords.size() && keywords[తదుపరి_Equalsign] == ' ') {
            తదుపరి_Equalsign++;
        }
        కీలకం_విలువ_తోక = తదుపరి_Separator == std::string_view::npos ? keywords.size() : తదుపరి_Separator;
        అయితే (కీలకం_విలువ_తోక > తదుపరి_Equalsign && keywords[కీలకం_విలువ_తోక - 1] == ' ') {
            కీలకం_విలువ_తోక--;
        }
        ఒకవేళ (తదుపరి_Equalsign == కీలకం_విలువ_తోక) {
            స్థితి_2 = U_ILLEGAL_వాదం_ERROR; /* empty key value in passed-in locale */
            తిరిగి 0;
        }

        rc = uprv_strcmp(canonKeywordName.దత్తాంశం(), localeKeywordName.దత్తాంశం());
        ఒకవేళ(rc == 0) {
            /* Current entry matches the input keyword. Update the entry */
            ఒకవేళ (!canon_Keyword_విలువ.is_ఖాళీలు()) { /* updating a value */
                updated_కీలకాలు_And_విలువలు.append(కీలకం_విలువ_ఉపసర్గ, స్థితి_2);
                కీలకం_విలువ_ఉపసర్గ = ';'; /* for any subsequent key-value pair */
                updated_కీలకాలు_And_విలువలు.append(canonKeywordName, స్థితి_2);
                updated_కీలకాలు_And_విలువలు.append('=', స్థితి_2);
                updated_కీలకాలు_And_విలువలు.append(canon_Keyword_విలువ, స్థితి_2);
            } /* else removing this entry, don't emit anything */
            handled_ఆదానం_కీలకం_And_విలువ = నిజం;
        } లేకపోతే {
           /* input keyword sorts earlier than current entry, add before current entry */
            ఒకవేళ (rc < 0 && !canon_Keyword_విలువ.is_ఖాళీలు() && !handled_ఆదానం_కీలకం_And_విలువ) {
                /* insert new entry at this location */
                updated_కీలకాలు_And_విలువలు.append(కీలకం_విలువ_ఉపసర్గ, స్థితి_2);
                కీలకం_విలువ_ఉపసర్గ = ';'; /* for any subsequent key-value pair */
                updated_కీలకాలు_And_విలువలు.append(canonKeywordName, స్థితి_2);
                updated_కీలకాలు_And_విలువలు.append('=', స్థితి_2);
                updated_కీలకాలు_And_విలువలు.append(canon_Keyword_విలువ, స్థితి_2);
                handled_ఆదానం_కీలకం_And_విలువ = నిజం;
            }
            /* copy the current entry */
            updated_కీలకాలు_And_విలువలు.append(కీలకం_విలువ_ఉపసర్గ, స్థితి_2);
            కీలకం_విలువ_ఉపసర్గ = ';'; /* for any subsequent key-value pair */
            updated_కీలకాలు_And_విలువలు.append(localeKeywordName, స్థితి_2);
            updated_కీలకాలు_And_విలువలు.append('=', స్థితి_2);
            updated_కీలకాలు_And_విలువలు.append(keywords.దత్తాంశం() + తదుపరి_Equalsign,
                                        static_cast<int32_t>(కీలకం_విలువ_తోక - తదుపరి_Equalsign), స్థితి_2);
        }
        ఒకవేళ (తదుపరి_Separator == std::string_view::npos && !canon_Keyword_విలువ.is_ఖాళీలు() && !handled_ఆదానం_కీలకం_And_విలువ) {
            /* append new entry at the end, it sorts later than existing entries */
            updated_కీలకాలు_And_విలువలు.append(కీలకం_విలువ_ఉపసర్గ, స్థితి_2);
            /* skip keyValuePrefix update, no subsequent key-value pair */
            updated_కీలకాలు_And_విలువలు.append(canonKeywordName, స్థితి_2);
            updated_కీలకాలు_And_విలువలు.append('=', స్థితి_2);
            updated_కీలకాలు_And_విలువలు.append(canon_Keyword_విలువ, స్థితి_2);
            handled_ఆదానం_కీలకం_And_విలువ = నిజం;
        }
        keyword_ప్రారంభం = తదుపరి_Separator;
    } /* end loop searching */

    /* Any error from updatedKeysAndValues.append above would be internal and not due to
     * problems with the passed-in locale. So if we did encounter problems with the
     * passed-in locale above, those errors took precedence and overrode any error
     * status from updatedKeysAndValues.append, and also caused a return of 0. If there
     * are errors here they are from updatedKeysAndValues.append; they do cause an
     * error return but the passed-in locale is unmodified and the original bufLen is
     * returned.
     */
    ఒకవేళ (!handled_ఆదానం_కీలకం_And_విలువ || U_FAILURE(స్థితి_2)) {
        /* if input key/value specified removal of a keyword not present in locale, or
         * there was an error in CharString.append, leave original locale alone. */
        U_ASSERT(స్థితి_2 != U_STRING_NOT_TERMINATED_WARNING);
        // The sink is expected to be a buffer which already contains the full
        // locale string, so when it isn't going to be modified there's no need
        // to actually write any data to it, as the data is already there. Only
        // the first character needs to be overwritten (changing '\0' to '@').
        needLen = static_cast<int32_t>(keywords.size());
        int32_t capacity = 0;
        అక్షరం* బఫరు = sink.Get_Append_బఫరు(
                needLen, needLen, శూన్యసూచిక, needLen, &capacity);
        ఒకవేళ (capacity < needLen || బఫరు == శూన్యసూచిక) {
            స్థితి_2 = U_బఫరు_అధికప్రవాహం_ERROR;
        } లేకపోతే {
            *బఫరు = '@';
            sink.Append(బఫరు, needLen);
        }
        తిరిగి needLen;
    }

    needLen = updated_కీలకాలు_And_విలువలు.length();
    // Check to see can we fit the updatedKeysAndValues, if not, return
    // U_BUFFER_OVERFLOW_ERROR without copy updatedKeysAndValues into it.
    // We do this because this API function does not behave like most others:
    // It promises never to set a U_STRING_NOT_TERMINATED_WARNING.
    // When the contents fits but without the terminating NUL, in this case we need to not change
    // the buffer contents and return with a buffer overflow error.
    ఒకవేళ (needLen > 0) {
        int32_t capacity = 0;
        అక్షరం* బఫరు = sink.Get_Append_బఫరు(
                needLen, needLen, శూన్యసూచిక, needLen, &capacity);
        ఒకవేళ (capacity < needLen || బఫరు == శూన్యసూచిక) {
            స్థితి_2 = U_బఫరు_అధికప్రవాహం_ERROR;
            తిరిగి needLen;
        }
        uprv_memcpy(బఫరు, updated_కీలకాలు_And_విలువలు.దత్తాంశం(), needLen);
        sink.Append(బఫరు, needLen);
    }
    U_ASSERT(స్థితి_2 != U_STRING_NOT_TERMINATED_WARNING);
    తిరిగి needLen;
}

/* ### ID parsing implementation **************************************************/

నామాకాశ {

అంతఃవరుస తార్కిక is_ఉపసర్గ_Letter(అక్షరం a) { తిరిగి a == 'x' || a == 'X' || a == 'i' || a == 'I'; }

/*returns true if one of the special prefixes is here (s=string)
  'x-' or 'i-' */
అంతఃవరుస తార్కిక _isIDPrefix(std::string_view s) {
    తిరిగి s.size() >= 2 && is_ఉపసర్గ_Letter(s[0]) && _isIDSeparator(s[1]);
}

/* Dot terminates it because of POSIX form  where dot precedes the codepage
 * except for variant
 */
అంతఃవరుస తార్కిక _isTerminator(అక్షరం a) { తిరిగి a == '.' || a == '@'; }

అంతఃవరుస తార్కిక _isBCP47Extension(std::string_view p) {
    తిరిగి p.size() >= 3 &&
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
std::optional<int16_t> కనుగొను_సూచీ_2(స్థిరరాశి అక్షరం* స్థిరరాశి* list, స్థిరరాశి అక్షరం* కీలకం)
{
    స్థిరరాశి అక్షరం* స్థిరరాశి* anchor = list;
    int32_t pass = 0;

    /* Make two passes through two nullptr-terminated arrays at 'list' */
    అయితే (pass++ < 2) {
        అయితే (*list) {
            ఒకవేళ (uprv_strcmp(కీలకం, *list) == 0) {
                తిరిగి static_cast<int16_t>(list - anchor);
            }
            list++;
        }
        ++list;     /* skip final nullptr *CWB*/
    }
    తిరిగి std::nullopt;
}

}  // namespace

U_CFUNC స్థిరరాశి అక్షరం*
uloc_getCurrentCountryID(స్థిరరాశి అక్షరం* oldID U_LIFETIME_BOUND) {
    std::optional<int16_t> స్థానభ్రంశం = కనుగొను_సూచీ_2(DEPRECATED_COUNTRIES, oldID);
    తిరిగి స్థానభ్రంశం.has_value() ? REPLACEMENT_COUNTRIES[*స్థానభ్రంశం] : oldID;
}
U_CFUNC స్థిరరాశి అక్షరం*
uloc_getCurrentLanguageID(స్థిరరాశి అక్షరం* oldID U_LIFETIME_BOUND) {
    std::optional<int16_t> స్థానభ్రంశం = కనుగొను_సూచీ_2(DEPRECATED_LANGUAGES, oldID);
    తిరిగి స్థానభ్రంశం.has_value() ? REPLACEMENT_LANGUAGES[*స్థానభ్రంశం] : oldID;
}

నామాకాశ {

/*
 * the internal functions _getLanguage(), _getScript(), _getRegion(), _getVariant()
 * avoid duplicating code to handle the earlier locale ID pieces
 * in the functions for the later ones by
 * setting the *pEnd pointer to where they stopped parsing
 *
 * TODO try to use this in Locale
 */

size_t _getLanguage(std::string_view localeID, ByteSink* sink, UError_సంకేతం& స్థితి_2) {
    size_t skip = 0;
    ఒకవేళ (localeID.size() == 4 && uprv_strnicmp(localeID.దత్తాంశం(), "root", 4) == 0) {
        skip = 4;
        localeID.remove_ఉపసర్గ(skip);
    } లేకపోతే ఒకవేళ (localeID.size() >= 3 && uprv_strnicmp(localeID.దత్తాంశం(), "und", 3) == 0 &&
               (localeID.size() == 3 ||
                localeID[3] == '-' ||
                localeID[3] == '_' ||
                localeID[3] == '@')) {
        skip = 3;
        localeID.remove_ఉపసర్గ(skip);
    }

    constexpr int32_t MAXLEN = ULOC_LANG_సామర్థ్యం - 1;  // Minus NUL.

    /* if it starts with i- or x- then copy that prefix */
    size_t len = _isIDPrefix(localeID) ? 2 : 0;
    అయితే (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len])) {
        ఒకవేళ (len == MAXLEN) {
            స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
            తిరిగి 0;
        }
        len++;
    }

    ఒకవేళ (sink == శూన్యసూచిక || len == 0) { తిరిగి skip + len; }

    int32_t min_సామర్థ్యం = uprv_max(static_cast<int32_t>(len), 4);  // Minimum 3 letters plus NUL.
    అక్షరం scratch[MAXLEN];
    int32_t capacity = 0;
    అక్షరం* బఫరు = sink->Get_Append_బఫరు(
            min_సామర్థ్యం, min_సామర్థ్యం, scratch, UPRV_LENGTHOF(scratch), &capacity);

    కొరకు (size_t i = 0; i < len; ++i) {
        బఫరు[i] = uprv_tolower(localeID[i]);
    }
    ఒకవేళ (localeID.size() >= 2 && _isIDSeparator(localeID[1])) {
        బఫరు[1] = '-';
    }

    ఒకవేళ (len == 3) {
        /* convert 3 character code to 2 character code if possible *CWB*/
        U_ASSERT(capacity >= 4);
        బఫరు[3] = '\0';
        std::optional<int16_t> స్థానభ్రంశం = కనుగొను_సూచీ_2(LANGUAGES_3, బఫరు);
        ఒకవేళ (స్థానభ్రంశం.has_value()) {
            స్థిరరాశి అక్షరం* స్థిరరాశి alias = LANGUAGES[*స్థానభ్రంశం];
            sink->Append(alias, static_cast<int32_t>(uprv_strlen(alias)));
            తిరిగి skip + len;
        }
    }

    sink->Append(బఫరు, static_cast<int32_t>(len));
    తిరిగి skip + len;
}

size_t _getScript(std::string_view localeID, ByteSink* sink) {
    constexpr int32_t నిడివి_3 = 4;

    size_t len = 0;
    అయితే (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len]) &&
            uprv_isASCIILetter(localeID[len])) {
        ఒకవేళ (len == నిడివి_3) { తిరిగి 0; }
        len++;
    }
    ఒకవేళ (len != నిడివి_3) { తిరిగి 0; }

    ఒకవేళ (sink == శూన్యసూచిక) { తిరిగి len; }

    అక్షరం scratch[నిడివి_3];
    int32_t capacity = 0;
    అక్షరం* బఫరు = sink->Get_Append_బఫరు(
            నిడివి_3, నిడివి_3, scratch, UPRV_LENGTHOF(scratch), &capacity);

    బఫరు[0] = uprv_toupper(localeID[0]);
    కొరకు (int32_t i = 1; i < నిడివి_3; ++i) {
        బఫరు[i] = uprv_tolower(localeID[i]);
    }

    sink->Append(బఫరు, నిడివి_3);
    తిరిగి len;
}

size_t _getRegion(std::string_view localeID, ByteSink* sink) {
    constexpr int32_t MINLEN = 2;
    constexpr int32_t MAXLEN = ULOC_COUNTRY_సామర్థ్యం - 1;  // Minus NUL.

    size_t len = 0;
    అయితే (len < localeID.size() && !_isTerminator(localeID[len]) && !_isIDSeparator(localeID[len])) {
        ఒకవేళ (len == MAXLEN) { తిరిగి 0; }
        len++;
    }
    ఒకవేళ (len < MINLEN) { తిరిగి 0; }

    ఒకవేళ (sink == శూన్యసూచిక) { తిరిగి len; }

    అక్షరం scratch[ULOC_COUNTRY_సామర్థ్యం];
    int32_t capacity = 0;
    అక్షరం* బఫరు = sink->Get_Append_బఫరు(
            ULOC_COUNTRY_సామర్థ్యం,
            ULOC_COUNTRY_సామర్థ్యం,
            scratch,
            UPRV_LENGTHOF(scratch),
            &capacity);

    కొరకు (size_t i = 0; i < len; ++i) {
        బఫరు[i] = uprv_toupper(localeID[i]);
    }

    ఒకవేళ (len == 3) {
        /* convert 3 character code to 2 character code if possible *CWB*/
        U_ASSERT(capacity >= 4);
        బఫరు[3] = '\0';
        std::optional<int16_t> స్థానభ్రంశం = కనుగొను_సూచీ_2(COUNTRIES_3, బఫరు);
        ఒకవేళ (స్థానభ్రంశం.has_value()) {
            స్థిరరాశి అక్షరం* స్థిరరాశి alias = COUNTRIES[*స్థానభ్రంశం];
            sink->Append(alias, static_cast<int32_t>(uprv_strlen(alias)));
            తిరిగి len;
        }
    }

    sink->Append(బఫరు, static_cast<int32_t>(len));
    తిరిగి len;
}

/**
 * @param needSeparator if true, then add leading '_' if any variants
 * are added to 'variant'
 */
size_t
_getVariant(std::string_view localeID,
            అక్షరం prev,
            ByteSink* sink,
            తార్కిక needSeparator,
            UError_సంకేతం& స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2) || localeID.empty()) తిరిగి 0;

    // Reasonable upper limit for variants
    // There are no strict limitation of the syntax of variant in the legacy
    // locale format. If the locale is constructed from unicode_locale_id
    // as defined in UTS35, then we know each unicode_variant_subtag
    // could have max length of 8 ((alphanum{5,8} | digit alphanum{3})
    // 179 would allow 20 unicode_variant_subtag with sep in the
    // unicode_locale_id
    // 8*20 + 1*(20-1) = 179
    constexpr int32_t MAX_VARIANTS_నిడివి = 179;

    /* get one or more variant tags and separate them with '_' */
    size_t సూచీ = 0;
    ఒకవేళ (_isIDSeparator(prev)) {
        /* get a variant string after a '-' or '_' */
        కొరకు (std::string_view sub = localeID;;) {
            size_t next = sub.find_first_of(".@_-");
            // For historical reasons, a trailing separator is included in the variant.
            తార్కిక ముగిసిన = next == std::string_view::npos || next + 1 == sub.length();
            size_t హద్దు = ముగిసిన ? sub.length() : next;
            సూచీ += హద్దు;
            ఒకవేళ (సూచీ > MAX_VARIANTS_నిడివి) {
                స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
                తిరిగి 0;
            }

            ఒకవేళ (sink != శూన్యసూచిక) {
                ఒకవేళ (needSeparator) {
                    sink->Append("_", 1);
                } లేకపోతే {
                    needSeparator = నిజం;
                }

                int32_t length = static_cast<int32_t>(హద్దు);
                int32_t min_సామర్థ్యం = uprv_min(length, MAX_VARIANTS_నిడివి);
                అక్షరం scratch[MAX_VARIANTS_నిడివి];
                int32_t capacity = 0;
                అక్షరం* బఫరు = sink->Get_Append_బఫరు(
                        min_సామర్థ్యం, min_సామర్థ్యం, scratch, UPRV_LENGTHOF(scratch), &capacity);

                కొరకు (size_t i = 0; i < హద్దు; ++i) {
                    బఫరు[i] = uprv_toupper(sub[i]);
                }
                sink->Append(బఫరు, length);
            }

            ఒకవేళ (ముగిసిన) { తిరిగి సూచీ; }
            sub.remove_ఉపసర్గ(next);
            ఒకవేళ (_isTerminator(sub.front()) || _isBCP47Extension(sub)) { తిరిగి సూచీ; }
            sub.remove_ఉపసర్గ(1);
            సూచీ++;
        }
    }

    size_t skip = 0;
    /* if there is no variant tag after a '-' or '_' then look for '@' */
    ఒకవేళ (prev == '@') {
        /* keep localeID */
    } లేకపోతే ఒకవేళ (స్థిరరాశి అక్షరం* p = locale_get_Keywords_ప్రారంభం(localeID); p != శూన్యసూచిక) {
        skip = 1 + p - localeID.దత్తాంశం(); /* point after the '@' */
        localeID.remove_ఉపసర్గ(skip);
    } లేకపోతే {
        తిరిగి 0;
    }
    కొరకు (; సూచీ < localeID.size() && !_isTerminator(localeID[సూచీ]); సూచీ++) {
        ఒకవేళ (సూచీ >= MAX_VARIANTS_నిడివి) { // same as length > MAX_VARIANTS_LENGTH
            స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
            తిరిగి 0;
        }
        ఒకవేళ (needSeparator) {
            ఒకవేళ (sink != శూన్యసూచిక) {
                sink->Append("_", 1);
            }
            needSeparator = అబద్ధం;
        }
        ఒకవేళ (sink != శూన్యసూచిక) {
            అక్షరం c = uprv_toupper(localeID[సూచీ]);
            ఒకవేళ (c == '-' || c == ',') c = '_';
            sink->Append(&c, 1);
        }
    }
    తిరిగి skip + సూచీ;
}

}  // namespace

U_EXPORT CharString
ulocimp_getLanguage(std::string_view localeID, UError_సంకేతం& స్థితి_2) {
    తిరిగి ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Char_String(
        [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            ulocimp_getSubtags(
                    localeID,
                    &sink,
                    శూన్యసూచిక,
                    శూన్యసూచిక,
                    శూన్యసూచిక,
                    శూన్యసూచిక,
                    స్థితి_2);
        },
        స్థితి_2);
}

U_EXPORT CharString
ulocimp_getScript(std::string_view localeID, UError_సంకేతం& స్థితి_2) {
    తిరిగి ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Char_String(
        [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            ulocimp_getSubtags(
                    localeID,
                    శూన్యసూచిక,
                    &sink,
                    శూన్యసూచిక,
                    శూన్యసూచిక,
                    శూన్యసూచిక,
                    స్థితి_2);
        },
        స్థితి_2);
}

U_EXPORT CharString
ulocimp_getRegion(std::string_view localeID, UError_సంకేతం& స్థితి_2) {
    తిరిగి ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Char_String(
        [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            ulocimp_getSubtags(
                    localeID,
                    శూన్యసూచిక,
                    శూన్యసూచిక,
                    &sink,
                    శూన్యసూచిక,
                    శూన్యసూచిక,
                    స్థితి_2);
        },
        స్థితి_2);
}

U_EXPORT CharString
ulocimp_getVariant(std::string_view localeID, UError_సంకేతం& స్థితి_2) {
    తిరిగి ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Char_String(
        [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            ulocimp_getSubtags(
                    localeID,
                    శూన్యసూచిక,
                    శూన్యసూచిక,
                    శూన్యసూచిక,
                    &sink,
                    శూన్యసూచిక,
                    స్థితి_2);
        },
        స్థితి_2);
}

U_EXPORT శూన్య
ulocimp_getSubtags(
        std::string_view localeID,
        CharString* language,
        CharString* script,
        CharString* region,
        CharString* variant,
        స్థిరరాశి అక్షరం** p_చివర,
        UError_సంకేతం& స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }

    std::optional<CharStringByteSink> languageSink;
    std::optional<CharStringByteSink> scriptSink;
    std::optional<CharStringByteSink> regionSink;
    std::optional<CharStringByteSink> variantSink;

    ఒకవేళ (language != శూన్యసూచిక) { languageSink.emplace(language); }
    ఒకవేళ (script != శూన్యసూచిక) { scriptSink.emplace(script); }
    ఒకవేళ (region != శూన్యసూచిక) { regionSink.emplace(region); }
    ఒకవేళ (variant != శూన్యసూచిక) { variantSink.emplace(variant); }

    ulocimp_getSubtags(
            localeID,
            languageSink.has_value() ? &*languageSink : శూన్యసూచిక,
            scriptSink.has_value() ? &*scriptSink : శూన్యసూచిక,
            regionSink.has_value() ? &*regionSink : శూన్యసూచిక,
            variantSink.has_value() ? &*variantSink : శూన్యసూచిక,
            p_చివర,
            స్థితి_2);
}

U_EXPORT శూన్య
ulocimp_getSubtags(
        std::string_view localeID,
        ByteSink* language,
        ByteSink* script,
        ByteSink* region,
        ByteSink* variant,
        స్థిరరాశి అక్షరం** p_చివర,
        UError_సంకేతం& స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }

    ఒకవేళ (p_చివర != శూన్యసూచిక) {
        *p_చివర = localeID.దత్తాంశం();
    } లేకపోతే ఒకవేళ (language == శూన్యసూచిక &&
               script == శూన్యసూచిక &&
               region == శూన్యసూచిక &&
               variant == శూన్యసూచిక) {
        తిరిగి;
    }

    ఒకవేళ (localeID.empty()) { తిరిగి; }

    తార్కిక hasRegion = అబద్ధం;

    {
        size_t len = _getLanguage(localeID, language, స్థితి_2);
        ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }
        ఒకవేళ (len > 0) {
            localeID.remove_ఉపసర్గ(len);
        }
    }

    ఒకవేళ (p_చివర != శూన్యసూచిక) {
        *p_చివర = localeID.దత్తాంశం();
    } లేకపోతే ఒకవేళ (script == శూన్యసూచిక &&
               region == శూన్యసూచిక &&
               variant == శూన్యసూచిక) {
        తిరిగి;
    }

    ఒకవేళ (localeID.empty()) { తిరిగి; }

    ఒకవేళ (_isIDSeparator(localeID.front())) {
        std::string_view sub = localeID;
        sub.remove_ఉపసర్గ(1);
        size_t len = _getScript(sub, script);
        ఒకవేళ (len > 0) {
            localeID.remove_ఉపసర్గ(len + 1);
            ఒకవేళ (p_చివర != శూన్యసూచిక) { *p_చివర = localeID.దత్తాంశం(); }
        }
    }

    ఒకవేళ ((region == శూన్యసూచిక && variant == శూన్యసూచిక && p_చివర == శూన్యసూచిక) || localeID.empty()) { తిరిగి; }

    ఒకవేళ (_isIDSeparator(localeID.front())) {
        std::string_view sub = localeID;
        sub.remove_ఉపసర్గ(1);
        size_t len = _getRegion(sub, region);
        ఒకవేళ (len > 0) {
            hasRegion = నిజం;
            localeID.remove_ఉపసర్గ(len + 1);
            ఒకవేళ (p_చివర != శూన్యసూచిక) { *p_చివర = localeID.దత్తాంశం(); }
        }
    }

    ఒకవేళ ((variant == శూన్యసూచిక && p_చివర == శూన్యసూచిక) || localeID.empty()) { తిరిగి; }

    తార్కిక hasVariant = అబద్ధం;

    ఒకవేళ (_isIDSeparator(localeID.front()) && !_isBCP47Extension(localeID)) {
        std::string_view sub = localeID;
        /* If there was no country ID, skip a possible extra IDSeparator */
        size_t skip = !hasRegion && localeID.size() > 1 && _isIDSeparator(localeID[1]) ? 2 : 1;
        sub.remove_ఉపసర్గ(skip);
        size_t len = _getVariant(sub, localeID[0], variant, అబద్ధం, స్థితి_2);
        ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }
        ఒకవేళ (len > 0) {
            hasVariant = నిజం;
            localeID.remove_ఉపసర్గ(skip + len);
            ఒకవేళ (p_చివర != శూన్యసూచిక) { *p_చివర = localeID.దత్తాంశం(); }
        }
    }

    ఒకవేళ ((variant == శూన్యసూచిక && p_చివర == శూన్యసూచిక) || localeID.empty()) { తిరిగి; }

    ఒకవేళ (_isBCP47Extension(localeID)) {
        localeID.remove_ఉపసర్గ(2);
        constexpr అక్షరం vaposix[] = "-va-posix";
        constexpr size_t length = పరిమాణం vaposix - 1;
        కొరకు (size_t next;; localeID.remove_ఉపసర్గ(next)) {
            next = localeID.find('-', 1);
            ఒకవేళ (next == std::string_view::npos) { ఆపు; }
            next = localeID.find('-', next + 1);
            తార్కిక ముగిసిన = next == std::string_view::npos;
            std::string_view sub = localeID;
            ఒకవేళ (!ముగిసిన) { sub.remove_suffix(sub.length() - next); }

            ఒకవేళ (sub.length() == length && uprv_strnicmp(sub.దత్తాంశం(), vaposix, length) == 0) {
                ఒకవేళ (variant != శూన్యసూచిక) {
                    ఒకవేళ (hasVariant) { variant->Append("_", 1); }
                    constexpr అక్షరం posix[] = "POSIX";
                    variant->Append(posix, పరిమాణం posix - 1);
                }
                ఒకవేళ (p_చివర != శూన్యసూచిక) { *p_చివర = localeID.దత్తాంశం() + length; }
            }

            ఒకవేళ (ముగిసిన) { ఆపు; }
        }
    }
}

/* Keyword enumeration */

రకనిర్వచనం నిర్మాణం UKeywordsContext {
    అక్షరం* keywords;
    అక్షరం* current;
} UKeywordsContext;

U_CDECL_BEGIN

స్థిర శూన్య U_CALLCONV
uloc_kw_closeKeywords(UEnumeration *enumerator) {
    uprv_free(((UKeywordsContext *)enumerator->context)->keywords);
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

స్థిర int32_t U_CALLCONV
uloc_kw_లెక్క_Keywords(UEnumeration *en, UError_సంకేతం * /*స్థితి*/) {
    అక్షరం *kw = ((UKeywordsContext *)en->context)->keywords;
    int32_t ఫలితం = 0;
    అయితే(*kw) {
        ఫలితం++;
        kw += uprv_strlen(kw)+1;
    }
    తిరిగి ఫలితం;
}

స్థిర స్థిరరాశి అక్షరం * U_CALLCONV
uloc_kw_తదుపరి_Keyword(UEnumeration* en,
                    int32_t* ఫలితం_నిడివి,
                    UError_సంకేతం* /*స్థితి*/) {
    స్థిరరాశి అక్షరం* ఫలితం = ((UKeywordsContext *)en->context)->current;
    int32_t len = 0;
    ఒకవేళ(*ఫలితం) {
        len = (int32_t)uprv_strlen(((UKeywordsContext *)en->context)->current);
        ((UKeywordsContext *)en->context)->current += len+1;
    } లేకపోతే {
        ఫలితం = శూన్యసూచిక;
    }
    ఒకవేళ (ఫలితం_నిడివి) {
        *ఫలితం_నిడివి = len;
    }
    తిరిగి ఫలితం;
}

స్థిర శూన్య U_CALLCONV
uloc_kw_resetKeywords(UEnumeration* en,
                      UError_సంకేతం* /*స్థితి*/) {
    ((UKeywordsContext *)en->context)->current = ((UKeywordsContext *)en->context)->keywords;
}

U_CDECL_చివర


స్థిర స్థిరరాశి UEnumeration gKeywordsEnum = {
    శూన్యసూచిక,
    శూన్యసూచిక,
    uloc_kw_closeKeywords,
    uloc_kw_లెక్క_Keywords,
    uenum_unextDefault,
    uloc_kw_తదుపరి_Keyword,
    uloc_kw_resetKeywords
};

U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywordList(స్థిరరాశి అక్షరం *keywordList, int32_t keyword_List_కొలత, UError_సంకేతం* స్థితి_2)
{
    ఒకవేళ (U_FAILURE(*స్థితి_2)) { తిరిగి శూన్యసూచిక; }

    Local_జ్ఞాపకం<UKeywordsContext> myContext;
    Local_జ్ఞాపకం<UEnumeration> ఫలితం;

    myContext.adoptInstead(static_cast<UKeywordsContext *>(uprv_malloc(పరిమాణం(UKeywordsContext))));
    ఫలితం.adoptInstead(static_cast<UEnumeration *>(uprv_malloc(పరిమాణం(UEnumeration))));
    ఒకవేళ (myContext.isNull() || ఫలితం.isNull()) {
        *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    uprv_memcpy(ఫలితం.getAlias(), &gKeywordsEnum, పరిమాణం(UEnumeration));
    myContext->keywords = static_cast<అక్షరం *>(uprv_malloc(keyword_List_కొలత+1));
    ఒకవేళ (myContext->keywords == శూన్యసూచిక) {
        *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    uprv_memcpy(myContext->keywords, keywordList, keyword_List_కొలత);
    myContext->keywords[keyword_List_కొలత] = 0;
    myContext->current = myContext->keywords;
    ఫలితం->context = myContext.orphan();
    తిరిగి ఫలితం.orphan();
}

U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywords(స్థిరరాశి అక్షరం* localeID,
                        UError_సంకేతం* స్థితి_2)
{
    ఒకవేళ(స్థితి_2==శూన్యసూచిక || U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    CharString తాత్కాలిక_బఫరు;
    స్థిరరాశి అక్షరం* tmpLocaleID;

    ఒకవేళ (localeID != శూన్యసూచిక && _hasBCP47Extension(localeID)) {
        తాత్కాలిక_బఫరు = ulocimp_for_Language_ట్యాగ్(localeID, -1, శూన్యసూచిక, *స్థితి_2);
        tmpLocaleID = U_SUCCESS(*స్థితి_2) && !తాత్కాలిక_బఫరు.is_ఖాళీలు() ? తాత్కాలిక_బఫరు.దత్తాంశం() : localeID;
    } లేకపోతే {
        ఒకవేళ (localeID==శూన్యసూచిక) {
            localeID=uloc_getDefault();
        }
        tmpLocaleID=localeID;
    }

    ulocimp_getSubtags(
            tmpLocaleID,
            శూన్యసూచిక,
            శూన్యసూచిక,
            శూన్యసూచిక,
            శూన్యసూచిక,
            &tmpLocaleID,
            *స్థితి_2);
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    /* keywords are located after '@' */
    ఒకవేళ((tmpLocaleID = locale_get_Keywords_ప్రారంభం(tmpLocaleID)) != శూన్యసూచిక) {
        CharString keywords = ulocimp_getKeywords(tmpLocaleID + 1, '@', అబద్ధం, *స్థితి_2);
        ఒకవేళ (U_FAILURE(*స్థితి_2)) {
            తిరిగి శూన్యసూచిక;
        }
        తిరిగి uloc_openKeywordList(keywords.దత్తాంశం(), keywords.length(), స్థితి_2);
    }
    తిరిగి శూన్యసూచిక;
}


/* bit-flags for 'options' parameter of _canonicalize */
#నిర్వచించు _ULOC_STRIP_KEYWORDS 0x2
#నిర్వచించు _ULOC_CANONICALIZE   0x1

నామాకాశ {

అంతఃవరుస తార్కిక OPTION_సమితి(uint32_t options, uint32_t mask) { తిరిగి (options & mask) != 0; }

constexpr అక్షరం i_default[] = {'i', '-', 'd', 'e', 'f', 'a', 'u', 'l', 't'};
constexpr int32_t I_DEFAULT_నిడివి = UPRV_LENGTHOF(i_default);

/**
 * Canonicalize the given localeID, to level 1 or to level 2,
 * depending on the options.  To specify level 1, pass in options=0.
 * To specify level 2, pass in options=_ULOC_CANONICALIZE.
 *
 * This is the code underlying uloc_getName and uloc_canonicalize.
 */
శూన్య
_canonicalize(std::string_view localeID,
              ByteSink& sink,
              uint32_t options,
              UError_సంకేతం& err) {
    ఒకవేళ (U_FAILURE(err)) {
        తిరిగి;
    }

    int32_t j, క్షేత్రం_లెక్క=0;
    CharString తాత్కాలిక_బఫరు;  // if localeID has a BCP47 extension, tmpLocaleID points to this
    CharString localeIDWithHyphens;  // if localeID has a BPC47 extension and have _, tmpLocaleID points to this
    std::string_view origLocaleID;
    std::string_view tmpLocaleID;
    size_t keyword_కేటాయించు = std::string_view::npos;
    size_t separatorIndicator = std::string_view::npos;

    ఒకవేళ (_hasBCP47Extension(localeID)) {
        std::string_view localeIDPtr = localeID;

        // convert all underbars to hyphens, unless the "BCP47 extension" comes at the beginning of the string
        ఒకవేళ (localeID.size() >= 2 && localeID.find('_') != std::string_view::npos && localeID[1] != '-' && localeID[1] != '_') {
            localeIDWithHyphens.append(localeID, err);
            ఒకవేళ (U_SUCCESS(err)) {
                కొరకు (అక్షరం* p = localeIDWithHyphens.దత్తాంశం(); *p != '\0'; ++p) {
                    ఒకవేళ (*p == '_') {
                        *p = '-';
                    }
                }
                localeIDPtr = localeIDWithHyphens.గమ్యం_String_Piece();
            }
        }

        తాత్కాలిక_బఫరు = ulocimp_for_Language_ట్యాగ్(localeIDPtr.దత్తాంశం(), static_cast<int32_t>(localeIDPtr.size()), శూన్యసూచిక, err);
        tmpLocaleID = U_SUCCESS(err) && !తాత్కాలిక_బఫరు.is_ఖాళీలు() ? static_cast<std::string_view>(తాత్కాలిక_బఫరు.గమ్యం_String_Piece()) : localeIDPtr;
    } లేకపోతే {
        tmpLocaleID=localeID;
    }

    origLocaleID=tmpLocaleID;

    /* get all pieces, one after another, and separate with '_' */
    CharString ట్యాగ్;
    CharString script;
    CharString country;
    CharString variant;
    స్థిరరాశి అక్షరం* చివర = శూన్యసూచిక;
    ulocimp_getSubtags(
            tmpLocaleID,
            &ట్యాగ్,
            &script,
            &country,
            &variant,
            &చివర,
            err);
    ఒకవేళ (U_FAILURE(err)) {
        తిరిగి;
    }
    U_ASSERT(చివర != శూన్యసూచిక);
    ఒకవేళ (చివర > tmpLocaleID.దత్తాంశం()) {
        tmpLocaleID.remove_ఉపసర్గ(చివర - tmpLocaleID.దత్తాంశం());
    }

    ఒకవేళ (ట్యాగ్.length() == I_DEFAULT_నిడివి && origLocaleID.length() >= I_DEFAULT_నిడివి &&
            uprv_strncmp(origLocaleID.దత్తాంశం(), i_default, I_DEFAULT_నిడివి) == 0) {
        ట్యాగ్.clear();
        ట్యాగ్.append(uloc_getDefault(), err);
    } లేకపోతే {
        ఒకవేళ (!script.is_ఖాళీలు()) {
            ++క్షేత్రం_లెక్క;
            ట్యాగ్.append('_', err);
            ట్యాగ్.append(script, err);
        }
        ఒకవేళ (!country.is_ఖాళీలు()) {
            ++క్షేత్రం_లెక్క;
            ట్యాగ్.append('_', err);
            ట్యాగ్.append(country, err);
        }
        ఒకవేళ (!variant.is_ఖాళీలు()) {
            ++క్షేత్రం_లెక్క;
            ఒకవేళ (country.is_ఖాళీలు()) {
                ట్యాగ్.append('_', err);
            }
            ట్యాగ్.append('_', err);
            ట్యాగ్.append(variant, err);
        }
    }

    /* Copy POSIX-style charset specifier, if any [mr.utf8] */
    ఒకవేళ (!OPTION_సమితి(options, _ULOC_CANONICALIZE) && !tmpLocaleID.empty() && tmpLocaleID.front() == '.') {
        ట్యాగ్.append('.', err);
        tmpLocaleID.remove_ఉపసర్గ(1);
        size_t length;
        ఒకవేళ (size_t atPos = tmpLocaleID.find('@'); atPos != std::string_view::npos) {
            length = atPos;
        } లేకపోతే {
            length = tmpLocaleID.length();
        }
        // The longest charset name we found in IANA charset registry
        // https://www.iana.org/assignments/character-sets/ is
        // "Extended_UNIX_Code_Packed_Format_for_Japanese" in length 45.
        // we therefore restrict the length here to be 64 which is a power of 2
        // number that is longer than 45.
        constexpr size_t k_Max_Charset_నిడివి = 64;
        ఒకవేళ (length > k_Max_Charset_నిడివి) {
           err = U_ILLEGAL_వాదం_ERROR; /* malformed keyword name */
           తిరిగి;
        }
        ఒకవేళ (length > 0) {
            ట్యాగ్.append(tmpLocaleID.దత్తాంశం(), static_cast<int32_t>(length), err);
            tmpLocaleID.remove_ఉపసర్గ(length);
        }
    }

    /* Scan ahead to next '@' and determine if it is followed by '=' and/or ';'
       After this, tmpLocaleID either starts at '@' or is empty. */
    ఒకవేళ (స్థిరరాశి అక్షరం* ప్రారంభం = locale_get_Keywords_ప్రారంభం(tmpLocaleID); ప్రారంభం != శూన్యసూచిక) {
        ఒకవేళ (ప్రారంభం > tmpLocaleID.దత్తాంశం()) {
            tmpLocaleID.remove_ఉపసర్గ(ప్రారంభం - tmpLocaleID.దత్తాంశం());
        }
        keyword_కేటాయించు = tmpLocaleID.find('=');
        separatorIndicator = tmpLocaleID.find(';');
    } లేకపోతే {
        tmpLocaleID = {};
    }

    /* Copy POSIX-style variant, if any [mr@FOO] */
    ఒకవేళ (!OPTION_సమితి(options, _ULOC_CANONICALIZE) &&
        !tmpLocaleID.empty() && keyword_కేటాయించు == std::string_view::npos) {
        ట్యాగ్.append(tmpLocaleID, err);
        tmpLocaleID = {};
    }

    ఒకవేళ (OPTION_సమితి(options, _ULOC_CANONICALIZE)) {
        /* Handle @FOO variant if @ is present and not followed by = */
        ఒకవేళ (!tmpLocaleID.empty() && keyword_కేటాయించు == std::string_view::npos) {
            /* Add missing '_' if needed */
            ఒకవేళ (క్షేత్రం_లెక్క < 2 || (క్షేత్రం_లెక్క < 3 && !script.is_ఖాళీలు())) {
                చేయి {
                    ట్యాగ్.append('_', err);
                    ++క్షేత్రం_లెక్క;
                } అయితే(క్షేత్రం_లెక్క<2);
            }

            CharStringByteSink s(&ట్యాగ్);
            std::string_view sub = tmpLocaleID;
            sub.remove_ఉపసర్గ(1);
            _getVariant(sub, '@', &s, !variant.is_ఖాళీలు(), err);
            ఒకవేళ (U_FAILURE(err)) { తిరిగి; }
        }

        /* Look up the ID in the canonicalization map */
        కొరకు (j=0; j<UPRV_LENGTHOF(CANONICALIZE_MAP); j++) {
            StringPiece id(CANONICALIZE_MAP[j].id);
            ఒకవేళ (ట్యాగ్ == id) {
                ఒకవేళ (id.empty() && !tmpLocaleID.empty()) {
                    ఆపు; /* Don't remap "" if keywords present */
                }
                ట్యాగ్.clear();
                ట్యాగ్.append(CANONICALIZE_MAP[j].canonicalID, err);
                ఆపు;
            }
        }
    }

    sink.Append(ట్యాగ్.దత్తాంశం(), ట్యాగ్.length());

    ఒకవేళ (!OPTION_సమితి(options, _ULOC_STRIP_KEYWORDS)) {
        ఒకవేళ (!tmpLocaleID.empty() && keyword_కేటాయించు != std::string_view::npos &&
            (separatorIndicator == std::string_view::npos || separatorIndicator > keyword_కేటాయించు)) {
            sink.Append("@", 1);
            ++క్షేత్రం_లెక్క;
            tmpLocaleID.remove_ఉపసర్గ(1);
            ulocimp_getKeywords(tmpLocaleID, '@', sink, నిజం, err);
        }
    }
}

}  // namespace

/* ### ID parsing API **************************************************/

U_CAPI int32_t  U_EXPORT2
uloc_get_మాతృ(స్థిరరాశి అక్షరం*    localeID,
               అక్షరం* మాతృ,
               int32_t మాతృ_సామర్థ్యం,
               UError_సంకేతం* err)
{
    తిరిగి ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Terminated_Chars(
        మాతృ, మాతృ_సామర్థ్యం,
        [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            ulocimp_get_మాతృ(localeID, sink, స్థితి_2);
        },
        *err);
}

U_EXPORT CharString
ulocimp_get_మాతృ(స్థిరరాశి అక్షరం* localeID,
                  UError_సంకేతం& err)
{
    తిరిగి ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Char_String(
        [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            ulocimp_get_మాతృ(localeID, sink, స్థితి_2);
        },
        err);
}

U_EXPORT శూన్య
ulocimp_get_మాతృ(స్థిరరాశి అక్షరం* localeID,
                  icu::ByteSink& sink,
                  UError_సంకేతం& err)
{
    ఒకవేళ (U_FAILURE(err)) { తిరిగి; }

    స్థిరరాశి అక్షరం *చివరి_Underscore;
    int32_t i;

    ఒకవేళ (localeID == శూన్యసూచిక)
        localeID = uloc_getDefault();

    చివరి_Underscore=uprv_strrchr(localeID, '_');
    ఒకవేళ(చివరి_Underscore!=శూన్యసూచిక) {
        i = static_cast<int32_t>(చివరి_Underscore - localeID);
    } లేకపోతే {
        i=0;
    }

    ఒకవేళ (i > 0) {
        ఒకవేళ (uprv_strnicmp(localeID, "und_", 4) == 0) {
            localeID += 3;
            i -= 3;
        }
        sink.Append(localeID, i);
    }
}

U_CAPI int32_t U_EXPORT2
uloc_getLanguage(స్థిరరాశి అక్షరం*    localeID,
         అక్షరం* language,
         int32_t language_సామర్థ్యం,
         UError_సంకేతం* err)
{
    ఒకవేళ (localeID == శూన్యసూచిక) {
        localeID = uloc_getDefault();
    }

    /* uloc_getLanguage will return a 2 character iso-639 code if one exists. *CWB*/
    తిరిగి ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Terminated_Chars(
        language, language_సామర్థ్యం,
        [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            ulocimp_getSubtags(
                    localeID,
                    &sink,
                    శూన్యసూచిక,
                    శూన్యసూచిక,
                    శూన్యసూచిక,
                    శూన్యసూచిక,
                    స్థితి_2);
        },
        *err);
}

U_CAPI int32_t U_EXPORT2
uloc_getScript(స్థిరరాశి అక్షరం*    localeID,
         అక్షరం* script,
         int32_t script_సామర్థ్యం,
         UError_సంకేతం* err)
{
    ఒకవేళ (localeID == శూన్యసూచిక) {
        localeID = uloc_getDefault();
    }

    తిరిగి ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Terminated_Chars(
        script, script_సామర్థ్యం,
        [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            ulocimp_getSubtags(
                    localeID,
                    శూన్యసూచిక,
                    &sink,
                    శూన్యసూచిక,
                    శూన్యసూచిక,
                    శూన్యసూచిక,
                    స్థితి_2);
        },
        *err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getCountry(స్థిరరాశి అక్షరం* localeID,
            అక్షరం* country,
            int32_t country_సామర్థ్యం,
            UError_సంకేతం* err)
{
    ఒకవేళ (localeID == శూన్యసూచిక) {
        localeID = uloc_getDefault();
    }

    తిరిగి ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Terminated_Chars(
        country, country_సామర్థ్యం,
        [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            ulocimp_getSubtags(
                    localeID,
                    శూన్యసూచిక,
                    శూన్యసూచిక,
                    &sink,
                    శూన్యసూచిక,
                    శూన్యసూచిక,
                    స్థితి_2);
        },
        *err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getVariant(స్థిరరాశి అక్షరం* localeID,
                అక్షరం* variant,
                int32_t variant_సామర్థ్యం,
                UError_సంకేతం* err)
{
    ఒకవేళ (localeID == శూన్యసూచిక) {
        localeID = uloc_getDefault();
    }

    తిరిగి ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Terminated_Chars(
        variant, variant_సామర్థ్యం,
        [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            ulocimp_getSubtags(
                    localeID,
                    శూన్యసూచిక,
                    శూన్యసూచిక,
                    శూన్యసూచిక,
                    &sink,
                    శూన్యసూచిక,
                    స్థితి_2);
        },
        *err);
}

U_CAPI int32_t  U_EXPORT2
uloc_getName(స్థిరరాశి అక్షరం* localeID,
             అక్షరం* name,
             int32_t name_సామర్థ్యం,
             UError_సంకేతం* err)
{
    ఒకవేళ (localeID == శూన్యసూచిక) {
        localeID = uloc_getDefault();
    }
    తిరిగి ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Terminated_Chars(
        name, name_సామర్థ్యం,
        [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            ulocimp_getName(localeID, sink, స్థితి_2);
        },
        *err);
}

U_EXPORT CharString
ulocimp_getName(std::string_view localeID,
                UError_సంకేతం& err)
{
    తిరిగి ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Char_String(
        [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            ulocimp_getName(localeID, sink, స్థితి_2);
        },
        err);
}

U_EXPORT శూన్య
ulocimp_getName(std::string_view localeID,
                ByteSink& sink,
                UError_సంకేతం& err)
{
    _canonicalize(localeID, sink, 0, err);
}

U_CAPI int32_t  U_EXPORT2
uloc_get_పునాది_Name(స్థిరరాశి అక్షరం* localeID,
                 అక్షరం* name,
                 int32_t name_సామర్థ్యం,
                 UError_సంకేతం* err)
{
    ఒకవేళ (localeID == శూన్యసూచిక) {
        localeID = uloc_getDefault();
    }
    తిరిగి ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Terminated_Chars(
        name, name_సామర్థ్యం,
        [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            ulocimp_get_పునాది_Name(localeID, sink, స్థితి_2);
        },
        *err);
}

U_EXPORT CharString
ulocimp_get_పునాది_Name(std::string_view localeID,
                    UError_సంకేతం& err)
{
    తిరిగి ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Char_String(
        [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            ulocimp_get_పునాది_Name(localeID, sink, స్థితి_2);
        },
        err);
}

U_EXPORT శూన్య
ulocimp_get_పునాది_Name(std::string_view localeID,
                    ByteSink& sink,
                    UError_సంకేతం& err)
{
    _canonicalize(localeID, sink, _ULOC_STRIP_KEYWORDS, err);
}

U_CAPI int32_t  U_EXPORT2
uloc_canonicalize(స్థిరరాశి అక్షరం* localeID,
                  అక్షరం* name,
                  int32_t name_సామర్థ్యం,
                  UError_సంకేతం* err)
{
    ఒకవేళ (localeID == శూన్యసూచిక) {
        localeID = uloc_getDefault();
    }
    తిరిగి ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Terminated_Chars(
        name, name_సామర్థ్యం,
        [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            ulocimp_canonicalize(localeID, sink, స్థితి_2);
        },
        *err);
}

U_EXPORT CharString
ulocimp_canonicalize(std::string_view localeID,
                     UError_సంకేతం& err)
{
    తిరిగి ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Char_String(
        [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            ulocimp_canonicalize(localeID, sink, స్థితి_2);
        },
        err);
}

U_EXPORT శూన్య
ulocimp_canonicalize(std::string_view localeID,
                     ByteSink& sink,
                     UError_సంకేతం& err)
{
    _canonicalize(localeID, sink, _ULOC_CANONICALIZE, err);
}

U_CAPI స్థిరరాశి అక్షరం*  U_EXPORT2
uloc_getISO3Language(స్థిరరాశి అక్షరం* localeID)
{
    UError_సంకేతం err = U_ZERO_ERROR;

    ఒకవేళ (localeID == శూన్యసూచిక)
    {
        localeID = uloc_getDefault();
    }
    CharString lang = ulocimp_getLanguage(localeID, err);
    ఒకవేళ (U_FAILURE(err))
        తిరిగి "";
    std::optional<int16_t> స్థానభ్రంశం = కనుగొను_సూచీ_2(LANGUAGES, lang.దత్తాంశం());
    తిరిగి స్థానభ్రంశం.has_value() ? LANGUAGES_3[*స్థానభ్రంశం] : "";
}

U_CAPI స్థిరరాశి అక్షరం*  U_EXPORT2
uloc_getISO3Country(స్థిరరాశి అక్షరం* localeID)
{
    UError_సంకేతం err = U_ZERO_ERROR;

    ఒకవేళ (localeID == శూన్యసూచిక)
    {
        localeID = uloc_getDefault();
    }
    CharString cntry = ulocimp_getRegion(localeID, err);
    ఒకవేళ (U_FAILURE(err))
        తిరిగి "";
    std::optional<int16_t> స్థానభ్రంశం = కనుగొను_సూచీ_2(COUNTRIES, cntry.దత్తాంశం());
    తిరిగి స్థానభ్రంశం.has_value() ? COUNTRIES_3[*స్థానభ్రంశం] : "";
}

U_CAPI uint32_t  U_EXPORT2
uloc_getLCID(స్థిరరాశి అక్షరం* localeID)
{
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    uint32_t   lcid = 0;

    /* Check for incomplete id. */
    ఒకవేళ (!localeID || uprv_strlen(localeID) < 2) {
        తిరిగి 0;
    }

    // First, attempt Windows platform lookup if available, but fall
    // through to catch any special cases (ICU vs Windows name differences).
    lcid = uprv_convert_గమ్యం_LCIDPlatform(localeID, &స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి 0;
    }
    ఒకవేళ (lcid > 0) {
        // Windows found an LCID, return that
        తిరిగి lcid;
    }

    CharString langID = ulocimp_getLanguage(localeID, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి 0;
    }

    ఒకవేళ (uprv_strchr(localeID, '@')) {
        // uprv_convertToLCID does not support keywords other than collation.
        // Remove all keywords except collation.
        CharString collVal = ulocimp_get_Keyword_విలువ(localeID, "collation", స్థితి_2);
        ఒకవేళ (U_SUCCESS(స్థితి_2) && !collVal.is_ఖాళీలు()) {
            CharString tmpLocaleID = ulocimp_get_పునాది_Name(localeID, స్థితి_2);
            ulocimp_సమితి_Keyword_విలువ("collation", collVal.గమ్యం_String_Piece(), tmpLocaleID, స్థితి_2);
            ఒకవేళ (U_SUCCESS(స్థితి_2)) {
                తిరిగి uprv_convert_గమ్యం_LCID(langID.దత్తాంశం(), tmpLocaleID.దత్తాంశం(), &స్థితి_2);
            }
        }

        // fall through - all keywords are simply ignored
        స్థితి_2 = U_ZERO_ERROR;
    }

    తిరిగి uprv_convert_గమ్యం_LCID(langID.దత్తాంశం(), localeID, &స్థితి_2);
}

U_CAPI int32_t U_EXPORT2
uloc_getLocaleForLCID(uint32_t hostid, అక్షరం *locale, int32_t locale_సామర్థ్యం,
                UError_సంకేతం *స్థితి_2)
{
    తిరిగి uprv_convert_గమ్యం_Posix(hostid, locale, locale_సామర్థ్యం, స్థితి_2);
}

/* ### Default locale **************************************************/

U_CAPI స్థిరరాశి అక్షరం*  U_EXPORT2
uloc_getDefault()
{
    తిరిగి locale_get_default();
}

U_CAPI శూన్య  U_EXPORT2
uloc_సమితి_Default(స్థిరరాశి అక్షరం*   newDefaultLocale,
             UError_సంకేతం* err)
{
    ఒకవేళ (U_FAILURE(*err))
        తిరిగి;
    /* the error code isn't currently used for anything by this function*/

    /* propagate change to C++ */
    locale_సమితి_default(newDefaultLocale);
}

/**
 * Returns a list of all 2-letter language codes defined in ISO 639.  This is a pointer
 * to an array of pointers to arrays of char.  All of these pointers are owned
 * by ICU-- do not delete them, and do not write through them.  The array is
 * terminated with a null pointer.
 */
U_CAPI స్థిరరాశి అక్షరం* స్థిరరాశి*  U_EXPORT2
uloc_getISOLanguages()
{
    తిరిగి LANGUAGES;
}

/**
 * Returns a list of all 2-letter country codes defined in ISO 639.  This is a
 * pointer to an array of pointers to arrays of char.  All of these pointers are
 * owned by ICU-- do not delete them, and do not write through them.  The array is
 * terminated with a null pointer.
 */
U_CAPI స్థిరరాశి అక్షరం* స్థిరరాశి*  U_EXPORT2
uloc_getISOCountries()
{
    తిరిగి COUNTRIES;
}

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
uloc_గమ్యం_Unicode_Locale_కీలకం(స్థిరరాశి అక్షరం* keyword U_LIFETIME_BOUND)
{
    ఒకవేళ (keyword == శూన్యసూచిక || *keyword == '\0') { తిరిగి శూన్యసూచిక; }
    std::optional<std::string_view> ఫలితం = ulocimp_గమ్యం_Bcp_కీలకం_With_Fallback(keyword);
    తిరిగి ఫలితం.has_value() ? ఫలితం->దత్తాంశం() : శూన్యసూచిక;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_గమ్యం_Bcp_కీలకం_With_Fallback(std::string_view keyword U_LIFETIME_BOUND)
{
    std::optional<std::string_view> bcp_కీలకం = ulocimp_గమ్యం_Bcp_కీలకం(keyword);
    ఒకవేళ (!bcp_కీలకం.has_value() &&
        ultag_is_Unicode_Locale_కీలకం(keyword.దత్తాంశం(), static_cast<int32_t>(keyword.size()))) {
        // unknown keyword, but syntax is fine..
        తిరిగి keyword;
    }
    తిరిగి bcp_కీలకం;
}

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
uloc_గమ్యం_Unicode_Locale_Type(స్థిరరాశి అక్షరం* keyword, స్థిరరాశి అక్షరం* value U_LIFETIME_BOUND)
{
    ఒకవేళ (keyword == శూన్యసూచిక || *keyword == '\0' ||
        value == శూన్యసూచిక || *value == '\0') { తిరిగి శూన్యసూచిక; }
    std::optional<std::string_view> ఫలితం = ulocimp_గమ్యం_Bcp_Type_With_Fallback(keyword, value);
    తిరిగి ఫలితం.has_value() ? ఫలితం->దత్తాంశం() : శూన్యసూచిక;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_గమ్యం_Bcp_Type_With_Fallback(std::string_view keyword, std::string_view value U_LIFETIME_BOUND)
{
    std::optional<std::string_view> bcpType = ulocimp_గమ్యం_Bcp_Type(keyword, value);
    ఒకవేళ (!bcpType.has_value() &&
        ultag_isUnicodeLocaleType(value.దత్తాంశం(), static_cast<int32_t>(value.size()))) {
        // unknown keyword, but syntax is fine..
        తిరిగి value;
    }
    తిరిగి bcpType;
}

నామాకాశ {

తార్కిక
is_Well_Formed_Legacy_కీలకం(std::string_view కీలకం)
{
    తిరిగి std::all_of(కీలకం.begin(), కీలకం.చివర(), UPRV_ISALPHANUM);
}

తార్కిక
isWellFormedLegacyType(std::string_view legacyType)
{
    int32_t alpha_అంకె_Len = 0;
    కొరకు (అక్షరం c : legacyType) {
        ఒకవేళ (c == '_' || c == '/' || c == '-') {
            ఒకవేళ (alpha_అంకె_Len == 0) {
                తిరిగి అబద్ధం;
            }
            alpha_అంకె_Len = 0;
        } లేకపోతే ఒకవేళ (UPRV_ISALPHANUM(c)) {
            alpha_అంకె_Len++;
        } లేకపోతే {
            తిరిగి అబద్ధం;
        }
    }
    తిరిగి alpha_అంకె_Len != 0;
}

}  // namespace

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
uloc_గమ్యం_Legacy_కీలకం(స్థిరరాశి అక్షరం* keyword U_LIFETIME_BOUND)
{
    ఒకవేళ (keyword == శూన్యసూచిక || *keyword == '\0') { తిరిగి శూన్యసూచిక; }
    std::optional<std::string_view> ఫలితం = ulocimp_గమ్యం_Legacy_కీలకం_With_Fallback(keyword);
    తిరిగి ఫలితం.has_value() ? ఫలితం->దత్తాంశం() : శూన్యసూచిక;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_గమ్యం_Legacy_కీలకం_With_Fallback(std::string_view keyword U_LIFETIME_BOUND)
{
    std::optional<std::string_view> legacy_కీలకం_2 = ulocimp_గమ్యం_Legacy_కీలకం(keyword);
    ఒకవేళ (!legacy_కీలకం_2.has_value() && is_Well_Formed_Legacy_కీలకం(keyword)) {
        // Checks if the specified locale key is well-formed with the legacy locale syntax.
        //
        // Note:
        //  LDML/CLDR provides some definition of keyword syntax in
        //  * http://www.unicode.org/reports/tr35/#Unicode_locale_identifier and
        //  * http://www.unicode.org/reports/tr35/#Old_Locale_Extension_Syntax
        //  Keys can only consist of [0-9a-zA-Z].
        తిరిగి keyword;
    }
    తిరిగి legacy_కీలకం_2;
}

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
uloc_గమ్యం_Legacy_Type(స్థిరరాశి అక్షరం* keyword, స్థిరరాశి అక్షరం* value U_LIFETIME_BOUND)
{
    ఒకవేళ (keyword == శూన్యసూచిక || *keyword == '\0' ||
        value == శూన్యసూచిక || *value == '\0') { తిరిగి శూన్యసూచిక; }
    std::optional<std::string_view> ఫలితం = ulocimp_గమ్యం_Legacy_Type_With_Fallback(keyword, value);
    తిరిగి ఫలితం.has_value() ? ఫలితం->దత్తాంశం() : శూన్యసూచిక;  // Known to be NUL terminated.
}

U_EXPORT std::optional<std::string_view>
ulocimp_గమ్యం_Legacy_Type_With_Fallback(std::string_view keyword, std::string_view value U_LIFETIME_BOUND)
{
    std::optional<std::string_view> legacyType = ulocimp_గమ్యం_Legacy_Type(keyword, value);
    ఒకవేళ (!legacyType.has_value() && isWellFormedLegacyType(value)) {
        // Checks if the specified locale type is well-formed with the legacy locale syntax.
        //
        // Note:
        //  LDML/CLDR provides some definition of keyword syntax in
        //  * http://www.unicode.org/reports/tr35/#Unicode_locale_identifier and
        //  * http://www.unicode.org/reports/tr35/#Old_Locale_Extension_Syntax
        //  Values (types) can only consist of [0-9a-zA-Z], plus for legacy values
        //  we allow [/_-+] in the middle (e.g. "Etc/GMT+1", "Asia/Tel_Aviv")
        తిరిగి value;
    }
    తిరిగి legacyType;
}

/*eof*/
