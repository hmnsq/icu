// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ಅನ್ತರ್ಭಾವ <complex>
#ಅನ್ತರ್ಭಾವ <utility>

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "brkeng.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "lstmbe.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ubrkimpl.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "uvectr32.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"

#ಅನ್ತರ್ಭಾವ "unicode/brkiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/resbund.h"
#ಅನ್ತರ್ಭಾವ "unicode/ubrk.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf.h"

U_NAMESPACE_BEGIN

// Uncomment the following #define to debug.
// #define LSTM_DEBUG 1
// #define LSTM_VECTORIZER_DEBUG 1

/**
 * Interface for reading 1D array.
 */
ವರ್ಗ ReadArray1D {
ಸಾರ್ವಜನಿಕ:
    ವಾಸ್ತವಿಕ ~ReadArray1D();
    ವಾಸ್ತವಿಕ int32_t d1() ಸ್ಥಿರಾಂಕ = 0;
    ವಾಸ್ತವಿಕ ದಶಮಾಂಶ get(int32_t i) ಸ್ಥಿರಾಂಕ = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LSTM_DEBUG
    ಶೂನ್ಯ print() ಸ್ಥಿರಾಂಕ {
        ಬರೆ("\n[");
        ಗಾಗಿ (int32_t i = 0; i < d1(); i++) {
           ಬರೆ("%0.8e ", get(i));
           ಆದರೆ (i % 4 == 3) ಬರೆ("\n");
        }
        ಬರೆ("]\n");
    }
#ಆದರೆ_ಅಂತ್ಯ
};

ReadArray1D::~ReadArray1D()
{
}

/**
 * Interface for reading 2D array.
 */
ವರ್ಗ ReadArray2D {
ಸಾರ್ವಜನಿಕ:
    ವಾಸ್ತವಿಕ ~ReadArray2D();
    ವಾಸ್ತವಿಕ int32_t d1() ಸ್ಥಿರಾಂಕ = 0;
    ವಾಸ್ತವಿಕ int32_t d2() ಸ್ಥಿರಾಂಕ = 0;
    ವಾಸ್ತವಿಕ ದಶಮಾಂಶ get(int32_t i, int32_t j) ಸ್ಥಿರಾಂಕ = 0;
};

ReadArray2D::~ReadArray2D()
{
}

/**
 * A class to index a float array as a 1D Array without owning the pointer or
 * copy the data.
 */
ವರ್ಗ ConstArray1D : ಸಾರ್ವಜನಿಕ ReadArray1D {
ಸಾರ್ವಜನಿಕ:
    ConstArray1D() : ದತ್ತಾಂಶ_2(ಶೂನ್ಯಸೂಚಕ), d1_(0) {}

    ConstArray1D(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ* ದತ್ತಾಂಶ, int32_t d1) : ದತ್ತಾಂಶ_2(ದತ್ತಾಂಶ), d1_(d1) {}

    ವಾಸ್ತವಿಕ ~ConstArray1D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    ಶೂನ್ಯ init(ಸ್ಥಿರಾಂಕ int32_t* ದತ್ತಾಂಶ, int32_t d1) {
        U_ASSERT(IEEE_754 == 1);
        ದತ್ತಾಂಶ_2 = reinterpret_cast<ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ*>(ದತ್ತಾಂಶ);
        d1_ = d1;
    }

    // ReadArray1D methods.
    ವಾಸ್ತವಿಕ int32_t d1() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ d1_; }
    ವಾಸ್ತವಿಕ ದಶಮಾಂಶ get(int32_t i) ಸ್ಥಿರಾಂಕ override {
        U_ASSERT(i < d1_);
        ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ_2[i];
    }

ಖಾಸಗಿ:
    ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ* ದತ್ತಾಂಶ_2;
    int32_t d1_;
};

ConstArray1D::~ConstArray1D()
{
}

/**
 * A class to index a float array as a 2D Array without owning the pointer or
 * copy the data.
 */
ವರ್ಗ ConstArray2D : ಸಾರ್ವಜನಿಕ ReadArray2D {
ಸಾರ್ವಜನಿಕ:
    ConstArray2D() : ದತ್ತಾಂಶ_2(ಶೂನ್ಯಸೂಚಕ), d1_(0), d2_(0) {}

    ConstArray2D(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ* ದತ್ತಾಂಶ, int32_t d1, int32_t d2)
        : ದತ್ತಾಂಶ_2(ದತ್ತಾಂಶ), d1_(d1), d2_(d2) {}

    ವಾಸ್ತವಿಕ ~ConstArray2D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    ಶೂನ್ಯ init(ಸ್ಥಿರಾಂಕ int32_t* ದತ್ತಾಂಶ, int32_t d1, int32_t d2) {
        U_ASSERT(IEEE_754 == 1);
        ದತ್ತಾಂಶ_2 = reinterpret_cast<ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ*>(ದತ್ತಾಂಶ);
        d1_ = d1;
        d2_ = d2;
    }

    // ReadArray2D methods.
    ಸಾಲೊಳಗೆ int32_t d1() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ d1_; }
    ಸಾಲೊಳಗೆ int32_t d2() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ d2_; }
    ದಶಮಾಂಶ get(int32_t i, int32_t j) ಸ್ಥಿರಾಂಕ override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ_2[i * d2_ + j];
    }

    // Expose the ith row as a ConstArray1D
    ಸಾಲೊಳಗೆ ConstArray1D ಅಡ್ಡಸಾಲು(int32_t i) ಸ್ಥಿರಾಂಕ {
        U_ASSERT(i < d1_);
        ಹಿಂತಿರುಗಿ ConstArray1D(ದತ್ತಾಂಶ_2 + i * d2_, d2_);
    }

ಖಾಸಗಿ:
    ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ* ದತ್ತಾಂಶ_2;
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
ವರ್ಗ Array1D : ಸಾರ್ವಜನಿಕ ReadArray1D {
ಸಾರ್ವಜನಿಕ:
    Array1D() : ಸ್ಮರಣೆ_2(ಶೂನ್ಯಸೂಚಕ), ದತ್ತಾಂಶ_2(ಶೂನ್ಯಸೂಚಕ), d1_(0) {}
    Array1D(int32_t d1, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
        : ಸ್ಮರಣೆ_2(uprv_malloc(d1 * ಗಾತ್ರ(ದಶಮಾಂಶ))),
          ದತ್ತಾಂಶ_2(static_cast<ದಶಮಾಂಶ*>(ಸ್ಮರಣೆ_2)), d1_(d1) {
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
            ಆದರೆ (ಸ್ಮರಣೆ_2 == ಶೂನ್ಯಸೂಚಕ) {
                ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ;
            }
            clear();
        }
    }

    ವಾಸ್ತವಿಕ ~Array1D();

    // A special constructor which does not own the memory but writeable
    // as a slice of an array.
    Array1D(ದಶಮಾಂಶ* ದತ್ತಾಂಶ, int32_t d1)
        : ಸ್ಮರಣೆ_2(ಶೂನ್ಯಸೂಚಕ), ದತ್ತಾಂಶ_2(ದತ್ತಾಂಶ), d1_(d1) {}

    // ReadArray1D methods.
    ವಾಸ್ತವಿಕ int32_t d1() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ d1_; }
    ವಾಸ್ತವಿಕ ದಶಮಾಂಶ get(int32_t i) ಸ್ಥಿರಾಂಕ override {
        U_ASSERT(i < d1_);
        ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ_2[i];
    }

    // Return the index which point to the max data in the array.
    ಸಾಲೊಳಗೆ int32_t max_ಸೂಚ್ಯಂಕ() ಸ್ಥಿರಾಂಕ {
        int32_t ಸೂಚ್ಯಂಕ = 0;
        ದಶಮಾಂಶ max = ದತ್ತಾಂಶ_2[0];
        ಗಾಗಿ (int32_t i = 1; i < d1_; i++) {
            ಆದರೆ (ದತ್ತಾಂಶ_2[i] > max) {
                max = ದತ್ತಾಂಶ_2[i];
                ಸೂಚ್ಯಂಕ = i;
            }
        }
        ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ;
    }

    // Slice part of the array to a new one.
    ಸಾಲೊಳಗೆ Array1D ತುಂಡು(int32_t from, int32_t size) ಸ್ಥಿರಾಂಕ {
        U_ASSERT(from >= 0);
        U_ASSERT(from < d1_);
        U_ASSERT(from + size <= d1_);
        ಹಿಂತಿರುಗಿ Array1D(ದತ್ತಾಂಶ_2 + from, size);
    }

    // Add dot product of a 1D array and a 2D array into this one.
    ಸಾಲೊಳಗೆ Array1D& ಕೂಡಿಸು_Dot_ಗುಣಲಬ್ಧ(ಸ್ಥಿರಾಂಕ ReadArray1D& a, ಸ್ಥಿರಾಂಕ ReadArray2D& b) {
        U_ASSERT(a.d1() == b.d1());
        U_ASSERT(b.d2() == d1());
        ಗಾಗಿ (int32_t i = 0; i < d1(); i++) {
            ಗಾಗಿ (int32_t j = 0; j < a.d1(); j++) {
                ದತ್ತಾಂಶ_2[i] += a.get(j) * b.get(j, i);
            }
        }
        ಹಿಂತಿರುಗಿ *ಇದು;
    }

    // Hadamard Product the values of another array of the same size into this one.
    ಸಾಲೊಳಗೆ Array1D& hadamard_ಗುಣಲಬ್ಧ(ಸ್ಥಿರಾಂಕ ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        ಗಾಗಿ (int32_t i = 0; i < d1(); i++) {
            ದತ್ತಾಂಶ_2[i] *= a.get(i);
        }
        ಹಿಂತಿರುಗಿ *ಇದು;
    }

    // Add the Hadamard Product of two arrays of the same size into this one.
    ಸಾಲೊಳಗೆ Array1D& ಕೂಡಿಸು_Hadamard_ಗುಣಲಬ್ಧ(ಸ್ಥಿರಾಂಕ ReadArray1D& a, ಸ್ಥಿರಾಂಕ ReadArray1D& b) {
        U_ASSERT(a.d1() == d1());
        U_ASSERT(b.d1() == d1());
        ಗಾಗಿ (int32_t i = 0; i < d1(); i++) {
            ದತ್ತಾಂಶ_2[i] += a.get(i) * b.get(i);
        }
        ಹಿಂತಿರುಗಿ *ಇದು;
    }

    // Add the values of another array of the same size into this one.
    ಸಾಲೊಳಗೆ Array1D& ಕೂಡಿಸು(ಸ್ಥಿರಾಂಕ ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        ಗಾಗಿ (int32_t i = 0; i < d1(); i++) {
            ದತ್ತಾಂಶ_2[i] += a.get(i);
        }
        ಹಿಂತಿರುಗಿ *ಇದು;
    }

    // Assign the values of another array of the same size into this one.
    ಸಾಲೊಳಗೆ Array1D& assign(ಸ್ಥಿರಾಂಕ ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        ಗಾಗಿ (int32_t i = 0; i < d1(); i++) {
            ದತ್ತಾಂಶ_2[i] = a.get(i);
        }
        ಹಿಂತಿರುಗಿ *ಇದು;
    }

    // Apply tanh to all the elements in the array.
    ಸಾಲೊಳಗೆ Array1D& tanh() {
        ಹಿಂತಿರುಗಿ tanh(*ಇದು);
    }

    // Apply tanh of a and store into this array.
    ಸಾಲೊಳಗೆ Array1D& tanh(ಸ್ಥಿರಾಂಕ Array1D& a) {
        U_ASSERT(a.d1() == d1());
        ಗಾಗಿ (int32_t i = 0; i < d1_; i++) {
            ದತ್ತಾಂಶ_2[i] = std::tanh(a.get(i));
        }
        ಹಿಂತಿರುಗಿ *ಇದು;
    }

    // Apply sigmoid to all the elements in the array.
    ಸಾಲೊಳಗೆ Array1D& sigmoid() {
        ಗಾಗಿ (int32_t i = 0; i < d1_; i++) {
            ದತ್ತಾಂಶ_2[i] = 1.0f/(1.0f + expf(-ದತ್ತಾಂಶ_2[i]));
        }
        ಹಿಂತಿರುಗಿ *ಇದು;
    }

    ಸಾಲೊಳಗೆ Array1D& clear() {
        uprv_memset(ದತ್ತಾಂಶ_2, 0, d1_ * ಗಾತ್ರ(ದಶಮಾಂಶ));
        ಹಿಂತಿರುಗಿ *ಇದು;
    }

ಖಾಸಗಿ:
    ಶೂನ್ಯ* ಸ್ಮರಣೆ_2;
    ದಶಮಾಂಶ* ದತ್ತಾಂಶ_2;
    int32_t d1_;
};

Array1D::~Array1D()
{
    uprv_free(ಸ್ಮರಣೆ_2);
}

ವರ್ಗ Array2D : ಸಾರ್ವಜನಿಕ ReadArray2D {
ಸಾರ್ವಜನಿಕ:
    Array2D() : ಸ್ಮರಣೆ_2(ಶೂನ್ಯಸೂಚಕ), ದತ್ತಾಂಶ_2(ಶೂನ್ಯಸೂಚಕ), d1_(0), d2_(0) {}
    Array2D(int32_t d1, int32_t d2, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
        : ಸ್ಮರಣೆ_2(uprv_malloc(d1 * d2 * ಗಾತ್ರ(ದಶಮಾಂಶ))),
          ದತ್ತಾಂಶ_2(static_cast<ದಶಮಾಂಶ*>(ಸ್ಮರಣೆ_2)), d1_(d1), d2_(d2) {
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
            ಆದರೆ (ಸ್ಮರಣೆ_2 == ಶೂನ್ಯಸೂಚಕ) {
                ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ;
            }
            clear();
        }
    }
    ವಾಸ್ತವಿಕ ~Array2D();

    // ReadArray2D methods.
    ವಾಸ್ತವಿಕ int32_t d1() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ d1_; }
    ವಾಸ್ತವಿಕ int32_t d2() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ d2_; }
    ವಾಸ್ತವಿಕ ದಶಮಾಂಶ get(int32_t i, int32_t j) ಸ್ಥಿರಾಂಕ override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ_2[i * d2_ + j];
    }

    ಸಾಲೊಳಗೆ Array1D ಅಡ್ಡಸಾಲು(int32_t i) ಸ್ಥಿರಾಂಕ {
        U_ASSERT(i < d1_);
        ಹಿಂತಿರುಗಿ Array1D(ದತ್ತಾಂಶ_2 + i * d2_, d2_);
    }

    ಸಾಲೊಳಗೆ Array2D& clear() {
        uprv_memset(ದತ್ತಾಂಶ_2, 0, d1_ * d2_ * ಗಾತ್ರ(ದಶಮಾಂಶ));
        ಹಿಂತಿರುಗಿ *ಇದು;
    }

ಖಾಸಗಿ:
    ಶೂನ್ಯ* ಸ್ಮರಣೆ_2;
    ದಶಮಾಂಶ* ದತ್ತಾಂಶ_2;
    int32_t d1_;
    int32_t d2_;
};

Array2D::~Array2D()
{
    uprv_free(ಸ್ಮರಣೆ_2);
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    BEGIN,
    INSIDE,
    ಕೊನೆ_2,
    SINGLE
} LSTMClass;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    UNKNOWN,
    ಸಂಕೇತ_POINTS,
    GRAPHEME_CLUSTER,
} EmbeddingType;

ರಚನೆ LSTMData : ಸಾರ್ವಜನಿಕ UMemory {
    LSTMData(UResourceBundle* rb, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);
    ~LSTMData();
    UHashtable* fDict;
    EmbeddingType fType;
    ಸ್ಥಿರಾಂಕ char16_t* fName;
    ConstArray2D fEmbedding;
    ConstArray2D fForwardW;
    ConstArray2D fForwardU;
    ConstArray1D fForwardB;
    ConstArray2D fBackwardW;
    ConstArray2D fBackwardU;
    ConstArray1D fBackwardB;
    ConstArray2D fOutputW;
    ConstArray1D fOutputB;

ಖಾಸಗಿ:
    UResourceBundle* fBundle;
};

LSTMData::LSTMData(UResourceBundle* rb, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
    : fDict(ಶೂನ್ಯಸೂಚಕ), fType(UNKNOWN), fName(ಶೂನ್ಯಸೂಚಕ),
      fBundle(rb)
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (IEEE_754 != 1) {
        ಸ್ಥಿತಿ_2 = U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    Local_UResource_Bundle_ಸೂಚಕ embeddings_res(
        ures_get_By_ಕೀಲಿ(rb, "embeddings", ಶೂನ್ಯಸೂಚಕ, &ಸ್ಥಿತಿ_2));
    int32_t embedding_ಗಾತ್ರ = ures_getInt(embeddings_res.getAlias(), &ಸ್ಥಿತಿ_2);
    Local_UResource_Bundle_ಸೂಚಕ hunits_res(
        ures_get_By_ಕೀಲಿ(rb, "hunits", ಶೂನ್ಯಸೂಚಕ, &ಸ್ಥಿತಿ_2));
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ;
    int32_t hunits = ures_getInt(hunits_res.getAlias(), &ಸ್ಥಿತಿ_2);
    ಸ್ಥಿರಾಂಕ char16_t* type = ures_get_String_By_ಕೀಲಿ(rb, "type", ಶೂನ್ಯಸೂಚಕ, &ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ;
    ಆದರೆ (u_strCompare(type, -1, u"codepoints", -1, ಸುಳ್ಳು) == 0) {
        fType = ಸಂಕೇತ_POINTS;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (u_strCompare(type, -1, u"graphclust", -1, ಸುಳ್ಳು) == 0) {
        fType = GRAPHEME_CLUSTER;
    }
    fName = ures_get_String_By_ಕೀಲಿ(rb, "model", ಶೂನ್ಯಸೂಚಕ, &ಸ್ಥಿತಿ_2);
    Local_UResource_Bundle_ಸೂಚಕ ದತ್ತಾಂಶ_Res(ures_get_By_ಕೀಲಿ(rb, "data", ಶೂನ್ಯಸೂಚಕ, &ಸ್ಥಿತಿ_2));
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ;
    int32_t ದತ್ತಾಂಶ_len = 0;
    ಸ್ಥಿರಾಂಕ int32_t* ದತ್ತಾಂಶ = ures_get_Int_ಸದಿಶ(ದತ್ತಾಂಶ_Res.getAlias(), &ದತ್ತಾಂಶ_len, &ಸ್ಥಿತಿ_2);
    fDict = uhash_open(uhash_hashUChars, uhash_compareUChars, ಶೂನ್ಯಸೂಚಕ, &ಸ್ಥಿತಿ_2);

    ರಾಶಿ_UResource_Bundle ರಾಶಿ_ತಾತ್ಕಾಲಿಕ_Bundle;
    Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ value;
    ures_get_ಮೌಲ್ಯ_With_Fallback(rb, "dict", ರಾಶಿ_ತಾತ್ಕಾಲಿಕ_Bundle.getAlias(), value, ಸ್ಥಿತಿ_2);
    ResourceArray stringArray = value.getArray(ಸ್ಥಿತಿ_2);
    int32_t ಅಂಕೆ_ಸೂಚ್ಯಂಕ = stringArray.get_ಗಾತ್ರ();
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }

    // put dict into hash
    int32_t string_ಉದ್ದಳತೆ;
    ಗಾಗಿ (int32_t idx = 0; idx < ಅಂಕೆ_ಸೂಚ್ಯಂಕ; idx++) {
        stringArray.get_ಮೌಲ್ಯ(idx, value);
        ಸ್ಥಿರಾಂಕ char16_t* str = value.getString(string_ಉದ್ದಳತೆ, ಸ್ಥಿತಿ_2);
        uhash_putiAllowZero(fDict, (ಶೂನ್ಯ*)str, idx, &ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LSTM_VECTORIZER_DEBUG
        ಬರೆ("Assign [");
        ಆಗ (*str != 0x0000) {
            ಬರೆ("U+%04x ", *str);
            str++;
        }
        ಬರೆ("] map to %d\n", idx-1);
#ಆದರೆ_ಅಂತ್ಯ
    }
    int32_t mat1_ಗಾತ್ರ = (ಅಂಕೆ_ಸೂಚ್ಯಂಕ + 1) * embedding_ಗಾತ್ರ;
    int32_t mat2_ಗಾತ್ರ = embedding_ಗಾತ್ರ * 4 * hunits;
    int32_t mat3_ಗಾತ್ರ = hunits * 4 * hunits;
    int32_t mat4_ಗಾತ್ರ = 4 * hunits;
    int32_t mat5_ಗಾತ್ರ = mat2_ಗಾತ್ರ;
    int32_t mat6_ಗಾತ್ರ = mat3_ಗಾತ್ರ;
    int32_t mat7_ಗಾತ್ರ = mat4_ಗಾತ್ರ;
    int32_t mat8_ಗಾತ್ರ = 2 * hunits * 4;
#ಆದರೆ U_DEBUG
    int32_t mat9_ಗಾತ್ರ = 4;
    U_ASSERT(ದತ್ತಾಂಶ_len == mat1_ಗಾತ್ರ + mat2_ಗಾತ್ರ + mat3_ಗಾತ್ರ + mat4_ಗಾತ್ರ + mat5_ಗಾತ್ರ +
        mat6_ಗಾತ್ರ + mat7_ಗಾತ್ರ + mat8_ಗಾತ್ರ + mat9_ಗಾತ್ರ);
#ಆದರೆ_ಅಂತ್ಯ

    fEmbedding.init(ದತ್ತಾಂಶ, (ಅಂಕೆ_ಸೂಚ್ಯಂಕ + 1), embedding_ಗಾತ್ರ);
    ದತ್ತಾಂಶ += mat1_ಗಾತ್ರ;
    fForwardW.init(ದತ್ತಾಂಶ, embedding_ಗಾತ್ರ, 4 * hunits);
    ದತ್ತಾಂಶ += mat2_ಗಾತ್ರ;
    fForwardU.init(ದತ್ತಾಂಶ, hunits, 4 * hunits);
    ದತ್ತಾಂಶ += mat3_ಗಾತ್ರ;
    fForwardB.init(ದತ್ತಾಂಶ, 4 * hunits);
    ದತ್ತಾಂಶ += mat4_ಗಾತ್ರ;
    fBackwardW.init(ದತ್ತಾಂಶ, embedding_ಗಾತ್ರ, 4 * hunits);
    ದತ್ತಾಂಶ += mat5_ಗಾತ್ರ;
    fBackwardU.init(ದತ್ತಾಂಶ, hunits, 4 * hunits);
    ದತ್ತಾಂಶ += mat6_ಗಾತ್ರ;
    fBackwardB.init(ದತ್ತಾಂಶ, 4 * hunits);
    ದತ್ತಾಂಶ += mat7_ಗಾತ್ರ;
    fOutputW.init(ದತ್ತಾಂಶ, 2 * hunits, 4);
    ದತ್ತಾಂಶ += mat8_ಗಾತ್ರ;
    fOutputB.init(ದತ್ತಾಂಶ, 4);
}

LSTMData::~LSTMData() {
    uhash_close(fDict);
    ures_close(fBundle);
}

ವರ್ಗ Vectorizer : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    Vectorizer(UHashtable* dict) : fDict(dict) {}
    ವಾಸ್ತವಿಕ ~Vectorizer();
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ vectorize(UText *ಪಠ್ಯ, int32_t ಆರಂಭ_Pos, int32_t ಕೊನೆ_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ = 0;
ಸಂರಕ್ಷಿತ:
    int32_t string_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ(ಸ್ಥಿರಾಂಕ char16_t* str) ಸ್ಥಿರಾಂಕ {
        UBool ಸಿಕ್ಕಿತು = ಸುಳ್ಳು;
        int32_t ret = uhash_geti_And_ಸಿಕ್ಕಿತು(fDict, (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ*)str, &ಸಿಕ್ಕಿತು);
        ಆದರೆ (!ಸಿಕ್ಕಿತು) {
            ret = fDict->count;
        }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LSTM_VECTORIZER_DEBUG
        ಬರೆ("[");
        ಆಗ (*str != 0x0000) {
            ಬರೆ("U+%04x ", *str);
            str++;
        }
        ಬರೆ("] map to %d\n", ret);
#ಆದರೆ_ಅಂತ್ಯ
        ಹಿಂತಿರುಗಿ ret;
    }

ಖಾಸಗಿ:
    UHashtable* fDict;
};

Vectorizer::~Vectorizer()
{
}

ವರ್ಗ ಸಂಕೇತ_Points_Vectorizer : ಸಾರ್ವಜನಿಕ Vectorizer {
ಸಾರ್ವಜನಿಕ:
    ಸಂಕೇತ_Points_Vectorizer(UHashtable* dict) : Vectorizer(dict) {}
    ವಾಸ್ತವಿಕ ~ಸಂಕೇತ_Points_Vectorizer();
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ vectorize(UText *ಪಠ್ಯ, int32_t ಆರಂಭ_Pos, int32_t ಕೊನೆ_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ override;
};

ಸಂಕೇತ_Points_Vectorizer::~ಸಂಕೇತ_Points_Vectorizer()
{
}

ಶೂನ್ಯ ಸಂಕೇತ_Points_Vectorizer::vectorize(
    UText *ಪಠ್ಯ, int32_t ಆರಂಭ_Pos, int32_t ಕೊನೆ_Pos,
    UVector32 &offsets, UVector32 &indices, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ
{
    ಆದರೆ (offsets.ensure_ಸಾಮರ್ಥ್ಯ(ಕೊನೆ_Pos - ಆರಂಭ_Pos, ಸ್ಥಿತಿ_2) &&
            indices.ensure_ಸಾಮರ್ಥ್ಯ(ಕೊನೆ_Pos - ಆರಂಭ_Pos, ಸ್ಥಿತಿ_2)) {
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ;
        utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, ಆರಂಭ_Pos);
        int32_t current;
        char16_t str[2] = {0, 0};
        ಆಗ (U_SUCCESS(ಸ್ಥಿತಿ_2) &&
               (current = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ))) < ಕೊನೆ_Pos) {
            // Since the LSTMBreakEngine is currently only accept chars in BMP,
            // we can ignore the possibility of hitting supplementary code
            // point.
            str[0] = static_cast<char16_t>(utext_next32(ಪಠ್ಯ));
            U_ASSERT(!U_IS_SURROGATE(str[0]));
            offsets.ಕೂಡಿಸು_Element(current, ಸ್ಥಿತಿ_2);
            indices.ಕೂಡಿಸು_Element(string_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ(str), ಸ್ಥಿತಿ_2);
        }
    }
}

ವರ್ಗ GraphemeClusterVectorizer : ಸಾರ್ವಜನಿಕ Vectorizer {
ಸಾರ್ವಜನಿಕ:
    GraphemeClusterVectorizer(UHashtable* dict)
        : Vectorizer(dict)
    {
    }
    ವಾಸ್ತವಿಕ ~GraphemeClusterVectorizer();
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ vectorize(UText *ಪಠ್ಯ, int32_t ಆರಂಭ_Pos, int32_t ಕೊನೆ_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ override;
};

GraphemeClusterVectorizer::~GraphemeClusterVectorizer()
{
}

constexpr int32_t MAX_GRAPHEME_CLSTER_ಉದ್ದಳತೆ = 10;

ಶೂನ್ಯ GraphemeClusterVectorizer::vectorize(
    UText *ಪಠ್ಯ, int32_t ಆರಂಭ_Pos, int32_t ಕೊನೆ_Pos,
    UVector32 &offsets, UVector32 &indices, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ;
    ಆದರೆ (!offsets.ensure_ಸಾಮರ್ಥ್ಯ(ಕೊನೆ_Pos - ಆರಂಭ_Pos, ಸ್ಥಿತಿ_2) ||
            !indices.ensure_ಸಾಮರ್ಥ್ಯ(ಕೊನೆ_Pos - ಆರಂಭ_Pos, ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ;
    Local_ಸೂಚಕ<BreakIterator> graphemeIter(BreakIterator::createCharacterInstance(Locale(), ಸ್ಥಿತಿ_2));
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ;
    graphemeIter->ಗಣ_ಪಠ್ಯ(ಪಠ್ಯ, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಆರಂಭ_Pos != 0) {
        graphemeIter->preceding(ಆರಂಭ_Pos);
    }
    int32_t ಕೊನೆಯ = ಆರಂಭ_Pos;
    int32_t current = ಆರಂಭ_Pos;
    char16_t str[MAX_GRAPHEME_CLSTER_ಉದ್ದಳತೆ];
    ಆಗ ((current = graphemeIter->next()) != BreakIterator::ಮುಗಿದ_2) {
        ಆದರೆ (current >= ಕೊನೆ_Pos) {
            ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ (current > ಆರಂಭ_Pos) {
            utext_ಹೊರತೆಗೆ(ಪಠ್ಯ, ಕೊನೆಯ, current, str, MAX_GRAPHEME_CLSTER_ಉದ್ದಳತೆ, &ಸ್ಥಿತಿ_2);
            ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ;
            offsets.ಕೂಡಿಸು_Element(ಕೊನೆಯ, ಸ್ಥಿತಿ_2);
            indices.ಕೂಡಿಸು_Element(string_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ(str), ಸ್ಥಿತಿ_2);
            ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ;
        }
        ಕೊನೆಯ = current;
    }
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2) || ಕೊನೆಯ >= ಕೊನೆ_Pos) {
        ಹಿಂತಿರುಗಿ;
    }
    utext_ಹೊರತೆಗೆ(ಪಠ್ಯ, ಕೊನೆಯ, ಕೊನೆ_Pos, str, MAX_GRAPHEME_CLSTER_ಉದ್ದಳತೆ, &ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        offsets.ಕೂಡಿಸು_Element(ಕೊನೆಯ, ಸ್ಥಿತಿ_2);
        indices.ಕೂಡಿಸು_Element(string_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ(str), ಸ್ಥಿತಿ_2);
    }
}

// Computing LSTM as stated in
// https://en.wikipedia.org/wiki/Long_short-term_memory#LSTM_with_a_forget_gate
// ifco is temp array allocate outside which does not need to be
// input/output value but could avoid unnecessary memory alloc/free if passing
// in.
ಶೂನ್ಯ compute(
    int32_t hunits,
    ಸ್ಥಿರಾಂಕ ReadArray2D& W, ಸ್ಥಿರಾಂಕ ReadArray2D& U, ಸ್ಥಿರಾಂಕ ReadArray1D& b,
    ಸ್ಥಿರಾಂಕ ReadArray1D& x, Array1D& h, Array1D& c,
    Array1D& ifco)
{
    // ifco = x * W + h * U + b
    ifco.assign(b)
        .ಕೂಡಿಸು_Dot_ಗುಣಲಬ್ಧ(x, W)
        .ಕೂಡಿಸು_Dot_ಗುಣಲಬ್ಧ(h, U);

    ifco.ತುಂಡು(0*hunits, hunits).sigmoid();  // i: sigmod
    ifco.ತುಂಡು(1*hunits, hunits).sigmoid(); // f: sigmoid
    ifco.ತುಂಡು(2*hunits, hunits).tanh(); // c_: tanh
    ifco.ತುಂಡು(3*hunits, hunits).sigmoid(); // o: sigmod

    c.hadamard_ಗುಣಲಬ್ಧ(ifco.ತುಂಡು(hunits, hunits))
        .ಕೂಡಿಸು_Hadamard_ಗುಣಲಬ್ಧ(ifco.ತುಂಡು(0, hunits), ifco.ತುಂಡು(2*hunits, hunits));

    h.tanh(c)
        .hadamard_ಗುಣಲಬ್ಧ(ifco.ತುಂಡು(3*hunits, hunits));
}

// Minimum word size
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t MIN_ಶಬ್ದ = 2;

// Minimum number of characters for two words
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t MIN_ಶಬ್ದ_ಹರವು = MIN_ಶಬ್ದ * 2;

int32_t
LSTMBreakEngine::divide_ಮೇಲೆ_Dictionary_Range( UText *ಪಠ್ಯ,
                                                int32_t ಆರಂಭ_Pos,
                                                int32_t ಕೊನೆ_Pos,
                                                UVector32 &ಸಿಕ್ಕಿತು_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ 0;
    int32_t begin_ಸಿಕ್ಕಿತು_Break_ಗಾತ್ರ = ಸಿಕ್ಕಿತು_Breaks.size();
    utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, ಆರಂಭ_Pos);
    utext_moveIndex32(ಪಠ್ಯ, MIN_ಶಬ್ದ_ಹರವು);
    ಆದರೆ (utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ) >= ಕೊನೆ_Pos) {
        ಹಿಂತಿರುಗಿ 0;       // Not enough characters for two words
    }
    utext_ಗಣ_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ, ಆರಂಭ_Pos);

    UVector32 offsets(ಸ್ಥಿತಿ_2);
    UVector32 indices(ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ 0;
    fVectorizer->vectorize(ಪಠ್ಯ, ಆರಂಭ_Pos, ಕೊನೆ_Pos, offsets, indices, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ 0;
    int32_t* offsetsBuf = offsets.get_ಬಫರ್();
    int32_t* indicesBuf = indices.get_ಬಫರ್();

    int32_t ಒಳಹರಿವು_seq_len = indices.size();
    int32_t hunits = f_ದತ್ತಾಂಶ->fForwardU.d1();

    // ----- Begin of all the Array memory allocation needed for this function
    // Allocate temp array used inside compute()
    Array1D ifco(4 * hunits, ಸ್ಥಿತಿ_2);

    Array1D c(hunits, ಸ್ಥಿತಿ_2);
    Array1D logp(4, ಸ್ಥಿತಿ_2);

    // TODO: limit size of hBackward. If input_seq_len is too big, we could
    // run out of memory.
    // Backward LSTM
    Array2D hBackward(ಒಳಹರಿವು_seq_len, hunits, ಸ್ಥಿತಿ_2);

    // Allocate fbRow and slice the internal array in two.
    Array1D fb_ಅಡ್ಡಸಾಲು(2 * hunits, ಸ್ಥಿತಿ_2);

    // ----- End of all the Array memory allocation needed for this function
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ 0;

    // To save the needed memory usage, the following is different from the
    // Python or ICU4X implementation. We first perform the Backward LSTM
    // and then merge the iteration of the forward LSTM and the output layer
    // together because we only neetdto remember the h[t-1] for Forward LSTM.
    ಗಾಗಿ (int32_t i = ಒಳಹರಿವು_seq_len - 1; i >= 0; i--) {
        Array1D h_ಅಡ್ಡಸಾಲು = hBackward.ಅಡ್ಡಸಾಲು(i);
        ಆದರೆ (i != ಒಳಹರಿವು_seq_len - 1) {
            h_ಅಡ್ಡಸಾಲು.assign(hBackward.ಅಡ್ಡಸಾಲು(i+1));
        }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LSTM_DEBUG
        ಬರೆ("hRow %d\n", i);
        h_ಅಡ್ಡಸಾಲು.print();
        ಬರೆ("indicesBuf[%d] = %d\n", i, indicesBuf[i]);
        ಬರೆ("fData->fEmbedding.row(indicesBuf[%d]):\n", i);
        f_ದತ್ತಾಂಶ->fEmbedding.ಅಡ್ಡಸಾಲು(indicesBuf[i]).print();
#ಆದರೆ_ಅಂತ್ಯ  // LSTM_DEBUG
        compute(hunits,
                f_ದತ್ತಾಂಶ->fBackwardW, f_ದತ್ತಾಂಶ->fBackwardU, f_ದತ್ತಾಂಶ->fBackwardB,
                f_ದತ್ತಾಂಶ->fEmbedding.ಅಡ್ಡಸಾಲು(indicesBuf[i]),
                h_ಅಡ್ಡಸಾಲು, c, ifco);
    }


    Array1D forward_ಅಡ್ಡಸಾಲು = fb_ಅಡ್ಡಸಾಲು.ತುಂಡು(0, hunits);  // point to first half of data in fbRow.
    Array1D backward_ಅಡ್ಡಸಾಲು = fb_ಅಡ್ಡಸಾಲು.ತುಂಡು(hunits, hunits);  // point to second half of data n fbRow.

    // The following iteration merge the forward LSTM and the output layer
    // together.
    c.clear();  // reuse c since it is the same size.
    ಗಾಗಿ (int32_t i = 0; i < ಒಳಹರಿವು_seq_len; i++) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LSTM_DEBUG
        ಬರೆ("forwardRow %d\n", i);
        forward_ಅಡ್ಡಸಾಲು.print();
#ಆದರೆ_ಅಂತ್ಯ  // LSTM_DEBUG
        // Forward LSTM
        // Calculate the result into forwardRow, which point to the data in the first half
        // of fbRow.
        compute(hunits,
                f_ದತ್ತಾಂಶ->fForwardW, f_ದತ್ತಾಂಶ->fForwardU, f_ದತ್ತಾಂಶ->fForwardB,
                f_ದತ್ತಾಂಶ->fEmbedding.ಅಡ್ಡಸಾಲು(indicesBuf[i]),
                forward_ಅಡ್ಡಸಾಲು, c, ifco);

        // assign the data from hBackward.row(i) to second half of fbRowa.
        backward_ಅಡ್ಡಸಾಲು.assign(hBackward.ಅಡ್ಡಸಾಲು(i));

        logp.assign(f_ದತ್ತಾಂಶ->fOutputB).ಕೂಡಿಸು_Dot_ಗುಣಲಬ್ಧ(fb_ಅಡ್ಡಸಾಲು, f_ದತ್ತಾಂಶ->fOutputW);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LSTM_DEBUG
        ಬರೆ("backwardRow %d\n", i);
        backward_ಅಡ್ಡಸಾಲು.print();
        ಬರೆ("logp %d\n", i);
        logp.print();
#ಆದರೆ_ಅಂತ್ಯ  // LSTM_DEBUG

        // current = argmax(logp)
        LSTMClass current = static_cast<LSTMClass>(logp.max_ಸೂಚ್ಯಂಕ());
        // BIES logic.
        ಆದರೆ (current == BEGIN || current == SINGLE) {
            ಆದರೆ (i != 0) {
                ಸಿಕ್ಕಿತು_Breaks.ಕೂಡಿಸು_Element(offsetsBuf[i], ಸ್ಥಿತಿ_2);
                ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ 0;
            }
        }
    }
    ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು_Breaks.size() - begin_ಸಿಕ್ಕಿತು_Break_ಗಾತ್ರ;
}

Vectorizer* createVectorizer(ಸ್ಥಿರಾಂಕ LSTMData* ದತ್ತಾಂಶ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆಯ್ಕೆ (ದತ್ತಾಂಶ->fType) {
        ಸ್ಥಿತಿ ಸಂಕೇತ_POINTS:
            ಹಿಂತಿರುಗಿ ಹೊಸ ಸಂಕೇತ_Points_Vectorizer(ದತ್ತಾಂಶ->fDict);
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ GRAPHEME_CLUSTER:
            ಹಿಂತಿರುಗಿ ಹೊಸ GraphemeClusterVectorizer(ದತ್ತಾಂಶ->fDict);
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            ನಿಲ್ಲಿಸು;
    }
    UPRV_UNREACHABLE_EXIT;
}

LSTMBreakEngine::LSTMBreakEngine(ಸ್ಥಿರಾಂಕ LSTMData* ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ Unicode_ಗಣ& ಗಣ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
    : DictionaryBreakEngine(), f_ದತ್ತಾಂಶ(ದತ್ತಾಂಶ), fVectorizer(createVectorizer(f_ದತ್ತಾಂಶ, ಸ್ಥಿತಿ_2))
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
      f_ದತ್ತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;  // If failure, we should not delete fData in destructor because the caller will do so.
      ಹಿಂತಿರುಗಿ;
    }
    ಗಣ_Characters(ಗಣ);
}

LSTMBreakEngine::~LSTMBreakEngine() {
    ಅಳಿಸು f_ದತ್ತಾಂಶ;
    ಅಳಿಸು fVectorizer;
}

ಸ್ಥಿರಾಂಕ char16_t* LSTMBreakEngine::name() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ f_ದತ್ತಾಂಶ->fName;
}

UnicodeString defaultLSTM(UScript_ಸಂಕೇತ script, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &ಸ್ಥಿತಿ_2);
    b = ures_get_By_ಕೀಲಿ_With_Fallback(b, "lstm", b, &ಸ್ಥಿತಿ_2);
    UnicodeString ಫಲಿತಾಂಶ = ures_get_Unicode_String_By_ಕೀಲಿ(b, uscript_getShortName(script), &ಸ್ಥಿತಿ_2);
    ures_close(b);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

U_CAPI ಸ್ಥಿರಾಂಕ LSTMData* U_EXPORT2 CreateLSTMDataForScript(UScript_ಸಂಕೇತ script, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (script != USCRIPT_KHMER && script != USCRIPT_LAO && script != USCRIPT_MYANMAR && script != USCRIPT_THAI) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    UnicodeString name = defaultLSTM(script, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    CharString namebuf;
    namebuf.appendInvariantChars(name, ಸ್ಥಿತಿ_2).truncate(namebuf.ಕೊನೆಯ_ಸೂಚ್ಯಂಕ_ದ('.'));

    Local_UResource_Bundle_ಸೂಚಕ rb(
        ures_open_ನೇರ(U_ICUDATA_BRKITR, namebuf.ದತ್ತಾಂಶ(), &ಸ್ಥಿತಿ_2));
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;

    ಹಿಂತಿರುಗಿ CreateLSTMData(rb.orphan(), ಸ್ಥಿತಿ_2);
}

U_CAPI ಸ್ಥಿರಾಂಕ LSTMData* U_EXPORT2 CreateLSTMData(UResourceBundle* rb, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಸ್ಥಿರಾಂಕ LSTMData* ಫಲಿತಾಂಶ = ಹೊಸ LSTMData(rb, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಅಳಿಸು ಫಲಿತಾಂಶ;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

U_CAPI ಸ್ಥಿರಾಂಕ LanguageBreakEngine* U_EXPORT2
CreateLSTMBreakEngine(UScript_ಸಂಕೇತ script, ಸ್ಥಿರಾಂಕ LSTMData* ದತ್ತಾಂಶ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    UnicodeString unicode_ಗಣ_String;
    ಆಯ್ಕೆ(script) {
        ಸ್ಥಿತಿ USCRIPT_THAI:
            unicode_ಗಣ_String = UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]");
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ USCRIPT_MYANMAR:
            unicode_ಗಣ_String = UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]");
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            ಅಳಿಸು ದತ್ತಾಂಶ;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    Unicode_ಗಣ unicode_ಗಣ;
    unicode_ಗಣ.apply_ನಮೂನೆ(unicode_ಗಣ_String, ಸ್ಥಿತಿ_2);
    ಸ್ಥಿರಾಂಕ LanguageBreakEngine* engine = ಹೊಸ LSTMBreakEngine(ದತ್ತಾಂಶ, unicode_ಗಣ, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2) || engine == ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ (engine != ಶೂನ್ಯಸೂಚಕ) {
            ಅಳಿಸು engine;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        }
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ engine;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2 DeleteLSTMData(ಸ್ಥಿರಾಂಕ LSTMData* ದತ್ತಾಂಶ)
{
    ಅಳಿಸು ದತ್ತಾಂಶ;
}

U_CAPI ಸ್ಥಿರಾಂಕ char16_t* U_EXPORT2 LSTMDataName(ಸ್ಥಿರಾಂಕ LSTMData* ದತ್ತಾಂಶ)
{
    ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ->fName;
}

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_BREAK_ITERATION */
