# Contributing to AIC8800 macOS WiFi Driver

Thank you for your interest in contributing! This document provides guidelines and information for contributors.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [How to Contribute](#how-to-contribute)
- [Pull Request Process](#pull-request-process)
- [Coding Standards](#coding-standards)
- [Reporting Bugs](#reporting-bugs)
- [Feature Requests](#feature-requests)

## Code of Conduct

- Be respectful and inclusive
- Focus on constructive feedback
- Help create a welcoming environment

## Getting Started

1. **Fork** the repository
2. **Clone** your fork:
   ```bash
   git clone https://github.com/YOUR_USERNAME/AIC8800-macOS-WiFi-Driver.git
   cd AIC8800-macOS-WiFi-Driver
   ```
3. **Add upstream** remote:
   ```bash
   git remote add upstream https://github.com/brkNx/AIC8800-macOS-WiFi-Driver.git
   ```
4. **Create a branch** for your changes:
   ```bash
   git checkout -b feature/your-feature-name
   ```

## Development Setup

### Requirements

- macOS 13.0 or later
- Xcode 15.0 or later
- Apple Developer ID (for testing on real hardware)

### Building

```bash
# Run setup script
./setup.sh

# Open in Xcode
open AIC8800WiFi.xcodeproj
```

### Testing

1. Connect a supported USB WiFi adapter
2. Run the app from Xcode
3. Check the console for logs
4. Verify WiFi functionality

## How to Contribute

### Types of Contributions

- **Bug fixes** - Fix issues with existing code
- **Features** - Add new functionality
- **Documentation** - Improve docs and README
- **Testing** - Add or improve tests
- **Code review** - Review pull requests

### Areas for Improvement

- [ ] Bluetooth support (AIC8800 has BT)
- [ ] Power management (suspend/resume)
- [ ] 5GHz band optimization
- [ ] MU-MIMO support
- [ ] Beacon filtering
- [ ] Roaming support
- [ ] Debug/logging improvements

## Pull Request Process

### Before Submitting

1. **Update your fork**:
   ```bash
   git fetch upstream
   git merge upstream/main
   ```

2. **Test your changes**:
   - Build without errors
   - Test on real hardware if possible
   - Verify no regressions

3. **Write clear commit messages**:
   ```
   feat: add Bluetooth HCI support
   
   - Add AIC8800_BT class for Bluetooth functionality
   - Implement HCI command/response handling
   - Add BT firmware loading sequence
   
   Fixes #42
   ```

### PR Template

```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Documentation update
- [ ] Other (please describe)

## Testing
Describe how you tested these changes

## Checklist
- [ ] Code follows project style
- [ ] Self-review completed
- [ ] Comments added for complex code
- [ ] Documentation updated
- [ ] No new warnings
```

## Coding Standards

### C++ (Driver Code)

- Use **lowercase_with_underscores** for functions and variables
- Use **UPPER_CASE** for constants and macros
- Add comments for complex logic
- Handle all error cases
- Use `IOLog` for logging

```cpp
// Good
kern_return_t AIC8800_USB_ReadRegister(uint16_t address, uint32_t *value)
{
    if (!value) return kIOReturnBadArgument;
    // ... implementation
}

// Bad
kern_return_t readReg(uint16_t addr, uint32_t *val)
```

### Swift (App Code)

- Follow Swift API Design Guidelines
- Use **camelCase** for functions and variables
- Use **PascalCase** for types
- Add documentation comments

```swift
// Good
func installDriver() {
    // Implementation
}

// Bad
func InstallDriver() {
    // Implementation
}
```

### File Headers

```cpp
/*
 * Copyright (c) 2024. AIC8800 macOS WiFi Driver
 *
 * Brief description of the file
 */
```

## Reporting Bugs

### Bug Report Template

```markdown
**Describe the bug**
A clear description of what the bug is.

**To Reproduce**
Steps to reproduce the behavior:
1. Plug in adapter
2. Click '...'
3. See error

**Expected behavior**
What you expected to happen.

**Environment**
- macOS version: [e.g., 14.0]
- Adapter model: [e.g., HOCO HI34]
- Xcode version: [e.g., 15.0]

**Logs**
```
Paste relevant logs here
```

**Additional context**
Any other information about the problem.
```

## Feature Requests

### Feature Request Template

```markdown
**Is your feature request related to a problem?**
A clear description of the problem. Ex. "I'm always frustrated when..."

**Describe the solution you'd like**
A clear description of what you want to happen.

**Describe alternatives you've considered**
Any alternative solutions or features you've considered.

**Additional context**
Any other information, screenshots, or logs.
```

## Questions?

Feel free to open an issue with the label "question" if you have any questions about contributing.

## License

By contributing, you agree that your contributions will be licensed under the GPL-2.0 License.
