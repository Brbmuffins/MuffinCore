@AGENTS.md

# Claude Code specifics

## Before starting work
- Check `AUDIT_2026-09-13.md` (and any newer `AUDIT_*`/plan docs) for the relevant bug or feature ID, and reference IDs (B#, F#, U#) in summaries.
- Confirm which install a change targets. Default to the live one, `Conquest Of Azerothcore/Server`, and verify its `worldserver.exe` hash against the versions table.

## Offloading to Codex
Tyler wants token usage split with the local Codex CLI. Use it for bounded, self-contained tasks: pre-commit secret or size reviews, sweeps over many files, first-pass code review, mechanical edits in a named file set, test writing.
- Write a self-contained prompt file (scratchpad), then run:
  `powershell -NoProfile -File tools\codex-task.ps1 -PromptFile <prompt.md> -OutFile <result.md> [-EventsFile <events.jsonl>] [-Sandbox workspace-write]`
- Run it in the background. Reviews can take 10–25 minutes, so pass `-EventsFile` and tail it to confirm progress before assuming a hang.
- The default sandbox is `read-only`. Use `workspace-write` only for edits Tyler has already approved, and review the diff before reporting.
- Treat Codex output as a draft: verify its decision-critical claims before acting on them.
- Don't give Codex work that stops or starts services, touches live databases, or needs conversation context it doesn't have.

## Deliverables
- Plans and audits go in Markdown at the repo root (or `docs/` once it exists) so they're committed and reviewable in GitHub Desktop.
- After making changes, list the files touched and why. Tyler commits them himself.
