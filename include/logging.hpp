#pragma once

#include "modloader/shared/modloader.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"

extern ModInfo myLoggerInfo;

Logger& getLogger();

#define INFO(value...) getLogger().info(value);
#define ERROR(value...) getLogger().error(value);