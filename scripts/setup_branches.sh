#!/usr/bin/env bash
# Создаёт стандартный набор веток. Запускать в свежеинициализированном репозитории.
set -e
git init -b main
git add .
git commit -m "init: project scaffold"

for b in develop feature/core-algorithms feature/server feature/client feature/docker feature/tests; do
    git branch "$b"
done
git checkout develop
echo "Branches:"
git branch
