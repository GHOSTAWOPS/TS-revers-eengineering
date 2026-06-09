#include "ui/LineGroupParameterDialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QSpinBox>
#include <QVBoxLayout>

#include <algorithm>
#include <utility>

namespace tsrebar {
namespace {

QDoubleSpinBox* doubleSpin(QWidget* parent,
                           const char* objectName,
                           double value,
                           double minimum,
                           double maximum,
                           double step,
                           const QString& suffix)
{
    auto* spin = new QDoubleSpinBox(parent);
    spin->setObjectName(QString::fromLatin1(objectName));
    spin->setRange(minimum, maximum);
    spin->setDecimals(3);
    spin->setSingleStep(step);
    spin->setSuffix(suffix);
    spin->setValue(value);
    return spin;
}

QStringList steelLevels()
{
    return {
        QStringLiteral("HPB300"),
        QStringLiteral("HRB400"),
        QStringLiteral("HRB500"),
        QStringLiteral("HRBF400"),
    };
}

} // namespace

LineGroupParameterDialog::LineGroupParameterDialog(
    RebarLineGroupCommandParameters parameters,
    QWidget* parent)
    : QDialog(parent)
    , m_baseParameters(std::move(parameters))
{
    setObjectName(QStringLiteral("line_group_parameter_dialog"));
    setWindowTitle(QStringLiteral("线配筋参数"));
    setModal(true);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(8);

    auto* form = new QFormLayout();
    form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    form->setLabelAlignment(Qt::AlignRight);

    m_diameterSpin = doubleSpin(this,
                                "line_group_diameter_spin",
                                m_baseParameters.diameter,
                                1.0,
                                100.0,
                                1.0,
                                QStringLiteral(" mm"));
    m_intervalSpin = doubleSpin(this,
                                "line_group_interval_spin",
                                m_baseParameters.interval,
                                1.0,
                                10000.0,
                                10.0,
                                QStringLiteral(" mm"));
    m_barCountSpin = new QSpinBox(this);
    m_barCountSpin->setObjectName(QStringLiteral("line_group_bar_count_spin"));
    m_barCountSpin->setRange(1, 100000);
    m_barCountSpin->setValue(std::max(1, m_baseParameters.requestedBarCount));

    m_steelLevelCombo = new QComboBox(this);
    m_steelLevelCombo->setObjectName(QStringLiteral("line_group_steel_level_combo"));
    m_steelLevelCombo->addItems(steelLevels());
    const QString steelLevel = QString::fromStdString(m_baseParameters.steelLevel);
    if (m_steelLevelCombo->findText(steelLevel) < 0 && !steelLevel.isEmpty()) {
        m_steelLevelCombo->addItem(steelLevel);
    }
    m_steelLevelCombo->setCurrentText(steelLevel);

    m_distanceASpin = doubleSpin(this,
                                 "line_group_distance_a_spin",
                                 m_baseParameters.distanceA,
                                 0.0,
                                 100000.0,
                                 0.1,
                                 QStringLiteral(" mm"));
    m_distanceBSpin = doubleSpin(this,
                                 "line_group_distance_b_spin",
                                 m_baseParameters.distanceB,
                                 0.0,
                                 100000.0,
                                 0.1,
                                 QStringLiteral(" mm"));

    form->addRow(QStringLiteral("直径"), m_diameterSpin);
    form->addRow(QStringLiteral("间距"), m_intervalSpin);
    form->addRow(QStringLiteral("数量"), m_barCountSpin);
    form->addRow(QStringLiteral("级别"), m_steelLevelCombo);
    form->addRow(QStringLiteral("起点距离"), m_distanceASpin);
    form->addRow(QStringLiteral("终点距离"), m_distanceBSpin);
    layout->addLayout(form);

    auto* buttons =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->setObjectName(QStringLiteral("line_group_button_box"));
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

RebarLineGroupCommandParameters LineGroupParameterDialog::parameters() const
{
    RebarLineGroupCommandParameters result = m_baseParameters;
    result.diameter = m_diameterSpin->value();
    result.interval = m_intervalSpin->value();
    result.requestedBarCount = m_barCountSpin->value();
    result.steelLevel = m_steelLevelCombo->currentText().toStdString();
    result.distanceA = m_distanceASpin->value();
    result.distanceB = m_distanceBSpin->value();
    return result;
}

} // namespace tsrebar
