---
name: accessibility-mod-practices
description: Apply proven screen-reader accessibility-mod patterns when designing, implementing, reviewing, or documenting game accessibility features.
---

# Accessibility Mod Practices

Use this skill when building or reviewing a screen-reader accessibility mod, speech layer, accessible menu system, review cursor, spatial-audio cue system, or player-facing accessibility documentation.

Treat these practices as starting hypotheses, not universal law. Verify them against the current game's own UI model, controls, update cadence, and player workflows.

## Model the game, not the pixels

- Prefer the game's own objects, events, focus state, and data APIs over OCR, drawn-text scraping, screen coordinates, or synthesized clicks.
- Model important screens one at a time. For each screen, identify its controls in player-sensible order, their labels, values, states, positions, and activation paths.
- Read the screen on top of the stack. Announce its title or context and the initially focused control when it opens.
- Give every focus announcement the information needed to act: label, value, state, and position such as "3 of 8" where that helps orientation.
- Keep detailed tooltips or descriptions available on demand instead of forcing the player to hear everything on every focus move.
- Use an honest unsupported-screen fallback. Say what will not be read instead of guessing from pixels or stale state.

## Own focus and keyboard routing

- Decide keyboard ownership from the current screen or mode, not from a player-managed "focus mode."
- When a modelled screen owns keys, do not let the same keys also drive the underlying game.
- When the game must receive text or movement input, pass keys through deliberately.
- Preserve screen-reader modifier keys and operating-system shortcuts. Do not throw the player out of their chosen input mode merely because they used a reader command.
- Provide deterministic navigation: next/previous, first/last, grouping or section jumps, and repeat-last or detailed-info commands where the screen warrants them.
- Re-resolve controls and entities at use time. Do not retain UI-node or entity pointers across frames when the game rebuilds them.

## Speak with restraint

- Distinguish immediate feedback from queued announcements. Interrupt for focus movement and direct responses to the player's input; queue or coalesce ambient events.
- Deduplicate and rate-limit repeated announcements. Gate event speech by game state so stale, duplicated, or off-screen events do not talk over play.
- Prefer semantic state transitions and game events over polling arbitrary visual changes.
- Offer verbosity choices where information density varies: for example, detailed combat events, periodic summaries, or alerts only.
- Announce sighted-glance information when it changes materially: health or resources crossing thresholds, charges, objectives, discovery, victory or defeat, connection or microphone failures.
- Never rely only on speech when timing is continuous. Pair it with short sounds whose meaning can be learned and previewed.

## Make the world inspectable

- Provide a review mechanism that can step through meaningful groups: enemies, interactables, loot, exits, objectives, party members, or points of interest.
- Support nearest, next, and previous review; report direction and distance in a consistent frame.
- Give important object classes distinct, repeatable cues. Encode distance through loudness, rate, pitch, or pan consistently, and cap how many objects can sound at once.
- Provide directional obstruction information when movement depends on it, such as wall tones or a short radar probe.
- Stop world cues during conversations, cutscenes, menus, loading, or when the game is unfocused unless their information remains relevant.
- Let players inspect and act at a review position without requiring pixel-perfect aiming when the game's own targeting model permits it.

## Separate reusable access code from game hooks

- Put speech output behind one small wrapper. It should expose availability, normal versus interrupting speech, silence, repeat-last-message, clean shutdown, and useful logging.
- Let the wrapper recover if the screen reader starts after the game or its backend is reacquired. Do not scatter native screen-reader calls through game patches.
- Keep reusable navigation, announcement gating, cue playback, and configuration separate from game-specific readers and hooks.
- Name game-specific hook modules after the screen, event, or system they adapt. A patch should translate one game occurrence into one semantic announcement or state update.
- Extract ordering, geometry, text composition, threshold, deduplication, and cue-selection logic into testable units where the engine allows it.
- Pin support to verified game builds when using private layouts or signatures. A version check plus an honest refusal is safer than reading the wrong objects after a patch.
- Log spoken lines and major state transitions in a form a blind tester or support volunteer can inspect after the fact.

## Make controls and settings discoverable

- Build around the input method the game actually supports well, then add only the mod shortcuts that input method lacks.
- Document every player-facing key in the primary player documentation in the same change that adds or rebinds it.
- Group controls by menu, inventory, world review, combat, and global speech functions rather than presenting one undifferentiated list.
- Offer per-channel controls where players plausibly differ: speech interruption or verbosity, cue volume, threshold alerts, maximum simultaneous object cues, and optional automatic announcements.
- Provide cue previews. A sound's name rarely teaches its meaning as reliably as hearing it outside combat.
- Keep defaults conservative: enough information to orient a new player without drowning urgent events in ambient detail.

## Verify with the whole play surface

- Test with the intended screen reader and with the display ignored or covered. Sighted inspection can supplement this, not replace it.
- Cover first launch, empty lists, disabled controls, modal dialogs, screen-stack return, settings persistence, loading, death or defeat, disconnects, and loss of window focus.
- Check that opening each supported screen announces context once, focus moves speak once, and returning restores a sensible focus.
- Check repeated events, rapid combat, threshold crossings, and many nearby objects for speech and cue spam.
- Verify installation, update, uninstall, missing dependencies, and an unsupported game version. The failure message should identify the next action.
- Mark by-ear tuning as unverified until a blind player has actually heard it in play. A log line proves dispatch, not audibility or intelligibility.

## Document for players, not only developers

- State requirements up front: screen reader, controller or keyboard expectations, first-launch downloads, operating system, and game version.
- Document what the mod reads, what it sounds, what remains unsupported, and whether it plays any part of the game automatically.
- Include controls, settings, known issues, update steps, uninstall steps, and a short troubleshooting path based on observable log lines.
- Warn about online, anti-cheat, or ban risk where the integration method could create it. Do not imply safety that has not been established.
- Ship an easy full install for new players and, when useful, a smaller update path for people who already have the loader.

## Source patterns to inspect

These informed the distilled rules above. Inspect the nearest comparable implementation before copying a pattern into a new game:

- [LeAccess](https://github.com/Ali-Bueno/LeAccess) for controller-first menus, world sounds, threshold announcements, and player documentation.
- [Diablo IV Assist](https://github.com/Ali-Bueno/d4-assist) for directional radar, object sounds, quest guidance, silence during non-gameplay states, and explicit limits.
- [Mimic Party Access](https://github.com/Ali-Bueno/mimic-party-access) for a reusable AccessKit, Prism wrapper, deterministic menu navigation, announcement modules, and focused tests.
- [Core Keeper Access](https://github.com/Ali-Bueno/CkAccess) for virtual-cursor world review, inventory workflows, notifications, and proximity sounds.
- [Bazaar Access](https://github.com/Ali-Bueno/bazaar-access) for combat verbosity modes, quick stats, end-of-match announcements, and full versus update-only packages.
- [DRG Survivor Access](https://github.com/Ali-Bueno/drg-access) for wall tones, cue previews, and concise setup documentation.
