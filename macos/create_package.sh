#!/bin/bash

# Configuration
APP_NAME="MicroCI"
VERSION="0.39.0"
IDENTIFIER="dev.microci.macos"
INSTALL_DIR="/usr/local/bin"

rm -rf package
rm -f ${APP_NAME}-${VERSION}.pkg

make -C ../src/

mkdir -p package${INSTALL_DIR}
cp ../bin/microCI package${INSTALL_DIR}

# Create component package
pkgbuild \
    --root package \
    --identifier ${IDENTIFIER} \
    --version ${VERSION} \
    --install-location / \
    ${APP_NAME}-${VERSION}-component.pkg

# Create distribution file
cat <<EOF > Distribution.xml
<?xml version="1.0" encoding="utf-8"?>
<installer-gui-script minSpecVersion="1">
    <title>${APP_NAME} ${VERSION}</title>
    <organization>dev.microci</organization>
    <domains enable_localSystem="true"/>
    <options customize="never" require-scripts="false"/>
    <allowed-os-versions>
        <os-version min="10.12"/>
    </allowed-os-versions>
    <welcome file="welcome.html"/>
    <license file="license.html"/>
    <conclusion file="conclusion.html"/>
    <pkg-ref id="${IDENTIFIER}"/>
    <choices-outline>
        <line choice="default">
            <line choice="${IDENTIFIER}"/>
        </line>
    </choices-outline>
    <choice id="default"/>
    <choice id="${IDENTIFIER}" visible="false">
        <pkg-ref id="${IDENTIFIER}"/>
    </choice>
    <pkg-ref id="${IDENTIFIER}" version="${VERSION}" onConclusion="none">${APP_NAME}-${VERSION}-component.pkg</pkg-ref>
</installer-gui-script>
EOF

# Create welcome/license/conclusion files
cat <<EOF > welcome.html
<!DOCTYPE html>
<html>
<head>
    <title>Welcome</title>
</head>
<body>
    <h1>Welcome to ${APP_NAME} ${VERSION}</h1>
    <p>This package will install ${APP_NAME} on your system.</p>
</body>
</html>
EOF

cat <<EOF > license.html
<!DOCTYPE html>
<html>
<head>
    <title>License</title>
</head>
<body>
    <h1>MIT License</h1>

<p>Copyright (c) 2022-2025 Geraldo Luis da Silva Ribeiro</p>

<p>
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
</p>

<p>
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
</p>

<p>
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
</p>
</body>
</html>
EOF

cat <<EOF > conclusion.html
<!DOCTYPE html>
<html>
<head>
    <title>Installation Complete</title>
</head>
<body>
    <h1>Installation Complete</h1>
    <p>${APP_NAME} has been successfully installed.</p>
    <p>You can now run it from the terminal with: microCI</p>
</body>
</html>
EOF

# Create the product package
productbuild \
    --distribution Distribution.xml \
    --resources . \
    --package-path ${APP_NAME}-${VERSION}-component.pkg \
    ${APP_NAME}-${VERSION}.pkg

# Clean up
rm -f ${APP_NAME}-${VERSION}-component.pkg
rm -f Distribution.xml
rm -f welcome.html license.html conclusion.html

echo "Package created: ${APP_NAME}-${VERSION}.pkg"
