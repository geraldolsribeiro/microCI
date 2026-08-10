class Microci < Formula
  desc "Minimalist continuous integration tool"
  homepage "https://microci.dev"
  url "https://github.com/geraldolsribeiro/microCI/archive/refs/tags/v0.49.1.tar.gz"
  sha256 "b8bf5c601d1b7d8c1058127c7ae1c33f80e5d680fa7062c505b9a66668def0c3"
  license "MIT"
  head "https://github.com/geraldolsribeiro/microci.git", branch: "master"

  depends_on "gettext"
  depends_on "spdlog"
  depends_on "yaml-cpp"

  def install
    system "make", "-C", "src", "CXXFLAGS=-std=c++20 -DFMT_HEADER_ONLY -DSPDLOG_FMT_EXTERNAL=1 -I../include/ -I../include/3rd"
    bin.mkpath
    bin.install "bin/microCI"
  end

  test do
    # `test do` will create, run in and delete a temporary directory.
    #
    # This test will fail and we won't accept that! For Homebrew/homebrew-core
    # this will need to be a test that verifies the functionality of the
    # software. Run the test with `brew test microCI`. Options passed
    # to `brew install` such as `--HEAD` also need to be provided to `brew test`.
    #
    # The installed folder is not in the path, so use the entire path to any
    # executables being tested: `system bin/"program", "do", "something"`.
    system "bin/microCI", "--version"
  end
end
