// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ਅਨ੍ਤਰ੍ਭਾਵ <complex>
#ਅਨ੍ਤਰ੍ਭਾਵ <utility>

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

#ਅਨ੍ਤਰ੍ਭਾਵ "brkeng.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "lstmbe.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "putilimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ubrkimpl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uresimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvectr32.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvector.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/brkiter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/resbund.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ubrk.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf.h"

U_NAMESPACE_BEGIN

// Uncomment the following #define to debug.
// #define LSTM_DEBUG 1
// #define LSTM_VECTORIZER_DEBUG 1

/**
 * Interface for reading 1D array.
 */
ਵਰਗ ReadArray1D {
ਜਨਤਕ:
    ਆਭਾਸੀ ~ReadArray1D();
    ਆਭਾਸੀ int32_t d1() ਅਚਲ = 0;
    ਆਭਾਸੀ ਦਸ਼ਮਲਵ get(int32_t i) ਅਚਲ = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LSTM_DEBUG
    ਖਾਲੀ print() ਅਚਲ {
        ਲਿਖੋ("\n[");
        ਲਈ (int32_t i = 0; i < d1(); i++) {
           ਲਿਖੋ("%0.8e ", get(i));
           ਜੇ (i % 4 == 3) ਲਿਖੋ("\n");
        }
        ਲਿਖੋ("]\n");
    }
#ਜੇ_ਅੰਤ
};

ReadArray1D::~ReadArray1D()
{
}

/**
 * Interface for reading 2D array.
 */
ਵਰਗ ReadArray2D {
ਜਨਤਕ:
    ਆਭਾਸੀ ~ReadArray2D();
    ਆਭਾਸੀ int32_t d1() ਅਚਲ = 0;
    ਆਭਾਸੀ int32_t d2() ਅਚਲ = 0;
    ਆਭਾਸੀ ਦਸ਼ਮਲਵ get(int32_t i, int32_t j) ਅਚਲ = 0;
};

ReadArray2D::~ReadArray2D()
{
}

/**
 * A class to index a float array as a 1D Array without owning the pointer or
 * copy the data.
 */
ਵਰਗ ConstArray1D : ਜਨਤਕ ReadArray1D {
ਜਨਤਕ:
    ConstArray1D() : ਅੰਕੜੇ_2(ਖਾਲੀ_ਸੂਚਕ), d1_(0) {}

    ConstArray1D(ਅਚਲ ਦਸ਼ਮਲਵ* ਅੰਕੜੇ, int32_t d1) : ਅੰਕੜੇ_2(ਅੰਕੜੇ), d1_(d1) {}

    ਆਭਾਸੀ ~ConstArray1D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    ਖਾਲੀ init(ਅਚਲ int32_t* ਅੰਕੜੇ, int32_t d1) {
        U_ASSERT(IEEE_754 == 1);
        ਅੰਕੜੇ_2 = reinterpret_cast<ਅਚਲ ਦਸ਼ਮਲਵ*>(ਅੰਕੜੇ);
        d1_ = d1;
    }

    // ReadArray1D methods.
    ਆਭਾਸੀ int32_t d1() ਅਚਲ override { ਵਾਪਸ d1_; }
    ਆਭਾਸੀ ਦਸ਼ਮਲਵ get(int32_t i) ਅਚਲ override {
        U_ASSERT(i < d1_);
        ਵਾਪਸ ਅੰਕੜੇ_2[i];
    }

ਨਿਜੀ:
    ਅਚਲ ਦਸ਼ਮਲਵ* ਅੰਕੜੇ_2;
    int32_t d1_;
};

ConstArray1D::~ConstArray1D()
{
}

/**
 * A class to index a float array as a 2D Array without owning the pointer or
 * copy the data.
 */
ਵਰਗ ConstArray2D : ਜਨਤਕ ReadArray2D {
ਜਨਤਕ:
    ConstArray2D() : ਅੰਕੜੇ_2(ਖਾਲੀ_ਸੂਚਕ), d1_(0), d2_(0) {}

    ConstArray2D(ਅਚਲ ਦਸ਼ਮਲਵ* ਅੰਕੜੇ, int32_t d1, int32_t d2)
        : ਅੰਕੜੇ_2(ਅੰਕੜੇ), d1_(d1), d2_(d2) {}

    ਆਭਾਸੀ ~ConstArray2D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    ਖਾਲੀ init(ਅਚਲ int32_t* ਅੰਕੜੇ, int32_t d1, int32_t d2) {
        U_ASSERT(IEEE_754 == 1);
        ਅੰਕੜੇ_2 = reinterpret_cast<ਅਚਲ ਦਸ਼ਮਲਵ*>(ਅੰਕੜੇ);
        d1_ = d1;
        d2_ = d2;
    }

    // ReadArray2D methods.
    ਸਤਰਅੰਦਰ int32_t d1() ਅਚਲ override { ਵਾਪਸ d1_; }
    ਸਤਰਅੰਦਰ int32_t d2() ਅਚਲ override { ਵਾਪਸ d2_; }
    ਦਸ਼ਮਲਵ get(int32_t i, int32_t j) ਅਚਲ override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        ਵਾਪਸ ਅੰਕੜੇ_2[i * d2_ + j];
    }

    // Expose the ith row as a ConstArray1D
    ਸਤਰਅੰਦਰ ConstArray1D ਪੰਕਤੀ(int32_t i) ਅਚਲ {
        U_ASSERT(i < d1_);
        ਵਾਪਸ ConstArray1D(ਅੰਕੜੇ_2 + i * d2_, d2_);
    }

ਨਿਜੀ:
    ਅਚਲ ਦਸ਼ਮਲਵ* ਅੰਕੜੇ_2;
    int32_t d1_;
    int32_t d2_;
};

ConstArray2D::~ConstArray2D()
{
}

/**
 * A class to allocate data as a writable 1D array.
 * This is the main class implement matrix operation.
 */
ਵਰਗ Array1D : ਜਨਤਕ ReadArray1D {
ਜਨਤਕ:
    Array1D() : ਸਮ੍ਰਿਤੀ_2(ਖਾਲੀ_ਸੂਚਕ), ਅੰਕੜੇ_2(ਖਾਲੀ_ਸੂਚਕ), d1_(0) {}
    Array1D(int32_t d1, UError_ਕੋਡ &ਸਥਿਤੀ)
        : ਸਮ੍ਰਿਤੀ_2(uprv_malloc(d1 * ਆਕਾਰ(ਦਸ਼ਮਲਵ))),
          ਅੰਕੜੇ_2(static_cast<ਦਸ਼ਮਲਵ*>(ਸਮ੍ਰਿਤੀ_2)), d1_(d1) {
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
            ਜੇ (ਸਮ੍ਰਿਤੀ_2 == ਖਾਲੀ_ਸੂਚਕ) {
                ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ;
            }
            clear();
        }
    }

    ਆਭਾਸੀ ~Array1D();

    // A special constructor which does not own the memory but writeable
    // as a slice of an array.
    Array1D(ਦਸ਼ਮਲਵ* ਅੰਕੜੇ, int32_t d1)
        : ਸਮ੍ਰਿਤੀ_2(ਖਾਲੀ_ਸੂਚਕ), ਅੰਕੜੇ_2(ਅੰਕੜੇ), d1_(d1) {}

    // ReadArray1D methods.
    ਆਭਾਸੀ int32_t d1() ਅਚਲ override { ਵਾਪਸ d1_; }
    ਆਭਾਸੀ ਦਸ਼ਮਲਵ get(int32_t i) ਅਚਲ override {
        U_ASSERT(i < d1_);
        ਵਾਪਸ ਅੰਕੜੇ_2[i];
    }

    // Return the index which point to the max data in the array.
    ਸਤਰਅੰਦਰ int32_t max_ਸੂਚਕਾਂਕ() ਅਚਲ {
        int32_t ਸੂਚਕਾਂਕ = 0;
        ਦਸ਼ਮਲਵ max = ਅੰਕੜੇ_2[0];
        ਲਈ (int32_t i = 1; i < d1_; i++) {
            ਜੇ (ਅੰਕੜੇ_2[i] > max) {
                max = ਅੰਕੜੇ_2[i];
                ਸੂਚਕਾਂਕ = i;
            }
        }
        ਵਾਪਸ ਸੂਚਕਾਂਕ;
    }

    // Slice part of the array to a new one.
    ਸਤਰਅੰਦਰ Array1D ਟੁਕੜਾ(int32_t from, int32_t size) ਅਚਲ {
        U_ASSERT(from >= 0);
        U_ASSERT(from < d1_);
        U_ASSERT(from + size <= d1_);
        ਵਾਪਸ Array1D(ਅੰਕੜੇ_2 + from, size);
    }

    // Add dot product of a 1D array and a 2D array into this one.
    ਸਤਰਅੰਦਰ Array1D& ਜੋੜੋ_Dot_ਗੁਣਨਫਲ(ਅਚਲ ReadArray1D& a, ਅਚਲ ReadArray2D& b) {
        U_ASSERT(a.d1() == b.d1());
        U_ASSERT(b.d2() == d1());
        ਲਈ (int32_t i = 0; i < d1(); i++) {
            ਲਈ (int32_t j = 0; j < a.d1(); j++) {
                ਅੰਕੜੇ_2[i] += a.get(j) * b.get(j, i);
            }
        }
        ਵਾਪਸ *ਇਹ;
    }

    // Hadamard Product the values of another array of the same size into this one.
    ਸਤਰਅੰਦਰ Array1D& hadamard_ਗੁਣਨਫਲ(ਅਚਲ ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        ਲਈ (int32_t i = 0; i < d1(); i++) {
            ਅੰਕੜੇ_2[i] *= a.get(i);
        }
        ਵਾਪਸ *ਇਹ;
    }

    // Add the Hadamard Product of two arrays of the same size into this one.
    ਸਤਰਅੰਦਰ Array1D& ਜੋੜੋ_Hadamard_ਗੁਣਨਫਲ(ਅਚਲ ReadArray1D& a, ਅਚਲ ReadArray1D& b) {
        U_ASSERT(a.d1() == d1());
        U_ASSERT(b.d1() == d1());
        ਲਈ (int32_t i = 0; i < d1(); i++) {
            ਅੰਕੜੇ_2[i] += a.get(i) * b.get(i);
        }
        ਵਾਪਸ *ਇਹ;
    }

    // Add the values of another array of the same size into this one.
    ਸਤਰਅੰਦਰ Array1D& ਜੋੜੋ(ਅਚਲ ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        ਲਈ (int32_t i = 0; i < d1(); i++) {
            ਅੰਕੜੇ_2[i] += a.get(i);
        }
        ਵਾਪਸ *ਇਹ;
    }

    // Assign the values of another array of the same size into this one.
    ਸਤਰਅੰਦਰ Array1D& assign(ਅਚਲ ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        ਲਈ (int32_t i = 0; i < d1(); i++) {
            ਅੰਕੜੇ_2[i] = a.get(i);
        }
        ਵਾਪਸ *ਇਹ;
    }

    // Apply tanh to all the elements in the array.
    ਸਤਰਅੰਦਰ Array1D& tanh() {
        ਵਾਪਸ tanh(*ਇਹ);
    }

    // Apply tanh of a and store into this array.
    ਸਤਰਅੰਦਰ Array1D& tanh(ਅਚਲ Array1D& a) {
        U_ASSERT(a.d1() == d1());
        ਲਈ (int32_t i = 0; i < d1_; i++) {
            ਅੰਕੜੇ_2[i] = std::tanh(a.get(i));
        }
        ਵਾਪਸ *ਇਹ;
    }

    // Apply sigmoid to all the elements in the array.
    ਸਤਰਅੰਦਰ Array1D& sigmoid() {
        ਲਈ (int32_t i = 0; i < d1_; i++) {
            ਅੰਕੜੇ_2[i] = 1.0f/(1.0f + expf(-ਅੰਕੜੇ_2[i]));
        }
        ਵਾਪਸ *ਇਹ;
    }

    ਸਤਰਅੰਦਰ Array1D& clear() {
        uprv_memset(ਅੰਕੜੇ_2, 0, d1_ * ਆਕਾਰ(ਦਸ਼ਮਲਵ));
        ਵਾਪਸ *ਇਹ;
    }

ਨਿਜੀ:
    ਖਾਲੀ* ਸਮ੍ਰਿਤੀ_2;
    ਦਸ਼ਮਲਵ* ਅੰਕੜੇ_2;
    int32_t d1_;
};

Array1D::~Array1D()
{
    uprv_free(ਸਮ੍ਰਿਤੀ_2);
}

ਵਰਗ Array2D : ਜਨਤਕ ReadArray2D {
ਜਨਤਕ:
    Array2D() : ਸਮ੍ਰਿਤੀ_2(ਖਾਲੀ_ਸੂਚਕ), ਅੰਕੜੇ_2(ਖਾਲੀ_ਸੂਚਕ), d1_(0), d2_(0) {}
    Array2D(int32_t d1, int32_t d2, UError_ਕੋਡ &ਸਥਿਤੀ)
        : ਸਮ੍ਰਿਤੀ_2(uprv_malloc(d1 * d2 * ਆਕਾਰ(ਦਸ਼ਮਲਵ))),
          ਅੰਕੜੇ_2(static_cast<ਦਸ਼ਮਲਵ*>(ਸਮ੍ਰਿਤੀ_2)), d1_(d1), d2_(d2) {
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
            ਜੇ (ਸਮ੍ਰਿਤੀ_2 == ਖਾਲੀ_ਸੂਚਕ) {
                ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ;
            }
            clear();
        }
    }
    ਆਭਾਸੀ ~Array2D();

    // ReadArray2D methods.
    ਆਭਾਸੀ int32_t d1() ਅਚਲ override { ਵਾਪਸ d1_; }
    ਆਭਾਸੀ int32_t d2() ਅਚਲ override { ਵਾਪਸ d2_; }
    ਆਭਾਸੀ ਦਸ਼ਮਲਵ get(int32_t i, int32_t j) ਅਚਲ override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        ਵਾਪਸ ਅੰਕੜੇ_2[i * d2_ + j];
    }

    ਸਤਰਅੰਦਰ Array1D ਪੰਕਤੀ(int32_t i) ਅਚਲ {
        U_ASSERT(i < d1_);
        ਵਾਪਸ Array1D(ਅੰਕੜੇ_2 + i * d2_, d2_);
    }

    ਸਤਰਅੰਦਰ Array2D& clear() {
        uprv_memset(ਅੰਕੜੇ_2, 0, d1_ * d2_ * ਆਕਾਰ(ਦਸ਼ਮਲਵ));
        ਵਾਪਸ *ਇਹ;
    }

ਨਿਜੀ:
    ਖਾਲੀ* ਸਮ੍ਰਿਤੀ_2;
    ਦਸ਼ਮਲਵ* ਅੰਕੜੇ_2;
    int32_t d1_;
    int32_t d2_;
};

Array2D::~Array2D()
{
    uprv_free(ਸਮ੍ਰਿਤੀ_2);
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    BEGIN,
    INSIDE,
    ਸਿਰਾ_2,
    SINGLE
} LSTMClass;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    UNKNOWN,
    ਕੋਡ_POINTS,
    GRAPHEME_CLUSTER,
} EmbeddingType;

ਬਣਤਰ LSTMData : ਜਨਤਕ UMemory {
    LSTMData(UResourceBundle* rb, UError_ਕੋਡ &ਸਥਿਤੀ);
    ~LSTMData();
    UHashtable* fDict;
    EmbeddingType fType;
    ਅਚਲ char16_t* fName;
    ConstArray2D fEmbedding;
    ConstArray2D fForwardW;
    ConstArray2D fForwardU;
    ConstArray1D fForwardB;
    ConstArray2D fBackwardW;
    ConstArray2D fBackwardU;
    ConstArray1D fBackwardB;
    ConstArray2D fOutputW;
    ConstArray1D fOutputB;

ਨਿਜੀ:
    UResourceBundle* fBundle;
};

LSTMData::LSTMData(UResourceBundle* rb, UError_ਕੋਡ &ਸਥਿਤੀ)
    : fDict(ਖਾਲੀ_ਸੂਚਕ), fType(UNKNOWN), fName(ਖਾਲੀ_ਸੂਚਕ),
      fBundle(rb)
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    ਜੇ (IEEE_754 != 1) {
        ਸਥਿਤੀ = U_UNSUPPORTED_ERROR;
        ਵਾਪਸ;
    }
    Local_UResource_Bundle_ਸੂਚਕ embeddings_res(
        ures_get_By_ਕੁੰਜੀ(rb, "embeddings", ਖਾਲੀ_ਸੂਚਕ, &ਸਥਿਤੀ));
    int32_t embedding_ਮਾਪ = ures_getInt(embeddings_res.getAlias(), &ਸਥਿਤੀ);
    Local_UResource_Bundle_ਸੂਚਕ hunits_res(
        ures_get_By_ਕੁੰਜੀ(rb, "hunits", ਖਾਲੀ_ਸੂਚਕ, &ਸਥਿਤੀ));
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ;
    int32_t hunits = ures_getInt(hunits_res.getAlias(), &ਸਥਿਤੀ);
    ਅਚਲ char16_t* type = ures_get_String_By_ਕੁੰਜੀ(rb, "type", ਖਾਲੀ_ਸੂਚਕ, &ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ;
    ਜੇ (u_strCompare(type, -1, u"codepoints", -1, ਝੂਠ) == 0) {
        fType = ਕੋਡ_POINTS;
    } ਨਹੀਂਤਾਂ ਜੇ (u_strCompare(type, -1, u"graphclust", -1, ਝੂਠ) == 0) {
        fType = GRAPHEME_CLUSTER;
    }
    fName = ures_get_String_By_ਕੁੰਜੀ(rb, "model", ਖਾਲੀ_ਸੂਚਕ, &ਸਥਿਤੀ);
    Local_UResource_Bundle_ਸੂਚਕ ਅੰਕੜੇ_Res(ures_get_By_ਕੁੰਜੀ(rb, "data", ਖਾਲੀ_ਸੂਚਕ, &ਸਥਿਤੀ));
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ;
    int32_t ਅੰਕੜੇ_len = 0;
    ਅਚਲ int32_t* ਅੰਕੜੇ = ures_get_Int_ਸਦਿਸ਼(ਅੰਕੜੇ_Res.getAlias(), &ਅੰਕੜੇ_len, &ਸਥਿਤੀ);
    fDict = uhash_open(uhash_hashUChars, uhash_compareUChars, ਖਾਲੀ_ਸੂਚਕ, &ਸਥਿਤੀ);

    ਢੇਰ_UResource_Bundle ਢੇਰ_ਆਰਜ਼ੀ_Bundle;
    Resource_ਅੰਕੜੇ_ਮੁੱਲ value;
    ures_get_ਮੁੱਲ_With_Fallback(rb, "dict", ਢੇਰ_ਆਰਜ਼ੀ_Bundle.getAlias(), value, ਸਥਿਤੀ);
    ResourceArray stringArray = value.getArray(ਸਥਿਤੀ);
    int32_t ਅੰਕ_ਸੂਚਕਾਂਕ = stringArray.get_ਮਾਪ();
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }

    // put dict into hash
    int32_t string_ਲੰਮਾਈ;
    ਲਈ (int32_t idx = 0; idx < ਅੰਕ_ਸੂਚਕਾਂਕ; idx++) {
        stringArray.get_ਮੁੱਲ(idx, value);
        ਅਚਲ char16_t* str = value.getString(string_ਲੰਮਾਈ, ਸਥਿਤੀ);
        uhash_putiAllowZero(fDict, (ਖਾਲੀ*)str, idx, &ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LSTM_VECTORIZER_DEBUG
        ਲਿਖੋ("Assign [");
        ਜਦੋਂ (*str != 0x0000) {
            ਲਿਖੋ("U+%04x ", *str);
            str++;
        }
        ਲਿਖੋ("] map to %d\n", idx-1);
#ਜੇ_ਅੰਤ
    }
    int32_t mat1_ਮਾਪ = (ਅੰਕ_ਸੂਚਕਾਂਕ + 1) * embedding_ਮਾਪ;
    int32_t mat2_ਮਾਪ = embedding_ਮਾਪ * 4 * hunits;
    int32_t mat3_ਮਾਪ = hunits * 4 * hunits;
    int32_t mat4_ਮਾਪ = 4 * hunits;
    int32_t mat5_ਮਾਪ = mat2_ਮਾਪ;
    int32_t mat6_ਮਾਪ = mat3_ਮਾਪ;
    int32_t mat7_ਮਾਪ = mat4_ਮਾਪ;
    int32_t mat8_ਮਾਪ = 2 * hunits * 4;
#ਜੇ U_DEBUG
    int32_t mat9_ਮਾਪ = 4;
    U_ASSERT(ਅੰਕੜੇ_len == mat1_ਮਾਪ + mat2_ਮਾਪ + mat3_ਮਾਪ + mat4_ਮਾਪ + mat5_ਮਾਪ +
        mat6_ਮਾਪ + mat7_ਮਾਪ + mat8_ਮਾਪ + mat9_ਮਾਪ);
#ਜੇ_ਅੰਤ

    fEmbedding.init(ਅੰਕੜੇ, (ਅੰਕ_ਸੂਚਕਾਂਕ + 1), embedding_ਮਾਪ);
    ਅੰਕੜੇ += mat1_ਮਾਪ;
    fForwardW.init(ਅੰਕੜੇ, embedding_ਮਾਪ, 4 * hunits);
    ਅੰਕੜੇ += mat2_ਮਾਪ;
    fForwardU.init(ਅੰਕੜੇ, hunits, 4 * hunits);
    ਅੰਕੜੇ += mat3_ਮਾਪ;
    fForwardB.init(ਅੰਕੜੇ, 4 * hunits);
    ਅੰਕੜੇ += mat4_ਮਾਪ;
    fBackwardW.init(ਅੰਕੜੇ, embedding_ਮਾਪ, 4 * hunits);
    ਅੰਕੜੇ += mat5_ਮਾਪ;
    fBackwardU.init(ਅੰਕੜੇ, hunits, 4 * hunits);
    ਅੰਕੜੇ += mat6_ਮਾਪ;
    fBackwardB.init(ਅੰਕੜੇ, 4 * hunits);
    ਅੰਕੜੇ += mat7_ਮਾਪ;
    fOutputW.init(ਅੰਕੜੇ, 2 * hunits, 4);
    ਅੰਕੜੇ += mat8_ਮਾਪ;
    fOutputB.init(ਅੰਕੜੇ, 4);
}

LSTMData::~LSTMData() {
    uhash_close(fDict);
    ures_close(fBundle);
}

ਵਰਗ Vectorizer : ਜਨਤਕ UMemory {
ਜਨਤਕ:
    Vectorizer(UHashtable* dict) : fDict(dict) {}
    ਆਭਾਸੀ ~Vectorizer();
    ਆਭਾਸੀ ਖਾਲੀ vectorize(UText *ਪਾਠ, int32_t ਸ਼ੁਰੂ_Pos, int32_t ਸਿਰਾ_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ = 0;
ਸੁਰੱਖਿਅਤ:
    int32_t string_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ(ਅਚਲ char16_t* str) ਅਚਲ {
        UBool ਮਿਲਿਆ = ਝੂਠ;
        int32_t ret = uhash_geti_And_ਮਿਲਿਆ(fDict, (ਅਚਲ ਖਾਲੀ*)str, &ਮਿਲਿਆ);
        ਜੇ (!ਮਿਲਿਆ) {
            ret = fDict->count;
        }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LSTM_VECTORIZER_DEBUG
        ਲਿਖੋ("[");
        ਜਦੋਂ (*str != 0x0000) {
            ਲਿਖੋ("U+%04x ", *str);
            str++;
        }
        ਲਿਖੋ("] map to %d\n", ret);
#ਜੇ_ਅੰਤ
        ਵਾਪਸ ret;
    }

ਨਿਜੀ:
    UHashtable* fDict;
};

Vectorizer::~Vectorizer()
{
}

ਵਰਗ ਕੋਡ_Points_Vectorizer : ਜਨਤਕ Vectorizer {
ਜਨਤਕ:
    ਕੋਡ_Points_Vectorizer(UHashtable* dict) : Vectorizer(dict) {}
    ਆਭਾਸੀ ~ਕੋਡ_Points_Vectorizer();
    ਆਭਾਸੀ ਖਾਲੀ vectorize(UText *ਪਾਠ, int32_t ਸ਼ੁਰੂ_Pos, int32_t ਸਿਰਾ_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ override;
};

ਕੋਡ_Points_Vectorizer::~ਕੋਡ_Points_Vectorizer()
{
}

ਖਾਲੀ ਕੋਡ_Points_Vectorizer::vectorize(
    UText *ਪਾਠ, int32_t ਸ਼ੁਰੂ_Pos, int32_t ਸਿਰਾ_Pos,
    UVector32 &offsets, UVector32 &indices, UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ
{
    ਜੇ (offsets.ensure_ਸਮਰੱਥਾ(ਸਿਰਾ_Pos - ਸ਼ੁਰੂ_Pos, ਸਥਿਤੀ) &&
            indices.ensure_ਸਮਰੱਥਾ(ਸਿਰਾ_Pos - ਸ਼ੁਰੂ_Pos, ਸਥਿਤੀ)) {
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ;
        utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, ਸ਼ੁਰੂ_Pos);
        int32_t current;
        char16_t str[2] = {0, 0};
        ਜਦੋਂ (U_SUCCESS(ਸਥਿਤੀ) &&
               (current = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ))) < ਸਿਰਾ_Pos) {
            // Since the LSTMBreakEngine is currently only accept chars in BMP,
            // we can ignore the possibility of hitting supplementary code
            // point.
            str[0] = static_cast<char16_t>(utext_next32(ਪਾਠ));
            U_ASSERT(!U_IS_SURROGATE(str[0]));
            offsets.ਜੋੜੋ_Element(current, ਸਥਿਤੀ);
            indices.ਜੋੜੋ_Element(string_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ(str), ਸਥਿਤੀ);
        }
    }
}

ਵਰਗ GraphemeClusterVectorizer : ਜਨਤਕ Vectorizer {
ਜਨਤਕ:
    GraphemeClusterVectorizer(UHashtable* dict)
        : Vectorizer(dict)
    {
    }
    ਆਭਾਸੀ ~GraphemeClusterVectorizer();
    ਆਭਾਸੀ ਖਾਲੀ vectorize(UText *ਪਾਠ, int32_t ਸ਼ੁਰੂ_Pos, int32_t ਸਿਰਾ_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ override;
};

GraphemeClusterVectorizer::~GraphemeClusterVectorizer()
{
}

constexpr int32_t MAX_GRAPHEME_CLSTER_ਲੰਮਾਈ = 10;

ਖਾਲੀ GraphemeClusterVectorizer::vectorize(
    UText *ਪਾਠ, int32_t ਸ਼ੁਰੂ_Pos, int32_t ਸਿਰਾ_Pos,
    UVector32 &offsets, UVector32 &indices, UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ;
    ਜੇ (!offsets.ensure_ਸਮਰੱਥਾ(ਸਿਰਾ_Pos - ਸ਼ੁਰੂ_Pos, ਸਥਿਤੀ) ||
            !indices.ensure_ਸਮਰੱਥਾ(ਸਿਰਾ_Pos - ਸ਼ੁਰੂ_Pos, ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ;
    Local_ਸੂਚਕ<BreakIterator> graphemeIter(BreakIterator::createCharacterInstance(Locale(), ਸਥਿਤੀ));
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ;
    graphemeIter->ਸੈੱਟ_ਪਾਠ(ਪਾਠ, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ;

    ਜੇ (ਸ਼ੁਰੂ_Pos != 0) {
        graphemeIter->preceding(ਸ਼ੁਰੂ_Pos);
    }
    int32_t ਆਖਰੀ = ਸ਼ੁਰੂ_Pos;
    int32_t current = ਸ਼ੁਰੂ_Pos;
    char16_t str[MAX_GRAPHEME_CLSTER_ਲੰਮਾਈ];
    ਜਦੋਂ ((current = graphemeIter->next()) != BreakIterator::ਪੂਰਾ_2) {
        ਜੇ (current >= ਸਿਰਾ_Pos) {
            ਰੋਕੋ;
        }
        ਜੇ (current > ਸ਼ੁਰੂ_Pos) {
            utext_ਕੱਢੋ(ਪਾਠ, ਆਖਰੀ, current, str, MAX_GRAPHEME_CLSTER_ਲੰਮਾਈ, &ਸਥਿਤੀ);
            ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ;
            offsets.ਜੋੜੋ_Element(ਆਖਰੀ, ਸਥਿਤੀ);
            indices.ਜੋੜੋ_Element(string_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ(str), ਸਥਿਤੀ);
            ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ;
        }
        ਆਖਰੀ = current;
    }
    ਜੇ (U_FAILURE(ਸਥਿਤੀ) || ਆਖਰੀ >= ਸਿਰਾ_Pos) {
        ਵਾਪਸ;
    }
    utext_ਕੱਢੋ(ਪਾਠ, ਆਖਰੀ, ਸਿਰਾ_Pos, str, MAX_GRAPHEME_CLSTER_ਲੰਮਾਈ, &ਸਥਿਤੀ);
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        offsets.ਜੋੜੋ_Element(ਆਖਰੀ, ਸਥਿਤੀ);
        indices.ਜੋੜੋ_Element(string_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ(str), ਸਥਿਤੀ);
    }
}

// Computing LSTM as stated in
// https://en.wikipedia.org/wiki/Long_short-term_memory#LSTM_with_a_forget_gate
// ifco is temp array allocate outside which does not need to be
// input/output value but could avoid unnecessary memory alloc/free if passing
// in.
ਖਾਲੀ compute(
    int32_t hunits,
    ਅਚਲ ReadArray2D& W, ਅਚਲ ReadArray2D& U, ਅਚਲ ReadArray1D& b,
    ਅਚਲ ReadArray1D& x, Array1D& h, Array1D& c,
    Array1D& ifco)
{
    // ifco = x * W + h * U + b
    ifco.assign(b)
        .ਜੋੜੋ_Dot_ਗੁਣਨਫਲ(x, W)
        .ਜੋੜੋ_Dot_ਗੁਣਨਫਲ(h, U);

    ifco.ਟੁਕੜਾ(0*hunits, hunits).sigmoid();  // i: sigmod
    ifco.ਟੁਕੜਾ(1*hunits, hunits).sigmoid(); // f: sigmoid
    ifco.ਟੁਕੜਾ(2*hunits, hunits).tanh(); // c_: tanh
    ifco.ਟੁਕੜਾ(3*hunits, hunits).sigmoid(); // o: sigmod

    c.hadamard_ਗੁਣਨਫਲ(ifco.ਟੁਕੜਾ(hunits, hunits))
        .ਜੋੜੋ_Hadamard_ਗੁਣਨਫਲ(ifco.ਟੁਕੜਾ(0, hunits), ifco.ਟੁਕੜਾ(2*hunits, hunits));

    h.tanh(c)
        .hadamard_ਗੁਣਨਫਲ(ifco.ਟੁਕੜਾ(3*hunits, hunits));
}

// Minimum word size
ਸਥਿਰ ਅਚਲ int32_t MIN_ਸ਼ਬਦ = 2;

// Minimum number of characters for two words
ਸਥਿਰ ਅਚਲ int32_t MIN_ਸ਼ਬਦ_ਵਿਸਤਾਰ = MIN_ਸ਼ਬਦ * 2;

int32_t
LSTMBreakEngine::divide_ਉੱਪਰ_Dictionary_Range( UText *ਪਾਠ,
                                                int32_t ਸ਼ੁਰੂ_Pos,
                                                int32_t ਸਿਰਾ_Pos,
                                                UVector32 &ਮਿਲਿਆ_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ 0;
    int32_t begin_ਮਿਲਿਆ_Break_ਮਾਪ = ਮਿਲਿਆ_Breaks.size();
    utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, ਸ਼ੁਰੂ_Pos);
    utext_moveIndex32(ਪਾਠ, MIN_ਸ਼ਬਦ_ਵਿਸਤਾਰ);
    ਜੇ (utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ) >= ਸਿਰਾ_Pos) {
        ਵਾਪਸ 0;       // Not enough characters for two words
    }
    utext_ਸੈੱਟ_Native_ਸੂਚਕਾਂਕ(ਪਾਠ, ਸ਼ੁਰੂ_Pos);

    UVector32 offsets(ਸਥਿਤੀ);
    UVector32 indices(ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ 0;
    fVectorizer->vectorize(ਪਾਠ, ਸ਼ੁਰੂ_Pos, ਸਿਰਾ_Pos, offsets, indices, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ 0;
    int32_t* offsetsBuf = offsets.get_ਬਫਰ();
    int32_t* indicesBuf = indices.get_ਬਫਰ();

    int32_t ਨਿਵੇਸ਼_seq_len = indices.size();
    int32_t hunits = f_ਅੰਕੜੇ->fForwardU.d1();

    // ----- Begin of all the Array memory allocation needed for this function
    // Allocate temp array used inside compute()
    Array1D ifco(4 * hunits, ਸਥਿਤੀ);

    Array1D c(hunits, ਸਥਿਤੀ);
    Array1D logp(4, ਸਥਿਤੀ);

    // TODO: limit size of hBackward. If input_seq_len is too big, we could
    // run out of memory.
    // Backward LSTM
    Array2D hBackward(ਨਿਵੇਸ਼_seq_len, hunits, ਸਥਿਤੀ);

    // Allocate fbRow and slice the internal array in two.
    Array1D fb_ਪੰਕਤੀ(2 * hunits, ਸਥਿਤੀ);

    // ----- End of all the Array memory allocation needed for this function
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ 0;

    // To save the needed memory usage, the following is different from the
    // Python or ICU4X implementation. We first perform the Backward LSTM
    // and then merge the iteration of the forward LSTM and the output layer
    // together because we only neetdto remember the h[t-1] for Forward LSTM.
    ਲਈ (int32_t i = ਨਿਵੇਸ਼_seq_len - 1; i >= 0; i--) {
        Array1D h_ਪੰਕਤੀ = hBackward.ਪੰਕਤੀ(i);
        ਜੇ (i != ਨਿਵੇਸ਼_seq_len - 1) {
            h_ਪੰਕਤੀ.assign(hBackward.ਪੰਕਤੀ(i+1));
        }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LSTM_DEBUG
        ਲਿਖੋ("hRow %d\n", i);
        h_ਪੰਕਤੀ.print();
        ਲਿਖੋ("indicesBuf[%d] = %d\n", i, indicesBuf[i]);
        ਲਿਖੋ("fData->fEmbedding.row(indicesBuf[%d]):\n", i);
        f_ਅੰਕੜੇ->fEmbedding.ਪੰਕਤੀ(indicesBuf[i]).print();
#ਜੇ_ਅੰਤ  // LSTM_DEBUG
        compute(hunits,
                f_ਅੰਕੜੇ->fBackwardW, f_ਅੰਕੜੇ->fBackwardU, f_ਅੰਕੜੇ->fBackwardB,
                f_ਅੰਕੜੇ->fEmbedding.ਪੰਕਤੀ(indicesBuf[i]),
                h_ਪੰਕਤੀ, c, ifco);
    }


    Array1D forward_ਪੰਕਤੀ = fb_ਪੰਕਤੀ.ਟੁਕੜਾ(0, hunits);  // point to first half of data in fbRow.
    Array1D backward_ਪੰਕਤੀ = fb_ਪੰਕਤੀ.ਟੁਕੜਾ(hunits, hunits);  // point to second half of data n fbRow.

    // The following iteration merge the forward LSTM and the output layer
    // together.
    c.clear();  // reuse c since it is the same size.
    ਲਈ (int32_t i = 0; i < ਨਿਵੇਸ਼_seq_len; i++) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LSTM_DEBUG
        ਲਿਖੋ("forwardRow %d\n", i);
        forward_ਪੰਕਤੀ.print();
#ਜੇ_ਅੰਤ  // LSTM_DEBUG
        // Forward LSTM
        // Calculate the result into forwardRow, which point to the data in the first half
        // of fbRow.
        compute(hunits,
                f_ਅੰਕੜੇ->fForwardW, f_ਅੰਕੜੇ->fForwardU, f_ਅੰਕੜੇ->fForwardB,
                f_ਅੰਕੜੇ->fEmbedding.ਪੰਕਤੀ(indicesBuf[i]),
                forward_ਪੰਕਤੀ, c, ifco);

        // assign the data from hBackward.row(i) to second half of fbRowa.
        backward_ਪੰਕਤੀ.assign(hBackward.ਪੰਕਤੀ(i));

        logp.assign(f_ਅੰਕੜੇ->fOutputB).ਜੋੜੋ_Dot_ਗੁਣਨਫਲ(fb_ਪੰਕਤੀ, f_ਅੰਕੜੇ->fOutputW);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LSTM_DEBUG
        ਲਿਖੋ("backwardRow %d\n", i);
        backward_ਪੰਕਤੀ.print();
        ਲਿਖੋ("logp %d\n", i);
        logp.print();
#ਜੇ_ਅੰਤ  // LSTM_DEBUG

        // current = argmax(logp)
        LSTMClass current = static_cast<LSTMClass>(logp.max_ਸੂਚਕਾਂਕ());
        // BIES logic.
        ਜੇ (current == BEGIN || current == SINGLE) {
            ਜੇ (i != 0) {
                ਮਿਲਿਆ_Breaks.ਜੋੜੋ_Element(offsetsBuf[i], ਸਥਿਤੀ);
                ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ 0;
            }
        }
    }
    ਵਾਪਸ ਮਿਲਿਆ_Breaks.size() - begin_ਮਿਲਿਆ_Break_ਮਾਪ;
}

Vectorizer* createVectorizer(ਅਚਲ LSTMData* ਅੰਕੜੇ, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਚੋਣ (ਅੰਕੜੇ->fType) {
        ਹਾਲਤ ਕੋਡ_POINTS:
            ਵਾਪਸ ਨਵਾਂ ਕੋਡ_Points_Vectorizer(ਅੰਕੜੇ->fDict);
            ਰੋਕੋ;
        ਹਾਲਤ GRAPHEME_CLUSTER:
            ਵਾਪਸ ਨਵਾਂ GraphemeClusterVectorizer(ਅੰਕੜੇ->fDict);
            ਰੋਕੋ;
        ਮੂਲ:
            ਰੋਕੋ;
    }
    UPRV_UNREACHABLE_EXIT;
}

LSTMBreakEngine::LSTMBreakEngine(ਅਚਲ LSTMData* ਅੰਕੜੇ, ਅਚਲ Unicode_ਸੈੱਟ& ਸੈੱਟ, UError_ਕੋਡ &ਸਥਿਤੀ)
    : DictionaryBreakEngine(), f_ਅੰਕੜੇ(ਅੰਕੜੇ), fVectorizer(createVectorizer(f_ਅੰਕੜੇ, ਸਥਿਤੀ))
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
      f_ਅੰਕੜੇ = ਖਾਲੀ_ਸੂਚਕ;  // If failure, we should not delete fData in destructor because the caller will do so.
      ਵਾਪਸ;
    }
    ਸੈੱਟ_Characters(ਸੈੱਟ);
}

LSTMBreakEngine::~LSTMBreakEngine() {
    ਮਿਟਾਓ f_ਅੰਕੜੇ;
    ਮਿਟਾਓ fVectorizer;
}

ਅਚਲ char16_t* LSTMBreakEngine::name() ਅਚਲ {
    ਵਾਪਸ f_ਅੰਕੜੇ->fName;
}

UnicodeString defaultLSTM(UScript_ਕੋਡ script, UError_ਕੋਡ& ਸਥਿਤੀ) {
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &ਸਥਿਤੀ);
    b = ures_get_By_ਕੁੰਜੀ_With_Fallback(b, "lstm", b, &ਸਥਿਤੀ);
    UnicodeString ਨਤੀਜਾ = ures_get_Unicode_String_By_ਕੁੰਜੀ(b, uscript_getShortName(script), &ਸਥਿਤੀ);
    ures_close(b);
    ਵਾਪਸ ਨਤੀਜਾ;
}

U_CAPI ਅਚਲ LSTMData* U_EXPORT2 CreateLSTMDataForScript(UScript_ਕੋਡ script, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਜੇ (script != USCRIPT_KHMER && script != USCRIPT_LAO && script != USCRIPT_MYANMAR && script != USCRIPT_THAI) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    UnicodeString name = defaultLSTM(script, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    CharString namebuf;
    namebuf.appendInvariantChars(name, ਸਥਿਤੀ).truncate(namebuf.ਆਖਰੀ_ਸੂਚਕਾਂਕ_ਦਾ('.'));

    Local_UResource_Bundle_ਸੂਚਕ rb(
        ures_open_ਸਿੱਧਾ(U_ICUDATA_BRKITR, namebuf.ਅੰਕੜੇ(), &ਸਥਿਤੀ));
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;

    ਵਾਪਸ CreateLSTMData(rb.orphan(), ਸਥਿਤੀ);
}

U_CAPI ਅਚਲ LSTMData* U_EXPORT2 CreateLSTMData(UResourceBundle* rb, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਅਚਲ LSTMData* ਨਤੀਜਾ = ਨਵਾਂ LSTMData(rb, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਮਿਟਾਓ ਨਤੀਜਾ;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

U_CAPI ਅਚਲ LanguageBreakEngine* U_EXPORT2
CreateLSTMBreakEngine(UScript_ਕੋਡ script, ਅਚਲ LSTMData* ਅੰਕੜੇ, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    UnicodeString unicode_ਸੈੱਟ_String;
    ਚੋਣ(script) {
        ਹਾਲਤ USCRIPT_THAI:
            unicode_ਸੈੱਟ_String = UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]");
            ਰੋਕੋ;
        ਹਾਲਤ USCRIPT_MYANMAR:
            unicode_ਸੈੱਟ_String = UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]");
            ਰੋਕੋ;
        ਮੂਲ:
            ਮਿਟਾਓ ਅੰਕੜੇ;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    Unicode_ਸੈੱਟ unicode_ਸੈੱਟ;
    unicode_ਸੈੱਟ.apply_ਨਮੂਨਾ(unicode_ਸੈੱਟ_String, ਸਥਿਤੀ);
    ਅਚਲ LanguageBreakEngine* engine = ਨਵਾਂ LSTMBreakEngine(ਅੰਕੜੇ, unicode_ਸੈੱਟ, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ) || engine == ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ (engine != ਖਾਲੀ_ਸੂਚਕ) {
            ਮਿਟਾਓ engine;
        } ਨਹੀਂਤਾਂ {
            ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        }
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ engine;
}

U_CAPI ਖਾਲੀ U_EXPORT2 DeleteLSTMData(ਅਚਲ LSTMData* ਅੰਕੜੇ)
{
    ਮਿਟਾਓ ਅੰਕੜੇ;
}

U_CAPI ਅਚਲ char16_t* U_EXPORT2 LSTMDataName(ਅਚਲ LSTMData* ਅੰਕੜੇ)
{
    ਵਾਪਸ ਅੰਕੜੇ->fName;
}

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_BREAK_ITERATION */
