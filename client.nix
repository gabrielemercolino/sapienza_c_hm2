{
  stdenv,
  gcc,
  cmake,
}:
stdenv.mkDerivation rec {
  name = "client";
  pversion = "1.0.0";

  src = ./.;

  nativeBuildInputs = [cmake];

  buildInputs = [gcc];

  cmakeFlags = [];

  installPhase = ''
    mkdir -p $out/bin
    cp ${name} $out/bin
  '';
}
