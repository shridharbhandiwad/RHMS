/**
 * @file RadarSubsystem.cpp
 * @brief Implementation of RadarSubsystem registry
 */

#include "RadarSubsystem.h"
#include "SubsystemNode.h"
#include <QDebug>

RadarSubsystem& RadarSubsystem::instance()
{
    static RadarSubsystem inst;
    return inst;
}

void RadarSubsystem::registerSubsystem(const SubsystemDefinition& def, FactoryFunction factory)
{
    m_definitions[def.type] = def;
    m_factories[def.type] = factory;
    qDebug() << "Registered subsystem type:" << def.type;
}

SubsystemNode* RadarSubsystem::createSubsystem(const QString& type, QObject* parent)
{
    if (!m_factories.contains(type)) {
        qWarning() << "Unknown subsystem type:" << type;
        return nullptr;
    }
    
    SubsystemNode* node = m_factories[type](parent);
    if (node) {
        qDebug() << "Created subsystem:" << type;
    }
    return node;
}

QStringList RadarSubsystem::availableTypes() const
{
    return m_definitions.keys();
}

QStringList RadarSubsystem::categoriesWithTypes(const QString& category) const
{
    QStringList types;
    for (auto it = m_definitions.begin(); it != m_definitions.end(); ++it) {
        if (it.value().category == category) {
            types.append(it.key());
        }
    }
    return types;
}

QStringList RadarSubsystem::allCategories() const
{
    QStringList categories;
    for (auto it = m_definitions.begin(); it != m_definitions.end(); ++it) {
        QString cat = it.value().category;
        if (!categories.contains(cat)) {
            categories.append(cat);
        }
    }
    return categories;
}

SubsystemDefinition RadarSubsystem::definition(const QString& type) const
{
    return m_definitions.value(type, SubsystemDefinition());
}

bool RadarSubsystem::hasType(const QString& type) const
{
    return m_definitions.contains(type);
}

void RadarSubsystem::registerBuiltInSubsystems()
{
    // This will be populated when we create the specific node types
    qDebug() << "Registering built-in radar subsystems...";
}
