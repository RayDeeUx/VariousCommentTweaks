#include <Geode/modify/ShareCommentLayer.hpp>
#include "Utils.hpp"

using namespace geode::prelude;

class $modify(MyShareCommentLayer, ShareCommentLayer) {
    bool init(gd::string title, int charLimit, CommentType type, int ID, gd::string desc) {
        if (!ShareCommentLayer::init(title, charLimit, type, ID, desc)) return false;
        if (!Utils::modEnabled() || !Utils::getBool("copyCommentText") || !this->m_mainLayer || type == CommentType::FriendRequest) return true; // i could block ListDescription as well, but i'm being nice this time

        CircleButtonSprite* pasteSprite = CircleButtonSprite::createWithSpriteFrameName("paste.png"_spr, 1.f, CircleBaseColor::Green, CircleBaseSize::Tiny);
        CCMenuItemSpriteExtra* pasteButton = CCMenuItemSpriteExtra::create(pasteSprite, this, menu_selector(MyShareCommentLayer::onVCTPaste));
        pasteButton->setID("paste-shortcut-button"_spr);

        CCMenu* menu = CCMenu::create();
        AxisLayout* layout = ColumnLayout::create()->setDefaultScaleLimits(1.f, 1.f)->setGap(2.f);
        layout->ignoreInvisibleChildren(true);

        menu->setContentSize({26.5f, 29.f});
        menu->setPosition({200 + menu->getPositionX(), 247.f});
        menu->addChild(pasteButton);
        menu->setLayout(layout);
        menu->setID("paste-shortcut-menu"_spr);

        this->m_mainLayer->addChild(menu);
        return true;
    }

    void onVCTPaste(CCObject*) {
        if (!Utils::modEnabled() || !Utils::getBool("copyCommentText") || !this->m_commentInput) return;
        this->m_commentInput->setString(this->m_commentInput->getString().append(utils::clipboard::read()));
    }
};