# VariousCommentTweaks Changelog
## v1.2.0
- Added an option to *truly* hide levels/level lists from ignored people (requested by [Nightcat](https://github.com/Nightcaat)).
- Fixed a potential bug where requesting the friends list and blocked users list in GD at the same time would fail.
- Added logging options, because why not.
## v1.1.4
- Experimental iOS support.
- Added option to rescale text from large comments, <cl>suggested by [CherryChip](https://youtube.com/channel/UCEk7PhJTjabhGsov0l9JUzg)</c>.
## v1.1.3
- Attempt to scale comments from Emojis in Comments to avoid overlap.
## v1.1.2
- The "Detect language" option from the `Translation Source Language` mod setting will be treated as "translate from Spanish", *just* so DeepL will properly read a comment's text from the URL.
  - DeepL's devs are a bit... "special", so this was my most reasonable solution. Thanks to [CherryChip](https://youtube.com/channel/UCEk7PhJTjabhGsov0l9JUzg) for the reminder!
- Minor futureproofing with hiding comments.
  - Semi-tempted to hardcode the futureproofing with regex. But I digress.
- Fix a bug where you could add yourself as a favorite person.
## v1.1.1
- Fix another batch of crashes. Thanks to AlphaQuata for the report!
## v1.1.0
- New button sprites and gradient designs by [Brift](https://twitter.com/BriftXD)!
- New mod logo based on new button sprites!
- Change highlighting behavior to use gradients instead of recoloring the background.
  - A toggle has been added in case you prefer the old behavior.
- Added option to obfuscate ignored users when viewing comments on a level or level list. Enabled by default.
- Make controller glyphs in the Translation menu invisible. Thanks to [CherryChip](https://youtube.com/channel/UCEk7PhJTjabhGsov0l9JUzg) for reporting!
## v1.0.4
- Fix one unexpected crash.
- Removed Mr. Bean from the premises.
## v1.0.3
- Add <cl>favoriting users</c> as a feature, <cl>suggested by [JollySpider](https://www.youtube.com/@JollySpider/)</c>.
- Fix a few potential crashes. Apparently CommentCells are a pain to deal with.
## v1.0.2
- Refactor how ignored users are fetched to address potential memory/lag concerns.
## v1.0.1
- Added a shortcut to refresh ignored users/filtered phrases.
- Rename a few config settings to color more words and make it clearer on how to adjust ignored users/filtered phrases.
- Added `Better Comments Section` by Jumbonova as a <co>suggested</c> dependency for additional filters.
## v1.0.0
- Initial release (on GitHub).