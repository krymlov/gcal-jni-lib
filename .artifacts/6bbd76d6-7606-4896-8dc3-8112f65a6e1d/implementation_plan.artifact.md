# Implementation Plan - Build Configuration Optimization

Optimize the Gradle build configuration for the `gcal-jni-lib` AAR library to improve portability, performance, and correctness.

## User Review Required

> [!WARNING]
> I am removing the hardcoded `buildDir` override in the root `build.gradle`. This will change where build artifacts are stored (defaulting to the project's `build` folders) but is necessary for the project to build on any machine without a `u:` drive.

## Proposed Changes

### Build Configuration

#### [MODIFY] [build.gradle (root)](file:///E:/Github/gcal-jni-lib/build.gradle)
- Remove the hardcoded `buildDir` override (`u:/apps/...`) which causes build failures on most machines.
- Clean up the `clean` task.

#### [MODIFY] [app/build.gradle](file:///E:/Github/gcal-jni-lib/app/build.gradle)
- Add `consumerProguardFiles` to ensure JNI methods are kept when the AAR is used in an app.
- Refine the `publishing` configuration to follow modern AGP practices.
- Add `ndk { abiFilters ... }` if we want to restrict which ABIs are bundled (currently it seems all are provided in `jniLibs`).
- Ensure `minifyEnabled` is handled correctly for a library.

#### [MODIFY] [gradle.properties](file:///E:/Github/gcal-jni-lib/gradle.properties)
- Enable Gradle parallel execution and build caching for faster builds.
- Disable Jetifier if no legacy support libraries are used.

## Verification Plan

### Automated Tests
- Run `./gradlew clean assembleRelease` to verify the build completes successfully without the `u:` drive dependency.
- Run `./gradlew help` to verify configuration cache compatibility.

### Manual Verification
- Inspect the generated AAR in `app/build/outputs/aar/` to ensure it contains the expected `.so` files and `proguard.txt`.
