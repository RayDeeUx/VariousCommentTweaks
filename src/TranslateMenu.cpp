#include <Geode/ui/GeodeUI.hpp>
#include <Geode/utils/web.hpp>
#include "TranslateMenu.hpp"
#include "Manager.hpp"
#include "Utils.hpp"

using namespace geode::prelude;

TranslateMenu* TranslateMenu::create(const std::string& text) {
	TranslateMenu* ret = new TranslateMenu();
	if (ret && ret->init(text)) {
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}

bool TranslateMenu::init(const std::string& text) {
	if (!geode::Popup::init(420.f, 200.f, "GJ_square02.png")) return false;

	std::string sourceLanguage = Utils::getString("sourceLanguage");
	if (geode::utils::string::toLower(sourceLanguage) == "detect language") sourceLanguage = "Auto";
	this->setTitle(fmt::format("From: {} • To: {}", sourceLanguage, Utils::getString("targetLanguage")));
	this->setID("TranslateMenu"_spr);

	this->m_title->limitLabelWidth(390.f, .9f, 0.001f);
	this->m_title->setID("title"_spr);
	this->m_bgSprite->setID("background"_spr);
	TranslateMenu::encodeToURL(text);

	ButtonSprite* cancelButton = ButtonSprite::create("    Close", "goldFont.fnt", "GJ_button_01.png", 0.7f);
	CCMenuItemSpriteExtra* cancelTranslateButton = CCMenuItemSpriteExtra::create(cancelButton, this, menu_selector(TranslateMenu::onClose));
	CCSprite* cancelButtonLogo = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
	cancelButtonLogo->setScale(0.9f);
	cancelButtonLogo->setID("cancel-logo"_spr);
	cancelButton->addChild(cancelButtonLogo);
	cancelButtonLogo->setPosition({24, 15});
	cancelTranslateButton->setID("cancel-button"_spr);

	ButtonSprite* libreTranslate = ButtonSprite::create("    LibreTranslate", "goldFont.fnt", "GJ_button_01.png", 0.7f);
	CCMenuItemSpriteExtra* libreTranslateButton = CCMenuItemSpriteExtra::create(libreTranslate, this, menu_selector(TranslateMenu::onLibreTranslate));
	CCSprite* libreTranslateLogo = CCSprite::createWithSpriteFrameName("libreTranslate.png"_spr);
	libreTranslateLogo->setScale(1.8f);
	libreTranslateLogo->setID("libre-translate-logo"_spr);
	libreTranslate->addChild(libreTranslateLogo);
	libreTranslateLogo->setPosition({24, 15});
	libreTranslateButton->setID("libre-translate-button"_spr);

	ButtonSprite* deeplTranslate = ButtonSprite::create("    DeepL", "goldFont.fnt", "GJ_button_01.png", 0.7f);
	CCMenuItemSpriteExtra* deeplTranslateButton = CCMenuItemSpriteExtra::create(deeplTranslate, this, menu_selector(TranslateMenu::onDeepLTranslate));
	CCSprite* deeplTranslateLogo = CCSprite::createWithSpriteFrameName("deeplTranslate.png"_spr);
	deeplTranslateLogo->setScale(1.8f);
	deeplTranslateLogo->setID("deepl-translate-logo"_spr);
	deeplTranslate->addChild(deeplTranslateLogo);
	deeplTranslateLogo->setPosition({24, 15});
	deeplTranslateButton->setID("deepl-translate-button"_spr);

	ButtonSprite* boogleTranslate = ButtonSprite::create("    Google", "goldFont.fnt", "GJ_button_01.png", 0.7f);
	CCMenuItemSpriteExtra* boogleTranslateButton = CCMenuItemSpriteExtra::create(boogleTranslate, this, menu_selector(TranslateMenu::onBoogleTranslate));
	CCSprite* boogleTranslateLogo = CCSprite::createWithSpriteFrameName("boogleTranslate.png"_spr);
	boogleTranslateLogo->setScale(1.8f);
	boogleTranslateLogo->setID("boogle-translate-logo"_spr);
	boogleTranslate->addChild(boogleTranslateLogo);
	boogleTranslateLogo->setPosition({24, 15});
	boogleTranslateButton->setID("boogle-translate-button"_spr);

	CCSprite* modSettingsButtonSprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
	modSettingsButtonSprite->setScale(.75f);
	CCMenuItemSpriteExtra* modSettingsButton = CCMenuItemSpriteExtra::create(modSettingsButtonSprite, this, menu_selector(TranslateMenu::onOpenModSettings));
	modSettingsButton->setID("mod-setting-button"_spr);

	CCMenu* buttonMenu = CCMenu::create();
	buttonMenu->setContentWidth(390.f);
	buttonMenu->addChild(cancelTranslateButton);
	buttonMenu->addChild(libreTranslateButton);
	buttonMenu->addChild(deeplTranslateButton);
	buttonMenu->addChild(boogleTranslateButton);
	buttonMenu->addChild(modSettingsButton);
	buttonMenu->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Center)->setAutoScale(true));
	buttonMenu->setID("translate-options-menu"_spr);

	geode::SimpleTextArea* originalCommentText = geode::SimpleTextArea::create("");
	originalCommentText->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
	originalCommentText->setWidth(this->m_mainLayer->getContentWidth() * .95f * .95f);
	originalCommentText->setText(text);
	originalCommentText->setID("original-comment-text"_spr);

	this->m_mainLayer->addChildAtPosition(buttonMenu, Anchor::Bottom, {0.f, 24.f});
	this->m_mainLayer->addChildAtPosition(originalCommentText, Anchor::Center);

	this->m_closeBtn->setID("close-button"_spr);
	this->m_buttonMenu->setID("button-menu"_spr);

	return true;
}

void TranslateMenu::onOpenModSettings(CCObject*) {
	openSettingsPopup(Mod::get());
}

void TranslateMenu::onBoogleTranslate(CCObject*) {
	if (!Utils::modEnabled() || !Utils::getBool("translateComments")) return;
	this->keyBackClicked();
	Manager* manager = Manager::getSharedInstance();
	const auto [sourceLanguage, targetLanguage] = TranslateMenu::findLanguageCodes(manager->languages);
	const std::string& url = fmt::format("https://translate.google.com/?sl={}&tl={}&text={}", sourceLanguage, targetLanguage, manager->urlEncoded);
	manager->urlEncoded = ""; // empty string to be safe; url has been created anyway
	if (CCKeyboardDispatcher::get()->getAltKeyPressed()) geode::utils::clipboard::write(url);
	else geode::utils::web::openLinkInBrowser(url);
}

void TranslateMenu::onDeepLTranslate(CCObject*) {
	if (!Utils::modEnabled() || !Utils::getBool("translateComments")) return;
	this->keyBackClicked();
	Manager* manager = Manager::getSharedInstance();
	const auto [sourceLanguage, targetLanguage] = TranslateMenu::findLanguageCodes(manager->deeplLanguages, true);
	geode::utils::web::openLinkInBrowser(fmt::format("https://www.deepl.com/en/translator#{}/{}/{}", targetLanguage, sourceLanguage, manager->urlEncoded));
	manager->urlEncoded = ""; // empty string to be safe; url has been created anyway
}

void TranslateMenu::onLibreTranslate(CCObject*) {
	if (!Utils::modEnabled() || !Utils::getBool("translateComments")) return;
	this->keyBackClicked();
	Manager* manager = Manager::getSharedInstance();
	const auto [sourceLanguage, targetLanguage] = TranslateMenu::findLanguageCodes(manager->languages);
	geode::utils::web::openLinkInBrowser(fmt::format("https://libretranslate.com/?source={}&target={}&q={}", sourceLanguage, targetLanguage, manager->urlEncoded));
	manager->urlEncoded = ""; // empty string to be safe; url has been created anyway
}

void TranslateMenu::encodeToURL(const std::string& text) {
	// adapted from tastyforreal
	// then edited for consistency
	// should work in most cases w/o "%20" or '+' collisions
	std::stringstream stream;
	for (const char c : utils::string::split(text)) {
		if (c == ' ') stream << '%' << '2' << '0';
		else if (c == '+' || c == '!' || c == '*' || c == '(' || c == ')' || c == '-' || c == '.' || c == '_' || isalnum(static_cast<unsigned char>(c))) stream << c;
		else stream << '%' << std::hex << (static_cast<unsigned int>(c) & 0xFF);
	}
	Manager::getSharedInstance()->urlEncoded = stream.str();
}

std::pair<std::string, std::string> TranslateMenu::findLanguageCodes(const std::unordered_map<std::string, std::string>& languagesMap, const bool isDeepL) {
	const std::string& sourceLangSetting = Utils::getString("sourceLanguage");
	const std::string& targetLangSetting = Utils::getString("targetLanguage");
	const std::string& sourceLang = languagesMap.contains(sourceLangSetting)
		? languagesMap.find(sourceLangSetting)->second : isDeepL ? "es" : "auto";
	const std::string& targetLang = languagesMap.contains(targetLangSetting)
		? languagesMap.find(targetLangSetting)->second : "en";
	return {sourceLang, targetLang};
}