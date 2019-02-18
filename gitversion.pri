# If there is no version tag in git this one will be used
VERSION = 0.1.0

# Need to discard STDERR so get path to NULL device
win32 {
    NULL_DEVICE = NUL # Windows doesn't have /dev/null but has NUL
} else {
    NULL_DEVICE = /dev/null
}

# Trying to get version from git tag / revision
GIT_VERSION = $$system(git describe --long 2> $$NULL_DEVICE)

# Version is initial dd.dd.dd from tag.
VERSION = $$GIT_VERSION
VERSION ~= s/-\d+-.*//

# Bundle version is VERSION plus 'd' and number of commits if not 0.
BUNDLE_VERSION = $$GIT_VERSION
BUNDLE_VERSION ~= s/-(\d+)-.*/d\1/
BUNDLE_VERSION ~= s/d0$//

DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\" APP_VERSION=\\\"$$VERSION\\\"

# With Qt 5.12.1, Qt Creator 4.8.1, no version info gets into Info.plist.
# Add it. CFBundleShortVersionString and CFBundleVersion aren't set when
# Info.plist is created. We could just Add them, but that will error
# if either has already been added (i.e. we're not doing a clean build).
# So merge it in if missing; this is the only way I can find which doesn't
# error if either already exists. It doesn't update them either, which
# is not what I'd prefer, but I'l settle for.
macx {
    INFO_PLIST_PATH = $$shell_quote($${OUT_PWD}/$${TARGET}.app/Contents/Info.plist)
    QMAKE_POST_LINK += /usr/libexec/PlistBuddy -c \"Clear dict\" -c \"Add :CFBundleShortVersionString string $${VERSION}\" -c \"Add :CFBundleVersion string $${BUNDLE_VERSION}\" version.plist;  /usr/libexec/PlistBuddy -c \"Merge version.plist\"  -c \"Set :CFBundleGetInfoString $${GIT_VERSION}\" $${INFO_PLIST_PATH}
}
