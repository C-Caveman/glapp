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
  name = "cg23";
  src = pkgs.fetchgit {
    url = "https://github.com:C-Caveman/cg23";
    #rev = "41473ff9d321ff48f362f9d15c92f9113032c16c";
    #sha256 = "";
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
    cd ./GLapp
    cmake -S ./. -B ./build
    cd build
    make
  '';
 
} 
