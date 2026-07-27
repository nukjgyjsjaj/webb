# Contributing to Amnesia Browser

## Code of Conduct
- Be respectful and inclusive
- Focus on constructive feedback
- Respect differing viewpoints and experiences

## How to Contribute

### 1. Setup Development Environment
```bash
# Clone repository
git clone https://github.com/amnesia-browser/amnesia.git
cd amnesia

# Install depot_tools
git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git ~/depot_tools
export PATH=~/depot_tools:$PATH

# Fetch Chromium
cd AmnesiaBrowser
./scripts/fetch_chromium.sh

# Configure
./scripts/configure.sh

# Build
./scripts/build.sh
```

### 2. Find an Issue
- Check GitHub Issues labeled `good first issue` or `help wanted`
- Comment on the issue to claim it

### 3. Create a Branch
```bash
git checkout -b feature/your-feature-name
```

### 4. Make Changes
- Follow the project's coding style
- Write tests for new features
- Update documentation as needed

### 5. Run Tests
```bash
# Unit tests
./scripts/run_unit_tests.sh

# Integration tests
./scripts/run_integration_tests.sh

# Consistency validation
./scripts/run_consistency_tests.sh
```

### 6. Submit PR
- Push your branch
- Create a Pull Request with a clear description
- Ensure CI passes
- Address review feedback

## Coding Standards
- C++17 standard
- Chromium style guide
- clang-format for formatting
- Meaningful variable/function names
- Comments for complex logic

## Security Considerations
- All fingerprinting patches must pass consistency validation
- No hardcoded credentials
- Memory must be zeroized when appropriate
- No telemetry without explicit user consent

## Review Process
1. CI must pass (build + tests)
2. At least one maintainer approval
3. Security review for fingerprinting patches
4. Consistency validation for new signals

## License
By contributing, you agree that your contributions will be licensed under the project's license (TBD).

## Questions?
Join our Discord or open a Discussion on GitHub.
