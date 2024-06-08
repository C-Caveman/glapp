# glapp.nix
let
  pkgs = import <nixpkgs> { };
in
  pkgs.stdenv.mkDerivation {
    name = "glapp";
    builder = "${pkgs.cmake}/bin/make";
    args = [ "make" ];
    inherit (pkgs) gcc coreutils;
    #src = ./simple.c;
    system = builtins.currentSystem;
  }
