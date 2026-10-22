// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#شامل <optional>
#شامل <string_view>
#شامل <utility>

#شامل "bytesinkutil.h"  // StringByteSink<CharString>
#شامل "charstr.h"
#شامل "cstring.h"
#شامل "fixedstring.h"
#شامل "ulocimp.h"
#شامل "unicode/localebuilder.h"
#شامل "unicode/locid.h"

نامخلا {

اندرونی منطقی UPRV_ISDIGIT(حرف c) { واپس c >= '0' && c <= '9'; }
اندرونی منطقی UPRV_ISALPHANUM(حرف c) { واپس uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr ثابت حرف* k_Attribute_کلید = "attribute";

منطقی _isExtensionSubtags(حرف کلید, ثابت حرف* s, int32_t len) {
    انتخاب (uprv_tolower(کلید)) {
        حالت 'u':
            واپس ultag_isUnicodeExtensionSubtags(s, len);
        حالت 't':
            واپس ultag_isTransformedExtensionSubtags(s, len);
        حالت 'x':
            واپس ultag_is_Privateuse_قدر_Subtags(s, len);
        طےشدہ:
            واپس ultag_isExtensionSubtags(s, len);
    }
}

}  // namespace

U_NAMESPACE_BEGIN

LocaleBuilder::LocaleBuilder() : UObject(), حالت_3(U_ZERO_ERROR), language_(),
    script_(), region_(), variant_(خالی_اشاریہ), extensions_(خالی_اشاریہ)
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

LocaleBuilder& LocaleBuilder::سیٹ_Locale(ثابت Locale& locale) U_LIFETIME_BOUND
{
    clear();
    سیٹ_Language(locale.getLanguage());
    سیٹ_Script(locale.getScript());
    سیٹ_Region(locale.getCountry());
    سیٹ_Variant(locale.getVariant());
    extensions_ = locale.clone();
    اگر (extensions_ == خالی_اشاریہ) {
        حالت_3 = U_حافظہ_تخصیص_ERROR;
    }
    واپس *یہ;
}

LocaleBuilder& LocaleBuilder::سیٹ_Language_لیبل(StringPiece لیبل) U_LIFETIME_BOUND
{
    Locale l = Locale::for_Language_لیبل(لیبل, حالت_3);
    اگر (U_FAILURE(حالت_3)) { واپس *یہ; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    سیٹ_Locale(l);
    واپس *یہ;
}

نامخلا {

خالی سیٹ_میدان(StringPiece input, حرف* dest, UError_رمز& error_رمز,
              منطقی (*test)(ثابت حرف*, int32_t)) {
    اگر (U_FAILURE(error_رمز)) { واپس; }
    اگر (input.empty()) {
        dest[0] = '\0';
    } ورنہ اگر (test(input.ڈیٹا(), input.length())) {
        uprv_memcpy(dest, input.ڈیٹا(), input.length());
        dest[input.length()] = '\0';
    } ورنہ {
        error_رمز = U_ILLEGAL_دلیل_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::سیٹ_Language(StringPiece language) U_LIFETIME_BOUND
{
    سیٹ_میدان(language, language_, حالت_3, &ultag_isLanguageSubtag);
    واپس *یہ;
}

LocaleBuilder& LocaleBuilder::سیٹ_Script(StringPiece script) U_LIFETIME_BOUND
{
    سیٹ_میدان(script, script_, حالت_3, &ultag_isScriptSubtag);
    واپس *یہ;
}

LocaleBuilder& LocaleBuilder::سیٹ_Region(StringPiece region) U_LIFETIME_BOUND
{
    سیٹ_میدان(region, region_, حالت_3, &ultag_isRegionSubtag);
    واپس *یہ;
}

نامخلا {

خالی transform(حرف* ڈیٹا, int32_t len) {
    برائے (int32_t i = 0; i < len; i++, ڈیٹا++) {
        اگر (*ڈیٹا == '_') {
            *ڈیٹا = '-';
        } ورنہ {
            *ڈیٹا = uprv_tolower(*ڈیٹا);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::سیٹ_Variant(StringPiece variant) U_LIFETIME_BOUND
{
    اگر (U_FAILURE(حالت_3)) { واپس *یہ; }
    اگر (variant.empty()) {
        حذف variant_;
        variant_ = خالی_اشاریہ;
        واپس *یہ;
    }
    FixedString* new_variant = نیا FixedString(variant);
    اگر (new_variant == خالی_اشاریہ || new_variant->is_خالی_خانے()) {
        حالت_3 = U_حافظہ_تخصیص_ERROR;
        واپس *یہ;
    }
    transform(new_variant->getAlias(), variant.length());
    اگر (!ultag_isVariantSubtags(new_variant->ڈیٹا(), variant.length())) {
        حذف new_variant;
        حالت_3 = U_ILLEGAL_دلیل_ERROR;
        واپس *یہ;
    }
    حذف variant_;
    variant_ = new_variant;
    واپس *یہ;
}

نامخلا {

منطقی
is_Keyword_قدر(ثابت حرف* کلید, ثابت حرف* value, int32_t قدر_len)
{
    اگر (کلید[1] == '\0') {
        // one char key
        واپس (UPRV_ISALPHANUM(uprv_tolower(کلید[0])) &&
                _isExtensionSubtags(کلید[0], value, قدر_len));
    } ورنہ اگر (uprv_strcmp(کلید, k_Attribute_کلید) == 0) {
        // unicode attributes
        واپس ultag_is_Unicode_Locale_اوصاف(value, قدر_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_کلید = ulocimp_مقصد_Bcp_کلید_With_Fallback(کلید);
    std::optional<std::string_view> unicode_locale_type = ulocimp_مقصد_Bcp_Type_With_Fallback(کلید, value);

    واپس unicode_locale_کلید.has_value() &&
           unicode_locale_type.has_value() &&
           ultag_is_Unicode_Locale_کلید(unicode_locale_کلید->ڈیٹا(),
                                    static_cast<int32_t>(unicode_locale_کلید->size())) &&
           ultag_isUnicodeLocaleType(unicode_locale_type->ڈیٹا(),
                                     static_cast<int32_t>(unicode_locale_type->size()));
}

خالی
_copyExtensions(ثابت Locale& from, icu::StringEnumeration *keywords,
                Locale& مقصد, منطقی validate, UError_رمز& error_رمز)
{
    اگر (U_FAILURE(error_رمز)) { واپس; }
    Local_اشارہ_گر<icu::StringEnumeration> ownedKeywords;
    اگر (keywords == خالی_اشاریہ) {
        ownedKeywords.adoptInstead(from.createKeywords(error_رمز));
        اگر (U_FAILURE(error_رمز) || ownedKeywords.isNull()) { واپس; }
        keywords = ownedKeywords.getAlias();
    }
    ثابت حرف* کلید;
    جب ((کلید = keywords->next(خالی_اشاریہ, error_رمز)) != خالی_اشاریہ) {
        خودکار value = from.get_Keyword_قدر<CharString>(کلید, error_رمز);
        اگر (U_FAILURE(error_رمز)) { واپس; }
        اگر (uprv_strcmp(کلید, k_Attribute_کلید) == 0) {
            transform(value.ڈیٹا(), value.length());
        }
        اگر (validate &&
            !is_Keyword_قدر(کلید, value.ڈیٹا(), value.length())) {
            error_رمز = U_ILLEGAL_دلیل_ERROR;
            واپس;
        }
        مقصد.سیٹ_Keyword_قدر(کلید, value.ڈیٹا(), error_رمز);
        اگر (U_FAILURE(error_رمز)) { واپس; }
    }
}

خالی
clear_UAttributes_And_کلید_Type(Locale& locale, UError_رمز& error_رمز)
{
    اگر (U_FAILURE(error_رمز)) { واپس; }
    // Clear Unicode attributes
    locale.سیٹ_Keyword_قدر(k_Attribute_کلید, "", error_رمز);

    // Clear all Unicode keyword values
    Local_اشارہ_گر<icu::StringEnumeration> iter(locale.createUnicodeKeywords(error_رمز));
    اگر (U_FAILURE(error_رمز) || iter.isNull()) { واپس; }
    ثابت حرف* کلید;
    جب ((کلید = iter->next(خالی_اشاریہ, error_رمز)) != خالی_اشاریہ) {
        locale.سیٹ_Unicode_Keyword_قدر(کلید, خالی_اشاریہ, error_رمز);
    }
}

خالی
سیٹ_Unicode_Extensions(Locale& locale, ثابت CharString& value, UError_رمز& error_رمز)
{
    اگر (U_FAILURE(error_رمز)) { واپس; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", error_رمز);
    locale_str.append(value, error_رمز);
    _copyExtensions(
        Locale::for_Language_لیبل(locale_str.ڈیٹا(), error_رمز), خالی_اشاریہ,
        locale, جھوٹ, error_رمز);
}

}  // namespace

LocaleBuilder& LocaleBuilder::سیٹ_Extension(حرف کلید, StringPiece value) U_LIFETIME_BOUND
{
    اگر (U_FAILURE(حالت_3)) { واپس *یہ; }
    اگر (!UPRV_ISALPHANUM(کلید)) {
        حالت_3 = U_ILLEGAL_دلیل_ERROR;
        واپس *یہ;
    }
    CharString قدر_str(value, حالت_3);
    اگر (U_FAILURE(حالت_3)) { واپس *یہ; }
    transform(قدر_str.ڈیٹا(), قدر_str.length());
    اگر (!قدر_str.is_خالی_خانے() &&
            !_isExtensionSubtags(کلید, قدر_str.ڈیٹا(), قدر_str.length())) {
        حالت_3 = U_ILLEGAL_دلیل_ERROR;
        واپس *یہ;
    }
    اگر (extensions_ == خالی_اشاریہ) {
        extensions_ = Locale::get_جڑ().clone();
        اگر (extensions_ == خالی_اشاریہ) {
            حالت_3 = U_حافظہ_تخصیص_ERROR;
            واپس *یہ;
        }
    }
    اگر (uprv_tolower(کلید) != 'u') {
        // for t, x and others extension.
        extensions_->سیٹ_Keyword_قدر(StringPiece(&کلید, 1), قدر_str.ڈیٹا(),
                                     حالت_3);
        واپس *یہ;
    }
    clear_UAttributes_And_کلید_Type(*extensions_, حالت_3);
    اگر (U_FAILURE(حالت_3)) { واپس *یہ; }
    اگر (!value.empty()) {
        سیٹ_Unicode_Extensions(*extensions_, قدر_str, حالت_3);
    }
    واپس *یہ;
}

LocaleBuilder& LocaleBuilder::سیٹ_Unicode_Locale_Keyword(StringPiece کلید,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    اگر (U_FAILURE(حالت_3)) { واپس *یہ; }
    اگر (!ultag_is_Unicode_Locale_کلید(کلید.ڈیٹا(), کلید.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.ڈیٹا(), type.length()))) {
      حالت_3 = U_ILLEGAL_دلیل_ERROR;
      واپس *یہ;
    }
    اگر (extensions_ == خالی_اشاریہ) {
        extensions_ = Locale::get_جڑ().clone();
        اگر (extensions_ == خالی_اشاریہ) {
            حالت_3 = U_حافظہ_تخصیص_ERROR;
            واپس *یہ;
        }
    }
    extensions_->سیٹ_Unicode_Keyword_قدر(کلید, type, حالت_3);
    واپس *یہ;
}

LocaleBuilder& LocaleBuilder::جوڑو_Unicode_Locale_Attribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString قدر_str(value, حالت_3);
    اگر (U_FAILURE(حالت_3)) { واپس *یہ; }
    transform(قدر_str.ڈیٹا(), قدر_str.length());
    اگر (!ultag_isUnicodeLocaleAttribute(قدر_str.ڈیٹا(), قدر_str.length())) {
        حالت_3 = U_ILLEGAL_دلیل_ERROR;
        واپس *یہ;
    }
    اگر (extensions_ == خالی_اشاریہ) {
        extensions_ = Locale::get_جڑ().clone();
        اگر (extensions_ == خالی_اشاریہ) {
            حالت_3 = U_حافظہ_تخصیص_ERROR;
            واپس *یہ;
        }
        extensions_->سیٹ_Keyword_قدر(k_Attribute_کلید, قدر_str.ڈیٹا(), حالت_3);
        واپس *یہ;
    }

    UError_رمز local_Error_رمز = U_ZERO_ERROR;
    خودکار اوصاف = extensions_->get_Keyword_قدر<CharString>(k_Attribute_کلید, local_Error_رمز);
    اگر (U_FAILURE(local_Error_رمز)) {
        CharString new_اوصاف(قدر_str.ڈیٹا(), حالت_3);
        // No attributes, set the attribute.
        extensions_->سیٹ_Keyword_قدر(k_Attribute_کلید, new_اوصاف.ڈیٹا(), حالت_3);
        واپس *یہ;
    }

    transform(اوصاف.ڈیٹا(),اوصاف.length());
    ثابت حرف* آغاز = اوصاف.ڈیٹا();
    ثابت حرف* حد_بندی = اوصاف.ڈیٹا() + اوصاف.length();
    CharString new_اوصاف;
    منطقی inserted = جھوٹ;
    جب (آغاز < حد_بندی) {
        اگر (!inserted) {
            صحیح cmp = uprv_strcmp(آغاز, قدر_str.ڈیٹا());
            اگر (cmp == 0) { واپس *یہ; }  // Found it in attributes: Just return
            اگر (cmp > 0) {
                اگر (!new_اوصاف.is_خالی_خانے()) new_اوصاف.append('_', حالت_3);
                new_اوصاف.append(قدر_str.ڈیٹا(), حالت_3);
                inserted = سچ;
            }
        }
        اگر (!new_اوصاف.is_خالی_خانے()) {
            new_اوصاف.append('_', حالت_3);
        }
        new_اوصاف.append(آغاز, حالت_3);
        آغاز += uprv_strlen(آغاز) + 1;
    }
    اگر (!inserted) {
        اگر (!new_اوصاف.is_خالی_خانے()) {
            new_اوصاف.append('_', حالت_3);
        }
        new_اوصاف.append(قدر_str.ڈیٹا(), حالت_3);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->سیٹ_Keyword_قدر(k_Attribute_کلید, new_اوصاف.ڈیٹا(), حالت_3);
    واپس *یہ;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString قدر_str(value, حالت_3);
    اگر (U_FAILURE(حالت_3)) { واپس *یہ; }
    transform(قدر_str.ڈیٹا(), قدر_str.length());
    اگر (!ultag_isUnicodeLocaleAttribute(قدر_str.ڈیٹا(), قدر_str.length())) {
        حالت_3 = U_ILLEGAL_دلیل_ERROR;
        واپس *یہ;
    }
    اگر (extensions_ == خالی_اشاریہ) { واپس *یہ; }
    UError_رمز local_Error_رمز = U_ZERO_ERROR;
    خودکار اوصاف = extensions_->get_Keyword_قدر<CharString>(k_Attribute_کلید, local_Error_رمز);
    // get failure, just return
    اگر (U_FAILURE(local_Error_رمز)) { واپس *یہ; }
    // Do not have any attributes, just return.
    اگر (اوصاف.is_خالی_خانے()) { واپس *یہ; }

    حرف* p = اوصاف.ڈیٹا();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    برائے (int32_t i = 0; i < اوصاف.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    ثابت حرف* آغاز = اوصاف.ڈیٹا();
    ثابت حرف* حد_بندی = اوصاف.ڈیٹا() + اوصاف.length();
    CharString new_اوصاف;
    منطقی ملا = جھوٹ;
    جب (آغاز < حد_بندی) {
        اگر (uprv_strcmp(آغاز, قدر_str.ڈیٹا()) == 0) {
            ملا = سچ;
        } ورنہ {
            اگر (!new_اوصاف.is_خالی_خانے()) {
                new_اوصاف.append('_', حالت_3);
            }
            new_اوصاف.append(آغاز, حالت_3);
        }
        آغاز += uprv_strlen(آغاز) + 1;
    }
    // Found the value in attributes, set the attribute.
    اگر (ملا) {
        extensions_->سیٹ_Keyword_قدر(k_Attribute_کلید, new_اوصاف.ڈیٹا(), حالت_3);
    }
    واپس *یہ;
}

LocaleBuilder& LocaleBuilder::clear() U_LIFETIME_BOUND
{
    حالت_3 = U_ZERO_ERROR;
    language_[0] = 0;
    script_[0] = 0;
    region_[0] = 0;
    حذف variant_;
    variant_ = خالی_اشاریہ;
    clearExtensions();
    واپس *یہ;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    حذف extensions_;
    extensions_ = خالی_اشاریہ;
    واپس *یہ;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.سیٹ_مقصد_Bogus();
  واپس bogus;
}

خالی LocaleBuilder::copyExtensionsFrom(ثابت Locale& src, UError_رمز& error_رمز)
{
    اگر (U_FAILURE(error_رمز)) { واپس; }
    Local_اشارہ_گر<icu::StringEnumeration> keywords(src.createKeywords(error_رمز));
    اگر (U_FAILURE(error_رمز) || keywords.isNull() || keywords->count(error_رمز) == 0) {
        // Error, or no extensions to copy.
        واپس;
    }
    اگر (extensions_ == خالی_اشاریہ) {
        extensions_ = Locale::get_جڑ().clone();
        اگر (extensions_ == خالی_اشاریہ) {
            حالت_3 = U_حافظہ_تخصیص_ERROR;
            واپس;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, جھوٹ, error_رمز);
}

Locale LocaleBuilder::build(UError_رمز& error_رمز)
{
    اگر (U_FAILURE(error_رمز)) {
        واپس makeBogusLocale();
    }
    اگر (U_FAILURE(حالت_3)) {
        error_رمز = حالت_3;
        واپس makeBogusLocale();
    }
    CharString locale_str(language_, error_رمز);
    اگر (uprv_strlen(script_) > 0) {
        locale_str.append('-', error_رمز).append(StringPiece(script_), error_رمز);
    }
    اگر (uprv_strlen(region_) > 0) {
        locale_str.append('-', error_رمز).append(StringPiece(region_), error_رمز);
    }
    اگر (variant_ != خالی_اشاریہ) {
        locale_str.append('-', error_رمز).append(StringPiece(variant_->ڈیٹا()), error_رمز);
    }
    اگر (U_FAILURE(error_رمز)) {
        واپس makeBogusLocale();
    }
    Locale حاصل_ضرب(locale_str.ڈیٹا());
    اگر (extensions_ != خالی_اشاریہ) {
        _copyExtensions(*extensions_, خالی_اشاریہ, حاصل_ضرب, سچ, error_رمز);
    }
    اگر (U_FAILURE(error_رمز)) {
        واپس makeBogusLocale();
    }
    واپس حاصل_ضرب;
}

UBool LocaleBuilder::copy_Error_مقصد(UError_رمز &برآمد_Error_رمز) ثابت {
    اگر (U_FAILURE(برآمد_Error_رمز)) {
        // Do not overwrite the older error code
        واپس سچ;
    }
    برآمد_Error_رمز = حالت_3;
    واپس U_FAILURE(برآمد_Error_رمز);
}

U_NAMESPACE_انتہا
