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
    مجازی ~ReadArray1D();
    مجازی int32_t d1() ثابت = 0;
    مجازی عائم get(int32_t i) ثابت = 0;

#جے_تعریف_شدہ LSTM_DEBUG
    فراغ print() ثابت {
        لکھو("\n[");
        لكل (int32_t i = 0; i < d1(); i++) {
           لکھو("%0.8e ", get(i));
           إذا (i % 4 == 3) لکھو("\n");
        }
        لکھو("]\n");
    }
#جے_انت
};

ReadArray1D::~ReadArray1D()
{
}

/**
 * Interface for reading 2D array.
 */
صنف ReadArray2D {
عام:
    مجازی ~ReadArray2D();
    مجازی int32_t d1() ثابت = 0;
    مجازی int32_t d2() ثابت = 0;
    مجازی عائم get(int32_t i, int32_t j) ثابت = 0;
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
    ConstArray1D() : انکڑے_2(خالی_سوچک), d1_(0) {}

    ConstArray1D(ثابت عائم* انکڑے, int32_t d1) : انکڑے_2(انکڑے), d1_(d1) {}

    مجازی ~ConstArray1D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    فراغ init(ثابت int32_t* انکڑے, int32_t d1) {
        U_ASSERT(IEEE_754 == 1);
        انکڑے_2 = reinterpret_cast<ثابت عائم*>(انکڑے);
        d1_ = d1;
    }

    // ReadArray1D methods.
    مجازی int32_t d1() ثابت override { أرجع d1_; }
    مجازی عائم get(int32_t i) ثابت override {
        U_ASSERT(i < d1_);
        أرجع انکڑے_2[i];
    }

خاص:
    ثابت عائم* انکڑے_2;
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
    ConstArray2D() : انکڑے_2(خالی_سوچک), d1_(0), d2_(0) {}

    ConstArray2D(ثابت عائم* انکڑے, int32_t d1, int32_t d2)
        : انکڑے_2(انکڑے), d1_(d1), d2_(d2) {}

    مجازی ~ConstArray2D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    فراغ init(ثابت int32_t* انکڑے, int32_t d1, int32_t d2) {
        U_ASSERT(IEEE_754 == 1);
        انکڑے_2 = reinterpret_cast<ثابت عائم*>(انکڑے);
        d1_ = d1;
        d2_ = d2;
    }

    // ReadArray2D methods.
    مضمن int32_t d1() ثابت override { أرجع d1_; }
    مضمن int32_t d2() ثابت override { أرجع d2_; }
    عائم get(int32_t i, int32_t j) ثابت override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        أرجع انکڑے_2[i * d2_ + j];
    }

    // Expose the ith row as a ConstArray1D
    مضمن ConstArray1D صف(int32_t i) ثابت {
        U_ASSERT(i < d1_);
        أرجع ConstArray1D(انکڑے_2 + i * d2_, d2_);
    }

خاص:
    ثابت عائم* انکڑے_2;
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
    Array1D() : حافظہ_2(خالی_سوچک), انکڑے_2(خالی_سوچک), d1_(0) {}
    Array1D(int32_t d1, UError_کوڈ &حالت)
        : حافظہ_2(uprv_malloc(d1 * حجم(عائم))),
          انکڑے_2(static_cast<عائم*>(حافظہ_2)), d1_(d1) {
        إذا (U_SUCCESS(حالت)) {
            إذا (حافظہ_2 == خالی_سوچک) {
                حالت = U_حافظہ_ونڈ_ERROR;
                أرجع;
            }
            clear();
        }
    }

    مجازی ~Array1D();

    // A special constructor which does not own the memory but writeable
    // as a slice of an array.
    Array1D(عائم* انکڑے, int32_t d1)
        : حافظہ_2(خالی_سوچک), انکڑے_2(انکڑے), d1_(d1) {}

    // ReadArray1D methods.
    مجازی int32_t d1() ثابت override { أرجع d1_; }
    مجازی عائم get(int32_t i) ثابت override {
        U_ASSERT(i < d1_);
        أرجع انکڑے_2[i];
    }

    // Return the index which point to the max data in the array.
    مضمن int32_t max_اشاریہ() ثابت {
        int32_t اشاریہ = 0;
        عائم max = انکڑے_2[0];
        لكل (int32_t i = 1; i < d1_; i++) {
            إذا (انکڑے_2[i] > max) {
                max = انکڑے_2[i];
                اشاریہ = i;
            }
        }
        أرجع اشاریہ;
    }

    // Slice part of the array to a new one.
    مضمن Array1D ٹکڑا(int32_t from, int32_t size) ثابت {
        U_ASSERT(from >= 0);
        U_ASSERT(from < d1_);
        U_ASSERT(from + size <= d1_);
        أرجع Array1D(انکڑے_2 + from, size);
    }

    // Add dot product of a 1D array and a 2D array into this one.
    مضمن Array1D& جوڑو_Dot_ضرب_حاصل(ثابت ReadArray1D& a, ثابت ReadArray2D& b) {
        U_ASSERT(a.d1() == b.d1());
        U_ASSERT(b.d2() == d1());
        لكل (int32_t i = 0; i < d1(); i++) {
            لكل (int32_t j = 0; j < a.d1(); j++) {
                انکڑے_2[i] += a.get(j) * b.get(j, i);
            }
        }
        أرجع *هذا;
    }

    // Hadamard Product the values of another array of the same size into this one.
    مضمن Array1D& hadamard_ضرب_حاصل(ثابت ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        لكل (int32_t i = 0; i < d1(); i++) {
            انکڑے_2[i] *= a.get(i);
        }
        أرجع *هذا;
    }

    // Add the Hadamard Product of two arrays of the same size into this one.
    مضمن Array1D& جوڑو_Hadamard_ضرب_حاصل(ثابت ReadArray1D& a, ثابت ReadArray1D& b) {
        U_ASSERT(a.d1() == d1());
        U_ASSERT(b.d1() == d1());
        لكل (int32_t i = 0; i < d1(); i++) {
            انکڑے_2[i] += a.get(i) * b.get(i);
        }
        أرجع *هذا;
    }

    // Add the values of another array of the same size into this one.
    مضمن Array1D& جوڑو(ثابت ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        لكل (int32_t i = 0; i < d1(); i++) {
            انکڑے_2[i] += a.get(i);
        }
        أرجع *هذا;
    }

    // Assign the values of another array of the same size into this one.
    مضمن Array1D& assign(ثابت ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        لكل (int32_t i = 0; i < d1(); i++) {
            انکڑے_2[i] = a.get(i);
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
            انکڑے_2[i] = std::tanh(a.get(i));
        }
        أرجع *هذا;
    }

    // Apply sigmoid to all the elements in the array.
    مضمن Array1D& sigmoid() {
        لكل (int32_t i = 0; i < d1_; i++) {
            انکڑے_2[i] = 1.0f/(1.0f + expf(-انکڑے_2[i]));
        }
        أرجع *هذا;
    }

    مضمن Array1D& clear() {
        uprv_memset(انکڑے_2, 0, d1_ * حجم(عائم));
        أرجع *هذا;
    }

خاص:
    فراغ* حافظہ_2;
    عائم* انکڑے_2;
    int32_t d1_;
};

Array1D::~Array1D()
{
    uprv_free(حافظہ_2);
}

صنف Array2D : عام ReadArray2D {
عام:
    Array2D() : حافظہ_2(خالی_سوچک), انکڑے_2(خالی_سوچک), d1_(0), d2_(0) {}
    Array2D(int32_t d1, int32_t d2, UError_کوڈ &حالت)
        : حافظہ_2(uprv_malloc(d1 * d2 * حجم(عائم))),
          انکڑے_2(static_cast<عائم*>(حافظہ_2)), d1_(d1), d2_(d2) {
        إذا (U_SUCCESS(حالت)) {
            إذا (حافظہ_2 == خالی_سوچک) {
                حالت = U_حافظہ_ونڈ_ERROR;
                أرجع;
            }
            clear();
        }
    }
    مجازی ~Array2D();

    // ReadArray2D methods.
    مجازی int32_t d1() ثابت override { أرجع d1_; }
    مجازی int32_t d2() ثابت override { أرجع d2_; }
    مجازی عائم get(int32_t i, int32_t j) ثابت override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        أرجع انکڑے_2[i * d2_ + j];
    }

    مضمن Array1D صف(int32_t i) ثابت {
        U_ASSERT(i < d1_);
        أرجع Array1D(انکڑے_2 + i * d2_, d2_);
    }

    مضمن Array2D& clear() {
        uprv_memset(انکڑے_2, 0, d1_ * d2_ * حجم(عائم));
        أرجع *هذا;
    }

خاص:
    فراغ* حافظہ_2;
    عائم* انکڑے_2;
    int32_t d1_;
    int32_t d2_;
};

Array2D::~Array2D()
{
    uprv_free(حافظہ_2);
}

تعريفنوع تعداد {
    BEGIN,
    INSIDE,
    سرا_2,
    SINGLE
} LSTMClass;

تعريفنوع تعداد {
    UNKNOWN,
    کوڈ_POINTS,
    GRAPHEME_CLUSTER,
} EmbeddingType;

بنية LSTMData : عام UMemory {
    LSTMData(UResourceBundle* rb, UError_کوڈ &حالت);
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

LSTMData::LSTMData(UResourceBundle* rb, UError_کوڈ &حالت)
    : fDict(خالی_سوچک), fType(UNKNOWN), fName(خالی_سوچک),
      fBundle(rb)
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    إذا (IEEE_754 != 1) {
        حالت = U_UNSUPPORTED_ERROR;
        أرجع;
    }
    Local_UResource_Bundle_اشارہ_گر embeddings_res(
        ures_get_By_کلید(rb, "embeddings", خالی_سوچک, &حالت));
    int32_t embedding_ناپ = ures_getInt(embeddings_res.getAlias(), &حالت);
    Local_UResource_Bundle_اشارہ_گر hunits_res(
        ures_get_By_کلید(rb, "hunits", خالی_سوچک, &حالت));
    إذا (U_FAILURE(حالت)) أرجع;
    int32_t hunits = ures_getInt(hunits_res.getAlias(), &حالت);
    ثابت char16_t* type = ures_get_String_By_کلید(rb, "type", خالی_سوچک, &حالت);
    إذا (U_FAILURE(حالت)) أرجع;
    إذا (u_strCompare(type, -1, u"codepoints", -1, خطأ) == 0) {
        fType = کوڈ_POINTS;
    } وإلا إذا (u_strCompare(type, -1, u"graphclust", -1, خطأ) == 0) {
        fType = GRAPHEME_CLUSTER;
    }
    fName = ures_get_String_By_کلید(rb, "model", خالی_سوچک, &حالت);
    Local_UResource_Bundle_اشارہ_گر انکڑے_Res(ures_get_By_کلید(rb, "data", خالی_سوچک, &حالت));
    إذا (U_FAILURE(حالت)) أرجع;
    int32_t انکڑے_len = 0;
    ثابت int32_t* انکڑے = ures_get_Int_سمتیہ(انکڑے_Res.getAlias(), &انکڑے_len, &حالت);
    fDict = uhash_open(uhash_hashUChars, uhash_compareUChars, خالی_سوچک, &حالت);

    ڈھیر_UResource_Bundle ڈھیر_عارضی_Bundle;
    Resource_انکڑے_قدر value;
    ures_get_قدر_With_Fallback(rb, "dict", ڈھیر_عارضی_Bundle.getAlias(), value, حالت);
    ResourceArray stringArray = value.getArray(حالت);
    int32_t عدد_اشاریہ = stringArray.get_ناپ();
    إذا (U_FAILURE(حالت)) { أرجع; }

    // put dict into hash
    int32_t string_لمائی;
    لكل (int32_t idx = 0; idx < عدد_اشاریہ; idx++) {
        stringArray.get_قدر(idx, value);
        ثابت char16_t* str = value.getString(string_لمائی, حالت);
        uhash_putiAllowZero(fDict, (فراغ*)str, idx, &حالت);
        إذا (U_FAILURE(حالت)) أرجع;
#جے_تعریف_شدہ LSTM_VECTORIZER_DEBUG
        لکھو("Assign [");
        طالما (*str != 0x0000) {
            لکھو("U+%04x ", *str);
            str++;
        }
        لکھو("] map to %d\n", idx-1);
#جے_انت
    }
    int32_t mat1_ناپ = (عدد_اشاریہ + 1) * embedding_ناپ;
    int32_t mat2_ناپ = embedding_ناپ * 4 * hunits;
    int32_t mat3_ناپ = hunits * 4 * hunits;
    int32_t mat4_ناپ = 4 * hunits;
    int32_t mat5_ناپ = mat2_ناپ;
    int32_t mat6_ناپ = mat3_ناپ;
    int32_t mat7_ناپ = mat4_ناپ;
    int32_t mat8_ناپ = 2 * hunits * 4;
#إذا U_DEBUG
    int32_t mat9_ناپ = 4;
    U_ASSERT(انکڑے_len == mat1_ناپ + mat2_ناپ + mat3_ناپ + mat4_ناپ + mat5_ناپ +
        mat6_ناپ + mat7_ناپ + mat8_ناپ + mat9_ناپ);
#جے_انت

    fEmbedding.init(انکڑے, (عدد_اشاریہ + 1), embedding_ناپ);
    انکڑے += mat1_ناپ;
    fForwardW.init(انکڑے, embedding_ناپ, 4 * hunits);
    انکڑے += mat2_ناپ;
    fForwardU.init(انکڑے, hunits, 4 * hunits);
    انکڑے += mat3_ناپ;
    fForwardB.init(انکڑے, 4 * hunits);
    انکڑے += mat4_ناپ;
    fBackwardW.init(انکڑے, embedding_ناپ, 4 * hunits);
    انکڑے += mat5_ناپ;
    fBackwardU.init(انکڑے, hunits, 4 * hunits);
    انکڑے += mat6_ناپ;
    fBackwardB.init(انکڑے, 4 * hunits);
    انکڑے += mat7_ناپ;
    fOutputW.init(انکڑے, 2 * hunits, 4);
    انکڑے += mat8_ناپ;
    fOutputB.init(انکڑے, 4);
}

LSTMData::~LSTMData() {
    uhash_close(fDict);
    ures_close(fBundle);
}

صنف Vectorizer : عام UMemory {
عام:
    Vectorizer(UHashtable* dict) : fDict(dict) {}
    مجازی ~Vectorizer();
    مجازی فراغ vectorize(UText *متن, int32_t شروع_Pos, int32_t سرا_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_کوڈ &حالت) ثابت = 0;
محمي:
    int32_t string_منزل_اشاریہ(ثابت char16_t* str) ثابت {
        UBool لبھیا = خطأ;
        int32_t ret = uhash_geti_And_لبھیا(fDict, (ثابت فراغ*)str, &لبھیا);
        إذا (!لبھیا) {
            ret = fDict->count;
        }
#جے_تعریف_شدہ LSTM_VECTORIZER_DEBUG
        لکھو("[");
        طالما (*str != 0x0000) {
            لکھو("U+%04x ", *str);
            str++;
        }
        لکھو("] map to %d\n", ret);
#جے_انت
        أرجع ret;
    }

خاص:
    UHashtable* fDict;
};

Vectorizer::~Vectorizer()
{
}

صنف کوڈ_Points_Vectorizer : عام Vectorizer {
عام:
    کوڈ_Points_Vectorizer(UHashtable* dict) : Vectorizer(dict) {}
    مجازی ~کوڈ_Points_Vectorizer();
    مجازی فراغ vectorize(UText *متن, int32_t شروع_Pos, int32_t سرا_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_کوڈ &حالت) ثابت override;
};

کوڈ_Points_Vectorizer::~کوڈ_Points_Vectorizer()
{
}

فراغ کوڈ_Points_Vectorizer::vectorize(
    UText *متن, int32_t شروع_Pos, int32_t سرا_Pos,
    UVector32 &offsets, UVector32 &indices, UError_کوڈ &حالت) ثابت
{
    إذا (offsets.ensure_سمرتھا(سرا_Pos - شروع_Pos, حالت) &&
            indices.ensure_سمرتھا(سرا_Pos - شروع_Pos, حالت)) {
        إذا (U_FAILURE(حالت)) أرجع;
        utext_سیٹ_Native_اشاریہ(متن, شروع_Pos);
        int32_t current;
        char16_t str[2] = {0, 0};
        طالما (U_SUCCESS(حالت) &&
               (current = static_cast<int32_t>(utext_get_Native_اشاریہ(متن))) < سرا_Pos) {
            // Since the LSTMBreakEngine is currently only accept chars in BMP,
            // we can ignore the possibility of hitting supplementary code
            // point.
            str[0] = static_cast<char16_t>(utext_next32(متن));
            U_ASSERT(!U_IS_SURROGATE(str[0]));
            offsets.جوڑو_Element(current, حالت);
            indices.جوڑو_Element(string_منزل_اشاریہ(str), حالت);
        }
    }
}

صنف GraphemeClusterVectorizer : عام Vectorizer {
عام:
    GraphemeClusterVectorizer(UHashtable* dict)
        : Vectorizer(dict)
    {
    }
    مجازی ~GraphemeClusterVectorizer();
    مجازی فراغ vectorize(UText *متن, int32_t شروع_Pos, int32_t سرا_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_کوڈ &حالت) ثابت override;
};

GraphemeClusterVectorizer::~GraphemeClusterVectorizer()
{
}

constexpr int32_t MAX_GRAPHEME_CLSTER_لمائی = 10;

فراغ GraphemeClusterVectorizer::vectorize(
    UText *متن, int32_t شروع_Pos, int32_t سرا_Pos,
    UVector32 &offsets, UVector32 &indices, UError_کوڈ &حالت) ثابت
{
    إذا (U_FAILURE(حالت)) أرجع;
    إذا (!offsets.ensure_سمرتھا(سرا_Pos - شروع_Pos, حالت) ||
            !indices.ensure_سمرتھا(سرا_Pos - شروع_Pos, حالت)) {
        أرجع;
    }
    إذا (U_FAILURE(حالت)) أرجع;
    Local_اشارہ_گر<BreakIterator> graphemeIter(BreakIterator::createCharacterInstance(Locale(), حالت));
    إذا (U_FAILURE(حالت)) أرجع;
    graphemeIter->سیٹ_متن(متن, حالت);
    إذا (U_FAILURE(حالت)) أرجع;

    إذا (شروع_Pos != 0) {
        graphemeIter->preceding(شروع_Pos);
    }
    int32_t آخری = شروع_Pos;
    int32_t current = شروع_Pos;
    char16_t str[MAX_GRAPHEME_CLSTER_لمائی];
    طالما ((current = graphemeIter->next()) != BreakIterator::پورا_2) {
        إذا (current >= سرا_Pos) {
            اكسر;
        }
        إذا (current > شروع_Pos) {
            utext_کڈھو(متن, آخری, current, str, MAX_GRAPHEME_CLSTER_لمائی, &حالت);
            إذا (U_FAILURE(حالت)) أرجع;
            offsets.جوڑو_Element(آخری, حالت);
            indices.جوڑو_Element(string_منزل_اشاریہ(str), حالت);
            إذا (U_FAILURE(حالت)) أرجع;
        }
        آخری = current;
    }
    إذا (U_FAILURE(حالت) || آخری >= سرا_Pos) {
        أرجع;
    }
    utext_کڈھو(متن, آخری, سرا_Pos, str, MAX_GRAPHEME_CLSTER_لمائی, &حالت);
    إذا (U_SUCCESS(حالت)) {
        offsets.جوڑو_Element(آخری, حالت);
        indices.جوڑو_Element(string_منزل_اشاریہ(str), حالت);
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
        .جوڑو_Dot_ضرب_حاصل(x, W)
        .جوڑو_Dot_ضرب_حاصل(h, U);

    ifco.ٹکڑا(0*hunits, hunits).sigmoid();  // i: sigmod
    ifco.ٹکڑا(1*hunits, hunits).sigmoid(); // f: sigmoid
    ifco.ٹکڑا(2*hunits, hunits).tanh(); // c_: tanh
    ifco.ٹکڑا(3*hunits, hunits).sigmoid(); // o: sigmod

    c.hadamard_ضرب_حاصل(ifco.ٹکڑا(hunits, hunits))
        .جوڑو_Hadamard_ضرب_حاصل(ifco.ٹکڑا(0, hunits), ifco.ٹکڑا(2*hunits, hunits));

    h.tanh(c)
        .hadamard_ضرب_حاصل(ifco.ٹکڑا(3*hunits, hunits));
}

// Minimum word size
ساكن ثابت int32_t MIN_لفظ = 2;

// Minimum number of characters for two words
ساكن ثابت int32_t MIN_لفظ_وسعت = MIN_لفظ * 2;

int32_t
LSTMBreakEngine::divide_اتے_Dictionary_Range( UText *متن,
                                                int32_t شروع_Pos,
                                                int32_t سرا_Pos,
                                                UVector32 &لبھیا_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_کوڈ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) أرجع 0;
    int32_t begin_لبھیا_Break_ناپ = لبھیا_Breaks.size();
    utext_سیٹ_Native_اشاریہ(متن, شروع_Pos);
    utext_moveIndex32(متن, MIN_لفظ_وسعت);
    إذا (utext_get_Native_اشاریہ(متن) >= سرا_Pos) {
        أرجع 0;       // Not enough characters for two words
    }
    utext_سیٹ_Native_اشاریہ(متن, شروع_Pos);

    UVector32 offsets(حالت);
    UVector32 indices(حالت);
    إذا (U_FAILURE(حالت)) أرجع 0;
    fVectorizer->vectorize(متن, شروع_Pos, سرا_Pos, offsets, indices, حالت);
    إذا (U_FAILURE(حالت)) أرجع 0;
    int32_t* offsetsBuf = offsets.get_بفر();
    int32_t* indicesBuf = indices.get_بفر();

    int32_t داخلہ_seq_len = indices.size();
    int32_t hunits = f_انکڑے->fForwardU.d1();

    // ----- Begin of all the Array memory allocation needed for this function
    // Allocate temp array used inside compute()
    Array1D ifco(4 * hunits, حالت);

    Array1D c(hunits, حالت);
    Array1D logp(4, حالت);

    // TODO: limit size of hBackward. If input_seq_len is too big, we could
    // run out of memory.
    // Backward LSTM
    Array2D hBackward(داخلہ_seq_len, hunits, حالت);

    // Allocate fbRow and slice the internal array in two.
    Array1D fb_صف(2 * hunits, حالت);

    // ----- End of all the Array memory allocation needed for this function
    إذا (U_FAILURE(حالت)) أرجع 0;

    // To save the needed memory usage, the following is different from the
    // Python or ICU4X implementation. We first perform the Backward LSTM
    // and then merge the iteration of the forward LSTM and the output layer
    // together because we only neetdto remember the h[t-1] for Forward LSTM.
    لكل (int32_t i = داخلہ_seq_len - 1; i >= 0; i--) {
        Array1D h_صف = hBackward.صف(i);
        إذا (i != داخلہ_seq_len - 1) {
            h_صف.assign(hBackward.صف(i+1));
        }
#جے_تعریف_شدہ LSTM_DEBUG
        لکھو("hRow %d\n", i);
        h_صف.print();
        لکھو("indicesBuf[%d] = %d\n", i, indicesBuf[i]);
        لکھو("fData->fEmbedding.row(indicesBuf[%d]):\n", i);
        f_انکڑے->fEmbedding.صف(indicesBuf[i]).print();
#جے_انت  // LSTM_DEBUG
        compute(hunits,
                f_انکڑے->fBackwardW, f_انکڑے->fBackwardU, f_انکڑے->fBackwardB,
                f_انکڑے->fEmbedding.صف(indicesBuf[i]),
                h_صف, c, ifco);
    }


    Array1D forward_صف = fb_صف.ٹکڑا(0, hunits);  // point to first half of data in fbRow.
    Array1D backward_صف = fb_صف.ٹکڑا(hunits, hunits);  // point to second half of data n fbRow.

    // The following iteration merge the forward LSTM and the output layer
    // together.
    c.clear();  // reuse c since it is the same size.
    لكل (int32_t i = 0; i < داخلہ_seq_len; i++) {
#جے_تعریف_شدہ LSTM_DEBUG
        لکھو("forwardRow %d\n", i);
        forward_صف.print();
#جے_انت  // LSTM_DEBUG
        // Forward LSTM
        // Calculate the result into forwardRow, which point to the data in the first half
        // of fbRow.
        compute(hunits,
                f_انکڑے->fForwardW, f_انکڑے->fForwardU, f_انکڑے->fForwardB,
                f_انکڑے->fEmbedding.صف(indicesBuf[i]),
                forward_صف, c, ifco);

        // assign the data from hBackward.row(i) to second half of fbRowa.
        backward_صف.assign(hBackward.صف(i));

        logp.assign(f_انکڑے->fOutputB).جوڑو_Dot_ضرب_حاصل(fb_صف, f_انکڑے->fOutputW);
#جے_تعریف_شدہ LSTM_DEBUG
        لکھو("backwardRow %d\n", i);
        backward_صف.print();
        لکھو("logp %d\n", i);
        logp.print();
#جے_انت  // LSTM_DEBUG

        // current = argmax(logp)
        LSTMClass current = static_cast<LSTMClass>(logp.max_اشاریہ());
        // BIES logic.
        إذا (current == BEGIN || current == SINGLE) {
            إذا (i != 0) {
                لبھیا_Breaks.جوڑو_Element(offsetsBuf[i], حالت);
                إذا (U_FAILURE(حالت)) أرجع 0;
            }
        }
    }
    أرجع لبھیا_Breaks.size() - begin_لبھیا_Break_ناپ;
}

Vectorizer* createVectorizer(ثابت LSTMData* انکڑے, UError_کوڈ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }
    بدل (انکڑے->fType) {
        حالة کوڈ_POINTS:
            أرجع جديد کوڈ_Points_Vectorizer(انکڑے->fDict);
            اكسر;
        حالة GRAPHEME_CLUSTER:
            أرجع جديد GraphemeClusterVectorizer(انکڑے->fDict);
            اكسر;
        افتراضي:
            اكسر;
    }
    UPRV_UNREACHABLE_EXIT;
}

LSTMBreakEngine::LSTMBreakEngine(ثابت LSTMData* انکڑے, ثابت Unicode_سیٹ& سیٹ, UError_کوڈ &حالت)
    : DictionaryBreakEngine(), f_انکڑے(انکڑے), fVectorizer(createVectorizer(f_انکڑے, حالت))
{
    إذا (U_FAILURE(حالت)) {
      f_انکڑے = خالی_سوچک;  // If failure, we should not delete fData in destructor because the caller will do so.
      أرجع;
    }
    سیٹ_Characters(سیٹ);
}

LSTMBreakEngine::~LSTMBreakEngine() {
    مٹاؤ f_انکڑے;
    مٹاؤ fVectorizer;
}

ثابت char16_t* LSTMBreakEngine::name() ثابت {
    أرجع f_انکڑے->fName;
}

UnicodeString defaultLSTM(UScript_کوڈ script, UError_کوڈ& حالت) {
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &حالت);
    b = ures_get_By_کلید_With_Fallback(b, "lstm", b, &حالت);
    UnicodeString نتیجہ = ures_get_Unicode_String_By_کلید(b, uscript_getShortName(script), &حالت);
    ures_close(b);
    أرجع نتیجہ;
}

U_CAPI ثابت LSTMData* U_EXPORT2 CreateLSTMDataForScript(UScript_کوڈ script, UError_کوڈ& حالت)
{
    إذا (script != USCRIPT_KHMER && script != USCRIPT_LAO && script != USCRIPT_MYANMAR && script != USCRIPT_THAI) {
        أرجع خالی_سوچک;
    }
    UnicodeString name = defaultLSTM(script, حالت);
    إذا (U_FAILURE(حالت)) أرجع خالی_سوچک;
    CharString namebuf;
    namebuf.appendInvariantChars(name, حالت).truncate(namebuf.آخری_اشاریہ_دا('.'));

    Local_UResource_Bundle_اشارہ_گر rb(
        ures_open_سدھا(U_ICUDATA_BRKITR, namebuf.انکڑے(), &حالت));
    إذا (U_FAILURE(حالت)) أرجع خالی_سوچک;

    أرجع CreateLSTMData(rb.orphan(), حالت);
}

U_CAPI ثابت LSTMData* U_EXPORT2 CreateLSTMData(UResourceBundle* rb, UError_کوڈ& حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }
    ثابت LSTMData* نتیجہ = جديد LSTMData(rb, حالت);
    إذا (U_FAILURE(حالت)) {
        مٹاؤ نتیجہ;
        أرجع خالی_سوچک;
    }
    أرجع نتیجہ;
}

U_CAPI ثابت LanguageBreakEngine* U_EXPORT2
CreateLSTMBreakEngine(UScript_کوڈ script, ثابت LSTMData* انکڑے, UError_کوڈ& حالت)
{
    UnicodeString unicode_سیٹ_String;
    بدل(script) {
        حالة USCRIPT_THAI:
            unicode_سیٹ_String = UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]");
            اكسر;
        حالة USCRIPT_MYANMAR:
            unicode_سیٹ_String = UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]");
            اكسر;
        افتراضي:
            مٹاؤ انکڑے;
            أرجع خالی_سوچک;
    }
    Unicode_سیٹ unicode_سیٹ;
    unicode_سیٹ.apply_نمونہ(unicode_سیٹ_String, حالت);
    ثابت LanguageBreakEngine* engine = جديد LSTMBreakEngine(انکڑے, unicode_سیٹ, حالت);
    إذا (U_FAILURE(حالت) || engine == خالی_سوچک) {
        إذا (engine != خالی_سوچک) {
            مٹاؤ engine;
        } وإلا {
            حالت = U_حافظہ_ونڈ_ERROR;
        }
        أرجع خالی_سوچک;
    }
    أرجع engine;
}

U_CAPI فراغ U_EXPORT2 DeleteLSTMData(ثابت LSTMData* انکڑے)
{
    مٹاؤ انکڑے;
}

U_CAPI ثابت char16_t* U_EXPORT2 LSTMDataName(ثابت LSTMData* انکڑے)
{
    أرجع انکڑے->fName;
}

U_NAMESPACE_سرا

#جے_انت /* #if !UCONFIG_NO_BREAK_ITERATION */
