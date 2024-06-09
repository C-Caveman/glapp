#nix-build -E 'with import <nixpkgs> {}; callPackage ./default.nix {}'

{ stdenv, gcc, cmake,
  xorg, glm,
  gnumake, pkg-config, autoconf, lib, pkgs, glfw, glew
 } :
    let

        libPath = lib.makeLibraryPath [
        xorg.libX11.out
        (lib.getLib gcc.cc)
        ];
    in
stdenv.mkDerivation rec {
  name = "glapp";
  src = pkgs.fetchgit {
    url = "https://github.com/C-Caveman/glapp.git";
    #rev = "41473ff9d321ff48f362f9d15c92f9113032c16c";
    #sha256 = "sha256-u8lEnuxGRb8Rh+IAupzTdTV9PkeOb8VTK/Cu6xa1FG8=";
  };
  nativeBuildInputs = [ cmake gnumake ];
  buildInputs = [ pkg-config gnumake xorg.libX11 xorg.libX11.dev xorg.libX11.dev.out xorg.xrandr glfw xorg.xinput ];
  propagatedBuildInputs = [
    xorg.libX11 xorg.libX11.dev xorg.libX11.dev.out xorg.xrandr glfw xorg.libXrandr xorg.libXxf86vm xorg.libXcursor xorg.libXinerama
    glew glm xorg.xinput xorg.xinit xorg.libXi
  ];
  installPhase = ''
  mkdir -p $out/success
  cp /build/glapp/build/GLapp $out/success
  chmod +x $out/success/GLapp
  '';
} 
