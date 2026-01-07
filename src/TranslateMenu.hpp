#pragma once

class TranslateMenu final : public geode::Popup<const std::string&> {
protected:
	void onOpenModSettings(CCObject*);
public:
	static TranslateMenu* create(const std::string&);
	bool setup(const std::string& text) override;
	static std::pair<std::string, std::string> findLanguageCodes(const std::unordered_map<std::string, std::string>& languagesMap, const bool isDeepL = false);
	static void encodeToURL(const std::string&);
	void onBoogleTranslate(CCObject*);
	void onLibreTranslate(CCObject*);
	void onDeepLTranslate(CCObject*);
};