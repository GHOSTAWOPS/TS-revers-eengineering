#include "geometry/occ/import/OcctStepImportService.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <QUuid>

#include <IFSelect_ReturnStatus.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <Standard_Failure.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Document.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>
#include <XCAFApp_Application.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

#include <exception>

namespace {

std::string toStdString(const QString& value)
{
    return value.toUtf8().toStdString();
}

QString firstUnit(const TColStd_SequenceOfAsciiString& units)
{
    if (units.IsEmpty()) {
        return {};
    }
    return QString::fromUtf8(units.First().ToCString());
}

bool hasStepExtension(const QString& filePath)
{
    const QString suffix = QFileInfo(filePath).suffix().toLower();
    return suffix == QStringLiteral("stp") || suffix == QStringLiteral("step");
}

bool containsNonAscii(const QString& value)
{
    for (const QChar character : value) {
        if (character.unicode() > 0x7F) {
            return true;
        }
    }
    return false;
}

QString prepareOcctReadablePath(const QString& absolutePath,
                                QString* temporaryPath,
                                std::string* error)
{
    if (!containsNonAscii(absolutePath)) {
        return absolutePath;
    }

    const QString tempRoot = QStringLiteral("C:/Temp/tsrebar_step_import");
    if (!QDir().mkpath(tempRoot)) {
        *error = toStdString(QStringLiteral("Cannot create temporary STEP import directory: %1")
                                 .arg(tempRoot));
        return {};
    }

    const QString suffix = QFileInfo(absolutePath).suffix().toLower();
    const QString tempName = QStringLiteral("%1.%2")
                                 .arg(QUuid::createUuid().toString(QUuid::Id128), suffix);
    const QString tempPath = QDir(tempRoot).absoluteFilePath(tempName);

    if (!QFile::copy(absolutePath, tempPath)) {
        *error = toStdString(QStringLiteral("Cannot copy STEP file to temporary ASCII path: %1")
                                 .arg(tempPath));
        return {};
    }

    *temporaryPath = tempPath;
    return tempPath;
}

void countSubShapes(const TopoDS_Shape& shape, tsrebar::StepImportResult* result)
{
    for (TopExp_Explorer explorer(shape, TopAbs_SOLID); explorer.More(); explorer.Next()) {
        ++result->solids;
    }
    for (TopExp_Explorer explorer(shape, TopAbs_FACE); explorer.More(); explorer.Next()) {
        ++result->faces;
    }
    for (TopExp_Explorer explorer(shape, TopAbs_EDGE); explorer.More(); explorer.Next()) {
        ++result->edges;
    }
    for (TopExp_Explorer explorer(shape, TopAbs_VERTEX); explorer.More(); explorer.Next()) {
        ++result->vertices;
    }
}

QString labelEntry(const TDF_Label& label)
{
    TCollection_AsciiString entry;
    TDF_Tool::Entry(label, entry);
    return QString::fromUtf8(entry.ToCString());
}

} // namespace

namespace tsrebar {

StepImportResult OcctStepImportService::importStepFile(const QString& stepPath) const
{
    return importDocument(stepPath).summary;
}

OcctImportedDocument OcctStepImportService::importDocument(const QString& stepPath) const
{
    OcctImportedDocument document;
    StepImportResult& result = document.summary;
    const QString absolutePath = QFileInfo(stepPath).absoluteFilePath();
    result.filePath = toStdString(absolutePath);

    if (!QFileInfo::exists(absolutePath)) {
        result.error = toStdString(QStringLiteral("STEP file does not exist: %1").arg(absolutePath));
        return document;
    }
    if (!hasStepExtension(absolutePath)) {
        result.error = toStdString(QStringLiteral("Only .stp/.step files are supported: %1").arg(absolutePath));
        return document;
    }

    try {
        Handle(XCAFApp_Application) app = XCAFApp_Application::GetApplication();
        Handle(TDocStd_Document) occtDocument;
        app->NewDocument("MDTV-XCAF", occtDocument);

        STEPCAFControl_Reader reader;
        reader.SetColorMode(Standard_False);
        reader.SetNameMode(Standard_True);
        reader.SetLayerMode(Standard_False);
        reader.SetPropsMode(Standard_True);

        QString temporaryPath;
        const QString readablePath = prepareOcctReadablePath(absolutePath, &temporaryPath, &result.error);
        if (readablePath.isEmpty()) {
            return document;
        }

        const QByteArray nativePath = QFile::encodeName(readablePath);
        const IFSelect_ReturnStatus status = reader.ReadFile(nativePath.constData());
        if (!temporaryPath.isEmpty()) {
            QFile::remove(temporaryPath);
        }

        result.readOk = status == IFSelect_RetDone;
        if (!result.readOk) {
            result.error = toStdString(QStringLiteral("ReadFile failed with status %1.")
                                           .arg(static_cast<int>(status)));
            return document;
        }

        result.roots = reader.NbRootsForTransfer();

        TColStd_SequenceOfAsciiString lengthUnits;
        TColStd_SequenceOfAsciiString angleUnits;
        TColStd_SequenceOfAsciiString solidAngleUnits;
        reader.ChangeReader().FileUnits(lengthUnits, angleUnits, solidAngleUnits);
        result.lengthUnit = toStdString(firstUnit(lengthUnits));

        result.transferOk = reader.Transfer(occtDocument);
        if (!result.transferOk) {
            result.error = "Transfer to XCAF document failed.";
            return document;
        }

        const auto shapeTool = XCAFDoc_DocumentTool::ShapeTool(occtDocument->Main());
        TDF_LabelSequence freeShapes;
        shapeTool->GetFreeShapes(freeShapes);
        result.freeShapes = freeShapes.Length();

        for (Standard_Integer index = freeShapes.Lower(); index <= freeShapes.Upper(); ++index) {
            const TDF_Label label = freeShapes.Value(index);
            const TopoDS_Shape shape = XCAFDoc_ShapeTool::GetShape(label);
            if (!shape.IsNull()) {
                countSubShapes(shape, &result);
                OcctImportedPart part;
                part.id = document.parts.size() + 1;
                part.name = QStringLiteral("Part %1").arg(part.id);
                part.stableEntry = labelEntry(label);
                part.shape = shape;
                document.parts.push_back(part);
            }
        }

        result.ok = result.readOk && result.transferOk && result.error.empty();
    } catch (const Standard_Failure& failure) {
        result.error = std::string("OCCT exception: ") + failure.GetMessageString();
    } catch (const std::exception& exception) {
        result.error = std::string("std::exception: ") + exception.what();
    } catch (...) {
        result.error = "Unknown exception while importing STEP.";
    }

    return document;
}

} // namespace tsrebar
