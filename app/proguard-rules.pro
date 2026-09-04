# Keep JNI entry points
-keepclasseswithmembernames class * {
    native <methods>;
}

# Specific for GCalManager if it exists
-keep class org.gaurabda.GCalManager {
    native <methods>;
}
