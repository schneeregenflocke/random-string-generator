# AGENTS.md

## Clang-Tidy

```
clang-tidy -p build src/*.cpp src/*.hpp 2>&1 | tee build/clang-tidy.log
```

```
clang-tidy --verify-config  
```
