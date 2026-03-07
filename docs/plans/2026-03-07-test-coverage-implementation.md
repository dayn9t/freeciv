# Implementation Plan: Test Coverage 85% Target

## Overview

**Goal:** Improve test coverage from 24.7% to 85%

**Strategy:** Layered progressive approach - start with simple modules, progressively enable mock framework for complex modules.

**Timeline:** 6-8 weeks

---

## Current State Analysis

| Metric | Current | Target |
|--------|---------|--------|
| Line Coverage | 24.7% | 85% |
| Function Coverage | 42.1% | 90% |
| Test Count | 67 test files | 150+ |

### Test Infrastructure Status

| Component | Status | Location |
|-----------|--------|----------|
| CMocka Integration | Complete | `meson.build` |
| Mock Framework | Exists, disabled | `tests/unit/mock/` |
| Test Files | 67 files | `tests/unit/common/` |

---

## Phase 1: Quick Wins (Target: 45%, Week 1-2)

**Focus:** Extend existing tests for simple modules

### Task 1.1: Enhance unitlist.c Tests
- **Source:** `common/unitlist.c` (364 lines)
- **Current Coverage:** 0%
- **Target Coverage:** 80%
- **Effort:** 4 hours

### Task 1.2: Enhance clientutils.c Tests
- **Source:** `common/clientutils.c` (359 lines)
- **Current Coverage:** 0%
- **Target Coverage:** 80%
- **Effort:** 6 hours

### Task 1.3: Enhance victory.c Tests
- **Source:** `common/victory.c` (30 lines)
- **Current Coverage:** 50%
- **Target Coverage:** 85%
- **Effort:** 2 hours

### Task 1.4: Enhance workertask.c Tests
- **Source:** `common/workertask.c` (68 lines)
- **Current Coverage:** 22%
- **Target Coverage:** 80%
- **Effort:** 3 hours

### Task 1.5: Enhance calendar.c Tests
- **Source:** `common/calendar.c` (154 lines)
- **Current Coverage:** 29%
- **Target Coverage:** 80%
- **Effort:** 4 hours

### Task 1.6: Enhance borders.c Tests
- **Source:** `common/borders.c` (134 lines)
- **Current Coverage:** 31%
- **Target Coverage:** 80%
- **Effort:** 4 hours

**Phase 1 Total:** 23 hours, +18% coverage

---

## Phase 2: Enable Mock Framework (Target: 65%, Week 3-5)

**Focus:** Enable mock framework and test medium-complexity modules

### Task 2.1: Enable Mock Framework in Build
- **Effort:** 4 hours
- **Dependencies:** None

### Task 2.2: Create Mock Test Utilities
- **Effort:** 3 hours
- **Dependencies:** Task 2.1

### Task 2.3-2.8: Medium Complexity Modules
| Module | Lines | Effort | Target |
|--------|-------|--------|--------|
| research.c | 1383 | 8h | 70% |
| effects.c | 1379 | 6h | 75% |
| improvement.c | 1240 | 6h | 70% |
| nation.c | 1231 | 5h | 75% |
| movement.c | 1069 | 6h | 70% |
| combat.c | 1037 | 8h | 70% |

**Phase 2 Total:** 46 hours, +18% coverage

---

## Phase 3: Complex Modules (Target: 85%, Week 6-8)

**Focus:** Highly state-dependent modules

### Task 3.1-3.2: Extend Mock Framework
- mock_city extension: 4 hours
- mock_unit extension: 4 hours

### Task 3.3-3.7: Complex Modules
| Module | Lines | Effort | Target |
|--------|-------|--------|--------|
| actions.c | 7579 | 12h | 60% |
| city.c | 3768 | 6h | 90% |
| player.c | 2029 | 2h | 90% |
| unit.c | 2976 | 8h | 85% |
| game.c | 865 | 6h | 75% |

**Phase 3 Total:** 42 hours, +17% coverage

---

## Timeline Summary

| Week | Phase | Effort | Coverage Target |
|------|-------|--------|-----------------|
| 1-2 | Phase 1 | 23h | 45% |
| 3-5 | Phase 2 | 46h | 65% |
| 6-8 | Phase 3 | 42h | 85% |

**Total Effort:** 111 hours (~14 person-days)

---

## Dependencies

```
Phase 1 (parallel)
├── Task 1.1-1.6 (no dependencies)

Phase 2 (requires Phase 1)
├── Task 2.1: Enable Mock
│   └── Task 2.2: Mock Utilities
│       └── Tasks 2.3-2.8

Phase 3 (requires Phase 2)
├── Tasks 3.1-3.2: Extend Mocks
│   └── Tasks 3.3-3.7
```

---

## Success Criteria

- [ ] Phase 1: Coverage >= 45%
- [ ] Phase 2: Coverage >= 65%
- [ ] Phase 3: Coverage >= 85%
- [ ] All tests pass
- [ ] No skipped tests in covered modules