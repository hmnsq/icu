// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.cpp
// created: 2019may08 Markus W. Scherer

#شامل "unicode/utypes.h"
#شامل "unicode/bytestrie.h"
#شامل "unicode/localematcher.h"
#شامل "unicode/locid.h"
#شامل "unicode/uobject.h"
#شامل "unicode/ures.h"
#شامل "cstring.h"
#شامل "locdistance.h"
#شامل "loclikelysubtags.h"
#شامل "uassert.h"
#شامل "ucln_cmn.h"
#شامل "uinvchar.h"
#شامل "umutex.h"

U_NAMESPACE_BEGIN

نامخلا {

/**
 * Bit flag used on the last character of a subtag in the trie.
 * Must be set consistently by the builder and the lookup code.
 */
constexpr int32_t انتہا_کا_SUBTAG = 0x80;
/** Distance value bit flag, set by the builder. */
constexpr int32_t فاصلہ_SKIP_SCRIPT = 0x80;
/** Distance value bit flag, set by trieNext(). */
constexpr int32_t فاصلہ_IS_FINAL = 0x100;
constexpr int32_t فاصلہ_IS_FINAL_OR_SKIP_SCRIPT = فاصلہ_IS_FINAL | فاصلہ_SKIP_SCRIPT;

constexpr int32_t ABOVE_حد = 100;

// Indexes into array of distances.
شمار {
    IX_DEF_LANG_فاصلہ,
    IX_DEF_SCRIPT_فاصلہ,
    IX_DEF_REGION_فاصلہ,
    IX_MIN_REGION_فاصلہ,
    IX_حد_بندی
};

Locale_فاصلہ *g_Locale_فاصلہ = خالی_اشاریہ;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    حذف g_Locale_فاصلہ;
    g_Locale_فاصلہ = خالی_اشاریہ;
    gInitOnce.reset();
    واپس سچ;
}

}  // namespace

خالی U_CALLCONV Locale_فاصلہ::init_Locale_فاصلہ(UError_رمز &error_رمز) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(g_Locale_فاصلہ == خالی_اشاریہ);
    ثابت LikelySubtags &likely = *LikelySubtags::getSingleton(error_رمز);
    اگر (U_FAILURE(error_رمز)) { واپس; }
    ثابت Locale_فاصلہ_ڈیٹا &ڈیٹا = likely.get_فاصلہ_ڈیٹا();
    اگر (ڈیٹا.فاصلہ_Trie_بائٹیں == خالی_اشاریہ ||
            ڈیٹا.region_مقصد_Partitions == خالی_اشاریہ || ڈیٹا.partitions == خالی_اشاریہ ||
            // ok if no paradigms
            ڈیٹا.distances == خالی_اشاریہ) {
        error_رمز = U_MISSING_RESOURCE_ERROR;
        واپس;
    }
    g_Locale_فاصلہ = نیا Locale_فاصلہ(ڈیٹا, likely);
    اگر (g_Locale_فاصلہ == خالی_اشاریہ) {
        error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_LOCALE_فاصلہ, cleanup);
}

ثابت Locale_فاصلہ *Locale_فاصلہ::getSingleton(UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) { واپس خالی_اشاریہ; }
    umtx_initOnce(gInitOnce, &Locale_فاصلہ::init_Locale_فاصلہ, error_رمز);
    واپس g_Locale_فاصلہ;
}

Locale_فاصلہ::Locale_فاصلہ(ثابت Locale_فاصلہ_ڈیٹا &ڈیٹا, ثابت LikelySubtags &likely) :
        likelySubtags(likely),
        trie(ڈیٹا.فاصلہ_Trie_بائٹیں),
        region_مقصد_Partitions_اشاریہ(ڈیٹا.region_مقصد_Partitions), تقسیم_Arrays(ڈیٹا.partitions),
        paradigmLSRs(ڈیٹا.paradigms), paradigm_LSRs_لمبائی(ڈیٹا.paradigms_لمبائی),
        default_Language_فاصلہ(ڈیٹا.distances[IX_DEF_LANG_فاصلہ]),
        default_Script_فاصلہ(ڈیٹا.distances[IX_DEF_SCRIPT_فاصلہ]),
        default_Region_فاصلہ(ڈیٹا.distances[IX_DEF_REGION_فاصلہ]),
        min_Region_فاصلہ(ڈیٹا.distances[IX_MIN_REGION_فاصلہ]) {
    // For the default demotion value, use the
    // default region distance between unrelated Englishes.
    // Thus, unless demotion is turned off,
    // a mere region difference for one desired locale
    // is as good as a perfect match for the next following desired locale.
    // As of CLDR 36, we have <languageMatch desired="en_*_*" supported="en_*_*" distance="5"/>.
    LSR en("en", "Latn", "US", LSR::EXPLICIT_LSR);
    LSR enGB("en", "Latn", "GB", LSR::EXPLICIT_LSR);
    ثابت LSR *p_enGB = &enGB;
    int32_t اشاریہ_And_فاصلہ = get_بہترین_اشاریہ_And_فاصلہ(en, &p_enGB, 1,
            سرکاؤ_فاصلہ(50), ULOCMATCH_FAVOR_LANGUAGE, ULOCMATCH_DIRECTION_WITH_ONE_WAY);
    default_Demotion_فی_Desired_Locale  = get_فاصلہ_Floor(اشاریہ_And_فاصلہ);
}

int32_t Locale_فاصلہ::get_بہترین_اشاریہ_And_فاصلہ(
        ثابت LSR &desired,
        ثابت LSR **supportedLSRs, int32_t supported_LSRs_لمبائی,
        int32_t shifted_حد,
        ULoc_مطابقت_Favor_Subtag favorSubtag, ULoc_مطابقت_Direction direction) ثابت {
    بائٹیں_Trie iter(trie);
    // Look up the desired language only once for all supported LSRs.
    // Its "distance" is either a match point value of 0, or a non-match negative value.
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t des_Lang_فاصلہ = trie_اگلا(iter, desired.language, جھوٹ);
    uint64_t desLangState = des_Lang_فاصلہ >= 0 && supported_LSRs_لمبائی > 1 ? iter.getState64() : 0;
    // Index of the supported LSR with the lowest distance.
    int32_t بہترین_اشاریہ = -1;
    // Cached lookup info from LikelySubtags.compareLikely().
    int32_t بہترین_Likely_Info = -1;
    برائے (int32_t sl_اشاریہ = 0; sl_اشاریہ < supported_LSRs_لمبائی; ++sl_اشاریہ) {
        ثابت LSR &supported = *supportedLSRs[sl_اشاریہ];
        منطقی ستارہ = جھوٹ;
        int32_t فاصلہ = des_Lang_فاصلہ;
        اگر (فاصلہ >= 0) {
            U_ASSERT((فاصلہ & فاصلہ_IS_FINAL) == 0);
            اگر (sl_اشاریہ != 0) {
                iter.reset_مقصد_State64(desLangState);
            }
            فاصلہ = trie_اگلا(iter, supported.language, سچ);
        }
        // Note: The data builder verifies that there are no rules with "any" (*) language and
        // real (non *) script or region subtags.
        // This means that if the lookup for either language fails we can use
        // the default distances without further lookups.
        int32_t flags;
        اگر (فاصلہ >= 0) {
            flags = فاصلہ & فاصلہ_IS_FINAL_OR_SKIP_SCRIPT;
            فاصلہ &= ~فاصلہ_IS_FINAL_OR_SKIP_SCRIPT;
        } ورنہ {  // <*, *>
            اگر (uprv_strcmp(desired.language, supported.language) == 0) {
                فاصلہ = 0;
            } ورنہ {
                فاصلہ = default_Language_فاصلہ;
            }
            flags = 0;
            ستارہ = سچ;
        }
        U_ASSERT(0 <= فاصلہ && فاصلہ <= 100);
        // Round up the shifted threshold (if fraction bits are not 0)
        // for comparison with un-shifted distances until we need fraction bits.
        // (If we simply shifted non-zero fraction bits away, then we might ignore a language
        // when it's really still a micro distance below the threshold.)
        int32_t rounded_حد = (shifted_حد + فاصلہ_FRACTION_نقاب) >> فاصلہ_سرکاؤ;
        // We implement "favor subtag" by reducing the language subtag distance
        // (unscientifically reducing it to a quarter of the normal value),
        // so that the script distance is relatively more important.
        // For example, given a default language distance of 80, we reduce it to 20,
        // which is below the default threshold of 50, which is the default script distance.
        اگر (favorSubtag == ULOCMATCH_FAVOR_SCRIPT) {
            فاصلہ >>= 2;
        }
        // Let distance == roundedThreshold pass until the tie-breaker logic
        // at the end of the loop.
        اگر (فاصلہ > rounded_حد) {
            جاری;
        }

        int32_t script_فاصلہ;
        اگر (ستارہ || flags != 0) {
            اگر (uprv_strcmp(desired.script, supported.script) == 0) {
                script_فاصلہ = 0;
            } ورنہ {
                script_فاصلہ = default_Script_فاصلہ;
            }
        } ورنہ {
            script_فاصلہ = get_Des_Supp_Script_فاصلہ(iter, iter.getState64(),
                    desired.script, supported.script);
            flags = script_فاصلہ & فاصلہ_IS_FINAL;
            script_فاصلہ &= ~فاصلہ_IS_FINAL;
        }
        فاصلہ += script_فاصلہ;
        اگر (فاصلہ > rounded_حد) {
            جاری;
        }

        اگر (uprv_strcmp(desired.region, supported.region) == 0) {
            // regionDistance = 0
        } ورنہ اگر (ستارہ || (flags & فاصلہ_IS_FINAL) != 0) {
            فاصلہ += default_Region_فاصلہ;
        } ورنہ {
            int32_t بقایا_حد = rounded_حد - فاصلہ;
            اگر (min_Region_فاصلہ > بقایا_حد) {
                جاری;
            }

            // From here on we know the regions are not equal.
            // Map each region to zero or more partitions. (zero = one non-matching string)
            // (Each array of single-character partition strings is encoded as one string.)
            // If either side has more than one, then we find the maximum distance.
            // This could be optimized by adding some more structure, but probably not worth it.
            فاصلہ += get_Region_Partitions_فاصلہ(
                    iter, iter.getState64(),
                    partitionsForRegion(desired),
                    partitionsForRegion(supported),
                    بقایا_حد);
        }
        int32_t shifted_فاصلہ = سرکاؤ_فاصلہ(فاصلہ);
        اگر (shifted_فاصلہ == 0) {
            // Distinguish between equivalent but originally unequal locales via an
            // additional micro distance.
            shifted_فاصلہ |= (desired.flags ^ supported.flags);
            اگر (shifted_فاصلہ < shifted_حد) {
                اگر (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_مطابقت(supported, desired, shifted_حد, favorSubtag)) {
                    اگر (shifted_فاصلہ == 0) {
                        واپس sl_اشاریہ << اشاریہ_سرکاؤ;
                    }
                    بہترین_اشاریہ = sl_اشاریہ;
                    shifted_حد = shifted_فاصلہ;
                    بہترین_Likely_Info = -1;
                }
            }
        } ورنہ {
            اگر (shifted_فاصلہ < shifted_حد) {
                اگر (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_مطابقت(supported, desired, shifted_حد, favorSubtag)) {
                    بہترین_اشاریہ = sl_اشاریہ;
                    shifted_حد = shifted_فاصلہ;
                    بہترین_Likely_Info = -1;
                }
            } ورنہ اگر (shifted_فاصلہ == shifted_حد && بہترین_اشاریہ >= 0) {
                اگر (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_مطابقت(supported, desired, shifted_حد, favorSubtag)) {
                    بہترین_Likely_Info = likelySubtags.compareLikely(
                            supported, *supportedLSRs[بہترین_اشاریہ], بہترین_Likely_Info);
                    اگر ((بہترین_Likely_Info & 1) != 0) {
                        // This supported locale matches as well as the previous best match,
                        // and neither matches perfectly,
                        // but this one is "more likely" (has more-default subtags).
                        بہترین_اشاریہ = sl_اشاریہ;
                    }
                }
            }
        }
    }
    واپس بہترین_اشاریہ >= 0 ?
            (بہترین_اشاریہ << اشاریہ_سرکاؤ) | shifted_حد :
            اشاریہ_NEG_1 | سرکاؤ_فاصلہ(ABOVE_حد);
}

int32_t Locale_فاصلہ::get_Des_Supp_Script_فاصلہ(
        بائٹیں_Trie &iter, uint64_t آغاز_State, ثابت حرف *desired, ثابت حرف *supported) {
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t فاصلہ = trie_اگلا(iter, desired, جھوٹ);
    اگر (فاصلہ >= 0) {
        فاصلہ = trie_اگلا(iter, supported, سچ);
    }
    اگر (فاصلہ < 0) {
        UString_Trie_نتیجہ نتیجہ = iter.reset_مقصد_State64(آغاز_State).next(u'*');  // <*, *>
        U_ASSERT(USTRINGTRIE_HAS_قدر(نتیجہ));
        اگر (uprv_strcmp(desired, supported) == 0) {
            فاصلہ = 0;  // same script
        } ورنہ {
            فاصلہ = iter.get_قدر();
            U_ASSERT(فاصلہ >= 0);
        }
        اگر (نتیجہ == USTRINGTRIE_FINAL_قدر) {
            فاصلہ |= فاصلہ_IS_FINAL;
        }
    }
    واپس فاصلہ;
}

int32_t Locale_فاصلہ::get_Region_Partitions_فاصلہ(
        بائٹیں_Trie &iter, uint64_t آغاز_State,
        ثابت حرف *desiredPartitions, ثابت حرف *supportedPartitions, int32_t حد) {
    حرف desired = *desiredPartitions++;
    حرف supported = *supportedPartitions++;
    U_ASSERT(desired != 0 && supported != 0);
    // See if we have single desired/supported partitions, from NUL-terminated
    // partition strings without explicit length.
    منطقی supp_لمبائی_Gt1 = *supportedPartitions != 0;  // gt1: more than 1 character
    // equivalent to: if (desLength == 1 && suppLength == 1)
    اگر (*desiredPartitions == 0 && !supp_لمبائی_Gt1) {
        // Fastpath for single desired/supported partitions.
        UString_Trie_نتیجہ نتیجہ = iter.next(uprv_inv_Char_مقصد_Ascii(desired) | انتہا_کا_SUBTAG);
        اگر (USTRINGTRIE_HAS_اگلا(نتیجہ)) {
            نتیجہ = iter.next(uprv_inv_Char_مقصد_Ascii(supported) | انتہا_کا_SUBTAG);
            اگر (USTRINGTRIE_HAS_قدر(نتیجہ)) {
                واپس iter.get_قدر();
            }
        }
        واپس get_Fallback_Region_فاصلہ(iter, آغاز_State);
    }

    ثابت حرف *supported_آغاز = supportedPartitions - 1;  // for restart of inner loop
    int32_t region_فاصلہ = 0;
    // Fall back to * only once, not for each pair of partition strings.
    منطقی ستارہ = جھوٹ;
    برائے (;;) {
        // Look up each desired-partition string only once,
        // not for each (desired, supported) pair.
        UString_Trie_نتیجہ نتیجہ = iter.next(uprv_inv_Char_مقصد_Ascii(desired) | انتہا_کا_SUBTAG);
        اگر (USTRINGTRIE_HAS_اگلا(نتیجہ)) {
            uint64_t desState = supp_لمبائی_Gt1 ? iter.getState64() : 0;
            برائے (;;) {
                نتیجہ = iter.next(uprv_inv_Char_مقصد_Ascii(supported) | انتہا_کا_SUBTAG);
                int32_t d;
                اگر (USTRINGTRIE_HAS_قدر(نتیجہ)) {
                    d = iter.get_قدر();
                } ورنہ اگر (ستارہ) {
                    d = 0;
                } ورنہ {
                    d = get_Fallback_Region_فاصلہ(iter, آغاز_State);
                    ستارہ = سچ;
                }
                اگر (d > حد) {
                    واپس d;
                } ورنہ اگر (region_فاصلہ < d) {
                    region_فاصلہ = d;
                }
                اگر ((supported = *supportedPartitions++) != 0) {
                    iter.reset_مقصد_State64(desState);
                } ورنہ {
                    توڑو;
                }
            }
        } ورنہ اگر (!ستارہ) {
            int32_t d = get_Fallback_Region_فاصلہ(iter, آغاز_State);
            اگر (d > حد) {
                واپس d;
            } ورنہ اگر (region_فاصلہ < d) {
                region_فاصلہ = d;
            }
            ستارہ = سچ;
        }
        اگر ((desired = *desiredPartitions++) != 0) {
            iter.reset_مقصد_State64(آغاز_State);
            supportedPartitions = supported_آغاز;
            supported = *supportedPartitions++;
        } ورنہ {
            توڑو;
        }
    }
    واپس region_فاصلہ;
}

int32_t Locale_فاصلہ::get_Fallback_Region_فاصلہ(بائٹیں_Trie &iter, uint64_t آغاز_State) {
#اگر U_DEBUG
    UString_Trie_نتیجہ نتیجہ =
#اگر_ختم
    iter.reset_مقصد_State64(آغاز_State).next(u'*');  // <*, *>
    U_ASSERT(USTRINGTRIE_HAS_قدر(نتیجہ));
    int32_t فاصلہ = iter.get_قدر();
    U_ASSERT(فاصلہ >= 0);
    واپس فاصلہ;
}

int32_t Locale_فاصلہ::trie_اگلا(بائٹیں_Trie &iter, ثابت حرف *s, منطقی want_قدر) {
    uint8_t c;
    اگر ((c = *s) == 0) {
        واپس -1;  // no empty subtags in the distance data
    }
    برائے (;;) {
        c = uprv_inv_Char_مقصد_Ascii(c);
        // EBCDIC: If *s is not an invariant character,
        // then c is now 0 and will simply not match anything, which is harmless.
        uint8_t next = *++s;
        اگر (next != 0) {
            اگر (!USTRINGTRIE_HAS_اگلا(iter.next(c))) {
                واپس -1;
            }
        } ورنہ {
            // last character of this subtag
            UString_Trie_نتیجہ نتیجہ = iter.next(c | انتہا_کا_SUBTAG);
            اگر (want_قدر) {
                اگر (USTRINGTRIE_HAS_قدر(نتیجہ)) {
                    int32_t value = iter.get_قدر();
                    اگر (نتیجہ == USTRINGTRIE_FINAL_قدر) {
                        value |= فاصلہ_IS_FINAL;
                    }
                    واپس value;
                }
            } ورنہ {
                اگر (USTRINGTRIE_HAS_اگلا(نتیجہ)) {
                    واپس 0;
                }
            }
            واپس -1;
        }
        c = next;
    }
}

منطقی Locale_فاصلہ::isParadigmLSR(ثابت LSR &lsr) ثابت {
    // Linear search for a very short list (length 6 as of 2019),
    // because we look for equivalence not equality, and
    // because it's easy.
    // If there are many paradigm LSRs we should use a hash set
    // with custom comparator and hasher.
    U_ASSERT(paradigm_LSRs_لمبائی <= 15);
    برائے (int32_t i = 0; i < paradigm_LSRs_لمبائی; ++i) {
        اگر (lsr.is_Equivalent_مقصد(paradigmLSRs[i])) { واپس سچ; }
    }
    واپس جھوٹ;
}

U_NAMESPACE_انتہا
