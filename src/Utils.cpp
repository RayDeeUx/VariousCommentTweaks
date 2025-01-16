#include "Utils.hpp"

using namespace geode::cocos;

namespace Utils {
	template<class T> T getSetting(const std::string& setting) { return Mod::get()->getSettingValue<T>(setting); }

	bool getBool(const std::string& setting) { return getSetting<bool>(setting); }
	
	int64_t getInt(const std::string& setting) { return getSetting<int64_t>(setting); }
	
	double getDouble(const std::string& setting) { return getSetting<double>(setting); }

	std::string getString(const std::string& setting) { return getSetting<std::string>(setting); }

	ccColor3B getColor(const std::string& setting) { return getSetting<ccColor3B>(setting); }

	ccColor4B getColorAlpha(const std::string& setting) { return getSetting<ccColor4B>(setting); }

	bool modEnabled() { return getBool("enabled"); }
	
	bool isModLoaded(const std::string& modID) { return Loader::get()->isModLoaded(modID); }

	Mod* getMod(const std::string& modID) { return Loader::get()->getLoadedMod(modID); }

	std::string getModVersion(Mod* mod) { return mod->getVersion().toNonVString(); }

	void addIgnoredUser(int accountID, std::string username) {
		log::info("ignoring user: {} (username: {})", accountID, username);
		Manager::getSharedInstance()->ignoredUsers.push_back(accountID);
		std::ofstream output;
		output.open((Mod::get()->getConfigDir() / "ignoredUsers.txt"), std::ios_base::app);
		output << std::endl << fmt::format("{} # [VCT] Username: {} [VCT] #", accountID, username);
		output.close();
	}

	bool updateLists(Manager* manager) {
		manager->dislikedWords.clear();
		manager->ignoredUsers.clear();
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
				manager->dislikedWords.push_back(utils::string::toLower(dislikedString));
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
				if (ignoredUserStringModified.ends_with(" [VCT] #"))
					ignoredUserStringModified = ignoredUserStringModified.substr(0, ignoredUserStringModified.find(" # [VCT] Username: "));
				int ignoredUserID = utils::numFromString<int>(ignoredUserStringModified).unwrapOr(-2);
				if (ignoredUserID > 0 && ignoredUserID != 71) manager->ignoredUsers.push_back(ignoredUserID);
			}
		}
		return true;
	}
}