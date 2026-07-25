{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-25.11";
    # nixpkgs-unstable.url = "github:nixos/nixpkgs/nixos-unstable";
    utils.url = "github:numtide/flake-utils";
  };
  outputs =
    {
      self,
      nixpkgs,
      # nixpkgs-unstable,
      utils,
      ...
    }:
    utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        # pkgs-unstable = nixpkgs-unstable.legacyPackages.${system};
      in
      {
        devShell = pkgs.mkShell {
          nativeBuildInputs = [
            pkgs.clang-tools
            pkgs.clang
            pkgs.bear
            pkgs.pkg-config
            pkgs.gcc
          ];
          buildInputs = with pkgs; [
            xorg.libX11
            libxmu
            pcre2
            json_c
            curl
            libnotify
          ];
          hardeningDisable = [ "all" ];
        };
        packages.default = pkgs.stdenv.mkDerivation rec {
          pname = "chlsdl-modules";
          version = "0.0.1";

          src = ./.;

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

          buildPhase = ''
            make libchlsdl-common COLOR=1
            make modules COLOR=1
          '';
          installPhase = ''
            make install PREFIX=$out
          '';
          hardeningDisable = [ "all" ];
        };
      }
    );
}
