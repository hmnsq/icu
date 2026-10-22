// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ضمن <optional>
#ضمن <string_view>
#ضمن <utility>

#ضمن "bytesinkutil.h"  // StringByteSink<CharString>
#ضمن "charstr.h"
#ضمن "cstring.h"
#ضمن "fixedstring.h"
#ضمن "ulocimp.h"
#ضمن "unicode/localebuilder.h"
#ضمن "unicode/locid.h"

نطاق {

مضمن منطقي UPRV_ISDIGIT(حرف c) { أرجع c >= '0' && c <= '9'; }
مضمن منطقي UPRV_ISALPHANUM(حرف c) { أرجع uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr ثابت حرف* k_Attribute_کلید = "attribute";

منطقي _isExtensionSubtags(حرف کلید, ثابت حرف* s, int32_t len) {
    بدل (uprv_tolower(کلید)) {
        حالة 'u':
            أرجع ultag_isUnicodeExtensionSubtags(s, len);
        حالة 't':
            أرجع ultag_isTransformedExtensionSubtags(s, len);
        حالة 'x':
            أرجع ultag_is_Privateuse_مقدار_Subtags(s, len);
        افتراضي:
            أرجع ultag_isExtensionSubtags(s, len);
    }
}

}  // namespace

U_NAMESPACE_BEGIN

LocaleBuilder::LocaleBuilder() : UObject(), وضعیت_2(U_ZERO_ERROR), language_(),
    script_(), region_(), variant_(اشاره_تهی), extensions_(اشاره_تهی)
{
    language_[0] = 0;
    script_[0] = 0;
    region_[0] = 0;
}

LocaleBuilder::~LocaleBuilder()
{
    حذف variant_;
    حذف extensions_;
}

LocaleBuilder& LocaleBuilder::مجموعه_Locale(ثابت Locale& locale) U_LIFETIME_BOUND
{
    clear();
    مجموعه_Language(locale.getLanguage());
    مجموعه_Script(locale.getScript());
    مجموعه_Region(locale.getCountry());
    مجموعه_Variant(locale.getVariant());
    extensions_ = locale.clone();
    إذا (extensions_ == اشاره_تهی) {
        وضعیت_2 = U_حافظه_تخصیص_ERROR;
    }
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::مجموعه_Language_برچسب(StringPiece برچسب) U_LIFETIME_BOUND
{
    Locale l = Locale::for_Language_برچسب(برچسب, وضعیت_2);
    إذا (U_FAILURE(وضعیت_2)) { أرجع *هذا; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    مجموعه_Locale(l);
    أرجع *هذا;
}

نطاق {

فراغ مجموعه_فیلد(StringPiece input, حرف* dest, UError_کد& error_کد,
              منطقي (*test)(ثابت حرف*, int32_t)) {
    إذا (U_FAILURE(error_کد)) { أرجع; }
    إذا (input.empty()) {
        dest[0] = '\0';
    } وإلا إذا (test(input.داده(), input.length())) {
        uprv_memcpy(dest, input.داده(), input.length());
        dest[input.length()] = '\0';
    } وإلا {
        error_کد = U_ILLEGAL_آرگومان_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::مجموعه_Language(StringPiece language) U_LIFETIME_BOUND
{
    مجموعه_فیلد(language, language_, وضعیت_2, &ultag_isLanguageSubtag);
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::مجموعه_Script(StringPiece script) U_LIFETIME_BOUND
{
    مجموعه_فیلد(script, script_, وضعیت_2, &ultag_isScriptSubtag);
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::مجموعه_Region(StringPiece region) U_LIFETIME_BOUND
{
    مجموعه_فیلد(region, region_, وضعیت_2, &ultag_isRegionSubtag);
    أرجع *هذا;
}

نطاق {

فراغ transform(حرف* داده, int32_t len) {
    لكل (int32_t i = 0; i < len; i++, داده++) {
        إذا (*داده == '_') {
            *داده = '-';
        } وإلا {
            *داده = uprv_tolower(*داده);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::مجموعه_Variant(StringPiece variant) U_LIFETIME_BOUND
{
    إذا (U_FAILURE(وضعیت_2)) { أرجع *هذا; }
    إذا (variant.empty()) {
        حذف variant_;
        variant_ = اشاره_تهی;
        أرجع *هذا;
    }
    FixedString* new_variant = جديد FixedString(variant);
    إذا (new_variant == اشاره_تهی || new_variant->is_خالی()) {
        وضعیت_2 = U_حافظه_تخصیص_ERROR;
        أرجع *هذا;
    }
    transform(new_variant->getAlias(), variant.length());
    إذا (!ultag_isVariantSubtags(new_variant->داده(), variant.length())) {
        حذف new_variant;
        وضعیت_2 = U_ILLEGAL_آرگومان_ERROR;
        أرجع *هذا;
    }
    حذف variant_;
    variant_ = new_variant;
    أرجع *هذا;
}

نطاق {

منطقي
is_Keyword_مقدار(ثابت حرف* کلید, ثابت حرف* value, int32_t مقدار_len)
{
    إذا (کلید[1] == '\0') {
        // one char key
        أرجع (UPRV_ISALPHANUM(uprv_tolower(کلید[0])) &&
                _isExtensionSubtags(کلید[0], value, مقدار_len));
    } وإلا إذا (uprv_strcmp(کلید, k_Attribute_کلید) == 0) {
        // unicode attributes
        أرجع ultag_is_Unicode_Locale_صفات(value, مقدار_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_کلید = ulocimp_مقصد_Bcp_کلید_With_Fallback(کلید);
    std::optional<std::string_view> unicode_locale_type = ulocimp_مقصد_Bcp_Type_With_Fallback(کلید, value);

    أرجع unicode_locale_کلید.has_value() &&
           unicode_locale_type.has_value() &&
           ultag_is_Unicode_Locale_کلید(unicode_locale_کلید->داده(),
                                    static_cast<int32_t>(unicode_locale_کلید->size())) &&
           ultag_isUnicodeLocaleType(unicode_locale_type->داده(),
                                     static_cast<int32_t>(unicode_locale_type->size()));
}

فراغ
_copyExtensions(ثابت Locale& from, icu::StringEnumeration *keywords,
                Locale& مقصد, منطقي validate, UError_کد& error_کد)
{
    إذا (U_FAILURE(error_کد)) { أرجع; }
    Local_اشاره_گر<icu::StringEnumeration> ownedKeywords;
    إذا (keywords == اشاره_تهی) {
        ownedKeywords.adoptInstead(from.createKeywords(error_کد));
        إذا (U_FAILURE(error_کد) || ownedKeywords.isNull()) { أرجع; }
        keywords = ownedKeywords.getAlias();
    }
    ثابت حرف* کلید;
    طالما ((کلید = keywords->next(اشاره_تهی, error_کد)) != اشاره_تهی) {
        خودکار value = from.get_Keyword_مقدار<CharString>(کلید, error_کد);
        إذا (U_FAILURE(error_کد)) { أرجع; }
        إذا (uprv_strcmp(کلید, k_Attribute_کلید) == 0) {
            transform(value.داده(), value.length());
        }
        إذا (validate &&
            !is_Keyword_مقدار(کلید, value.داده(), value.length())) {
            error_کد = U_ILLEGAL_آرگومان_ERROR;
            أرجع;
        }
        مقصد.مجموعه_Keyword_مقدار(کلید, value.داده(), error_کد);
        إذا (U_FAILURE(error_کد)) { أرجع; }
    }
}

فراغ
clear_UAttributes_And_کلید_Type(Locale& locale, UError_کد& error_کد)
{
    إذا (U_FAILURE(error_کد)) { أرجع; }
    // Clear Unicode attributes
    locale.مجموعه_Keyword_مقدار(k_Attribute_کلید, "", error_کد);

    // Clear all Unicode keyword values
    Local_اشاره_گر<icu::StringEnumeration> iter(locale.createUnicodeKeywords(error_کد));
    إذا (U_FAILURE(error_کد) || iter.isNull()) { أرجع; }
    ثابت حرف* کلید;
    طالما ((کلید = iter->next(اشاره_تهی, error_کد)) != اشاره_تهی) {
        locale.مجموعه_Unicode_Keyword_مقدار(کلید, اشاره_تهی, error_کد);
    }
}

فراغ
مجموعه_Unicode_Extensions(Locale& locale, ثابت CharString& value, UError_کد& error_کد)
{
    إذا (U_FAILURE(error_کد)) { أرجع; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", error_کد);
    locale_str.append(value, error_کد);
    _copyExtensions(
        Locale::for_Language_برچسب(locale_str.داده(), error_کد), اشاره_تهی,
        locale, خطأ, error_کد);
}

}  // namespace

LocaleBuilder& LocaleBuilder::مجموعه_Extension(حرف کلید, StringPiece value) U_LIFETIME_BOUND
{
    إذا (U_FAILURE(وضعیت_2)) { أرجع *هذا; }
    إذا (!UPRV_ISALPHANUM(کلید)) {
        وضعیت_2 = U_ILLEGAL_آرگومان_ERROR;
        أرجع *هذا;
    }
    CharString مقدار_str(value, وضعیت_2);
    إذا (U_FAILURE(وضعیت_2)) { أرجع *هذا; }
    transform(مقدار_str.داده(), مقدار_str.length());
    إذا (!مقدار_str.is_خالی() &&
            !_isExtensionSubtags(کلید, مقدار_str.داده(), مقدار_str.length())) {
        وضعیت_2 = U_ILLEGAL_آرگومان_ERROR;
        أرجع *هذا;
    }
    إذا (extensions_ == اشاره_تهی) {
        extensions_ = Locale::get_ریشه().clone();
        إذا (extensions_ == اشاره_تهی) {
            وضعیت_2 = U_حافظه_تخصیص_ERROR;
            أرجع *هذا;
        }
    }
    إذا (uprv_tolower(کلید) != 'u') {
        // for t, x and others extension.
        extensions_->مجموعه_Keyword_مقدار(StringPiece(&کلید, 1), مقدار_str.داده(),
                                     وضعیت_2);
        أرجع *هذا;
    }
    clear_UAttributes_And_کلید_Type(*extensions_, وضعیت_2);
    إذا (U_FAILURE(وضعیت_2)) { أرجع *هذا; }
    إذا (!value.empty()) {
        مجموعه_Unicode_Extensions(*extensions_, مقدار_str, وضعیت_2);
    }
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::مجموعه_Unicode_Locale_Keyword(StringPiece کلید,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    إذا (U_FAILURE(وضعیت_2)) { أرجع *هذا; }
    إذا (!ultag_is_Unicode_Locale_کلید(کلید.داده(), کلید.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.داده(), type.length()))) {
      وضعیت_2 = U_ILLEGAL_آرگومان_ERROR;
      أرجع *هذا;
    }
    إذا (extensions_ == اشاره_تهی) {
        extensions_ = Locale::get_ریشه().clone();
        إذا (extensions_ == اشاره_تهی) {
            وضعیت_2 = U_حافظه_تخصیص_ERROR;
            أرجع *هذا;
        }
    }
    extensions_->مجموعه_Unicode_Keyword_مقدار(کلید, type, وضعیت_2);
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::جمع_کن_Unicode_Locale_Attribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString مقدار_str(value, وضعیت_2);
    إذا (U_FAILURE(وضعیت_2)) { أرجع *هذا; }
    transform(مقدار_str.داده(), مقدار_str.length());
    إذا (!ultag_isUnicodeLocaleAttribute(مقدار_str.داده(), مقدار_str.length())) {
        وضعیت_2 = U_ILLEGAL_آرگومان_ERROR;
        أرجع *هذا;
    }
    إذا (extensions_ == اشاره_تهی) {
        extensions_ = Locale::get_ریشه().clone();
        إذا (extensions_ == اشاره_تهی) {
            وضعیت_2 = U_حافظه_تخصیص_ERROR;
            أرجع *هذا;
        }
        extensions_->مجموعه_Keyword_مقدار(k_Attribute_کلید, مقدار_str.داده(), وضعیت_2);
        أرجع *هذا;
    }

    UError_کد local_Error_کد = U_ZERO_ERROR;
    خودکار صفات = extensions_->get_Keyword_مقدار<CharString>(k_Attribute_کلید, local_Error_کد);
    إذا (U_FAILURE(local_Error_کد)) {
        CharString new_صفات(مقدار_str.داده(), وضعیت_2);
        // No attributes, set the attribute.
        extensions_->مجموعه_Keyword_مقدار(k_Attribute_کلید, new_صفات.داده(), وضعیت_2);
        أرجع *هذا;
    }

    transform(صفات.داده(),صفات.length());
    ثابت حرف* شروع = صفات.داده();
    ثابت حرف* حد = صفات.داده() + صفات.length();
    CharString new_صفات;
    منطقي inserted = خطأ;
    طالما (شروع < حد) {
        إذا (!inserted) {
            صحيح cmp = uprv_strcmp(شروع, مقدار_str.داده());
            إذا (cmp == 0) { أرجع *هذا; }  // Found it in attributes: Just return
            إذا (cmp > 0) {
                إذا (!new_صفات.is_خالی()) new_صفات.append('_', وضعیت_2);
                new_صفات.append(مقدار_str.داده(), وضعیت_2);
                inserted = درست;
            }
        }
        إذا (!new_صفات.is_خالی()) {
            new_صفات.append('_', وضعیت_2);
        }
        new_صفات.append(شروع, وضعیت_2);
        شروع += uprv_strlen(شروع) + 1;
    }
    إذا (!inserted) {
        إذا (!new_صفات.is_خالی()) {
            new_صفات.append('_', وضعیت_2);
        }
        new_صفات.append(مقدار_str.داده(), وضعیت_2);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->مجموعه_Keyword_مقدار(k_Attribute_کلید, new_صفات.داده(), وضعیت_2);
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString مقدار_str(value, وضعیت_2);
    إذا (U_FAILURE(وضعیت_2)) { أرجع *هذا; }
    transform(مقدار_str.داده(), مقدار_str.length());
    إذا (!ultag_isUnicodeLocaleAttribute(مقدار_str.داده(), مقدار_str.length())) {
        وضعیت_2 = U_ILLEGAL_آرگومان_ERROR;
        أرجع *هذا;
    }
    إذا (extensions_ == اشاره_تهی) { أرجع *هذا; }
    UError_کد local_Error_کد = U_ZERO_ERROR;
    خودکار صفات = extensions_->get_Keyword_مقدار<CharString>(k_Attribute_کلید, local_Error_کد);
    // get failure, just return
    إذا (U_FAILURE(local_Error_کد)) { أرجع *هذا; }
    // Do not have any attributes, just return.
    إذا (صفات.is_خالی()) { أرجع *هذا; }

    حرف* p = صفات.داده();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    لكل (int32_t i = 0; i < صفات.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    ثابت حرف* شروع = صفات.داده();
    ثابت حرف* حد = صفات.داده() + صفات.length();
    CharString new_صفات;
    منطقي یافت = خطأ;
    طالما (شروع < حد) {
        إذا (uprv_strcmp(شروع, مقدار_str.داده()) == 0) {
            یافت = درست;
        } وإلا {
            إذا (!new_صفات.is_خالی()) {
                new_صفات.append('_', وضعیت_2);
            }
            new_صفات.append(شروع, وضعیت_2);
        }
        شروع += uprv_strlen(شروع) + 1;
    }
    // Found the value in attributes, set the attribute.
    إذا (یافت) {
        extensions_->مجموعه_Keyword_مقدار(k_Attribute_کلید, new_صفات.داده(), وضعیت_2);
    }
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::clear() U_LIFETIME_BOUND
{
    وضعیت_2 = U_ZERO_ERROR;
    language_[0] = 0;
    script_[0] = 0;
    region_[0] = 0;
    حذف variant_;
    variant_ = اشاره_تهی;
    clearExtensions();
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    حذف extensions_;
    extensions_ = اشاره_تهی;
    أرجع *هذا;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.مجموعه_مقصد_Bogus();
  أرجع bogus;
}

فراغ LocaleBuilder::copyExtensionsFrom(ثابت Locale& src, UError_کد& error_کد)
{
    إذا (U_FAILURE(error_کد)) { أرجع; }
    Local_اشاره_گر<icu::StringEnumeration> keywords(src.createKeywords(error_کد));
    إذا (U_FAILURE(error_کد) || keywords.isNull() || keywords->count(error_کد) == 0) {
        // Error, or no extensions to copy.
        أرجع;
    }
    إذا (extensions_ == اشاره_تهی) {
        extensions_ = Locale::get_ریشه().clone();
        إذا (extensions_ == اشاره_تهی) {
            وضعیت_2 = U_حافظه_تخصیص_ERROR;
            أرجع;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, خطأ, error_کد);
}

Locale LocaleBuilder::build(UError_کد& error_کد)
{
    إذا (U_FAILURE(error_کد)) {
        أرجع makeBogusLocale();
    }
    إذا (U_FAILURE(وضعیت_2)) {
        error_کد = وضعیت_2;
        أرجع makeBogusLocale();
    }
    CharString locale_str(language_, error_کد);
    إذا (uprv_strlen(script_) > 0) {
        locale_str.append('-', error_کد).append(StringPiece(script_), error_کد);
    }
    إذا (uprv_strlen(region_) > 0) {
        locale_str.append('-', error_کد).append(StringPiece(region_), error_کد);
    }
    إذا (variant_ != اشاره_تهی) {
        locale_str.append('-', error_کد).append(StringPiece(variant_->داده()), error_کد);
    }
    إذا (U_FAILURE(error_کد)) {
        أرجع makeBogusLocale();
    }
    Locale حاصلضرب(locale_str.داده());
    إذا (extensions_ != اشاره_تهی) {
        _copyExtensions(*extensions_, اشاره_تهی, حاصلضرب, درست, error_کد);
    }
    إذا (U_FAILURE(error_کد)) {
        أرجع makeBogusLocale();
    }
    أرجع حاصلضرب;
}

UBool LocaleBuilder::copy_Error_مقصد(UError_کد &بیرون_Error_کد) ثابت {
    إذا (U_FAILURE(بیرون_Error_کد)) {
        // Do not overwrite the older error code
        أرجع درست;
    }
    بیرون_Error_کد = وضعیت_2;
    أرجع U_FAILURE(بیرون_Error_کد);
}

U_NAMESPACE_انتها
