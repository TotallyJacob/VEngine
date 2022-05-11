#pragma once

#include <iostream>

// Defines that do stuff
#define VE_ENABLE_DEBUG_IF_DO
#define VE_ENABLE_DEBUG_DO

// Enabling Log stuff
#define VE_ENABLE_LOG_LOG
#define VE_ENABLE_LOG_SUGGESTION
#define VE_ENABLE_LOG_DEBUG
#define VE_ENABLE_LOG_WARNING
#define VE_ENABLE_LOG_ERROR

#ifdef VE_ENABLE_DEBUG_IF_DO
#define VE_DEBUG_IF_DO(condition, to_do)                                                                                                   \
    if (condition)                                                                                                                         \
        to_do;
#else
#define VE_DEBUG_IF_DO(condition, to_do)
#endif

#ifdef VE_ENABLE_DEBUG_DO
#define VE_DEBUG_DO(to_do) to_do;
#else
#define VE_DEBUG_DO(to_do)
#endif


// VE text colour codes
#define VE_COL_RESET       "\033[0m"
#define VE_COL_BLACK       "\033[30m"        /* Black */
#define VE_COL_RED         "\033[31m"        /* Red */
#define VE_COL_GREEN       "\033[32m"        /* Green */
#define VE_COL_YELLOW      "\033[33m"        /* Yellow */
#define VE_COL_BLUE        "\033[34m"        /* Blue */
#define VE_COL_MAGENTA     "\033[35m"        /* Magenta */
#define VE_COL_CYAN        "\033[36m"        /* Cyan */
#define VE_COL_WHITE       "\033[37m"        /* White */
#define VE_COL_BOLDBLACK   "\033[1m\033[30m" /* Bold Black */
#define VE_COL_BOLDRED     "\033[1m\033[31m" /* Bold Red */
#define VE_COL_BOLDGREEN   "\033[1m\033[32m" /* Bold Green */
#define VE_COL_BOLDYELLOW  "\033[1m\033[33m" /* Bold Yellow */
#define VE_COL_BOLDBLUE    "\033[1m\033[34m" /* Bold Blue */
#define VE_COL_BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define VE_COL_BOLDCYAN    "\033[1m\033[36m" /* Bold Cyan */
#define VE_COL_BOLDWHITE   "\033[1m\033[37m" /* Bold White */

// VE background colour codes
#define VE_BAC_RESET       "\033[0m"
#define VE_BAC_BLACK       "\033[40m"        /* Black */
#define VE_BAC_RED         "\033[41m"        /* Red */
#define VE_BAC_GREEN       "\033[42m"        /* Green */
#define VE_BAC_YELLOW      "\033[43m"        /* Yellow */
#define VE_BAC_BLUE        "\033[44m"        /* Blue */
#define VE_BAC_MAGENTA     "\033[45m"        /* Magenta */
#define VE_BAC_CYAN        "\033[46m"        /* Cyan */
#define VE_BAC_WHITE       "\033[47m"        /* White */
#define VE_BAC_BOLDBLACK   "\033[1m\033[40m" /* Bold Black */
#define VE_BAC_BOLDRED     "\033[1m\033[41m" /* Bold Red */
#define VE_BAC_BOLDGREEN   "\033[1m\033[42m" /* Bold Green */
#define VE_BAC_BOLDYELLOW  "\033[1m\033[43m" /* Bold Yellow */
#define VE_BAC_BOLDBLUE    "\033[1m\033[44m" /* Bold Blue */
#define VE_BAC_BOLDMAGENTA "\033[1m\033[45m" /* Bold Magenta */
#define VE_BAC_BOLDCYAN    "\033[1m\033[46m" /* Bold Cyan */
#define VE_BAC_BOLDWHITE   "\033[1m\033[47m" /* Bold White */

// Log templates
#define VE_LOG_TEMPLATE(prefix, to_log)             std::cout << prefix << to_log << VE_COL_RESET << std::endl;
#define VE_LOG_MULTI_START_TEMPLATE(prefix, to_log) std::cout << prefix << to_log << '\n'
#define VE_LOG_MULTI_TEMPLATE(to_log)               std::cout << "   " << to_log << '\n'
#define VE_LOG_MULTI_END_TEMPLATE()                 std::cout << VE_COL_RESET << std::endl;

// Actual logging
#ifdef VE_ENABLE_LOG_LOG
#define VE_LOG(to_log)             VE_LOG_TEMPLATE(vengine::Log::log_label, to_log)
#define VE_LOG_MULTI_START(to_log) VE_LOG_MULTI_START_TEMPLATE(vengine::Log::log_label, to_log)
#define VE_LOG_MULTI(to_log)       VE_LOG_MULTI_TEMPLATE(to_log)
#define VE_LOG_MULTI_END()         VE_LOG_MULTI_END_TEMPLATE()
#else
#define VE_LOG(to_log)
#define VE_LOG_MULTI_START(to_log)
#define VE_LOG_MULTI(to_log)
#define VE_LOG_MULTI_END()
#endif

#ifdef VE_ENABLE_LOG_SUGGESTION
#define VE_LOG_SUGGESTION(to_log)             VE_LOG_TEMPLATE(vengine::Log::suggestion_label, to_log)
#define VE_LOG_SUGGESTION_MULTI_START(to_log) VE_LOG_MULTI_START_TEMPLATE(vengine::Log::suggestion_label, to_log)
#define VE_LOG_SUGGESTION_MULTI(to_log)       VE_LOG_MULTI_TEMPLATE(to_log)
#define VE_LOG_SUGGESTION_MULTI_END()         VE_LOG_MULTI_END_TEMPLATE()
#else
#define VE_LOG_SUGGESTION(to_log)
#define VE_LOG_SUGGESTION_MULTI_START(to_log)
#define VE_LOG_SUGGESTION_MULTI(to_log)
#define VE_LOG_SUGGESTION_MULTI_END()
#endif

#ifdef VE_ENABLE_LOG_DEBUG
#define VE_LOG_DEBUG(to_log)             VE_LOG_TEMPLATE(vengine::Log::debug_label, to_log)
#define VE_LOG_DEBUG_MULTI_START(to_log) VE_LOG_MULTI_START_TEMPLATE(vengine::Log::debug_label, to_log)
#define VE_LOG_DEBUG_MULTI(to_log)       VE_LOG_MULTI_TEMPLATE(to_log)
#define VE_LOG_DEBUG_MULTI_END()         VE_LOG_MULTI_END_TEMPLATE()
#else
#define VE_LOG_DEBUG(to_log)
#define VE_LOG_DEBUG_MULTI_START(to_log)
#define VE_LOG_DEBUG_MULTI(to_log)
#define VE_LOG_DEBUG_MULTI_END()
#endif

#ifdef VE_ENABLE_LOG_WARNING
#define VE_LOG_WARNING(to_log)             VE_LOG_TEMPLATE(vengine::Log::warning_label, to_log)
#define VE_LOG_WARNING_MULTI_START(to_log) VE_LOG_MULTI_START_TEMPLATE(vengine::Log::warning_label, to_log)
#define VE_LOG_WARNING_MULTI(to_log)       VE_LOG_MULTI_TEMPLATE(to_log)
#define VE_LOG_WARNING_MULTI_END()         VE_LOG_MULTI_END_TEMPLATE()
#else
#define VE_LOG_WARNING(to_log)
#define VE_LOG_WARNING_MULTI_START(to_log)
#define VE_LOG_WARNING_MULTI(to_log)
#define VE_LOG_WARNING_MULTI_END()
#endif

#ifdef VE_ENABLE_LOG_ERROR
#define VE_LOG_ERROR(to_log)             VE_LOG_TEMPLATE(vengine::Log::error_label, to_log)
#define VE_LOG_ERROR_MULTI_START(to_log) VE_LOG_MULTI_START_TEMPLATE(vengine::Log::error_label, to_log)
#define VE_LOG_ERROR_MULTI(to_log)       VE_LOG_MULTI_TEMPLATE(to_log)
#define VE_LOG_ERROR_MULTI_END()         VE_LOG_MULTI_END_TEMPLATE()
#else
#define VE_LOG_ERROR(to_log)
#define VE_LOG_ERROR_MULTI_START(to_log)
#define VE_LOG_ERROR_MULTI(to_log)
#define VE_LOG_ERROR_MULTI_END()
#endif



namespace vengine
{

class LogType
{
    public:

        constexpr LogType(const char* prefix) : m_prefix(prefix), m_suffix("")
        {
        }
        constexpr LogType(const char* prefix, const char* suffix) : m_prefix(prefix), m_suffix(suffix)
        {
        }

        template <typename T>
        inline void log(T to_log) const
        {
            std::cout << m_prefix << to_log << m_suffix << std::endl;
        }
        template <typename T>
        inline void log_multi_start(T to_log) const
        {
            std::cout << m_prefix << to_log << '\n';
        }
        inline void log_multi_end() const
        {
            std::cout << m_suffix << std::endl;
        }

    private:

        const char* m_prefix;
        const char* m_suffix;
};


class Log
{
    public:

        constexpr static const char* log_label = VE_COL_BOLDWHITE "[LOG]" VE_COL_RESET " ";
        constexpr static const char* suggestion_label = VE_COL_BOLDGREEN "[SUGGESTION]" VE_COL_RESET " " VE_BAC_GREEN VE_COL_BLACK;
        constexpr static const char* debug_label = VE_BAC_CYAN VE_COL_BLACK "[DEBUG]" VE_COL_RESET " ";
        constexpr static const char* warning_label = VE_COL_BOLDYELLOW "[WARNING]" VE_COL_RESET " " VE_BAC_YELLOW VE_COL_BLACK;
        constexpr static const char* error_label = VE_COL_BOLDRED "[ERROR]" VE_COL_RESET " " VE_BAC_RED VE_COL_BLACK;

        inline const static LogType normal_log = {log_label};
        inline const static LogType debug_log = {debug_label};
        inline const static LogType warning_log = {warning_label, VE_COL_RESET};
        inline const static LogType error_log = {error_label, VE_COL_RESET};
};

}; // namespace vengine
