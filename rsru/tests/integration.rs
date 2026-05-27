//! Golden tests: run each tests/*.sru in the repo root and compare stdout
//! against the matching *.expected file.

use std::path::{Path, PathBuf};
use std::process::Command;

fn rsru_binary() -> PathBuf {
    // Cargo sets CARGO_BIN_EXE_<name> for [[bin]] targets when running tests.
    PathBuf::from(env!("CARGO_BIN_EXE_rsru"))
}

fn tests_root() -> PathBuf {
    let manifest = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    manifest.join("..").join("tests")
}

fn run_one(src: &Path, expected: &Path) {
    let out = Command::new(rsru_binary())
        .arg(src)
        .output()
        .expect("failed to spawn rsru");
    let stdout = String::from_utf8_lossy(&out.stdout).to_string();
    let want = std::fs::read_to_string(expected).expect("read expected");
    assert_eq!(
        stdout, want,
        "{} output diverged\n--- got ---\n{}--- want ---\n{}",
        src.display(),
        stdout,
        want
    );
}

fn run_dir(dir: &Path) {
    let entries: Vec<_> = std::fs::read_dir(dir)
        .expect("read dir")
        .filter_map(|e| e.ok())
        .collect();
    let mut tested = 0;
    for entry in entries {
        let path = entry.path();
        if path.extension().and_then(|s| s.to_str()) != Some("sru") {
            continue;
        }
        let expected = path.with_extension("expected");
        if !expected.exists() {
            continue;
        }
        eprintln!("--- {}", path.display());
        run_one(&path, &expected);
        tested += 1;
    }
    assert!(tested > 0, "no .sru tests found under {}", dir.display());
}

#[test]
fn core_tests() {
    run_dir(&tests_root());
}

#[test]
fn parser_extension_tests() {
    run_dir(&tests_root().join("parser_extension"));
}
