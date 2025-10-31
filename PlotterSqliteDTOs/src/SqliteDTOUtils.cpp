#include "plotter_sqlite_dtos/SqliteDTOs.h"
#include <chrono>

namespace plotter {
namespace sqlite_dtos {

// ============================================================================
// SqliteDTOUtils Implementation
// ============================================================================

long long SqliteDTOUtils::getCurrentTimestamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

} // namespace sqlite_dtos
} // namespace plotter
