// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ضمن <complex>
#ضمن <utility>

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_BREAK_ITERATION

#ضمن "brkeng.h"
#ضمن "charstr.h"
#ضمن "cmemory.h"
#ضمن "lstmbe.h"
#ضمن "putilimp.h"
#ضمن "uassert.h"
#ضمن "ubrkimpl.h"
#ضمن "uresimp.h"
#ضمن "uvectr32.h"
#ضمن "uvector.h"

#ضمن "unicode/brkiter.h"
#ضمن "unicode/resbund.h"
#ضمن "unicode/ubrk.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/utf.h"

U_NAMESPACE_BEGIN

// Uncomment the following #define to debug.
// #define LSTM_DEBUG 1
// #define LSTM_VECTORIZER_DEBUG 1

/**
 * Interface for reading 1D array.
 */
صنف ReadArray1D {
عام:
    مجازي ~ReadArray1D();
    مجازي int32_t d1() ثابت = 0;
    مجازي عائم get(int32_t i) ثابت = 0;

#جيڪڏهن_وصف_ٿيل LSTM_DEBUG
    فراغ print() ثابت {
        لکو("\n[");
        لكل (int32_t i = 0; i < d1(); i++) {
           لکو("%0.8e ", get(i));
           إذا (i % 4 == 3) لکو("\n");
        }
        لکو("]\n");
    }
#جيڪڏهن_پورو
};

ReadArray1D::~ReadArray1D()
{
}

/**
 * Interface for reading 2D array.
 */
صنف ReadArray2D {
عام:
    مجازي ~ReadArray2D();
    مجازي int32_t d1() ثابت = 0;
    مجازي int32_t d2() ثابت = 0;
    مجازي عائم get(int32_t i, int32_t j) ثابت = 0;
};

ReadArray2D::~ReadArray2D()
{
}

/**
 * A class to index a float array as a 1D Array without owning the pointer or
 * copy the data.
 */
صنف ConstArray1D : عام ReadArray1D {
عام:
    ConstArray1D() : معلومات_2(خالي_اشارو), d1_(0) {}

    ConstArray1D(ثابت عائم* معلومات, int32_t d1) : معلومات_2(معلومات), d1_(d1) {}

    مجازي ~ConstArray1D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    فراغ init(ثابت int32_t* معلومات, int32_t d1) {
        U_ASSERT(IEEE_754 == 1);
        معلومات_2 = reinterpret_cast<ثابت عائم*>(معلومات);
        d1_ = d1;
    }

    // ReadArray1D methods.
    مجازي int32_t d1() ثابت override { أرجع d1_; }
    مجازي عائم get(int32_t i) ثابت override {
        U_ASSERT(i < d1_);
        أرجع معلومات_2[i];
    }

خاص:
    ثابت عائم* معلومات_2;
    int32_t d1_;
};

ConstArray1D::~ConstArray1D()
{
}

/**
 * A class to index a float array as a 2D Array without owning the pointer or
 * copy the data.
 */
صنف ConstArray2D : عام ReadArray2D {
عام:
    ConstArray2D() : معلومات_2(خالي_اشارو), d1_(0), d2_(0) {}

    ConstArray2D(ثابت عائم* معلومات, int32_t d1, int32_t d2)
        : معلومات_2(معلومات), d1_(d1), d2_(d2) {}

    مجازي ~ConstArray2D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    فراغ init(ثابت int32_t* معلومات, int32_t d1, int32_t d2) {
        U_ASSERT(IEEE_754 == 1);
        معلومات_2 = reinterpret_cast<ثابت عائم*>(معلومات);
        d1_ = d1;
        d2_ = d2;
    }

    // ReadArray2D methods.
    مضمن int32_t d1() ثابت override { أرجع d1_; }
    مضمن int32_t d2() ثابت override { أرجع d2_; }
    عائم get(int32_t i, int32_t j) ثابت override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        أرجع معلومات_2[i * d2_ + j];
    }

    // Expose the ith row as a ConstArray1D
    مضمن ConstArray1D صف(int32_t i) ثابت {
        U_ASSERT(i < d1_);
        أرجع ConstArray1D(معلومات_2 + i * d2_, d2_);
    }

خاص:
    ثابت عائم* معلومات_2;
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
صنف Array1D : عام ReadArray1D {
عام:
    Array1D() : يادگيري_2(خالي_اشارو), معلومات_2(خالي_اشارو), d1_(0) {}
    Array1D(int32_t d1, UError_ڪوڊ &حالت)
        : يادگيري_2(uprv_malloc(d1 * حجم(عائم))),
          معلومات_2(static_cast<عائم*>(يادگيري_2)), d1_(d1) {
        إذا (U_SUCCESS(حالت)) {
            إذا (يادگيري_2 == خالي_اشارو) {
                حالت = U_يادگيري_ورهاست_ERROR;
                أرجع;
            }
            clear();
        }
    }

    مجازي ~Array1D();

    // A special constructor which does not own the memory but writeable
    // as a slice of an array.
    Array1D(عائم* معلومات, int32_t d1)
        : يادگيري_2(خالي_اشارو), معلومات_2(معلومات), d1_(d1) {}

    // ReadArray1D methods.
    مجازي int32_t d1() ثابت override { أرجع d1_; }
    مجازي عائم get(int32_t i) ثابت override {
        U_ASSERT(i < d1_);
        أرجع معلومات_2[i];
    }

    // Return the index which point to the max data in the array.
    مضمن int32_t max_اشاريو() ثابت {
        int32_t اشاريو = 0;
        عائم max = معلومات_2[0];
        لكل (int32_t i = 1; i < d1_; i++) {
            إذا (معلومات_2[i] > max) {
                max = معلومات_2[i];
                اشاريو = i;
            }
        }
        أرجع اشاريو;
    }

    // Slice part of the array to a new one.
    مضمن Array1D ٽڪرو(int32_t from, int32_t size) ثابت {
        U_ASSERT(from >= 0);
        U_ASSERT(from < d1_);
        U_ASSERT(from + size <= d1_);
        أرجع Array1D(معلومات_2 + from, size);
    }

    // Add dot product of a 1D array and a 2D array into this one.
    مضمن Array1D& جوڙيو_Dot_حاصل_ضرب(ثابت ReadArray1D& a, ثابت ReadArray2D& b) {
        U_ASSERT(a.d1() == b.d1());
        U_ASSERT(b.d2() == d1());
        لكل (int32_t i = 0; i < d1(); i++) {
            لكل (int32_t j = 0; j < a.d1(); j++) {
                معلومات_2[i] += a.get(j) * b.get(j, i);
            }
        }
        أرجع *هذا;
    }

    // Hadamard Product the values of another array of the same size into this one.
    مضمن Array1D& hadamard_حاصل_ضرب(ثابت ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        لكل (int32_t i = 0; i < d1(); i++) {
            معلومات_2[i] *= a.get(i);
        }
        أرجع *هذا;
    }

    // Add the Hadamard Product of two arrays of the same size into this one.
    مضمن Array1D& جوڙيو_Hadamard_حاصل_ضرب(ثابت ReadArray1D& a, ثابت ReadArray1D& b) {
        U_ASSERT(a.d1() == d1());
        U_ASSERT(b.d1() == d1());
        لكل (int32_t i = 0; i < d1(); i++) {
            معلومات_2[i] += a.get(i) * b.get(i);
        }
        أرجع *هذا;
    }

    // Add the values of another array of the same size into this one.
    مضمن Array1D& جوڙيو(ثابت ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        لكل (int32_t i = 0; i < d1(); i++) {
            معلومات_2[i] += a.get(i);
        }
        أرجع *هذا;
    }

    // Assign the values of another array of the same size into this one.
    مضمن Array1D& assign(ثابت ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        لكل (int32_t i = 0; i < d1(); i++) {
            معلومات_2[i] = a.get(i);
        }
        أرجع *هذا;
    }

    // Apply tanh to all the elements in the array.
    مضمن Array1D& tanh() {
        أرجع tanh(*هذا);
    }

    // Apply tanh of a and store into this array.
    مضمن Array1D& tanh(ثابت Array1D& a) {
        U_ASSERT(a.d1() == d1());
        لكل (int32_t i = 0; i < d1_; i++) {
            معلومات_2[i] = std::tanh(a.get(i));
        }
        أرجع *هذا;
    }

    // Apply sigmoid to all the elements in the array.
    مضمن Array1D& sigmoid() {
        لكل (int32_t i = 0; i < d1_; i++) {
            معلومات_2[i] = 1.0f/(1.0f + expf(-معلومات_2[i]));
        }
        أرجع *هذا;
    }

    مضمن Array1D& clear() {
        uprv_memset(معلومات_2, 0, d1_ * حجم(عائم));
        أرجع *هذا;
    }

خاص:
    فراغ* يادگيري_2;
    عائم* معلومات_2;
    int32_t d1_;
};

Array1D::~Array1D()
{
    uprv_free(يادگيري_2);
}

صنف Array2D : عام ReadArray2D {
عام:
    Array2D() : يادگيري_2(خالي_اشارو), معلومات_2(خالي_اشارو), d1_(0), d2_(0) {}
    Array2D(int32_t d1, int32_t d2, UError_ڪوڊ &حالت)
        : يادگيري_2(uprv_malloc(d1 * d2 * حجم(عائم))),
          معلومات_2(static_cast<عائم*>(يادگيري_2)), d1_(d1), d2_(d2) {
        إذا (U_SUCCESS(حالت)) {
            إذا (يادگيري_2 == خالي_اشارو) {
                حالت = U_يادگيري_ورهاست_ERROR;
                أرجع;
            }
            clear();
        }
    }
    مجازي ~Array2D();

    // ReadArray2D methods.
    مجازي int32_t d1() ثابت override { أرجع d1_; }
    مجازي int32_t d2() ثابت override { أرجع d2_; }
    مجازي عائم get(int32_t i, int32_t j) ثابت override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        أرجع معلومات_2[i * d2_ + j];
    }

    مضمن Array1D صف(int32_t i) ثابت {
        U_ASSERT(i < d1_);
        أرجع Array1D(معلومات_2 + i * d2_, d2_);
    }

    مضمن Array2D& clear() {
        uprv_memset(معلومات_2, 0, d1_ * d2_ * حجم(عائم));
        أرجع *هذا;
    }

خاص:
    فراغ* يادگيري_2;
    عائم* معلومات_2;
    int32_t d1_;
    int32_t d2_;
};

Array2D::~Array2D()
{
    uprv_free(يادگيري_2);
}

تعريفنوع تعداد {
    BEGIN,
    INSIDE,
    پڇاڙي_2,
    SINGLE
} LSTMClass;

تعريفنوع تعداد {
    UNKNOWN,
    ڪوڊ_POINTS,
    GRAPHEME_CLUSTER,
} EmbeddingType;

بنية LSTMData : عام UMemory {
    LSTMData(UResourceBundle* rb, UError_ڪوڊ &حالت);
    ~LSTMData();
    UHashtable* fDict;
    EmbeddingType fType;
    ثابت char16_t* fName;
    ConstArray2D fEmbedding;
    ConstArray2D fForwardW;
    ConstArray2D fForwardU;
    ConstArray1D fForwardB;
    ConstArray2D fBackwardW;
    ConstArray2D fBackwardU;
    ConstArray1D fBackwardB;
    ConstArray2D fOutputW;
    ConstArray1D fOutputB;

خاص:
    UResourceBundle* fBundle;
};

LSTMData::LSTMData(UResourceBundle* rb, UError_ڪوڊ &حالت)
    : fDict(خالي_اشارو), fType(UNKNOWN), fName(خالي_اشارو),
      fBundle(rb)
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    إذا (IEEE_754 != 1) {
        حالت = U_UNSUPPORTED_ERROR;
        أرجع;
    }
    Local_UResource_Bundle_اشارو embeddings_res(
        ures_get_By_ڪنجي(rb, "embeddings", خالي_اشارو, &حالت));
    int32_t embedding_سائيز = ures_getInt(embeddings_res.getAlias(), &حالت);
    Local_UResource_Bundle_اشارو hunits_res(
        ures_get_By_ڪنجي(rb, "hunits", خالي_اشارو, &حالت));
    إذا (U_FAILURE(حالت)) أرجع;
    int32_t hunits = ures_getInt(hunits_res.getAlias(), &حالت);
    ثابت char16_t* type = ures_get_String_By_ڪنجي(rb, "type", خالي_اشارو, &حالت);
    إذا (U_FAILURE(حالت)) أرجع;
    إذا (u_strCompare(type, -1, u"codepoints", -1, خطأ) == 0) {
        fType = ڪوڊ_POINTS;
    } وإلا إذا (u_strCompare(type, -1, u"graphclust", -1, خطأ) == 0) {
        fType = GRAPHEME_CLUSTER;
    }
    fName = ures_get_String_By_ڪنجي(rb, "model", خالي_اشارو, &حالت);
    Local_UResource_Bundle_اشارو معلومات_Res(ures_get_By_ڪنجي(rb, "data", خالي_اشارو, &حالت));
    إذا (U_FAILURE(حالت)) أرجع;
    int32_t معلومات_len = 0;
    ثابت int32_t* معلومات = ures_get_Int_ويڪٽر(معلومات_Res.getAlias(), &معلومات_len, &حالت);
    fDict = uhash_open(uhash_hashUChars, uhash_compareUChars, خالي_اشارو, &حالت);

    ڍير_UResource_Bundle ڍير_عارضي_Bundle;
    Resource_معلومات_قدر value;
    ures_get_قدر_With_Fallback(rb, "dict", ڍير_عارضي_Bundle.getAlias(), value, حالت);
    ResourceArray stringArray = value.getArray(حالت);
    int32_t انگ_اشاريو = stringArray.get_سائيز();
    إذا (U_FAILURE(حالت)) { أرجع; }

    // put dict into hash
    int32_t string_ڊيگهائي;
    لكل (int32_t idx = 0; idx < انگ_اشاريو; idx++) {
        stringArray.get_قدر(idx, value);
        ثابت char16_t* str = value.getString(string_ڊيگهائي, حالت);
        uhash_putiAllowZero(fDict, (فراغ*)str, idx, &حالت);
        إذا (U_FAILURE(حالت)) أرجع;
#جيڪڏهن_وصف_ٿيل LSTM_VECTORIZER_DEBUG
        لکو("Assign [");
        طالما (*str != 0x0000) {
            لکو("U+%04x ", *str);
            str++;
        }
        لکو("] map to %d\n", idx-1);
#جيڪڏهن_پورو
    }
    int32_t mat1_سائيز = (انگ_اشاريو + 1) * embedding_سائيز;
    int32_t mat2_سائيز = embedding_سائيز * 4 * hunits;
    int32_t mat3_سائيز = hunits * 4 * hunits;
    int32_t mat4_سائيز = 4 * hunits;
    int32_t mat5_سائيز = mat2_سائيز;
    int32_t mat6_سائيز = mat3_سائيز;
    int32_t mat7_سائيز = mat4_سائيز;
    int32_t mat8_سائيز = 2 * hunits * 4;
#إذا U_DEBUG
    int32_t mat9_سائيز = 4;
    U_ASSERT(معلومات_len == mat1_سائيز + mat2_سائيز + mat3_سائيز + mat4_سائيز + mat5_سائيز +
        mat6_سائيز + mat7_سائيز + mat8_سائيز + mat9_سائيز);
#جيڪڏهن_پورو

    fEmbedding.init(معلومات, (انگ_اشاريو + 1), embedding_سائيز);
    معلومات += mat1_سائيز;
    fForwardW.init(معلومات, embedding_سائيز, 4 * hunits);
    معلومات += mat2_سائيز;
    fForwardU.init(معلومات, hunits, 4 * hunits);
    معلومات += mat3_سائيز;
    fForwardB.init(معلومات, 4 * hunits);
    معلومات += mat4_سائيز;
    fBackwardW.init(معلومات, embedding_سائيز, 4 * hunits);
    معلومات += mat5_سائيز;
    fBackwardU.init(معلومات, hunits, 4 * hunits);
    معلومات += mat6_سائيز;
    fBackwardB.init(معلومات, 4 * hunits);
    معلومات += mat7_سائيز;
    fOutputW.init(معلومات, 2 * hunits, 4);
    معلومات += mat8_سائيز;
    fOutputB.init(معلومات, 4);
}

LSTMData::~LSTMData() {
    uhash_close(fDict);
    ures_close(fBundle);
}

صنف Vectorizer : عام UMemory {
عام:
    Vectorizer(UHashtable* dict) : fDict(dict) {}
    مجازي ~Vectorizer();
    مجازي فراغ vectorize(UText *متن, int32_t شروعات_Pos, int32_t پڇاڙي_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_ڪوڊ &حالت) ثابت = 0;
محمي:
    int32_t string_منزل_اشاريو(ثابت char16_t* str) ثابت {
        UBool مليو = خطأ;
        int32_t ret = uhash_geti_And_مليو(fDict, (ثابت فراغ*)str, &مليو);
        إذا (!مليو) {
            ret = fDict->count;
        }
#جيڪڏهن_وصف_ٿيل LSTM_VECTORIZER_DEBUG
        لکو("[");
        طالما (*str != 0x0000) {
            لکو("U+%04x ", *str);
            str++;
        }
        لکو("] map to %d\n", ret);
#جيڪڏهن_پورو
        أرجع ret;
    }

خاص:
    UHashtable* fDict;
};

Vectorizer::~Vectorizer()
{
}

صنف ڪوڊ_Points_Vectorizer : عام Vectorizer {
عام:
    ڪوڊ_Points_Vectorizer(UHashtable* dict) : Vectorizer(dict) {}
    مجازي ~ڪوڊ_Points_Vectorizer();
    مجازي فراغ vectorize(UText *متن, int32_t شروعات_Pos, int32_t پڇاڙي_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_ڪوڊ &حالت) ثابت override;
};

ڪوڊ_Points_Vectorizer::~ڪوڊ_Points_Vectorizer()
{
}

فراغ ڪوڊ_Points_Vectorizer::vectorize(
    UText *متن, int32_t شروعات_Pos, int32_t پڇاڙي_Pos,
    UVector32 &offsets, UVector32 &indices, UError_ڪوڊ &حالت) ثابت
{
    إذا (offsets.ensure_صلاحيت(پڇاڙي_Pos - شروعات_Pos, حالت) &&
            indices.ensure_صلاحيت(پڇاڙي_Pos - شروعات_Pos, حالت)) {
        إذا (U_FAILURE(حالت)) أرجع;
        utext_سيٽ_Native_اشاريو(متن, شروعات_Pos);
        int32_t current;
        char16_t str[2] = {0, 0};
        طالما (U_SUCCESS(حالت) &&
               (current = static_cast<int32_t>(utext_get_Native_اشاريو(متن))) < پڇاڙي_Pos) {
            // Since the LSTMBreakEngine is currently only accept chars in BMP,
            // we can ignore the possibility of hitting supplementary code
            // point.
            str[0] = static_cast<char16_t>(utext_next32(متن));
            U_ASSERT(!U_IS_SURROGATE(str[0]));
            offsets.جوڙيو_Element(current, حالت);
            indices.جوڙيو_Element(string_منزل_اشاريو(str), حالت);
        }
    }
}

صنف GraphemeClusterVectorizer : عام Vectorizer {
عام:
    GraphemeClusterVectorizer(UHashtable* dict)
        : Vectorizer(dict)
    {
    }
    مجازي ~GraphemeClusterVectorizer();
    مجازي فراغ vectorize(UText *متن, int32_t شروعات_Pos, int32_t پڇاڙي_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_ڪوڊ &حالت) ثابت override;
};

GraphemeClusterVectorizer::~GraphemeClusterVectorizer()
{
}

constexpr int32_t MAX_GRAPHEME_CLSTER_ڊيگهائي = 10;

فراغ GraphemeClusterVectorizer::vectorize(
    UText *متن, int32_t شروعات_Pos, int32_t پڇاڙي_Pos,
    UVector32 &offsets, UVector32 &indices, UError_ڪوڊ &حالت) ثابت
{
    إذا (U_FAILURE(حالت)) أرجع;
    إذا (!offsets.ensure_صلاحيت(پڇاڙي_Pos - شروعات_Pos, حالت) ||
            !indices.ensure_صلاحيت(پڇاڙي_Pos - شروعات_Pos, حالت)) {
        أرجع;
    }
    إذا (U_FAILURE(حالت)) أرجع;
    Local_اشارو<BreakIterator> graphemeIter(BreakIterator::createCharacterInstance(Locale(), حالت));
    إذا (U_FAILURE(حالت)) أرجع;
    graphemeIter->سيٽ_متن(متن, حالت);
    إذا (U_FAILURE(حالت)) أرجع;

    إذا (شروعات_Pos != 0) {
        graphemeIter->preceding(شروعات_Pos);
    }
    int32_t آخري = شروعات_Pos;
    int32_t current = شروعات_Pos;
    char16_t str[MAX_GRAPHEME_CLSTER_ڊيگهائي];
    طالما ((current = graphemeIter->next()) != BreakIterator::پورو_2) {
        إذا (current >= پڇاڙي_Pos) {
            اكسر;
        }
        إذا (current > شروعات_Pos) {
            utext_ڪڍو(متن, آخري, current, str, MAX_GRAPHEME_CLSTER_ڊيگهائي, &حالت);
            إذا (U_FAILURE(حالت)) أرجع;
            offsets.جوڙيو_Element(آخري, حالت);
            indices.جوڙيو_Element(string_منزل_اشاريو(str), حالت);
            إذا (U_FAILURE(حالت)) أرجع;
        }
        آخري = current;
    }
    إذا (U_FAILURE(حالت) || آخري >= پڇاڙي_Pos) {
        أرجع;
    }
    utext_ڪڍو(متن, آخري, پڇاڙي_Pos, str, MAX_GRAPHEME_CLSTER_ڊيگهائي, &حالت);
    إذا (U_SUCCESS(حالت)) {
        offsets.جوڙيو_Element(آخري, حالت);
        indices.جوڙيو_Element(string_منزل_اشاريو(str), حالت);
    }
}

// Computing LSTM as stated in
// https://en.wikipedia.org/wiki/Long_short-term_memory#LSTM_with_a_forget_gate
// ifco is temp array allocate outside which does not need to be
// input/output value but could avoid unnecessary memory alloc/free if passing
// in.
فراغ compute(
    int32_t hunits,
    ثابت ReadArray2D& W, ثابت ReadArray2D& U, ثابت ReadArray1D& b,
    ثابت ReadArray1D& x, Array1D& h, Array1D& c,
    Array1D& ifco)
{
    // ifco = x * W + h * U + b
    ifco.assign(b)
        .جوڙيو_Dot_حاصل_ضرب(x, W)
        .جوڙيو_Dot_حاصل_ضرب(h, U);

    ifco.ٽڪرو(0*hunits, hunits).sigmoid();  // i: sigmod
    ifco.ٽڪرو(1*hunits, hunits).sigmoid(); // f: sigmoid
    ifco.ٽڪرو(2*hunits, hunits).tanh(); // c_: tanh
    ifco.ٽڪرو(3*hunits, hunits).sigmoid(); // o: sigmod

    c.hadamard_حاصل_ضرب(ifco.ٽڪرو(hunits, hunits))
        .جوڙيو_Hadamard_حاصل_ضرب(ifco.ٽڪرو(0, hunits), ifco.ٽڪرو(2*hunits, hunits));

    h.tanh(c)
        .hadamard_حاصل_ضرب(ifco.ٽڪرو(3*hunits, hunits));
}

// Minimum word size
ساكن ثابت int32_t MIN_لفظ = 2;

// Minimum number of characters for two words
ساكن ثابت int32_t MIN_لفظ_وسعت = MIN_لفظ * 2;

int32_t
LSTMBreakEngine::divide_مٿي_Dictionary_Range( UText *متن,
                                                int32_t شروعات_Pos,
                                                int32_t پڇاڙي_Pos,
                                                UVector32 &مليو_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_ڪوڊ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    int32_t begin_مليو_Break_سائيز = مليو_Breaks.size();
    utext_سيٽ_Native_اشاريو(متن, شروعات_Pos);
    utext_moveIndex32(متن, MIN_لفظ_وسعت);
    إذا (utext_get_Native_اشاريو(متن) >= پڇاڙي_Pos) {
        أرجع 0;       // Not enough characters for two words
    }
    utext_سيٽ_Native_اشاريو(متن, شروعات_Pos);

    UVector32 offsets(حالت);
    UVector32 indices(حالت);
    إذا (U_FAILURE(حالت)) أرجع 0;
    fVectorizer->vectorize(متن, شروعات_Pos, پڇاڙي_Pos, offsets, indices, حالت);
    إذا (U_FAILURE(حالت)) أرجع 0;
    int32_t* offsetsBuf = offsets.get_بفر();
    int32_t* indicesBuf = indices.get_بفر();

    int32_t داخلا_seq_len = indices.size();
    int32_t hunits = f_معلومات->fForwardU.d1();

    // ----- Begin of all the Array memory allocation needed for this function
    // Allocate temp array used inside compute()
    Array1D ifco(4 * hunits, حالت);

    Array1D c(hunits, حالت);
    Array1D logp(4, حالت);

    // TODO: limit size of hBackward. If input_seq_len is too big, we could
    // run out of memory.
    // Backward LSTM
    Array2D hBackward(داخلا_seq_len, hunits, حالت);

    // Allocate fbRow and slice the internal array in two.
    Array1D fb_صف(2 * hunits, حالت);

    // ----- End of all the Array memory allocation needed for this function
    إذا (U_FAILURE(حالت)) أرجع 0;

    // To save the needed memory usage, the following is different from the
    // Python or ICU4X implementation. We first perform the Backward LSTM
    // and then merge the iteration of the forward LSTM and the output layer
    // together because we only neetdto remember the h[t-1] for Forward LSTM.
    لكل (int32_t i = داخلا_seq_len - 1; i >= 0; i--) {
        Array1D h_صف = hBackward.صف(i);
        إذا (i != داخلا_seq_len - 1) {
            h_صف.assign(hBackward.صف(i+1));
        }
#جيڪڏهن_وصف_ٿيل LSTM_DEBUG
        لکو("hRow %d\n", i);
        h_صف.print();
        لکو("indicesBuf[%d] = %d\n", i, indicesBuf[i]);
        لکو("fData->fEmbedding.row(indicesBuf[%d]):\n", i);
        f_معلومات->fEmbedding.صف(indicesBuf[i]).print();
#جيڪڏهن_پورو  // LSTM_DEBUG
        compute(hunits,
                f_معلومات->fBackwardW, f_معلومات->fBackwardU, f_معلومات->fBackwardB,
                f_معلومات->fEmbedding.صف(indicesBuf[i]),
                h_صف, c, ifco);
    }


    Array1D forward_صف = fb_صف.ٽڪرو(0, hunits);  // point to first half of data in fbRow.
    Array1D backward_صف = fb_صف.ٽڪرو(hunits, hunits);  // point to second half of data n fbRow.

    // The following iteration merge the forward LSTM and the output layer
    // together.
    c.clear();  // reuse c since it is the same size.
    لكل (int32_t i = 0; i < داخلا_seq_len; i++) {
#جيڪڏهن_وصف_ٿيل LSTM_DEBUG
        لکو("forwardRow %d\n", i);
        forward_صف.print();
#جيڪڏهن_پورو  // LSTM_DEBUG
        // Forward LSTM
        // Calculate the result into forwardRow, which point to the data in the first half
        // of fbRow.
        compute(hunits,
                f_معلومات->fForwardW, f_معلومات->fForwardU, f_معلومات->fForwardB,
                f_معلومات->fEmbedding.صف(indicesBuf[i]),
                forward_صف, c, ifco);

        // assign the data from hBackward.row(i) to second half of fbRowa.
        backward_صف.assign(hBackward.صف(i));

        logp.assign(f_معلومات->fOutputB).جوڙيو_Dot_حاصل_ضرب(fb_صف, f_معلومات->fOutputW);
#جيڪڏهن_وصف_ٿيل LSTM_DEBUG
        لکو("backwardRow %d\n", i);
        backward_صف.print();
        لکو("logp %d\n", i);
        logp.print();
#جيڪڏهن_پورو  // LSTM_DEBUG

        // current = argmax(logp)
        LSTMClass current = static_cast<LSTMClass>(logp.max_اشاريو());
        // BIES logic.
        إذا (current == BEGIN || current == SINGLE) {
            إذا (i != 0) {
                مليو_Breaks.جوڙيو_Element(offsetsBuf[i], حالت);
                إذا (U_FAILURE(حالت)) أرجع 0;
            }
        }
    }
    أرجع مليو_Breaks.size() - begin_مليو_Break_سائيز;
}

Vectorizer* createVectorizer(ثابت LSTMData* معلومات, UError_ڪوڊ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع خالي_اشارو;
    }
    بدل (معلومات->fType) {
        حالة ڪوڊ_POINTS:
            أرجع جديد ڪوڊ_Points_Vectorizer(معلومات->fDict);
            اكسر;
        حالة GRAPHEME_CLUSTER:
            أرجع جديد GraphemeClusterVectorizer(معلومات->fDict);
            اكسر;
        افتراضي:
            اكسر;
    }
    UPRV_UNREACHABLE_EXIT;
}

LSTMBreakEngine::LSTMBreakEngine(ثابت LSTMData* معلومات, ثابت Unicode_سيٽ& سيٽ, UError_ڪوڊ &حالت)
    : DictionaryBreakEngine(), f_معلومات(معلومات), fVectorizer(createVectorizer(f_معلومات, حالت))
{
    إذا (U_FAILURE(حالت)) {
      f_معلومات = خالي_اشارو;  // If failure, we should not delete fData in destructor because the caller will do so.
      أرجع;
    }
    سيٽ_Characters(سيٽ);
}

LSTMBreakEngine::~LSTMBreakEngine() {
    ميٽيو f_معلومات;
    ميٽيو fVectorizer;
}

ثابت char16_t* LSTMBreakEngine::name() ثابت {
    أرجع f_معلومات->fName;
}

UnicodeString defaultLSTM(UScript_ڪوڊ script, UError_ڪوڊ& حالت) {
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &حالت);
    b = ures_get_By_ڪنجي_With_Fallback(b, "lstm", b, &حالت);
    UnicodeString نتيجو = ures_get_Unicode_String_By_ڪنجي(b, uscript_getShortName(script), &حالت);
    ures_close(b);
    أرجع نتيجو;
}

U_CAPI ثابت LSTMData* U_EXPORT2 CreateLSTMDataForScript(UScript_ڪوڊ script, UError_ڪوڊ& حالت)
{
    إذا (script != USCRIPT_KHMER && script != USCRIPT_LAO && script != USCRIPT_MYANMAR && script != USCRIPT_THAI) {
        أرجع خالي_اشارو;
    }
    UnicodeString name = defaultLSTM(script, حالت);
    إذا (U_FAILURE(حالت)) أرجع خالي_اشارو;
    CharString namebuf;
    namebuf.appendInvariantChars(name, حالت).truncate(namebuf.آخري_اشاريو_جو('.'));

    Local_UResource_Bundle_اشارو rb(
        ures_open_سڌو(U_ICUDATA_BRKITR, namebuf.معلومات(), &حالت));
    إذا (U_FAILURE(حالت)) أرجع خالي_اشارو;

    أرجع CreateLSTMData(rb.orphan(), حالت);
}

U_CAPI ثابت LSTMData* U_EXPORT2 CreateLSTMData(UResourceBundle* rb, UError_ڪوڊ& حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع خالي_اشارو;
    }
    ثابت LSTMData* نتيجو = جديد LSTMData(rb, حالت);
    إذا (U_FAILURE(حالت)) {
        ميٽيو نتيجو;
        أرجع خالي_اشارو;
    }
    أرجع نتيجو;
}

U_CAPI ثابت LanguageBreakEngine* U_EXPORT2
CreateLSTMBreakEngine(UScript_ڪوڊ script, ثابت LSTMData* معلومات, UError_ڪوڊ& حالت)
{
    UnicodeString unicode_سيٽ_String;
    بدل(script) {
        حالة USCRIPT_THAI:
            unicode_سيٽ_String = UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]");
            اكسر;
        حالة USCRIPT_MYANMAR:
            unicode_سيٽ_String = UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]");
            اكسر;
        افتراضي:
            ميٽيو معلومات;
            أرجع خالي_اشارو;
    }
    Unicode_سيٽ unicode_سيٽ;
    unicode_سيٽ.apply_نمونو(unicode_سيٽ_String, حالت);
    ثابت LanguageBreakEngine* engine = جديد LSTMBreakEngine(معلومات, unicode_سيٽ, حالت);
    إذا (U_FAILURE(حالت) || engine == خالي_اشارو) {
        إذا (engine != خالي_اشارو) {
            ميٽيو engine;
        } وإلا {
            حالت = U_يادگيري_ورهاست_ERROR;
        }
        أرجع خالي_اشارو;
    }
    أرجع engine;
}

U_CAPI فراغ U_EXPORT2 DeleteLSTMData(ثابت LSTMData* معلومات)
{
    ميٽيو معلومات;
}

U_CAPI ثابت char16_t* U_EXPORT2 LSTMDataName(ثابت LSTMData* معلومات)
{
    أرجع معلومات->fName;
}

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو /* #if !UCONFIG_NO_BREAK_ITERATION */
