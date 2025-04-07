#!/bin/bash
# Deploy script for documentation pages
# Will host built documentation on https://documentation.beamng.com/api/matlab-simulink
set -eo pipefail

echo $DOCUMENTATION_REPOSITORY

# Set up SSH
mkdir -p ~/.ssh
chmod 700 ~/.ssh
echo "$DOCUMENTATION_SSH_PRIVATE_KEY" | base64 --decode > ~/.ssh/id_ed25519
chmod 600 ~/.ssh/id_ed25519
ls -al ~/.ssh

REPO_HOST=${DOCUMENTATION_REPOSITORY#*@}
REPO_HOST=${REPO_HOST%:*}
ssh-keyscan -t ed25519 $REPO_HOST >> ~/.ssh/known_hosts

# Clone repository
mkdir -p $DOCUMENTATION_LOCAL_REPO
git clone -b $DOCUMENTATION_BRANCH $DOCUMENTATION_REPOSITORY $DOCUMENTATION_LOCAL_REPO

# Git author config
git config --global user.name "$DOCUMENTATION_GIT_USERNAME"
git config --global user.email "$DOCUMENTATION_GIT_EMAIL"

# Copy artifacts
DEST_PATH="$PWD/$DOCUMENTATION_LOCAL_REPO/$DOCUMENTATION_TARGET_PATH"
rm -rf "$DEST_PATH"
mkdir -p "$(dirname "$DEST_PATH")"
cp -r "$DOCUMENTATION_ARTIFACTS_PATH" "$DEST_PATH"

# Commit and push
cd "$DOCUMENTATION_LOCAL_REPO"
git add --all
git commit -m "Update $DOCUMENTATION_TARGET_PATH (generated by $CI_PROJECT_NAME CI)"
git push
