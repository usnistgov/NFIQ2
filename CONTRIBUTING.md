# Contributing

Thanks for your interest in contributing to NFIQ 2! We'd love your help, whether it be in bug reports, documentation, or source code.

## Bug Reports

If you can reproduce a problem, please submit as much information as possible in a [**new** GitHub Issue](https://github.com/usnistgov/NFIQ2/issues). Include the version you're using and your operating system details. If you have more than one problem, please open multiple issues. This will help us better discuss the problem.

## Source Code

### Formatting

We are using [`clang-format`](https://clang.llvm.org/docs/ClangFormat.html) to format our code. A configuration file, `.clang-format`, is at the root of our repository. Please write code however you want, but before submitting a pull request, run it through `clang-format` using the supplied configuration. You may consider adding a git hook script to do this automatically. We've supplied a sample hook script below.

<details>
	<summary><em>Expand to view an example git hook script.</em></summary>

```sh
#!/bin/sh

# 1. Place this file at .git/hooks/prepare-commit-msg
# 2. Make that hook file executable
#    chmod u+x .git/hooks/prepare-commit-msg
# 3: Configure your path to clang-format
#    git config --bool hooks.formatter.run true
#    git config --path hooks.formatter.path /usr/local/bin/clang-format

run_formatter=$(git config --bool hooks.formatter.run)
formatter_exe=$(git config --path hooks.formatter.path)

if [ "${run_formatter}" = "true" ]; then
    # Look for clang-format binary
    echo "Formatting code with clang-format..."
    if ! [ -x "${formatter_exe}" ];  then
        echo "hooks.formatter.path is not the path to clang-format."
        exit 1
    fi

    # Determine which files changed
    changed_sources="$(git diff --cached --name-only --diff-filter=d | \
            grep -e '.cpp$' -e '.h$' -e '.hpp$' | \
            paste -s -d ';' -)"
    if [ "${changed_sources}" = "" ]; then
        # No files to style
        exit 0
    fi

    old_ifs="${IFS}"
    IFS=';'
    export IFS
    for file in ${changed_sources}; do
        # Style with clang-format
    	"${formatter_exe}" -i "${file}"

        # Add style change
    	git add "${file}"
    done

    # Sometimes, clang-format gets really confused
    for file in ${changed_sources}; do
        if ! "${formatter_exe}" --Werror --dry-run "${file}"; then
            echo "You confused clang-format. Please fix the above error."
            exit 1
        fi
    done
    IFS="${old_ifs}"
    export IFS
else
    echo "Formatting pass skipped."
fi
```
</details>

### Number of Changes

Please limit pull requests to a **single** fundamental change. This makes it easier to track changes and review.

### Continuous Integration Builds

NFIQ needs to run on many platforms, so we use continuous integration to make sure code changes continue to build and score images identically. If your code change breaks the build or alters values, we will need you to make the necessary corrections before it can be merged. We would suggest you allow maintainers to edit your pull request, as we may be able to make the changes for you if they're trivial enough.

### License

All code is released into the public domain. See the [LICENSE](https://github.com/usnistgov/NFIQ/blob/master/LICENSE.md) for more details.
