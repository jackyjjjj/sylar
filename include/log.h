#pragma once

#include <string>
#include <stdint.h>
#include <memory>
#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <vector>
#include <map>
#include <stdarg.h>
#include "../include/config.h"
#include "../include/singleton.h"
#include "../include/util.h"

using namespace std;

#define JYL_LOG_LEVEL(logger, level)                                                                  \
    if (logger->getLevel() <= level)                                                                  \
    jyl::LogEventWrap(jyl::LogEvent::ptr(new jyl::LogEvent(logger, level,                             \
                                                           __FILE__, __LINE__, 0, jyl::getThreadID(), \
                                                           jyl::getFiberID(), time(0))))              \
        .getSS()

#define JYL_LOG_DEBUG(logger) JYL_LOG_LEVEL(logger, jyl::LogLevel::DEBUG)
#define JYL_LOG_INFO(logger) JYL_LOG_LEVEL(logger, jyl::LogLevel::INFO)
#define JYL_LOG_WARN(logger) JYL_LOG_LEVEL(logger, jyl::LogLevel::WARN)
#define JYL_LOG_ERROR(logger) JYL_LOG_LEVEL(logger, jyl::LogLevel::ERROR)
#define JYL_LOG_FATAL(logger) JYL_LOG_LEVEL(logger, jyl::LogLevel::FATAL)

#define JYL_LOG_FMT_LEVEL(logger, level, fmt, ...)                                                               \
    if (logger->getLevel() <= level)                                                                             \
    jyl::LogEventWrap(jyl::LogEvent::ptr(new jyl::LogEvent(logger, level,                                        \
                                                           __FILE__, __LINE__, 0, jyl::GetThreadId(),            \
                                                           jyl::GetFiberId(), time(0), jyl::Thread::GetName()))) \
        .getEvent()                                                                                              \
        ->format(fmt, __VA_ARGS__)

#define JYL_LOG_ROOT() jyl::loggerManager::getInstance()->getRoot()

#define JYL_LOG_NAME(name) jyl::LoggerManager::getInstance()->getLogger(name)

namespace jyl
{
    class Logger;
    class LogEvent;
    class LoggerManager;
    class LogLevel
    {
    public:
        enum Level
        {
            UNKNOWN = 0,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL
        };
        static const char *toString(LogLevel::Level level);
    };
    class LogEvent
    {
    public:
        LogEvent(shared_ptr<Logger> logger, LogLevel::Level level, const char *file, int32_t m_line, uint32_t elapse, uint32_t thread_id, uint32_t fiber_id, uint32_t time);
        ~LogEvent();
        typedef shared_ptr<LogEvent> ptr;
        const char *getFile() const { return m_file; }
        int32_t getLine() const { return m_line; }
        uint32_t getThreadID() const { return m_threadID; }
        uint32_t getFiberID() const { return m_fiberID; }
        uint32_t getElapsed() const { return m_elapsed; }
        uint64_t getTime() const { return m_time; }
        string getContent() const { return m_ss.str(); }
        stringstream &getSS() { return m_ss; }
        shared_ptr<Logger> getLogger() const { return m_logger; }
        LogLevel::Level getLevel() const { return m_level; }
        void format(const char *fmt, ...);
        void format(const char *fmt, va_list al);

    private:
        const char *m_file = nullptr; // 文件名
        int32_t m_line = 0;           // 行号
        uint32_t m_threadID = 0;      // 线程id
        uint32_t m_fiberID = 0;       // 协程id
        uint32_t m_elapsed;           // 程序启动了多少时间
        uint64_t m_time;              // 时间戳
        stringstream m_ss;
        shared_ptr<Logger> m_logger;
        LogLevel::Level m_level;
    };

    class LogEventWrap
    {
    public:
        LogEventWrap(LogEvent::ptr event);
        ~LogEventWrap();
        stringstream &getSS();
        LogEvent::ptr getEvent() const { return m_event; }

    private:
        LogEvent::ptr m_event;
    };

    // 格式器
    class LogFormatter
    {
    public:
        typedef shared_ptr<LogFormatter> ptr;
        string format(shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
        LogFormatter(const string &pattern);

    public:
        class FormatItem
        {
        public:
            // virtual ~FormatItem() {}
            FormatItem(const string &fmt = "") {}
            virtual void format(ostream &os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
            typedef shared_ptr<FormatItem> ptr;
        };
        void init();
        bool isError() const { return m_error; }

    private:
        string m_pattern;
        vector<FormatItem::ptr> m_items;
        bool m_error = false;
    };
    // 日志输出地
    class LogAppender
    {
    public:
        // LogAppender();
        virtual ~LogAppender() {}
        typedef shared_ptr<LogAppender> ptr;

        virtual void log(shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
        void setFormatter(LogFormatter::ptr formatter) { m_formatter = formatter; }
        LogFormatter::ptr getFormatter() { return m_formatter; }

    protected:
        LogLevel::Level m_level = LogLevel::DEBUG;
        LogFormatter::ptr m_formatter;
    };

    // 日志器
    class Logger : public enable_shared_from_this<Logger>
    {
        friend class LoggerManager;

    public:
        Logger(const string &name = "root");
        typedef shared_ptr<Logger> ptr;
        void log(LogLevel::Level level, LogEvent::ptr event);

        void debug(LogEvent::ptr event);
        void info(LogEvent::ptr event);
        void warn(LogEvent::ptr event);
        void error(LogEvent::ptr event);
        void fatal(LogEvent::ptr event);

        void addAppender(LogAppender::ptr appender);
        void delAppender(LogAppender::ptr appender);
        LogLevel::Level getLevel() const { return m_level; }
        string getName() const { return m_name; }
        void setLevel(LogLevel::Level level) { m_level = level; }

    private:
        string m_name;                         // 日志名称
        LogLevel::Level m_level;               // 日志级别
        list<LogAppender::ptr> m_appenderList; // appender集合
        LogFormatter::ptr m_formatter;         // 日志格式化器
        Logger::ptr m_root;
    };

    // 输出到控制台
    class StdoutLogAppender : public LogAppender
    {
    public:
        typedef shared_ptr<StdoutLogAppender> ptr;
        void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;

    private:
    };
    // 输出到文件
    class FileLogAppender : public LogAppender
    {
    public:
        FileLogAppender(const string &filename);
        typedef shared_ptr<FileLogAppender> ptr;
        void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
        // 重新打开文件
        bool reopen();

    private:
        string m_filename;
        ofstream m_fileStream;
    };

    class LoggerManager
    {
    public:
        LoggerManager();
        Logger::ptr getLogger(const string &name);
        void init();
        Logger::ptr getRoot() const { return m_root; }

    private:
        Logger::ptr m_root;
        map<string, Logger::ptr> m_loggers;
    };

    typedef jyl::Singleton<LoggerManager> loggerManager;
}