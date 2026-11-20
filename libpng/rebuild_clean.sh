#!/bin/bash
#
# Clean and rebuild libpng using the project's Makefile system
# This ensures all .o files go to BUILD_DIR
#

set -e

echo "=========================================="
echo "Rebuilding libpng (using project Makefile)"
echo "=========================================="
echo ""

# Get project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

echo "Project root: $PROJECT_ROOT"
echo ""

# Check if we're in the right place
if [ ! -f "$PROJECT_ROOT/scripts/build.sh" ]; then
    echo "ERROR: Cannot find scripts/build.sh"
    echo "Please run this script from the project root or libpng directory"
    exit 1
fi

# Clean third_party install directory for libpng
echo "=========================================="
echo "Step 1: Clean old libpng installation"
echo "=========================================="

# Default output root follows build.sh: out/<product-name>/...
OUT_ROOT="${PROJECT_ROOT}/out/${PRODUCT_NAME:-anypi_nova}"

INSTALL_DIRS=(
    "$OUT_ROOT/install/third_party/libpng"
    "$PROJECT_ROOT/out/install/third_party/libpng"
    "$PROJECT_ROOT/install/third_party/libpng"
    "$PROJECT_ROOT/third_party/install/libpng"
)

for dir in "${INSTALL_DIRS[@]}"; do
    if [ -d "$dir" ]; then
        echo "Removing: $dir"
        rm -rf "$dir"
    fi
done

echo ""
echo "=========================================="
echo "Step 2: Clean build directory"
echo "=========================================="

BUILD_DIRS=(
    "$OUT_ROOT/build/third_party/libpng"
    "$PROJECT_ROOT/out/build/third_party/libpng"
    "$PROJECT_ROOT/build/third_party/libpng"
    "$PROJECT_ROOT/third_party/libpng/build"
)

for dir in "${BUILD_DIRS[@]}"; do
    if [ -d "$dir" ]; then
        echo "Removing: $dir"
        rm -rf "$dir"
    fi
done

# Clean any .o files in source directory
echo ""
echo "Cleaning source directory..."
cd "$SCRIPT_DIR"
find . -name "*.o" -type f -delete 2>/dev/null || true
find . -name "*.a" -type f -delete 2>/dev/null || true
echo "Source directory cleaned"

echo ""
echo "=========================================="
echo "Step 3: Rebuild using build.sh"
echo "=========================================="
echo ""
echo "Running: cd $PROJECT_ROOT && ./scripts/build.sh --product-name ${PRODUCT_NAME:-anypi_nova}"
echo ""

cd "$PROJECT_ROOT"

# Build third_party
# Keep output isolated per product (build.sh writes to out/<product>/...).
# If PRODUCT_NAME is unset, default to anypi_nova (legacy behavior).
BUILD_PRODUCT_NAME="${PRODUCT_NAME:-anypi_nova}"
if ./scripts/build.sh --product-name "${BUILD_PRODUCT_NAME}" 2>&1 | tee /tmp/libpng_build.log; then
    echo ""
    echo "=========================================="
    echo "Step 4: Verify build"
    echo "=========================================="
    echo ""
    
    # Find the installed library
    LIBPNG_LIB=""
    for dir in "${INSTALL_DIRS[@]}"; do
        if [ -f "$dir/lib/libpng.a" ]; then
            LIBPNG_LIB="$dir/lib/libpng.a"
            break
        fi
    done
    
    if [ -z "$LIBPNG_LIB" ]; then
        echo "ERROR: libpng.a not found after build"
        exit 1
    fi
    
    echo "Found: $LIBPNG_LIB"
    echo ""
    
    # Verify it's clean
    if command -v nm >/dev/null 2>&1; then
        MAIN_COUNT=$(nm "$LIBPNG_LIB" 2>/dev/null | grep -c " T main$" || echo "0")
        if [ "$MAIN_COUNT" -gt 0 ]; then
            echo "ERROR: Library contains main() function!"
            echo "The build process may have included test files"
            exit 1
        else
            echo "✓ Library is clean (no main() function)"
        fi
        
        PNGTEST_COUNT=$(nm "$LIBPNG_LIB" 2>/dev/null | grep -ic "pngtest" || echo "0")
        if [ "$PNGTEST_COUNT" -gt 0 ]; then
            echo "WARNING: Found $PNGTEST_COUNT pngtest symbols"
        else
            echo "✓ No pngtest symbols found"
        fi
    fi
    
    if command -v strings >/dev/null 2>&1; then
        if strings "$LIBPNG_LIB" 2>/dev/null | grep -q "Testing libpng"; then
            echo "ERROR: Library contains 'Testing libpng' string!"
            exit 1
        else
            echo "✓ No 'Testing libpng' string found"
        fi
    fi
    
    # Check build directory
    echo ""
    echo "Checking build directory structure..."
    BUILD_DIR=$(find "$PROJECT_ROOT" -path "*/build/third_party/libpng" -type d 2>/dev/null | head -1)
    if [ -n "$BUILD_DIR" ]; then
        echo "Build directory: $BUILD_DIR"
        OBJ_COUNT=$(find "$BUILD_DIR" -name "*.o" | wc -l)
        echo "  Object files (.o): $OBJ_COUNT"
        LIB_COUNT=$(find "$BUILD_DIR" -name "*.a" | wc -l)
        echo "  Library files (.a): $LIB_COUNT"
    fi
    
    # Check source directory is clean
    echo ""
    echo "Checking source directory..."
    SRC_OBJ_COUNT=$(find "$SCRIPT_DIR" -name "*.o" -not -path "*/build/*" | wc -l)
    if [ "$SRC_OBJ_COUNT" -eq 0 ]; then
        echo "✓ Source directory is clean (no .o files)"
    else
        echo "WARNING: Found $SRC_OBJ_COUNT .o files in source directory"
        find "$SCRIPT_DIR" -name "*.o" -not -path "*/build/*"
    fi
    
    echo ""
    echo "=========================================="
    echo "SUCCESS!"
    echo "=========================================="
    echo ""
    echo "libpng has been rebuilt cleanly"
    echo ""
    echo "Next steps:"
    echo "  1. Rebuild framework tests:"
    echo "     make -C framework/tests clean"
    echo "     ./scripts/build.sh"
    echo ""
    echo "  2. Test the binary:"
    echo "     ~/nfs/framework/fw_multimedia_sample --help"
    echo ""
else
    echo ""
    echo "=========================================="
    echo "BUILD FAILED"
    echo "=========================================="
    echo ""
    echo "Check the build log: /tmp/libpng_build.log"
    exit 1
fi
