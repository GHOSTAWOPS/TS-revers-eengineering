#!/usr/bin/env python3
from pathlib import Path
import argparse
import sys


FORBIDDEN_TOKENS = [
    "TopoDS_",
    "TopoDS::",
    "TopAbs_",
    "TopExp_",
    "BRep",
    "BRepAdaptor_",
    "BRepAlgoAPI_",
    "BRepBuilderAPI_",
    "BRep_Tool",
    "BRepGProp",
    "BRepClass3d",
    "Geom_",
    "Geom2d_",
    "GeomAbs_",
    "GCPnts_",
    "gp_",
    "AIS_",
    "Standard_",
    "Handle(Geom",
]


def scan(root: Path) -> list[str]:
    findings: list[str] = []
    for path in sorted(root.rglob("*")):
        if path.suffix.lower() not in {".h", ".hpp", ".cpp", ".cxx"}:
            continue
        text = path.read_text(encoding="utf-8", errors="replace")
        for line_number, line in enumerate(text.splitlines(), start=1):
            for token in FORBIDDEN_TOKENS:
                if token in line:
                    findings.append(f"{path}:{line_number}: forbidden token {token}")
    return findings


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("root", type=Path)
    args = parser.parse_args()

    if not args.root.exists():
        print(f"domain rebar path does not exist: {args.root}", file=sys.stderr)
        return 2

    findings = scan(args.root)
    if findings:
        print("\n".join(findings), file=sys.stderr)
        return 1

    print(f"domain/rebar boundary ok: {args.root}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

