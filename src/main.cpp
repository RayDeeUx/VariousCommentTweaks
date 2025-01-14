#include <Geode/modify/MenuLayer.hpp>
#include "Settings.hpp"
#include "Manager.hpp"
#include "Utils.hpp"

using namespace geode::prelude;

$on_mod(Loaded) {
	(void) Mod::get()->registerCustomSettingType("configdir", &MyButtonSettingV3::parse);
	(void) Mod::get()->registerCustomSettingType("likebaitexterminator", &MyButtonSettingV3::parse);
	auto pathDislikedWords = (Mod::get()->getConfigDir() / "dislikedWords.txt");
	auto pathIgnoredUsers = (Mod::get()->getConfigDir() / "ignoredUsers.txt");
	if (!std::filesystem::exists(pathDislikedWords)) {
		std::string content = R"(# hello there
# this is the text file where you add phrases or words you don't want to see in comments
# separate phrases/words by new lines like you see in this file
# also, i didn't include any words in here by default as that would
# cause more confusion for you in the long run, let's be honest
# don't worry, the mod ignores all lines that start with "#"
# you will need to restart the game to apply your changes when editing this file
# all matches will be case insensitive and will ignore your comments; keep that in mind! :)
# have fun!
# --raydeeux)";
		(void) utils::file::writeString(pathDislikedWords, content);
	} else {
		std::ifstream dislikedWordsFile(pathDislikedWords);
		std::string dislikedString;
		while (std::getline(dislikedWordsFile, dislikedString)) {
			if (dislikedString.starts_with('#') || dislikedString.empty()) continue;
			Manager::getSharedInstance()->dislikedWords.push_back(utils::string::toLower(dislikedString));
		}
	}
	if (!std::filesystem::exists(pathIgnoredUsers)) {
		std::string content = R"(# hello there
# this is the text file where you add user IDs of those whose comments you'd like to ignore
# separate user IDs by new lines like you see in this file
# also, i didn't include any user IDs in here by default as that would
# cause more confusion for you in the long run, let's be honest
# don't worry, the mod ignores all lines that start with "#" and aren't exclusively numbers
# you will need to restart the game to apply your changes when editing this file
# any crashes or bugs caused by improperly editing this file will be ignored
# have fun!
# --raydeeux)";
		(void) utils::file::writeString(pathIgnoredUsers, content);
	} else {
		std::ifstream ignoredUsersFile(pathIgnoredUsers);
		std::string ignoredUserString;
		while (std::getline(ignoredUsersFile, ignoredUserString)) {
			if (ignoredUserString.starts_with('#') || ignoredUserString.empty()) continue;
			std::string ignoredUserStringModified = ignoredUserString;
			if (ignoredUserStringModified.ends_with(" [VCT] #")) {
				ignoredUserStringModified = ignoredUserStringModified.substr(0, ignoredUserStringModified.find(" # [VCT] Username: "));
			}
			int ignoredUserID = utils::numFromString<int>(ignoredUserStringModified).unwrapOr(-2);
			if (ignoredUserID > 0 && ignoredUserID != 71) Manager::getSharedInstance()->ignoredUsers.insert(ignoredUserID);
		}
	}
	listenForSettingChanges("sourceLanguage", [](std::string sourceLanguage) {
		if (sourceLanguage == "Detect language") sourceLanguage = "Auto";
		std::string otherLanguage = Utils::getString("targetLanguage");
		if (otherLanguage == "Detect language") otherLanguage = "Auto";
		const auto title = typeinfo_cast<CCLabelBMFont*>(CCScene::get()->querySelector("TranslateMenu > main-layer > title"));
		if (!title) return;
		title->setString(fmt::format("From: {} • To: {}", sourceLanguage, otherLanguage).c_str());
		title->limitLabelWidth(390.f, .9f, 0.001f);
	});
	listenForSettingChanges("targetLanguage", [](std::string targetLanguage) {
		if (targetLanguage == "Detect language") targetLanguage = "Auto";
		std::string otherLanguage = Utils::getString("sourceLanguage");
		if (otherLanguage == "Detect language") otherLanguage = "Auto";
		const auto title = typeinfo_cast<CCLabelBMFont*>(CCScene::get()->querySelector("TranslateMenu > main-layer > title"));
		if (!title) return;
		title->setString(fmt::format("From: {} • To: {}", otherLanguage, targetLanguage).c_str());
		title->limitLabelWidth(390.f, .9f, 0.001f);
	});
}

class $modify(MyMenuLayer, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;

		if (Manager::getSharedInstance()->calledAlready) return true;
		const auto manager = Manager::getSharedInstance();
		manager->calledAlready = true;

		manager->ownUsername = GameManager::get()->m_playerName;

		return true;
	}
};