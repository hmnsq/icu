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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/bytestream.h"
#અન્તર્ભાવ "unicode/bytestrie.h"
#અન્તર્ભાવ "unicode/uobject.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "uassert.h"

U_NAMESPACE_BEGIN

BytesTrie::~BytesTrie() {
    uprv_free(ownedArray_);
}

// lead byte already shifted right by 1.
int32_t
BytesTrie::read_કિંમત(અચલ uint8_t *pos, int32_t leadByte) {
    int32_t value;
    જો(leadByte<k_Min_Two_Byte_કિંમત_Lead) {
        value=leadByte-k_Min_One_Byte_કિંમત_Lead;
    } નહીંતર જો(leadByte<k_Min_Three_Byte_કિંમત_Lead) {
        value=((leadByte-k_Min_Two_Byte_કિંમત_Lead)<<8)|*pos;
    } નહીંતર જો(leadByte<k_Four_Byte_કિંમત_Lead) {
        value=((leadByte-k_Min_Three_Byte_કિંમત_Lead)<<16)|(pos[0]<<8)|pos[1];
    } નહીંતર જો(leadByte==k_Four_Byte_કિંમત_Lead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } નહીંતર {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    પાછા value;
}

અચલ uint8_t*
BytesTrie::jumpByDelta(અચલ uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    જો(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } નહીંતર જો(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } નહીંતર જો(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } નહીંતર જો(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } નહીંતર {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    પાછા pos+delta;
}

UStringTrieResult
BytesTrie::current() અચલ {
    અચલ uint8_t *pos=pos_;
    જો(pos==nullptr) {
        પાછા USTRINGTRIE_NO_MATCH;
    } નહીંતર {
        int32_t ગાંઠ;
        પાછા (remainingMatchLength_<0 && (ગાંઠ=*pos)>=k_Min_કિંમત_Lead) ?
                કિંમત_Result(ગાંઠ) : USTRINGTRIE_NO_કિંમત;
    }
}

UStringTrieResult
BytesTrie::branchNext(અચલ uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    જો(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    જ્યારે(length>k_Max_Branch_Linear_Sub_ગાંઠ_Length) {
        જો(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } નહીંતર {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    કરો {
        જો(inByte==*pos++) {
            UStringTrieResult result;
            int32_t ગાંઠ=*pos;
            U_ASSERT(ગાંઠ>=k_Min_કિંમત_Lead);
            જો(ગાંઠ&k_કિંમત_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_કિંમત;
            } નહીંતર {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                ગાંઠ>>=1;
                int32_t delta;
                જો(ગાંઠ<k_Min_Two_Byte_કિંમત_Lead) {
                    delta=ગાંઠ-k_Min_One_Byte_કિંમત_Lead;
                } નહીંતર જો(ગાંઠ<k_Min_Three_Byte_કિંમત_Lead) {
                    delta=((ગાંઠ-k_Min_Two_Byte_કિંમત_Lead)<<8)|*pos++;
                } નહીંતર જો(ગાંઠ<k_Four_Byte_કિંમત_Lead) {
                    delta=((ગાંઠ-k_Min_Three_Byte_કિંમત_Lead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } નહીંતર જો(ગાંઠ==k_Four_Byte_કિંમત_Lead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } નહીંતર {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                ગાંઠ=*pos;
                result= ગાંઠ>=k_Min_કિંમત_Lead ? કિંમત_Result(ગાંઠ) : USTRINGTRIE_NO_કિંમત;
            }
            pos_=pos;
            પાછા result;
        }
        --length;
        pos=skip_કિંમત(pos);
    } જ્યારે(length>1);
    જો(inByte==*pos++) {
        pos_=pos;
        int32_t ગાંઠ=*pos;
        પાછા ગાંઠ>=k_Min_કિંમત_Lead ? કિંમત_Result(ગાંઠ) : USTRINGTRIE_NO_કિંમત;
    } નહીંતર {
        stop();
        પાછા USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(અચલ uint8_t *pos, int32_t inByte) {
    માટે(;;) {
        int32_t ગાંઠ=*pos++;
        જો(ગાંઠ<kMinLinearMatch) {
            પાછા branchNext(pos, ગાંઠ, inByte);
        } નહીંતર જો(ગાંઠ<k_Min_કિંમત_Lead) {
            // Match the first of length+1 bytes.
            int32_t length=ગાંઠ-kMinLinearMatch;  // Actual match length minus 1.
            જો(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                પાછા (length<0 && (ગાંઠ=*pos)>=k_Min_કિંમત_Lead) ?
                        કિંમત_Result(ગાંઠ) : USTRINGTRIE_NO_કિંમત;
            } નહીંતર {
                // No match.
                તોડો;
            }
        } નહીંતર જો(ગાંઠ&k_કિંમત_Is_Final) {
            // No further matching bytes.
            તોડો;
        } નહીંતર {
            // Skip intermediate value.
            pos=skip_કિંમત(pos, ગાંઠ);
            // The next node must not also be a value node.
            U_ASSERT(*pos<k_Min_કિંમત_Lead);
        }
    }
    stop();
    પાછા USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    અચલ uint8_t *pos=pos_;
    જો(pos==nullptr) {
        પાછા USTRINGTRIE_NO_MATCH;
    }
    જો(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    જો(length>=0) {
        // Remaining part of a linear-match node.
        જો(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t ગાંઠ;
            પાછા (length<0 && (ગાંઠ=*pos)>=k_Min_કિંમત_Lead) ?
                    કિંમત_Result(ગાંઠ) : USTRINGTRIE_NO_કિંમત;
        } નહીંતર {
            stop();
            પાછા USTRINGTRIE_NO_MATCH;
        }
    }
    પાછા nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(અચલ અક્ષર *s, int32_t sLength) {
    જો(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        પાછા current();
    }
    અચલ uint8_t *pos=pos_;
    જો(pos==nullptr) {
        પાછા USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    માટે(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        જો(sLength<0) {
            માટે(;;) {
                જો((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t ગાંઠ;
                    પાછા (length<0 && (ગાંઠ=*pos)>=k_Min_કિંમત_Lead) ?
                            કિંમત_Result(ગાંઠ) : USTRINGTRIE_NO_કિંમત;
                }
                જો(length<0) {
                    remainingMatchLength_=length;
                    તોડો;
                }
                જો(inByte!=*pos) {
                    stop();
                    પાછા USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } નહીંતર {
            માટે(;;) {
                જો(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t ગાંઠ;
                    પાછા (length<0 && (ગાંઠ=*pos)>=k_Min_કિંમત_Lead) ?
                            કિંમત_Result(ગાંઠ) : USTRINGTRIE_NO_કિંમત;
                }
                inByte=*s++;
                --sLength;
                જો(length<0) {
                    remainingMatchLength_=length;
                    તોડો;
                }
                જો(inByte!=*pos) {
                    stop();
                    પાછા USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        માટે(;;) {
            int32_t ગાંઠ=*pos++;
            જો(ગાંઠ<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, ગાંઠ, inByte);
                જો(result==USTRINGTRIE_NO_MATCH) {
                    પાછા USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                જો(sLength<0) {
                    જો((inByte=*s++)==0) {
                        પાછા result;
                    }
                } નહીંતર {
                    જો(sLength==0) {
                        પાછા result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                જો(result==USTRINGTRIE_FINAL_કિંમત) {
                    // No further matching bytes.
                    stop();
                    પાછા USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } નહીંતર જો(ગાંઠ<k_Min_કિંમત_Lead) {
                // Match length+1 bytes.
                length=ગાંઠ-kMinLinearMatch;  // Actual match length minus 1.
                જો(inByte!=*pos) {
                    stop();
                    પાછા USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                તોડો;
            } નહીંતર જો(ગાંઠ&k_કિંમત_Is_Final) {
                // No further matching bytes.
                stop();
                પાછા USTRINGTRIE_NO_MATCH;
            } નહીંતર {
                // Skip intermediate value.
                pos=skip_કિંમત(pos, ગાંઠ);
                // The next node must not also be a value node.
                U_ASSERT(*pos<k_Min_કિંમત_Lead);
            }
        }
    }
}

અચલ uint8_t*
BytesTrie::find_Unique_કિંમત_From_Branch(અચલ uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool have_Unique_કિંમત,
                                     int32_t& unique_કિંમત) {
    જ્યારે(length>k_Max_Branch_Linear_Sub_ગાંઠ_Length) {
        ++pos;  // ignore the comparison byte
        જો(nullptr==find_Unique_કિંમત_From_Branch(jumpByDelta(pos), length>>1, have_Unique_કિંમત, unique_કિંમત)) {
            પાછા nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    કરો {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t ગાંઠ=*pos++;
        UBool isFinal = static_cast<UBool>(ગાંઠ & k_કિંમત_Is_Final);
        int32_t value=read_કિંમત(pos, ગાંઠ>>1);
        pos=skip_કિંમત(pos, ગાંઠ);
        જો(isFinal) {
            જો(have_Unique_કિંમત) {
                જો(value!=unique_કિંમત) {
                    પાછા nullptr;
                }
            } નહીંતર {
                unique_કિંમત=value;
                have_Unique_કિંમત=સાચું;
            }
        } નહીંતર {
            જો(!find_Unique_કિંમત(pos+value, have_Unique_કિંમત, unique_કિંમત)) {
                પાછા nullptr;
            }
            have_Unique_કિંમત=સાચું;
        }
    } જ્યારે(--length>1);
    પાછા pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::find_Unique_કિંમત(અચલ uint8_t *pos, UBool have_Unique_કિંમત, int32_t &unique_કિંમત) {
    માટે(;;) {
        int32_t ગાંઠ=*pos++;
        જો(ગાંઠ<kMinLinearMatch) {
            જો(ગાંઠ==0) {
                ગાંઠ=*pos++;
            }
            pos=find_Unique_કિંમત_From_Branch(pos, ગાંઠ+1, have_Unique_કિંમત, unique_કિંમત);
            જો(pos==nullptr) {
                પાછા ખોટું;
            }
            have_Unique_કિંમત=સાચું;
        } નહીંતર જો(ગાંઠ<k_Min_કિંમત_Lead) {
            // linear-match node
            pos+=ગાંઠ-kMinLinearMatch+1;  // Ignore the match bytes.
        } નહીંતર {
            UBool isFinal = static_cast<UBool>(ગાંઠ & k_કિંમત_Is_Final);
            int32_t value=read_કિંમત(pos, ગાંઠ>>1);
            જો(have_Unique_કિંમત) {
                જો(value!=unique_કિંમત) {
                    પાછા ખોટું;
                }
            } નહીંતર {
                unique_કિંમત=value;
                have_Unique_કિંમત=સાચું;
            }
            જો(isFinal) {
                પાછા સાચું;
            }
            pos=skip_કિંમત(pos, ગાંઠ);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) અચલ {
    અચલ uint8_t *pos=pos_;
    જો(pos==nullptr) {
        પાછા 0;
    }
    જો(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        પાછા 1;
    }
    int32_t ગાંઠ=*pos++;
    જો(ગાંઠ>=k_Min_કિંમત_Lead) {
        જો(ગાંઠ&k_કિંમત_Is_Final) {
            પાછા 0;
        } નહીંતર {
            pos=skip_કિંમત(pos, ગાંઠ);
            ગાંઠ=*pos++;
            U_ASSERT(ગાંઠ<k_Min_કિંમત_Lead);
        }
    }
    જો(ગાંઠ<kMinLinearMatch) {
        જો(ગાંઠ==0) {
            ગાંઠ=*pos++;
        }
        getNextBranchBytes(pos, ++ગાંઠ, out);
        પાછા ગાંઠ;
    } નહીંતર {
        // First byte of the linear-match node.
        append(out, *pos);
        પાછા 1;
    }
}

શૂન્ય
BytesTrie::getNextBranchBytes(અચલ uint8_t *pos, int32_t length, ByteSink &out) {
    જ્યારે(length>k_Max_Branch_Linear_Sub_ગાંઠ_Length) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    કરો {
        append(out, *pos++);
        pos=skip_કિંમત(pos);
    } જ્યારે(--length>1);
    append(out, *pos);
}

શૂન્ય
BytesTrie::append(ByteSink &out, પૂર્ણાંક c) {
    અક્ષર ch = static_cast<અક્ષર>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
