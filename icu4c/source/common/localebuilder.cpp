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
            أرجع ultag_is_Privateuse_قدر_Subtags(s, len);
        افتراضي:
            أرجع ultag_isExtensionSubtags(s, len);
    }
}

}  // namespace

U_NAMESPACE_BEGIN

LocaleBuilder::LocaleBuilder() : UObject(), حالت_2(U_ZERO_ERROR), language_(),
    script_(), region_(), variant_(خالی_سوچک), extensions_(خالی_سوچک)
{
    language_[0] = 0;
    script_[0] = 0;
    region_[0] = 0;
}

LocaleBuilder::~LocaleBuilder()
{
    مٹاؤ variant_;
    مٹاؤ extensions_;
}

LocaleBuilder& LocaleBuilder::سیٹ_Locale(ثابت Locale& locale) U_LIFETIME_BOUND
{
    clear();
    سیٹ_Language(locale.getLanguage());
    سیٹ_Script(locale.getScript());
    سیٹ_Region(locale.getCountry());
    سیٹ_Variant(locale.getVariant());
    extensions_ = locale.clone();
    إذا (extensions_ == خالی_سوچک) {
        حالت_2 = U_حافظہ_ونڈ_ERROR;
    }
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::سیٹ_Language_ٹیگ(StringPiece ٹیگ) U_LIFETIME_BOUND
{
    Locale l = Locale::for_Language_ٹیگ(ٹیگ, حالت_2);
    إذا (U_FAILURE(حالت_2)) { أرجع *هذا; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    سیٹ_Locale(l);
    أرجع *هذا;
}

نطاق {

فراغ سیٹ_میدان(StringPiece input, حرف* dest, UError_کوڈ& error_کوڈ,
              منطقي (*test)(ثابت حرف*, int32_t)) {
    إذا (U_FAILURE(error_کوڈ)) { أرجع; }
    إذا (input.empty()) {
        dest[0] = '\0';
    } وإلا إذا (test(input.انکڑے(), input.length())) {
        uprv_memcpy(dest, input.انکڑے(), input.length());
        dest[input.length()] = '\0';
    } وإلا {
        error_کوڈ = U_ILLEGAL_دلیل_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::سیٹ_Language(StringPiece language) U_LIFETIME_BOUND
{
    سیٹ_میدان(language, language_, حالت_2, &ultag_isLanguageSubtag);
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::سیٹ_Script(StringPiece script) U_LIFETIME_BOUND
{
    سیٹ_میدان(script, script_, حالت_2, &ultag_isScriptSubtag);
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::سیٹ_Region(StringPiece region) U_LIFETIME_BOUND
{
    سیٹ_میدان(region, region_, حالت_2, &ultag_isRegionSubtag);
    أرجع *هذا;
}

نطاق {

فراغ transform(حرف* انکڑے, int32_t len) {
    لكل (int32_t i = 0; i < len; i++, انکڑے++) {
        إذا (*انکڑے == '_') {
            *انکڑے = '-';
        } وإلا {
            *انکڑے = uprv_tolower(*انکڑے);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::سیٹ_Variant(StringPiece variant) U_LIFETIME_BOUND
{
    إذا (U_FAILURE(حالت_2)) { أرجع *هذا; }
    إذا (variant.empty()) {
        مٹاؤ variant_;
        variant_ = خالی_سوچک;
        أرجع *هذا;
    }
    FixedString* new_variant = جديد FixedString(variant);
    إذا (new_variant == خالی_سوچک || new_variant->is_خالی_تھاواں()) {
        حالت_2 = U_حافظہ_ونڈ_ERROR;
        أرجع *هذا;
    }
    transform(new_variant->getAlias(), variant.length());
    إذا (!ultag_isVariantSubtags(new_variant->انکڑے(), variant.length())) {
        مٹاؤ new_variant;
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        أرجع *هذا;
    }
    مٹاؤ variant_;
    variant_ = new_variant;
    أرجع *هذا;
}

نطاق {

منطقي
is_Keyword_قدر(ثابت حرف* کلید, ثابت حرف* value, int32_t قدر_len)
{
    إذا (کلید[1] == '\0') {
        // one char key
        أرجع (UPRV_ISALPHANUM(uprv_tolower(کلید[0])) &&
                _isExtensionSubtags(کلید[0], value, قدر_len));
    } وإلا إذا (uprv_strcmp(کلید, k_Attribute_کلید) == 0) {
        // unicode attributes
        أرجع ultag_is_Unicode_Locale_گن(value, قدر_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_کلید = ulocimp_منزل_Bcp_کلید_With_Fallback(کلید);
    std::optional<std::string_view> unicode_locale_type = ulocimp_منزل_Bcp_Type_With_Fallback(کلید, value);

    أرجع unicode_locale_کلید.has_value() &&
           unicode_locale_type.has_value() &&
           ultag_is_Unicode_Locale_کلید(unicode_locale_کلید->انکڑے(),
                                    static_cast<int32_t>(unicode_locale_کلید->size())) &&
           ultag_isUnicodeLocaleType(unicode_locale_type->انکڑے(),
                                     static_cast<int32_t>(unicode_locale_type->size()));
}

فراغ
_copyExtensions(ثابت Locale& from, icu::StringEnumeration *keywords,
                Locale& منزل, منطقي validate, UError_کوڈ& error_کوڈ)
{
    إذا (U_FAILURE(error_کوڈ)) { أرجع; }
    Local_اشارہ_گر<icu::StringEnumeration> ownedKeywords;
    إذا (keywords == خالی_سوچک) {
        ownedKeywords.adoptInstead(from.createKeywords(error_کوڈ));
        إذا (U_FAILURE(error_کوڈ) || ownedKeywords.isNull()) { أرجع; }
        keywords = ownedKeywords.getAlias();
    }
    ثابت حرف* کلید;
    طالما ((کلید = keywords->next(خالی_سوچک, error_کوڈ)) != خالی_سوچک) {
        خودکار value = from.get_Keyword_قدر<CharString>(کلید, error_کوڈ);
        إذا (U_FAILURE(error_کوڈ)) { أرجع; }
        إذا (uprv_strcmp(کلید, k_Attribute_کلید) == 0) {
            transform(value.انکڑے(), value.length());
        }
        إذا (validate &&
            !is_Keyword_قدر(کلید, value.انکڑے(), value.length())) {
            error_کوڈ = U_ILLEGAL_دلیل_ERROR;
            أرجع;
        }
        منزل.سیٹ_Keyword_قدر(کلید, value.انکڑے(), error_کوڈ);
        إذا (U_FAILURE(error_کوڈ)) { أرجع; }
    }
}

فراغ
clear_UAttributes_And_کلید_Type(Locale& locale, UError_کوڈ& error_کوڈ)
{
    إذا (U_FAILURE(error_کوڈ)) { أرجع; }
    // Clear Unicode attributes
    locale.سیٹ_Keyword_قدر(k_Attribute_کلید, "", error_کوڈ);

    // Clear all Unicode keyword values
    Local_اشارہ_گر<icu::StringEnumeration> iter(locale.createUnicodeKeywords(error_کوڈ));
    إذا (U_FAILURE(error_کوڈ) || iter.isNull()) { أرجع; }
    ثابت حرف* کلید;
    طالما ((کلید = iter->next(خالی_سوچک, error_کوڈ)) != خالی_سوچک) {
        locale.سیٹ_Unicode_Keyword_قدر(کلید, خالی_سوچک, error_کوڈ);
    }
}

فراغ
سیٹ_Unicode_Extensions(Locale& locale, ثابت CharString& value, UError_کوڈ& error_کوڈ)
{
    إذا (U_FAILURE(error_کوڈ)) { أرجع; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", error_کوڈ);
    locale_str.append(value, error_کوڈ);
    _copyExtensions(
        Locale::for_Language_ٹیگ(locale_str.انکڑے(), error_کوڈ), خالی_سوچک,
        locale, خطأ, error_کوڈ);
}

}  // namespace

LocaleBuilder& LocaleBuilder::سیٹ_Extension(حرف کلید, StringPiece value) U_LIFETIME_BOUND
{
    إذا (U_FAILURE(حالت_2)) { أرجع *هذا; }
    إذا (!UPRV_ISALPHANUM(کلید)) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        أرجع *هذا;
    }
    CharString قدر_str(value, حالت_2);
    إذا (U_FAILURE(حالت_2)) { أرجع *هذا; }
    transform(قدر_str.انکڑے(), قدر_str.length());
    إذا (!قدر_str.is_خالی_تھاواں() &&
            !_isExtensionSubtags(کلید, قدر_str.انکڑے(), قدر_str.length())) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        أرجع *هذا;
    }
    إذا (extensions_ == خالی_سوچک) {
        extensions_ = Locale::get_جڑ().clone();
        إذا (extensions_ == خالی_سوچک) {
            حالت_2 = U_حافظہ_ونڈ_ERROR;
            أرجع *هذا;
        }
    }
    إذا (uprv_tolower(کلید) != 'u') {
        // for t, x and others extension.
        extensions_->سیٹ_Keyword_قدر(StringPiece(&کلید, 1), قدر_str.انکڑے(),
                                     حالت_2);
        أرجع *هذا;
    }
    clear_UAttributes_And_کلید_Type(*extensions_, حالت_2);
    إذا (U_FAILURE(حالت_2)) { أرجع *هذا; }
    إذا (!value.empty()) {
        سیٹ_Unicode_Extensions(*extensions_, قدر_str, حالت_2);
    }
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::سیٹ_Unicode_Locale_Keyword(StringPiece کلید,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    إذا (U_FAILURE(حالت_2)) { أرجع *هذا; }
    إذا (!ultag_is_Unicode_Locale_کلید(کلید.انکڑے(), کلید.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.انکڑے(), type.length()))) {
      حالت_2 = U_ILLEGAL_دلیل_ERROR;
      أرجع *هذا;
    }
    إذا (extensions_ == خالی_سوچک) {
        extensions_ = Locale::get_جڑ().clone();
        إذا (extensions_ == خالی_سوچک) {
            حالت_2 = U_حافظہ_ونڈ_ERROR;
            أرجع *هذا;
        }
    }
    extensions_->سیٹ_Unicode_Keyword_قدر(کلید, type, حالت_2);
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::جوڑو_Unicode_Locale_Attribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString قدر_str(value, حالت_2);
    إذا (U_FAILURE(حالت_2)) { أرجع *هذا; }
    transform(قدر_str.انکڑے(), قدر_str.length());
    إذا (!ultag_isUnicodeLocaleAttribute(قدر_str.انکڑے(), قدر_str.length())) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        أرجع *هذا;
    }
    إذا (extensions_ == خالی_سوچک) {
        extensions_ = Locale::get_جڑ().clone();
        إذا (extensions_ == خالی_سوچک) {
            حالت_2 = U_حافظہ_ونڈ_ERROR;
            أرجع *هذا;
        }
        extensions_->سیٹ_Keyword_قدر(k_Attribute_کلید, قدر_str.انکڑے(), حالت_2);
        أرجع *هذا;
    }

    UError_کوڈ local_Error_کوڈ = U_ZERO_ERROR;
    خودکار گن = extensions_->get_Keyword_قدر<CharString>(k_Attribute_کلید, local_Error_کوڈ);
    إذا (U_FAILURE(local_Error_کوڈ)) {
        CharString new_گن(قدر_str.انکڑے(), حالت_2);
        // No attributes, set the attribute.
        extensions_->سیٹ_Keyword_قدر(k_Attribute_کلید, new_گن.انکڑے(), حالت_2);
        أرجع *هذا;
    }

    transform(گن.انکڑے(),گن.length());
    ثابت حرف* شروع = گن.انکڑے();
    ثابت حرف* حد_بندی = گن.انکڑے() + گن.length();
    CharString new_گن;
    منطقي inserted = خطأ;
    طالما (شروع < حد_بندی) {
        إذا (!inserted) {
            صحيح cmp = uprv_strcmp(شروع, قدر_str.انکڑے());
            إذا (cmp == 0) { أرجع *هذا; }  // Found it in attributes: Just return
            إذا (cmp > 0) {
                إذا (!new_گن.is_خالی_تھاواں()) new_گن.append('_', حالت_2);
                new_گن.append(قدر_str.انکڑے(), حالت_2);
                inserted = سچ;
            }
        }
        إذا (!new_گن.is_خالی_تھاواں()) {
            new_گن.append('_', حالت_2);
        }
        new_گن.append(شروع, حالت_2);
        شروع += uprv_strlen(شروع) + 1;
    }
    إذا (!inserted) {
        إذا (!new_گن.is_خالی_تھاواں()) {
            new_گن.append('_', حالت_2);
        }
        new_گن.append(قدر_str.انکڑے(), حالت_2);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->سیٹ_Keyword_قدر(k_Attribute_کلید, new_گن.انکڑے(), حالت_2);
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString قدر_str(value, حالت_2);
    إذا (U_FAILURE(حالت_2)) { أرجع *هذا; }
    transform(قدر_str.انکڑے(), قدر_str.length());
    إذا (!ultag_isUnicodeLocaleAttribute(قدر_str.انکڑے(), قدر_str.length())) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        أرجع *هذا;
    }
    إذا (extensions_ == خالی_سوچک) { أرجع *هذا; }
    UError_کوڈ local_Error_کوڈ = U_ZERO_ERROR;
    خودکار گن = extensions_->get_Keyword_قدر<CharString>(k_Attribute_کلید, local_Error_کوڈ);
    // get failure, just return
    إذا (U_FAILURE(local_Error_کوڈ)) { أرجع *هذا; }
    // Do not have any attributes, just return.
    إذا (گن.is_خالی_تھاواں()) { أرجع *هذا; }

    حرف* p = گن.انکڑے();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    لكل (int32_t i = 0; i < گن.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    ثابت حرف* شروع = گن.انکڑے();
    ثابت حرف* حد_بندی = گن.انکڑے() + گن.length();
    CharString new_گن;
    منطقي لبھیا = خطأ;
    طالما (شروع < حد_بندی) {
        إذا (uprv_strcmp(شروع, قدر_str.انکڑے()) == 0) {
            لبھیا = سچ;
        } وإلا {
            إذا (!new_گن.is_خالی_تھاواں()) {
                new_گن.append('_', حالت_2);
            }
            new_گن.append(شروع, حالت_2);
        }
        شروع += uprv_strlen(شروع) + 1;
    }
    // Found the value in attributes, set the attribute.
    إذا (لبھیا) {
        extensions_->سیٹ_Keyword_قدر(k_Attribute_کلید, new_گن.انکڑے(), حالت_2);
    }
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::clear() U_LIFETIME_BOUND
{
    حالت_2 = U_ZERO_ERROR;
    language_[0] = 0;
    script_[0] = 0;
    region_[0] = 0;
    مٹاؤ variant_;
    variant_ = خالی_سوچک;
    clearExtensions();
    أرجع *هذا;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    مٹاؤ extensions_;
    extensions_ = خالی_سوچک;
    أرجع *هذا;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.سیٹ_منزل_Bogus();
  أرجع bogus;
}

فراغ LocaleBuilder::copyExtensionsFrom(ثابت Locale& src, UError_کوڈ& error_کوڈ)
{
    إذا (U_FAILURE(error_کوڈ)) { أرجع; }
    Local_اشارہ_گر<icu::StringEnumeration> keywords(src.createKeywords(error_کوڈ));
    إذا (U_FAILURE(error_کوڈ) || keywords.isNull() || keywords->count(error_کوڈ) == 0) {
        // Error, or no extensions to copy.
        أرجع;
    }
    إذا (extensions_ == خالی_سوچک) {
        extensions_ = Locale::get_جڑ().clone();
        إذا (extensions_ == خالی_سوچک) {
            حالت_2 = U_حافظہ_ونڈ_ERROR;
            أرجع;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, خطأ, error_کوڈ);
}

Locale LocaleBuilder::build(UError_کوڈ& error_کوڈ)
{
    إذا (U_FAILURE(error_کوڈ)) {
        أرجع makeBogusLocale();
    }
    إذا (U_FAILURE(حالت_2)) {
        error_کوڈ = حالت_2;
        أرجع makeBogusLocale();
    }
    CharString locale_str(language_, error_کوڈ);
    إذا (uprv_strlen(script_) > 0) {
        locale_str.append('-', error_کوڈ).append(StringPiece(script_), error_کوڈ);
    }
    إذا (uprv_strlen(region_) > 0) {
        locale_str.append('-', error_کوڈ).append(StringPiece(region_), error_کوڈ);
    }
    إذا (variant_ != خالی_سوچک) {
        locale_str.append('-', error_کوڈ).append(StringPiece(variant_->انکڑے()), error_کوڈ);
    }
    إذا (U_FAILURE(error_کوڈ)) {
        أرجع makeBogusLocale();
    }
    Locale ضرب_حاصل(locale_str.انکڑے());
    إذا (extensions_ != خالی_سوچک) {
        _copyExtensions(*extensions_, خالی_سوچک, ضرب_حاصل, سچ, error_کوڈ);
    }
    إذا (U_FAILURE(error_کوڈ)) {
        أرجع makeBogusLocale();
    }
    أرجع ضرب_حاصل;
}

UBool LocaleBuilder::copy_Error_منزل(UError_کوڈ &باہرلا_Error_کوڈ) ثابت {
    إذا (U_FAILURE(باہرلا_Error_کوڈ)) {
        // Do not overwrite the older error code
        أرجع سچ;
    }
    باہرلا_Error_کوڈ = حالت_2;
    أرجع U_FAILURE(باہرلا_Error_کوڈ);
}

U_NAMESPACE_سرا
