# Stand

The ultimate trainer for Stealy V-eely Automobiley, allowing you to finally become a **[Stand User]**!

## Dependencies

- [Soup](https://github.com/calamity-inc/Soup)
- [Soup-Lua-Bindings](https://github.com/calamity-inc/Soup-Lua-Bindings)

Make sure you have a flat folder structure, such that Stand and its dependencies have the same parent folder.

## Building

To make a Debug build of the Stand project, you need to perform the following steps:

1. Build the ScriptHookV project
2. Build the Soup project in the ReleaseDLL configuration
3. Ensure you have PHP installed and its php.ini has a memory_limit of at least 2G
4. Double-click `lang/_update.bat`
5. Open a console in the repository root and run `php generate_bin.php && php generate_font_bevietnamprolight.php && php generate_font_nanumgothic.php && php generate_font_yahei.php && php xormagics.php`

After that, you can compile the Stand project to your heart's content.

## Updating

Every once in a while, Rockstar will actually bother to change this game, which will make all mod menus unusable until they are updated. In order to update Stand to a new GTA version, the following changes must be made:

- [x] **Sigs.**
- [x] **ASI Game Version.**

And for major updates:

- [x] **Crossmap.**
- [x] **Peds.**
- [x] **Vehicles.**
- [x] **Weapons.**
- [x] **Native Tables.** From decompiled scripts with up-to-date crossmap.
- [x] **Script Funcs.**
- [x] **Script Locals.**
- [x] **Script Globals.**
- [x] **Script Globals in MusinessBanager.**
- [x] **Script Events.**
- [x] **Session Scripts.**
- [x] **Asset Hashes.**

Additionally, structs may be off; [ReClassEx64](https://github.com/ajkhoury/ReClassEx) and Cheat Engine will help update them.
