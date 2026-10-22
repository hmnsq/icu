// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// locdistance.cpp
// created: 2019may08 Markus W. Scherer

#ضمن "unicode/utypes.h"
#ضمن "unicode/bytestrie.h"
#ضمن "unicode/localematcher.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/uobject.h"
#ضمن "unicode/ures.h"
#ضمن "cstring.h"
#ضمن "locdistance.h"
#ضمن "loclikelysubtags.h"
#ضمن "uassert.h"
#ضمن "ucln_cmn.h"
#ضمن "uinvchar.h"
#ضمن "umutex.h"

U_NAMESPACE_BEGIN

نطاق {

/**
 * Bit flag used on the last character of a subtag in the trie.
 * Must be set consistently by the builder and the lookup code.
 */
constexpr int32_t انتها_از_SUBTAG = 0x80;
/** Distance value bit flag, set by the builder. */
constexpr int32_t فاصله_SKIP_SCRIPT = 0x80;
/** Distance value bit flag, set by trieNext(). */
constexpr int32_t فاصله_IS_FINAL = 0x100;
constexpr int32_t فاصله_IS_FINAL_OR_SKIP_SCRIPT = فاصله_IS_FINAL | فاصله_SKIP_SCRIPT;

constexpr int32_t ABOVE_آستانه = 100;

// Indexes into array of distances.
تعداد {
    IX_DEF_LANG_فاصله,
    IX_DEF_SCRIPT_فاصله,
    IX_DEF_REGION_فاصله,
    IX_MIN_REGION_فاصله,
    IX_حد
};

Locale_فاصله *g_Locale_فاصله = اشاره_تهی;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    حذف g_Locale_فاصله;
    g_Locale_فاصله = اشاره_تهی;
    gInitOnce.reset();
    أرجع درست;
}

}  // namespace

فراغ U_CALLCONV Locale_فاصله::init_Locale_فاصله(UError_کد &error_کد) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(g_Locale_فاصله == اشاره_تهی);
    ثابت LikelySubtags &likely = *LikelySubtags::getSingleton(error_کد);
    إذا (U_FAILURE(error_کد)) { أرجع; }
    ثابت Locale_فاصله_داده &داده = likely.get_فاصله_داده();
    إذا (داده.فاصله_Trie_بایتها == اشاره_تهی ||
            داده.region_مقصد_Partitions == اشاره_تهی || داده.partitions == اشاره_تهی ||
            // ok if no paradigms
            داده.distances == اشاره_تهی) {
        error_کد = U_MISSING_RESOURCE_ERROR;
        أرجع;
    }
    g_Locale_فاصله = جديد Locale_فاصله(داده, likely);
    إذا (g_Locale_فاصله == اشاره_تهی) {
        error_کد = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    ucln_common_ثبات_Cleanup(UCLN_COMMON_LOCALE_فاصله, cleanup);
}

ثابت Locale_فاصله *Locale_فاصله::getSingleton(UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) { أرجع اشاره_تهی; }
    umtx_initOnce(gInitOnce, &Locale_فاصله::init_Locale_فاصله, error_کد);
    أرجع g_Locale_فاصله;
}

Locale_فاصله::Locale_فاصله(ثابت Locale_فاصله_داده &داده, ثابت LikelySubtags &likely) :
        likelySubtags(likely),
        trie(داده.فاصله_Trie_بایتها),
        region_مقصد_Partitions_شاخص(داده.region_مقصد_Partitions), افراز_Arrays(داده.partitions),
        paradigmLSRs(داده.paradigms), paradigm_LSRs_درازی(داده.paradigms_درازی),
        default_Language_فاصله(داده.distances[IX_DEF_LANG_فاصله]),
        default_Script_فاصله(داده.distances[IX_DEF_SCRIPT_فاصله]),
        default_Region_فاصله(داده.distances[IX_DEF_REGION_فاصله]),
        min_Region_فاصله(داده.distances[IX_MIN_REGION_فاصله]) {
    // For the default demotion value, use the
    // default region distance between unrelated Englishes.
    // Thus, unless demotion is turned off,
    // a mere region difference for one desired locale
    // is as good as a perfect match for the next following desired locale.
    // As of CLDR 36, we have <languageMatch desired="en_*_*" supported="en_*_*" distance="5"/>.
    LSR en("en", "Latn", "US", LSR::EXPLICIT_LSR);
    LSR enGB("en", "Latn", "GB", LSR::EXPLICIT_LSR);
    ثابت LSR *p_enGB = &enGB;
    int32_t شاخص_And_فاصله = get_بهترین_شاخص_And_فاصله(en, &p_enGB, 1,
            لغزش_فاصله(50), ULOCMATCH_FAVOR_LANGUAGE, ULOCMATCH_DIRECTION_WITH_ONE_WAY);
    default_Demotion_در_هر_Desired_Locale  = get_فاصله_Floor(شاخص_And_فاصله);
}

int32_t Locale_فاصله::get_بهترین_شاخص_And_فاصله(
        ثابت LSR &desired,
        ثابت LSR **supportedLSRs, int32_t supported_LSRs_درازی,
        int32_t shifted_آستانه,
        ULoc_تطابق_Favor_Subtag favorSubtag, ULoc_تطابق_Direction direction) ثابت {
    بایتها_Trie iter(trie);
    // Look up the desired language only once for all supported LSRs.
    // Its "distance" is either a match point value of 0, or a non-match negative value.
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t des_Lang_فاصله = trie_بعدی(iter, desired.language, خطأ);
    uint64_t desLangState = des_Lang_فاصله >= 0 && supported_LSRs_درازی > 1 ? iter.getState64() : 0;
    // Index of the supported LSR with the lowest distance.
    int32_t بهترین_شاخص = -1;
    // Cached lookup info from LikelySubtags.compareLikely().
    int32_t بهترین_Likely_Info = -1;
    لكل (int32_t sl_شاخص = 0; sl_شاخص < supported_LSRs_درازی; ++sl_شاخص) {
        ثابت LSR &supported = *supportedLSRs[sl_شاخص];
        منطقي ستاره = خطأ;
        int32_t فاصله = des_Lang_فاصله;
        إذا (فاصله >= 0) {
            U_ASSERT((فاصله & فاصله_IS_FINAL) == 0);
            إذا (sl_شاخص != 0) {
                iter.reset_مقصد_State64(desLangState);
            }
            فاصله = trie_بعدی(iter, supported.language, درست);
        }
        // Note: The data builder verifies that there are no rules with "any" (*) language and
        // real (non *) script or region subtags.
        // This means that if the lookup for either language fails we can use
        // the default distances without further lookups.
        int32_t flags;
        إذا (فاصله >= 0) {
            flags = فاصله & فاصله_IS_FINAL_OR_SKIP_SCRIPT;
            فاصله &= ~فاصله_IS_FINAL_OR_SKIP_SCRIPT;
        } وإلا {  // <*, *>
            إذا (uprv_strcmp(desired.language, supported.language) == 0) {
                فاصله = 0;
            } وإلا {
                فاصله = default_Language_فاصله;
            }
            flags = 0;
            ستاره = درست;
        }
        U_ASSERT(0 <= فاصله && فاصله <= 100);
        // Round up the shifted threshold (if fraction bits are not 0)
        // for comparison with un-shifted distances until we need fraction bits.
        // (If we simply shifted non-zero fraction bits away, then we might ignore a language
        // when it's really still a micro distance below the threshold.)
        int32_t rounded_آستانه = (shifted_آستانه + فاصله_FRACTION_ماسک) >> فاصله_لغزش;
        // We implement "favor subtag" by reducing the language subtag distance
        // (unscientifically reducing it to a quarter of the normal value),
        // so that the script distance is relatively more important.
        // For example, given a default language distance of 80, we reduce it to 20,
        // which is below the default threshold of 50, which is the default script distance.
        إذا (favorSubtag == ULOCMATCH_FAVOR_SCRIPT) {
            فاصله >>= 2;
        }
        // Let distance == roundedThreshold pass until the tie-breaker logic
        // at the end of the loop.
        إذا (فاصله > rounded_آستانه) {
            استمر;
        }

        int32_t script_فاصله;
        إذا (ستاره || flags != 0) {
            إذا (uprv_strcmp(desired.script, supported.script) == 0) {
                script_فاصله = 0;
            } وإلا {
                script_فاصله = default_Script_فاصله;
            }
        } وإلا {
            script_فاصله = get_Des_Supp_Script_فاصله(iter, iter.getState64(),
                    desired.script, supported.script);
            flags = script_فاصله & فاصله_IS_FINAL;
            script_فاصله &= ~فاصله_IS_FINAL;
        }
        فاصله += script_فاصله;
        إذا (فاصله > rounded_آستانه) {
            استمر;
        }

        إذا (uprv_strcmp(desired.region, supported.region) == 0) {
            // regionDistance = 0
        } وإلا إذا (ستاره || (flags & فاصله_IS_FINAL) != 0) {
            فاصله += default_Region_فاصله;
        } وإلا {
            int32_t مانده_آستانه = rounded_آستانه - فاصله;
            إذا (min_Region_فاصله > مانده_آستانه) {
                استمر;
            }

            // From here on we know the regions are not equal.
            // Map each region to zero or more partitions. (zero = one non-matching string)
            // (Each array of single-character partition strings is encoded as one string.)
            // If either side has more than one, then we find the maximum distance.
            // This could be optimized by adding some more structure, but probably not worth it.
            فاصله += get_Region_Partitions_فاصله(
                    iter, iter.getState64(),
                    partitionsForRegion(desired),
                    partitionsForRegion(supported),
                    مانده_آستانه);
        }
        int32_t shifted_فاصله = لغزش_فاصله(فاصله);
        إذا (shifted_فاصله == 0) {
            // Distinguish between equivalent but originally unequal locales via an
            // additional micro distance.
            shifted_فاصله |= (desired.flags ^ supported.flags);
            إذا (shifted_فاصله < shifted_آستانه) {
                إذا (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_تطابق(supported, desired, shifted_آستانه, favorSubtag)) {
                    إذا (shifted_فاصله == 0) {
                        أرجع sl_شاخص << شاخص_لغزش;
                    }
                    بهترین_شاخص = sl_شاخص;
                    shifted_آستانه = shifted_فاصله;
                    بهترین_Likely_Info = -1;
                }
            }
        } وإلا {
            إذا (shifted_فاصله < shifted_آستانه) {
                إذا (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_تطابق(supported, desired, shifted_آستانه, favorSubtag)) {
                    بهترین_شاخص = sl_شاخص;
                    shifted_آستانه = shifted_فاصله;
                    بهترین_Likely_Info = -1;
                }
            } وإلا إذا (shifted_فاصله == shifted_آستانه && بهترین_شاخص >= 0) {
                إذا (direction != ULOCMATCH_DIRECTION_ONLY_TWO_WAY ||
                        // Is there also a match when we swap desired/supported?
                        is_تطابق(supported, desired, shifted_آستانه, favorSubtag)) {
                    بهترین_Likely_Info = likelySubtags.compareLikely(
                            supported, *supportedLSRs[بهترین_شاخص], بهترین_Likely_Info);
                    إذا ((بهترین_Likely_Info & 1) != 0) {
                        // This supported locale matches as well as the previous best match,
                        // and neither matches perfectly,
                        // but this one is "more likely" (has more-default subtags).
                        بهترین_شاخص = sl_شاخص;
                    }
                }
            }
        }
    }
    أرجع بهترین_شاخص >= 0 ?
            (بهترین_شاخص << شاخص_لغزش) | shifted_آستانه :
            شاخص_NEG_1 | لغزش_فاصله(ABOVE_آستانه);
}

int32_t Locale_فاصله::get_Des_Supp_Script_فاصله(
        بایتها_Trie &iter, uint64_t شروع_State, ثابت حرف *desired, ثابت حرف *supported) {
    // Note: The data builder verifies that there are no <*, supported> or <desired, *> rules.
    int32_t فاصله = trie_بعدی(iter, desired, خطأ);
    إذا (فاصله >= 0) {
        فاصله = trie_بعدی(iter, supported, درست);
    }
    إذا (فاصله < 0) {
        UString_Trie_نتیجه نتیجه = iter.reset_مقصد_State64(شروع_State).next(u'*');  // <*, *>
        U_ASSERT(USTRINGTRIE_HAS_مقدار(نتیجه));
        إذا (uprv_strcmp(desired, supported) == 0) {
            فاصله = 0;  // same script
        } وإلا {
            فاصله = iter.get_مقدار();
            U_ASSERT(فاصله >= 0);
        }
        إذا (نتیجه == USTRINGTRIE_FINAL_مقدار) {
            فاصله |= فاصله_IS_FINAL;
        }
    }
    أرجع فاصله;
}

int32_t Locale_فاصله::get_Region_Partitions_فاصله(
        بایتها_Trie &iter, uint64_t شروع_State,
        ثابت حرف *desiredPartitions, ثابت حرف *supportedPartitions, int32_t آستانه) {
    حرف desired = *desiredPartitions++;
    حرف supported = *supportedPartitions++;
    U_ASSERT(desired != 0 && supported != 0);
    // See if we have single desired/supported partitions, from NUL-terminated
    // partition strings without explicit length.
    منطقي supp_درازی_Gt1 = *supportedPartitions != 0;  // gt1: more than 1 character
    // equivalent to: if (desLength == 1 && suppLength == 1)
    إذا (*desiredPartitions == 0 && !supp_درازی_Gt1) {
        // Fastpath for single desired/supported partitions.
        UString_Trie_نتیجه نتیجه = iter.next(uprv_inv_Char_مقصد_Ascii(desired) | انتها_از_SUBTAG);
        إذا (USTRINGTRIE_HAS_بعدی(نتیجه)) {
            نتیجه = iter.next(uprv_inv_Char_مقصد_Ascii(supported) | انتها_از_SUBTAG);
            إذا (USTRINGTRIE_HAS_مقدار(نتیجه)) {
                أرجع iter.get_مقدار();
            }
        }
        أرجع get_Fallback_Region_فاصله(iter, شروع_State);
    }

    ثابت حرف *supported_شروع = supportedPartitions - 1;  // for restart of inner loop
    int32_t region_فاصله = 0;
    // Fall back to * only once, not for each pair of partition strings.
    منطقي ستاره = خطأ;
    لكل (;;) {
        // Look up each desired-partition string only once,
        // not for each (desired, supported) pair.
        UString_Trie_نتیجه نتیجه = iter.next(uprv_inv_Char_مقصد_Ascii(desired) | انتها_از_SUBTAG);
        إذا (USTRINGTRIE_HAS_بعدی(نتیجه)) {
            uint64_t desState = supp_درازی_Gt1 ? iter.getState64() : 0;
            لكل (;;) {
                نتیجه = iter.next(uprv_inv_Char_مقصد_Ascii(supported) | انتها_از_SUBTAG);
                int32_t d;
                إذا (USTRINGTRIE_HAS_مقدار(نتیجه)) {
                    d = iter.get_مقدار();
                } وإلا إذا (ستاره) {
                    d = 0;
                } وإلا {
                    d = get_Fallback_Region_فاصله(iter, شروع_State);
                    ستاره = درست;
                }
                إذا (d > آستانه) {
                    أرجع d;
                } وإلا إذا (region_فاصله < d) {
                    region_فاصله = d;
                }
                إذا ((supported = *supportedPartitions++) != 0) {
                    iter.reset_مقصد_State64(desState);
                } وإلا {
                    اكسر;
                }
            }
        } وإلا إذا (!ستاره) {
            int32_t d = get_Fallback_Region_فاصله(iter, شروع_State);
            إذا (d > آستانه) {
                أرجع d;
            } وإلا إذا (region_فاصله < d) {
                region_فاصله = d;
            }
            ستاره = درست;
        }
        إذا ((desired = *desiredPartitions++) != 0) {
            iter.reset_مقصد_State64(شروع_State);
            supportedPartitions = supported_شروع;
            supported = *supportedPartitions++;
        } وإلا {
            اكسر;
        }
    }
    أرجع region_فاصله;
}

int32_t Locale_فاصله::get_Fallback_Region_فاصله(بایتها_Trie &iter, uint64_t شروع_State) {
#إذا U_DEBUG
    UString_Trie_نتیجه نتیجه =
#پایان_اگر
    iter.reset_مقصد_State64(شروع_State).next(u'*');  // <*, *>
    U_ASSERT(USTRINGTRIE_HAS_مقدار(نتیجه));
    int32_t فاصله = iter.get_مقدار();
    U_ASSERT(فاصله >= 0);
    أرجع فاصله;
}

int32_t Locale_فاصله::trie_بعدی(بایتها_Trie &iter, ثابت حرف *s, منطقي want_مقدار) {
    uint8_t c;
    إذا ((c = *s) == 0) {
        أرجع -1;  // no empty subtags in the distance data
    }
    لكل (;;) {
        c = uprv_inv_Char_مقصد_Ascii(c);
        // EBCDIC: If *s is not an invariant character,
        // then c is now 0 and will simply not match anything, which is harmless.
        uint8_t next = *++s;
        إذا (next != 0) {
            إذا (!USTRINGTRIE_HAS_بعدی(iter.next(c))) {
                أرجع -1;
            }
        } وإلا {
            // last character of this subtag
            UString_Trie_نتیجه نتیجه = iter.next(c | انتها_از_SUBTAG);
            إذا (want_مقدار) {
                إذا (USTRINGTRIE_HAS_مقدار(نتیجه)) {
                    int32_t value = iter.get_مقدار();
                    إذا (نتیجه == USTRINGTRIE_FINAL_مقدار) {
                        value |= فاصله_IS_FINAL;
                    }
                    أرجع value;
                }
            } وإلا {
                إذا (USTRINGTRIE_HAS_بعدی(نتیجه)) {
                    أرجع 0;
                }
            }
            أرجع -1;
        }
        c = next;
    }
}

منطقي Locale_فاصله::isParadigmLSR(ثابت LSR &lsr) ثابت {
    // Linear search for a very short list (length 6 as of 2019),
    // because we look for equivalence not equality, and
    // because it's easy.
    // If there are many paradigm LSRs we should use a hash set
    // with custom comparator and hasher.
    U_ASSERT(paradigm_LSRs_درازی <= 15);
    لكل (int32_t i = 0; i < paradigm_LSRs_درازی; ++i) {
        إذا (lsr.is_Equivalent_مقصد(paradigmLSRs[i])) { أرجع درست; }
    }
    أرجع خطأ;
}

U_NAMESPACE_انتها
