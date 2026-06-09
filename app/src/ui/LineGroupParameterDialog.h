#pragma once

#include "command/RebarLineGroupCommandHandler.h"

#include <QDialog>

class QComboBox;
class QDoubleSpinBox;
class QSpinBox;

namespace tsrebar {

class LineGroupParameterDialog final : public QDialog
{
public:
    explicit LineGroupParameterDialog(RebarLineGroupCommandParameters parameters,
                                      QWidget* parent = nullptr);

    [[nodiscard]] RebarLineGroupCommandParameters parameters() const;

private:
    RebarLineGroupCommandParameters m_baseParameters;
    QDoubleSpinBox* m_diameterSpin = nullptr;
    QDoubleSpinBox* m_intervalSpin = nullptr;
    QSpinBox* m_barCountSpin = nullptr;
    QComboBox* m_steelLevelCombo = nullptr;
    QDoubleSpinBox* m_distanceASpin = nullptr;
    QDoubleSpinBox* m_distanceBSpin = nullptr;
};

} // namespace tsrebar
