#pragma once

#include <QString>
#include <QVector>

namespace tsrebar {

struct CommandGroupDefinition
{
    QString title;
    QString objectName;
};

struct MainTabDefinition
{
    QString title;
    QString objectName;
    QVector<CommandGroupDefinition> groups;
};

QVector<MainTabDefinition> mainTabDefinitions();

} // namespace tsrebar

