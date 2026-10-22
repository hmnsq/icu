// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  bytestrie.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010sep25
*   created by: Markus W. Scherer
*/

#समावेश "unicode/utypes.h"
#समावेश "unicode/bytestream.h"
#समावेश "unicode/bytestrie.h"
#समावेश "unicode/uobject.h"
#समावेश "cmemory.h"
#समावेश "uassert.h"

U_NAMESPACE_BEGIN

BytesTrie::~BytesTrie() {
    uprv_free(ownedArray_);
}

// lead byte already shifted right by 1.
int32_t
BytesTrie::read_मूल्य(स्थिर uint8_t *pos, int32_t leadByte) {
    int32_t value;
    अगर(leadByte<k_Min_Two_Byte_मूल्य_Lead) {
        value=leadByte-k_Min_One_Byte_मूल्य_Lead;
    } अन्यथा अगर(leadByte<k_Min_Three_Byte_मूल्य_Lead) {
        value=((leadByte-k_Min_Two_Byte_मूल्य_Lead)<<8)|*pos;
    } अन्यथा अगर(leadByte<k_Four_Byte_मूल्य_Lead) {
        value=((leadByte-k_Min_Three_Byte_मूल्य_Lead)<<16)|(pos[0]<<8)|pos[1];
    } अन्यथा अगर(leadByte==k_Four_Byte_मूल्य_Lead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } अन्यथा {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    वापस value;
}

स्थिर uint8_t*
BytesTrie::jumpByDelta(स्थिर uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    अगर(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } अन्यथा अगर(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } अन्यथा अगर(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } अन्यथा अगर(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } अन्यथा {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    वापस pos+delta;
}

UString_Trie_परिणाम
BytesTrie::current() स्थिर {
    स्थिर uint8_t *pos=pos_;
    अगर(pos==nullptr) {
        वापस USTRINGTRIE_NO_MATCH;
    } अन्यथा {
        int32_t गाँठ;
        वापस (remaining_Match_लंबाई<0 && (गाँठ=*pos)>=k_Min_मूल्य_Lead) ?
                मूल्य_परिणाम(गाँठ) : USTRINGTRIE_NO_मूल्य;
    }
}

UString_Trie_परिणाम
BytesTrie::branch_अग्र(स्थिर uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    अगर(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    जबतक(length>k_Max_Branch_Linear_Sub_गाँठ_लंबाई) {
        अगर(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } अन्यथा {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    करो {
        अगर(inByte==*pos++) {
            UString_Trie_परिणाम परिणाम;
            int32_t गाँठ=*pos;
            U_ASSERT(गाँठ>=k_Min_मूल्य_Lead);
            अगर(गाँठ&k_मूल्य_Is_Final) {
                // Leave the final value for getValue() to read.
                परिणाम=USTRINGTRIE_FINAL_मूल्य;
            } अन्यथा {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                गाँठ>>=1;
                int32_t delta;
                अगर(गाँठ<k_Min_Two_Byte_मूल्य_Lead) {
                    delta=गाँठ-k_Min_One_Byte_मूल्य_Lead;
                } अन्यथा अगर(गाँठ<k_Min_Three_Byte_मूल्य_Lead) {
                    delta=((गाँठ-k_Min_Two_Byte_मूल्य_Lead)<<8)|*pos++;
                } अन्यथा अगर(गाँठ<k_Four_Byte_मूल्य_Lead) {
                    delta=((गाँठ-k_Min_Three_Byte_मूल्य_Lead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } अन्यथा अगर(गाँठ==k_Four_Byte_मूल्य_Lead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } अन्यथा {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                गाँठ=*pos;
                परिणाम= गाँठ>=k_Min_मूल्य_Lead ? मूल्य_परिणाम(गाँठ) : USTRINGTRIE_NO_मूल्य;
            }
            pos_=pos;
            वापस परिणाम;
        }
        --length;
        pos=skip_मूल्य(pos);
    } जबतक(length>1);
    अगर(inByte==*pos++) {
        pos_=pos;
        int32_t गाँठ=*pos;
        वापस गाँठ>=k_Min_मूल्य_Lead ? मूल्य_परिणाम(गाँठ) : USTRINGTRIE_NO_मूल्य;
    } अन्यथा {
        stop();
        वापस USTRINGTRIE_NO_MATCH;
    }
}

UString_Trie_परिणाम
BytesTrie::अग्र_Impl(स्थिर uint8_t *pos, int32_t inByte) {
    क्रम(;;) {
        int32_t गाँठ=*pos++;
        अगर(गाँठ<kMinLinearMatch) {
            वापस branch_अग्र(pos, गाँठ, inByte);
        } अन्यथा अगर(गाँठ<k_Min_मूल्य_Lead) {
            // Match the first of length+1 bytes.
            int32_t length=गाँठ-kMinLinearMatch;  // Actual match length minus 1.
            अगर(inByte==*pos++) {
                remaining_Match_लंबाई=--length;
                pos_=pos;
                वापस (length<0 && (गाँठ=*pos)>=k_Min_मूल्य_Lead) ?
                        मूल्य_परिणाम(गाँठ) : USTRINGTRIE_NO_मूल्य;
            } अन्यथा {
                // No match.
                अवरोध;
            }
        } अन्यथा अगर(गाँठ&k_मूल्य_Is_Final) {
            // No further matching bytes.
            अवरोध;
        } अन्यथा {
            // Skip intermediate value.
            pos=skip_मूल्य(pos, गाँठ);
            // The next node must not also be a value node.
            U_ASSERT(*pos<k_Min_मूल्य_Lead);
        }
    }
    stop();
    वापस USTRINGTRIE_NO_MATCH;
}

UString_Trie_परिणाम
BytesTrie::next(int32_t inByte) {
    स्थिर uint8_t *pos=pos_;
    अगर(pos==nullptr) {
        वापस USTRINGTRIE_NO_MATCH;
    }
    अगर(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remaining_Match_लंबाई;  // Actual remaining match length minus 1.
    अगर(length>=0) {
        // Remaining part of a linear-match node.
        अगर(inByte==*pos++) {
            remaining_Match_लंबाई=--length;
            pos_=pos;
            int32_t गाँठ;
            वापस (length<0 && (गाँठ=*pos)>=k_Min_मूल्य_Lead) ?
                    मूल्य_परिणाम(गाँठ) : USTRINGTRIE_NO_मूल्य;
        } अन्यथा {
            stop();
            वापस USTRINGTRIE_NO_MATCH;
        }
    }
    वापस अग्र_Impl(pos, inByte);
}

UString_Trie_परिणाम
BytesTrie::next(स्थिर अक्षर *s, int32_t s_लंबाई) {
    अगर(s_लंबाई<0 ? *s==0 : s_लंबाई==0) {
        // Empty input.
        वापस current();
    }
    स्थिर uint8_t *pos=pos_;
    अगर(pos==nullptr) {
        वापस USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remaining_Match_लंबाई;  // Actual remaining match length minus 1.
    क्रम(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        अगर(s_लंबाई<0) {
            क्रम(;;) {
                अगर((inByte=*s++)==0) {
                    remaining_Match_लंबाई=length;
                    pos_=pos;
                    int32_t गाँठ;
                    वापस (length<0 && (गाँठ=*pos)>=k_Min_मूल्य_Lead) ?
                            मूल्य_परिणाम(गाँठ) : USTRINGTRIE_NO_मूल्य;
                }
                अगर(length<0) {
                    remaining_Match_लंबाई=length;
                    अवरोध;
                }
                अगर(inByte!=*pos) {
                    stop();
                    वापस USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } अन्यथा {
            क्रम(;;) {
                अगर(s_लंबाई==0) {
                    remaining_Match_लंबाई=length;
                    pos_=pos;
                    int32_t गाँठ;
                    वापस (length<0 && (गाँठ=*pos)>=k_Min_मूल्य_Lead) ?
                            मूल्य_परिणाम(गाँठ) : USTRINGTRIE_NO_मूल्य;
                }
                inByte=*s++;
                --s_लंबाई;
                अगर(length<0) {
                    remaining_Match_लंबाई=length;
                    अवरोध;
                }
                अगर(inByte!=*pos) {
                    stop();
                    वापस USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        क्रम(;;) {
            int32_t गाँठ=*pos++;
            अगर(गाँठ<kMinLinearMatch) {
                UString_Trie_परिणाम परिणाम=branch_अग्र(pos, गाँठ, inByte);
                अगर(परिणाम==USTRINGTRIE_NO_MATCH) {
                    वापस USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                अगर(s_लंबाई<0) {
                    अगर((inByte=*s++)==0) {
                        वापस परिणाम;
                    }
                } अन्यथा {
                    अगर(s_लंबाई==0) {
                        वापस परिणाम;
                    }
                    inByte=*s++;
                    --s_लंबाई;
                }
                अगर(परिणाम==USTRINGTRIE_FINAL_मूल्य) {
                    // No further matching bytes.
                    stop();
                    वापस USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } अन्यथा अगर(गाँठ<k_Min_मूल्य_Lead) {
                // Match length+1 bytes.
                length=गाँठ-kMinLinearMatch;  // Actual match length minus 1.
                अगर(inByte!=*pos) {
                    stop();
                    वापस USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                अवरोध;
            } अन्यथा अगर(गाँठ&k_मूल्य_Is_Final) {
                // No further matching bytes.
                stop();
                वापस USTRINGTRIE_NO_MATCH;
            } अन्यथा {
                // Skip intermediate value.
                pos=skip_मूल्य(pos, गाँठ);
                // The next node must not also be a value node.
                U_ASSERT(*pos<k_Min_मूल्य_Lead);
            }
        }
    }
}

स्थिर uint8_t*
BytesTrie::find_Unique_मूल्य_From_Branch(स्थिर uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool have_Unique_मूल्य,
                                     int32_t& unique_मूल्य) {
    जबतक(length>k_Max_Branch_Linear_Sub_गाँठ_लंबाई) {
        ++pos;  // ignore the comparison byte
        अगर(nullptr==find_Unique_मूल्य_From_Branch(jumpByDelta(pos), length>>1, have_Unique_मूल्य, unique_मूल्य)) {
            वापस nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    करो {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t गाँठ=*pos++;
        UBool isFinal = static_cast<UBool>(गाँठ & k_मूल्य_Is_Final);
        int32_t value=read_मूल्य(pos, गाँठ>>1);
        pos=skip_मूल्य(pos, गाँठ);
        अगर(isFinal) {
            अगर(have_Unique_मूल्य) {
                अगर(value!=unique_मूल्य) {
                    वापस nullptr;
                }
            } अन्यथा {
                unique_मूल्य=value;
                have_Unique_मूल्य=true;
            }
        } अन्यथा {
            अगर(!find_Unique_मूल्य(pos+value, have_Unique_मूल्य, unique_मूल्य)) {
                वापस nullptr;
            }
            have_Unique_मूल्य=true;
        }
    } जबतक(--length>1);
    वापस pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::find_Unique_मूल्य(स्थिर uint8_t *pos, UBool have_Unique_मूल्य, int32_t &unique_मूल्य) {
    क्रम(;;) {
        int32_t गाँठ=*pos++;
        अगर(गाँठ<kMinLinearMatch) {
            अगर(गाँठ==0) {
                गाँठ=*pos++;
            }
            pos=find_Unique_मूल्य_From_Branch(pos, गाँठ+1, have_Unique_मूल्य, unique_मूल्य);
            अगर(pos==nullptr) {
                वापस false;
            }
            have_Unique_मूल्य=true;
        } अन्यथा अगर(गाँठ<k_Min_मूल्य_Lead) {
            // linear-match node
            pos+=गाँठ-kMinLinearMatch+1;  // Ignore the match bytes.
        } अन्यथा {
            UBool isFinal = static_cast<UBool>(गाँठ & k_मूल्य_Is_Final);
            int32_t value=read_मूल्य(pos, गाँठ>>1);
            अगर(have_Unique_मूल्य) {
                अगर(value!=unique_मूल्य) {
                    वापस false;
                }
            } अन्यथा {
                unique_मूल्य=value;
                have_Unique_मूल्य=true;
            }
            अगर(isFinal) {
                वापस true;
            }
            pos=skip_मूल्य(pos, गाँठ);
        }
    }
}

int32_t
BytesTrie::get_अग्र_Bytes(ByteSink &out) स्थिर {
    स्थिर uint8_t *pos=pos_;
    अगर(pos==nullptr) {
        वापस 0;
    }
    अगर(remaining_Match_लंबाई>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        वापस 1;
    }
    int32_t गाँठ=*pos++;
    अगर(गाँठ>=k_Min_मूल्य_Lead) {
        अगर(गाँठ&k_मूल्य_Is_Final) {
            वापस 0;
        } अन्यथा {
            pos=skip_मूल्य(pos, गाँठ);
            गाँठ=*pos++;
            U_ASSERT(गाँठ<k_Min_मूल्य_Lead);
        }
    }
    अगर(गाँठ<kMinLinearMatch) {
        अगर(गाँठ==0) {
            गाँठ=*pos++;
        }
        get_अग्र_Branch_Bytes(pos, ++गाँठ, out);
        वापस गाँठ;
    } अन्यथा {
        // First byte of the linear-match node.
        append(out, *pos);
        वापस 1;
    }
}

व्योम
BytesTrie::get_अग्र_Branch_Bytes(स्थिर uint8_t *pos, int32_t length, ByteSink &out) {
    जबतक(length>k_Max_Branch_Linear_Sub_गाँठ_लंबाई) {
        ++pos;  // ignore the comparison byte
        get_अग्र_Branch_Bytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    करो {
        append(out, *pos++);
        pos=skip_मूल्य(pos);
    } जबतक(--length>1);
    append(out, *pos);
}

व्योम
BytesTrie::append(ByteSink &out, पूर्णांक c) {
    अक्षर ch = static_cast<अक्षर>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
