#nix-build -E 'with import <nixpkgs> {}; callPackage ./default.nix {}'

{ stdenv, gcc, cmake,
  xorg,
  gnumake, pkg-config, autoconf, lib, pkgs
 } :
    let

        libPath = lib.makeLibraryPath [
        xorg.libX11.out
        (lib.getLib gcc.cc)
        ];
    in
stdenv.mkDerivation rec {
  #version = "9.7.6";
  name = "glapp";
  src = pkgs.fetchgit {
    url = "https://github.com/C-Caveman/glapp.git";
    #rev = "41473ff9d321ff48f362f9d15c92f9113032c16c";
    sha256 = "sha256-DhuHvRD/vElJdSEMNwmeckddRsGGY60u0xlsq4oddHE=";
  };
  sourceRoot = ./.;
  nativeBuildInputs = [ cmake gnumake ];
  buildInputs = [ pkg-config gnumake xorg.libX11 xorg.libX11.dev xorg.libX11.dev.out ];
  phases = [ "installPhase" "buildPhase" ];
  installPhase = ''
    mkdir -p $out/bin
    #cp bin/ds9 $out/bin/ds9
  '';
  buildPhase = ''
    cmake -S ./. -B ./build
    cd build
    make
  '';
 
} 
