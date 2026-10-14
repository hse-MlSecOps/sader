# SADER — AI Agent Rules

## Project

SADER is a C++20 runtime for discovering, describing and executing registered Workers.

The current repository is the source of truth.

## Mandatory rules

1. Inspect relevant source files before making claims.
2. Preserve exact C++ identifiers and CLI syntax.
3. Distinguish IMPLEMENTED, TESTED, PLANNED and UNKNOWN.
4. Do not claim tests passed unless they were actually executed.
5. Never modify production code without explicit approval.
6. Never perform Git commit, push, reset, clean or rebase automatically.
7. Do not access or expose secrets.
8. Keep documentation in Russian, preserving technical identifiers in English.
9. Record assumptions and unresolved questions explicitly.

## Architecture

Inspect the current implementation of:

- Worker and concrete Worker implementations
- WorkerRegistry
- WorkerFactory
- Parser and Command
- Executor
- CMake and test targets

Do not assume ThreadPool is already implemented.

## Collaboration

The team is preparing parallel development:

- ThreadPool and asynchronous Worker execution
- Docker packaging
- GitHub Actions CI

Do not change public contracts while these workstreams are being planned.

## Quality gates

Every generated document must:

- Reflect the current repository
- Link to relevant source files where practical
- Clearly separate existing functionality from future plans
- Include unresolved technical questions
- Avoid invented APIs or test results