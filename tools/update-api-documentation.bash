#!/bin/bash

set -e

if [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
    echo -e \
	 "Usage: $0\n" \
	 "\n" \
         "This scripts creates doxygen API documentation based on the given\n" \
	 "branch. It then creates or updates the branch\n\n" \
	 "    /gh-pages\n\n" \
	 "accordingly.\n" \
	 "\n" \
	 "This script has to be executed in the root directory of a Git\n" \
	 "repository clone." >&2
    exit 1
fi

if [ ! -d ".git" ] ; then
    echo -e \
	 "This script has to be executed in the root directory of a Git\n" \
	 "repository clone." >&2
    exit 1
fi

git fetch
git config --global user.email "php-agent@newrelic.com"
git config --global user.name "Doc Generator"

# The base branch the documentation is built from
branch="$1"

logfile=$(basename "$0").log
exec > "$logfile"

# The branch which will hold the final state that can be synced to the public
# C Telemetry SDK repository.
#
release_branch="gh-pages"

# Make sure we end up on the original branch when the script exits. Clean up
# any pending doxygen output.
#
do_cleanup() {
    rm -rf html/
    git checkout "$branch" > /dev/null 2>&1
}
trap do_cleanup EXIT

# Step 1
# ======
#
# Check out the base branch and run doxygen.
#
git checkout "$branch"
cd _doxygen
doxygen
cd ../

# Step 2
# ======
# Check out the release branch. Create an empty release branch if it doesn't
# exist.
#
if ! git rev-parse origin/$release_branch ; then
    git checkout --orphan $release_branch
    git commit --allow-empty -m "Initialize documentation"
else
    git checkout $release_branch
    git reset --hard origin/$release_branch
fi

# Step 3
# ======
# Remove all known files and replace them with the new doxygen output.
#
git rm -f $(git ls-files)
git add $(find html)
git mv html/* .
git commit --allow-empty -m "Update documentation"
git push origin "$release_branch"
