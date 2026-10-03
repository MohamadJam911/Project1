#pragma once

#include <string>
#include <unordered_map>

/**
 * @brief Configuration Management (Single Responsibility)
 * 
 * Handles application configuration settings from config files.
 */
class ConfigurationManager {
private:
    int cache_size;
    int bpm_tolerance;
    bool auto_sync;
    
    // Additional config parameters
    std::unordered_map<std::string, std::string> additional_settings;

public:
    /**
     * @brief Construct with default configuration
     */
    ConfigurationManager();
    
    /**
     * @brief Load configuration from file
     * @param config_path Path to dj_config.txt file
     * @return true if loaded successfully
     */
    bool loadFromFile(const std::string& config_path);
    
    // Configuration getters
    int getCacheSize() const { return cache_size; }
    int getBPMTolerance() const { return bpm_tolerance; }
    bool getAutoSync() const { return auto_sync; }
    
    /**
     * @brief Get additional configuration value by key
     */
    std::string getSetting(const std::string& key) const;
    
    /**
     * @brief Display current configuration
     */
    void displayConfiguration() const;
};