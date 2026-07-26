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
        packages = {
          default = self.packages.${system}.chlsdl-modules;
          chlsdl-modules-debug = self.packages.${system}.chlsdl-modules.override {isDebug = true;};
          chlsdl-modules = pkgs.callPackage ./package.nix {};
        };
      }
    );
}
