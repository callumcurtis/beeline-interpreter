{
  description = "Beeline Interpreter";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        packages.default = pkgs.callPackage ./beeline.nix { };
        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            hyperfine
            python313
            cmake
            boost182
            catch2
          ];
        };
      }
    );
}
