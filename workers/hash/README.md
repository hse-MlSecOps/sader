# HashWorker Threading Update

This update adds multithreaded hashing to `HashWorker`.

The main code change is in:

```text
workers/hash/HashWorker.cpp
```

No CMake or build preset changes are required for this update.

## What Changed

`HashWorker` now supports a third algorithm mode:

```text
both
```

Before this update, the worker supported:

- `md5`
- `sha256`

Now it supports:

- `md5`
- `sha256`
- `both`

## Why Threading Was Added

When a caller needs both MD5 and SHA-256 for the same input, calculating them
one after another wastes time because the two hash algorithms are independent.

The new `algorithm=both` mode calculates them concurrently:

- one `std::thread` calculates MD5;
- another `std::thread` calculates SHA-256.

Both threads read the same immutable byte buffer and write their results into
separate strings. After both threads finish, `hashBytes()` joins them and
returns both digests in one structured result.

## Example Calls

Hash text with both algorithms:

```text
CALL hash algorithm=both text=abc
```

Hash file with both algorithms:

```text
CALL hash algorithm=both path=data.txt
```

The existing single-algorithm calls still work:

```text
CALL hash algorithm=md5 text=abc
CALL hash algorithm=sha256 text=abc
```

## Result Format

For `algorithm=both`, the output contains two hashes:

```text
Algorithm: both
Input: text
MD5: 900150983cd24fb0d6963f7d28e17f72
SHA-256: ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
```

For `algorithm=md5` or `algorithm=sha256`, the previous format is preserved:

```text
Algorithm: md5
Input: text
Digest: 900150983cd24fb0d6963f7d28e17f72
```

## Implementation Details

The threading logic is inside `HashWorker::hashBytes()`.

For `algorithm=both`, it creates two local result strings:

```cpp
std::string md5Digest;
std::string sha256Digest;
```

Then it starts two threads:

```cpp
std::thread md5Thread(...);
std::thread sha256Thread(...);
```

The first thread calls:

```cpp
md5(data);
```

The second thread calls:

```cpp
sha256(data);
```

After that, both threads are joined:

```cpp
md5Thread.join();
sha256Thread.join();
```

This makes sure both hashes are finished before the final `Result` is returned.

## Thread Safety

The implementation is safe because:

- the input byte vector is only read by both threads;
- MD5 and SHA-256 write to different local strings;
- the final output is created only after both `join()` calls;
- no shared mutable worker state is used.

## Validation

The existing validation remains in place:

- `algorithm` is required;
- only `md5`, `sha256` and `both` are supported;
- exactly one input source must be provided: `text` or `path`;
- file input is checked before hashing.

## Commit Scope

The intended commit scope is small:

- update `workers/hash/HashWorker.cpp`;
- add this README;
- do not include CMake preset or build-system changes.
