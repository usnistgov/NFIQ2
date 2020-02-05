# Contributing

Thanks for your interest in contributing to NFIQ 2! We'd love your help, whether it be in bug reports, documentation, or source code.

## Bug Reports

If you can reproduce a problem, please submit as much information as possible in a [**new** GitHub Issue](https://github.com/usnistgov/NFIQ2/issues). Include the version you're using and your operating system details. If you have more than one problem, please open multiple issues. This will help us better discuss the problem.

## Source Code

### Formatting

We are using [`astyle`](http://astyle.sourceforge.net) to format our code. A configuration file, `.astylerc`, is at the root of our repository. Please write code however you want, but before submitting a pull request, run it through `astyle` using the supplied configuration. You may consider adding a git hook script to do this automatically. We've supplied a sample hook script below.

<details>
	<summary><em>Expand to view an example git hook script.</em></summary>

```
#!/bin/sh

# First: place this file at .git/hooks/prepare-commit-msg
# Second: Configure your path to astyle
# git config --bool hooks.astyle.run true
# git config --path hooks.astyle.path /usr/local/bin/astyle

run_astyle=$(git config --bool hooks.astyle.run)
astyle_exe=$(git config --path hooks.astyle.path)

if [ "${run_astyle}" == "true" ]; then
	# Look for astyle binary
	echo "Formatting code with astyle..."
	if ! [ -x "${astyle_exe}" ];  then
		echo "hooks.astyle.path is not the path to astyle."
		exit 1
	fi
	
	# Determine which files changed
	changed_sources="$(git diff --cached --name-only --diff-filter=d | \
	        grep -e '.cpp$' -e '.h$' -e '.hpp$' | \
	        paste -s -d ' ' -)"
	if [ "${changed_sources}" == "" ]; then
		# No files to style
		exit 0
	fi
	
	# Style with astyle
	repo_root="$(git rev-parse --show-toplevel)"
	"${astyle_exe}" --options=${repo_root}/astyle.cfg ${changed_sources}
	if [ $? -ne 0 ]; then
		exit 1
	fi
	
	# Add style changes from all files
	git add ${changed_sources}
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