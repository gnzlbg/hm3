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

- Naming convention (STL/Boost): everything in lower-case with underscores
  (`push_back`), except:
  - macros: capital with underscores prefixed with `HM3_` (`HM3_ASSERT`), and
  - type parameters/concepts: camel case (`RandomAccessIterator`).
  - use `make check-tidy` target to check the naming convetion.
  - use `make format` to correctly format your code (requires `clang-format`).
  - Document code using `///` doxygen-style comments.
- Unit tests are in the
  [`/test`](https://github.com/gnzlbg/hm3/tree/master/test) directory.
