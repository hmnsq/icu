// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  ucharstrie.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010nov14
*   created by: Markus W. Scherer
*/

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/appendable.h"
#అన్తర్భావ "unicode/ucharstrie.h"
#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "uassert.h"

U_NAMESPACE_BEGIN

UCharsTrie::~UCharsTrie() {
    uprv_free(ownedArray_);
}

UString_Trie_ఫలితం
UCharsTrie::current() స్థిరరాశి {
    స్థిరరాశి char16_t *pos=pos_;
    ఒకవేళ(pos==శూన్యసూచిక) {
        తిరిగి USTRINGTRIE_NO_సరిపోలిక;
    } లేకపోతే {
        int32_t కణుపు;
        తిరిగి (మిగిలిన_సరిపోలిక_నిడివి<0 && (కణుపు=*pos)>=k_Min_విలువ_Lead) ?
                విలువ_ఫలితం(కణుపు) : USTRINGTRIE_NO_విలువ;
    }
}

UString_Trie_ఫలితం
UCharsTrie::మొదటి_For_సంకేతం_బిందువు(UChar32 cp) {
    తిరిగి cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_తదుపరి(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_సరిపోలిక);
}

UString_Trie_ఫలితం
UCharsTrie::తదుపరి_For_సంకేతం_బిందువు(UChar32 cp) {
    తిరిగి cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_తదుపరి(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_సరిపోలిక);
}

UString_Trie_ఫలితం
UCharsTrie::branch_తదుపరి(స్థిరరాశి char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    ఒకవేళ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    అయితే(length>k_Max_Branch_రేఖీయ_Sub_కణుపు_నిడివి) {
        ఒకవేళ(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } లేకపోతే {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    చేయి {
        ఒకవేళ(uchar==*pos++) {
            UString_Trie_ఫలితం ఫలితం;
            int32_t కణుపు=*pos;
            ఒకవేళ(కణుపు&k_విలువ_Is_Final) {
                // Leave the final value for getValue() to read.
                ఫలితం=USTRINGTRIE_FINAL_విలువ;
            } లేకపోతే {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                ఒకవేళ(కణుపు<k_Min_Two_Unit_విలువ_Lead) {
                    delta=కణుపు;
                } లేకపోతే ఒకవేళ(కణుపు<k_Three_Unit_విలువ_Lead) {
                    delta=((కణుపు-k_Min_Two_Unit_విలువ_Lead)<<16)|*pos++;
                } లేకపోతే {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                కణుపు=*pos;
                ఫలితం= కణుపు>=k_Min_విలువ_Lead ? విలువ_ఫలితం(కణుపు) : USTRINGTRIE_NO_విలువ;
            }
            pos_=pos;
            తిరిగి ఫలితం;
        }
        --length;
        pos=skip_విలువ(pos);
    } అయితే(length>1);
    ఒకవేళ(uchar==*pos++) {
        pos_=pos;
        int32_t కణుపు=*pos;
        తిరిగి కణుపు>=k_Min_విలువ_Lead ? విలువ_ఫలితం(కణుపు) : USTRINGTRIE_NO_విలువ;
    } లేకపోతే {
        stop();
        తిరిగి USTRINGTRIE_NO_సరిపోలిక;
    }
}

UString_Trie_ఫలితం
UCharsTrie::తదుపరి_Impl(స్థిరరాశి char16_t *pos, int32_t uchar) {
    int32_t కణుపు=*pos++;
    కొరకు(;;) {
        ఒకవేళ(కణుపు<k_Min_రేఖీయ_సరిపోలిక) {
            తిరిగి branch_తదుపరి(pos, కణుపు, uchar);
        } లేకపోతే ఒకవేళ(కణుపు<k_Min_విలువ_Lead) {
            // Match the first of length+1 units.
            int32_t length=కణుపు-k_Min_రేఖీయ_సరిపోలిక;  // Actual match length minus 1.
            ఒకవేళ(uchar==*pos++) {
                మిగిలిన_సరిపోలిక_నిడివి=--length;
                pos_=pos;
                తిరిగి (length<0 && (కణుపు=*pos)>=k_Min_విలువ_Lead) ?
                        విలువ_ఫలితం(కణుపు) : USTRINGTRIE_NO_విలువ;
            } లేకపోతే {
                // No match.
                ఆపు;
            }
        } లేకపోతే ఒకవేళ(కణుపు&k_విలువ_Is_Final) {
            // No further matching units.
            ఆపు;
        } లేకపోతే {
            // Skip intermediate value.
            pos=skip_కణుపు_విలువ(pos, కణుపు);
            కణుపు&=k_కణుపు_Type_ముసుగు;
        }
    }
    stop();
    తిరిగి USTRINGTRIE_NO_సరిపోలిక;
}

UString_Trie_ఫలితం
UCharsTrie::next(int32_t uchar) {
    స్థిరరాశి char16_t *pos=pos_;
    ఒకవేళ(pos==శూన్యసూచిక) {
        తిరిగి USTRINGTRIE_NO_సరిపోలిక;
    }
    int32_t length=మిగిలిన_సరిపోలిక_నిడివి;  // Actual remaining match length minus 1.
    ఒకవేళ(length>=0) {
        // Remaining part of a linear-match node.
        ఒకవేళ(uchar==*pos++) {
            మిగిలిన_సరిపోలిక_నిడివి=--length;
            pos_=pos;
            int32_t కణుపు;
            తిరిగి (length<0 && (కణుపు=*pos)>=k_Min_విలువ_Lead) ?
                    విలువ_ఫలితం(కణుపు) : USTRINGTRIE_NO_విలువ;
        } లేకపోతే {
            stop();
            తిరిగి USTRINGTRIE_NO_సరిపోలిక;
        }
    }
    తిరిగి తదుపరి_Impl(pos, uchar);
}

UString_Trie_ఫలితం
UCharsTrie::next(ConstChar16Ptr ptr, int32_t s_నిడివి) {
    స్థిరరాశి char16_t *s=ptr;
    ఒకవేళ(s_నిడివి<0 ? *s==0 : s_నిడివి==0) {
        // Empty input.
        తిరిగి current();
    }
    స్థిరరాశి char16_t *pos=pos_;
    ఒకవేళ(pos==శూన్యసూచిక) {
        తిరిగి USTRINGTRIE_NO_సరిపోలిక;
    }
    int32_t length=మిగిలిన_సరిపోలిక_నిడివి;  // Actual remaining match length minus 1.
    కొరకు(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        ఒకవేళ(s_నిడివి<0) {
            కొరకు(;;) {
                ఒకవేళ((uchar=*s++)==0) {
                    మిగిలిన_సరిపోలిక_నిడివి=length;
                    pos_=pos;
                    int32_t కణుపు;
                    తిరిగి (length<0 && (కణుపు=*pos)>=k_Min_విలువ_Lead) ?
                            విలువ_ఫలితం(కణుపు) : USTRINGTRIE_NO_విలువ;
                }
                ఒకవేళ(length<0) {
                    మిగిలిన_సరిపోలిక_నిడివి=length;
                    ఆపు;
                }
                ఒకవేళ(uchar!=*pos) {
                    stop();
                    తిరిగి USTRINGTRIE_NO_సరిపోలిక;
                }
                ++pos;
                --length;
            }
        } లేకపోతే {
            కొరకు(;;) {
                ఒకవేళ(s_నిడివి==0) {
                    మిగిలిన_సరిపోలిక_నిడివి=length;
                    pos_=pos;
                    int32_t కణుపు;
                    తిరిగి (length<0 && (కణుపు=*pos)>=k_Min_విలువ_Lead) ?
                            విలువ_ఫలితం(కణుపు) : USTRINGTRIE_NO_విలువ;
                }
                uchar=*s++;
                --s_నిడివి;
                ఒకవేళ(length<0) {
                    మిగిలిన_సరిపోలిక_నిడివి=length;
                    ఆపు;
                }
                ఒకవేళ(uchar!=*pos) {
                    stop();
                    తిరిగి USTRINGTRIE_NO_సరిపోలిక;
                }
                ++pos;
                --length;
            }
        }
        int32_t కణుపు=*pos++;
        కొరకు(;;) {
            ఒకవేళ(కణుపు<k_Min_రేఖీయ_సరిపోలిక) {
                UString_Trie_ఫలితం ఫలితం=branch_తదుపరి(pos, కణుపు, uchar);
                ఒకవేళ(ఫలితం==USTRINGTRIE_NO_సరిపోలిక) {
                    తిరిగి USTRINGTRIE_NO_సరిపోలిక;
                }
                // Fetch the next input unit, if there is one.
                ఒకవేళ(s_నిడివి<0) {
                    ఒకవేళ((uchar=*s++)==0) {
                        తిరిగి ఫలితం;
                    }
                } లేకపోతే {
                    ఒకవేళ(s_నిడివి==0) {
                        తిరిగి ఫలితం;
                    }
                    uchar=*s++;
                    --s_నిడివి;
                }
                ఒకవేళ(ఫలితం==USTRINGTRIE_FINAL_విలువ) {
                    // No further matching units.
                    stop();
                    తిరిగి USTRINGTRIE_NO_సరిపోలిక;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                కణుపు=*pos++;
            } లేకపోతే ఒకవేళ(కణుపు<k_Min_విలువ_Lead) {
                // Match length+1 units.
                length=కణుపు-k_Min_రేఖీయ_సరిపోలిక;  // Actual match length minus 1.
                ఒకవేళ(uchar!=*pos) {
                    stop();
                    తిరిగి USTRINGTRIE_NO_సరిపోలిక;
                }
                ++pos;
                --length;
                ఆపు;
            } లేకపోతే ఒకవేళ(కణుపు&k_విలువ_Is_Final) {
                // No further matching units.
                stop();
                తిరిగి USTRINGTRIE_NO_సరిపోలిక;
            } లేకపోతే {
                // Skip intermediate value.
                pos=skip_కణుపు_విలువ(pos, కణుపు);
                కణుపు&=k_కణుపు_Type_ముసుగు;
            }
        }
    }
}

స్థిరరాశి char16_t*
UCharsTrie::కనుగొను_Unique_విలువ_From_Branch(స్థిరరాశి char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool have_Unique_విలువ,
                                      int32_t& unique_విలువ) {
    అయితే(length>k_Max_Branch_రేఖీయ_Sub_కణుపు_నిడివి) {
        ++pos;  // ignore the comparison unit
        ఒకవేళ(శూన్యసూచిక==కనుగొను_Unique_విలువ_From_Branch(jumpByDelta(pos), length>>1, have_Unique_విలువ, unique_విలువ)) {
            తిరిగి శూన్యసూచిక;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    చేయి {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t కణుపు=*pos++;
        UBool isFinal = static_cast<UBool>(కణుపు >> 15);
        కణుపు&=0x7fff;
        int32_t value=read_విలువ(pos, కణుపు);
        pos=skip_విలువ(pos, కణుపు);
        ఒకవేళ(isFinal) {
            ఒకవేళ(have_Unique_విలువ) {
                ఒకవేళ(value!=unique_విలువ) {
                    తిరిగి శూన్యసూచిక;
                }
            } లేకపోతే {
                unique_విలువ=value;
                have_Unique_విలువ=నిజం;
            }
        } లేకపోతే {
            ఒకవేళ(!కనుగొను_Unique_విలువ(pos+value, have_Unique_విలువ, unique_విలువ)) {
                తిరిగి శూన్యసూచిక;
            }
            have_Unique_విలువ=నిజం;
        }
    } అయితే(--length>1);
    తిరిగి pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::కనుగొను_Unique_విలువ(స్థిరరాశి char16_t *pos, UBool have_Unique_విలువ, int32_t &unique_విలువ) {
    int32_t కణుపు=*pos++;
    కొరకు(;;) {
        ఒకవేళ(కణుపు<k_Min_రేఖీయ_సరిపోలిక) {
            ఒకవేళ(కణుపు==0) {
                కణుపు=*pos++;
            }
            pos=కనుగొను_Unique_విలువ_From_Branch(pos, కణుపు+1, have_Unique_విలువ, unique_విలువ);
            ఒకవేళ(pos==శూన్యసూచిక) {
                తిరిగి అబద్ధం;
            }
            have_Unique_విలువ=నిజం;
            కణుపు=*pos++;
        } లేకపోతే ఒకవేళ(కణుపు<k_Min_విలువ_Lead) {
            // linear-match node
            pos+=కణుపు-k_Min_రేఖీయ_సరిపోలిక+1;  // Ignore the match units.
            కణుపు=*pos++;
        } లేకపోతే {
            UBool isFinal = static_cast<UBool>(కణుపు >> 15);
            int32_t value;
            ఒకవేళ(isFinal) {
                value=read_విలువ(pos, కణుపు&0x7fff);
            } లేకపోతే {
                value=read_కణుపు_విలువ(pos, కణుపు);
            }
            ఒకవేళ(have_Unique_విలువ) {
                ఒకవేళ(value!=unique_విలువ) {
                    తిరిగి అబద్ధం;
                }
            } లేకపోతే {
                unique_విలువ=value;
                have_Unique_విలువ=నిజం;
            }
            ఒకవేళ(isFinal) {
                తిరిగి నిజం;
            }
            pos=skip_కణుపు_విలువ(pos, కణుపు);
            కణుపు&=k_కణుపు_Type_ముసుగు;
        }
    }
}

int32_t
UCharsTrie::get_తదుపరి_UChars(Appendable &out) స్థిరరాశి {
    స్థిరరాశి char16_t *pos=pos_;
    ఒకవేళ(pos==శూన్యసూచిక) {
        తిరిగి 0;
    }
    ఒకవేళ(మిగిలిన_సరిపోలిక_నిడివి>=0) {
        out.append_సంకేతం_Unit(*pos);  // Next unit of a pending linear-match node.
        తిరిగి 1;
    }
    int32_t కణుపు=*pos++;
    ఒకవేళ(కణుపు>=k_Min_విలువ_Lead) {
        ఒకవేళ(కణుపు&k_విలువ_Is_Final) {
            తిరిగి 0;
        } లేకపోతే {
            pos=skip_కణుపు_విలువ(pos, కణుపు);
            కణుపు&=k_కణుపు_Type_ముసుగు;
        }
    }
    ఒకవేళ(కణుపు<k_Min_రేఖీయ_సరిపోలిక) {
        ఒకవేళ(కణుపు==0) {
            కణుపు=*pos++;
        }
        out.reserve_Append_సామర్థ్యం(++కణుపు);
        get_తదుపరి_Branch_UChars(pos, కణుపు, out);
        తిరిగి కణుపు;
    } లేకపోతే {
        // First unit of the linear-match node.
        out.append_సంకేతం_Unit(*pos);
        తిరిగి 1;
    }
}

శూన్య
UCharsTrie::get_తదుపరి_Branch_UChars(స్థిరరాశి char16_t *pos, int32_t length, Appendable &out) {
    అయితే(length>k_Max_Branch_రేఖీయ_Sub_కణుపు_నిడివి) {
        ++pos;  // ignore the comparison unit
        get_తదుపరి_Branch_UChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    చేయి {
        out.append_సంకేతం_Unit(*pos++);
        pos=skip_విలువ(pos);
    } అయితే(--length>1);
    out.append_సంకేతం_Unit(*pos);
}

U_NAMESPACE_చివర
