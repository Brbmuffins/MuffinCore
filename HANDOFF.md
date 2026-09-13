# Handoff — 2026-09-13 (end of Claude session; Codex queue running)

## State right now

- **Live server** (`Conquest Of Azerothcore/Server`) is updated to **issue-fixes-20260912** (`worldserver.exe` `6633ba33…`), verified, and running. No new startup errors; 3 characters and 4 accounts intact.
  - Rollback: `Server/CoA-Repack-Update/Rollback_Update.bat`
  - Extra DB dump: `Server/Database-Backups/manual-pre-issue-fixes-20260913/`
- **New in repo** (uncommitted): `.gitignore`, `AUDIT_2026-09-13.md`, `AGENTS.md`, `CLAUDE.md`, `REFERENCES.md`, `HANDOFF.md`, `tools/codex-task.ps1`.
- **Archived** (moved, not deleted) to `E:\MuffinCoreRepack\_archive\2026-09-13-post-update\`: the applied 09-11 update folder and `MuffinCoreUpdate.zip`. The queue also moves the stray update files out of the `MuffinCore2.1` root.

## ⚠️ Before you commit

Read `E:\MuffinCoreRepack\codex-out\gitignore-review.result.md` (Codex's pre-commit secrets and size review).
- Result (2026-09-13): **no non-default secrets and no binaries** in the files to commit. Only known default logins (`local`/`local`, `Admin`/`Password`) and stock `.conf.dist` samples.
- Its two fixes are both handled: `*.log` was added to `.gitignore`, and the stale file list is regenerated after cleanup (`git-would-add.after-cleanup.txt`).
- Commit is OK once you've glanced at that list. The list of files git would add is `codex-out\git-would-add.after-cleanup.txt`.

## Codex job queue (runs detached)

- Script: `E:\MuffinCoreRepack\codex-out\run-queue.ps1`; progress in `codex-out\run-queue.log`; per-task event streams in `codex-out\logs\`.
- Outputs are drafts in `E:\MuffinCoreRepack\codex-out\`, outside the repo on purpose. Review them, then copy the good ones into the repo (e.g. a `docs/` folder).

| Task | Output | What |
|---|---|---|
| gitignore review | `gitignore-review.result.md` | Secrets/size check before committing |
| T2 | `T2-gm-commands.draft.md` | GM/dev command cheat sheet (audit §8 phase 1) |
| T3 | `T3-build-linux.draft.md` | Plan to build and run on Ubuntu (audit F2; prerequisite for hosting) |
| T4 | `T4-investigate-b8-b18.draft.md` | Venomancer orphaned scripts (B8) + skills stripped on login (B18), with proposed diffs (not applied) |
| — | `HOSTING_PLAN.draft.md` | Claude research workflow: update source, remote-play blockers, safe hosting architecture, licensing |
| — | `REFERENCE_EVAL.draft.md` | Claude workflow: evaluation of the shared data sources + Ascension handshake findings |
| T5 | `T5-talent-crosscheck.draft.md` | Our talent table (3,618) vs CoA-Databank (3,612) |
| T6 | `T6-reference-batch2.draft.md` | mod-classless-wildcard evaluation (audit U1) |
| T7 | `T7-next-session-brief.draft.md` | **Read this first next time**: a one-page summary of everything above |
| T8 | `T8-playerbots-options.draft.md` | Playerbots options (mod-playerbots vs NPCBots vs old Muffin Companions) and merge effort with this fork. Runs after T7 (`run-after-queue.ps1`), so T7 won't include it |

If a task's output is missing or empty, check `run-queue.log` and `logs\<task>.stderr.txt`. Re-run a single task with:
`powershell -NoProfile -File tools\codex-task.ps1 -PromptFile E:\MuffinCoreRepack\codex-out\prompts\<task>.prompt.md -OutFile E:\MuffinCoreRepack\codex-out\<task>.draft.md -EventsFile E:\MuffinCoreRepack\codex-out\logs\<task>.events.jsonl`

## Open decisions (from the audit)

1. Bug reports: disable, repoint to your own repo, or keep sending upstream (B3)?
2. Should CoA talents cost points (B4)?
3. Keep pulling upstream updates, or fork fully?
4. How friends get the client: shared privately, or bring their own?
5. Keep or discard `MuffinCore2.1` as a clean baseline?

## Suggested next session

Read `T7-next-session-brief.draft.md`, then commit (if safe), then decide on B3, then start the Linux build per T3.
