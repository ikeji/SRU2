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

#[test]
fn rsru_only_tests() {
    run_dir(&tests_root().join("rsru_only"));
}

/// Compile every `tests/*.sru` to bytecode with `rsruc`, then execute the
/// `.bc` and check stdout against the matching `.expected`. Confirms that
/// the bytecode round-trip (compile → encode → decode → eval) preserves
/// semantics for every core test.
#[test]
fn bytecode_roundtrip() {
    let rsruc = PathBuf::from(env!("CARGO_BIN_EXE_rsruc"));
    let dir = tests_root();
    let mut tested = 0;
    for entry in std::fs::read_dir(&dir).expect("read dir") {
        let path = entry.expect("entry").path();
        if path.extension().and_then(|s| s.to_str()) != Some("sru") {
            continue;
        }
        let expected = path.with_extension("expected");
        if !expected.exists() {
            continue;
        }
        let bc = tempfile_path(&path);
        let status = Command::new(&rsruc)
            .arg(&path)
            .arg("-o")
            .arg(&bc)
            .status()
            .expect("rsruc spawn");
        assert!(status.success(), "rsruc failed on {}", path.display());

        let out = Command::new(rsru_binary())
            .arg(&bc)
            .output()
            .expect("rsru bytecode spawn");
        let stdout = String::from_utf8_lossy(&out.stdout).to_string();
        let want = std::fs::read_to_string(&expected).expect("read expected");
        assert_eq!(
            stdout, want,
            "{} bytecode output diverged",
            path.display()
        );
        tested += 1;
    }
    assert!(tested > 0);
}

fn tempfile_path(src: &Path) -> PathBuf {
    let name = src.file_stem().and_then(|s| s.to_str()).unwrap_or("out");
    let mut tmp = std::env::temp_dir();
    tmp.push(format!("rsru-bc-test-{}.bc", name));
    tmp
}
