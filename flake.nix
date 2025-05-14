{
  description = "c dev flake";
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = {
    self,
    nixpkgs,
    flake-utils,
    ...
  }:
    flake-utils.lib.eachDefaultSystem (
      system: let
        pkgs = import nixpkgs {inherit system;};
        client = pkgs.callPackage ./client.nix {};
        server = pkgs.callPackage ./server.nix {};
      in {
        packages = {
          inherit client;
          inherit server;
        };

        apps.${system} = {
          client = {
            type = "app";
            package = self.packages.${system}.client;
          };
          server = {
            type = "app";
            package = self.packages.${system}.server;
          };
        };

        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [gcc gnumake cmake client server];
          shellHook = ''
            echo "C env activated"
          '';
        };
      }
    );
}
