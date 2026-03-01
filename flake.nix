{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-25.11";
    # nixpkgs-unstable.url = "github:nixos/nixpkgs/nixos-unstable";
    utils.url = "github:numtide/flake-utils";
    chlsdl.url = "github:usadev1984/chlsdl";
  };
  outputs =
    {
      self,
      nixpkgs,
      # nixpkgs-unstable,
      utils,
      chlsdl,
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
            chlsdl.packages.${system}.default
            xorg.libX11.dev
            libxmu.dev
          ];
          hardeningDisable = [ "all" ];
        };
        packages.default = pkgs.stdenv.mkDerivation rec {
          pname = "chlsdl-modules";
          version = "0.0.1";

          src = ./.;

          buildInputs = with pkgs; [
            chlsdl.packages.${system}.default
            xorg.libX11.dev
            libxmu.dev
          ];

          buildPhase = ''
            echo FIXME
            exit 1
            make -B libchlsdl-common COLOR=1
            make -B release COLOR=1
          '';
          installPhase = ''
            make install PREFIX=$out
          '';
          hardeningDisable = [ "all" ];
        };
      }
    );
}
