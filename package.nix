{
  lib,
  stdenv,
  pkgs,
  isDebug ? false,
  enableColor ? false,
  ...
}:

stdenv.mkDerivation rec {
  pname = "chlsdl-modules";
  version = "0.0.1";

  src = pkgs.nix-gitignore.gitignoreSourcePure ''
    *

    !**/Makefile
    !**/config.mk

    !src
    !src/**
    !src/**/*.[ch]

    !include
    !include/chlsdl-modules
    !include/chlsdl-modules/**
    !include/chlsdl-modules/**/*.[ch]'' ./.;

  outputs = [ "out" "dev" ];

  # needed because certain public headers may include headers from
  # programs the main program may not depend on
  propagatedNativeBuildInputs = with pkgs; [
    curl.dev
  ];

  buildInputs = with pkgs; [
    xorg.libX11
    libxmu
    pcre2
    json_c
    curl
    libnotify
  ];

  dontStrip = if isDebug then true else false;

  buildPhase =
    lib.concatStringsSep " " (
      [
        "make"
        (if isDebug then "debug_libchlsdl-common" else "libchlsdl-common")
      ]
      ++ lib.optionals enableColor [ "COLOR=1" ]
    )
    + "\n" + lib.concatStringsSep " " (
      [
        "make"
        (if isDebug then "debug_modules" else "modules")
      ]
      ++ lib.optionals enableColor [ "COLOR=1" ]
    );

  # buildPhase = ''
  #   make libchlsdl-common COLOR=1
  #   make modules COLOR=1
  # '';
  installPhase = ''
    make install PREFIX=$out
  '';
  hardeningDisable = [ "all" ];
}
