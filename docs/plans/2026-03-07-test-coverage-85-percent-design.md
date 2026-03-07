# Test Coverage Improvement Plan

## Overview

**Goal:** Improve test coverage from 24.7% to 85%

**Strategy:** Layered progressive approach - start with simple modules, progressively enable mock framework for complex modules.

**Timeline:** 6-8 weeks

---

## Current State

| Metric | Current | Target |
|--------|---------|--------|
| Line Coverage | 24.7% | 85% |
| Function Coverage | 42.1% | 90% |
| Test Count | 59 | 150+ |

### High Coverage Modules (>70%)
- city.c: 87.3%
- player.c: 122%
- requirements.c: 106%
- unit.c: 75.5%

### Low Coverage Modules (Needs Work)
- unitlist.c: 0%
- clientutils.c: 0%
- research.c: 17%
- effects.c: 25%

---

## Phase 1: Quick Wins (Target: 45%, 2 weeks)

**Focus:** Simple modules with minimal dependencies

### Modules to Test

| Module | Lines | Current | Target | Difficulty |
|--------|-------|---------|--------|------------|
| unitlist.c | 104 | 0% | 80% | Easy |
| clientutils.c | 150 | 0% | 80% | Easy |
| victory.c | 200 | 50% | 85% | Easy |
| workertask.c | 150 | 22% | 80% | Easy |
| calendar.c | 300 | 29% | 80% | Easy |
| borders.c | 200 | 31% | 80% | Easy |

### Expected Gain: +15-20% coverage

---

## Phase 2: Medium Difficulty (Target: 65%, 3 weeks)

**Focus:** Modules requiring game state initialization

### Key Tasks
1. Enable `tests/unit/mock/` framework
2. Create minimal test ruleset
3. Write setup/teardown fixtures

### Modules to Test

| Module | Lines | Strategy |
|--------|-------|----------|
| research.c | 1383 | Enable mock_game |
| effects.c | 1379 | Extend effect tests |
| improvement.c | 1240 | Needs ruleset data |
| nation.c | 1231 | Simple lookup functions |
| movement.c | 1069 | Movement calculations |
| combat.c | 1037 | Combat calculations |

### Expected Gain: +15-20% coverage

---

## Phase 3: Complex Modules (Target: 85%, 3 weeks)

**Focus:** Highly state-dependent modules

### Key Tasks
1. Complete mock_city, mock_unit, mock_player
2. Write integration test cases
3. Handle network-related code

### Modules to Test

| Module | Lines | Difficulty | Strategy |
|--------|-------|------------|----------|
| actions.c | 7579 | High | Sub-module testing |
| city.c | 3768 | High | Integration tests |
| player.c | 2029 | Medium | mock_player extension |
| unit.c | 2976 | High | mock_unit extension |
| game.c | 865 | Medium | Full game state mock |

### Expected Gain: +15-20% coverage

---

## Key Success Factors

1. **Mock Framework** - Must be enabled by Phase 2
2. **Test Ruleset** - Create minimal ruleset for testing
3. **Parallel Development** - Use background agents to accelerate
4. **Continuous Verification** - Verify coverage targets each phase

---

## Technical Dependencies

### Mock Framework Files
- `tests/unit/mock/mock_game.c/h`
- `tests/unit/mock/mock_player.c/h`
- `tests/unit/mock/mock_map.c/h`
- `tests/unit/mock/mock_city.c/h`
- `tests/unit/mock/mock_unit.c/h`

### Build System
- Meson build configuration
- CMocka test framework
- lcov/gcov for coverage

---

## Risk Mitigation

| Risk | Mitigation |
|------|------------|
| Mock framework incomplete | Start with simpler modules first |
| Complex module dependencies | Use integration tests where unit tests fail |
| Agent-generated code errors | Manual review and fix before commit |
| Time overrun | Prioritize by ROI, accept 70-80% if needed |

---

## Approval

- **Date:** 2026-03-07
- **Strategy:** High ROI priority, layered progressive approach
- **Timeline:** 6-8 weeks for full 85% coverage