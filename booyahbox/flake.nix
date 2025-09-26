{
  description = "BooyahBox — Raspberry Pi Pico (C++/CMake) with Nix flake";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.11";
  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
    let
      pkgs = import nixpkgs { inherit system; };

      picoSdkPath = "${pkgs.pico-sdk}/lib/pico-sdk";  # utilisé par CMakeLists.txt
    in {
      # ── Dev shell: toolchain ARM + Pico SDK ─────────────────────────────────
      devShells.default = pkgs.mkShell {
        packages = with pkgs; [
          cmake ninja pkg-config
          gcc-arm-embedded     # toolchain ARM (arm-none-eabi-*)
          python312             # requis par des scripts CMake du SDK
          pico-sdk
          elf2uf2-rs            # util convert .elf → .uf2 (rapide et pratique)
          gdb-multiarch         # debug si besoin
          openocd               # flash/debug (optionnel)
        ];
        PICO_SDK_PATH = "${pkgs.pico-sdk}/lib/pico-sdk";
        shellHook = ''
          echo "→ BooyahBox (Pico) dev-shell"
          echo "  PICO_SDK_PATH=$PICO_SDK_PATH"
          echo "  Toolchain: $(arm-none-eabi-gcc --version | head -n1)"
        '';
      };

      # ── Package: construit .uf2 avec Nix (nix build .#default) ──────────────
      packages.default = pkgs.stdenv.mkDerivation {
        pname = "booyahbox";
        version = "0.1.0";
        src = ./.;

        nativeBuildInputs = with pkgs; [ cmake ninja python312 pkg-config ];
        buildInputs = with pkgs; [ gcc-arm-embedded pico-sdk ];

        # Le CMakeLists attend PICO_SDK_PATH; on le passe explicitement
        cmakeFlags = [
          "-G" "Ninja"
          "-DCMAKE_BUILD_TYPE=Release"
          "-DPICO_SDK_PATH=${picoSdkPath}"
        ];

        doCheck = false;

        installPhase = ''
          runHook preInstall
          mkdir -p $out/uf2 $out/build $out/bin
          # Copier .uf2 et binaires liés si présents
          if [ -d build ]; then src_build=build; else src_build=. ; fi
          find "$src_build" -type f -name "*.uf2" -exec cp -v {} $out/uf2/ \; || true
          # Certains projets génèrent aussi des .elf/.bin
          find "$src_build" -type f \( -name "*.elf" -o -name "*.bin" \) -exec cp -v {} $out/build/ \; || true
          # Si un exécutable hôte existe (rare pour Pico), on peut le copier
          find "$src_build" -type f -perm -111 -exec cp -t $out/bin {} + 2>/dev/null || true
          echo "Install done. UF2 in $out/uf2" >&2
          runHook postInstall
        '';
      };

      # ── App facultative: echo chemin UF2 (nix run) ──────────────────────────
      apps.default = {
        type = "app";
        program = pkgs.writeShellScript "show-uf2" ''
          set -euo pipefail
          outPath=${self.packages.${system}.default}
          echo "UF2 files in: $outPath/uf2";
          ls -1 "$outPath/uf2" || echo "(aucun .uf2 construit — lancez: nix build)"
        '';
      };
    });
}
