#include <Geode/modify/CommentCell.hpp>
#include "TranslateMenu.hpp"
#include "Manager.hpp"
#include "Utils.hpp"

#define MENU_ID "the-menu-with-buttons-that-im-too-lazy-to-move-into-main-menu-sorry"_spr
#define REPLACEMENT_ID "hidden-comment-replacement-label"_spr
#define formattedReason fmt::format("[{}]", reason).c_str()

using namespace geode::prelude;

class $modify(MyCommentCell, CommentCell) {
	static void onModify(auto& self) {
		(void) self.setHookPriority("CommentCell::loadFromComment", -3999); // using -3999 int prio because im doing stuff before AND after a function call and an enum would make it unclear. also -2123456789 prio on top
	}
	struct Fields {
		int authorAccountID;
		std::string authorUsername;
		std::string originalCommentText;
		bool isHidden = false;
	};
	void loadFromComment(GJComment* comment) {
		m_fields->originalCommentText = comment->m_commentString; // store comment text early before other mods edit the member variable

		CommentCell::loadFromComment(comment);
		m_fields->authorAccountID = comment->m_accountID;
		m_fields->authorUsername = comment->m_userName;

		if (!Utils::modEnabled() || comment->m_commentDeleted || !m_mainLayer) return;

		const auto manager = Manager::getSharedInstance();

		const auto dateLabel = this->getChildByIDRecursive("date-label");
		const auto commentTextLabel = this->getChildByIDRecursive("comment-text-label");
		if (m_comment->m_isSpam || !dateLabel) return;

		if (Utils::getBool("nameInButton")) MyCommentCell::replaceButtonWithName(comment);
		if (Utils::getBool("blendingComments")) MyCommentCell::applyBlendingToComment();

		const bool isLargeComment = this->m_height != 36;
		const float spriteScale = isLargeComment ? 0.95f : 1.0f;
		const float menuContentWidth = !isLargeComment ? 30.f : 40.f;

		CircleButtonSprite* copyButtonSprite = CircleButtonSprite::createWithSpriteFrameName("copy.png"_spr, spriteScale, CircleBaseColor::Green, CircleBaseSize::Tiny);
		CCMenuItemSpriteExtra* copyStuffButton = CCMenuItemSpriteExtra::create(copyButtonSprite, this, menu_selector(MyCommentCell::onVCTCopy));
		copyStuffButton->setID("copy-button"_spr);

		CircleButtonSprite* markAsHiddenSprite = CircleButtonSprite::createWithSpriteFrameName("hide.png"_spr, spriteScale, CircleBaseColor::Green, CircleBaseSize::Tiny);
		if (const auto sprite = markAsHiddenSprite->getChildByType<CCSprite>(0)) sprite->setColor({255, 225, 50}); // i don't trust CircleButtonSprite::create()
		markAsHiddenSprite->setID("mark-as-hidden"_spr);
		CircleButtonSprite* markAsVisibleSprite = CircleButtonSprite::createWithSpriteFrameName("hide.png"_spr, spriteScale, CircleBaseColor::Gray, CircleBaseSize::Tiny);
		if (const auto sprite = markAsVisibleSprite->getChildByType<CCSprite>(0)) sprite->setColor({255, 255, 255}); // i don't trust CircleButtonSprite::create()
		markAsVisibleSprite->setID("mark-as-visible"_spr);
		CCMenuItemToggler* markAsHiddenToggler = CCMenuItemToggler::create(markAsHiddenSprite, markAsVisibleSprite, this, menu_selector(MyCommentCell::onVCTHide));
		markAsHiddenToggler->setID("hidden-toggler"_spr);

		CircleButtonSprite* ignoreButtonSprite = CircleButtonSprite::createWithSpriteFrameName("ignore.png"_spr, spriteScale, CircleBaseColor::Green, CircleBaseSize::Tiny);
		CCMenuItemSpriteExtra* ignoreButton = CCMenuItemSpriteExtra::create(ignoreButtonSprite, this, menu_selector(MyCommentCell::onVCTIgnore));
		ignoreButton->setID("ignore-button"_spr);

		CircleButtonSprite* translateButtonSprite = CircleButtonSprite::createWithSpriteFrameName("translate.png"_spr, spriteScale, CircleBaseColor::Green, CircleBaseSize::Tiny);
		CCMenuItemSpriteExtra* translateButton = CCMenuItemSpriteExtra::create(translateButtonSprite, this, menu_selector(MyCommentCell::onVCTTrans));
		translateButton->setID("translate-button"_spr);

		const float dateLabelScale = dateLabel->getScale();

		CCMenu* menu = CCMenu::create();
		if (Utils::getBool("copyCommentText")) menu->addChild(copyStuffButton);
		if (Utils::getBool("toggleCommentVisibility")) menu->addChild(markAsHiddenToggler);
		if (Utils::getBool("ignorePeople") && comment->m_userName != Manager::getSharedInstance()->ownUsername) menu->addChild(ignoreButton);
		if (Utils::getBool("translateComments")) menu->addChild(translateButton);

		menu->setContentWidth(menuContentWidth);

		const auto tempLabel = static_cast<CCLabelBMFont*>(dateLabel);
		menu->setPositionX((dateLabel->getPositionX() - (dateLabel->getContentWidth() / 2.f)) + (isLargeComment ? -2.f : 7.5f));
		if (static_cast<std::string>(tempLabel->getString()).length() > 12 && !isLargeComment) menu->setPositionX(menu->getPositionX() + 5.25f); // stupit edge case >:(
		menu->setPositionY(dateLabel->getPositionY());

		AxisLayout* layout = RowLayout::create()->setGap(2.f)->setDefaultScaleLimits(dateLabelScale, dateLabelScale);
		layout->ignoreInvisibleChildren(true);
		menu->setLayout(layout);
		menu->setID(MENU_ID);
		menu->setAnchorPoint({1.f, 0.5f});
		if (Utils::getBool("largerButtons")) menu->setScale(1.2f); else menu->setScale(1.0f);
		this->m_mainLayer->addChild(menu);

		if (Utils::getBool("ignorePeople") && manager->ignoredUsers.contains(comment->m_accountID)) {
			MyCommentCell::passiveHidingComment("Comment from someone you ignored");
			MyCommentCell::hideButtons(menu);
		} else if (Utils::getBool("personalFilter") && manager->ownUsername != comment->m_userName && string::containsAny(string::toLower(m_fields->originalCommentText), manager->dislikedWords)) {
			MyCommentCell::passiveHidingComment("Comment contains word(s) you dislike");
			MyCommentCell::hideButtons(menu, false);
		} else if (Utils::getBool("hideRoleplay")) { // roleplay filter (inaccurate?)
			int numAsterisks = 0;
			int numVerbs = 0;
			for (const std::string& word : string::split(string::toLower(m_fields->originalCommentText), " ")) {
				if (utils::string::startsWith(word, "*")) numAsterisks += 1;
				if (numAsterisks % 2 == 0) continue;
				if (utils::string::endsWith(word, "ing") || utils::string::endsWith(word, "ed") || utils::string::endsWith(word, "s") || utils::string::endsWith(word, "es") || utils::string::endsWith(word, "ly"))
					numVerbs += 1;
				if (utils::string::endsWith(word, "ing*") || utils::string::endsWith(word, "ed*") || utils::string::endsWith(word, "s*") || utils::string::endsWith(word, "es*") || utils::string::endsWith(word, "ly*")) {
					numVerbs += 1;
					numAsterisks += 1;
				}
				else if (utils::string::endsWith(word, "*")) numAsterisks += 1;
			}
			if (numAsterisks > 1 && numVerbs > 0) {
				MyCommentCell::passiveHidingComment("Comment contains concerning roleplay");
				MyCommentCell::hideButtons(menu, false);
			}
		}

		if (isLargeComment || !commentTextLabel || (!isLargeComment && m_fields->originalCommentText.length() < 31)) return;
		if (menu->getChildrenCount() < 1) return menu->removeMeAndCleanup();

		const float scale = commentTextLabel->getScale();
		if (scale <= 0.0f) return;

		const float commentWidth = commentTextLabel->getContentWidth();
		if (commentWidth <= 0.0f) return;

		const float widthDiff = commentWidth - (menu->getContentWidth() * menu->getScale());
		if (widthDiff <= 0.0f) return;

		commentTextLabel->setScale(scale * (widthDiff / commentWidth) * .975f);
	}
	void applyBlendingToComment() {
		if (!Utils::modEnabled() || !Utils::getBool("blendingComments")) return;
		if (const auto label = this->getChildByIDRecursive("comment-text-label"); this->m_height == 36) return static_cast<CCLabelBMFont*>(label)->setBlendFunc({GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA});

		const auto commentTextArea = this->getChildByIDRecursive("comment-text-area");
		if (!commentTextArea) return;

		const auto multilineBMFont = commentTextArea->getChildByType<MultilineBitmapFont>(0);
		if (!multilineBMFont) return;

		for (CCLabelBMFont* labelChild : CCArrayExt<CCLabelBMFont*>(multilineBMFont->m_lines))
			labelChild->setBlendFunc({GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA});
	}
	void replaceButtonWithName(const GJComment* comment) {
		if (!Utils::modEnabled() || !comment->m_hasLevelID || !Utils::getBool("nameInButton")) return;

		const auto levelIDButton = this->querySelector("main-menu > level-id-button");
		if (!levelIDButton) return;

		const auto buttonSprite = levelIDButton->getChildByType<ButtonSprite>(0);
		if (!buttonSprite) return;

		const auto label = buttonSprite->getChildByType<CCLabelBMFont>(0);
		if (!label) return;

		const int levelID = comment->m_levelID;
		const auto glm = GameLevelManager::get();
		if ((levelID < 129 && levelID > 0) || (!glm->hasDownloadedLevel(levelID) && !glm->hasDownloadedList(levelID))) return;

		const auto level = glm->getSavedLevel(levelID);
		const auto list = glm->getSavedLevelList(-1 * levelID);

		if (!level && !list) return;

		CCLabelBMFont* newLabel = CCLabelBMFont::create("FOOBARBAZ", "bigFont.fnt");
		newLabel->setScale(label->getScale());

		if (level) newLabel->setString(level->m_levelName.c_str());
		else if (list) newLabel->setString(list->m_listName.c_str());

		label->setVisible(false);
		label->setOpacity(0);
		newLabel->limitLabelWidth(buttonSprite->getContentWidth() * 0.9f, label->getScale(), 0.01f);
		newLabel->setID("level-or-list-name"_spr);
		buttonSprite->addChild(newLabel);
		newLabel->setPosition(label->getPosition());
	}
	void onVCTTrans(CCObject*) {
		if (!Utils::modEnabled() || !Utils::getBool("translateComments")) return;
		TranslateMenu::create(m_fields->originalCommentText)->show();
	}
	void onVCTCopy(CCObject*) {
		if (!Utils::modEnabled() || !Utils::getBool("copyCommentText")) return;
		geode::utils::clipboard::write(m_fields->originalCommentText);
		Notification::create("Comment text copied!")->show();
	}
	void onVCTHide(CCObject*) {
		if (!Utils::modEnabled() || !Utils::getBool("toggleCommentVisibility") || !m_mainLayer) return;
		MyCommentCell::hideCommentVCT();
	}
	void onVCTIgnore(CCObject*) {
		if (!Utils::modEnabled() || !Utils::getBool("ignorePeople") || !m_mainLayer) return;
		if (m_fields->authorUsername == Manager::getSharedInstance()->ownUsername) return Notification::create("You can't ignore yourself!")->show();
		if (m_fields->authorAccountID == 71) return Notification::create("Nice try, but you can't ignore RobTop!")->show();
		MyCommentCell::passiveHidingComment("Comment from someone you ignored");
		MyCommentCell::hideButtons(this->m_mainLayer->getChildByID(MENU_ID));
		Utils::addIgnoredUser(m_fields->authorAccountID, m_fields->authorUsername);
		Notification::create(fmt::format("{} was ignored!", m_fields->authorUsername))->show();
	}
	void passiveHidingComment(const std::string_view reason) {
		if (!Utils::modEnabled() || !m_mainLayer) return;
		if (!m_fields->isHidden) MyCommentCell::hideCommentVCT(reason);
		else if (const auto fakeLabel = this->m_mainLayer->getChildByID(REPLACEMENT_ID)) static_cast<CCLabelBMFont*>(fakeLabel)->setString(formattedReason);
	}
	void hideCommentVCT(const std::string_view reason = "Comment was hidden manually") {
		if (!Utils::modEnabled() || !m_mainLayer) return;

		const bool isHiddenBefore = m_fields->isHidden;
		m_fields->isHidden = !m_fields->isHidden;

		const auto replacement = this->m_mainLayer->getChildByID(REPLACEMENT_ID);
		if (replacement) {
			static_cast<CCLabelBMFont*>(replacement)->setString(formattedReason);
			replacement->setVisible(m_fields->isHidden);
		}

		CCLabelBMFont* fakeLabel = !replacement ? CCLabelBMFont::create(formattedReason, "chatFont.fnt") : nullptr;
		if (fakeLabel) {
			fakeLabel->setID(REPLACEMENT_ID);
			if (Utils::getBool("blendingComments")) fakeLabel->setBlendFunc({GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA});
			this->m_mainLayer->addChild(fakeLabel);
		}

		if (this->m_height == 36) {
			const auto label = static_cast<CCLabelBMFont*>(this->getChildByIDRecursive("comment-text-label"));
			if (const auto node = this->getChildByIDRecursive("thesillydoggo.comment_emojis/comment-text-label"))
				node->setVisible(isHiddenBefore);
			else label->setVisible(isHiddenBefore);

			if (!fakeLabel) return;
			fakeLabel->setColor(label->getColor());
			fakeLabel->setPosition(label->getPosition());
			fakeLabel->setAnchorPoint(label->getAnchorPoint());
			return fakeLabel->setScale(.7f);
		}

		const auto commentTextArea = this->getChildByIDRecursive("comment-text-area");
		if (!commentTextArea) return;

		if (const auto node = this->getChildByIDRecursive("thesillydoggo.comment_emojis/comment-text-area"))
			node->setVisible(isHiddenBefore);
		else commentTextArea->setVisible(isHiddenBefore);

		if (!fakeLabel) return;

		const auto mlbmf = commentTextArea->getChildByType<MultilineBitmapFont>(0);
		if (!mlbmf) return;

		const auto cclbmf = mlbmf->getChildByType<CCLabelBMFont>(0);
		if (!cclbmf) return;

		if (const auto firstChar = cclbmf->getChildByType<CCSprite>(0)) fakeLabel->setColor(firstChar->getColor());

		fakeLabel->setPosition(commentTextArea->getPosition());
		return fakeLabel->setAnchorPoint({0.f, 1.f});
	}
	static void hideButtons(CCNode* node, const bool hideIgnoreButton = true) {
		if (!Utils::modEnabled() || !node) return;
		if (const auto hideToggler = node->getChildByID("hidden-toggler"_spr)) hideToggler->setVisible(false);
		if (const auto ignoreButton = node->getChildByID("ignore-button"_spr); hideIgnoreButton) ignoreButton->setVisible(false);
		node->updateLayout();
	}
};