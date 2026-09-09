# Repository Guidelines

## Project Structure & Module Organization

This repository contains a small macOS launcher written in C:

- `main.c` contains the launcher logic, including proxy environment setup and `posix_spawn` of ChatGPT.
- `Makefile` defines the compiler settings and build targets.
- `ProxyGPT.app/Contents/Info.plist` defines the application bundle metadata.
- `ProxyGPT.app/Contents/MacOS/launcher` is the generated executable and is ignored by Git.
- `ProxyGPT.app/Contents/Resources/` holds bundle resources, if needed in the future.

Keep platform-specific behavior in `main.c` and preserve the bundle layout expected by macOS.

## Build, Test, and Development Commands

Run these commands from the repository root on macOS with Xcode Command Line Tools installed:

- `make` — compile `main.c` with `clang` into the app bundle.
- `make -B` — force a rebuild after changing launcher constants or build inputs.
- `make clean` — remove the generated launcher binary.
- `open ProxyGPT.app` — launch the built application for manual verification.
- `codesign --force --sign - ProxyGPT.app` — apply an ad-hoc signature for local use when macOS requires it.

There is currently no automated test suite. Manual checks should verify that ChatGPT starts, receives the intended proxy environment, forwards command-line arguments, and that the launcher exits immediately.

## Coding Style & Naming Conventions

Follow the existing C style: two-space indentation, braces on the same line,  camelCase for local variables and functions, and uppercase names for configuration constants. Compile warning-clean with `-Wall -Wextra -Wpedantic -O2`. Prefer standard/POSIX APIs already used by the project and check failures explicitly. Keep comments focused on non-obvious process or macOS behavior.

## Testing Guidelines

For source changes, run `make clean && make`, then launch with `open ProxyGPT.app`. Test proxy and ChatGPT path changes on a system matching the documented configuration, and do not commit generated binaries or `.DS_Store` files.

## Commit & Pull Request Guidelines

No commit history exists yet, so no established commit format can be inferred. Use short, imperative commit subjects (for example, `Handle missing ChatGPT executable`). Pull requests should explain the behavior change, include the manual verification commands and results, and mention any changes to paths, proxy settings, bundle metadata, or signing requirements. Include screenshots only for visible bundle or Finder behavior changes.
