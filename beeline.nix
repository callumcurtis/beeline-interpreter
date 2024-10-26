{ lib, stdenv, cmake, boost182, catch2 }:

stdenv.mkDerivation {
  name = "beeline";

  src = lib.sourceByRegex ./. [
    "^src.*$"
    "^test.*$"
    "^example.*$"
    "^include.*$"
    "^app.*$"
    "^demo$"
    "^CMakeLists\.txt$"
    "^Sanitizers\.cmake$"
    "^CodeCoverage\.cmake$"
  ];

  nativeBuildInputs = [ cmake ];
  buildInputs = [ boost182 catch2 ];

  doCheck = true;
}
