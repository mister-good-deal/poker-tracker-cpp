#include "Logger.hpp"

namespace Logger {
    quill::Logger* Quill::_logger = nullptr;

    auto Quill::getLogger() -> quill::Logger* {
        if (_logger == nullptr) { _logger = _initLogger(); }

        return _logger;
    }

    auto Quill::_initLogger() -> quill::Logger* {
        quill::Config cfg;
        cfg.enable_console_colours = true;
        quill::configure(cfg);
        quill::start();

        auto* logger = quill::get_root_logger();

#ifdef LOG_LEVEL_DEBUG
        logger->set_log_level(quill::LogLevel::Debug);
#endif
#ifdef LOG_LEVEL_INFO
        logger->set_log_level(quill::LogLevel::Info);
#endif
        // enable a backtrace that will get flushed when we log CRITICAL
        logger->init_backtrace(2, quill::LogLevel::Critical);

        return logger;
    }

}  // namespace Logger
