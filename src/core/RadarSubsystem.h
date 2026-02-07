/**
 * @file RadarSubsystem.h
 * @brief Radar subsystem registry and factory
 */

#ifndef RADARSUBSYSTEM_H
#define RADARSUBSYSTEM_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <functional>

class SubsystemNode;

/**
 * @struct SubsystemDefinition
 * @brief Metadata for a radar subsystem type
 */
struct SubsystemDefinition {
    QString type;
    QString category;
    QString displayName;
    QString description;
    QString iconPath;
    QStringList inputPorts;
    QStringList outputPorts;
    bool supportsHierarchy;
};

/**
 * @class RadarSubsystem
 * @brief Registry and factory for radar subsystem nodes
 * 
 * Provides centralized management of available subsystem types
 * and factory methods for creating subsystem instances.
 */
class RadarSubsystem
{
public:
    // Factory function type
    using FactoryFunction = std::function<SubsystemNode*(QObject*)>;
    
    // Singleton access
    static RadarSubsystem& instance();
    
    // Registration
    void registerSubsystem(const SubsystemDefinition& def, FactoryFunction factory);
    
    // Factory methods
    SubsystemNode* createSubsystem(const QString& type, QObject* parent = nullptr);
    
    // Query methods
    QStringList availableTypes() const;
    QStringList categoriesWithTypes(const QString& category) const;
    QStringList allCategories() const;
    SubsystemDefinition definition(const QString& type) const;
    bool hasType(const QString& type) const;
    
    // Register all built-in subsystems
    void registerBuiltInSubsystems();
    
private:
    RadarSubsystem() = default;
    ~RadarSubsystem() = default;
    RadarSubsystem(const RadarSubsystem&) = delete;
    RadarSubsystem& operator=(const RadarSubsystem&) = delete;
    
    QMap<QString, SubsystemDefinition> m_definitions;
    QMap<QString, FactoryFunction> m_factories;
};

#endif // RADARSUBSYSTEM_H
