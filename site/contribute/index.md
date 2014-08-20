---
---

# How to contribute

## Git workflow

We work on the master branch but otherwise follow the standard git workflow:

1. Fork, branch, modify.
2. Commit, push.
3. Open pull request.

Worth a read: [git basics](http://try.github.io/), [git
branching](http://pcottle.github.io/learnGitBranching/), [git commit
messages](http://tbaggery.com/2008/04/19/a-note-about-git-commit-messages.html),
[git workflow](http://nvie.com/posts/a-successful-git-branching-model/).

## Development guidelines

- Stick to the STL/Boost naming convention, that is, write everything in
  lower-case with underscores (`push_back`), except:
  - macros: capital with underscores prefixed with `HOM3_` (`HOM3_ASSERT`), and
  - concepts: camel case (`RandomAccessIterator`).
- Document code using `///` doxygen-style comments.
- Indent code using `make fmt` (requires `clang-format`).
- Unit-Test your code:
  - for examples, see the `/test` directory,
  - use `make cov` to check the tests coverage.
