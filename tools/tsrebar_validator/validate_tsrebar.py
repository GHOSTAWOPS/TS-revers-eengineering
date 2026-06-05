"""tsrebar package validator.

This is a development gate for the documentation fixtures. It validates the
new package contract; it does not validate old VisualTS runtime behavior.
"""

from __future__ import annotations

import argparse
import hashlib
import json
from datetime import datetime, timezone, timedelta
from pathlib import Path
from typing import Any


RUNNER_VERSION = "tsrebar-validator/0.1"
SCHEMA_VERSION = "validation-report/v1"
CHINA_TZ = timezone(timedelta(hours=8))


REQUIRED_PACKAGE_FILES = [
    "manifest.json",
    "project.json",
    "geometry/source_step.json",
    "geometry/topology_refs.json",
    "rebar/steel_data.json",
    "rebar/groups.json",
    "rebar/bars.json",
    "rebar/segments.json",
    "drawing/detail_mapping.json",
    "evidence/evidence_index.json",
    "evidence/unresolved_fields.json",
]


def load_json(path: Path) -> Any:
    with path.open("r", encoding="utf-8-sig") as handle:
        return json.load(handle)


def package_hash(package_path: Path) -> str:
    digest = hashlib.sha256()
    for path in sorted(package_path.rglob("*")):
        if not path.is_file():
            continue
        rel = path.relative_to(package_path).as_posix()
        digest.update(rel.encode("utf-8"))
        digest.update(b"\0")
        digest.update(path.read_bytes())
        digest.update(b"\0")
    return f"sha256:{digest.hexdigest()}"


def json_pointer_get(documents: dict[str, Any], pointer: str) -> Any:
    if not pointer or "#" not in pointer:
        raise KeyError(pointer)
    rel, fragment = pointer.split("#", 1)
    if rel not in documents:
        raise KeyError(pointer)
    value = documents[rel]
    if fragment in ("", "/"):
        return value
    for raw_part in fragment.lstrip("/").split("/"):
        part = raw_part.replace("~1", "/").replace("~0", "~")
        if isinstance(value, list):
            value = value[int(part)]
        elif isinstance(value, dict):
            value = value[part]
        else:
            raise KeyError(pointer)
    return value


class ValidationContext:
    def __init__(self, package_path: Path):
        self.package_path = package_path
        self.documents: dict[str, Any] = {}
        self.findings: list[dict[str, Any]] = []
        self.known_evidence_ids: set[str] = set()
        self.unresolved_paths: set[str] = set()
        self.missing_required_files: set[str] = set()

    def add(
        self,
        validator: str,
        error_code: str,
        message: str,
        *,
        severity: str = "error",
        package_file: str = "",
        json_path: str = "",
        object_id: str = "",
        evidence_hint: list[str] | None = None,
    ) -> None:
        self.findings.append(
            {
                "validatorName": validator,
                "objectId": object_id,
                "packageFile": package_file,
                "jsonPath": json_path,
                "errorCode": error_code,
                "message": message,
                "evidenceHint": evidence_hint or [],
                "severity": severity,
            }
        )

    def read_if_present(self, rel: str) -> Any | None:
        path = self.package_path / rel
        if not path.exists():
            return None
        try:
            data = load_json(path)
        except json.JSONDecodeError as exc:
            self.add(
                "PackageValidator",
                "PV002_JSON_PARSE_FAILED",
                f"JSON parse failed: {exc}",
                package_file=rel,
            )
            return None
        self.documents[rel] = data
        return data


def validate_package(package_path: str | Path) -> dict[str, Any]:
    package = Path(package_path)
    ctx = ValidationContext(package)

    validate_package_files(ctx)
    load_evidence_context(ctx)
    validate_project_references(ctx)
    validate_legacy_objects(ctx)
    validate_geometry_refs(ctx)
    validate_bindings(ctx)
    validate_detail_mapping(ctx)
    validate_evidence(ctx)
    validate_transaction(ctx)

    error_count = sum(1 for item in ctx.findings if item["severity"] == "error")
    warning_count = sum(1 for item in ctx.findings if item["severity"] == "warning")
    if error_count:
        decision = "fail"
    elif warning_count:
        decision = "warning-only"
    else:
        decision = "pass"

    return {
        "schemaVersion": SCHEMA_VERSION,
        "reportId": report_id_for(package),
        "packagePath": package.as_posix(),
        "packageHash": package_hash(package) if package.exists() else None,
        "createdAt": datetime.now(CHINA_TZ).isoformat(timespec="seconds"),
        "runnerVersion": RUNNER_VERSION,
        "decision": decision,
        "errorCount": error_count,
        "warningCount": warning_count,
        "findings": ctx.findings,
    }


def validate_package_files(ctx: ValidationContext) -> None:
    manifest = ctx.read_if_present("manifest.json")
    if manifest is None:
        ctx.missing_required_files.add("manifest.json")
        ctx.add("PackageValidator", "PV001_REQUIRED_FILE_MISSING", "manifest.json is missing", package_file="manifest.json")
        return

    required = manifest.get("requiredFiles", [])
    for rel in ["manifest.json", *required]:
        if not (ctx.package_path / rel).exists():
            ctx.missing_required_files.add(rel)
            ctx.add(
                "PackageValidator",
                "PV001_REQUIRED_FILE_MISSING",
                f"manifest.requiredFiles contains missing file: {rel}",
                package_file=rel,
            )

    for rel in REQUIRED_PACKAGE_FILES:
        ctx.read_if_present(rel)

    save_state = manifest.get("saveTransaction", {}).get("state")
    if save_state and save_state != "complete":
        ctx.add(
            "PackageValidator",
            "PV004_SAVE_TRANSACTION_INCOMPLETE",
            f"saveTransaction.state is {save_state}, expected complete",
            package_file="manifest.json",
            json_path="#/saveTransaction/state",
        )


def load_evidence_context(ctx: ValidationContext) -> None:
    evidence_index = ctx.documents.get("evidence/evidence_index.json", {})
    for item in evidence_index.get("items", []):
        evidence_id = item.get("id")
        if evidence_id:
            ctx.known_evidence_ids.add(evidence_id)

    unresolved = ctx.documents.get("evidence/unresolved_fields.json", {})
    for item in unresolved.get("items", []):
        path = item.get("jsonPath")
        if path:
            ctx.unresolved_paths.add(path)


def validate_project_references(ctx: ValidationContext) -> None:
    project = ctx.documents.get("project.json")
    if not isinstance(project, dict):
        return

    for name, rel in project.get("models", {}).items():
        if rel in ctx.missing_required_files:
            continue
        if rel not in ctx.documents:
            ctx.add(
                "ProjectReferenceValidator",
                "PRV001_MODEL_FILE_NOT_FOUND",
                f"project.models.{name} points to a missing model file",
                package_file="project.json",
                json_path=f"#/models/{name}",
            )

    groups = list_items(ctx, "rebar/groups.json")
    bars = list_items(ctx, "rebar/bars.json")
    segments = list_items(ctx, "rebar/segments.json")
    group_ids = {item.get("id") for item in groups}
    bar_ids = {item.get("id") for item in bars}
    segment_ids = {item.get("id") for item in segments}

    for index, group in enumerate(groups):
        for bar_id in group.get("barIds", []):
            if bar_id not in bar_ids:
                ctx.add("ProjectReferenceValidator", "PRV002_REBAR_REF_BROKEN", "group.barIds contains missing bar", package_file="rebar/groups.json", json_path=f"#/items/{index}/barIds", object_id=group.get("id", ""))
        for segment_id in group.get("segmentIds", []):
            if segment_id not in segment_ids:
                ctx.add("ProjectReferenceValidator", "PRV002_REBAR_REF_BROKEN", "group.segmentIds contains missing segment", package_file="rebar/groups.json", json_path=f"#/items/{index}/segmentIds", object_id=group.get("id", ""))

    for index, bar in enumerate(bars):
        if bar.get("groupId") not in group_ids:
            ctx.add("ProjectReferenceValidator", "PRV002_REBAR_REF_BROKEN", "bar.groupId points to missing group", package_file="rebar/bars.json", json_path=f"#/items/{index}/groupId", object_id=bar.get("id", ""))
        for segment_id in bar.get("segmentIds", []):
            if segment_id not in segment_ids:
                ctx.add("ProjectReferenceValidator", "PRV002_REBAR_REF_BROKEN", "bar.segmentIds contains missing segment", package_file="rebar/bars.json", json_path=f"#/items/{index}/segmentIds", object_id=bar.get("id", ""))

    for index, segment in enumerate(segments):
        if segment.get("groupId") not in group_ids:
            ctx.add("ProjectReferenceValidator", "PRV002_REBAR_REF_BROKEN", "segment.groupId points to missing group", package_file="rebar/segments.json", json_path=f"#/items/{index}/groupId", object_id=segment.get("id", ""))
        if segment.get("barId") not in bar_ids:
            ctx.add("ProjectReferenceValidator", "PRV002_REBAR_REF_BROKEN", "segment.barId points to missing bar", package_file="rebar/segments.json", json_path=f"#/items/{index}/barId", object_id=segment.get("id", ""))


def validate_legacy_objects(ctx: ValidationContext) -> None:
    for rel in ["rebar/steel_data.json", "rebar/groups.json", "rebar/bars.json", "rebar/segments.json"]:
        for index, item in enumerate(list_items(ctx, rel)):
            raw = item.get("legacyObject", {}).get("raw")
            if raw is None:
                ctx.add(
                    "CoreObjectBindingValidator",
                    "COB001_CORE_LEGACY_RAW_MISSING",
                    "core rebar object is missing legacyObject.raw",
                    package_file=rel,
                    json_path=f"#/items/{index}/legacyObject/raw",
                    object_id=item.get("id", ""),
                )


def validate_geometry_refs(ctx: ValidationContext) -> None:
    topology_doc = ctx.documents.get("geometry/topology_refs.json", {})
    for index, topology in enumerate(topology_doc.get("topologyRefs", [])):
        if topology.get("occtTransientIndex") is not None and topology.get("fingerprint") is None:
            ctx.add(
                "GeometryRefValidator",
                "GRV005_TRANSIENT_INDEX_USED_AS_ONLY_ID",
                "topologyRef only has occtTransientIndex and no stable fingerprint",
                package_file="geometry/topology_refs.json",
                json_path=f"#/topologyRefs/{index}",
                object_id=topology.get("topologyId", ""),
            )

    for rel in ["geometry/topology_refs.json", "rebar/groups.json", "rebar/bars.json", "rebar/segments.json"]:
        scan_for_acis_pointer(ctx, rel, ctx.documents.get(rel))

    source = ctx.documents.get("geometry/source_step.json", {})
    if source.get("importStatus") == "completed" and source.get("stepSelectionDiff") is None:
        resolved = [
            ref for ref in topology_doc.get("geometryRefs", [])
            if ref.get("state", "resolved") == "resolved"
        ]
        if resolved:
            ctx.add(
                "GeometryRefValidator",
                "GRV006_STEP_DIFF_MISSING",
                "resolved geometryRef has completed import but no STEP selection diff report",
                package_file="geometry/source_step.json",
                json_path="#/stepSelectionDiff",
                evidence_hint=["GAP-DEV-002", "E-DEV-009"],
            )


def scan_for_acis_pointer(ctx: ValidationContext, rel: str, value: Any, path: str = "#") -> None:
    if isinstance(value, str):
        if "ENTITY*" in value or "0x" in value:
            ctx.add(
                "GeometryRefValidator",
                "GRV004_ACIS_POINTER_USED_AS_STABLE_ID",
                "ACIS pointer or memory address is used as a stable geometry ID",
                package_file=rel,
                json_path=path,
                evidence_hint=["GAP-TECH-003"],
            )
    elif isinstance(value, list):
        for index, item in enumerate(value):
            scan_for_acis_pointer(ctx, rel, item, f"{path}/{index}")
    elif isinstance(value, dict):
        for key, item in value.items():
            scan_for_acis_pointer(ctx, rel, item, f"{path}/{key}")


def validate_bindings(ctx: ValidationContext) -> None:
    for rel in ["rebar/groups.json", "rebar/bars.json", "rebar/segments.json"]:
        for index, item in enumerate(list_items(ctx, rel)):
            binding = item.get("binding")
            if binding is None:
                ctx.add("LegacyGeometryBindingValidator", "LGV001_BINDING_MISSING", "core rebar object is missing binding", package_file=rel, json_path=f"#/items/{index}/binding", object_id=item.get("id", ""))
                continue
            if binding.get("state") not in {"resolved", "unresolvedGeometry", "pendingLegacyEvidence"}:
                ctx.add("LegacyGeometryBindingValidator", "LGV002_BINDING_STATE_INVALID", "binding.state is invalid", package_file=rel, json_path=f"#/items/{index}/binding/state", object_id=item.get("id", ""))
            for item_index, binding_item in enumerate(binding.get("items", [])):
                legacy_path = binding_item.get("legacyPath")
                if legacy_path:
                    try:
                        json_pointer_get(ctx.documents, legacy_path)
                    except (KeyError, IndexError, ValueError):
                        ctx.add("LegacyGeometryBindingValidator", "LGV003_LEGACY_PATH_BROKEN", "binding legacyPath points to a missing raw field", package_file=rel, json_path=f"#/items/{index}/binding/items/{item_index}/legacyPath", object_id=item.get("id", ""))

                geometry_path = binding_item.get("geometryPath")
                if geometry_path:
                    try:
                        json_pointer_get(ctx.documents, geometry_path)
                    except (KeyError, IndexError, ValueError):
                        ctx.add("LegacyGeometryBindingValidator", "LGV004_GEOMETRY_PATH_BROKEN", "binding geometryPath points to a missing geometry target", package_file=rel, json_path=f"#/items/{index}/binding/items/{item_index}/geometryPath", object_id=item.get("id", ""), evidence_hint=["GAP-DEV-002", "E-DEV-004"])
                elif binding.get("state") == "resolved":
                    ctx.add("LegacyGeometryBindingValidator", "LGV004_GEOMETRY_PATH_BROKEN", "resolved binding has no geometryPath", package_file=rel, json_path=f"#/items/{index}/binding/items/{item_index}/geometryPath", object_id=item.get("id", ""))

            if binding.get("state") == "unresolvedGeometry" and item.get("openMode") not in {"repairRequired", "readOnlyGeometry"}:
                ctx.add("LegacyGeometryBindingValidator", "LGV007_UNRESOLVED_STATE_NOT_ENFORCED", "unresolvedGeometry object must open in repair or read-only mode", package_file=rel, json_path=f"#/items/{index}/openMode", object_id=item.get("id", ""))


def validate_detail_mapping(ctx: ValidationContext) -> None:
    groups_by_id = {group.get("id"): group for group in list_items(ctx, "rebar/groups.json")}
    detail = ctx.documents.get("drawing/detail_mapping.json", {})
    for index, table in enumerate(detail.get("tables", [])):
        group = groups_by_id.get(table.get("sourceGroupId"))
        if group is None:
            ctx.add("DetailMappingValidator", "DMV004_SCHEDULE_ROW_ORPHANED", "detail table row points to missing source group", package_file="drawing/detail_mapping.json", json_path=f"#/tables/{index}/sourceGroupId")
            continue
        if table.get("rsdId") != group.get("rsdId"):
            ctx.add(
                "DetailMappingValidator",
                "DMV002_RSD_ID_MISMATCH",
                "detail table rsdId differs from source RebarGroup.rsdId",
                package_file="drawing/detail_mapping.json",
                json_path=f"#/tables/{index}/rsdId",
                object_id=group.get("id", ""),
                evidence_hint=["E-IDA-018", "E-IDA-019"],
            )


def validate_evidence(ctx: ValidationContext) -> None:
    unknown_ids: set[str] = set()
    for rel, document in ctx.documents.items():
        collect_unknown_evidence(ctx, rel, document, "#", unknown_ids)

    for evidence_id in sorted(unknown_ids):
        ctx.add("EvidenceValidator", "EV001_EVIDENCE_ID_NOT_FOUND", f"evidence ID is not listed in evidence_index.json: {evidence_id}", evidence_hint=[evidence_id])
        ctx.add("ProjectReferenceValidator", "PRV004_EVIDENCE_REF_BROKEN", f"object evidence points to unknown evidence ID: {evidence_id}", evidence_hint=[evidence_id])

    for rel in ["rebar/steel_data.json", "rebar/groups.json", "rebar/bars.json", "rebar/segments.json"]:
        scan_field_confidence(ctx, rel, ctx.documents.get(rel), "#")


def collect_unknown_evidence(ctx: ValidationContext, rel: str, value: Any, path: str, unknown_ids: set[str]) -> None:
    if isinstance(value, dict):
        for key, item in value.items():
            if key == "evidence" and isinstance(item, list):
                for evidence_id in item:
                    if is_evidence_ref_unknown(ctx, evidence_id):
                        unknown_ids.add(evidence_id)
            else:
                collect_unknown_evidence(ctx, rel, item, f"{path}/{key}", unknown_ids)
    elif isinstance(value, list):
        for index, item in enumerate(value):
            collect_unknown_evidence(ctx, rel, item, f"{path}/{index}", unknown_ids)


def is_evidence_ref_unknown(ctx: ValidationContext, evidence_id: Any) -> bool:
    if not isinstance(evidence_id, str):
        return False
    if evidence_id.startswith("GAP-"):
        return False
    if evidence_id in ctx.known_evidence_ids:
        return False
    return evidence_id.startswith("E-")


def scan_field_confidence(ctx: ValidationContext, rel: str, value: Any, path: str) -> None:
    if isinstance(value, dict):
        if "confidence" in value:
            full_path = f"{rel}{path}"
            confidence = value.get("confidence")
            evidence = value.get("evidence", [])
            if confidence == "pending" and full_path in ctx.unresolved_paths:
                ctx.add(
                    "EvidenceValidator",
                    "EVW001_PENDING_LEGACY_FIELD",
                    "pending legacy field is preserved and listed in unresolved_fields",
                    severity="warning",
                    package_file=rel,
                    json_path=path,
                    evidence_hint=[item for item in evidence if isinstance(item, str)],
                )
            if confidence == "confirmed" and full_path in ctx.unresolved_paths:
                ctx.add(
                    "EvidenceValidator",
                    "EV003_PENDING_MARKED_CONFIRMED",
                    "field is listed as unresolved but marked confirmed",
                    package_file=rel,
                    json_path=path,
                    evidence_hint=[item for item in evidence if isinstance(item, str)],
                )
            if confidence == "confirmed" and evidence and all(isinstance(item, str) and item.startswith("GAP-") for item in evidence):
                ctx.add(
                    "EvidenceValidator",
                    "EV004_CONFIRMED_WITHOUT_EVIDENCE",
                    "confirmed field is backed only by unresolved GAP evidence",
                    package_file=rel,
                    json_path=path,
                    evidence_hint=evidence,
                )
        for key, item in value.items():
            scan_field_confidence(ctx, rel, item, f"{path}/{key}")
    elif isinstance(value, list):
        for index, item in enumerate(value):
            scan_field_confidence(ctx, rel, item, f"{path}/{index}")


def validate_transaction(ctx: ValidationContext) -> None:
    manifest = ctx.documents.get("manifest.json", {})
    state = manifest.get("saveTransaction", {}).get("state")
    if state and state != "complete":
        ctx.add(
            "TransactionValidator",
            "TXV001_TEMP_PACKAGE_INVALID",
            "temporary package is not a complete validated save result",
            package_file="manifest.json",
            json_path="#/saveTransaction/state",
        )
        ctx.add(
            "TransactionValidator",
            "TXV006_HALF_WRITTEN_PACKAGE_OPENED_AS_FORMAL",
            "half-written package must not open as a formal saved project",
            package_file="manifest.json",
            json_path="#/saveTransaction/state",
        )


def list_items(ctx: ValidationContext, rel: str) -> list[dict[str, Any]]:
    document = ctx.documents.get(rel, {})
    items = document.get("items", []) if isinstance(document, dict) else []
    return [item for item in items if isinstance(item, dict)]


def report_id_for(package_path: Path) -> str:
    parent = package_path.parent.name
    if parent == "negative":
        parent = package_path.name
    return f"{package_path.parent.name}_validation_001"


def default_report_name(package_path: Path) -> str:
    if package_path.parent.parent.name == "negative":
        return f"negative_{package_path.parent.name}_validation_001.json"
    return f"{package_path.parent.name}_validation_001.json"


def discover_packages(root: Path) -> list[Path]:
    packages = [
        root / "fixtures" / "tsrebar" / "fixture_a_empty_step" / "project.tsrebar",
        root / "fixtures" / "tsrebar" / "fixture_b_single_group" / "project.tsrebar",
        root / "fixtures" / "tsrebar" / "fixture_c_pending_legacy" / "project.tsrebar",
    ]
    negative_root = root / "fixtures" / "tsrebar" / "negative"
    for child in sorted(negative_root.iterdir() if negative_root.exists() else []):
        package = child / "project.tsrebar"
        if package.exists():
            packages.append(package)
    return packages


def write_report(report: dict[str, Any], path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8", newline="\n") as handle:
        json.dump(report, handle, ensure_ascii=False, indent=2)
        handle.write("\n")


def main() -> int:
    parser = argparse.ArgumentParser(description="Validate tsrebar fixture packages.")
    parser.add_argument("packages", nargs="*", type=Path, help="project.tsrebar package paths")
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[2], help="documentation root")
    parser.add_argument("--all-fixtures", action="store_true", help="validate all known fixture packages")
    parser.add_argument("--out-dir", type=Path, help="write validation reports to this directory")
    args = parser.parse_args()

    packages = list(args.packages)
    if args.all_fixtures:
        packages.extend(discover_packages(args.root))
    if not packages:
        parser.error("provide package paths or --all-fixtures")

    failed = False
    for package in packages:
        report = validate_package(package)
        if args.out_dir:
            write_report(report, args.out_dir / default_report_name(Path(package)))
        else:
            print(json.dumps(report, ensure_ascii=False, indent=2))
        failed = failed or report["decision"] == "fail"
    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
