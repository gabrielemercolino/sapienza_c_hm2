{
  stdenv,
  gcc,
  cmake,
}:
stdenv.mkDerivation rec {
  name = "server";
  pversion = "0.9";

  src = ./.;

  nativeBuildInputs = [cmake];

  buildInputs = [gcc];

  cmakeFlags = [];

  installPhase = ''
    mkdir -p $out/bin
    cp ${name} $out/bin
  '';
}
