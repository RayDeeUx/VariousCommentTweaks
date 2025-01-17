#include <Geode/modify/CommentCell.hpp>
#include "TranslateMenu.hpp"
#include "Manager.hpp"
#include "Utils.hpp"

#define MENU_ID "the-menu-with-buttons-that-im-too-lazy-to-move-into-main-menu-sorry"_spr
#define REPLACEMENT_ID "hidden-comment-replacement-label"_spr
#define FAVORITE_USER_HIGHLIGHT "favorite-user-highlight"_spr
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
		int originalLikeCount;
	};
	void loadFromComment(GJComment* comment) {
		const auto fields = m_fields.self();
		fields->originalCommentText = comment->m_commentString; // store comment text early before other mods edit the member variable
		fields->originalLikeCount = comment->m_likeCount;

		CommentCell::loadFromComment(comment);
		fields->authorAccountID = comment->m_accountID;
		fields->authorUsername = comment->m_userName;

		if (!Utils::modEnabled() || comment->m_commentDeleted || !m_mainLayer) return;

		const auto manager = Manager::getSharedInstance();

		const auto dateLabel = this->getChildByIDRecursive("date-label");
		const auto commentTextLabel = this->getChildByIDRecursive("comment-text-label");
		if (m_comment->m_isSpam || !dateLabel) return;

		if (Utils::getBool("nameInButton")) MyCommentCell::replaceButtonWithName(comment);
		if (Utils::getBool("blendingComments")) MyCommentCell::applyBlendingToComment();

		const bool isLargeComment = this->m_height != 36;
		const bool isOwnComment = comment->m_userName == Manager::getSharedInstance()->ownUsername;
		const float spriteScale = isLargeComment ? 1.1f : 1.2f;
		const float menuContentWidth = !isLargeComment ? 35.f : 45.f;

		CCSprite* copyButtonSprite = CCSprite::createWithSpriteFrameName("copy.png"_spr);
		CCMenuItemSpriteExtra* copyStuffButton = CCMenuItemSpriteExtra::create(copyButtonSprite, this, menu_selector(MyCommentCell::onVCTCopy));
		copyStuffButton->setID("copy-button"_spr);

		CCSprite* markAsHiddenSprite = CCSprite::createWithSpriteFrameName("show.png"_spr);
		markAsHiddenSprite->setID("mark-as-hidden"_spr);
		CCSprite* markAsVisibleSprite = CCSprite::createWithSpriteFrameName("hide.png"_spr);
		markAsVisibleSprite->setID("mark-as-visible"_spr);
		CCMenuItemToggler* markAsHiddenToggler = CCMenuItemToggler::create(markAsHiddenSprite, markAsVisibleSprite, this, menu_selector(MyCommentCell::onVCTHide));
		markAsHiddenToggler->setID("hidden-toggler"_spr);

		CCSprite* ignoreButtonSprite = CCSprite::createWithSpriteFrameName("ignore.png"_spr);
		CCMenuItemSpriteExtra* ignoreButton = CCMenuItemSpriteExtra::create(ignoreButtonSprite, this, menu_selector(MyCommentCell::onVCTIgnore));
		ignoreButton->setID("ignore-button"_spr);

		CCSprite* favoriteButtonSprite = CCSprite::createWithSpriteFrameName("favorite.png"_spr);
		CCMenuItemSpriteExtra* favoriteButton = CCMenuItemSpriteExtra::create(favoriteButtonSprite, this, menu_selector(MyCommentCell::onVCTFavorite));
		favoriteButton->setID("favorite-button"_spr);

		CCSprite* translateButtonSprite = CCSprite::createWithSpriteFrameName("translate.png"_spr);
		CCMenuItemSpriteExtra* translateButton = CCMenuItemSpriteExtra::create(translateButtonSprite, this, menu_selector(MyCommentCell::onVCTTrans));
		translateButton->setID("translate-button"_spr);

		const float dateLabelScale = dateLabel->getScale();

		CCMenu* menu = CCMenu::create();
		if (Utils::getBool("copyCommentText")) menu->addChild(copyStuffButton);
		if (Utils::getBool("toggleCommentVisibility")) menu->addChild(markAsHiddenToggler);
		if (!isOwnComment) {
			if (Utils::getBool("ignorePeople")) menu->addChild(ignoreButton);
			if (Utils::getBool("favoriteUsers")) menu->addChild(favoriteButton);
		}
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

		menu->setTag(0);

		if (Utils::getBool("favoriteUsers") && Utils::contains<int>(manager->favoriteUsers, fields->authorAccountID)) {
			menu->setTag(1);
			MyCommentCell::recolorCellBackground();
			MyCommentCell::hideButtons(menu);
			menu->setTag(0);
		} else if (Utils::getBool("ignorePeople") && Utils::contains<int>(manager->ignoredUsers, fields->authorAccountID)) {
			MyCommentCell::passiveHidingComment("Comment from someone you ignored");
			MyCommentCell::hideButtons(menu);
		} else if (Utils::getBool("personalFilter") && manager->ownUsername != comment->m_userName && string::containsAny(string::toLower(fields->originalCommentText), manager->dislikedWords)) {
			MyCommentCell::passiveHidingComment("Comment contains word(s) you dislike");
			MyCommentCell::hideButtons(menu, false);
		} else if (Utils::getBool("hideRoleplay")) { // roleplay filter (inaccurate?)
			int numAsterisks = 0;
			int numVerbs = 0;
			for (const std::string& word : string::split(string::toLower(fields->originalCommentText), " ")) {
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

		MyCommentCell::likeBaitExterminator(comment, isOwnComment);

		if (isLargeComment || !commentTextLabel || (!isLargeComment && fields->originalCommentText.length() < 31)) return;
		if (menu->getChildrenCount() < 1) return menu->removeMeAndCleanup();

		const float scale = commentTextLabel->getScale();
		if (scale <= 0.0f) return;

		const float commentWidth = commentTextLabel->getContentWidth();
		if (commentWidth <= 0.0f) return;

		const float widthDiff = commentWidth - (menuContentWidth * menu->getScale());
		if (widthDiff <= 0.0f) return;

		commentTextLabel->setScale(scale * (widthDiff / commentWidth) * .95f);
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
		TranslateMenu* translateMenu = TranslateMenu::create(m_fields->originalCommentText);
		if (!Utils::isModLoaded("timestepyt.deltarune_textboxes") && !CCKeyboardDispatcher::get()->getShiftKeyPressed())
			return translateMenu->show();
		TranslateMenu::encodeToURL(m_fields->originalCommentText);
		const std::string& preferred = Utils::getString("preferredTranslator");
		if (preferred == "Google") return translateMenu->onBoogleTranslate(nullptr);
		if (preferred == "DeepL") return translateMenu->onDeepLTranslate(nullptr);
		if (preferred == "LibreTranslate") return translateMenu->onLibreTranslate(nullptr);
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
		if (!Utils::addIgnoredUser(m_fields->authorAccountID, m_fields->authorUsername)) return;
		MyCommentCell::passiveHidingComment("Comment from someone you ignored");
		MyCommentCell::hideButtons(this->m_mainLayer->getChildByID(MENU_ID));
		Notification::create(fmt::format("{} was ignored!", m_fields->authorUsername))->show();
	}
	void onVCTFavorite(CCObject*) {
		if (!Utils::modEnabled() || !Utils::getBool("favoriteUsers") || !m_mainLayer) return;
		if (m_fields->authorUsername == Manager::getSharedInstance()->ownUsername) return Notification::create("You can't favorite yourself!")->show();
		if (!Utils::addFavoriteUser(m_fields->authorAccountID, m_fields->authorUsername)) return;
		const auto menu = this->m_mainLayer->getChildByID(MENU_ID);
		menu->setTag(1);
		MyCommentCell::recolorCellBackground();
		MyCommentCell::hideButtons(menu);
		menu->setTag(0);
		Notification::create(fmt::format("{} is now a favorite user!", m_fields->authorUsername))->show();
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

		if (const auto highlight = this->m_mainLayer->getChildByID(FAVORITE_USER_HIGHLIGHT))
			highlight->setVisible(isHiddenBefore);

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
	static void hideButtons(CCNode* node, const bool hideIgnoreAndFavoriteButtons = true) {
		if (!Utils::modEnabled() || !node) return;
		if (Utils::getBool("toggleCommentVisibility")) {
			if (CCNode* hideToggler = node->getChildByIDRecursive("hidden-toggler"_spr); node->getTag() != 1) {
				hideToggler->setVisible(false);
			}
		}
		if (Utils::getBool("favoriteUsers")) {
			if (CCNode* ignoreButton = node->getChildByIDRecursive("ignore-button"_spr); hideIgnoreAndFavoriteButtons) {
				ignoreButton->setVisible(false);
			}
		}
		if (Utils::getBool("ignorePeople")) {
			if (CCNode *favoriteButton = node->getChildByIDRecursive("favorite-button"_spr); hideIgnoreAndFavoriteButtons) {
				favoriteButton->setVisible(false);
			}
		}
		node->updateLayout();
	}
	void recolorCellBackground() {
		if (!Utils::modEnabled() || !Utils::getBool("favoriteUsers") || !this->m_mainLayer || this->m_mainLayer->getChildByID(FAVORITE_USER_HIGHLIGHT) || Manager::getSharedInstance()->doNotHighlight) return;
		if (Utils::getBool("gradientHighlight")) {
			const bool isLargeComment = this->m_height != 36;
			CCSprite* gradientHighlight = isLargeComment ? CCSprite::createWithSpriteFrameName("largeHighlight.png"_spr) : CCSprite::createWithSpriteFrameName("tinyHighlight.png"_spr);
			auto [r, g, b, a] = Utils::getColorAlpha("favoriteUserColor");
			gradientHighlight->setColor({r, g, b});
			gradientHighlight->setOpacity(a);
			gradientHighlight->setAnchorPoint({0, 0});
			gradientHighlight->setBlendFunc({GL_ONE, GL_ONE_MINUS_CONSTANT_ALPHA});
			gradientHighlight->setScale(1.675f);
			gradientHighlight->setID(FAVORITE_USER_HIGHLIGHT);
			this->m_mainLayer->addChild(gradientHighlight);
			gradientHighlight->setZOrder(-10);
			return gradientHighlight->setVisible(!m_fields->isHidden);
		}
		CCLayerColor* highlight = CCLayerColor::create(Utils::getColorAlpha("favoriteUserColor"));
		highlight->setContentSize({340, this->m_height});
		highlight->setID(FAVORITE_USER_HIGHLIGHT);
		this->m_mainLayer->addChild(highlight);
		highlight->setZOrder(-10);
		return highlight->setVisible(!m_fields->isHidden);
	}
	void likeBaitExterminator(GJComment* comment, const bool isOwnComment) {
		const auto fields = m_fields.self();
		if (!Utils::isModLoaded("raydeeux.likebaitexterminator")) return;
		const auto lbe = Utils::getMod("raydeeux.likebaitexterminator");
		if (!lbe->getSettingValue<bool>("enabled")) return;
		comment->m_likeCount = fields->originalLikeCount;
		if (!isOwnComment || comment->m_commentString != lbe->getSettingValue<std::string>("replacementText")) return;
		comment->m_commentString = fields->originalCommentText;
		MyCommentCell::passiveHidingComment("Comment is probably(?) likebait");
	}
};